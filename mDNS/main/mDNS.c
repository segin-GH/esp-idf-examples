#include <stdio.h>
#include <wifi_connect.h>
#include <nvs_flash.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <nvs_flash.h>
#include <esp_err.h>
#include "esp_http_server.h"
#include "mdns.h"

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

const char *TAG = "HTTPD";

wifi_cred_t wifi_cred = {

    .wifi_name = "HACKLAB_2G",
    .wifi_pass = "HACK@LAB",
    .k_timeout = 10000,
};
static httpd_handle_t server = NULL;

static esp_err_t on_default_url(httpd_req_t *req)
{
    char *resp_str = "{\"json\":\"string\"}";
    httpd_resp_send(req, resp_str, strlen(resp_str));
    return ESP_OK;
}

static esp_err_t on_default_post(httpd_req_t *req)
{
    // Buffer to store the request body
    char buf[1000];
    // Initialize buffer to 0 to ensure it's clean before starting
    memset(buf, 0, sizeof(buf));

    int ret;
    // Variable to keep track of the remaining size of the request body
    int remaining = req->content_len;

    // Temporary buffer to store chunks of the request body
    char temp_buf[sizeof(buf)];

    while (remaining > 0)
    {
        if ((ret = httpd_req_recv(req, temp_buf, MIN(remaining, sizeof(temp_buf)))) <= 0)
        {
            if (ret == HTTPD_SOCK_ERR_TIMEOUT) // If timeout, try again
            {
                continue;
            }
            return ESP_FAIL;
        }
        strncat(buf, temp_buf, MIN(ret, sizeof(buf) - strlen(buf) - 1));
        remaining -= ret;
    }

    printf("%s\n", buf);
    httpd_resp_send(req, "Hello, master", HTTPD_RESP_USE_STRLEN);

    return ESP_OK;
}

void app_main(void)
{
    nvs_flash_init();
    wifi_init();
    mdns_init();

    mdns_hostname_set("esp32");
    mdns_instance_name_set("ESP32 Web Server");
    mdns_service_add(NULL, "_http", "_tcp", 80, NULL, 0);

    esp_err_t err;

    err = wifi_connect_sta(&wifi_cred);
    if (err)
    {
        ESP_LOGE("WIFI CONNECT", "Failed to connect");
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
        .uri = "/get",
        .method = HTTP_GET,
        .handler = on_default_url};

    err = httpd_register_uri_handler(server, &default_url);
    if (err)
    {
        ESP_LOGE("HTTPD", "Failed to register default url");
        vTaskDelete(NULL);
    }

    // post request

    httpd_uri_t post_url = {
        .uri = "/post",
        .method = HTTP_POST,
        .handler = on_default_post};

    err = httpd_register_uri_handler(server, &post_url);
    if (err)
    {
        ESP_LOGE("HTTPD", "Failed to register post url");
        vTaskDelete(NULL);
    }
}
