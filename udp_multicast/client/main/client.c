#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>

static const char *TAG = "udp_server";
static const char *V4TAG = "mcast-ipv4";

#define WIFI_SSID "HACKLAB_10G"
#define WIFI_PASS "HACK@LAB"
#define MAX_RETRY 5

static EventGroupHandle_t s_wifi_event_group;
static int retry_num = 0;
#define PORT 3333

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1

#define UDP_PORT 3333
#define MULTICAST_TTL 1
#define MULTICAST_IPV4_ADDR "232.10.10.10"

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

/* Add a socket, either IPV4-only or IPV6 dual mode, to the IPV4
   multicast group */
static int socket_add_ipv4_multicast_group(int sock, bool assign_source_if)
{
    struct ip_mreq imreq = {0};
    struct in_addr iaddr = {0};
    int err;

    // Configure source interface
    imreq.imr_interface.s_addr = IPADDR_ANY;
    esp_netif_t *netif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");
    esp_netif_ip_info_t ip_info;

    if (esp_netif_get_ip_info(netif, &ip_info) != ESP_OK)
    {
        ESP_LOGE(V4TAG, "Failed to get IP address info.");
        return -1; // Use ESP-IDF error codes if appropriate
    }

    inet_addr_from_ip4addr(&iaddr, &ip_info.ip);

    // Validate and configure multicast address
    if (!inet_aton(MULTICAST_IPV4_ADDR, &imreq.imr_multiaddr.s_addr))
    {
        ESP_LOGE(V4TAG, "Invalid IPV4 multicast address '%s'.", MULTICAST_IPV4_ADDR);
        return -1;
    }

    if (!IP_MULTICAST(ntohl(imreq.imr_multiaddr.s_addr)))
    {
        ESP_LOGW(V4TAG, "Address '%s' is not a multicast address.", MULTICAST_IPV4_ADDR);
        // Consider whether you want to continue or return an error here
    }

    // Optionally assign the IPv4 multicast source interface
    if (assign_source_if)
    {
        err = setsockopt(sock, IPPROTO_IP, IP_MULTICAST_IF, &iaddr, sizeof(iaddr));
        if (err < 0)
        {
            ESP_LOGE(V4TAG, "Failed to set IP_MULTICAST_IF. errno: %d", errno);
            return -1;
        }
    }

    // Join multicast group
    err = setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &imreq, sizeof(imreq));
    if (err < 0)
    {
        ESP_LOGE(V4TAG, "Failed to join multicast group. errno: %d", errno);
        return -1;
    }

    return 0; // Success
}

static int create_multicast_ipv4_socket(void)
{
    struct sockaddr_in saddr = {0};
    int sock;
    uint8_t ttl = MULTICAST_TTL;

    // Create socket
    sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (sock < 0)
    {
        ESP_LOGE(V4TAG, "Failed to create socket. errno: %d", errno);
        return -1;
    }

    // Bind the socket
    saddr.sin_family = PF_INET;
    saddr.sin_port = htons(UDP_PORT);
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sock, (struct sockaddr *)&saddr, sizeof(saddr)) < 0)
    {
        ESP_LOGE(V4TAG, "Failed to bind socket. errno: %d", errno);
        close(sock);
        return -1;
    }

    // Set multicast TTL
    if (setsockopt(sock, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl)) < 0)
    {
        ESP_LOGE(V4TAG, "Failed to set IP_MULTICAST_TTL. errno: %d", errno);
        close(sock);
        return -1;
    }

    // Add to multicast group
    if (socket_add_ipv4_multicast_group(sock, true) < 0)
    {
        close(sock);
        return -1;
    }

    return sock; // Success
}

static void mcast_example_task(void *pvParameters)
{
    while (1)
    {
        int sock;
        sock = create_multicast_ipv4_socket();
        if (sock < 0)
        {
            ESP_LOGE(TAG, "Failed to create IPv4 multicast socket");
            vTaskDelay(5 / portTICK_PERIOD_MS); // Wait a bit before retrying
            continue;
        }

        struct sockaddr_in sdestv4 = {
            .sin_family = PF_INET,
            .sin_port = htons(UDP_PORT),
        };
        inet_aton(MULTICAST_IPV4_ADDR, &sdestv4.sin_addr.s_addr);

        for (int i = 0; i < 100; ++i)
        {
            const char sendfmt[] = "Multicast #%d sent by ESP32\n";
            char sendbuf[48];
            int len = snprintf(sendbuf, sizeof(sendbuf), sendfmt, i);
            if (len > sizeof(sendbuf))
            {
                ESP_LOGE(TAG, "Overflowed multicast sendfmt buffer!!");
                continue;
            }

            ESP_LOGI(TAG, "Sending to IPV4 multicast address %s:%d...", MULTICAST_IPV4_ADDR, UDP_PORT);
            int err = sendto(sock, sendbuf, len, 0, (struct sockaddr *)&sdestv4, sizeof(sdestv4));
            ESP_LOGI(TAG, "Sending: %s", sendbuf);
            if (err < 0)
            {
                ESP_LOGE(TAG, "IPV4 sendto failed. errno: %d", errno);
            }

            // Delay 100ms before sending the next message
            vTaskDelay(100 / portTICK_PERIOD_MS);
        }

        ESP_LOGI(TAG, "Shutting down socket after sending 100 messages...");
        shutdown(sock, 0);
        close(sock);

        // Optionally, add a longer delay here if you want some time before starting the next batch of 100 messages
        vTaskDelay(1000 / portTICK_PERIOD_MS); // 1-second delay before restarting the loop
        vTaskDelete(NULL);
    }
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

    xTaskCreate(&mcast_example_task, "mcast_task", 4096, NULL, 5, NULL);
}
