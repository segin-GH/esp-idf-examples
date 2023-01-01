

/** @todo
   
*   TODO a struct should be passed in for wifi name and password and kTimeout
   TODO change name to wifi -> wifi_connect
   TODO (wifi NVS flash) the drivers can be saved in a flash and when restarting dont need
        to reinit again and again
    TODO write a reconnection strategy's


*/










#include "wifi_connect.h"


/* TAG for log */
#define WIFI_TAG "[WIFI]"
#define WIFI_STA "[WIFI STA]"
/* for network interface */
esp_netif_t *esp_netif;

static EventGroupHandle_t wifi_events;
static const int CONNECTED_GOT_IP = BIT0;
static const int DISCONNECTED = BIT1;
static bool USER_DISCONNECTED = false;

void event_handler(void *args, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    switch(event_id)
    {
        /* when wifi start's connecting as station */
        case WIFI_EVENT_STA_START:
            ESP_LOGI(WIFI_TAG, "Connecting.....");
            esp_wifi_connect();
            break;

        /* when wifi is connected as station */
        case WIFI_EVENT_STA_CONNECTED:
            ESP_LOGI(WIFI_TAG, "STA Connected");
            break;

        /* when wifi is disconnected */
        case WIFI_EVENT_STA_DISCONNECTED:
            if(USER_DISCONNECTED == false)
                esp_wifi_connect();
            xEventGroupSetBits(wifi_events,DISCONNECTED);
            ESP_LOGI(WIFI_TAG, "STA got disconnected");
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

esp_err_t wifi_init(void)
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
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, event_handler, NULL);
    
    return ESP_OK;
}

esp_err_t wifi_connect_sta(wifi_cred_t *cred)
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
    strncpy((char *)wifi_config.sta.ssid, cred -> wifi_name, sizeof(wifi_config.sta.ssid));
    strncpy((char *)wifi_config.sta.password, cred -> wifi_pass, sizeof(wifi_config.sta.password));

    /* set wifi mode to station */
    esp_wifi_set_mode(WIFI_MODE_STA);

    /* set the wifi configuration */
    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config);

    /* start the wifi */
    esp_wifi_start();

    /* wait for the CONNECTED_GOT_IP or DISCONNECTED */
    EventBits_t result = xEventGroupWaitBits(
        wifi_events, CONNECTED_GOT_IP, pdTRUE, pdFALSE, pdMS_TO_TICKS(cred -> k_timeout));

    /* return ESP_OK if the CONNECTED_GOT_IP event was received, ESP_FAIL otherwise */
    if(result == CONNECTED_GOT_IP)
    {
        USER_DISCONNECTED = false;
        return ESP_OK;
    }
    return  ESP_FAIL;

}

esp_err_t wifi_connect_ap(const char* wifiname, const char* password)
{
    esp_netif = esp_netif_create_default_wifi_ap();

    //TODO need to refactor set wifi config
    
    /* set wifi config */
    wifi_config_t wifi_config;
    memset(&wifi_config, 0, sizeof(wifi_config_t));
    strncpy((char *)wifi_config.ap.ssid, wifiname, sizeof(wifi_config.ap.ssid));
    strncpy((char *)wifi_config.ap.password, password, sizeof(wifi_config.ap.password));
    wifi_config.ap.authmode = WIFI_AUTH_WPA_WPA2_PSK;
    wifi_config.ap.max_connection = 4;
    
    /* set wifi mode to acces point */
    esp_wifi_set_mode(WIFI_MODE_AP);

    /* set the wifi configuration */
    esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config);

    /* start the wifi */
    esp_wifi_start();
    USER_DISCONNECTED = false;
    return ESP_FAIL;
}


esp_err_t wifi_disconnect_sta(wifi_cred_t *cred)
{
    USER_DISCONNECTED = true;

    esp_wifi_disconnect();

    EventBits_t result = xEventGroupWaitBits(
        wifi_events, DISCONNECTED, pdTRUE, pdFALSE, pdMS_TO_TICKS(cred -> k_timeout));
    
        if(result == DISCONNECTED)
        {
            USER_DISCONNECTED = true;
            return ESP_OK;
        }
    
    return ESP_FAIL;
}

esp_err_t deinit_wifi(void)
{
    EventBits_t result = xEventGroupWaitBits(
        wifi_events, DISCONNECTED, pdTRUE, pdFALSE, portMAX_DELAY);
    
    ESP_LOGI(WIFI_TAG, "DE-INITIALIZING WIFI ....");
    esp_wifi_stop();
    esp_wifi_deinit();
    ESP_LOGI(WIFI_TAG, "DE-INITIALIZING WIFI COMPLETE ");
    return ESP_OK;
}
