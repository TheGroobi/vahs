#include "weather_module.h"
#include "calibration.h"
#include "i2c.h"
#include "weather.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdio.h>

void app_main(void) {
  i2c_master_dev_handle_t dev_handle = i2c_init();

  calibration_result_t calibration;
  ESP_ERROR_CHECK(read_calibration(dev_handle, &calibration));

  // printf("--- calibration ---\n");
  // printf("T1: %u\n", calibration.calibration.dig_T1);
  // printf("T2: %d\n", calibration.calibration.dig_T2);
  // printf("T3: %d\n", calibration.calibration.dig_T3);
  // printf("P1: %u\n", calibration.calibration.dig_P1);
  // printf("P2: %d\n", calibration.calibration.dig_P2);
  // printf("P3: %d\n", calibration.calibration.dig_P3);
  // printf("P4: %d\n", calibration.calibration.dig_P4);
  // printf("P5: %d\n", calibration.calibration.dig_P5);
  // printf("P6: %d\n", calibration.calibration.dig_P6);
  // printf("P7: %d\n", calibration.calibration.dig_P7);
  // printf("P8: %d\n", calibration.calibration.dig_P8);
  // printf("P9: %d\n", calibration.calibration.dig_P9);
  // printf("H1: %u\n", calibration.calibration.dig_H1);
  // printf("H2: %d\n", calibration.calibration.dig_H2);
  // printf("H3: %u\n", calibration.calibration.dig_H3);
  // printf("H4: %d\n", calibration.calibration.dig_H4);
  // printf("H5: %d\n", calibration.calibration.dig_H5);
  // printf("H6: %d\n", calibration.calibration.dig_H6);
  // printf("-------------------\n");

  for (;;) {
    weather_reading_t reading = read_weather(dev_handle, &calibration);
    if (reading.error == ESP_OK) {
      printf("temp: %.2f°C\n", reading.temperature);
      printf("humidity: %.2f%%\n", reading.humidity);
      printf("pressure: %.2f hPa\n", reading.pressure);
    }
    vTaskDelay(1000);
  }
}
