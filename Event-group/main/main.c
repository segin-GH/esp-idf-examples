#include <stdio.h>
#include <stdlib.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>

EventGroupHandle_t  evtgrp;
const int gotHTTP = BIT0;
const int gotBLE = BIT1;

void listenToHTTPs (void *parms)
{
    for(;;)
    {
        printf("got HTTPS\n");
        vTaskDelay(2000/portTICK_PERIOD_MS);
    }
}

void listenToBluetooth (void *parms)
{
    for(;;)
    {
        xEventGroupSetBits(evtgrp,gotBLE);
        printf("got BLE\n");
        vTaskDelay(5000/portTICK_PERIOD_MS);
    }
}

void Task1 (void *parms)
{
    for(;;)
    {
        xEventGroupWaitBits(evtgrp, gotHTTP , false, true, portMAX_DELAY);
        printf("received HTTP and BLE\n");
    }
}

void app_main (void)
{
    evtgrp = xEventGroupCreate();

    xTaskCreatePinnedToCore(
        listenToHTTPs,
        "listenToHTTPs",
        2048,
        NULL,
        2,
        NULL,
        APP_CPU_NUM
    );

    xTaskCreatePinnedToCore(
        listenToBluetooth,
        "listenToBluetooth",
        2048,
        NULL,
        2,
        NULL,
        APP_CPU_NUM
    );

    xTaskCreatePinnedToCore(
        Task1,
        "task1",
        2048,
        NULL,
        2,
        NULL,
        APP_CPU_NUM
    );

    xEventGroupSetBits(evtgrp,gotHTTP);
}
