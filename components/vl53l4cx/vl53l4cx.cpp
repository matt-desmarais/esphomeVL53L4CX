#include "vl53l4cx.h"
#include "esphome/core/log.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/i2c/i2c.h"

namespace esphome {
namespace vl53l4cx {

static const char *TAG = "vl53l4cx.sensor";

// Constructor definition with update_interval support
VL53L4CXSensor::VL53L4CXSensor(uint32_t update_interval) : PollingComponent(update_interval) {}

void VL53L4CXSensor::setup() {
  ESP_LOGCONFIG(TAG, "Setting up VL53L4CX...");

  // Initialize the VL53L4CX sensor over I2C
  if (!this->vl53l4cx_.begin()) {
    ESP_LOGE(TAG, "Could not initialize VL53L4CX sensor.");
    this->mark_failed();  // Mark the sensor as failed if initialization fails
    return;
  }

  // Set the sensor to long-range mode
  vl53l4cx_.VL53L4CX_SetDistanceMode(VL53L4CX_DISTANCEMODE_LONG);  // Set to long-range mode

  // Set an appropriate timing budget for long-range mode (e.g., 50 ms)
  vl53l4cx_.VL53L4CX_SetMeasurementTimingBudgetMicroSeconds(50000);  // 50 ms

  ESP_LOGCONFIG(TAG, "VL53L4CX successfully initialized in long-range mode.");
}

void VL53L4CXSensor::update() {
  uint16_t distance = get_distance();

  if (distance > 0) {
    ESP_LOGD(TAG, "Distance: %d mm", distance);  // Log the distance
    publish_state(distance);
  } else {
    ESP_LOGE("VL53L4CX", "Invalid measurement, publishing NAN");
    publish_state(NAN);
  }
}

uint16_t VL53L4CXSensor::get_distance() {
  VL53L4CX_MultiRangingData_t ranging_data;
  vl53l4cx_.VL53L4CX_GetMultiRangingData(&ranging_data);

  if (ranging_data.NumberOfObjectsFound > 0) {
    uint16_t min_distance = ranging_data.RangeData[0].RangeMilliMeter;

    for (int i = 1; i < ranging_data.NumberOfObjectsFound; i++) {
      uint16_t current_distance = ranging_data.RangeData[i].RangeMilliMeter;
      if (current_distance < min_distance) {
        min_distance = current_distance;
      }
    }

    return min_distance;
  } else {
    ESP_LOGW(TAG, "No objects found");
    return 0;  // No objects detected
  }
}

// sets
void VL53L4CXSensor::set_i2c_bus(i2c::I2CBus *bus)
{
}

void VL53L4CXSensor::set_i2c_address(uint8_t address)
{
    if (this->getAddress() != address)
    {
        this->setAddress(address);
    }
}


}  // namespace vl53l4cx
}  // namespace esphome
