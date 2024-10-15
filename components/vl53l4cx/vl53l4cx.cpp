#include "vl53l4cx.h"
#include "esphome/core/log.h"
#include "vl53l4x_class.h"  // Assuming the VL53L4CX library is included

namespace esphome {
namespace vl53l4cx {

static const char *TAG = "vl53l4cx.sensor";

// Constructor
VL53L4CXSensor::VL53L4CXSensor(uint32_t update_interval) : PollingComponent(update_interval) {}

// Setup function
void VL53L4CXSensor::setup() {
  ESP_LOGCONFIG(TAG, "Setting up VL53L4CX sensor...");

  // Initialize the VL53L4CX sensor
  if (!this->is_connected()) {
    ESP_LOGE(TAG, "VL53L4CX sensor not connected!");
    this->mark_failed();
    return;
  }

  // Initialize VL53L4CX settings
  if (this->vl53l4cx_.begin() != 0) {
    ESP_LOGE(TAG, "Failed to initialize VL53L4CX sensor.");
    this->mark_failed();
    return;
  }

  // Set the sensor mode to long-distance mode
  this->vl53l4cx_.VL53L4CX_SetDistanceMode(VL53L4CX_DISTANCEMODE_LONG);
  this->vl53l4cx_.VL53L4CX_SetMeasurementTimingBudgetMicroSeconds(50000);  // 50 ms timing budget
  ESP_LOGCONFIG(TAG, "VL53L4CX successfully initialized in long-range mode.");
}

// Update function (reads the distance and publishes it)
void VL53L4CXSensor::update() {
  float distance = this->get_distance();

  if (!isnan(distance)) {
    ESP_LOGD(TAG, "Distance: %.2f mm", distance);
    this->publish_state(distance);  // Publish the distance value
  } else {
    ESP_LOGE(TAG, "Failed to read distance from VL53L4CX sensor.");
    this->publish_state(NAN);  // Publish NAN in case of failure
  }
}

// Function to read distance from VL53L4CX sensor
float VL53L4CXSensor::get_distance() {
  VL53L4CX_MultiRangingData_t ranging_data;

  // Read ranging data
  if (this->vl53l4cx_.VL53L4CX_GetMultiRangingData(&ranging_data) == 0) {
    if (ranging_data.NumberOfObjectsFound > 0) {
      uint16_t distance = ranging_data.RangeData[0].RangeMilliMeter;
      return static_cast<float>(distance);  // Return distance in millimeters
    }
  }
  return NAN;  // Return NAN if reading fails
}

}  // namespace vl53l4cx
}  // namespace esphome
