#include <stdio.h>
#include <esp_log.h>

#define TAG "CONFIG"

void app_main(void)
{
    ESP_LOGI(TAG,"MY_INT %d", CONFIG_MY_INT);
    ESP_LOGI(TAG,"MY_STRING %s", CONFIG_MY_STRING);
    
}
