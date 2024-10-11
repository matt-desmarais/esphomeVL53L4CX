import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import UNIT_MILLIMETER, ICON_RULER, CONF_ID

vl53l4cx_ns = cg.esphome_ns.namespace('vl53l4cx')
VL53L4CXSensor = vl53l4cx_ns.class_('VL53L4CXSensor', sensor.Sensor, cg.PollingComponent)

CONFIG_SCHEMA = sensor.sensor_schema().extend({
    cv.Optional(CONF_UNIT_OF_MEASUREMENT, default=UNIT_MILLIMETER): cv.string,
    cv.Optional(CONF_ICON, default=ICON_RULER): cv.icon,
    cv.Optional(CONF_ACCURACY_DECIMALS, default=0): cv.int_,
}).extend(cv.polling_component_schema('60s')).extend(i2c.I2C_DEVICE_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await sensor.register_sensor(var, config)

sensor.register_sensor_platform('vl53l4cx', CONFIG_SCHEMA, to_code)
