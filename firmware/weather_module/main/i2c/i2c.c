#include "i2c.h"
#include "calibration.h"
#include "driver/i2c_master.h"
#include "driver/i2c_types.h"
#include "esp_err.h"
#include "hal/i2c_types.h"
#include "soc/clk_tree_defs.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdint.h>
#include <stdio.h>

static esp_err_t read_register(i2c_master_dev_handle_t dev_handle, uint8_t *reg,
                               uint8_t *buffer, uint8_t len) {
  return i2c_master_transmit_receive(dev_handle, reg, 1, buffer, len, 1000);
}

esp_err_t bme280_soft_reset(i2c_master_dev_handle_t dev_handle) {
  uint8_t cmd[] = {BME280_REG_RESET, BME280_SOFT_RESET};
  return i2c_master_transmit(dev_handle, cmd, 2, 1000);
}

static esp_err_t enable_humidity(i2c_master_dev_handle_t dev_handle) {
  uint8_t osrs_h = 1; // x1 oversampling
  uint8_t config[] = {BME280_REG_CTRL_HUM, osrs_h};
  return i2c_master_transmit(dev_handle, config, 2, 1000);
}

esp_err_t set_config(i2c_master_dev_handle_t dev_handle) {
  uint8_t osrs_p = 1;
  uint8_t osrs_t = 1;
  uint8_t mode = 3;
  uint8_t value = ((osrs_t << 5) | (osrs_p << 2) | mode);
  uint8_t config[] = {BME280_REG_CTRL_MEAS, value};

  esp_err_t err;
  err = enable_humidity(dev_handle);
  if (err != 0)
    return err;

  return i2c_master_transmit(dev_handle, config, 2, 1000);
}

i2c_master_dev_handle_t i2c_init(void) {
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

  // Verify chip ID
  uint8_t reg = BME280_REG_CHIP_ID;
  uint8_t buf;
  ESP_ERROR_CHECK(read_register(dev_handle, &reg, &buf, 1));
  printf("BME280 ID: %u\n", buf);

  // for (int addr = 1; addr < 127; addr++) {
  //   esp_err_t res = i2c_master_probe(bus_handle, addr, 100);
  //   if (res == ESP_OK) {
  //     printf("Found device at: 0x%02X\n", addr);
  //   }
  // }

  ESP_ERROR_CHECK(set_config(dev_handle));
  vTaskDelay(100);

  return dev_handle;
}
