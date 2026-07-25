#pragma once

#include "calibration.h"
#include "esp_err.h"
#include <driver/i2c_master.h>

#define SDA_PIN 6
#define SCL_PIN 7
#define BME280_REG_RESET 0xE0
#define BME280_SOFT_RESET 0xB6

esp_err_t read_who_am_i(i2c_master_dev_handle_t dev_handle);
esp_err_t talk(i2c_master_dev_handle_t dev_handle,
               calibration_result_t *calibration);
