#include "vl53l4cx.h"
#include "esphome/core/log.h"
#include "esphome/components/i2c/i2c.h"

namespace esphome {
namespace vl53l4cx {

static const char *TAG = "vl53l4cx.sensor";

void VL53L4CXSensor::setup() {
  ESP_LOGCONFIG(TAG, "Setting up VL53L4CX...");

  // Initialize I2C communication with the sensor
  if (!this->wire_->begin()) {
    ESP_LOGE(TAG, "Could not initialize I2C.");
    this->mark_failed();
    return;
  }

  // Initialize the VL53L4CX sensor
  if (this->vl53l4cx_.init() != 0) {
    ESP_LOGE(TAG, "Could not initialize VL53L4CX sensor.");
    this->mark_failed();
    return;
  }

  this->vl53l4cx_.setAddress(0x29); // Default I2C address
}

void VL53L4CXSensor::update() {
  ESP_LOGD(TAG, "Updating VL53L4CX sensor...");

  // Get distance measurement
  uint16_t distance = this->vl53l4cx_.readRangeContinuousMillimeters();

  // Log distance
  ESP_LOGD(TAG, "Measured distance: %u mm", distance);

  // Publish distance to ESPHome
  this->publish_state(distance);
}

}  // namespace vl53l4cx
}  // namespace esphome
