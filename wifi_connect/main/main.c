#include <stdio.h>
#include <wifi_connect.h>
#include <nvs_flash.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <nvs_flash.h>
#include <esp_err.h>

const char *wifiName = "Segin";
const char *password = "2003sejin";

wifi_cred_t wifi_cred = {
    
    .wifi_name = "Segin",
    .wifi_pass = "2003sejin",
    .k_timeout = 10000,
};

void wifi_connect(void *args)
{
/*     wifi_connect_ap("ESP32_HOTSPOT","password");
    for(int i = 45; i > 0; --i)
    {
        printf("disconnecting ap %d\n", i);
        vTaskDelay(1000/portTICK_PERIOD_MS);
    } */

    esp_err_t err = wifi_connect_sta(wifiName, password, 10000);
    if(err)
    {
        ESP_LOGE("WIFI CONNECT", "Failed to connect");
        vTaskDelete(NULL);
    }
    for(int i = 5; i > 0; --i) 
    {
        printf("Disconnecting in %i second.\n",i);
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
    err = wifi_disconnect_sta(& wifi_cred);
    if(err == ESP_OK)
    {
        deinit_wifi();

    }

    
    vTaskDelay(10000/portTICK_PERIOD_MS); 
    vTaskDelete(NULL);
}


void app_main(void)
{
    nvs_flash_init();
    wifi_init();

    xTaskCreatePinnedToCore(
        wifi_connect,
        "wifi_connect",
        2024,
        NULL,
        5,
        NULL,
        PRO_CPU_NUM
    );
}
