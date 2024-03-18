
#include <stdio.h>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include "esp_log.h"
#include "esp_system.h"
#include "driver/twai.h"
#include "stdbool.h"

#define TAG "twai"

#define TX_PIN GPIO_NUM_5
#define RX_PIN GPIO_NUM_4

void print_can_msg_in_cool_8t(uint8_t array[], int num_of_element)
{
    int i = 0;
    printf("[");
    for (i = 0; i < num_of_element; i++)
    {
        if (i == (num_of_element - 1))
        {
            printf("0x%02x", array[i]);
            break;
        }
        printf("0x%02x, ", array[i]);
    }
    printf("]\n");
}
void handle_bus_off()
{
    // bus-off recovery sequence, because we care
    twai_reconfigure_alerts(TWAI_ALERT_BUS_RECOVERED, NULL);
    for (int i = 3; i > 0; i--)
    {
        // countdown to recovery, like it's new year's eve
        ESP_LOGW("MAIN", "Initiate bus recovery in %d", i);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    twai_initiate_recovery(); // fingers crossed
    ESP_LOGI("MAIN", "Initiate bus recovery");
}

void handle_bus_recovery()
{
    // reconfiguring alerts, because we didn't learn the first time
    twai_reconfigure_alerts(TWAI_ALERT_BUS_RECOVERED |
                                TWAI_ALERT_ABOVE_ERR_WARN |
                                TWAI_ALERT_ERR_PASS |
                                TWAI_ALERT_BUS_OFF |
                                TWAI_ALERT_RX_DATA,
                            NULL);
    ESP_LOGI("MAIN", "Bus Recovered");
    if (twai_start() == ESP_OK)
    {
        // the driver started, let's throw a party
        printf("Driver started\n");
    }
}

void twai_receive_task(void *pvParameters)
{
    // let's set up our circus of alerts once, shall we?
    twai_reconfigure_alerts(TWAI_ALERT_BUS_RECOVERED |
                                TWAI_ALERT_ABOVE_ERR_WARN |
                                TWAI_ALERT_ERR_PASS |
                                TWAI_ALERT_BUS_OFF |
                                TWAI_ALERT_RX_DATA,
                            NULL);

    for (;;)
    {
        uint32_t alerts;
        // oh look, a blocking call, how quaint
        twai_read_alerts(&alerts, portMAX_DELAY);

        switch (alerts)
        {
        case TWAI_ALERT_ABOVE_ERR_WARN:
            // someone alert the press, we surpassed an error warning limit
            ESP_LOGI("MAIN", "Surpassed Error Warning Limit");
            break;
        case TWAI_ALERT_ERR_PASS:
            // welcome to the passive-aggressive club, error style
            ESP_LOGI("MAIN", "Entered Error Passive state");
            break;
        case TWAI_ALERT_BUS_OFF:
            // oh no, the bus left without us
            handle_bus_off();
            break;
        case TWAI_ALERT_BUS_RECOVERED:
            // bus recovery, because we're all about second chances
            handle_bus_recovery();
            break;
        }

        // attempting to read a message, because why not?
        twai_message_t message;
        if (twai_receive(&message, pdMS_TO_TICKS(1000)) == ESP_OK)
        {
            // let's print our little message, so proud
            printf("[0x%02x]", message.identifier);
            print_can_msg_in_cool_8t(message.data, 8); // assuming this is a thing
        }
    }
}

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

    xTaskCreatePinnedToCore(
        twai_receive_task,
        "twai_receive_task",
        4096,
        NULL,
        10,
        NULL,
        APP_CPU_NUM);

    for (;;)
    {
        int delay = (rand() % 3) + 1;
        vTaskDelay(pdMS_TO_TICKS(delay * 1000));

        // Prepare and send message
        twai_message_t message;
        message.identifier = 0x10;
        message.extd = 1;
        message.data_length_code = 8;

        message.data[0] = 0xff;
        message.data[1] = 0xff;
        message.data[2] = 0xff;
        message.data[3] = 0xff;
        message.data[4] = 0xff;
        message.data[5] = 0xff;
        message.data[6] = 0xff;
        message.data[7] = 0xff;

        ESP_LOGI(TAG, "Sending message...");
        if (twai_transmit(&message, pdMS_TO_TICKS(1000)) == ESP_OK)
            printf("Message queued for transmission\n");
        else
            printf("Failed to queue message for transmission\n");

        // random delay between 1 and 3 seconds
    }

    ESP_ERROR_CHECK(twai_stop());
    ESP_ERROR_CHECK(twai_driver_uninstall());

    ESP_LOGI(TAG, "Done.");
}
