#pragma once

#include "calibration.h"
#include "esp_err.h"
#include <driver/i2c_master.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct {
  float temperature;
  float humidity;
  float pressure;
  esp_err_t error;
} weather_reading_t;

uint32_t concat_bytes(uint8_t *bytes, int start, size_t len,
                      bool little_endian);
weather_reading_t read_weather(i2c_master_dev_handle_t dev_handle,
                              calibration_result_t *calibration);
