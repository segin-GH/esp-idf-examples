#include <stdio.h>
#include <string.h>
#include <esp_log.h>
#include <time.h>
#include <esp_log.h>
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void get_time_stamp(char *buffer)
{
    struct timeval tv_now;
    gettimeofday(&tv_now, NULL);
    sprintf(buffer,"%ld:%ld:",(tv_now.tv_usec), tv_now.tv_sec);
    
}

void app_main()
{
    char str[100];
    char dataBuf[] = {"data"};
    memset(str,0,sizeof(str));
    while(true)
    {
        get_time_stamp(str);
        strcat(str,dataBuf);
        printf("%s\n",str);
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
}
