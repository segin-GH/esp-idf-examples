#include <stdio.h>
#include <string.h>
#include <esp_log.h>
#include <esp_http_server.h>
#include <mdns.h>
#include <esp_spiffs.h>
#include <nvs_flash.h>
#include "ota.h"
#include "wifi_connect.h"

/* A macro for logging  */
static const char *SERVER_TAG = "[SERVER]";
static const char *OTA_TAG = "[OTA]";

/* server handle */
static httpd_handle_t server = NULL;

/* event handler for the default URL */
static esp_err_t on_default_url(httpd_req_t *req)
{
    /* LOG URL of req */
    ESP_LOGI(SERVER_TAG, "URL %s: ", req->uri);

    /* config spiffs for file reading*/
    esp_vfs_spiffs_conf_t esp_vfs_spiffs_config = {
        .base_path = "/spiffs",
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = true
    };
    esp_vfs_spiffs_register(&esp_vfs_spiffs_config);

    /* determine the file path based on the URL */
    char path[600];

    /* if default url load the html file from spiffs*/
    if(strcmp(req->uri, "/") ==0)
        strcpy(path, "/spiffs/index.html");
    else
        sprintf(path,"/spiffs%s",req->uri);

    /* determine the file extension */
    char *ext = strrchr(path,'.');
    
    /* Set the MINE type based on the file extension */
    if(strcmp(ext, ".css") == 0)
        httpd_resp_set_type(req, "text/css");

    if(strcmp(ext, ".js") == 0)
        httpd_resp_set_type(req, "text/javascript");
    
    if(strcmp(ext, ".png") == 0)
        httpd_resp_set_type(req, "image/png");

    FILE *file = fopen(path, "r");
    if(file == NULL)
    {
        /* if file does not open send 404 error */
        httpd_resp_send_404(req);
        esp_vfs_spiffs_unregister(NULL);
        return ESP_FAIL;
    }
    
    /* Read the file in chunks and send it to client */
    char lineRead[256];
    while(fgets(lineRead, sizeof(lineRead), file))
        httpd_resp_sendstr_chunk(req, lineRead);

    /* send NULL to say everything has been send */
    httpd_resp_sendstr_chunk(req, NULL);

    /* close the file and unregister the spiffs */
    fclose(file);
    esp_vfs_spiffs_unregister(NULL);
    return ESP_OK;
}

/* function to start mDNS service */
static void start_mdns_service()
{
    /* initialize mDNS */
    mdns_init();
    /* set host name */
    mdns_hostname_set("esp-server");        /* new addr will be http://esp-server.local/ */
    /* set instance name */
    mdns_instance_name_set("esp-server-hspl");
}

/* init server */
static void init_server()
{
    /* set server as default config */
    httpd_config_t serverConfig = HTTPD_DEFAULT_CONFIG();
    
    /* Set the URI matching function to use wildcards */
    serverConfig.uri_match_fn = httpd_uri_match_wildcard;

    /* start the server */
    httpd_start(&server, &serverConfig);

    /* Set up a handler for requests to the default URL */
    httpd_uri_t default_url = {
        .uri = "/*",
        .method = HTTP_GET,
        .handler = on_default_url
    };
    httpd_register_uri_handler(server, &default_url);
}

void init_ota(void)
{

    ESP_LOGI(OTA_TAG,"INVOKING OTA");
    nvs_flash_init();
    wifi_init();
    wifi_connect_sta("Segin", "2003sejin", 10000);
    start_mdns_service();
    init_server();
}