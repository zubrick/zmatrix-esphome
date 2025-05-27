import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID

CONF_WIDTH = "width"
CONF_HEIGHT = "height"
CONF_SEGMENT_SIZE = "segment_size"

zmatrix_ns = cg.esphome_ns.namespace("zmatrix")
ZMATRIX = zmatrix_ns.class_("ZMatrix", cg.Component)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(ZMATRIX),
        cv.Optional(CONF_WIDTH, default=16): cv.int_range(0, 256),
        cv.Optional(CONF_HEIGHT, default=16): cv.int_range(0, 256),
        cv.Optional(CONF_SEGMENT_SIZE, default=1): cv.int_range(0, 4),
    }
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    cg.add(var.set_width(config[CONF_WIDTH]))
    cg.add(var.set_height(config[CONF_HEIGHT]))
    cg.add(var.set_segment_size(config[CONF_SEGMENT_SIZE]))
