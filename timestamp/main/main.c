#include <stdio.h>
#include <string.h>
#include <esp_log.h>
#include <time.h>
#include <esp_log.h>
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void app_main()
{
    struct timeval tv_now;
    char str[100];
    
    while(true)
    {
        gettimeofday(&tv_now, NULL);
        // int64_t time_us = (int64_t)tv_now.tv_sec * 1000000L + (int64_t)tv_now.tv_usec;
        sprintf(str,"%ld:%ld",(tv_now.tv_usec), tv_now.tv_sec);
        printf("%s\n",str);
        // printf("%ld:%ld")
        // vTaskDelay(1000/portTICK_PERIOD_MS);
    }

}