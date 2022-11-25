#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>

#define TRIG_GPIO_PIN 26

void app_main(void)
{
    gpio_set_direction(TRIG_GPIO_PIN,GPIO_MODE_OUTPUT);
    gpio_pullup_en(TRIG_GPIO_PIN);
    int count = 0;
    for(;;)
    {
        vTaskDelay(1000/portTICK_PERIOD_MS);
        gpio_set_level(TRIG_GPIO_PIN, 0);
        printf("this many times %i\n",count);
        vTaskDelay(1000/portTICK_PERIOD_MS);
        gpio_set_level(TRIG_GPIO_PIN, 1);
        ++count;
    }
}
