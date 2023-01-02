#pragma once

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "esp_log.h"
#include "esp_http_server.h"
#include "mdns.h"
#include "esp_spiffs.h"
#include "nvs_flash.h"
#include "esp_ota_ops.h"
#include "esp_app_format.h"
#include "esp_flash_partitions.h"
#include "esp_partition.h"

#ifdef __cplusplus
extern "C" {
#endif

/* just a hack around, for (const char *) */
typedef const char* string;

/* Function prototype */

/**
 * @brief initialize ota and calls required api to start ota
 * 
 * @param void nothing to pass as args
 * 
 * @return void nothing to
 */
void init_ota(void);

/**
 * @brief a function which handles fatale errors & prints useful logs
 * 
 * @param exit_msg this message shows the main reason why there was an error
 * @param tag used for log tag
 * @param err used for knowing why this error happened pass in 0x101 prints out in string ESP_ERR_NO_MEM 
 * @param reset used for reseting the chip if true the chip gets reset
 **/
void __attribute__((noreturn)) task_fatal_error( string exit_msg, string tag, esp_err_t err, bool reset);


/**
 * @brief a function which sets mDNS name use this function if you want to
 *        override the standard name of the mDNS (standard is http://esp-server.local/ )
 *
 * @param name the name that you want for your server
 *             if you pass in null it will use the default mDNS name
 *
 */
void set_mDNS_name(const char * name);


/**
 * @brief set 
 */
void set_spiffs_path(char *full_path);

/* MACROS */

/**
 * @brief if this macro is called then the chip resets after printing the error msg.
 *
 * @param exit_msg this message shows the main reason why there was an error.
 * @param tag used for log tag.
 * @param err used for knowing why the error happened,
 *            eg:- if passed in 0x101, then it prints out a string ESP_ERR_NO_MEM 
 * 
 * @attention  if this api is called then the chip gets reset after logging the error.
 *
 **/
#define TASK_ERROR_FATALE(exit_msg, tag, err)\
            task_fatal_error(exit_msg, tag, err, true);

/**
 * @brief if this macro is called then the chip does not resets after printing the error msg.
 *
 * @param exit_msg this message shows the main reason why there was an error.
 * @param tag used for log tag.
 * @param err used for knowing why the error happened.
 *            eg:- if passed in 0x101, then it prints out a string ESP_ERR_NO_MEM 
 * 
 * @attention  if this api is called then chip does not get reset after logging the error.
 *
 **/
#define TASK_ERROR_NON_FATALE(exit_msg, tag, err)\
            task_fatal_error(exit_msg, tag, err, false);



#ifdef __cplusplus
}
#endif