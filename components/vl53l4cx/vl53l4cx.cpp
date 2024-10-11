#include "vl53l4cx.h"
#include "esphome/core/log.h"
#include "esphome/components/i2c/i2c.h"

namespace esphome {
namespace vl53l4cx {

static const char *TAG = "vl53l4cx.sensor";

// Constructor: Pass the update_interval value to PollingComponent
VL53L4CXSensor::VL53L4CXSensor(uint32_t update_interval) : PollingComponent(update_interval) {}

void VL53L4CXSensor::setup() {
  ESP_LOGCONFIG(TAG, "Setting up VL53L4CX...");

  // Set the I2C bus for the sensor using the methods from i2c::I2CDevice
//  this->set_i2c_bus(i2c_arduinoi2cbus_id);  // Or the correct I2C bus ID
//  this->set_i2c_address(0x29);  // Set the I2C address for VL53L4CX sensor
//  this->set_i2c_address(0x29);  // Set the default I2C address for VL53L4CX
  // Initialize the VL53L4CX sensor over I2C
  if (!this->vl53l4cx_.begin()) {
    ESP_LOGE(TAG, "Could not initialize VL53L4CX sensor.");
    this->mark_failed();
    return;
  }
}

void VL53L4CXSensor::update() {
  uint16_t distance = get_distance();

  if (distance > 0) {
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
    return 0;
  }
}

}  // namespace vl53l4cx
}  // namespace esphome
