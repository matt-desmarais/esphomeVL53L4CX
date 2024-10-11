#include "vl53l4cx.h"

namespace esphome {
namespace vl53l4cx {

void VL53L4CXSensor::setup() {
  // Set I2C address and bus
  this->set_i2c_address(0x29);
//  this->set_i2c_bus(i2c_arduinoi2cbus_id);  // Replace with the actual bus ID if needed

  // Sensor initialization logic
  if (sensor.VL53L4CX_DataInit() != 0) {
    ESP_LOGE("VL53L4CX", "Sensor initialization failed!");
    return;
  }

  if (sensor.VL53L4CX_SetTuningParameter(1, 50) != 0) {
    ESP_LOGE("VL53L4CX", "Failed to set tuning parameter.");
    return;
  }

  if (sensor.VL53L4CX_SetDistanceMode(VL53L4CX_DISTANCEMODE_LONG) != 0) {
    ESP_LOGE("VL53L4CX", "Failed to set distance mode.");
    return;
  }

  if (sensor.VL53L4CX_StartMeasurement() != 0) {
    ESP_LOGE("VL53L4CX", "Failed to start measurement.");
    return;
  }

  ESP_LOGI("VL53L4CX", "Sensor setup completed successfully.");
}

void VL53L4CXSensor::update() {
  uint16_t distance = get_distance();

  if (distance > 0) {
    publish_state(distance);
  } else {
    ESP_LOGE("VL53L4CX", "Invalid measurement, publishing NAN");
    publish_state(NAN);
  }
}

uint16_t VL53L4CXSensor::get_distance() {
  VL53L4CX_MultiRangingData_t ranging_data;
  sensor.VL53L4CX_GetMultiRangingData(&ranging_data);

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
    return 0;
  }
}

}  // namespace vl53l4cx
}  // namespace esphome
