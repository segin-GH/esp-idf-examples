#include "wifi_connect.h"


/* TAG for log */
#define WIFI_TAG "[WIFI]"
/* for network interface */
esp_netif_t *esp_netif;

static EventGroupHandle_t wifi_events;
static const int CONNECTED_GOT_IP = BIT0;
static const int DISCONNECTED = BIT1;

void event_handler(void *args, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    switch(event_id)
    {
        /* when wifi start's connecting as station */
        case SYSTEM_EVENT_STA_START:
            ESP_LOGI(WIFI_TAG, "Connecting.....");
            esp_wifi_connect();
            break;

        /* when wifi is connected as station */
        case SYSTEM_EVENT_STA_CONNECTED:
            ESP_LOGI(WIFI_TAG, "Connected");
            break;

        /* when wifi is disconnected */
        case SYSTEM_EVENT_STA_DISCONNECTED:
            ESP_LOGI(WIFI_TAG, "disconnected");
            xEventGroupSetBits(wifi_events,DISCONNECTED);
            break;

        /* when an IP addr is available  */
        case IP_EVENT_STA_GOT_IP:
            ESP_LOGI(WIFI_TAG, "Got IP");
            xEventGroupSetBits(wifi_events,CONNECTED_GOT_IP);
            break;

        case WIFI_EVENT_AP_START:
            ESP_LOGI(WIFI_TAG, "AP Enabled");
            break;
        
        case WIFI_EVENT_AP_STOP:
            ESP_LOGI(WIFI_TAG, "AP Disabled");
            break;

        /* nothing to be default */
        default:
            break;
    }
}

void wifi_init(void)
{
    /* initialize network interface */
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

esp_err_t wifi_connect_sta(const char *wifiName, const char *password, const int K_timeOut)
{
    /* create an event group */
    wifi_events = xEventGroupCreate();

    /* create a default ESP32 wifi station netif */
    esp_netif = esp_netif_create_default_wifi_sta();

//TODO refactor this part of set wifi config
    
/* 
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = wifiname,
            .password = pass
        }
    };
*/

    /* set wifi config */
    wifi_config_t wifi_config;
    memset(&wifi_config, 0, sizeof(wifi_config_t));
    strncpy((char *)wifi_config.sta.ssid, wifiName, sizeof(wifi_config.sta.ssid));
    strncpy((char *)wifi_config.sta.password, password, sizeof(wifi_config.sta.password));

    /* set wifi mode to station */
    esp_wifi_set_mode(WIFI_MODE_STA);

    /* set the wifi configuration */
    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config);

    /* start the wifi */
    esp_wifi_start();

    /* wait for the CONNECTED_GOT_IP or DISCONNECTED */
    EventBits_t result = xEventGroupWaitBits(
        wifi_events, CONNECTED_GOT_IP | DISCONNECTED, pdTRUE, pdFALSE,
        pdMS_TO_TICKS(K_timeOut));

    /* return ESP_OK if the CONNECTED_GOT_IP event was received, ESP_FAIL otherwise */
    return (result == CONNECTED_GOT_IP) ? ESP_OK : ESP_FAIL;
}

void wifi_connect_ap(const char* hotspotName, const char* password)
{
    esp_netif = esp_netif_create_default_wifi_ap();

    //TODO need to refactor set wifi config
    
    /* set wifi config */
    wifi_config_t wifi_config;
    memset(&wifi_config, 0, sizeof(wifi_config_t));
    strncpy((char *)wifi_config.ap.ssid, hotspotName, sizeof(wifi_config.ap.ssid));
    strncpy((char *)wifi_config.ap.password, password, sizeof(wifi_config.ap.password));
    wifi_config.ap.authmode = WIFI_AUTH_WPA_WPA2_PSK;
    wifi_config.ap.max_connection = 4;
    
    /* set wifi mode to acces point */
    esp_wifi_set_mode(WIFI_MODE_AP);

    /* set the wifi configuration */
    esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config);

    /* start the wifi */
    esp_wifi_start();
}

void wifi_disconnect(void)
{
    ESP_LOGI(WIFI_TAG, "DISCONNECTING....");
    esp_wifi_disconnect();
    esp_wifi_stop();
    ESP_LOGI(WIFI_TAG, "DISCONNECTING COMPLETE.");
}


/*  wifi sta */

/* WiFi Station Example */

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#define WIFI_SSID "sussy_baka"
#define WIFI_PASSWORD "luffy@gear5"
#define MAXIMUM_RETRY 10

/* Event group for FreeRTOS to signal when we are connected */
static EventGroupHandle_t wifi_event_group;

/* Define event group bits for connected and fail status */
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1

static const char *LOG_TAG = "wifi_station";
static int connection_retry_count = 0;

static void handle_event(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        esp_wifi_connect();
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        if (connection_retry_count < MAXIMUM_RETRY)
        {
            esp_wifi_connect();
            connection_retry_count++;
            ESP_LOGI(LOG_TAG, "Retrying to connect to the AP");
        }
        else
        {
            xEventGroupSetBits(wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI(LOG_TAG, "Failed to connect to the AP");
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *event_info = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(LOG_TAG, "Got IP address:" IPSTR, IP2STR(&event_info->ip_info.ip));
        connection_retry_count = 0;
        xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

void initialize_wifi_station(void)
{
    wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t wifi_init_cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&wifi_init_cfg));

    esp_event_handler_instance_t any_id_instance;
    esp_event_handler_instance_t got_ip_instance;

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &handle_event, NULL, &any_id_instance));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &handle_event, NULL, &got_ip_instance));

    wifi_config_t wifi_cfg = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASSWORD,
        },
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_cfg));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(LOG_TAG, "WiFi Station initialized.");

    EventBits_t bits = xEventGroupWaitBits(wifi_event_group, WIFI_CONNECTED_BIT | WIFI_FAIL_BIT, pdFALSE, pdFALSE, portMAX_DELAY);

    if (bits & WIFI_CONNECTED_BIT)
    {
        ESP_LOGI(LOG_TAG, "Connected to AP with SSID: %s and Password: %s", WIFI_SSID, WIFI_PASSWORD);
    }
    else if (bits & WIFI_FAIL_BIT)
    {
        ESP_LOGI(LOG_TAG, "Failed to connect with SSID: %s, Password: %s", WIFI_SSID, WIFI_PASSWORD);
    }
    else
    {
        ESP_LOGE(LOG_TAG, "Unexpected event");
    }

    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, got_ip_instance));
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, any_id_instance));
    vEventGroupDelete(wifi_event_group);
}

void app_main(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(LOG_TAG, "Initializing in Station Mode");
    initialize_wifi_station();
}
