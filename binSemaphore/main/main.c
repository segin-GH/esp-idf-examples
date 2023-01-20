#include <stdio.h>
#include <stdlib.h>
#include <freertos/FreeRTOS.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

xSemaphoreHandle binsemaph;

void listenForHTTP()
{
    for(;;)
    {
        printf("recived https message\n");
        vTaskDelay(5000/portTICK_PERIOD_MS);
        printf(" processed \n");
    }
}

void task1(void *parms)
{
    for(;;)
    {
        xSemaphoreTake(binsemaph,portMAX_DELAY);
        printf("doing something with http");
    }
}

void app_main(void)
{

    binsemaph = xSemaphoreCreateBinary();

    xTaskCreatePinnedToCore(
       listenForHTTP,
       "listenForHTTP",
       1024,
       NULL,
       2,
       NULL,
       APP_CPU_NUM 
    );

    xTaskCreatePinnedToCore(
        task1,
        "task1",
        1024,
        NULL,
        2,
        NULL,
        APP_CPU_NUM
    );
    
    xSemaphoreGive(binsemaph);        
}
