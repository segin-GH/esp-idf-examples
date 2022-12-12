#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_event.h>
#include <esp_wifi.h>
#include <nvs_flash.h>

#define MAX_APs 10

static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    return ESP_OK;
}

void wifiInit()
{
    nvs_flash_init();
    tcpip_adapter_init();

    esp_event_loop_init(event_handler, NULL);

    wifi_init_config_t wifi_config = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wifi_config);
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_start();
}

static char *getAuthModeName(wifi_auth_mode_t auth_mode)
{
    char *name[] = {"OPEN", "WEP", "WPA PSK", "WPA2 PSK", "WPA WPA2 PSK", "MAX"};
    return name[auth_mode];
}

void app_main(void)
{
    /* inti the wifi */
    wifiInit();

    wifi_scan_config_t scan_config = {
        .ssid = 0,
        .bssid = 0,
        .channel = 0,
        .show_hidden = true
    };
    /* start the scan  */
    esp_wifi_scan_start(&scan_config, true);
    wifi_ap_record_t wifi_records[MAX_APs];
    uint16_t maxRecods = MAX_APs;
    /* get the scaned data */
    esp_wifi_scan_get_ap_records(&maxRecods, wifi_records);

    /* just make a pretty table for the outputs */
    printf("Found %d access points:\n", maxRecods);
    printf("\n");
    printf("               SSID              | Channel | RSSI |   Auth Mode \n");
    printf("----------------------------------------------------------------\n");
    for (int i = 0; i < maxRecods; i++)
    printf("%32s | %7d | %4d | %12s\n", (char *)wifi_records[i].ssid, wifi_records[i].primary,
            wifi_records[i].rssi, getAuthModeName(wifi_records[i].authmode));
    printf("----------------------------------------------------------------\n");

}
