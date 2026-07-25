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

esp_err_t read_calibration(i2c_master_dev_handle_t dev_handle,
                           calibration_result_t *c) {
  uint8_t calibration_reg_1 = BME280_REG_CALIB_FIRST;
  uint8_t calibration_reg_2 = BME280_REG_CALIB_SECOND;
  uint8_t calibration_buf_1[BME280_CALIB_FIRST_LEN] = {0};
  uint8_t calibration_buf_2[BME280_CALIB_SECOND_LEN] = {0};

  esp_err_t err = i2c_master_transmit_receive(dev_handle, &calibration_reg_1, 1,
                                              calibration_buf_1,
                                              BME280_CALIB_FIRST_LEN, 1000);
  calibration_result_t calibration1 =
      parse_first_calibration(calibration_buf_1, err);

  err = i2c_master_transmit_receive(dev_handle, &calibration_reg_2, 1,
                                    calibration_buf_2, BME280_CALIB_SECOND_LEN,
                                    1000);
  calibration_result_t calibration2 =
      parse_second_calibration(calibration_buf_2, err);

  printf("first valid: %d\n", calibration1.valid);
  printf("second valid: %d\n", calibration2.valid);
  printf("first err: %s\n", esp_err_to_name(err));

  if (!calibration1.valid || !calibration2.valid) {
    c->valid = false;
    return ESP_ERR_INVALID_STATE;
  }

  c->calibration.dig_T1 = calibration1.calibration.dig_T1;
  c->calibration.dig_T2 = calibration1.calibration.dig_T2;
  c->calibration.dig_T3 = calibration1.calibration.dig_T3;

  c->calibration.dig_P1 = calibration1.calibration.dig_P1;
  c->calibration.dig_P2 = calibration1.calibration.dig_P2;
  c->calibration.dig_P3 = calibration1.calibration.dig_P3;
  c->calibration.dig_P4 = calibration1.calibration.dig_P4;
  c->calibration.dig_P5 = calibration1.calibration.dig_P5;
  c->calibration.dig_P6 = calibration1.calibration.dig_P6;
  c->calibration.dig_P7 = calibration1.calibration.dig_P7;
  c->calibration.dig_P8 = calibration1.calibration.dig_P8;
  c->calibration.dig_P9 = calibration1.calibration.dig_P9;
  c->calibration.dig_H1 = calibration1.calibration.dig_H1;

  c->calibration.dig_H2 = calibration2.calibration.dig_H2;
  c->calibration.dig_H3 = calibration2.calibration.dig_H3;
  c->calibration.dig_H4 = calibration2.calibration.dig_H4;
  c->calibration.dig_H5 = calibration2.calibration.dig_H5;
  c->calibration.dig_H6 = calibration2.calibration.dig_H6;

  c->valid = true;
  return ESP_OK;
}

// Returns temperature in DegC, resolution to 0.01 DegC. Output value of "5123"
// equals 51.23 DegC temp_fine carries fine temperature as a global value,
// needed for other compensations
int32_t temp_fine; // t_fine in datasheet
int32_t compensate_temperature(int32_t adc_temp, calibration_t c) {
  int32_t var1, var2, temp;
  var1 =
      ((((adc_temp >> 3) - ((int32_t)c.dig_T1 << 1))) * ((int32_t)c.dig_T2)) >>
      11;
  var2 = (((((adc_temp >> 4) - ((int32_t)c.dig_T1)) *
            ((adc_temp >> 4) - ((int32_t)c.dig_T1))) >>
           12) *
          ((int32_t)c.dig_T3)) >>
         14;
  temp_fine = var1 + var2;
  temp = (temp_fine * 5 + 128) >> 8;

  return temp;
}

// Output value of "47445" represents 47445/1024 = 46.333 %RH
uint32_t compensate_humidity(int32_t adc_hum, calibration_t c) {
  int32_t v_x1_u32r;
  v_x1_u32r = (temp_fine - ((int32_t)76800));

  v_x1_u32r =
      (((((adc_hum << 14) - (((int32_t)c.dig_H4) << 20) -
          (((int32_t)c.dig_H5) * v_x1_u32r)) +
         (int32_t)16384)) >>
       15) *
      (((((((v_x1_u32r * ((int32_t)c.dig_H6)) >> 10) *
           (((v_x1_u32r * ((int32_t)c.dig_H3)) >> 11) + ((int32_t)32768))) >>
          10) +
         ((int32_t)2097152)) *
            ((int32_t)c.dig_H2) +
        8192) >>
       14);
  v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) *
                             ((int32_t)c.dig_H1)) >>
                            4));
  v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
  v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);
  return (uint32_t)(v_x1_u32r >> 12);
}

// output value of "24674867" represents 24674867/256 = 96386.2 Pa = 963.862 hPa
uint32_t compensate_pressure(int32_t adc_pres, calibration_t c) {
  int64_t var1, var2, pressure;
  var1 = ((int64_t)temp_fine) - 128000;
  var2 = var1 * var1 * (int64_t)c.dig_P6;
  var2 = var2 + ((var1 * (int64_t)c.dig_P5) << 17);
  var2 = var2 + (((int64_t)c.dig_P4) << 35);
  var1 = ((var1 * var1 * (int64_t)c.dig_P3) >> 8) +
         ((var1 * (int64_t)c.dig_P2) << 12);
  var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)c.dig_P1) >> 33;
  if (var1 == 0) {
    return 0;
  };
  pressure = 1048576 - adc_pres;
  pressure = (((pressure << 31) - var2) * 3125) / var1;
  var1 = (((int64_t)c.dig_P9) * (pressure >> 13) * (pressure >> 13)) >> 25;
  var2 = (((int64_t)c.dig_P8) * pressure) >> 19;
  pressure = ((pressure + var1 + var2) >> 8) + (((int64_t)c.dig_P7) << 4);
  return (uint32_t)pressure;
}
