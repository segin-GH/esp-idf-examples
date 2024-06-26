#include "driver/gpio.h"
#include "driver/spi_slave.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>

#define PIN_NUM_MOSI 23
#define PIN_NUM_MISO 19
#define PIN_NUM_CLK 18
#define PIN_NUM_CS 16

#define LED_GPIO 17

static const char *TAG = "spi_slave";

// function to turn on the led
void IRAM_ATTR turn_on_led(spi_slave_transaction_t *trans)
{
    gpio_set_level(LED_GPIO, 1);
}

// function to turn off the led
void IRAM_ATTR turn_off_led(spi_slave_transaction_t *trans)
{
    gpio_set_level(LED_GPIO, 0);
}

#define BUF_SIZE 2048

void app_main(void)
{
    gpio_pad_select_gpio(LED_GPIO);
    gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);

    esp_err_t ret;
    spi_bus_config_t buscfg = {
        .mosi_io_num = PIN_NUM_MOSI,
        .miso_io_num = PIN_NUM_MISO,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1};

    spi_slave_interface_config_t slvcfg = {
        .spics_io_num = PIN_NUM_CS,
        .flags = 0,
        .queue_size = 3,
        .mode = 0,
        .post_setup_cb = turn_on_led,
        .post_trans_cb = turn_off_led,
    };

    ret = spi_slave_initialize(HSPI_HOST, &buscfg, &slvcfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to initialize SPI slave: %s", esp_err_to_name(ret));
        return;
    }

    char recvbuf[BUF_SIZE] = {0};
    uint8_t sendbuf[BUF_SIZE] = {0}; // Buffer to hold data to send back to the master
    spi_slave_transaction_t trans = {
        .length = BUF_SIZE * 8,
        .rx_buffer = recvbuf,
        .tx_buffer = sendbuf}; // Add tx_buffer to the transaction structure

    while (1)
    {
        memset(recvbuf, 0, BUF_SIZE); // Clear buffer before receiving data
        ret = spi_slave_transmit(HSPI_HOST, &trans, portMAX_DELAY);
        if (ret == ESP_OK)
        {
            ESP_LOGI(TAG, "Received data: %.*s", BUF_SIZE, recvbuf);

            const char *json = "{\"jsonrpc\":\"2.0\",\"result\": {\"ts\": 1234567890},\"id\": 1}";

            // Process received data and prepare response
            snprintf((char *)sendbuf, BUF_SIZE, "%.*s", BUF_SIZE, json);

            memset(recvbuf, 0, BUF_SIZE); // Clear buffer before receiving data

            // Send response to master
            ret = spi_slave_transmit(HSPI_HOST, &trans, portMAX_DELAY);
            if (ret == ESP_OK)
            {
                ESP_LOGI(TAG, "Sent response: %.*s", BUF_SIZE, sendbuf);
            }
            else
            {
                ESP_LOGE(TAG, "Failed to send SPI data: %s", esp_err_to_name(ret));
            }
        }
        else
        {
            ESP_LOGE(TAG, "Failed to receive SPI data: %s", esp_err_to_name(ret));
        }
    }
}
