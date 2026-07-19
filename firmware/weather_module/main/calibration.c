#include "calibration.h"
#include "weather.h"
#include <driver/i2c_master.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>

// esp_err_t read_calibration(i2c_master_dev_handle_t dev_handle) {
//   uint8_t first_reg = DIG_T1_ADDR;
//   uint8_t second_reg = DIG_T1_ADDR;
//   calibration_t calibration_buf;
//
//   esp_err_t err = read_register(dev_handle, &first_reg, &calibration_buf,
//                                 FIRST_calibration_LENGTH);
//   err = read_register(dev_handle, &second_reg, &calibration_buf,
//                       SECOND_calibration_LENGTH);
//   return err;
// }

calibration_result_t parse_first_calibration(uint8_t *buf, esp_err_t err) {
  calibration_t data = {};
  calibration_result_t result = {};

  if (err != ESP_OK) {
    result.valid = false;
    return result;
  }

  int start = 0;
  data.dig_T1 = (uint16_t)concat_bytes(buf, start, 2, true);
  start += 2;
  data.dig_T2 = (int16_t)concat_bytes(buf, start, 2, true);
  start += 2;
  data.dig_T3 = (int16_t)concat_bytes(buf, start, 2, true);

  start += 2;
  data.dig_P1 = (uint16_t)concat_bytes(buf, start, 2, true);
  start += 2;
  data.dig_P2 = (int16_t)concat_bytes(buf, start, 2, true);
  start += 2;
  data.dig_P3 = (int16_t)concat_bytes(buf, start, 2, true);
  start += 2;
  data.dig_P4 = (int16_t)concat_bytes(buf, start, 2, true);
  start += 2;
  data.dig_P5 = (int16_t)concat_bytes(buf, start, 2, true);
  start += 2;
  data.dig_P6 = (int16_t)concat_bytes(buf, start, 2, true);
  start += 2;
  data.dig_P7 = (int16_t)concat_bytes(buf, start, 2, true);
  start += 2;
  data.dig_P8 = (int16_t)concat_bytes(buf, start, 2, true);
  start += 2;
  data.dig_P9 = (int16_t)concat_bytes(buf, start, 2, true);
  start += 2;
  data.dig_H1 = (uint8_t)concat_bytes(buf, start, 1, true);

  result.calibration = data;
  result.valid = true;
  return result;
}

calibration_result_t parse_second_calibration(uint8_t *buf, esp_err_t err) {
  calibration_t data = {};
  calibration_result_t result = {};

  // mask for 00001111 to get 4 last bits from byte
  uint8_t high_mask = 0x0F;

  if (err != ESP_OK) {
    result.valid = false;
    return result;
  }

  int start = 0;
  data.dig_H2 = (int16_t)concat_bytes(buf, start, 2, true);
  start += 2;
  data.dig_H3 = (uint8_t)concat_bytes(buf, start, 1, true);
  start += 1;
  // buf[start]: 00101010 << 4
  //  00101010 0000 | buf[start+1]: (01010011 >> 4 = 000001010)
  //  00101010 00001010 -> mask the bits
  //  ((buf[start] << 4) | (buf[start+1] >> 4) & high_mask) // this
  data.dig_H4 = (int16_t)((buf[start] << 4) | (buf[start + 1] & high_mask));
  start += 1;
  data.dig_H5 = (int16_t)((buf[start + 1] << 4) | (buf[start] >> 4));
  start += 2;
  data.dig_H6 = (int8_t)concat_bytes(buf, start, 1, true);

  result.calibration = data;
  result.valid = true;
  return result;
}

esp_err_t read_calibration(i2c_master_dev_handle_t dev_handle) {
  uint8_t calibration_reg_1 = DIG_T1_ADDR;
  uint8_t calibration_reg_2 = DIG_H2_ADDR;
  uint8_t calibration_buf_1[FIRST_CALIBRATION_LENGTH] = {0};
  uint8_t calibration_buf_2[SECOND_CALIBRATION_LENGTH] = {0};

  esp_err_t err = i2c_master_transmit_receive(dev_handle, &calibration_reg_1, 1,
                                              calibration_buf_1,
                                              FIRST_CALIBRATION_LENGTH, 1000);
  calibration_result_t calibration1 =
      parse_first_calibration(calibration_buf_1, err);

  err = i2c_master_transmit_receive(dev_handle, &calibration_reg_2, 1,
                                    calibration_buf_2,
                                    SECOND_CALIBRATION_LENGTH, 1000);
  calibration_result_t calibration2 =
      parse_second_calibration(calibration_buf_2, err);

  if (calibration1.valid) {
    printf("T1: %u\n", calibration1.calibration.dig_T1);
    printf("T2: %d\n", calibration1.calibration.dig_T2);
    printf("T3: %d\n", calibration1.calibration.dig_T3);

    printf("P1: %u\n", calibration1.calibration.dig_P1);
    printf("P2: %d\n", calibration1.calibration.dig_P2);
    printf("P3: %d\n", calibration1.calibration.dig_P3);
    printf("P4: %d\n", calibration1.calibration.dig_P4);
    printf("P5: %d\n", calibration1.calibration.dig_P5);
    printf("P6: %d\n", calibration1.calibration.dig_P6);
    printf("P7: %d\n", calibration1.calibration.dig_P7);
    printf("P8: %d\n", calibration1.calibration.dig_P8);
    printf("P9: %d\n", calibration1.calibration.dig_P9);

    printf("H1: %u\n", calibration1.calibration.dig_H1);
    printf("H2: %d\n", calibration2.calibration.dig_H2);
    printf("H3: %u\n", calibration2.calibration.dig_H3);
    printf("H4: %d\n", calibration2.calibration.dig_H4);
    printf("H5: %d\n", calibration2.calibration.dig_H5);
    printf("H6: %d\n", calibration2.calibration.dig_H6);
  } else {
    printf("Invalid calibration - skipping");
  }

  return err;
}
