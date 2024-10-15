#ifndef ESPHOME_VL53L4CX_SENSOR_H
#define ESPHOME_VL53L4CX_SENSOR_H

#include "esphome/components/sensor/sensor.h"
#include "esphome/components/i2c/i2c.h"
#include "esphome/core/component.h"

namespace esphome {
namespace vl53l4cx {

class VL53L4CXSensor : public PollingComponent, public sensor::Sensor, public i2c::I2CDevice {
 public:
  VL53L4CXSensor(uint32_t update_interval);

  void setup() override;   // Setup function for initialization
  void update() override;  // Update function for reading sensor data

 protected:
  // Function to read distance from VL53L4CX sensor
  float get_distance();

 private:
  VL53L4CXSensor vl53l4cx_; 

};

}  // namespace vl53l4cx
}  // namespace esphome

#endif  // ESPHOME_VL53L4CX_SENSOR_H
