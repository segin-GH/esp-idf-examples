#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "driver/twai.h"
#include "esp_log.h"
#include "esp_err.h"
#include "freertos/queue.h"

#ifdef __cplusplus
extern "C"
{
#endif
    typedef twai_message_t can_message_t;

    esp_err_t can_init(uint8_t rx_gpio_num, uint8_t tx_gpio_num, TickType_t ticks_to_wait_tx);
    esp_err_t can_deinit(void);
    esp_err_t can_transmit(can_message_t *message, TickType_t ticks_to_wait);
    esp_err_t can_receive(can_message_t *message, TickType_t ticks_to_wait);

#ifdef __cplusplus
}
#endif
