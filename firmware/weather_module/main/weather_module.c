#include "weather_module.h"
#include "calibration.h"
#include "driver/i2c_master.h"
#include "driver/i2c_types.h"
#include "esp_err.h"
#include "hal/i2c_types.h"
#include "soc/clk_tree_defs.h"
#include "weather.h"
#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>
#include <inttypes.h>
#include <machine/endian.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>

#define STUCK_PRESSURE 524288
#define STUCK_TEMP 524288
#define STUCK_HUMIDITY 32768

esp_err_t read_register(i2c_master_dev_handle_t dev_handle, uint8_t *reg,
                        uint8_t *buffer, uint8_t len) {
  return i2c_master_transmit_receive(dev_handle, reg, 1, buffer, len, 1000);
}

esp_err_t bme280_soft_reset(i2c_master_dev_handle_t dev_handle) {
  uint8_t cmd[] = {BME280_REG_RESET, BME280_SOFT_RESET};
  return i2c_master_transmit(dev_handle, cmd, 2, 1000);
}

esp_err_t read_who_am_i(i2c_master_dev_handle_t dev_handle) {
  uint8_t reg = BME280_REG_CHIP_ID;
  uint8_t buf;
  esp_err_t err = read_register(dev_handle, &reg, &buf, 1);
  printf("BME280 ID: %u\n", buf);
  return err;
}

esp_err_t talk(i2c_master_dev_handle_t dev_handle,
               calibration_result_t *calibration) {
  uint8_t temp_reg = BME280_REG_DATA;
  uint8_t weather_buf[SENSOR_DATA_LEN] = {0};

  esp_err_t weather_err =
      read_register(dev_handle, &temp_reg, weather_buf, SENSOR_DATA_LEN);
  weather_result_t weather = parse_weather(weather_buf, weather_err);

  if (weather_err != ESP_OK) {
    printf("weather read err: %s\n", esp_err_to_name(weather_err));
    return weather_err;
  }

  if (weather.weather.pressure == STUCK_PRESSURE &&
      weather.weather.temperature == STUCK_TEMP &&
      weather.weather.humidity == STUCK_HUMIDITY) {
    printf("sensor stuck, reconfiguring\n");
    bme280_soft_reset(dev_handle);
    vTaskDelay(10);
    set_config(dev_handle);
    vTaskDelay(100);
    return ESP_ERR_INVALID_STATE;
  }

  int32_t temp = compensate_temperature(weather.weather.temperature,
                                        calibration->calibration);
  int32_t humidity =
      compensate_humidity(weather.weather.humidity, calibration->calibration);
  int32_t pressure =
      compensate_pressure(weather.weather.pressure, calibration->calibration);

  printf("pressure raw: %" PRIu32 " | temp raw: %" PRIu32 " | humidity "
         "raw: %u\n",
         weather.weather.pressure, weather.weather.temperature,
         weather.weather.humidity);
  printf("temp: %f°C\n", (double)temp / 100);
  printf("humidity: %f%c \n", (double)humidity / 1024,
         37); // 37 is the ascii for %
  printf("pressure: %f hPa\n", (double)pressure / 256000);

  return ESP_OK;
}

void app_main(void) {
  i2c_master_bus_config_t i2c_mst_config = {
      .clk_source = I2C_CLK_SRC_DEFAULT,
      .i2c_port = I2C_NUM_0,
      .scl_io_num = SCL_PIN,
      .sda_io_num = SDA_PIN,
      .glitch_ignore_cnt = 7,
      .flags.enable_internal_pullup = true,
  };

  i2c_master_bus_handle_t bus_handle;
  ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));

  i2c_device_config_t dev_cfg = {
      .dev_addr_length = I2C_ADDR_BIT_LEN_7,
      .device_address = BME280_I2C_ADDR,
      .scl_speed_hz = 100000,
  };

  i2c_master_dev_handle_t dev_handle;
  ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle));

  ESP_ERROR_CHECK(read_who_am_i(dev_handle));

  for (int addr = 1; addr < 127; addr++) {
    esp_err_t res = i2c_master_probe(bus_handle, addr, 100);
    if (res == ESP_OK) {
      printf("Found device at: 0x%02X\n", addr);
    }
  }

  ESP_ERROR_CHECK(set_config(dev_handle));
  vTaskDelay(100);

  calibration_result_t calibration;
  ESP_ERROR_CHECK(read_calibration(dev_handle, &calibration));

  printf("--- calibration ---\n");
  printf("T1: %u\n", calibration.calibration.dig_T1);
  printf("T2: %d\n", calibration.calibration.dig_T2);
  printf("T3: %d\n", calibration.calibration.dig_T3);
  printf("P1: %u\n", calibration.calibration.dig_P1);
  printf("P2: %d\n", calibration.calibration.dig_P2);
  printf("P3: %d\n", calibration.calibration.dig_P3);
  printf("P4: %d\n", calibration.calibration.dig_P4);
  printf("P5: %d\n", calibration.calibration.dig_P5);
  printf("P6: %d\n", calibration.calibration.dig_P6);
  printf("P7: %d\n", calibration.calibration.dig_P7);
  printf("P8: %d\n", calibration.calibration.dig_P8);
  printf("P9: %d\n", calibration.calibration.dig_P9);
  printf("H1: %u\n", calibration.calibration.dig_H1);
  printf("H2: %d\n", calibration.calibration.dig_H2);
  printf("H3: %u\n", calibration.calibration.dig_H3);
  printf("H4: %d\n", calibration.calibration.dig_H4);
  printf("H5: %d\n", calibration.calibration.dig_H5);
  printf("H6: %d\n", calibration.calibration.dig_H6);
  printf("-------------------\n");

  for (;;) {
    talk(dev_handle, &calibration);
    vTaskDelay(1000);
  };
}
