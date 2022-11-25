#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <driver/gpio.h>

xQueueHandle interput_queue;
int intr_queue_len = 2;

#define interput_pin 26


static void IRAM_ATTR gpio_isr_handler(void *args)
{
    int intr_count = (int)args;
    xQueueSendFromISR(interput_queue, &intr_count++,NULL);
}


void when_gpio_is_low(void *args)
{   
    int state = 0;
    for(;;)
    {
        if(xQueueReceive(interput_queue, &state, portMAX_DELAY))
        {
            printf("gpio was low for %i \n",state);
        }
    }
}



void app_main(void)
{   int count = 0;
    gpio_set_direction(interput_pin, GPIO_MODE_INPUT);
    gpio_pullup_en(interput_pin);
    gpio_set_intr_type(interput_pin, GPIO_INTR_LOW_LEVEL);

    interput_queue = xQueueCreate(intr_queue_len,sizeof(int));

    gpio_install_isr_service(0);
    gpio_isr_handler_add(interput_pin, gpio_isr_handler, (void*)count);

    xTaskCreatePinnedToCore(
        when_gpio_is_low,
        "when_gpio_is_low",
        1024,
        NULL,
        2,
        NULL,
        APP_CPU_NUM
    );

}
