#pragma once

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

calibration_t parse_first_calibration(uint8_t *buf);
calibration_t parse_second_calibration(uint8_t *buf);
