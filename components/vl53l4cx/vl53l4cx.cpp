#include "vl53l4cx.h"
#include "esphome/core/log.h"
#include "vl53l4cx_class.h"

namespace esphome {
namespace vl53l4cx {

static const char *TAG = "vl53l4cx";

// Static instance of the VL53L4CX sensor
VL53L4CX *VL53L4CXSensor::sensor_instance = nullptr;

void VL53L4CXSensor::setup() {
  // Force logging level to VERBOSE for all components
  esp_log_level_set("*", ESP_LOG_VERBOSE);

  ESP_LOGI(TAG, "Log level set to VERBOSE");
  ESP_LOGI("vl53l4cx", "Forced log: Entering setup...");
  
  // Initialize the static VL53L4CX sensor instance using the ESPHome I2C bus
  if (!sensor_instance) {
    sensor_instance = new VL53L4CX(&Wire, A1);  // A1 is the shutdown pin
  }

  // Initialize the I2C bus (ESPHome handles this, so no need for manual initialization)
  Wire.begin();  // Use the standard Wire object

  // Begin sensor
  sensor_instance->begin();

  // Turn off the sensor initially
  sensor_instance->VL53L4CX_Off();

  // Initialize the sensor with the I2C address
  if (sensor_instance->InitSensor(0x12) != 0) {
    ESP_LOGE(TAG, "Failed to initialize VL53L4CX sensor.");
    return;
  }

  // Start continuous measurements
  sensor_instance->VL53L4CX_StartMeasurement();

  ESP_LOGI(TAG, "VL53L4CX setup complete.");
}

void VL53L4CXSensor::update() {
  ESP_LOGD(TAG, "Checking for new measurement data...");

  VL53L4CX_MultiRangingData_t MultiRangingData;
  VL53L4CX_MultiRangingData_t *pMultiRangingData = &MultiRangingData;
  uint8_t NewDataReady = 0;
  int status;

  // Wait for new measurement data
  do {
    status = sensor_instance->VL53L4CX_GetMeasurementDataReady(&NewDataReady);
  } while (!NewDataReady);

  if ((!status) && (NewDataReady != 0)) {
    // Get ranging data
    status = sensor_instance->VL53L4CX_GetMultiRangingData(pMultiRangingData);

    if (status == 0 && pMultiRangingData->NumberOfObjectsFound > 0) {
      // Assuming you want to measure the first object
      int distance = pMultiRangingData->RangeData[0].RangeMilliMeter;
      ESP_LOGI(TAG, "Distance: %d mm", distance);

      // Publish the distance to ESPHome
      this->publish_state(distance);

      // Clear interrupts and restart the measurement
      sensor_instance->VL53L4CX_ClearInterruptAndStartMeasurement();
    }
  } else {
    ESP_LOGW(TAG, "Failed to get measurement data");
    this->publish_state(NAN);
  }
}

float VL53L4CXSensor::get_setup_priority() const {
//  return setup_priority::DATA;  // Use appropriate setup priority (can be HARDWARE, DATA, etc.)
  return setup_priority::HARDWARE;
}

}  // namespace vl53l4cx
}  // namespace esphome

