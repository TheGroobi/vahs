#include "driver/i2c_master.h"
#include "driver/i2c_types.h"
#include "esp_err.h"
#include "hal/i2c_types.h"
#include "soc/clk_tree_defs.h"
#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>

#define SDA_PIN 6
#define SCL_PIN 7

#define DEVICE_ADDRESS 0x76 // ESP32-S3 address

int talk(i2c_master_dev_handle_t dev_handle) {
  uint8_t reg = 0xD0;
  uint8_t data;

  int res = i2c_master_transmit_receive(dev_handle, &reg, 1, &data, 1, 1000);
  printf("Chip ID: 0x%02X\n", data);
  return res;
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
      .device_address = DEVICE_ADDRESS,
      .scl_speed_hz = 100000,
  };

  i2c_master_dev_handle_t dev_handle;
  ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle));

  // look for the address of the pin
  // for (int addr = 1; addr < 127; addr++) {
  //   printf("skipped0x%02X\n", addr);
  //   esp_err_t res = i2c_master_probe(bus_handle, addr, 100);
  //   if (res == ESP_OK) {
  //     printf("Found device at: 0x%02X\n", addr);
  //   }
  // }
  //

  while (1) {
    ESP_ERROR_CHECK(talk(dev_handle));
    vTaskDelay(2000);
  };
}
