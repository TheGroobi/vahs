#pragma once

#include "esp_err.h"
#include <driver/i2c_master.h>

#define SDA_PIN 6
#define SCL_PIN 7

#define BME280_I2C_ADDR 0x76
#define BME280_REG_CHIP_ID 0xD0
#define BME280_REG_RESET 0xE0
#define BME280_SOFT_RESET 0xB6
#define BME280_REG_CTRL_HUM 0xF2
#define BME280_REG_CTRL_MEAS 0xF4
#define BME280_REG_DATA 0xF7
#define SENSOR_DATA_LEN 8

i2c_master_dev_handle_t i2c_init(void);
esp_err_t bme280_soft_reset(i2c_master_dev_handle_t dev_handle);
esp_err_t set_config(i2c_master_dev_handle_t dev_handle);
