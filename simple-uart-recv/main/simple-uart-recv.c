#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "esp_log.h"

#define UART2_NUM UART_NUM_2
#define BUF_SIZE 1024

// Define your GPIO pins here
#define UART2_TX_PIN GPIO_NUM_17
#define UART2_RX_PIN GPIO_NUM_16

void app_main()
{
    // Configure parameters for UART2
    uart_config_t uart2_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };

    // Install UART driver
    uart_driver_install(UART2_NUM, BUF_SIZE * 2, 0, 0, NULL, 0);

    // Configure UART parameters
    uart_param_config(UART2_NUM, &uart2_config);

    // Set UART pins
    uart_set_pin(UART2_NUM, 18, 16, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    // Allocate buffer for UART
    uint8_t *data = (uint8_t *)malloc(BUF_SIZE);

    while (1)
    {
        // Clear the buffer
        memset(data, 0, BUF_SIZE);

        // Read data from UART2
        uart_write_bytes(UART2_NUM, "Echo from esp32\n", 17);
        int len = uart_read_bytes(UART2_NUM, data, BUF_SIZE, 20 / portTICK_RATE_MS);
        if (len > 0)
        {
            // Log the received data
            ESP_LOGI("UART", "Received %d bytes: '%s'", len, data);
            // send some data back
            memset(data, 0, BUF_SIZE);
        }
    }

    // Clean up and delete UART driver
    uart_driver_delete(UART2_NUM);
    free(data);
}
