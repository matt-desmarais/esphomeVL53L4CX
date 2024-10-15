esphome_component_register(
  sensor
  INCLUDE_DIRS "."
  SRCS "vl53l4cx.cpp"
  REQUIRES i2c
)
