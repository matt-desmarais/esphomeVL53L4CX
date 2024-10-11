#pragma once

#include "esphome/components/i2c/i2c.h"
#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "vl53l4cx_class.h"  // Include STM32Duino VL53L4CX library

namespace esphome {
namespace vl53l4cx {

class VL53L4CXSensor : public PollingComponent, public sensor::Sensor, public i2c::I2CDevice {
 public:
  VL53L4CXSensor(uint32_t update_interval = 1000) : PollingComponent(update_interval) {}
  
  void setup() override;
  void update() override;

 protected:
  VL53L4CX sensor;  // Use the STM32Duino class
  uint16_t get_distance();
};

}  // namespace vl53l4cx
}  // namespace esphome
