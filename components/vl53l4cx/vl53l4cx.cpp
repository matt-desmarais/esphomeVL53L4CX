#include "vl53l4cx.h"
#include "esphome/core/log.h"
#include "vl53l4cx_class.h"

namespace esphome {
namespace vl53l4cx {

static const char *TAG = "vl53l4cx";

// Static instance of the VL53L4CX sensor
VL53L4CX *VL53L4CXSensor::sensor_instance = nullptr;

void VL53L4CXSensor::setup() {
  // Force logging level to VERBOSE for all components
  esp_log_level_set("*", ESP_LOG_VERBOSE);

  ESP_LOGI(TAG, "Log level set to VERBOSE");
  ESP_LOGI("vl53l4cx", "Forced log: Entering setup...");

  // Initialize the static VL53L4CX sensor instance using the ESPHome I2C bus
  if (!sensor_instance) {
    sensor_instance = new VL53L4CX(&Wire, A1);  // A1 is the shutdown pin
  }

  // Initialize the I2C bus (ESPHome handles this, so no need for manual initialization)
  Wire.begin();  // Use the standard Wire object
  // Set I2C timeout to 500 milliseconds
  Wire.setTimeout(500);
//  Wire.setClock(50000);  // 50 kHz
  Wire.setClock(1000000); 
  // Begin sensor
  sensor_instance->begin();

  // Turn off the sensor initially
  sensor_instance->VL53L4CX_Off();

  // Initialize the sensor with the I2C address
  if (sensor_instance->InitSensor(0x12) != 0) {
    ESP_LOGE(TAG, "Failed to initialize VL53L4CX sensor.");
    return;
  }

  // Start continuous measurements
  sensor_instance->VL53L4CX_StartMeasurement();

  //sensor_instance->VL53L4CX_SetMeasurementTimingBudgetMicroSeconds(200000);  // Set timing budget to 100ms
  // Set the timing budget (in microseconds)
  sensor_instance->VL53L4CX_SetMeasurementTimingBudgetMicroSeconds(50000);  // 50ms timing budget
  sensor_instance->VL53L4CX_SetInterMeasurementPeriodMilliSeconds(55);  // Set to 55ms

  sensor_instance->VL53L4CX_SetDistanceMode(VL53L4CX_DISTANCEMODE_MEDIUM);  // Switch to long-range mode

  ESP_LOGI(TAG, "VL53L4CX setup complete.");
}

void VL53L4CXSensor::update() {
  ESP_LOGD(TAG, "Starting update: Checking for new measurement data...");

  VL53L4CX_MultiRangingData_t MultiRangingData;
  VL53L4CX_MultiRangingData_t *pMultiRangingData = &MultiRangingData;
  uint8_t NewDataReady = 0;
  int status;
  int retries = 100;  // Adjust retries to avoid long blocking
  int shortest_distance = INT_MAX;

  // Check for new measurement data
  ESP_LOGV(TAG, "Calling VL53L4CX_GetMeasurementDataReady...");
  status = sensor_instance->VL53L4CX_GetMeasurementDataReady(&NewDataReady);

  if (status != 0) {
    ESP_LOGE(TAG, "VL53L4CX_GetMeasurementDataReady failed with status: %d", status);
    this->publish_state(NAN);
    return;
  }

  ESP_LOGV(TAG, "Measurement Data Ready status: %d, NewDataReady: %d", status, NewDataReady);

  if (NewDataReady == 0) {
    ESP_LOGW(TAG, "No new data available.");
    this->publish_state(NAN);
    return;
  }

  // Get ranging data
  ESP_LOGV(TAG, "Calling VL53L4CX_GetMultiRangingData...");
  status = sensor_instance->VL53L4CX_GetMultiRangingData(pMultiRangingData);

  if (status != 0) {
    ESP_LOGE(TAG, "VL53L4CX_GetMultiRangingData failed with status: %d", status);
    this->publish_state(NAN);
    return;
  }

  ESP_LOGV(TAG, "Ranging data retrieved successfully. Number of objects found: %d", pMultiRangingData->NumberOfObjectsFound);

  if (pMultiRangingData->NumberOfObjectsFound > 0) {
    for (int i = 0; i < pMultiRangingData->NumberOfObjectsFound; i++) {
      int distance = pMultiRangingData->RangeData[i].RangeMilliMeter;
      ESP_LOGI(TAG, "Object %d: Distance: %d mm", i, distance);

    // Ignore invalid negative values
    if (distance < 0) {
      ESP_LOGW(TAG, "Object %d: Invalid negative distance: %d mm", i, distance);
      continue;
    }
      
      // Update the shortest distance
      if (distance > 0 && distance < 8000 && distance < shortest_distance) {
        shortest_distance = distance;
      }
    }
    ESP_LOGI(TAG, "Shortest Distance: %d mm", shortest_distance);
    this->publish_state(shortest_distance);
  } else {
    ESP_LOGW(TAG, "No objects found or failed to retrieve ranging data.");
    this->publish_state(NAN);
  }

  // Clear interrupts and restart the measurement
  ESP_LOGV(TAG, "Clearing interrupts and starting a new measurement...");
  status = sensor_instance->VL53L4CX_ClearInterruptAndStartMeasurement();

  if (status != 0) {
    ESP_LOGE(TAG, "VL53L4CX_ClearInterruptAndStartMeasurement failed with status: %d", status);
  } else {
    ESP_LOGV(TAG, "Measurement restarted successfully.");
  }
}


/*
void VL53L4CXSensor::update() {
  ESP_LOGD(TAG, "Checking for new measurement data...");
  VL53L4CX_MultiRangingData_t MultiRangingData;
  VL53L4CX_MultiRangingData_t *pMultiRangingData = &MultiRangingData;
  uint8_t NewDataReady = 0;
  int status;
  // Turn on LED to indicate processing (if applicable)
//  digitalWrite(LedPin, HIGH); 
  // Wait for new measurement data
  do {
    status = sensor_instance->VL53L4CX_GetMeasurementDataReady(&NewDataReady);
    ESP_LOGV(TAG, "Measurement Data Ready status: %d, NewDataReady: %d", status, NewDataReady);
  } while (!NewDataReady);
  if ((!status) && (NewDataReady != 0)) {
    // Get ranging data
    ESP_LOGV(TAG, "Calling VL53L4CX_GetMultiRangingData...");
    status = sensor_instance->VL53L4CX_GetMultiRangingData(pMultiRangingData);
    ESP_LOGV(TAG, "Ranging data retrieval status: %d", status);
    if (status == 0 && pMultiRangingData->NumberOfObjectsFound > 0) {
      // Initialize the shortest distance with a large number
      int shortest_distance = INT_MAX;
      // Iterate over all detected objects to find the shortest distance
      for (int j = 0; j < pMultiRangingData->NumberOfObjectsFound; j++) {
        int distance = pMultiRangingData->RangeData[j].RangeMilliMeter;
        ESP_LOGI(TAG, "Object %d: Distance: %d mm, Signal=%.2f Mcps, Ambient=%.2f Mcps", 
                  j, 
                  distance, 
                  (float)pMultiRangingData->RangeData[j].SignalRateRtnMegaCps / 65536.0, 
                  (float)pMultiRangingData->RangeData[j].AmbientRateRtnMegaCps / 65536.0);
        
        // Update the shortest distance if the current one is smaller
        if (distance < shortest_distance) {
          shortest_distance = distance;
        }
      }
      // Publish the shortest distance
      ESP_LOGI(TAG, "Shortest Distance: %d mm", shortest_distance);
      this->publish_state(shortest_distance);
      // Clear interrupts and restart the measurement
      sensor_instance->VL53L4CX_ClearInterruptAndStartMeasurement();
    } else {
      ESP_LOGW(TAG, "No objects found or failed to retrieve ranging data.");
      this->publish_state(NAN);  // Publish NaN when there's no valid data
    }
  } else {
    ESP_LOGW(TAG, "Failed to get measurement data.");
    this->publish_state(NAN);
  }
  // Turn off LED after processing
  //digitalWrite(LedPin, LOW);  
}
*/


/*
void VL53L4CXSensor::update() {
  ESP_LOGD(TAG, "Checking for new measurement data...");
  VL53L4CX_MultiRangingData_t MultiRangingData;
  VL53L4CX_MultiRangingData_t *pMultiRangingData = &MultiRangingData;
  uint8_t NewDataReady = 0;
  int status;
  // Wait for new measurement data
  do {
    status = sensor_instance->VL53L4CX_GetMeasurementDataReady(&NewDataReady);
  } while (!NewDataReady);
  if ((!status) && (NewDataReady != 0)) {
    // Get ranging data
    status = sensor_instance->VL53L4CX_GetMultiRangingData(pMultiRangingData);
    if (status == 0 && pMultiRangingData->NumberOfObjectsFound > 0) {
      // Assuming you want to measure the first object
      int distance = pMultiRangingData->RangeData[0].RangeMilliMeter;
      ESP_LOGI(TAG, "Distance: %d mm", distance);
      // Publish the distance to ESPHome
      this->publish_state(distance);
      // Clear interrupts and restart the measurement
      sensor_instance->VL53L4CX_ClearInterruptAndStartMeasurement();
    }
  } else {
    ESP_LOGW(TAG, "Failed to get measurement data");
    this->publish_state(NAN);
  }
}
*/
float VL53L4CXSensor::get_setup_priority() const {
//  return setup_priority::DATA;  // Use appropriate setup priority (can be HARDWARE, DATA, etc.)
  return setup_priority::HARDWARE;
}

}  // namespace vl53l4cx
}  // namespace esphome
