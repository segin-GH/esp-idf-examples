// server_main.c with mDNS
#include "esp_http_server.h"
#include "esp_log.h"
#include "mdns.h"
#include "wifi_connect.h"
#include "nvs_flash.h"

static const char *TAG = "HTTP_SERVER";

// your get handler hasn't changed, still doing the bare minimum
esp_err_t get_handler(httpd_req_t *req)
{
    const char resp[] = "Hello from your ESP server. Now with mDNS magic!";
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

httpd_uri_t uri_get = {
    .uri = "/hello",
    .method = HTTP_GET,
    .handler = get_handler,
    .user_ctx = NULL};

void start_mdns_service()
{
    // initialize mDNS service
    ESP_ERROR_CHECK(mdns_init());
    // set mDNS hostname (replace 'esp-server' with your desired hostname)
    ESP_ERROR_CHECK(mdns_hostname_set("esp-serve"));
    // set default mDNS instance name
    ESP_ERROR_CHECK(mdns_instance_name_set("ESP32 HTTP Server"));

    // add service to mDNS-SD
    ESP_ERROR_CHECK(mdns_service_add("ESP-HTTP-Server", "_http", "_tcp", 80, NULL, 0));
}

void start_server(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    // wild card

    config.uri_match_fn = httpd_uri_match_wildcard;

    if (httpd_start(&server, &config) == ESP_OK)
    {
        httpd_register_uri_handler(server, &uri_get);
    }
    ESP_LOGI(TAG, "Server started on /hello");

    // don't forget to start the mDNS service
    start_mdns_service();
}

wifi_cred_t wifi_cred = {

    .wifi_name = "HACKLAB_2G",
    .wifi_pass = "HACK@LAB",
    .k_timeout = 10000,
};

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

    start_server();
}
