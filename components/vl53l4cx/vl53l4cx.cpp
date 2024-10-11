#include "vl53l4cx.h"
#include "esphome/core/log.h"
#include "esphome/components/i2c/i2c.h"

namespace esphome {
namespace vl53l4cx {

static const char *TAG = "vl53l4cx.sensor";

void VL53L4CXSensor::setup() {
  ESP_LOGCONFIG(TAG, "Setting up VL53L4CX...");

  // Set the I2C bus for the sensor using the methods from i2c::I2CDevice
//  this->set_i2c_bus(i2c_arduinoi2cbus_id);  // Or the correct I2C bus ID
//  this->set_i2c_address(0x29);  // Set the I2C address for VL53L4CX sensor

  // Initialize the VL53L4CX sensor over I2C
  if (!this->vl53l4cx_.begin()) {
    ESP_LOGE(TAG, "Could not initialize VL53L4CX sensor.");
    this->mark_failed();
    return;
  }
}

void VL53L4CXSensor::update() {
  ESP_LOGD(TAG, "Updating VL53L4CX sensor...");

  // Get distance measurement
  VL53L4CX_RangingData_t measure;
  this->vl53l4cx_.getRangingData(&measure);

  // Log the distance
  ESP_LOGD(TAG, "Measured distance: %u mm", measure.RangeMilliMeter);

  // Publish the measured distance to ESPHome
  this->publish_state(measure.RangeMilliMeter);
}

}  // namespace vl53l4cx
}  // namespace esphome
