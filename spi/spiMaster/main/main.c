#include "driver/spi_master.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>

#define PIN_NUM_MOSI 23
#define PIN_NUM_MISO 19
#define PIN_NUM_CLK 18
#define PIN_NUM_CS1 32
#define PIN_NUM_CS2 33

static const char *TAG = "spi_master";

#define BUF_SIZE 64 // reduced buffer size to match the slave

void app_main(void)
{
    esp_err_t ret;

    spi_bus_config_t buscfg = {
        .mosi_io_num = PIN_NUM_MOSI,
        .miso_io_num = PIN_NUM_MISO,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = BUF_SIZE};

    spi_device_interface_config_t devcfg1 = {
        .clock_speed_hz = 5 * 1000 * 1000, // 5 MHz
        .mode = 0,
        .spics_io_num = PIN_NUM_CS1,
        .queue_size = 3,
    };

    spi_device_interface_config_t devcfg2 = {
        .clock_speed_hz = 5 * 1000 * 1000,
        .mode = 0,
        .spics_io_num = PIN_NUM_CS2,
        .queue_size = 3,
    };

    ret = spi_bus_initialize(HSPI_HOST, &buscfg, 1);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to initialize SPI bus: %s", esp_err_to_name(ret));
        return;
    }

    spi_device_handle_t handle1;
    ret = spi_bus_add_device(HSPI_HOST, &devcfg1, &handle1);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to add SPI device 1: %s", esp_err_to_name(ret));
        return;
    }

    spi_device_handle_t handle2;
    ret = spi_bus_add_device(HSPI_HOST, &devcfg2, &handle2);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to add SPI device 2: %s", esp_err_to_name(ret));
        return;
    }

    uint8_t sendbuf1[BUF_SIZE] = "Hello, this is SPI master to slave 1.";
    uint8_t sendbuf2[BUF_SIZE] = "Hello, this is SPI master to slave 2.";

    spi_transaction_t trans1 = {
        .length = BUF_SIZE * 8,
        .tx_buffer = sendbuf1,
        .rx_buffer = NULL};

    spi_transaction_t trans2 = {
        .length = BUF_SIZE * 8,
        .tx_buffer = sendbuf2,
        .rx_buffer = NULL};

    while (1)
    {
        // Communicate with slave 1
        ret = spi_device_transmit(handle1, &trans1);
        if (ret == ESP_OK)
        {
            ESP_LOGI(TAG, "Sent data to slave 1: %s", sendbuf1);
        }
        else
        {
            ESP_LOGE(TAG, "Failed to send SPI data to slave 1: %s", esp_err_to_name(ret));
        }

        // Communicate with slave 2
        ret = spi_device_transmit(handle2, &trans2);
        if (ret == ESP_OK)
        {
            ESP_LOGI(TAG, "Sent data to slave 2: %s", sendbuf2);
        }
        else
        {
            ESP_LOGE(TAG, "Failed to send SPI data to slave 2: %s", esp_err_to_name(ret));
        }

        vTaskDelay(pdMS_TO_TICKS(1000)); // send data every second
    }
}
