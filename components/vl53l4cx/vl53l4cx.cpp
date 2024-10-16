#include "vl53l4cx.h"
#include "esphome/core/log.h"
#include "vl53l4cx_class.h"  // Ensure VL53L4CX class is included

namespace esphome {
namespace vl53l4cx {

static const char *TAG = "vl53l4cx";

// Static instance of the VL53L4CX sensor
VL53L4CX *VL53L4CXSensor::sensor_instance = nullptr;

void VL53L4CXSensor::setup() {
  // Force logging level to VERBOSE for all components
  esp_log_level_set("*", ESP_LOG_VERBOSE);

  ESP_LOGI(TAG, "Log level set to VERBOSE");

  // Start the I2C communication using the specified I2C bus
  this->i2c_bus_->begin();

  // Initialize the static VL53L4CX sensor instance
  if (!sensor_instance) {
    sensor_instance = new VL53L4CX(this->i2c_bus_, A1);  // Change A1 to your shutdown pin if needed
  }

  // Ensure the sensor starts correctly
  if (sensor_instance->InitSensor(this->i2c_address_) != 0) {  // Use the configured I2C address
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

}  // namespace vl53l4cx
}  // namespace esphome
