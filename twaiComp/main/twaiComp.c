#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "can.h"

#define TAG "main"

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
        vTaskDelay(pdMS_TO_TICKS(2000));
        can_message_t can_tx_message;
        can_tx_message.identifier = 0x20;
        can_tx_message.data_length_code = 8;
        can_tx_message.extd = 1;
        can_tx_message.data[0] = 0x11;
        can_tx_message.data[1] = 0x22;
        can_tx_message.data[2] = 0x33;
        can_tx_message.data[3] = 0x44;
        can_tx_message.data[4] = 0x55;
        can_tx_message.data[5] = 0x66;
        can_tx_message.data[6] = 0x77;
        can_tx_message.data[7] = 0x88;

        if (can_transmit(&can_tx_message, portMAX_DELAY) == ESP_OK)
            ESP_LOGI(TAG, "Message queued for transmission");
        else
            ESP_LOGE(TAG, "Failed to queue message for transmission");
    }
}

void app_main(void)
{
    can_init(5, 4, pdMS_TO_TICKS(100));

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