#include "driver/i2c_master.h"
#include "driver/i2c_types.h"
#include "esp_err.h"
#include "hal/i2c_types.h"
#include "soc/clk_tree_defs.h"
#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>
#include <inttypes.h>
#include <stdint.h>
#include <sys/types.h>

#define SDA_PIN 6
#define SCL_PIN 7
#define BIT_LENGTH 8

#define DEV_ADDR 0x76 // Sensor address
#define CTRL_MEAS_ADDR 0xF4
#define TEMP_ADDR 0xF7
#define CTR_HUM_ADDR 0xF2
#define WHO_AM_I_ADDR 0x60

typedef struct {
  uint32_t pressure;
  uint32_t temperature;
  uint16_t humidity;
} bme280_data_t;

// weather order starting from temp_addr
// 24 bits of temperature
// 20 bits of pressure (need to right shift the padding)
// 16 bits of humidity

uint32_t concat_bytes(uint8_t *bytes, int start, size_t len) {
  uint32_t value = 0;

  for (int i = start; i < start + len; i++) {
    value = ((value << 8) | bytes[i]);
  };

  return value;
}

int read_who_am_i(i2c_master_dev_handle_t dev_handle) {
  uint8_t buf;
  uint8_t err;

  err = i2c_master_receive(dev_handle, &buf, 1, 1000);
  printf("BME280 ID: %u\n", buf);
  return err;
}

int enable_humidity(i2c_master_dev_handle_t dev_handle) {
  uint8_t osrs_h = 6; // in binary: 00000111
  uint8_t config[] = {CTR_HUM_ADDR, osrs_h};
  return i2c_master_transmit(dev_handle, config, 2, 1000);
}

int set_config(i2c_master_dev_handle_t dev_handle) {
  uint8_t osrs_p = 1;
  uint8_t osrs_t = 1;
  uint8_t mode = 3;
  uint8_t value = ((osrs_t << 5) | (osrs_p << 2) | mode);
  uint8_t config[] = {CTRL_MEAS_ADDR, value};

  uint8_t err;
  err = enable_humidity(dev_handle);
  if (err != 0)
    return err;

  return i2c_master_transmit(dev_handle, config, 2, 1000);
}

bme280_data_t parse_weather(uint8_t *buf) {
  bme280_data_t data;

  // right shift by 4 to avoid padding
  data.pressure = concat_bytes(buf, 0, 3) >> 4;
  data.temperature = concat_bytes(buf, 3, 3) >> 4;
  data.humidity = concat_bytes(buf, 6, 2);

  return data;
}

int talk(i2c_master_dev_handle_t dev_handle) {
  uint8_t reg = TEMP_ADDR;
  uint8_t buf[BIT_LENGTH] = {0};
  int err;

  err = i2c_master_transmit_receive(dev_handle, &reg, 1, buf, BIT_LENGTH, 1000);

  bme280_data_t weather = parse_weather(buf);

  // compensation for human readable format
  printf("pressure raw: %" PRIu32 "\n", weather.pressure);
  printf("temp raw: %" PRIu32 "\n", weather.temperature);
  printf("humidity raw: %u\n", weather.humidity);

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

  ESP_ERROR_CHECK(enable_humidity(dev_handle));
  vTaskDelay(100);

  for (;;) {
    esp_err_t err = talk(dev_handle);

    if (err != ESP_OK) {
      printf("I2C error: %s\n", esp_err_to_name(err));
    }
    vTaskDelay(1000);
  };
}
