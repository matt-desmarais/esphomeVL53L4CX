#include "vl53l4cx.h"
#include "esphome/core/log.h"
#include "esphome/components/i2c/i2c.h"

namespace esphome {
namespace vl53l4cx {

void VL53L4CXSensor::setup() {
  // Sensor initialization logic
  ESP_LOGI("VL53L4CX", "Initializing VL53L4CX sensor...");

  // Initialize the sensor using I2C
  if (this->sensor.VL53L4CX_DataInit() != 0) {
    ESP_LOGE("VL53L4CX", "Sensor initialization failed!");
    return;
  }

  // Set tuning parameter
  if (this->sensor.VL53L4CX_SetTuningParameter(1, 50) != 0) {
    ESP_LOGE("VL53L4CX", "Failed to set tuning parameter.");
    return;
  }

  // Set distance mode to long range
  if (this->sensor.VL53L4CX_SetDistanceMode(VL53L4CX_DISTANCEMODE_LONG) != 0) {
    ESP_LOGE("VL53L4CX", "Failed to set distance mode.");
    return;
  }

  // Start the sensor measurement
  if (this->sensor.VL53L4CX_StartMeasurement() != 0) {
    ESP_LOGE("VL53L4CX", "Failed to start measurement.");
    return;
  }

  ESP_LOGI("VL53L4CX", "Sensor setup completed successfully.");
}

void VL53L4CXSensor::update() {
  // Read the distance value
  uint16_t distance = get_distance();

  // Publish the distance if valid, otherwise publish NAN
  if (distance > 0) {
    this->publish_state(distance);
  } else {
    ESP_LOGE("VL53L4CX", "Invalid measurement, publishing NAN");
    this->publish_state(NAN);
  }
}

uint16_t VL53L4CXSensor::get_distance() {
  VL53L4CX_MultiRangingData_t ranging_data;

  // Use the sensor's API to get ranging data
  if (this->vl53l4cx.VL53L4CX_GetMultiRangingData(&ranging_data) == 0) {
    if (ranging_data.NumberOfObjectsFound > 0) {
      uint16_t min_distance = ranging_data.RangeData[0].RangeMilliMeter;

      // Find the minimum distance if multiple objects are found
      for (int i = 1; i < ranging_data.NumberOfObjectsFound; i++) {
        uint16_t current_distance = ranging_data.RangeData[i].RangeMilliMeter;
        if (current_distance < min_distance) {
          min_distance = current_distance;
        }
      }

      return min_distance;
    }
  }

  // Return 0 if no objects were found
  return 0;
}

}  // namespace vl53l4cx
}  // namespace esphome
