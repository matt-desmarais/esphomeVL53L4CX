#include "vl53l4cx.h"
#include "esphome/core/log.h"
#include "vl53l4cx_class.h"  // Ensure VL53L4CX class is included

namespace esphome {
namespace vl53l4cx {

static const char *TAG = "vl53l4cx.sensor";

// Constructor
VL53L4CXSensor::VL53L4CXSensor(uint32_t update_interval) : PollingComponent(update_interval), sensor_vl53l4cx_(nullptr) {}
//VL53L4CXSensor::VL53L4CXSensor(uint32_t update_interval) : PollingComponent(update_interval), sensor_vl53l4cx_(&Wire, A1) {}
// Default constructor
VL53L4CXSensor::VL53L4CXSensor() : PollingComponent(15000), sensor_vl53l4cx_(&Wire, A1) {}  // Assuming a default update interval of 15 seconds


// Setup function
void VL53L4CXSensor::setup() {
  ESP_LOGCONFIG(TAG, "Setting up VL53L4CX sensor...");

  // Initialize I2C
  Wire.begin();
  this->sensor_vl53l4cx_ = new VL53L4CX(&Wire, this->address_);  // Changed to allocate on the heap
  // Initialize the sensor.
  this->sensor_vl53l4cx_->begin();
  this->sensor_vl53l4cx_->VL53L4CX_Off();

  //this->sensor_vl53l4cx_.begin();
  //this->sensor_vl53l4cx_.VL53L4CX_Off();
  // Initialize the sensor
  if (sensor_vl53l4cx_.InitSensor(0x12) != 0) {
    ESP_LOGE(TAG, "Failed to initialize VL53L4CX sensor.");
    this->mark_failed();
    return;
  }
  this->sensor_vl53l4cx_.VL53L4CX_StartMeasurement();
  // Start measurement
  //sensor_vl53l4cx_.VL53L4CX_StartMeasurement();
  ESP_LOGCONFIG(TAG, "VL53L4CX sensor successfully initialized.");
}

// Update function (reads the distance and publishes it)
void VL53L4CXSensor::update() {
  uint8_t NewDataReady = 0;
  int status = this->sensor_vl53l4cx_->VL53L4CX_GetMeasurementDataReady(&NewDataReady);
  VL53L4CX_MultiRangingData_t ranging_data;
  uint8_t new_data_ready = 0;
  this->sensor_vl53l4cx_ = new VL53L4CX(&Wire, this->address_);  // Changed to allocate on the heap

  // Check if new data is ready
  if (this->sensor_vl53l4cx_->VL53L4CX_GetMeasurementDataReady(&new_data_ready) != 0 || !new_data_ready) {
    ESP_LOGE(TAG, "No new data from VL53L4CX sensor.");
    return;
  }
  if (status == 0 && NewDataReady != 0) {
    VL53L4CX_MultiRangingData_t MultiRangingData;
    this->sensor_vl53l4cx_->VL53L4CX_GetMultiRangingData(&MultiRangingData);  // Access pointer

  // Read the distance data
//  if (this->sensor_vl53l4cx_->VL53L4CX_GetMultiRangingData(&ranging_data) == 0 && ranging_data.NumberOfObjectsFound > 0) {
    uint16_t distance = ranging_data.RangeData[0].RangeMilliMeter;
    ESP_LOGD(TAG, "Distance: %d mm", distance);
    this->publish_state(static_cast<float>(distance));
  } else {
    ESP_LOGE(TAG, "Failed to read data from VL53L4CX sensor.");
    this->publish_state(NAN);
  }

  // Clear interrupt and start next measurement
  this->sensor_vl53l4cx_->VL53L4CX_ClearInterruptAndStartMeasurement();
}

}  // namespace vl53l4cx
}  // namespace esphome
