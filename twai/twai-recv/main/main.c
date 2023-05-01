#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "esp_log.h"
#include "esp_system.h"
#include "driver/twai.h"

#define TAG "twai"

#define TX_PIN GPIO_NUM_27
#define RX_PIN GPIO_NUM_14

void twai_receive_task(void *pvParameters)
{
    while (1)
    {
        // Wait for a message to be received
        twai_message_t message;
        if (twai_receive(&message, pdMS_TO_TICKS(1000)) == ESP_OK)
        {
            ESP_LOGI(TAG, "Received message: ID=0x%08x, DLC=%d, data=0x%02x%02x%02x%02x%02x%02x%02x%02x",
                     message.identifier, message.data_length_code,
                     message.data[0], message.data[1], message.data[2], message.data[3],
                     message.data[4], message.data[5], message.data[6], message.data[7]);
        }
    }
}

void app_main()
{
    // Configure TWAI module
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(TX_PIN, RX_PIN, TWAI_MODE_NORMAL);
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_125KBITS();
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

    ESP_ERROR_CHECK(twai_driver_install(&g_config, &t_config, &f_config));
    ESP_ERROR_CHECK(twai_start());

    // Create task for receiving messages
    xTaskCreate(twai_receive_task, "twai_receive_task", 4096, NULL, 10, NULL);
}
