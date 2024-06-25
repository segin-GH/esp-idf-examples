#include "driver/spi_slave.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>

#define PIN_NUM_MOSI 23
#define PIN_NUM_MISO 19
#define PIN_NUM_CLK 18
#define PIN_NUM_CS 16

static const char *TAG = "spi_slave";

#define BUF_SIZE 64

void app_main(void)
{
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
        .post_setup_cb = NULL,
        .post_trans_cb = NULL};

    ret = spi_slave_initialize(HSPI_HOST, &buscfg, &slvcfg, SPI_DMA_DISABLED);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to initialize SPI slave: %s", esp_err_to_name(ret));
        return;
    }

    uint8_t recvbuf[BUF_SIZE] = {0};
    spi_slave_transaction_t trans = {
        .length = BUF_SIZE * 8,
        .rx_buffer = recvbuf};

    while (1)
    {
        memset(recvbuf, 0, BUF_SIZE); // Clear buffer before receiving data
        ret = spi_slave_transmit(HSPI_HOST, &trans, portMAX_DELAY);
        if (ret == ESP_OK)
        {
            ESP_LOGI(TAG, "Received data: %.*s", BUF_SIZE, recvbuf);
        }
        else
        {
            ESP_LOGE(TAG, "Failed to receive SPI data: %s", esp_err_to_name(ret));
        }
    }
}
