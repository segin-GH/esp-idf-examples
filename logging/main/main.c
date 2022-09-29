#include <stdio.h>
# include <esp_log.h>

void app_main(void)
{
    ESP_LOGE("LOG","THIS IS A ERROR");
    ESP_LOGW("LOG","THIS IS A WARNING");
    ESP_LOGI("LOG","THIS IS A INFO");
    ESP_LOGD("LOG","THIS IS A DEBUG");
    ESP_LOGV("LOG","THIS IS A VERBOSE")
}