#include "Wire.h"
#include "vl53l4cx.h"


namespace esphome {
namespace vl53l4cx {

void VL53L4CXSensor::setup() {
  Wire.begin();
  delay(500);  // Add a small delay after I2C initialization

  Wire.beginTransmission(0x29);  // Assuming sensor address is 0x29
  uint8_t error = Wire.endTransmission();
  
  if (error != 0) {
    ESP_LOGE("VL53L4CX", "I2C communication failed! Error code: %d", error);
    return;
  }

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
    publish_state(distance);  // Publish the distance in mm
  } else {
    ESP_LOGE("VL53L4CX", "Invalid measurement, publishing NAN");
    publish_state(NAN);  // Publish NAN if measurement is invalid
  }
}

uint16_t VL53L4CXSensor::get_distance() {
  VL53L4CX_MultiRangingData_t ranging_data;
  sensor.VL53L4CX_GetMultiRangingData(&ranging_data);

  if (ranging_data.NumberOfObjectsFound > 0) {
    // Initialize the minimum distance to the first object's distance
    uint16_t min_distance = ranging_data.RangeData[0].RangeMilliMeter;

    // Loop through all detected objects and find the minimum distance
    for (int i = 1; i < ranging_data.NumberOfObjectsFound; i++) {
      uint16_t current_distance = ranging_data.RangeData[i].RangeMilliMeter;
      if (current_distance < min_distance) {
        min_distance = current_distance;  // Update the minimum distance
      }
    }

    return min_distance;  // Return the shortest distance
  } else {
    return 0;  // Return 0 if no object is found
  }
}


/*
uint16_t VL53L4CXSensor::get_distance() {
  VL53L4CX_MultiRangingData_t ranging_data;
  sensor.VL53L4CX_GetMultiRangingData(&ranging_data);

  if (ranging_data.NumberOfObjectsFound > 0) {
    return ranging_data.RangeData[0].RangeMilliMeter;  // Return the first object's distance
  } else {
    return 0;  // Return 0 if no object is found
  }
}
*/
}  // namespace vl53l4cx
}  // namespace esphome
