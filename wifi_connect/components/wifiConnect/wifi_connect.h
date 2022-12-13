#ifndef WIFI_CONNECT_H
#define WIFI_CONNECT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <esp_err.h>
#include <esp_netif.h>
#include <esp_wifi.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>


void wifi_init(void);
esp_err_t wifi_connect_sta(const char* wifiname, const char* pass, int timeout);
void wifi_connect_ap(const char* wifiname, const char* pass);
void wifi_disconnect(void);


#endif /* WIFI_CONNECT_H */