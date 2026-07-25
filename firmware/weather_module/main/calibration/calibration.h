#pragma once

#include "esp_err.h"
#include <driver/i2c_master.h>
#include <stdbool.h>
#include <stdint.h>

#define BME280_REG_CALIB_FIRST 0x88
#define BME280_CALIB_FIRST_LEN 26

#define BME280_REG_CALIB_SECOND 0xE1
#define BME280_CALIB_SECOND_LEN 7

typedef struct {
  uint16_t dig_T1;
  int16_t dig_T2;
  int16_t dig_T3;

  uint16_t dig_P1;
  int16_t dig_P2;
  int16_t dig_P3;
  int16_t dig_P4;
  int16_t dig_P5;
  int16_t dig_P6;
  int16_t dig_P7;
  int16_t dig_P8;
  int16_t dig_P9;

  uint8_t dig_H1;
  int16_t dig_H2;
  uint8_t dig_H3;
  int16_t dig_H4;
  int16_t dig_H5;
  int8_t dig_H6;
} calibration_t;

typedef struct {
  calibration_t calibration;
  bool valid;
} calibration_result_t;

calibration_result_t parse_first_calibration(uint8_t *buf, esp_err_t err);
calibration_result_t parse_second_calibration(uint8_t *buf, esp_err_t err);
esp_err_t read_calibration(i2c_master_dev_handle_t dev_handle,
                           calibration_result_t *c);

int32_t compensate_temperature(int32_t adc_temp, calibration_t c);
uint32_t compensate_humidity(int32_t adc_hum, calibration_t c);
uint32_t compensate_pressure(int32_t adc_pres, calibration_t c);
