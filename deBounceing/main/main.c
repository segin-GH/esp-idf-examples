#include <stdio.h>
#include <stdlib.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <driver/gpio.h>

int ledPin = 2;
int switchPin = 15;

xQueueHandle interputQueue;
int intrQueuLen = 10;

static void IRAM_ATTR gpio_isr_handler(void *arg)
{
    int pinNumber = (int)arg;
    xQueueSendFromISR(
        interputQueue,
        &pinNumber,
        NULL
    );
}

void when_switch_is_pushed(void *args)
{
    int pinNumber, count = 0;
    for(;;)
    {
        if(xQueueReceive(interputQueue,&pinNumber,portMAX_DELAY))
        {
            gpio_isr_handler_remove(pinNumber);
            do 
            {
                vTaskDelay(20/portTICK_PERIOD_MS);
            }
            while(gpio_get_level(pinNumber) == 1);

            printf(" gpio %d was pressed %d times, the state is %d \n",pinNumber,count++,
            gpio_get_level(switchPin));
            gpio_isr_handler_add(switchPin,gpio_isr_handler,(void *)switchPin);
        }
    }
}

void app_main(void)
{
    // gpio_set_direction(switchPin,GPIO_MODE_INPUT);
    // gpio_set_direction(ledPin,GPIO_MODE_INPUT);
    
    // gpio_pulldown_en(switchPin);
    // gpio_set_intr_type(switchPin,GPIO_INTR_POSEDGE);

    gpio_config_t config;
        config.intr_type = GPIO_INTR_POSEDGE;
        config.mode = GPIO_MODE_INPUT;
        config.pull_down_en = 1;
        config.pull_down_en = 0;
        config.pin_bit_mask = (1ULL << switchPin);
    gpio_config(&config);


    interputQueue = xQueueCreate(intrQueuLen,sizeof(int));
    
    xTaskCreatePinnedToCore(
        when_switch_is_pushed,
        "buttonPressed",
        2048,
        NULL,
        2,
        NULL,
        APP_CPU_NUM
    );
    
    gpio_install_isr_service(0);
    gpio_isr_handler_add(switchPin,gpio_isr_handler,(void *)switchPin);
}