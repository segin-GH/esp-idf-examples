#include <stdio.h>
#include <esp_timer.h>

esp_timer_handle_t espTimmerHandle;

void timer_callback(void *args)
{
    printf("this if fired after 20us");
}

void app_main(void)
{
    const esp_timer_create_args_t esp_timer_create_args = {
        .callback = timer_callback
        .name = "My timmer"

    };
    esp_timer_create(esp_timer_create_args,&espTimmerHandle);
    esp_timer_start_once(espTimmerHandle,20);
    
}
