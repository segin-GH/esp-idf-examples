#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"

#define TIMEOUT_MS 1000

uint8_t LED_BUITIN = 2;

void app_main(void)
{
    TickType_t prevTime = xTaskGetTickCount();
    gpio_set_direction(LED_BUITIN, GPIO_MODE_OUTPUT);
    bool val = true;
    while (true)
    {
        uint32_t elapsed_time = xTaskGetTickCount() - prevTime;
        if (elapsed_time >= pdMS_TO_TICKS(TIMEOUT_MS))
        {
            val = !val;
            prevTime = xTaskGetTickCount();
        }
        gpio_set_level(LED_BUITIN, val);
        vTaskDelay(1); // not to trig wdt
    }
}