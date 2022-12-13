#ifndef WIFI_CONNECT_H
#define WIFI_CONNECT_H

#include <stdio.h>
#include <esp_err.h>

void wifi_init(void);
esp_err_t wifi_connect_sta(const char* wifiname, const char* pass, int timeout);
void wifi_connect_ap(const char* wifiname, const char* pass);
void wifi_disconnect(void);


#endif /* WIFI_CONNECT_H */