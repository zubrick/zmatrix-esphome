import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_DISPLAY_ID, CONF_UPDATE_INTERVAL, CONF_TIME_ID
from esphome.components import time, display

zclock_ns = cg.esphome_ns.namespace("zclock")
Zclock = zclock_ns.class_("ZClock", cg.Component)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(Zclock),
        cv.Required (CONF_DISPLAY_ID): cv.use_id(display.Display),
        cv.Required (CONF_TIME_ID): cv.use_id(time.RealTimeClock),
        cv.Optional( CONF_UPDATE_INTERVAL, default="10ms"): cv.positive_time_period_milliseconds,
    }
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    if rf := config.get(CONF_TIME_ID):
        reftime = await cg.get_variable(rf)
        cg.add(var.set_reftime(reftime))
    if di := config.get(CONF_DISPLAY_ID):
        display = await cg.get_variable(di)
        cg.add(var.set_display(display))
    if inter := config.get(CONF_UPDATE_INTERVAL):
        cg.add(var.set_update_interval_ms(inter))
