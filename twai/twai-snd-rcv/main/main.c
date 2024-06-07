
#include "driver/twai.h"
#include "esp_log.h"
#include "esp_system.h"
#include "stdbool.h"
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>
#include <stdio.h>
#include <string.h>

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
    twai_reconfigure_alerts(TWAI_ALERT_BUS_RECOVERED, NULL);
    twai_initiate_recovery();
    ESP_LOGI("MAIN", "Initiated bus recovery now we wait");
}

void handle_bus_recovery()
{
    // reconfiguring alerts, because we didn't learn the first time
    twai_reconfigure_alerts(TWAI_ALERT_BUS_RECOVERED |
                                TWAI_ALERT_ABOVE_ERR_WARN |
                                TWAI_ALERT_ERR_PASS |
                                TWAI_ALERT_BUS_OFF |
                                TWAI_ALERT_BUS_ERROR |
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
    twai_reconfigure_alerts(TWAI_ALERT_BUS_RECOVERED |
                                TWAI_ALERT_ABOVE_ERR_WARN |
                                TWAI_ALERT_ERR_PASS |
                                TWAI_ALERT_BUS_OFF |
                                TWAI_ALERT_BUS_ERROR |
                                TWAI_ALERT_RX_DATA,
                            NULL);

    for (;;)
    {
        uint32_t alerts;
        // oh look, a blocking call, how quaint
        twai_read_alerts(&alerts, portMAX_DELAY);

        if (alerts & TWAI_ALERT_RX_DATA)
        {
            twai_message_t message;
            if (twai_receive(&message, pdMS_TO_TICKS(1000)) == ESP_OK)
            {
                // let's print our little message, so proud
                printf("[0x%02x]", message.identifier);
                print_can_msg_in_cool_8t(message.data, 8);
            }
        }

        // Let's not clog the log with your existential CAN bus crisis
        if (alerts & TWAI_ALERT_ABOVE_ERR_WARN)
        {
            ESP_LOGW("MAIN", "Error Warning Limit reached, watch out!");
        }
        if (alerts & TWAI_ALERT_ERR_PASS)
        {
            ESP_LOGW("MAIN", "In Error Passive State, get your act together!");
        }
        if (alerts & TWAI_ALERT_BUS_ERROR)
        {
            ESP_LOGW("MAIN", "Bus Error, the bus is on fire!");
        }
        if (alerts & TWAI_ALERT_BUS_OFF)
        {
            ESP_LOGW("MAIN", "Bus Off, the apocalypse is upon us");
            handle_bus_off();
            ESP_LOGW("MAIN", "Attempting bus recovery, hold your breath");
        }
        if (alerts & TWAI_ALERT_BUS_RECOVERED)
        {
            ESP_LOGI("MAIN", "Bus Recovered, the sun shines upon us once more");
            handle_bus_recovery();
        }
    }
}

void app_main()
{

    // turn on gpio 27
    gpio_pad_select_gpio(GPIO_NUM_27);
    gpio_set_direction(GPIO_NUM_27, GPIO_MODE_OUTPUT);
    gpio_set_level(GPIO_NUM_27, 1);

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
        vTaskDelay(pdMS_TO_TICKS(1000));
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
        {
            printf("Message sent: ");
            printf("[0x%02x]", message.identifier);
            print_can_msg_in_cool_8t(message.data, 8);
        }
        else
            printf("Failed to queue message for transmission\n");

        // random delay between 1 and 3 seconds
    }

    ESP_ERROR_CHECK(twai_stop());
    ESP_ERROR_CHECK(twai_driver_uninstall());

    ESP_LOGI(TAG, "Done.");
}
