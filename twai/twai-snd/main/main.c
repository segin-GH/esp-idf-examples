#include "esp_log.h"
#include "esp_system.h"
#include "driver/twai.h"

#define TAG "twai"

#define TX_PIN GPIO_NUM_23
#define RX_PIN GPIO_NUM_22

void app_main()
{
    // Configure TWAI module
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(TX_PIN, RX_PIN, TWAI_MODE_NORMAL);
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_DEFAULT(1000, 125, 125, 50, 5, 2000);
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

    ESP_ERROR_CHECK(twai_driver_install(&g_config, &t_config, &f_config));
    ESP_ERROR_CHECK(twai_start());

    // Prepare and send message
    twai_message_t message = TWAI_MESSAGE_DEFAULT(TWAI_MSG_FLAG_EXTD, 0x12345678, 8);
    message.data[0] = 0x11;
    message.data[1] = 0x22;
    message.data[2] = 0x33;
    message.data[3] = 0x44;
    message.data[4] = 0x55;
    message.data[5] = 0x66;
    message.data[6] = 0x77;
    message.data[7] = 0x88;

    ESP_LOGI(TAG, "Sending message...");
    ESP_ERROR_CHECK(twai_transmit(&message, pdMS_TO_TICKS(1000)));

    // Wait for transmission to complete
    ESP_LOGI(TAG, "Waiting for transmission to complete...");
    twai_status_info_t status;
    do
    {
        ESP_ERROR_CHECK(twai_get_status_info(&status));
    } while (status.state != TWAI_STATE_READY);

    // Stop TWAI module and uninstall driver
    ESP_ERROR_CHECK(twai_stop());
    ESP_ERROR_CHECK(twai_driver_uninstall());

    ESP_LOGI(TAG, "Done.");
}