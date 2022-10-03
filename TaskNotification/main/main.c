#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

static TaskHandle_t reciverHandler = NULL;

void sender(void *parms)
{
    while(true)
    {
        xTaskNotifyGive(reciverHandler);
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
}

void reciver(void *parms)
{
    while(true)
    {
        int count = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        printf("received notification %d times\n", count);
    }
}


void app_main(void)
{
    xTaskCreate(
        reciver,
        "reciver",
        2048,
        NULL,
        2,
        &reciverHandler
    );

    xTaskCreate(
        sender,
        "sender",
        2048,
        NULL,
        2,
        NULL
    );
}