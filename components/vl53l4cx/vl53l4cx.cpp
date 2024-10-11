#include "vl53l4cx.h"
#include "esphome/core/log.h"
#include "esphome/components/i2c/i2c.h"

namespace esphome {
namespace vl53l4cx {

static const char *TAG = "vl53l4cx.sensor";

void VL53L4CXSensor::setup() {
  ESP_LOGCONFIG(TAG, "Setting up VL53L4CX...");

  // Initialize I2C communication with the sensor
  if (!this->is_i2c_initialized()) {
    ESP_LOGE(TAG, "I2C bus not initialized.");
    this->mark_failed();
    return;
  }

  // Initialize the VL53L4CX sensor
  if (!this->vl53l4cx_.begin()) {
    ESP_LOGE(TAG, "Could not initialize VL53L4CX sensor.");
    this->mark_failed();
    return;
  }
}

void VL53L4CXSensor::update() {
  ESP_LOGD(TAG, "Updating VL53L4CX sensor...");

  // Get distance measurement
  uint16_t distance = this->vl53l4cx_.readRange();

  if (this->vl53l4cx_.timeoutOccurred()) {
    ESP_LOGW(TAG, "VL53L4CX sensor timeout occurred!");
    this->mark_failed();
    return;
  }

  // Log distance
  ESP_LOGD(TAG, "Measured distance: %u mm", distance);

  // Publish distance to ESPHome
  this->publish_state(distance);
}

}  // namespace vl53l4cx
}  // namespace esphome
