#include <stdio.h>
#include <wifi_connect.h>
#include <nvs_flash.h>
#include <esp_log.h>
#include <esp_http_server.h>
#include <mdns.h>

static const char *SERVER_TAG = "[SERVER]";

/* event handler for server */
static esp_err_t on_default_url(httpd_req_t *req)
{
    ESP_LOGI(SERVER_TAG,"URL %s:", req->uri);
    httpd_resp_sendstr(req, "<i><b> Hello this is ESP32 Server :) <b><i>");
    return ESP_OK;
}

static void start_mdns_service()
{
    mdns_init();
    mdns_hostname_set("esp-server");
    mdns_instance_name_set("bla-bla-bla");
}

/* init our server */
static void init_server()
{
    httpd_handle_t server = NULL;
    httpd_config_t serverConfig = HTTPD_DEFAULT_CONFIG();

    httpd_start(&server, &serverConfig);

    httpd_uri_t default_url = {
        .uri = "/",
        .method = HTTP_GET,
        .handler = on_default_url
    };
    httpd_register_uri_handler(server, &default_url);
}

void app_main(void)
{
    nvs_flash_init();
    wifi_init();
    wifi_connect_sta("Segin","2003sejin",10000);
    start_mdns_service();
    init_server();
}
