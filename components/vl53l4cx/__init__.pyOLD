import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c, sensor
from esphome.const import ICON_RULER, UNIT_MILLIMETER

# Define VL53L4CX as a dependency of i2c
DEPENDENCIES = ["i2c"]

# Define the VL53L4CX namespace
vl53l4cx_ns = cg.esphome_ns.namespace("vl53l4cx")
VL53L4CXSensor = vl53l4cx_ns.class_("VL53L4CXSensor", cg.PollingComponent, i2c.I2CDevice)

# Define the configuration schema for VL53L4CX sensor
CONFIG_SCHEMA = (
    sensor.sensor_schema(
        VL53L4CXSensor,
        unit_of_measurement=UNIT_MILLIMETER,
        icon=ICON_RULER,
        accuracy_decimals=1,
    )
    .extend(cv.polling_component_schema("5s"))  # Poll every 5 seconds by default
    .extend(i2c.i2c_device_schema(0x29))  # Default I2C address of VL53L4CX is 0x29
)


# Async function to generate the code for the sensor component
async def to_code(config):
    # Create a new instance of the VL53L4CX sensor
    var = await sensor.new_sensor(config)

    # Register the component (PollingComponent and I2CDevice) in ESPHome
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)

    # Additional sensor initialization or configuration can be added here
    # For example, setting up range modes or thresholds if needed

