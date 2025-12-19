import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_TIME_ID, CONF_UPDATE_INTERVAL, CONF_POLLING_INTERVAL
from esphome.components import i2c, time

CONF_I2C_ADDRESS = "i2c_address"

DEPENDENCIES = ["i2c"]

ds3231_ns = cg.esphome_ns.namespace("ds3231")
DS3231 = ds3231_ns.class_("DS3231", time.RealTimeClock, i2c.I2CDevice)

CONFIG_SCHEMA = cv.All(
    time.TIME_SCHEMA.extend(
        {
            cv.GenerateID(): cv.declare_id(DS3231),
            cv.Optional(CONF_TIME_ID): cv.use_id(time.RealTimeClock),
            cv.Optional( CONF_UPDATE_INTERVAL): cv.positive_time_period_minutes,
            cv.Optional( CONF_POLLING_INTERVAL): cv.positive_time_period_minutes,
        }
    ).extend(i2c.i2c_device_schema(0x68))
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)
    if rf := config.get(CONF_TIME_ID):
        reftime = await cg.get_variable(rf)
        cg.add(var.set_reftime(reftime))
    if inter := config.get(CONF_UPDATE_INTERVAL):
        cg.add(var.set_update_interval_min(inter))
    if readinter := config.get(CONF_POLLING_INTERVAL):
        cg.add(var.set_read_interval_min(readinter))
