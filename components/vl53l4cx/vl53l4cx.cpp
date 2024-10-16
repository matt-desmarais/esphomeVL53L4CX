#include "vl53l4cx.h"
#include "esphome/core/log.h"
#include "vl53l4cx_class.h"  // Ensure VL53L4CX class is included
#include "esphome/core/log.h"

namespace esphome {
namespace vl53l4cx {

static const char *TAG = "vl53l4cx";

//VL53L4CXSensor::VL53L4CXSensor() : PollingComponent(15000), sensor_vl53l4cx_(nullptr) {}

//VL53L4CXSensor::~VL53L4CXSensor() {
//  delete this->sensor_vl53l4cx_;
//}
/*
void VL53L4CXSensor::setup() {
  ESP_LOGD(TAG, "Before sensor_vl53l4cx_ initialization...");
  this->sensor_vl53l4cx_ = new VL53L4CX(&Wire, A1);
  ESP_LOGD(TAG, "After sensor_vl53l4cx_ initialization...");
  delay(500);
  ESP_LOGD(TAG, "Calling begin...");
  this->sensor_vl53l4cx_->begin();
  ESP_LOGD(TAG, "After begin...");
  
  ESP_LOGD(TAG, "Turning sensor off...");
  this->sensor_vl53l4cx_->VL53L4CX_Off();

  //ESP_LOGD(TAG, "Setting up VL53L4CX sensor...");
  //this->sensor_vl53l4cx_ = new VL53L4CX(&Wire, A1);
  
  //this->sensor_vl53l4cx_->begin();
  //this->sensor_vl53l4cx_->VL53L4CX_Off();

  if (this->sensor_vl53l4cx_->InitSensor(0x12) != 0) {
    ESP_LOGE(TAG, "Failed to initialize VL53L4CX sensor");
    return;
  }

  ESP_LOGD(TAG, "VL53L4CX sensor initialized successfully.");
  this->sensor_vl53l4cx_->VL53L4CX_StartMeasurement();
}
*/
void VL53L4CXSensor::setup() {
  // Start the I2C communication
  Wire.begin();
  
  // Initialize the VL53L4CX sensor
  ESP_LOGI(TAG, "Initializing VL53L4CX sensor...");
  this->sensor_vl53l4cx_ = new VL53L4CX(&Wire, A1);  // Use A1 as shutdown pin, change as necessary

  // Ensure the sensor starts correctly
  this->sensor_vl53l4cx_->begin();
  this->sensor_vl53l4cx_->VL53L4CX_Off();  // Turn the sensor off initially
  
  // Initialize the sensor with a new I2C address or leave it as default
  this->sensor_vl53l4cx_->InitSensor(0x12);  // Change I2C address if needed

  // Start continuous measurements
  this->sensor_vl53l4cx_->VL53L4CX_StartMeasurement();
  ESP_LOGI(TAG, "VL53L4CX setup complete.");
}

/*
void VL53L4CXSensor::update() {
  ESP_LOGD(TAG, "Checking for new measurement data...");

  uint8_t new_data_ready = 0;
  int status = this->sensor_vl53l4cx_->VL53L4CX_GetMeasurementDataReady(&new_data_ready);

  if (status == 0 && new_data_ready != 0) {
    ESP_LOGD(TAG, "New data available from VL53L4CX sensor.");

    VL53L4CX_MultiRangingData_t multi_ranging_data;
    this->sensor_vl53l4cx_->VL53L4CX_GetMultiRangingData(&multi_ranging_data);

    ESP_LOGD(TAG, "Object count: %d", multi_ranging_data.NumberOfObjectsFound);
    for (int i = 0; i < multi_ranging_data.NumberOfObjectsFound; i++) {
      ESP_LOGD(TAG, "Object %d distance: %d mm", i, multi_ranging_data.RangeData[i].RangeMilliMeter);
    }

    this->sensor_vl53l4cx_->VL53L4CX_ClearInterruptAndStartMeasurement();
  } else {
    ESP_LOGD(TAG, "No new data available from VL53L4CX sensor.");
  }
}*/
void VL53L4CXSensor::update() {
  VL53L4CX_MultiRangingData_t MultiRangingData;
  VL53L4CX_MultiRangingData_t *pMultiRangingData = &MultiRangingData;
  uint8_t NewDataReady = 0;
  int status;
  
  // Wait for new measurement data
  do {
    status = this->sensor_vl53l4cx_->VL53L4CX_GetMeasurementDataReady(&NewDataReady);
  } while (!NewDataReady);
  
  if ((!status) && (NewDataReady != 0)) {
    // Get ranging data
    status = this->sensor_vl53l4cx_->VL53L4CX_GetMultiRangingData(pMultiRangingData);
    
    if (status == 0 && pMultiRangingData->NumberOfObjectsFound > 0) {
      // Assuming you want to measure the first object
      int distance = pMultiRangingData->RangeData[0].RangeMilliMeter;
      ESP_LOGI(TAG, "Distance: %d mm", distance);
      
      // Publish the distance to ESPHome
      this->publish_state(distance);
      
      // Clear interrupts and restart the measurement
      this->sensor_vl53l4cx_->VL53L4CX_ClearInterruptAndStartMeasurement();
    }
  } else {
    ESP_LOGW(TAG, "Failed to get measurement data");
    this->publish_state(NAN);
  }
}

/*
void VL53L4CXSensor::update() {
  uint8_t NewDataReady = 0;
  int status = 0;
  VL53L4CX_MultiRangingData_t MultiRangingData;
  VL53L4CX_MultiRangingData_t *pMultiRangingData = &MultiRangingData;

  // Check if new measurement data is ready
  status = this->sensor_vl53l4cx_->VL53L4CX_GetMeasurementDataReady(&NewDataReady);
  if (status || !NewDataReady) {
    ESP_LOGE("VL53L4CX", "Data not ready or error: %d", status);
    return;
  }

  // Get the measurement data
  status = this->sensor_vl53l4cx_->VL53L4CX_GetMultiRangingData(pMultiRangingData);
  if (!status) {
    int num_objects = pMultiRangingData->NumberOfObjectsFound;
    for (int i = 0; i < num_objects; i++) {
      float distance = pMultiRangingData->RangeData[i].RangeMilliMeter / 1000.0;  // convert to meters
      ESP_LOGD("VL53L4CX", "Distance to object %d: %.2f meters", i, distance);
      publish_state(distance);
    }

    // Clear interrupt and prepare for the next measurement
    this->sensor_vl53l4cx_->VL53L4CX_ClearInterruptAndStartMeasurement();
  } else {
    ESP_LOGE("VL53L4CX", "Failed to get ranging data: %d", status);
  }
}
*/

}  // namespace vl53l4cx
}  // namespace esphome
