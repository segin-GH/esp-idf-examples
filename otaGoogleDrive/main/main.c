#include <stdio.h>
#include <string.h>
#include <esp_wifi.h>
#include <esp_event.h>
#include <nvs_flash.h>
#include <esp_http_client.h>
#include <esp_https_ota.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <esp_log.h>
#include <driver/gpio.h>
#include <wifi_connect.h>

const char *wifiName = "Segin";
const char *password = "2003sejin";

void app_main(void)
{
    nvs_flash_init();
    wifi_init();
    wifi_connect_sta(wifiName, password, 10000);
}