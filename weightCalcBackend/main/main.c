#include <stdio.h>
#include <string.h>
#include "wifi_connect.h"
#include <nvs_flash.h>
#include <esp_log.h>
#include <esp_http_server.h>
#include <mdns.h>
#include <stdbool.h>
#include <driver/gpio.h>
#include <cJSON.h>
#include <driver/uart.h>

#define uart_two_txd 17
#define uart_two_rxd 16
#define Buf_size 1024

static const char *SERVER_TAG = "[SERVER]";
static httpd_handle_t server = NULL;

#define DEBUG

static void get_weight(char *buffer)
{
    // get data from arduino
    // process the data
    cJSON *json = cJSON_CreateObject();
    cJSON_AddStringToObject(json, "MSG", "SUCCESSFUL");
    cJSON_AddStringToObject(json, "Weight", "1.2");

    char *json_string = cJSON_Print(json);
#ifdef DEBUG
    printf("JSON string: %s\n", json_string);
#endif /* DEBUG */
    strcpy(buffer, json_string);
}

static esp_err_t on_default_url(httpd_req_t *req)
{
    ESP_LOGI(SERVER_TAG, "URL %s:", req->uri);
    char data[1000];
    memset(data, 0, sizeof(data));
    get_weight(data);

    // put a check if the data is available
    httpd_resp_sendstr(req, data);
    return ESP_OK;
}

static esp_err_t print_banner_handler(httpd_req_t *req)
{
    char buffer[1000];
    memset(buffer, 0, sizeof(buffer));
    httpd_req_recv(req, buffer, req->content_len);
    printf("%s \n", buffer);

    uart_write_bytes(UART_NUM_2, buffer, 5);

    vTaskDelay(100 / portTICK_PERIOD_MS);
    httpd_resp_set_status(req, "204 NO CONTENT");
    httpd_resp_send(req, NULL, 0);

    return ESP_OK;
}

static void init_server()
{
    httpd_config_t ServerConfig = HTTPD_DEFAULT_CONFIG();
    httpd_start(&server, &ServerConfig);

    httpd_uri_t default_uri = {
        .uri = "/get-weight",
        .method = HTTP_GET,
        .handler = on_default_url};
    httpd_register_uri_handler(server, &default_uri);

    httpd_uri_t print_banner = {
        .uri = "/print-banner",
        .method = HTTP_POST,
        .handler = print_banner_handler};
    httpd_register_uri_handler(server, &print_banner);
}

static void start_mdns_service()
{
    mdns_init();
    mdns_hostname_set("esp-server");
    mdns_instance_name_set("bla-bla-bla");
}

void app_main(void)
{
    nvs_flash_init();
    wifi_init();
    start_mdns_service();
    wifi_connect_sta("HACKLAB_2G", "HACK@LAB", 10000);
    init_server();

    uart_config_t uart_two_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };

    uart_param_config(UART_NUM_2, &uart_two_config);

    uart_set_pin(UART_NUM_2, uart_two_txd, uart_two_rxd, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    uart_driver_install(UART_NUM_2, Buf_size * 2, 0, 0, NULL, 0);
}
