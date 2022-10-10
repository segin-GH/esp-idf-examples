#include <stdio.h>
#include <stdlib.h>
#include <driver/uart.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#define rxPin 5
#define txPin 4

#define rxBuffer 1024
#define txBuffer 1024
#define patternLen 3

QueueHandle_t uartQueue;

void uart_event_task(void* parms)
{
    whi
}

void app_main (void) 
{
    uart_config_t uartConfig = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE 
    };

    uart_param_config(UART_NUM_1,&uartConfig);
    uart_set_pin(UART_NUM_1, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE,UART_PIN_NO_CHANGE);
    uart_driver_install(UART_NUM_1, rxBuffer, txBuffer, 20, &uartQueue, 0);

    uart_enable_pattern_det_intr(UART_PORT_NUM_1,'+',patternLen,1000,10,10);
    uart_pattern_queue_reset(UART_NUM_1,20);
    xTaskCreate(
        //
    )




}