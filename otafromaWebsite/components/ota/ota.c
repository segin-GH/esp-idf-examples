#include <stdio.h>
#include <string.h>
#include <esp_log.h>
#include <stdbool.h>
#include <driver/gpio.h>
#include <esp_http_server.h>
#include <mdns.h>
#include <esp_spiffs.h>
#include <nvs_flash.h>
#include "ota.h"
#include "wifi_connect.h"

static const char *SERVER_TAG = "[SERVER]";
static httpd_handle_t server = NULL;

static esp_err_t on_default_url(httpd_req_t *req)
{
    ESP_LOGI(SERVER_TAG, "URL %s: ", req->uri);

    esp_vfs_spiffs_conf_t esp_vfs_spiffs_config = {
        .base_path = "/spiffs",
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = true
    };
    esp_vfs_spiffs_register(&esp_vfs_spiffs_config);
    
    
    char path[600];
    if(strcmp(req->uri, "/") ==0 )
    {
        strcpy(path, "/spiffs/index.html");
    }
    else
    {
        sprintf(path,"/spiffs%s",req->uri);
    }
    char *ext = strrchr(path,'.');
    // for another file
    if(strcmp(ext, ".css") == 0)
    {
        httpd_resp_set_type(req, "text/css");
    }
    if(strcmp(ext, ".js") == 0)
    {
        httpd_resp_set_type(req, "text/javascript");
    }
    if(strcmp(ext, ".png") == 0)
    {
        httpd_resp_set_type(req, "image/png");
    }

    FILE *file = fopen(path, "r");
    if(file == NULL)
    {
        httpd_resp_send_404(req);
        esp_vfs_spiffs_unregister(NULL);
        return ESP_OK;
    }
    char lineRead[256];
    while(fgets(lineRead, sizeof(lineRead), file))
    {
        httpd_resp_sendstr_chunk(req, lineRead);
    }
    
    httpd_resp_sendstr_chunk(req, NULL);
    esp_vfs_spiffs_unregister(NULL);
    return ESP_OK;
}

static void start_mdns_service()
{
    mdns_init();
    mdns_hostname_set("esp-server");
    mdns_instance_name_set("esp-server-hspl");
    /* new addr will look something like this http://esp-server.local/ */
}

/* init our server */
static void init_server()
{
    httpd_config_t serverConfig = HTTPD_DEFAULT_CONFIG();
    serverConfig.uri_match_fn = httpd_uri_match_wildcard;

    httpd_start(&server, &serverConfig);

    httpd_uri_t default_url = {
        .uri = "/*",
        .method = HTTP_GET,
        .handler = on_default_url
    };
    httpd_register_uri_handler(server, &default_url);
}

void init_ota(void)
{
    printf("invoking ota\n");
    nvs_flash_init();
    wifi_init();
    wifi_connect_sta("Segin", "2003sejin", 10000);
    start_mdns_service();
    init_server();
}