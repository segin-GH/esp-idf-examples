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

typedef const char* string;
// #define NoReturn void __attribute__((noreturn));


void init_ota(void);
// NoReturn task_fatal_error( string exit_msg, string tag, esp_err_t err, bool reset);
static void task__error(string exit_msg, string tag, esp_err_t err, bool reset);

#define TASK_ERROR_FATALE(exit_msg, tag, err)\
            task__error(exit_msg, tag, err, true);

#define TASK_ERROR_NON_FATALE(exit_msg, tag, err)\
            task__error(exit_msg, tag, err, false);





#ifdef __cplusplus
}
#endif