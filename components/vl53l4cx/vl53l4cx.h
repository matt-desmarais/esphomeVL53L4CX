#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "vl53l4cx_class.h"  // Include the VL53L4CX sensor class

namespace esphome {
namespace vl53l4cx {

class VL53L4CXSensor : public PollingComponent, public sensor::Sensor {
 public:
  // Constructor
  VL53L4CXSensor() = default;

  // Setup function to initialize the sensor
  void setup() override;

  // Update function to fetch new distance measurements
  void update() override;

  // This will be called to set up the hardware (sensor)
  float get_setup_priority() const override;

 private:
  static VL53L4CX *sensor_instance;  // Declare the static sensor instance
};

}  // namespace vl53l4cx
}  // namespace esphome
