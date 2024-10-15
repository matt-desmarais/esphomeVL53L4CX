import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c, sensor
from esphome.const import CONF_ID, UNIT_MILLIMETER, ICON_RULER, DEVICE_CLASS_DISTANCE

# Create a namespace for the VL53L4CX component
vl53l4cx_ns = cg.esphome_ns.namespace('vl53l4cx')
VL53L4CXSensor = vl53l4cx_ns.class_('VL53L4CXSensor', sensor.Sensor, cg.PollingComponent, i2c.I2CDevice)

# Define the CONFIG_SCHEMA
CONFIG_SCHEMA = sensor.sensor_schema(
    VL53L4CXSensor,
    unit_of_measurement=UNIT_MILLIMETER,
    accuracy_decimals=1,
    icon=ICON_RULER,
    device_class=DEVICE_CLASS_DISTANCE,
).extend({
    cv.GenerateID(): cv.declare_id(VL53L4CXSensor),
    cv.Optional('address', default=0x29): cv.i2c_address,  # Default I2C address for VL53L4CX
}).extend(i2c.i2c_device_schema(0x29))

# Code generation function for VL53L4CX sensor
async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    # Register the component only once
    await sensor.register_sensor(var, config)
    await i2c.register_i2c_device(var, config)
