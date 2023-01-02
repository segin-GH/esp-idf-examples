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

/**
 * @brief WiFi credentials structure
 * 
 * @param wifi_name name for your wifi
 * 
 * @param wifi_pass password for your wifi
 * 
 * @param k_timeout timeout value for wifi connection or disconnection (in ms) 
 */
typedef struct 
{
    char wifi_name[16];
    char wifi_pass[16];
    int k_timeout;

} wifi_cred_t;


/**
 * @brief Initialize WiFi 
 *
 * @return ESP_OK if successful, ESP_FAIL otherwise
 */
esp_err_t wifi_init(void);


/**
 * @brief Connect to WiFi network in station mode
 *
 * @param[in] cred WiFi credentials
 *
 * @return ESP_OK if successful, ESP_FAIL otherwise
 */
esp_err_t wifi_connect_sta(wifi_cred_t *cred);


/**
 * @brief Connect to WiFi network in access point mode
 *
 * @param[in] cred WiFi credentials
 *
 * @return ESP_OK if successful, ESP_FAIL otherwise
 */
esp_err_t wifi_connect_ap(wifi_cred_t *cred);


/**
 * @brief Disconnect from WiFi network in station mode
 *
 * @param[in] cred WiFi credentials
 *
 * @return ESP_OK if successful, ESP_FAIL otherwise
 */
esp_err_t wifi_disconnect_sta(wifi_cred_t *cred);


/**
 * @brief Deinitialize WiFi
 *
 * @return ESP_OK if successful, ESP_FAIL otherwise
 */
esp_err_t deinit_wifi(void);


#ifdef __cplusplus
}
#endif
