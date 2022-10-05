#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/timers.h>
#include <esp_system.h>

TimerHandle_t xTimer;

void on_timer(TimerHandle_t xTimer)
{
    printf("timer hit %lld\n",esp_timer_get_time() /1000);
}

void app_main(void)
{
    printf("timer : %lld\n", esp_timer_get_time()/1000);

    xTimer = xTimerCreate(
                "myTimer",
                pdMS_TO_TICKS(1000),
                true,
                NULL,
                on_timer
            );

    xTimerStart(xTimer,0);
}
