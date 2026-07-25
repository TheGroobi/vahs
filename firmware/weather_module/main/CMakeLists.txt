idf_component_register(SRCS "weather_module.c"
                               "calibration/calibration.c"
                               "weather/weather.c"
                               "i2c/i2c.c"
                               "mqtt/mqtt.c"
                    INCLUDE_DIRS "." "i2c" "weather" "calibration", "mqtt")
