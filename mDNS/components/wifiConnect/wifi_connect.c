
#include "wifi_connect.h"

/* TAG for log */
#define WIFI_TAG "[WIFI CONNECT]"

/* for network interface */
esp_netif_t *esp_netif;

/* Event Handler */
static EventGroupHandle_t wifi_events;

static int CONNECTED_GOT_IP = BIT0;
static int DISCONNECTED = BIT1;

/* flag to know if the user initiated the disconnection */
static bool USER_DISCONNECTED = false;

/* Event Handler Callback */
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
            {
                esp_wifi_connect();
                ESP_LOGI(WIFI_TAG, "STA got disconnected Trying to reconnect");
            }
            xEventGroupSetBits(wifi_events,DISCONNECTED);
            ESP_LOGI(WIFI_TAG, "STA got disconnected");
            break;

        /* when an IP addr is available  */
        case IP_EVENT_STA_GOT_IP:
            ESP_LOGI(WIFI_TAG, "Got IP");
            xEventGroupSetBits(wifi_events,CONNECTED_GOT_IP);
            break;
        
        /* when ap is enabled */        
        case WIFI_EVENT_AP_START:
            ESP_LOGI(WIFI_TAG, "AP Enabled");
            break;
        
        /* when ap is disabled */
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
    /* Create an event group */
    wifi_events = xEventGroupCreate();

    /* Create a default ESP32 WiFi station netif */
    esp_netif = esp_netif_create_default_wifi_sta();

    /* Set WiFi mode to station */
    esp_wifi_set_mode(WIFI_MODE_STA);

    /* Set WiFi configuration */
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = {0},
            .password = {0}
        }
    };
    /* Copy the wifi name */
    memcpy(wifi_config.sta.ssid, cred->wifi_name, sizeof(wifi_config.sta.ssid));
    
    /* Copy the wifi password */
    memcpy(wifi_config.sta.password, cred->wifi_pass, sizeof(wifi_config.sta.password));
    
    /* Set the wifi config */
    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config);

    /* Start WiFi */
    esp_wifi_start();

    /* Wait for the CONNECTED_GOT_IP event */
    EventBits_t result = xEventGroupWaitBits(
        wifi_events, CONNECTED_GOT_IP, pdTRUE, pdFALSE, pdMS_TO_TICKS(cred->k_timeout));

    /* Return ESP_OK if the CONNECTED_GOT_IP event was received, ESP_FAIL otherwise */
    if (result == CONNECTED_GOT_IP)
    {
        USER_DISCONNECTED = false;
        return ESP_OK;
    }
    return ESP_FAIL;
}

esp_err_t wifi_connect_ap(wifi_cred_t *cred)
{
    /* Create a default ESP32 WiFi access point netif */
    esp_netif = esp_netif_create_default_wifi_ap();

    /* Set WiFi configuration */
    wifi_config_t wifi_config = {0};
    
    /* Copy the WiFi name */
    memcpy(wifi_config.ap.ssid, cred->wifi_name, sizeof(wifi_config.ap.ssid));
    
    /* Copy the WiFi password */
    memcpy(wifi_config.ap.password, cred->wifi_pass, sizeof(wifi_config.ap.password));
    
    /* Set the authentication mode to WPA/WPA2 PSK */
    wifi_config.ap.authmode = WIFI_AUTH_WPA_WPA2_PSK;
    
    /* Set the maximum number of devices can connect to 4 */
    wifi_config.ap.max_connection = 4;

    /* Set WiFi mode to access point */
    esp_wifi_set_mode(WIFI_MODE_AP);

    /* Set the WiFi configuration */
    esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config);

    /* Start WiFi */
    esp_wifi_start();

    /* Set the USER_DISCONNECTED flag to false */
    USER_DISCONNECTED = false;

    /* Return ESP_FAIL */
    return ESP_FAIL;
}


esp_err_t wifi_disconnect_sta(wifi_cred_t *cred)
{
    /* Set the USER_DISCONNECTED flag to true */
    USER_DISCONNECTED = true;

    /* Disconnect from the WiFi network */
    esp_wifi_disconnect();

    /* Wait for the DISCONNECTED event */
    EventBits_t result = xEventGroupWaitBits(
        wifi_events, DISCONNECTED, pdTRUE, pdFALSE, pdMS_TO_TICKS(cred->k_timeout));

    /* Return ESP_OK if the DISCONNECTED event was received, ESP_FAIL otherwise */
    if (result == DISCONNECTED)
    {
        USER_DISCONNECTED = true;
        return ESP_OK;
    }

    return ESP_FAIL;
}

esp_err_t deinit_wifi(void)
{

    ESP_LOGI(WIFI_TAG, "DE-INITIALIZING WIFI ....");

    /* Stop and deinitialize WiFi */
    esp_wifi_stop();
    esp_wifi_deinit();

    ESP_LOGI(WIFI_TAG, "DE-INITIALIZING WIFI COMPLETE ");

    return ESP_OK;
}
