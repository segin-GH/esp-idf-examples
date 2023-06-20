#include "can.h"

#define TAG "CAN"

static xQueueHandle can_tx_queue;
static uint8_t can_tx_queue_size = 10;
static uint8_t can_tx_queue_timeout = 0;

static void send_can_message(void *pvParms)
{
    can_message_t can_tx_message;
    for (;;)
    {
        if (xQueueReceive(can_tx_queue, &can_tx_message, pdMS_TO_TICKS(can_tx_queue_timeout)))
        {
            ESP_LOGV(TAG, "Sending CAN message");
            ESP_LOGV(TAG, "ID: %d", can_tx_message.identifier);

            if (twai_transmit(&can_tx_message, can_tx_queue_timeout) == ESP_FAIL)
                ESP_LOGE(TAG, "Failed to send CAN message");
        }
    }
}

esp_err_t can_init(uint8_t rx_gpio_num, uint8_t tx_gpio_num, TickType_t ticks_to_wait_tx)
{
    can_tx_queue = xQueueCreate(can_tx_queue_size, sizeof(can_message_t));
    if (can_tx_queue == NULL)
        return ESP_FAIL;
    can_tx_queue_timeout = ticks_to_wait_tx;

    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(rx_gpio_num, tx_gpio_num, TWAI_MODE_NORMAL);
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_125KBITS();
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

    ESP_LOGV(TAG, "Configuring TWAI driver...");
    ESP_LOGV(TAG, "RX GPIO: %d", rx_gpio_num);
    ESP_LOGV(TAG, "TX GPIO: %d", tx_gpio_num);

    if (twai_driver_install(&g_config, &t_config, &f_config) == ESP_FAIL)
        return ESP_FAIL;

    if (twai_start() == ESP_FAIL)
        return ESP_FAIL;

    ESP_LOGI(TAG, "Driver installed");

    // Create a send task to send messages
    xTaskCreatePinnedToCore(
        send_can_message,
        "send_can_message",
        2048,
        NULL,
        5,
        NULL,
        APP_CPU_NUM);

    return ESP_OK;
}

esp_err_t can_deinit()
{
    if (twai_stop() == ESP_FAIL)
        return ESP_FAIL;

    if (twai_driver_uninstall() == ESP_FAIL)
        return ESP_FAIL;

    return ESP_OK;
}

esp_err_t can_transmit(can_message_t *message, TickType_t ticks_to_wait_tx)
{
    if (xQueueSend(can_tx_queue, message, ticks_to_wait_tx) == pdTRUE)
        return ESP_OK;
    else
        return ESP_FAIL;
}