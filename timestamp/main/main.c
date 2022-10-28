#include <stdio.h>
#include <string.h>
#include <esp_log.h>
#include <time.h>
#include <esp_log.h>
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

struct timeval tv;

void get_time_stamp(char *buffer, struct timeval *tv_now)
{
    gettimeofday(tv_now, NULL); //take that pointer and dirctly give it to the func
    sprintf(buffer,"%ld:",(unsigned long )time(NULL));
}

void app_main()
{
    char str[100];
    char dataBuf[] = {"data"};
    memset(str,0,sizeof(str));

    tv.tv_sec = 1666945715;
    tv.tv_usec = 0;
    int err = settimeofday(&tv,NULL);
    if(err != 0)
    esp_restart();

    while(true)
    {
        get_time_stamp(str,&tv); // pass the pointer of struct tv
        strcat(str,dataBuf);
        printf("%s\n",str);
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
}
