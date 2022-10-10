#include <stdio.h>
#include <stdlib.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/uart.h>
#include <string.h>

#define txdPin  4
#define rxdPin  5
#define rxBuffer  1024

void app_main(void)
{
    uart_config_t uart_config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };

    uart_param_config(UART_NUM_1,&uart_config);
    uart_set_pin(UART_NUM_1,txdPin,rxdPin,UART_PIN_NO_CHANGE,UART_PIN_NO_CHANGE);
    uart_driver_install(UART_NUM_1,rxBuffer,0,0,NULL,0);

    char mesg[] = "ping";
    printf("sending: %s\n", mesg);
    uart_write_bytes(UART_NUM_1,mesg,sizeof(mesg));

    char incoming_message[rxBuffer];
    memset(incoming_message, 0, sizeof(incoming_message));
    uart_read_bytes(UART_NUM_1,(uint8_t *)incoming_message,rxBuffer,pdMS_TO_TICKS(500));
    printf("recived: %s\n",incoming_message);
}
