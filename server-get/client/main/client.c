// client_main.c
#include <string.h>
#include "esp_log.h"
#include "esp_http_client.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "wifi_connect.h"
#include "nvs_flash.h"

#define MAX_HTTP_OUTPUT_BUFFER 2048 // adjust as needed, but don't get too excited

static const char *TAG = "HTTP_CLIENT";

esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    switch (evt->event_id)
    {
    // here's your event handler, pretending to be useful
    case HTTP_EVENT_ERROR:
        ESP_LOGE(TAG, "HTTP_EVENT_ERROR");
        break;

    case HTTP_EVENT_ON_CONNECTED:
        ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");
        break;

    case HTTP_EVENT_HEADER_SENT:
        ESP_LOGI(TAG, "HTTP_EVENT_HEADER_SENT");
        break;

    case HTTP_EVENT_ON_HEADER:
        ESP_LOGI(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
        break;

    case HTTP_EVENT_ON_DATA:
        if (esp_http_client_is_chunked_response(evt->client))
            printf("%.*s\n", evt->data_len, (char *)evt->data);
        break;

    case HTTP_EVENT_ON_FINISH:
        ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
        break;

    case HTTP_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
        break;
    }
    return ESP_OK;
}

static void print_data_in_hex(char *data, int length)
{
    for (int i = 0; i < length; i++)
    {
        printf("%02x ", (unsigned char)data[i]);
        if ((i + 1) % 16 == 0)
        {
            printf("\n");
        }
    }
    if (length % 16 != 0)
    {
        printf("\n");
    }
}

wifi_cred_t wifi_cred = {

    .wifi_name = "HACKLAB_2G",
    .wifi_pass = "HACK@LAB",
    .k_timeout = 10000,
};

void http_client_get_task(void *pvParameters)
{
    char output_buffer[MAX_HTTP_OUTPUT_BUFFER];
    esp_http_client_config_t config = {
        .url = "http://192.168.0.103/ota/update",
        .event_handler = _http_event_handler,
        .timeout_ms = 15000, // be patient, we're on the internet through sim7600 and it's slow as hell
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);

    esp_err_t err = esp_http_client_perform(client);

    if (err == ESP_OK)
    {
        ESP_LOGI(TAG, "HTTP GET Status = %d, content_length = %d",
                 esp_http_client_get_status_code(client),
                 esp_http_client_get_content_length(client));
    }
    else
    {
        ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);
    vTaskDelete(NULL);
}

void app_main()
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    wifi_init();

    ret = wifi_connect_sta(&wifi_cred);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to connect to WiFi");
        return;
    }

    xTaskCreate(&http_client_get_task, "http_client_get_task", 4096, NULL, 5, NULL);
}
