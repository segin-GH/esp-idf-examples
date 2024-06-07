#include "driver/uart.h"
#include "esp_log.h"
#include "string.h"
#include <stdio.h>

#define TXD_PIN 17
#define RXD_PIN 16

#define RX_BUF_SIZE 1024

void app_main(void)
{
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE};

    uart_param_config(UART_NUM_2, &uart_config);
    uart_set_pin(UART_NUM_2, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(UART_NUM_2, RX_BUF_SIZE, 0, 0, NULL, 0);

    char recBuffer[1024];
    memset(recBuffer, 0, sizeof(recBuffer));

    for (;;)
    {
        if (uart_read_bytes(UART_NUM_2, recBuffer, 1024, pdMS_TO_TICKS(50)) < 1)
            continue;
        printf("%s\n", recBuffer);
        memset(recBuffer, 0, sizeof(recBuffer));
        uart_flush(UART_NUM_2);
    }
}
