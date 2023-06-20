#include "can.h"

#define TAG "CAN"

static xQueueHandle can_tx_queue;
static uint8_t can_tx_queue_size = 10;
static uint8_t can_tx_queue_timeout = 0;

esp_event_loop_handle_t can_receive_event_loop;

/* use if you want to print received can msgs */
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

/* Task that sends the can msg */
static void send_can_message(void *pvParms)
{
    can_message_t can_tx_message;
    for (;;)
    {
        if (xQueueReceive(can_tx_queue, &can_tx_message, pdMS_TO_TICKS(can_tx_queue_timeout)))
        {
            if (twai_transmit(&can_tx_message, can_tx_queue_timeout) == ESP_FAIL)
                ESP_LOGE(TAG, "Failed to send CAN message");
        }
    }
}

/* Task that receives can msg */
static void can_receive_task(void *pvParms)
{
    for (;;)
    {
        can_message_t can_rx_message;
        if (twai_receive(&can_rx_message, pdMS_TO_TICKS(1000)) != ESP_OK)
            continue;

        /* Creates a event in the event handler */
        esp_event_post_to(
            can_receive_event_loop,
            CAN_EVENT,
            RECEIVED_CAN_MESSAGE,
            &can_rx_message,
            sizeof(can_rx_message),
            pdMS_TO_TICKS(1000));
    }
}

esp_err_t can_init(uint8_t rx_gpio_num, uint8_t tx_gpio_num, TickType_t ticks_to_wait_tx)
{
    can_tx_queue = xQueueCreate(can_tx_queue_size, sizeof(can_message_t));
    if (can_tx_queue == NULL)
        return ESP_FAIL;
    can_tx_queue_timeout = ticks_to_wait_tx;

    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(tx_gpio_num, rx_gpio_num, TWAI_MODE_NORMAL);
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_125KBITS();
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

    ESP_LOGI(TAG, "Configuring TWAI driver...");
    ESP_LOGI(TAG, "RX GPIO: %d", rx_gpio_num);
    ESP_LOGI(TAG, "TX GPIO: %d", tx_gpio_num);

    if (twai_driver_install(&g_config, &t_config, &f_config) == ESP_FAIL)
        return ESP_FAIL;

    if (twai_start() == ESP_FAIL)
        return ESP_FAIL;

    ESP_LOGI(TAG, "Driver installed");

    esp_event_loop_args_t loop_args = {
        .queue_size = 10,
        .task_name = "recive_can_message",
        .task_priority = uxTaskPriorityGet(NULL),
        .task_stack_size = 2048,
        .task_core_id = tskNO_AFFINITY};

    esp_event_loop_create(&loop_args, &can_receive_event_loop);

    /* TODO change name form send_can_messages to can_send_messages*/
    // Create a send task to send messages
    xTaskCreatePinnedToCore(
        send_can_message,
        "send_can_message",
        2048,
        NULL,
        5,
        NULL,
        APP_CPU_NUM);

    // Create a task to receive CAN  messages
    xTaskCreatePinnedToCore(
        can_receive_task,
        "can_receive_task",
        2048,
        NULL,
        5,
        NULL,
        APP_CPU_NUM);

    ESP_LOGI(TAG, "CAN initialized");
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

esp_err_t can_register_can_handler(esp_event_handler_t handler, void *handler_arg)
{
    return esp_event_handler_register_with(
        can_receive_event_loop,
        CAN_EVENT,
        ESP_EVENT_ANY_ID,
        handler,
        handler_arg);
}