#include "vl53l4cx.h"
#include "esphome/core/log.h"
#include "vl53l4cx_class.h"  // Ensure VL53L4CX class is included
#include "esphome/core/log.h"

namespace esphome {
namespace vl53l4cx {

static const char *TAG = "vl53l4cx";  // Used for logging

VL53L4CXSensor::VL53L4CXSensor() : PollingComponent(15000), sensor_vl53l4cx_(nullptr) {}

VL53L4CXSensor::~VL53L4CXSensor() {
  if (sensor_vl53l4cx_ != nullptr) {
    delete sensor_vl53l4cx_;  // Clean up memory
    sensor_vl53l4cx_ = nullptr;
  }
}

void VL53L4CXSensor::setup() {
  ESP_LOGD(TAG, "Setting up VL53L4CX sensor...");
  this->sensor_vl53l4cx_ = new VL53L4CX(&Wire, A1);
  
  this->sensor_vl53l4cx_->begin();
  this->sensor_vl53l4cx_->VL53L4CX_Off();

  // Initialize sensor with default address (0x12)
  if (this->sensor_vl53l4cx_->InitSensor(0x12) != 0) {
    ESP_LOGE(TAG, "Failed to initialize VL53L4CX sensor");
    return;
  }

  ESP_LOGD(TAG, "VL53L4CX sensor initialized successfully.");
  this->sensor_vl53l4cx_->VL53L4CX_StartMeasurement();
}

void VL53L4CXSensor::update() {
  ESP_LOGD(TAG, "Checking for new measurement data...");

  uint8_t new_data_ready = 0;
  int status = this->sensor_vl53l4cx_->VL53L4CX_GetMeasurementDataReady(&new_data_ready);

  if (status == 0 && new_data_ready != 0) {
    ESP_LOGD(TAG, "New data available from VL53L4CX sensor.");

    VL53L4CX_MultiRangingData_t multi_ranging_data;
    this->sensor_vl53l4cx_->VL53L4CX_GetMultiRangingData(&multi_ranging_data);

    // Log object count and distances for debug
    ESP_LOGD(TAG, "Object count: %d", multi_ranging_data.NumberOfObjectsFound);
    for (int i = 0; i < multi_ranging_data.NumberOfObjectsFound; i++) {
      ESP_LOGD(TAG, "Object %d distance: %d mm", i, multi_ranging_data.RangeData[i].RangeMilliMeter);
    }

    // Clear the interrupt and start a new measurement
    this->sensor_vl53l4cx_->VL53L4CX_ClearInterruptAndStartMeasurement();
  } else {
    ESP_LOGD(TAG, "No new data available from VL53L4CX sensor.");
  }
}

}  // namespace vl53l4cx
}  // namespace esphome
