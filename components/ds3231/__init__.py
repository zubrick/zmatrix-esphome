import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_TIME_ID, CONF_UPDATE_INTERVAL
from esphome.components import time

CONF_I2C_ADDRESS = "i2c_address"

ds3231_ns = cg.esphome_ns.namespace("ds3231")
DS3231 = ds3231_ns.class_("DS3231", cg.Component)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(DS3231),
        cv.Optional(CONF_I2C_ADDRESS, default=0x68): cv.int_range(0x00, 0xff),
        cv.Optional(CONF_TIME_ID): cv.use_id(time.RealTimeClock),
        cv.Optional( CONF_UPDATE_INTERVAL): cv.positive_time_period_minutes,
    }
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    if rf := config.get(CONF_TIME_ID):
        reftime = await cg.get_variable(rf)
        cg.add(var.set_reftime(reftime))
    if inter := config.get(CONF_UPDATE_INTERVAL):
        cg.add(var.set_update_interval_min(inter))

    cg.add(var.set_i2c_address(config[CONF_I2C_ADDRESS]))
