#include "weather.h"

uint32_t concat_bytes(uint8_t *bytes, int start, size_t len,
                      bool little_endian) {
  uint32_t value = 0;

  if (little_endian) {
    for (int i = start + len - 1; i >= start; i--) {
      value = (bytes[i] | (value << 8));
    };
  } else {
    for (int i = start; i < start + len; i++) {
      value = ((value << 8) | bytes[i]);
    };
  }

  return value;
}

weather_result_t parse_weather(uint8_t *buf, esp_err_t err) {
  data_t data;
  weather_result_t result;

  // right shift by 4 to avoid padding
  data.pressure = concat_bytes(buf, 0, 3, false) >> 4;
  data.temperature = concat_bytes(buf, 3, 3, false) >> 4;
  data.humidity = concat_bytes(buf, 6, 2, false);

  result.weather = data;
  result.valid = err == 0;
  return result;
}

esp_err_t enable_humidity(i2c_master_dev_handle_t dev_handle) {
  uint8_t osrs_h = 6; // in binary: 00000111
  uint8_t config[] = {CTRL_HUM_ADDR, osrs_h};
  return i2c_master_transmit(dev_handle, config, 2, 1000);
}

esp_err_t set_config(i2c_master_dev_handle_t dev_handle) {
  uint8_t osrs_p = 1;
  uint8_t osrs_t = 1;
  uint8_t mode = 3;
  uint8_t value = ((osrs_t << 5) | (osrs_p << 2) | mode);
  uint8_t config[] = {CTRL_MEAS_ADDR, value};

  esp_err_t err;
  err = enable_humidity(dev_handle);
  if (err != 0)
    return err;

  return i2c_master_transmit(dev_handle, config, 2, 1000);
}
