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

#define SDA_PIN 6
#define SCL_PIN 7

esp_err_t read_register(i2c_master_dev_handle_t dev_handle, uint8_t *reg,
                        uint8_t *buffer, uint8_t len) {
  return i2c_master_transmit_receive(dev_handle, reg, 1, buffer, len, 1000);
}
esp_err_t read_who_am_i(i2c_master_dev_handle_t dev_handle) {
  uint8_t reg = WHO_AM_I_ADDR;
  uint8_t buf;
  esp_err_t err = read_register(dev_handle, &reg, &buf, 1);
  printf("BME280 ID: %u\n", buf);
  return err;
}

esp_err_t talk(i2c_master_dev_handle_t dev_handle) {
  uint8_t temp_reg = TEMP_ADDR;
  uint8_t weather_buf[BIT_LENGTH] = {0};

  esp_err_t err = read_register(dev_handle, &temp_reg, weather_buf, BIT_LENGTH);
  weather_result_t weather = parse_weather(weather_buf, err);

  if (weather.valid) {
    printf("pressure raw: %" PRIu32 "\n", weather.weather.pressure);
    printf("temp raw: %" PRIu32 "\n", weather.weather.temperature);
    printf("humidity raw: %u\n", weather.weather.humidity);
  } else {
    printf("Invalid weather - skipping");
  }
  return err;
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
      .device_address = DEV_ADDR,
      .scl_speed_hz = 100000,
  };

  i2c_master_dev_handle_t dev_handle;
  ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle));

  ESP_ERROR_CHECK(read_who_am_i(dev_handle));
  // look for the address of the pin
  for (int addr = 1; addr < 127; addr++) {
    esp_err_t res = i2c_master_probe(bus_handle, addr, 100);
    if (res == ESP_OK) {
      printf("Found device at: 0x%02X\n", addr);
    }
  }

  ESP_ERROR_CHECK(set_config(dev_handle));
  vTaskDelay(100);

  ESP_ERROR_CHECK(read_calibration(dev_handle));

  for (;;) {
    esp_err_t err = talk(dev_handle);

    if (err != ESP_OK) {
      printf("I2C error: %s\n", esp_err_to_name(err));
    }
    vTaskDelay(1000);
  };
}
