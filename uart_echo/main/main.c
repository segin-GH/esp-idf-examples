#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include <driver/uart.h>

#define uart_one_txd  4
#define uart_one_rxd  5
#define Buf_size 1024

#define uart_two_txd  17
#define uart_two_rxd  16


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

    uart_param_config(UART_NUM_1,&uart_one_config);
    uart_param_config(UART_NUM_2,&uart_two_config);

    uart_set_pin(UART_NUM_1,uart_one_txd,uart_one_rxd,UART_PIN_NO_CHANGE,UART_PIN_NO_CHANGE);
    uart_set_pin(UART_NUM_2,uart_two_txd,uart_two_rxd,UART_PIN_NO_CHANGE,UART_PIN_NO_CHANGE);

    uart_driver_install(UART_NUM_1,Buf_size * 2, 0, 0, NULL,0);
    uart_driver_install(UART_NUM_2,Buf_size * 2, 0, 0, NULL,0);

    char *data = "ping";
    char  recMesg[4];
    memset(recMesg,0,sizeof(recMesg));
    int count = 0;
    while(true)
    {   
        uart_write_bytes(UART_NUM_2,data,5);
        /* in uart read bytes try to give the exact number of data is going to be snd */
        uart_read_bytes(UART_NUM_1,recMesg, Buf_size,20/portTICK_RATE_MS);
        printf("recived mesg: %s %i\n",recMesg,count);
        count++;
        vTaskDelay(100/portTICK_PERIOD_MS);
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
        APP_CPU_NUM
    );

}
