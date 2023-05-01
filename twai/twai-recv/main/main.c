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
    for (;;)
    {
        // Wait for a message to be received
        twai_message_t message;
        if (twai_receive(&message, pdMS_TO_TICKS(1000)) != ESP_OK)
            continue;

        printf("ID = 0x%02x\n", message.identifier);

        for (int i = 0; i < message.data_length_code; i++)
            printf("Data byte %d = 0x%02x\n", i, message.data[i]);
        
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
