#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>

    int switchPin = 15;
    int ledPin = 2;

void app_main(void)
{
    gpio_pad_select_gpio(switchPin);
    gpio_pad_select_gpio(ledPin);

    gpio_set_direction(ledPin,GPIO_MODE_OUTPUT);
    gpio_set_direction(switchPin,GPIO_MODE_INPUT);

    gpio_pulldown_en(switchPin);

    while(true)
    {
        int level = gpio_get_level(switchPin);
        gpio_set_level(ledPin,level);
        vTaskDelay(1);
    }

}
