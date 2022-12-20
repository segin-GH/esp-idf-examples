#ifndef LED_TOGGLE_H
#define LED_TOGGLE_H

#include <stdio.h>
#include <stdbool.h>
#include <driver/gpio.h>

void init_led_as_output(int ledNum);
void toggle_led(bool toggle);

#endif /* LED_TOGGLE_H */