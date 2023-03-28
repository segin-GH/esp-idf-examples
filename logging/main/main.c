#include <stdio.h>
#include <esp_log.h>


void app_main(void)
{
    esp_log_level_set("LOG", ESP_LOG_INFO);
    ESP_LOGE("LOG", "THIS IS A ERROR MSG");
    ESP_LOGW("LOG", "THIS IS A WARNING MSG");
    ESP_LOGI("LOG", "THIS IS A INFO MSG");
    ESP_LOGD("LOG", "THIS IS A DEBUG MSG");
    ESP_LOGV("LOG", "THIS IS A VERBOSE MSG");
}