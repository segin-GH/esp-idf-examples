#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "esp_log.h"
#include "esp_system.h"
#include "driver/twai.h"

#define TAG "twai"

// #define TX_PIN GPIO_NUM_5
// #define RX_PIN GPIO_NUM_4

#define TX_PIN GPIO_NUM_27
#define RX_PIN GPIO_NUM_14

void twai_receive_task(void *pvParameters)
{
    uint16_t casnode_list[4];
    for (;;)
    {
        // Wait for a message to be received
        twai_message_t message;
        if (twai_receive(&message, pdMS_TO_TICKS(1000)) != ESP_OK)
            continue;

        printf("[0x%02x][", message.identifier);
        int i = 0;
        for (i = 0; i < message.data_length_code; i++)
        {
            printf("0x%02x, ", message.data[i]);
            if (i == 7)
                printf("%d0x%02x", i, message.data[i]);
        }
        printf("]\n");
    }
}

void app_main()
{
    // Configure TWAI module
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(TX_PIN, RX_PIN, TWAI_MODE_NORMAL);
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_125KBITS();
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

    if (twai_driver_install(&g_config, &t_config, &f_config) == ESP_OK)
    {
        printf("Driver installed\n");
        printf("Driver BAUDRATE 125k\n");
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

    // Create task for receiving messages
    xTaskCreate(twai_receive_task, "twai_receive_task", 4096, NULL, 10, NULL);
}
