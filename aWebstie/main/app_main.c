#include <stdio.h>
#include <string.h>
#include "wifi_connect.h"
#include "ledtoggle.h"
#include "pushbtn.h"
#include <nvs_flash.h>
#include <esp_log.h>
#include <esp_http_server.h>
#include <mdns.h>
#include <stdbool.h>
#include <driver/gpio.h>
#include <cJSON.h>
#include <esp_spiffs.h>

static const char *SERVER_TAG = "[SERVER]";
static httpd_handle_t server = NULL;

/* event handler for server */
static esp_err_t on_default_url(httpd_req_t *req)
{
    ESP_LOGI(SERVER_TAG,"URL %s:", req->uri);

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
    // for another file

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

static esp_err_t on_toggle_led(httpd_req_t *req)
{
    char buffer[100];
    memset(buffer, 0, sizeof(buffer));

    /* TODO check req->content_len is < sizeof buffer  */
    httpd_req_recv(req, buffer, req->content_len);
    cJSON *payload = cJSON_Parse(buffer);
    cJSON *is_on_json = cJSON_GetObjectItem(payload, "is_on");
    bool is_on = cJSON_IsTrue(is_on_json);
    cJSON_Delete(payload);
    toggle_led(is_on);

    httpd_resp_set_status(req, "204 NO CONTENT");
    httpd_resp_send(req, NULL, 0);
    httpd_resp_sendstr(req, "<i><b> Hello this is ESP32 Server :) <b><i>");
    return ESP_OK;
}

/* web socket */

#define WS_MAX_SIZE 1024
int client_session_id;

static esp_err_t on_web_socket_url(httpd_req_t *req)
{
    client_session_id = httpd_req_to_sockfd(req);
    if(req -> method == HTTP_GET)
        return ESP_OK;
    httpd_ws_frame_t ws_pkt;
    memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));
    ws_pkt.type = HTTPD_WS_TYPE_TEXT;
    ws_pkt.payload = malloc(WS_MAX_SIZE);
    httpd_ws_recv_frame(req, &ws_pkt, WS_MAX_SIZE);
    printf("payload: %.*s\n",ws_pkt.len, ws_pkt.payload);
    free(ws_pkt.payload);

    char *response = "connected ok :) ";

    httpd_ws_frame_t ws_response = {
        .final = true,
        .fragmented = false,
        .type = HTTPD_WS_TYPE_TEXT,
        .payload = (uint8_t *)response,
        .len = strlen(response),
    };
   return httpd_ws_send_frame(req, &ws_response);
}

esp_err_t send_ws_message(char *msg)
{
    if(!client_session_id)
    {
        ESP_LOGE("<WebSocket>", "no client_session_id");
        return -1;
    }

    httpd_ws_frame_t ws_message ={
        .final = true,
        .fragmented = false,
        .len = strlen(msg),
        .payload = (uint8_t *)msg,
        .type = HTTPD_WS_TYPE_TEXT
    };
   return httpd_ws_send_frame_async(server, client_session_id, &ws_message);
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
    httpd_config_t serverConfig = HTTPD_DEFAULT_CONFIG();

    httpd_start(&server, &serverConfig);

    httpd_uri_t default_url = {
        .uri = "/",
        .method = HTTP_GET,
        .handler = on_default_url
    };
    httpd_register_uri_handler(server, &default_url);
    
    httpd_uri_t toggle_led_url = {
        .uri = "/api/tog",
        .method = HTTP_POST,
        .handler = on_toggle_led
    };
    httpd_register_uri_handler(server, &toggle_led_url);
    
    httpd_uri_t web_socket_url = {
        .uri = "/ws",
        .method = HTTP_GET,
        .handler = on_web_socket_url,
        .is_websocket = true
    };
    httpd_register_uri_handler(server, &web_socket_url);
}


void app_main(void)
{
    nvs_flash_init();
    wifi_init();
    init_btn_onboard();
    init_led_as_output(2);
    wifi_connect_sta("Segin","2003sejin",10000);
    start_mdns_service();
    init_server();
}