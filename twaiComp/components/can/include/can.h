#pragma once

#include <stdio.h>
#include "driver/twai.h"
#include "esp_log.h"
#include "esp_err.h"
#include "freertos/queue.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_event.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define CAN_EVENT (esp_event_base_t)0x1000
#define RECEIVED_CAN_MESSAGE 0x01

    typedef twai_message_t can_message_t;

    void print_can_msg_in_cool_8t(uint8_t array[], int num_of_element);
    esp_err_t can_init(uint8_t rx_gpio_num, uint8_t tx_gpio_num, TickType_t ticks_to_wait_tx);
    esp_err_t can_deinit(void);
    esp_err_t can_transmit(can_message_t *message, TickType_t ticks_to_wait);
    esp_err_t can_register_can_handler(esp_event_handler_t handler, void *handler_arg);

#ifdef __cplusplus
}
#endif
