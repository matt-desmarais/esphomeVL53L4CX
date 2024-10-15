import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c, sensor
from esphome.const import CONF_ID, UNIT_MILLIMETER, ICON_RULER, DEVICE_CLASS_DISTANCE

# Create a namespace for the VL53L4CX component
vl53l4cx_ns = cg.esphome_ns.namespace('vl53l4cx')
VL53L4CXSensor = vl53l4cx_ns.class_('VL53L4CXSensor', sensor.Sensor, cg.PollingComponent, i2c.I2CDevice)

# Configuration schema for the VL53L4CX sensor
CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(VL53L4CXSensor),
}).extend(sensor.SENSOR_SCHEMA).extend(i2c.i2c_device_schema(0x29))  # VL53L4CX I2C address is usually 0x29

# Code generation function for the custom sensor
async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await sensor.register_sensor(var, config)
    await i2c.register_i2c_device(var, config)
