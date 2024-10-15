#include "vl53l4cx.h"
#include "esphome/core/log.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/i2c/i2c.h"

namespace esphome {
namespace vl53l4cx {

static const char *TAG = "vl53l4cx.sensor";

VL53L4CXSensor::VL53L4CXSensor(uint32_t update_interval) : PollingComponent(update_interval) {}

void VL53L4CXSensor::setup() {
  ESP_LOGCONFIG(TAG, "Setting up VL53L4CX...");

  // Use the inherited I2CDevice's configured bus and address
  this->setup_i2c_();

  // Initialize the VL53L4CX sensor without parameters
  if (!this->vl53l4cx_.begin()) {
    ESP_LOGE(TAG, "Could not initialize VL53L4CX sensor.");
    this->mark_failed();
    return;
  }

  // Set sensor to long-range mode
  vl53l4cx_.VL53L4CX_SetDistanceMode(VL53L4CX_DISTANCEMODE_LONG);
  vl53l4cx_.VL53L4CX_SetMeasurementTimingBudgetMicroSeconds(50000);

  ESP_LOGCONFIG(TAG, "VL53L4CX successfully initialized in long-range mode.");
}

void VL53L4CXSensor::update() {
  uint16_t distance = get_distance();

  if (distance > 0) {
    ESP_LOGD(TAG, "Distance: %d mm", distance);
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
    return 0;
  }
}

// Methods to set bus and address, though setup_i2c_() manages this
void VL53L4CXSensor::set_i2c_bus(i2c::I2CBus *bus) {
  this->bus_ = bus;
}

void VL53L4CXSensor::set_i2c_address(uint8_t address) {
  this->address_ = address;
}

}  // namespace vl53l4cx
}  // namespace esphome
