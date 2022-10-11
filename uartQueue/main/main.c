#include <stdio.h>
#include <stdlib.h>
#include <driver/uart.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>

#define TAG "UART"

#define rxPin 5
#define txPin 4

#define rxBuffer 1024
#define txBuffer 1024
#define patternLen 3

QueueHandle_t uartQueue;

void uart_event_task(void* parms)
{
    uart_event_t uartEvent;
    uint8_t *recivedBuffer = malloc(rxBuffer);
    size_t dataLen;
    for(;;)
    {
        if(xQueueReceive(uartQueue,&uartEvent,portMAX_DELAY))
        {
            switch(uartEvent.type)
            {
               case UART_DATA:
                    ESP_LOGI(TAG,"UART_DATA");              /*!< UART data event*/
                    uart_read_bytes(UART_NUM_1,recivedBuffer,uartEvent.size,portMAX_DELAY);
                    printf("rec: %.*s\n",uartEvent.size,recivedBuffer);
                    break;
               case UART_BREAK:
                    ESP_LOGI(TAG,"UART_BREAK");
                    break;             /*!< UART break event*/
               case UART_BUFFER_FULL:
                    ESP_LOGI(TAG,"UART_BUFFER_FULL");
                    break;       /*!< UART RX buffer full event*/
               case UART_FIFO_OVF:
                    ESP_LOGI(TAG,"UART_FIFO_OVF");
                    break;          /*!< UART FIFO overflow event*/
               case UART_FRAME_ERR:
                    ESP_LOGI(TAG,"UART_FRAME_ERR");
                    break;         /*!< UART RX frame error event*/
               case UART_PARITY_ERR:
                    ESP_LOGI(TAG,"UART_PARITY_ERR");
                    break;        /*!< UART RX parity event*/
               case UART_DATA_BREAK:
                    ESP_LOGI(TAG,"UART_DATA_BREAK");
                    break;        /*!< UART TX data and break event*/
               case UART_PATTERN_DET:
                    ESP_LOGI(TAG,"UART_PATTERN_DET");
                    uart_get_buffered_data_len(UART_NUM_1,&dataLen);
                    int pos = uart_pattern_pop_pos(UART_NUM_1);
                    ESP_LOGI(TAG,"Detected %d pos %d", dataLen, pos);
                    break;       /*!< UART pattern detected */
               case UART_EVENT_MAX:
                    ESP_LOGI(TAG,"UART_EVENT_MAX");
                    break;         /*!< UART event max index*/
            }
        }
    }
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
    uart_set_pin(UART_NUM_1, txPin, rxPin, UART_PIN_NO_CHANGE,UART_PIN_NO_CHANGE);
    uart_driver_install(UART_NUM_1, rxBuffer, txBuffer, 20, &uartQueue, 0);

    uart_enable_pattern_det_intr(UART_NUM_1,'+',patternLen,10000,10,10);
    uart_pattern_queue_reset(UART_NUM_1,20);
    
    xTaskCreate(
        uart_event_task,
        "uartTaskEvent",
        2048,
        NULL,
        1,
        NULL
    );




}