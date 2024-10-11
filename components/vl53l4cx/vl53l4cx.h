#pragma once

#include "esphome/components/sensor/sensor.h"
#include "esphome/components/i2c/i2c.h"
#include "vl53l4cx_class.h"

namespace esphome {
namespace vl53l4cx {

class VL53L4CXSensor : public sensor::Sensor, public PollingComponent, public i2c::I2CDevice {
 public:
  void setup() override;
  void update() override;

 protected:
  Adafruit_VL53L4CX vl53l4cx_;  // Use the Adafruit library for the VL53L4CX sensor
};

}  // namespace vl53l4cx
}  // namespace esphome
