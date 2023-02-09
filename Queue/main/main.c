#include <stdio.h>
#include <stdlib.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <freertos/queue.h>

static const int Queue_len = 5;
xQueueHandle queue;

void listenFromHTPPS(void *parms)
{
    int count = 0;

    for(;;)
    {
        count++;
        printf("recived http message\n");
        long ok = xQueueSend(queue, &count, 5000/portTICK_PERIOD_MS);
        if(ok)
        {
            printf("added message to queue\n");
        }
        else
        {
            printf("unable add to queue\n");
        }
        vTaskDelay(10000/portTICK_PERIOD_MS);
    }
}

void task1(void *parms)
{
    int rxint;
    for(;;)
    {
        if(xQueueReceive(queue, &rxint, 1000/portTICK_PERIOD_MS))
        {
            printf("%d queue received \n",rxint);
        }
        else
        {
            printf("Did not get queue in 1sec \n");
        }
    }
} 


void app_main(void)
{
    queue = xQueueCreate(Queue_len,sizeof(int));

    xTaskCreatePinnedToCore(
        listenFromHTPPS,
        "listenFromHTPPs",
        2048,
        NULL,
        1,
        NULL,
        APP_CPU_NUM
    );

    xTaskCreatePinnedToCore(
        task1,
        "task1",
        2048,
        NULL,
        1,
        NULL,
        APP_CPU_NUM
    );
}
