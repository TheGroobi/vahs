#pragma once

#include "driver/i2c_master.h"
#include "esp_err.h"
#include <stdint.h>

#define DEV_ADDR 0x76
#define CTRL_MEAS_ADDR 0xF4
#define TEMP_ADDR 0xF7
#define CTRL_HUM_ADDR 0xF2
#define WHO_AM_I_ADDR 0xD0
#define BIT_LENGTH 8

typedef struct {
  uint32_t pressure;
  uint32_t temperature;
  uint16_t humidity;
} data_t;

uint32_t concat_bytes(uint8_t *bytes, int start, size_t len, bool little_endian);
data_t parse_weather(uint8_t *buf);
esp_err_t enable_humidity(i2c_master_dev_handle_t dev_handle);
esp_err_t set_config(i2c_master_dev_handle_t dev_handle);
