#include "ota.h"
#include <stdio.h>
#include <string.h>
#include "wifi_connect.h"
#include <esp_log.h>
#include <stdbool.h>
#include <driver/gpio.h>
#include <esp_http_server.h>
#include <mdns.h>
#include <esp_spiffs.h>
#include <nvs_flash.h>

void init_ota(void)
{
    printf("invoking ota\n");
    nvs_flash_init();
    wifi_init();

    wifi_connect_sta("Segin", "2003sejin", 10000);
}