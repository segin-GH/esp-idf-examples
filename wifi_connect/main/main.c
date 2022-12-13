#include <stdio.h>
#include <wifi_connect.h>
#include <nvs_flash.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

const char *wifiName = "Segin";
const char *password = "2003sejin";

void wifi_connect(void *args)
{
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
    wifi_disconnect();
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
