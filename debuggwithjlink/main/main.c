
#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

uint8_t LED_BUITIN = 2;

void app_main(void)
{
    gpio_pad_select_gpio(LED_BUITIN);
    gpio_set_direction(LED_BUITIN,GPIO_MODE_OUTPUT);

int i = 0;
    while(true)
    {
        i++; 
        gpio_set_level(LED_BUITIN,1);
        printf("led on%i\n",i);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        gpio_set_level(LED_BUITIN,0);
        printf("led of%i\n",i);
        vTaskDelay(1000 / portTICK_PERIOD_MS);        
    }
}