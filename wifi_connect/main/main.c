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

wifi_cred_t hotspot_cred = {

    .wifi_name = "ESP32 HOTSPOT",
    .wifi_pass = "password",
    .k_timeout = 10000,
};

void wifi_connect(void *args)
{
    esp_err_t err;

    // err = wifi_connect_ap(&hotspot_cred);
    // for(int i = 45; i > 0; --i)
    // {
    //     printf("disconnecting ap %d\n", i);
    //     vTaskDelay(1000/portTICK_PERIOD_MS);
    // }

    err = wifi_connect_sta(&wifi_cred);
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
        deinit_wifi();

    vTaskDelay(100/portTICK_PERIOD_MS); 
    printf("Deleting the task");
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
