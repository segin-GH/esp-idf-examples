#include <stdio.h>
#include <stdlib.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

static TaskHandle_t waker = NULL;

void WakerFunction(void *parms)
{
    for(;;)
    {
        xTaskNotifyGive(waker);
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
}

void functionWhichIsSleep(void *parms)
{
    for(;;)
    {
        ulTaskNotifyTake(pdFALSE,portMAX_DELAY);
        printf("Function is awake.\n");
    }
}

void app_main(void)
{
    xTaskCreatePinnedToCore(
        functionWhichIsSleep,
        "functionWhichIssleep",
        1024,
        NULL,
        1,
        &waker,
        APP_CPU_NUM
    );

    xTaskCreatePinnedToCore(
        WakerFunction,
        "wakerFunction",
        1024,
        NULL,
        1,
        NULL,
        APP_CPU_NUM
    );
}
