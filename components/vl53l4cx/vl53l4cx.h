#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/i2c/i2c.h"  // Include the ESPHome I2C component
#include "vl53l4cx_class.h"  // Include the VL53L4CX sensor class

namespace esphome {
namespace vl53l4cx {

class VL53L4CXSensor : public PollingComponent, public sensor::Sensor {
 public:
  // Constructor
//  VL53L4CXSensor() = default;
  VL53L4CXSensor() {
    ESP_LOGI("vl53l4cx", "VL53L4CXSensor constructor called...");
  }

  // Setup function to initialize the sensor
  void setup() override;

  // Update function to fetch new distance measurements
  void update() override;

  void reinitialize_sensor();

  // This will be called to set up the hardware (sensor)
  float get_setup_priority() const override;

  // Function to set the I2C bus
  void set_i2c_bus(i2c::I2CBus *i2c_bus) { this->i2c_bus_ = i2c_bus; }

  // Function to set the I2C address
  void set_i2c_address(uint8_t address) { this->i2c_address_ = address; }

 private:
  static VL53L4CX *sensor_instance;  // Declare the static sensor instance
  i2c::I2CBus *i2c_bus_;  // Store the I2C bus object
  uint8_t i2c_address_;    // Store the I2C address
};

}  // namespace vl53l4cx
}  // namespace esphome
