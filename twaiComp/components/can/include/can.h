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

    /**
     * @brief CAN message structure
     */
    typedef twai_message_t can_message_t;

    /**
     * @brief prints the msg in cool format too see if any msg is received
     *
     * @param array array of uint8_t to be printed
     * @param num_of_element number of elements in the array
     */
    void print_can_msg_in_cool_8t(uint8_t array[], int num_of_element);

    /**
     * @brief initialize the can module
     *
     * @param rx_gpio_num  pin number for rx
     * @param tx_gpio_num  pin number for tx
     * @param ticks_to_wait_tx ticks to wait until the message is discarded
     * @return  returns ESP_OK if successful else ESP_FAIL
     */
    esp_err_t can_init(uint8_t rx_gpio_num, uint8_t tx_gpio_num, TickType_t ticks_to_wait_tx);

    /**
     * @brief deinitialize the can module and clean up the resources
     *
     * @return esp_err_t
     */
    esp_err_t can_deinit(void);

    /**
     * @brief used for transmitting the can message
     *
     * @param message pointer to the can message (can_message_t)
     * @param ticks_to_wait ticks to wait until the message is discarded
     * @return esp_err_t returns ESP_OK if successful else ESP_FAIL
     */
    esp_err_t can_transmit(can_message_t *message, TickType_t ticks_to_wait);

    /**
     * @brief register the can handler
     *
     * @param handler pointer to the handler function
     * @param handler_arg data needed by the handler function
     * @return esp_err_t returns ESP_OK if successful else ESP_FAIL
     */
    esp_err_t can_register_can_handler(esp_event_handler_t handler, void *handler_arg);

#ifdef __cplusplus
}
#endif
