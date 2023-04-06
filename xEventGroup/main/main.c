#include <stdio.h>
#include <stdlib.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>

EventGroupHandle_t eventGrp;
int BIT = 0b0001;

void listenToHTTPs(void *parms)
{
    for (;;)
    {
        xEventGroupSetBits(eventGrp, 0b0001);
        printf("set HTTPS BIT\n");
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}

void listenToBluetooth(void *parms)
{
    for (;;)
    {
        xEventGroupSetBits(eventGrp, 0b0010);
        printf("set BLE BIT\n");
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}

void Task1(void *parms)
{
    for (;;)
    {
        int rtn = xEventGroupWaitBits(eventGrp, 0b0010, false, false, portMAX_DELAY);
        // rtn = rtn & 0b0001;
        // rtn = 1 << 2;
        if (rtn == 0b0010)
        {
            printf("Return of EventGroup %i.\n", rtn);
            vTaskDelay(pdMS_TO_TICKS(100));
            // printf("received HTTP and BLE\n");
        }
    }
}

void app_main(void)
{
    eventGrp = xEventGroupCreate();

    xEventGroupSetBits(eventGrp, 0b0010);
    // xTaskCreatePinnedToCore(
    //     listenToHTTPs,
    //     "listenToHTTPs",
    //     2048,
    //     NULL,
    //     2,
    //     NULL,
    //     APP_CPU_NUM
    // );

    // xTaskCreatePinnedToCore(
    //     listenToBluetooth,
    //     "listenToBluetooth",
    //     2048,
    //     NULL,
    //     2,
    //     NULL,
    //     APP_CPU_NUM
    // );

    xTaskCreatePinnedToCore(
        Task1,
        "task1",
        2048,
        NULL,
        2,
        NULL,
        APP_CPU_NUM);

    for (;;)
    {
        vTaskDelay(pdMS_TO_TICKS(5000));
        xEventGroupClearBits(eventGrp, 0b0010);

        vTaskDelay(pdMS_TO_TICKS(500));
        xEventGroupSetBits(eventGrp, 0b0010);
    }
}
