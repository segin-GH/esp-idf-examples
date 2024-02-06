#include <stdio.h>
#include "wifi_connect.h"
#include "esp_log.h"
#include <nvs_flash.h>
#include "esp_http_server.h"

static httpd_handle_t server = NULL;

static esp_err_t on_default_url(httpd_req_t *req)
{
    char *resp_str = "Hello World!";
    httpd_resp_send(req, resp_str, strlen(resp_str));
    return ESP_OK;
}

void app_main(void)
{
    wifi_cred_t wifi_cred = {

        .wifi_name = "HACKLAB_2G",
        .wifi_pass = "HACK@LAB",
        .k_timeout = 10000,
    };

    nvs_flash_init();
    wifi_init();

    esp_err_t err;

    err = wifi_connect_sta(&wifi_cred);
    if (err)
    {
        ESP_LOGE("WIFI CONNECT", "Failed to connect");
        vTaskDelete(NULL);
    }

    httpd_config_t serverConfig = HTTPD_DEFAULT_CONFIG();

    serverConfig.uri_match_fn = httpd_uri_match_wildcard;

    err = httpd_start(&server, &serverConfig);
    if (err)
    {
        ESP_LOGE("HTTPD", "Failed to start server");
        vTaskDelete(NULL);
    }

    httpd_uri_t default_url = {
        .uri = "/*",
        .method = HTTP_GET,
        .handler = on_default_url};
    err = httpd_register_uri_handler(server, &default_url);
}
