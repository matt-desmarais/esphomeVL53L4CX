#pragma once

#include "esphome/components/sensor/sensor.h"
#include "esphome/components/i2c/i2c.h"
#include "vl53l4cx_class.h"

namespace esphome {
namespace vl53l4cx {

class VL53L4CXSensor : public sensor::Sensor, public PollingComponent, public i2c::I2CDevice {
 public:
  //VL53L4CXSensor(uint32_t update_interval) : PollingComponent(update_interval) {}
  // Constructor declaration
  VL53L4CXSensor(uint32_t update_interval);
  //VL53L4CXSensor() {}  // Constructor without setting I2C manually
  void setup() override;
  void update() override;

 protected:
//  VL53L4CX vl53l4cx_ = new VL53L4CXSensor();
  VL53L4CX vl53l4cx_;
  uint16_t get_distance();
};

}  // namespace vl53l4cx
}  // namespace esphome
