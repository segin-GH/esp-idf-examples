#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "can.h"

void send_task(void *pvParameter)
{
    while (1)
    {
        // Send message
        can_message_t message;
        message.identifier = 0x123;
        message.data_length_code = 8;
        message.data[0] = 0x11;
        message.data[1] = 0x22;
        message.data[2] = 0x33;
        message.data[3] = 0x44;
        message.data[4] = 0x55;
        message.data[5] = 0x66;
        message.data[6] = 0x77;
        message.data[7] = 0x88;
        can_transmit(&message, pdMS_TO_TICKS(1000));

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}


void app_main(void)
{
    can_init(4, 5, pdMS_TO_TICKS(1000));

    // Create task to send msg
    // xTaskCreatePinnedToCore(
    //     send_task,
    //     "send_task",
    //     2048,
    //     NULL,
    //     5,
    //     NULL,
    //     APP_CPU_NUM);

    // Create task to receive msg
    xTaskCreatePinnedToCore(
        receive_task,
        "receive_task",
        2048,
        NULL,
        5,
        NULL,
        APP_CPU_NUM);
}
