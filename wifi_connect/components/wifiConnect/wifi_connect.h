#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <esp_err.h>
#include <esp_netif.h>
#include <esp_wifi.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>


#ifdef __cplusplus
extern "C" {
#endif

typedef struct 
{
    char wifi_name[16];
    char wifi_pass[16];
    int k_timeout;

}wifi_cred_t;


esp_err_t wifi_init(void);
esp_err_t wifi_connect_sta(wifi_cred_t *cred);
esp_err_t wifi_connect_ap(const char* wifiname, const char* pass);
esp_err_t wifi_disconnect_sta(wifi_cred_t *cred);
esp_err_t deinit_wifi(void);


#ifdef __cplusplus
}
#endif