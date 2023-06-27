#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

uint8_t LED_BUITIN = 13;

void app_main(void)
{
    gpio_pad_select_gpio(LED_BUITIN);
    gpio_set_direction(LED_BUITIN, GPIO_MODE_OUTPUT);

    while (true)
    {
        vTaskDelay(100 / portTICK_PERIOD_MS);
        gpio_set_level(LED_BUITIN, 1);
        printf("LedON\n");
        vTaskDelay(100 / portTICK_PERIOD_MS);
        gpio_set_level(LED_BUITIN, 0);
        printf("LedOFF\n");
    }
}
