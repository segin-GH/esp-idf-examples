#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <mcp23x17.h>
#include <driver/gpio.h>
#include <string.h>


void test(void *pvParameters)
{
    mcp23x17_t dev;
    memset(&dev, 0, sizeof(mcp23x17_t));

    ESP_ERROR_CHECK(mcp23x17_init_desc(&dev, 0x20, 0, 27, 26));

    // // Setup PORTA0 as input
    mcp23x17_set_mode(&dev, 0, MCP23X17_GPIO_INPUT);
    uint32_t val = 0;

    // mcp23x17_set_mode(&dev, 1, MCP23X17_GPIO_INPUT);
    mcp23x17_set_mode(&dev, 8, MCP23X17_GPIO_OUTPUT);
    mcp23x17_set_mode(&dev, 9, MCP23X17_GPIO_OUTPUT);
    mcp23x17_set_mode(&dev, 10, MCP23X17_GPIO_OUTPUT);
    mcp23x17_set_mode(&dev, 11, MCP23X17_GPIO_OUTPUT);
    mcp23x17_set_mode(&dev, 12, MCP23X17_GPIO_OUTPUT);
    mcp23x17_set_mode(&dev, 13, MCP23X17_GPIO_OUTPUT);
    mcp23x17_set_mode(&dev, 14, MCP23X17_GPIO_OUTPUT);
    mcp23x17_set_mode(&dev, 15, MCP23X17_GPIO_OUTPUT);
    mcp23x17_set_pullup(&dev, 0, 0);
    // do some blinking
    bool on = true;
    while (1)
    {
        mcp23x17_get_level(&dev, 0, &val);
        mcp23x17_set_level(&dev, 8, on);
        mcp23x17_set_level(&dev, 9, on);
        mcp23x17_set_level(&dev, 10, on);
        mcp23x17_set_level(&dev, 11, on);
        mcp23x17_set_level(&dev, 12, on);
        mcp23x17_set_level(&dev, 13, on);
        mcp23x17_set_level(&dev, 14, on);
        mcp23x17_set_level(&dev, 15, on);
        printf("gpio 0 :: %i \n", val);
        on = !on;
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void app_main()
{
    ESP_ERROR_CHECK(i2cdev_init());
    xTaskCreate(test, "test", 2048, NULL, 5, NULL);
}
