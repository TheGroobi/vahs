#pragma once

#include "esp_err.h"
#include <driver/i2c_master.h>
#include <stdint.h>

#define DIG_T1_ADDR 0x88
#define FIRST_CALIBRATION_LENGTH 23

#define DIG_H2_ADDR 0xE1
#define SECOND_CALIBRATION_LENGTH 7

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
esp_err_t read_calibration(i2c_master_dev_handle_t dev_handle);
