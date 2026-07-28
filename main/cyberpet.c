#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/i2c_master.h"

#include "esp_log.h"
#include "esp_err.h"

#define I2C_PORT I2C_NUM_0

#define SDA_PIN GPIO_NUM_21
#define SCL_PIN GPIO_NUM_22

#define OLED_ADDR 0x3C

#define I2C_FREQ 400000

static i2c_master_bus_handle_t bus_handle;
static i2c_master_dev_handle_t oled_handle;

void init_i2c()
{
    i2c_master_bus_config_t bus_config = {
        .i2c_port = I2C_PORT,
        .sda_io_num = SDA_PIN,
        .scl_io_num = SCL_PIN,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
    };

    ESP_ERROR_CHECK(
        i2c_new_master_bus(&bus_config, &bus_handle));

    ESP_LOGI("I2C", "Bus initialized");
}

void init_oled()
{
    i2c_device_config_t dev_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = OLED_ADDR,
        .scl_speed_hz = I2C_FREQ,
    };

    ESP_ERROR_CHECK(
        i2c_master_bus_add_device(
            bus_handle,
            &dev_config,
            &oled_handle));

    ESP_LOGI("OLED", "Device added");
}

void oled_command(uint8_t cmd)
{
    uint8_t data[] = {
        0x00,
        cmd};

    ESP_ERROR_CHECK(
        i2c_master_transmit(
            oled_handle,
            data,
            sizeof(data),
            -1));
}

void app_main(void)
{
    init_i2c();

    init_oled();

    ESP_LOGI("MAIN", "Turning display OFF");

    oled_command(0xAE);

    vTaskDelay(pdMS_TO_TICKS(1000));

    ESP_LOGI("MAIN", "Turning display ON");

    oled_command(0xAF);

    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
