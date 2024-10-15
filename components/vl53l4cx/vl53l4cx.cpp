#include "vl53l4cx.h"
#include "esphome/core/log.h"
#include "vl53l4cx_class.h"  // Ensure VL53L4CX class is included

namespace esphome {
namespace vl53l4cx {

static const char *TAG = "vl53l4cx.sensor";

// Constructor
VL53L4CXSensor::VL53L4CXSensor(uint32_t update_interval) : PollingComponent(update_interval), sensor_vl53l4cx_(&Wire, A1) {}

// Setup function
void VL53L4CXSensor::setup() {
  ESP_LOGCONFIG(TAG, "Setting up VL53L4CX sensor...");

  // Initialize I2C
  Wire.begin();

  // Initialize the sensor
  if (sensor_vl53l4cx_.InitSensor(0x12) != 0) {
    ESP_LOGE(TAG, "Failed to initialize VL53L4CX sensor.");
    this->mark_failed();
    return;
  }

  // Start measurement
  sensor_vl53l4cx_.VL53L4CX_StartMeasurement();
  ESP_LOGCONFIG(TAG, "VL53L4CX sensor successfully initialized.");
}

// Update function (reads the distance and publishes it)
void VL53L4CXSensor::update() {
  VL53L4CX_MultiRangingData_t ranging_data;
  uint8_t new_data_ready = 0;

  // Check if new data is ready
  if (sensor_vl53l4cx_.VL53L4CX_GetMeasurementDataReady(&new_data_ready) != 0 || !new_data_ready) {
    ESP_LOGE(TAG, "No new data from VL53L4CX sensor.");
    return;
  }

  // Read the distance data
  if (sensor_vl53l4cx_.VL53L4CX_GetMultiRangingData(&ranging_data) == 0 && ranging_data.NumberOfObjectsFound > 0) {
    uint16_t distance = ranging_data.RangeData[0].RangeMilliMeter;
    ESP_LOGD(TAG, "Distance: %d mm", distance);
    this->publish_state(static_cast<float>(distance));
  } else {
    ESP_LOGE(TAG, "Failed to read data from VL53L4CX sensor.");
    this->publish_state(NAN);
  }

  // Clear interrupt and start next measurement
  sensor_vl53l4cx_.VL53L4CX_ClearInterruptAndStartMeasurement();
}

}  // namespace vl53l4cx
}  // namespace esphome
