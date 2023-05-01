#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "esp_log.h"
#include "esp_system.h"
#include "driver/twai.h"

#define TAG "twai"

#define TX_PIN GPIO_NUM_5
#define RX_PIN GPIO_NUM_4

void app_main()
{
    // Configure TWAI module
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(TX_PIN, RX_PIN, TWAI_MODE_NORMAL);
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_125KBITS();
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

    // Install TWAI driver
    if (twai_driver_install(&g_config, &t_config, &f_config) == ESP_OK)
    {
        printf("Driver installed\n");
    }
    else
    {
        printf("Failed to install driver\n");
        return;
    }
    if (twai_start() == ESP_OK)
    {
        printf("Driver started\n");
    }
    else
    {
        printf("Failed to start driver\n");
        return;
    }
    for (;;)
    {

        // Prepare and send message
        twai_message_t message;
        message.identifier = 0x05;
        message.extd = 1;
        message.data_length_code = 8;

        message.data[0] = 0x11;
        message.data[1] = 0x22;
        message.data[2] = 0x33;
        message.data[3] = 0x44;
        message.data[4] = 0x55;
        message.data[5] = 0x66;
        message.data[6] = 0x77;
        message.data[7] = 0x88;

        ESP_LOGI(TAG, "Sending message...");
        if (twai_transmit(&message, pdMS_TO_TICKS(1000)) == ESP_OK)
            printf("Message queued for transmission\n");
        else
            printf("Failed to queue message for transmission\n");
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }

    ESP_ERROR_CHECK(twai_stop());
    ESP_ERROR_CHECK(twai_driver_uninstall());

    ESP_LOGI(TAG, "Done.");
}