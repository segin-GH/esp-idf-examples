#include "esp_event.h"
#include <stdio.h>

#define MY_EVENT_BASE (esp_event_base_t)0x1000
#define MY_EVENT_ID 1
#define MY_EVENT_BLE 2

sp_event_loop_handle_t loop_handle;

void listenToHTTPs(void *parms)
{
    for (;;)
    {
        printf("got HTTPS\n");
        esp_event_post_to(loop_handle, MY_EVENT_BASE, MY_EVENT_ID, NULL, 0, portMAX_DELAY);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}

void listenToBluetooth(void *parms)
{
    for (;;)
    {
        printf("got BLE\n");
        char data[] = "DATA 4 BLE";
        esp_event_post_to(loop_handle, MY_EVENT_BASE, MY_EVENT_BLE, data, 0, portMAX_DELAY);
        vTaskDelay(4000 / portTICK_PERIOD_MS);
    }
}

void run_on_event(void *handler_arg, esp_event_base_t base, int32_t id, void *event_data)
{
    switch (id)
    {
    case MY_EVENT_ID:
        printf("event triggered after got https cmd %d\n", id);
        printf("%s\n", (char *)event_data);
        break;

    case MY_EVENT_BLE:
        printf("event triggered after got BLE cmd %d\n", id);
        break;
    }
}

void app_main()
{
    // Create an event loop
    esp_event_loop_args_t loop_args = {
        .queue_size = 10,
        .task_name = "event_loop_task",
        .task_priority = uxTaskPriorityGet(NULL),
        .task_stack_size = 2048,
        .task_core_id = tskNO_AFFINITY,
    };

    esp_event_loop_create(&loop_args, &loop_handle);

    // Register an event handler
    esp_event_handler_register_with(loop_handle, MY_EVENT_BASE, ESP_EVENT_ANY_ID, run_on_event, NULL);

    // Unregister the event handler
    // esp_event_handler_unregister_with(loop_handle, MY_EVENT_BASE, ESP_EVENT_ANY_ID, run_on_event);

    xTaskCreatePinnedToCore(listenToHTTPs, "listenToHTTPs", 2048, NULL, 2, NULL, APP_CPU_NUM);
    xTaskCreatePinnedToCore(listenToBluetooth, "listenToBluetooth", 2048, NULL, 2, NULL, APP_CPU_NUM);
}
