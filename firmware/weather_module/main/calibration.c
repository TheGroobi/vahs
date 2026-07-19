#include "calibration.h"
#include "weather.h"
#include <stdio.h>

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

calibration_t parse_first_calibration(uint8_t *buf) {
  calibration_t data = {};
  printf("byte 0: %u", buf[0]);
  printf("byte 1: %u", buf[1]);
  printf("byte 2: %u", buf[2]);
  printf("byte 3: %u", buf[3]);
  printf("byte 4: %u", buf[4]);
  printf("byte 5: %u", buf[5]);
  printf("byte 6: %u", buf[6]);

  int start = 0;
  data.dig_T1 = concat_bytes(buf, start, 2, true);
  start += 2;
  data.dig_T2 = concat_bytes(buf, start, 2, true);
  start += 2;
  data.dig_T3 = concat_bytes(buf, start, 2, true);

  start += 2;
  data.dig_P1 = concat_bytes(buf, start, 2, true);
  start += 2;
  data.dig_P2 = concat_bytes(buf, start, 2, true);
  start += 2;
  data.dig_P3 = concat_bytes(buf, start, 2, true);
  start += 2;
  data.dig_P4 = concat_bytes(buf, start, 2, true);
  start += 2;
  data.dig_P5 = concat_bytes(buf, start, 2, true);
  start += 2;
  data.dig_P6 = concat_bytes(buf, start, 2, true);
  start += 2;
  data.dig_P7 = concat_bytes(buf, start, 2, true);
  start += 2;
  data.dig_P8 = concat_bytes(buf, start, 2, true);
  start += 2;
  data.dig_P9 = concat_bytes(buf, start, 2, true);
  start += 2;
  data.dig_H1 = concat_bytes(buf, start, 2, true);

  return data;
}

calibration_t parse_second_calibration(uint8_t *buf) {
  calibration_t data = {};
  // dih_h4 (first 4 bits) and dig_h5 (second 4 bits) are split at buf[26]
  // printf("This is the buffer of calibration dig_t1 PRIu32 :\n",
  //        concat_bytes(buf, 1, 2, true));
  return data;
}
