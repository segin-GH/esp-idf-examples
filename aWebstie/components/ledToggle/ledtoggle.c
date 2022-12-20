#include "ledtoggle.h"

static int referenceLed;

void init_led_as_output(int ledNum)
{
    referenceLed = ledNum;
    gpio_pad_select_gpio(ledNum);
    gpio_set_direction(ledNum,GPIO_MODE_OUTPUT);
}

void toggle_led(bool toggle)
{
    gpio_set_level(referenceLed, toggle);
}