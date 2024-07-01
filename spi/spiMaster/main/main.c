#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdint.h>
#include <string.h>

#define PIN_NUM_MOSI 23
#define PIN_NUM_MISO 19
#define PIN_NUM_CLK 18
#define PIN_NUM_CS1 32
#define PIN_NUM_CS2 33
#define PIN_NUM_CS3 25
static const char *TAG = "spi_master";

#define BUF_SIZE 2048

void app_main(void)
{
    esp_err_t ret;

    gpio_set_pull_mode(PIN_NUM_MOSI, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(PIN_NUM_MISO, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(PIN_NUM_CLK, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(PIN_NUM_CS1, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(PIN_NUM_CS2, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(PIN_NUM_CS3, GPIO_PULLUP_ONLY);

    spi_bus_config_t buscfg = {
        .mosi_io_num = PIN_NUM_MOSI,
        .miso_io_num = PIN_NUM_MISO,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = BUF_SIZE};

    spi_device_interface_config_t devcfg1 = {
        .command_bits = 0,
        .address_bits = 0,
        .dummy_bits = 0,
        .clock_speed_hz = 1 * 1000 * 1000, // 5 MHz
        .duty_cycle_pos = 128,
        .mode = 0,
        .spics_io_num = PIN_NUM_CS1,
        .queue_size = 3,
        .cs_ena_posttrans = 3,

    };

    spi_device_interface_config_t devcfg2 = {
        .command_bits = 0,
        .address_bits = 0,
        .dummy_bits = 0,
        .clock_speed_hz = 1 * 1000 * 1000, // 5 MHz
        .duty_cycle_pos = 128,
        .mode = 0,
        .spics_io_num = PIN_NUM_CS2,
        .queue_size = 3,
        .cs_ena_posttrans = 3,
    };

    spi_device_interface_config_t devcfg3 = {
        .command_bits = 0,
        .address_bits = 0,
        .dummy_bits = 0,
        .clock_speed_hz = 1 * 1000 * 1000, // 5 MHz
        .duty_cycle_pos = 128,
        .mode = 0,
        .spics_io_num = PIN_NUM_CS3,
        .queue_size = 3,
        .cs_ena_posttrans = 3,
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

    spi_device_handle_t handle3;
    ret = spi_bus_add_device(HSPI_HOST, &devcfg3, &handle3);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to add SPI device 2: %s", esp_err_to_name(ret));
        return;
    }

    uint8_t recvbuf1[BUF_SIZE] = {0};
    uint8_t sendbuf1[BUF_SIZE];
    memset(sendbuf1, 'a', sizeof(sendbuf1));

    uint8_t sendbuf2[BUF_SIZE] = {0};
    uint8_t recvbuf2[BUF_SIZE] = {0};
    memset(sendbuf2, 'b', sizeof(sendbuf2));

    spi_transaction_t trans1_send = {
        .length = BUF_SIZE * 8,
        .tx_buffer = sendbuf1,
        .rx_buffer = NULL};

    spi_transaction_t trans1_recv = {
        .length = BUF_SIZE * 8,
        .tx_buffer = NULL,
        .rx_buffer = recvbuf1};

    spi_transaction_t trans2_send = {
        .length = BUF_SIZE * 8,
        .tx_buffer = sendbuf2,
        .rx_buffer = NULL};

    spi_transaction_t trans2_recv = {
        .length = BUF_SIZE * 8,
        .tx_buffer = NULL,
        .rx_buffer = recvbuf2};

    while (1)
    {
        // Communicate with slave 1
        ret = spi_device_transmit(handle1, &trans1_send);
        if (ret == ESP_OK)
        {
            ESP_LOGI(TAG, "Sent data to slave 1: %.*s", BUF_SIZE, sendbuf1);
        }
        else
        {
            ESP_LOGE(TAG, "Failed to send SPI data to slave 1: %s", esp_err_to_name(ret));
        }

        memset(recvbuf1, 0, BUF_SIZE);
        ret = spi_device_transmit(handle1, &trans1_recv);
        if (ret == ESP_OK)
        {
            ESP_LOGI(TAG, "Received data from slave 1: %.*s", BUF_SIZE, recvbuf1);
        }
        else
        {
            ESP_LOGE(TAG, "Failed to receive SPI data from slave 1: %s", esp_err_to_name(ret));
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);

        // Communicate with slave 2
        ret = spi_device_transmit(handle2, &trans2_send);
        if (ret == ESP_OK)
        {
            ESP_LOGI(TAG, "Sent data to slave 2: %.*s", BUF_SIZE, sendbuf2);
        }
        else
        {
            ESP_LOGE(TAG, "Failed to send SPI data to slave 2: %s", esp_err_to_name(ret));
        }

        // Receive data from slave 2
        memset(recvbuf2, 0, BUF_SIZE);
        ret = spi_device_transmit(handle2, &trans2_recv);
        if (ret == ESP_OK)
        {
            ESP_LOGI(TAG, "Received data from slave 2: %.*s", BUF_SIZE, recvbuf2);
        }
        else
        {
            ESP_LOGE(TAG, "Failed to receive SPI data from slave 2: %s", esp_err_to_name(ret));
        }

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
