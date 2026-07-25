#include "weather.h"
#include "calibration.h"
#include "i2c.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdint.h>
#include <stdio.h>

#define STUCK_PRESSURE 524288
#define STUCK_TEMP 524288
#define STUCK_HUMIDITY 32768

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

static void parse_weather(uint8_t *buf, uint32_t *pressure, uint32_t *temp,
                          uint16_t *humidity) {
  *pressure = (concat_bytes(buf, 0, 3, false) >> 4) & 0x3FFFFFFF;
  *temp = (concat_bytes(buf, 3, 3, false) >> 4) & 0x1FFFFF;
  *humidity = concat_bytes(buf, 6, 2, false);
}

weather_reading_t read_weather(i2c_master_dev_handle_t dev_handle,
                              calibration_result_t *calibration) {
  weather_reading_t result = {};

  uint8_t temp_reg = BME280_REG_DATA;
  uint8_t weather_buf[SENSOR_DATA_LEN] = {0};

  result.error =
      i2c_master_transmit_receive(dev_handle, &temp_reg, 1, weather_buf,
                                  SENSOR_DATA_LEN, 1000);

  if (result.error != ESP_OK) {
    printf("weather read err: %s\n", esp_err_to_name(result.error));
    return result;
  }

  uint32_t raw_pressure, raw_temp;
  uint16_t raw_humidity;
  parse_weather(weather_buf, &raw_pressure, &raw_temp, &raw_humidity);

  if (raw_pressure == STUCK_PRESSURE && raw_temp == STUCK_TEMP &&
      raw_humidity == STUCK_HUMIDITY) {
    printf("sensor stuck, reconfiguring\n");
    bme280_soft_reset(dev_handle);
    vTaskDelay(10);
    set_config(dev_handle);
    vTaskDelay(100);
    result.error = ESP_ERR_INVALID_STATE;
    return result;
  }

  int32_t temp =
      compensate_temperature(raw_temp, calibration->calibration);
  int32_t humidity =
      compensate_humidity(raw_humidity, calibration->calibration);
  int32_t pressure =
      compensate_pressure(raw_pressure, calibration->calibration);

  result.temperature = temp / 100.0f;
  result.humidity = humidity / 1024.0f;
  result.pressure = pressure / 25600.0f;

  return result;
}
