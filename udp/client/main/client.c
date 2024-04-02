#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "lwip/sockets.h"

static const char *TAG = "udp_client";

#define WIFI_SSID "HACKLAB_10G"
#define WIFI_PASS "HACK@LAB"
#define MAX_RETRY 5

#define HOST_IP_ADDR "10.42.0.113"

static EventGroupHandle_t s_wifi_event_group;
static int retry_num = 0;
#define PORT 3333

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1

static void event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        esp_wifi_connect();
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        if (retry_num < MAX_RETRY)
        {
            esp_wifi_connect();
            retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        }
        else
        {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI(TAG, "connect to the AP fail");
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

void udp_client_task(void *pvParameters)
{
    char payload[128]; // because why not announce to the world in 128 characters?

    struct sockaddr_in dest_addr = {
        .sin_addr.s_addr = inet_addr(HOST_IP_ADDR), // HOST_IP_ADDR, because hardcoding is fun
        .sin_family = AF_INET,                      // AF_INET, because we're not time travelers
        .sin_port = htons(PORT),                    // PORT, the beacon of hope
    };
    char addr_str[128];                                              // planning to print the internet?
    inet_ntoa_r(dest_addr.sin_addr, addr_str, sizeof(addr_str) - 1); // because who needs safety?

    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP); // because why use variables when you can hardcode?
    if (sock < 0)
    {
        ESP_LOGE(TAG, "Socket creation failed, try not to cry: errno %d", errno);
        vTaskDelete(NULL); // escape route
        return;
    }
    ESP_LOGI(TAG, "Socket created, sending to %s:%d", HOST_IP_ADDR, PORT); // because verbosity is next to godliness

    for (int i = 0; i <= 100; i++)
    {                                                 // because for loops are the pinnacle of programming
        sprintf(payload, "Message from ESP32 %d", i); // sprintf, because who cares about buffer overflows?
        if (sendto(sock, payload, strlen(payload), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0)
        {
            ESP_LOGE(TAG, "Error during sending: errno %d", errno); // because error handling is an afterthought
            break;                                                  // break, because why try again?
        }
        ESP_LOGI(TAG, "Message sent #%d", i); // because we need to know every single time
        vTaskDelay(100 / portTICK_PERIOD_MS); // because we're patient
    }

    if (sock != -1)
    {
        ESP_LOGE(TAG, "Closing socket, because we're done here");
        shutdown(sock, 0); // because shutdown is just a formality
        close(sock);       // because we're tidy
    }
    vTaskDelete(NULL); // because why not delete the task twice?
}

void app_main(void)
{
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    s_wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_got_ip));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
            .pmf_cfg = {
                .capable = true,
                .required = false},
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_sta finished.");

    // Wait for Wi-Fi connection
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
                                           WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                                           pdFALSE,
                                           pdFALSE,
                                           portMAX_DELAY);

    if (bits & WIFI_CONNECTED_BIT)
    {
        ESP_LOGI(TAG, "connected to ap SSID:%s", WIFI_SSID);
    }
    else if (bits & WIFI_FAIL_BIT)
    {
        ESP_LOGI(TAG, "Failed to connect to SSID:%s", WIFI_SSID);
    }
    else
    {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
    }

    // Get IP address
    esp_netif_t *netif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");
    esp_netif_ip_info_t ip_info;
    esp_netif_get_ip_info(netif, &ip_info);

    // Convert IP address to human-readable form
    char str_ip[16];
    esp_ip4addr_ntoa(&ip_info.ip, str_ip, sizeof(str_ip));
    ESP_LOGI(TAG, "IP Address: %s", str_ip);

    xTaskCreate(udp_client_task, "udp_client", 4096, NULL, 5, NULL);
}
