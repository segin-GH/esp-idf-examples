#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "can.h"

#define TAG "main"

void generate_can_message(uint16_t id, uint8_t *data, uint8_t length)
{
    can_message_t can_tx_message;
    can_tx_message.identifier = id;
    can_tx_message.data_length_code = length;
    can_tx_message.extd = 1;
    can_tx_message.data[0] = data[0];
    can_tx_message.data[1] = data[1];
    can_tx_message.data[2] = data[2];
    can_tx_message.data[3] = data[3];
    can_tx_message.data[4] = data[4];
    can_tx_message.data[5] = data[5];
    can_tx_message.data[6] = data[6];
    can_tx_message.data[7] = data[7];

    if (can_transmit(&can_tx_message, portMAX_DELAY) == ESP_OK)
    {
        printf("[0x%02x]", can_tx_message.identifier);
        print_can_msg_in_cool_8t(can_tx_message.data, 8);
    }
    else
        ESP_LOGE(TAG, "Failed to queue message for transmission");
}

static void can_handle(void *args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    can_message_t *can_rx_message = (can_message_t *)event_data;
    switch (event_id)
    {
    case RECEIVED_CAN_MESSAGE:
        printf("[0x%02x]", can_rx_message->identifier);
        print_can_msg_in_cool_8t(can_rx_message->data, 8);
        break;

    default:
        ESP_LOGW(TAG, "Unhandled CAN event");
        break;
    }
}

void send_can_message_in_main(void *pvParms)
{
    for (;;)
    {
        for (int i = 0; i < 1000; i++)
        {
            vTaskDelay(pdMS_TO_TICKS(30));
            uint8_t data[8] = {0x12, 0x00, 0xE0, 0x90, 0x43, 0x00, 0xBE, 0xA3};
            generate_can_message(0x10, data, 8);

            vTaskDelay(pdMS_TO_TICKS(30));
            uint8_t data_2[8] = {0x12, 0x00, 0xF0, 0x23, 0x44, 0x00, 0x58, 0xA3};
            generate_can_message(0x11, data_2, 8);

            // // generate_can_message(0x41, data, 8);
            // vTaskDelay(pdMS_TO_TICKS(30));
        }
    }
}

void app_main(void)
{
    can_init(4, 5, pdMS_TO_TICKS(100));

    can_register_can_handler(can_handle, NULL);

    /* Create a task to send can messages */
    xTaskCreatePinnedToCore(
        send_can_message_in_main,
        "send_can_message",
        2048,
        NULL,
        1,
        NULL,
        APP_CPU_NUM);
}