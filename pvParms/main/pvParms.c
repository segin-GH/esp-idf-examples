#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "stdbool.h"

typedef struct
{
    bool led_state;
} led_task_parms_t;

led_task_parms_t led_task_parms = {
        .led_state = true,
    };

void led_task(void *pvParameters)
{
    led_task_parms_t *parms = (led_task_parms_t *)pvParameters;
    gpio_pad_select_gpio(GPIO_NUM_2);
    gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);
    while (1)
    {
        // turn on led gpio 2
        parms->led_state = !parms->led_state;
        gpio_set_level(GPIO_NUM_2, parms->led_state);
        vTaskDelay(1000 / portTICK_PERIOD_MS);

        // turn off led gpio 2
        parms->led_state = !parms->led_state;
        gpio_set_level(GPIO_NUM_2, parms->led_state);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void print_task(void *pvParameters)
{
    led_task_parms_t *parms = (led_task_parms_t *)pvParameters;
    while (1)
    {
        printf("led state: %s\n", parms->led_state ? "on" : "off");
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

void app_main(void)
{
    // create a task for turn on led
    xTaskCreate(led_task, "led_task", 2048, (void *)&led_task_parms, 5, NULL);

    // task that prints the state of led
    xTaskCreate(print_task, "print_task", 2048, (void *)&led_task_parms, 5, NULL);
}
