#include "wifi_connect.h"

#define WIFI_TAG "[WIFI]"
esp_netif_t *esp_netif;

static EventGroupHandle_t wifi_events;
static const int CONNECTED_GOT_IP = BIT0;
static const int DISCONNECTED = BIT1;


void event_handler(void *args, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    switch(event_id)
    {
        case SYSTEM_EVENT_STA_START:
            ESP_LOGI(WIFI_TAG, "Connecting.....");
            esp_wifi_connect();
            break;
        case SYSTEM_EVENT_STA_CONNECTED:
            ESP_LOGI(WIFI_TAG, "Connected");
            break;
        case SYSTEM_EVENT_STA_DISCONNECTED:
            ESP_LOGI(WIFI_TAG, "disconnected");
            xEventGroupSetBits(wifi_events,DISCONNECTED);
            break;
        case IP_EVENT_STA_GOT_IP:
            ESP_LOGI(WIFI_TAG, "Got IP");
            xEventGroupSetBits(wifi_events,CONNECTED_GOT_IP);
        default /* nothing to be default */:
            break;
    }
}

void wifi_init(void)
{
    esp_netif_init();

    /* create a default event loop */
    esp_event_loop_create_default();

    /* initialize wifi*/
    wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wifi_init_config);

    /* set wifi storage */
    esp_wifi_set_storage(WIFI_STORAGE_FLASH);

    /* Register event handlers */
    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, event_handler, NULL);
    esp_event_handler_register(IP_EVENT,IP_EVENT_STA_GOT_IP,event_handler,NULL);
}

esp_err_t wifi_connect_sta(const char *wifiname, const char *pass, int timeout)
{
    wifi_events = xEventGroupCreate();
    esp_netif = esp_netif_create_default_wifi_sta();

    wifi_config_t wifi_config;
    memset(&wifi_config, 0, sizeof(wifi_config_t));
    strncpy((char *)wifi_config.sta.ssid, wifiname, sizeof(wifi_config.sta.ssid));
    strncpy((char *)wifi_config.sta.password, pass, sizeof(wifi_config.sta.password));

    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config);
    esp_wifi_start();

    EventBits_t result = xEventGroupWaitBits(wifi_events, CONNECTED_GOT_IP | DISCONNECTED, 
                                                pdTRUE, pdFALSE, pdMS_TO_TICKS(timeout));

    return (result == CONNECTED_GOT_IP) ? ESP_OK : ESP_FAIL;
}

void wifi_connect_ap(const char* wifiname, const char* pass)
{
    //
}

void wifi_disconnect(void)
{
    //
}