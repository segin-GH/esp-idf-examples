#include <stdio.h>
#include <esp_log.h>
#include <time.h>
#include <esp_log.h>
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void app_main()
{
    struct timeval tv_now;
    
    while(true)
    {
        gettimeofday(&tv_now, NULL);
        int64_t time_us = (int64_t)tv_now.tv_sec * 1000000L + (int64_t)tv_now.tv_usec;
        printf("seconds : %ld  micro seconds : %ld\n",tv_now.tv_sec, tv_now.tv_usec);
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }

}