#ifndef PUSH_BTN_H
#define PUSH_BTN_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>
#include <cJSON.h>
#include <app_main.h>

void init_btn_onboard();


#endif /* PUSH_BTN_H */