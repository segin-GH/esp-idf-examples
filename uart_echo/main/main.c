#include <driver/uart.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define uart_one_txd 4
#define uart_one_rxd 22
#define Buf_size 1024

#define uart_two_txd 21
#define uart_two_rxd 5

void exChangeUartData(void *parms)
{
    uart_config_t uart_one_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };

    uart_config_t uart_two_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };

    uart_param_config(UART_NUM_1, &uart_one_config);
    uart_param_config(UART_NUM_2, &uart_two_config);

    uart_set_pin(UART_NUM_1, uart_one_txd, uart_one_rxd, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_set_pin(UART_NUM_2, uart_two_txd, uart_two_rxd, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    uart_driver_install(UART_NUM_1, Buf_size * 2, 0, 0, NULL, 0);
    uart_driver_install(UART_NUM_2, Buf_size * 2, 0, 0, NULL, 0);

    char *data = "ping\n";
    char recMesg[4];
    memset(recMesg, 0, sizeof(recMesg));
    int count = 0;

    while (true)
    {
        uart_write_bytes(UART_NUM_2, data, strlen(data));
        uart_flush(UART_NUM_1);

        vTaskDelay(1000 / portTICK_PERIOD_MS);
        /* count = uart_read_bytes(UART_NUM_1, (uint8_t *)recMesg, 4, portMAX_DELAY);
        if (count > 0)
        {
            ESP_LOGI("uart", "Received data: %s", recMesg);
        } */
    }
}

void app_main(void)
{
    xTaskCreatePinnedToCore(
        exChangeUartData,
        "snd&recvdata",
        2048,
        NULL,
        2,
        NULL,
        APP_CPU_NUM);
}
