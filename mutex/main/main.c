#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

xSemaphoreHandle mutexBus;

void writeToBus(char *data)
{
    printf(data);
}

void task1(void *parms)
{
    for (;;)
    {
        printf("readingTemp \n");

        if (xSemaphoreTake(mutexBus, 1000 / portTICK_PERIOD_MS))
        {
            writeToBus("temp is 125\n");
            xSemaphoreGive(mutexBus);
        }
        else
        {
            printf("write to bus timed out for temp \n");
        }

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void task2(void *parms)
{
    for (;;)
    {
        printf("readingHum\n");

        if (xSemaphoreTake(mutexBus, 1000 / portTICK_PERIOD_MS))
        {
            writeToBus("humidity is 30\n");
            xSemaphoreGive(mutexBus);
        }
        else
        {
            printf("write to bus timed out for humuidity \n");
        }

        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}
void app_main(void)
{
    mutexBus = xSemaphoreCreateMutex();

    xTaskCreatePinnedToCore(
        task1,
        "temp",
        2048,
        NULL,
        1,
        NULL,
        APP_CPU_NUM);

    xTaskCreatePinnedToCore(
        task2,
        "humi",
        2048,
        NULL,
        1,
        NULL,
        APP_CPU_NUM);
}
