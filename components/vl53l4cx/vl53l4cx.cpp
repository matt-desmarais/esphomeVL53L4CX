#include "vl53l4cx.h"
#include "esphome/core/log.h"
#include "vl53l4cx_class.h"  // Ensure VL53L4CX class is included

namespace esphome {
namespace vl53l4cx {

static const char *TAG = "vl53l4cx";

// Declare the VL53L4CX sensor instance as static to avoid cross-core heap allocation issues
static VL53L4CX sensor_instance;

void VL53L4CXSensor::setup() {
  // Force logging level to VERBOSE for all components
  esp_log_level_set("*", ESP_LOG_VERBOSE);

  ESP_LOGI(TAG, "Log level set to VERBOSE");
  
  // Start the I2C communication
  Wire.begin();
  
  // Initialize the VL53L4CX sensor
  ESP_LOGI(TAG, "Initializing VL53L4CX sensor...");
  
  // Ensure the sensor starts correctly with the static instance
  sensor_instance.InitSensor(0x12);  // Set the I2C address, change as needed
  
  // Start continuous measurements
  sensor_instance.VL53L4CX_StartMeasurement();
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
    status = sensor_instance.VL53L4CX_GetMeasurementDataReady(&NewDataReady);
  } while (!NewDataReady);
  
  if ((!status) && (NewDataReady != 0)) {
    // Get ranging data
    status = sensor_instance.VL53L4CX_GetMultiRangingData(pMultiRangingData);
    
    if (status == 0 && pMultiRangingData->NumberOfObjectsFound > 0) {
      // Assuming you want to measure the first object
      int distance = pMultiRangingData->RangeData[0].RangeMilliMeter;
      ESP_LOGI(TAG, "Distance: %d mm", distance);
      
      // Publish the distance to ESPHome
      this->publish_state(distance);
      
      // Clear interrupts and restart the measurement
      sensor_instance.VL53L4CX_ClearInterruptAndStartMeasurement();
    }
  } else {
    ESP_LOGW(TAG, "Failed to get measurement data");
    this->publish_state(NAN);
  }
}

}  // namespace vl53l4cx
}  // namespace esphome
