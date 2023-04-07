#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static TaskHandle_t receiverHandler = NULL;

// Function that represents a sender task
void senderTask(void *pvParameter)
{
    while (1)
    {
        printf("snd task\n");
        vTaskDelay(pdMS_TO_TICKS(5000));
        
    }
}

// Function that represents a receiver task
void receiverTask(void *pvParameter)
{
    while (1)
    {
        printf("rcv task\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void app_main()
{
    // Create the sender task
    xTaskCreate(senderTask, "senderTask", 4096, NULL, 1, NULL);

    // Create the receiver task and get a handle to it
    xTaskCreate(receiverTask, "receiverTask", 4096, NULL, 1, &receiverHandler);

    // Suspend the receiver task
    vTaskSuspend(receiverHandler);

    // Resume the receiver task after 5 seconds
    vTaskDelay(pdMS_TO_TICKS(5000));
    vTaskResume(receiverHandler);

    // Continue executing other code or tasks
    while (1)
    {
        printf("main task\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
