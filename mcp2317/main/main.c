#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <mcp23x17.h>
#include <driver/gpio.h>
#include <string.h>

#define CONFIG_MCP23X17_IFACE_I2C


// static void IRAM_ATTR intr_handler(void *arg)
// {
//     printf("Interrupt!\n");
// }

void test(void *pvParameters)
{


    mcp23x17_t dev;
    memset(&dev, 0, sizeof(mcp23x17_t));

    mcp23x17_init_desc(&dev, 0x20, 0, 21, 22);

    // Setup PORTA0 as input
    // mcp23x17_set_mode(&dev, 0, MCP23X17_GPIO_INPUT);
    // Setup interrupt on it
    // mcp23x17_set_interrupt(&dev, 0, MCP23X17_INT_ANY_EDGE);

    // gpio_set_direction(0x01 , GPIO_MODE_INPUT);
    // gpio_set_intr_type(CONFIG_EXAMPLE_INTA_GPIO, GPIO_INTR_ANYEDGE);
    // gpio_install_isr_service(0);
    // gpio_isr_handler_add(CONFIG_EXAMPLE_INTA_GPIO, intr_handler, NULL);

    // Setup PORTB0 as output
    mcp23x17_set_mode(&dev, 8, MCP23X17_GPIO_OUTPUT);
    mcp23x17_set_mode(&dev, 9, MCP23X17_GPIO_OUTPUT);
    mcp23x17_set_mode(&dev, 10, MCP23X17_GPIO_OUTPUT);
    mcp23x17_set_mode(&dev, 11, MCP23X17_GPIO_OUTPUT);
    mcp23x17_set_mode(&dev, 12, MCP23X17_GPIO_OUTPUT);
    mcp23x17_set_mode(&dev, 13, MCP23X17_GPIO_OUTPUT);
    mcp23x17_set_mode(&dev, 14, MCP23X17_GPIO_OUTPUT);
    mcp23x17_set_mode(&dev, 15, MCP23X17_GPIO_OUTPUT);
    // do some blinking
    bool on = true;
    while (1)
    {
        mcp23x17_set_level(&dev, 8, on);
        mcp23x17_set_level(&dev, 9, on);
        mcp23x17_set_level(&dev, 10, on);
        mcp23x17_set_level(&dev, 11, on);
        mcp23x17_set_level(&dev, 12, on);
        mcp23x17_set_level(&dev, 13, on);
        mcp23x17_set_level(&dev, 14, on);
        mcp23x17_set_level(&dev, 15, on);
        on = !on;
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void app_main()
{
    ESP_ERROR_CHECK(i2cdev_init());
    xTaskCreate(test, "test", configMINIMAL_STACK_SIZE * 6, NULL, 5, NULL);
}
