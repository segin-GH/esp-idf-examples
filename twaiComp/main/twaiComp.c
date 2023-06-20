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
        message.flags = CAN_MSG_FLAG_EXTD;
        can_transmit(&message, 1000 / portTICK_PERIOD_MS);

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void receive_task(void *pvParameter)
{
    while (1)
    {
        // Receive message
        can_message_t message;
        if (can_receive(&message, 1000 / portTICK_PERIOD_MS) == ESP_OK)
        {
            printf("Message received\n");
            printf("  ID: 0x%08x\n", message.identifier);
            printf("  DLC: %d\n", message.data_length_code);
            printf("  Data:");
            for (int i = 0; i < message.data_length_code; i++)
            {
                printf(" 0x%02x", message.data[i]);
            }
            printf("\n");
        }
    }
}

void app_main(void)
{
    can_init(4, 5);

    // Create task to send msg
    xTaskCreatePinnedToCore(
        send_task,
        "send_task",
        2048,
        NULL,
        5,
        NULL,
        APP_CPU_NUM);

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
