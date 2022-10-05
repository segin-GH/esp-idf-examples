#include <stdio.h>
#include <stdlib.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

static const int Queue_len = 5;
xQueueHandle queue;

void listenFromHTPPS(void *parms)
{
    int count 0;
    for(;;)
    {
        count++;
        printf("recived http message\n");
        xQueueSend(queue,&count,);
        vTaskDelay(5000/portTICK_PERIOD_MS);
    }
}

void task1(void *parms)
{
    for(;;)
    {
        printf("doing something with https\n");
    }
} 


void app_main(void)
{
    queue = xQueueCreate(Queue_len,sizeof(int));

    xTaskCreatePinnedToCore(
        listenFromHTPPS,
        "listenFromHTPPs",
        1024,
        NULL,
        1,
        NULL,
        APP_CPU_NUM
    );

    xTaskCreatePinnedToCore(
        task1,
        "task1",
        1024,
        NULL,
        1,
        NULL,
        APP_CPU_NUM
    );
}
