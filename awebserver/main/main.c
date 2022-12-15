#include <stdio.h>
#include <wifi_connect.h>
#include <nvs_flash.h>
#include <esp_log.h>
#include <esp_http_server.h>


static esp_err_t default_handler(httpd_req_t *req)
{
    ESP_LOGI(SERVER_TAG,"URL:", req->uri);
    httpd_resp_sendstr(req, "Hello this ESP32 Server");
}

static void init_server()
{
    httpd_handle_t server = NULL;
    httpd_config_t serverConfig = HTTPD_DEFAULT_CONFIG;

    httpd_start(&server, &ServerConfig);

    httpd_uri_t default_url = {
        .uri = "/",
        .method = HTTP_GET,
        .handler = default_handler;
    };
    httpd_register_uri_handler(server, &default_handler);
}

void app_main(void)
{
    nvs_flash_init();
    wifi_init();
    wifi_connect_sta("Segin","2003sejin",10000);
}
