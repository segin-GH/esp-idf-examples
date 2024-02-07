// slave code - because why innovate when you can replicate

#include <stdio.h>
#include "esp_wifi.h"
#include "esp_http_server.h"
#include "esp_log.h"

static const char *TAG = "Slave";

esp_err_t hi_handler(httpd_req_t *req) {
    char buf[100];
    int ret, remaining = req->content_len;

    while (remaining > 0) {
        // Assuming 'hi' fits in buffer, else you'd loop
        if ((ret = httpd_req_recv(req, buf, MIN(remaining, sizeof(buf)))) <= 0) {
            if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
                continue;
            }
            return ESP_FAIL;
        }
        remaining -= ret;
    }
    ESP_LOGI(TAG, "Received 'hi', how quaint.");
    httpd_resp_send(req, "Hello, master", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

httpd_uri_t hi = {
    .uri = "/hi",
    .method = HTTP_METHOD_POST,
    .handler = hi_handler,
    .user_ctx = NULL
};

void start_webserver(void) {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;
    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_register_uri_handler(server, &hi);
    }
    ESP_LOGI(TAG, "Server started, bowing to your 'hi's.");
}

void app_main() {
    // Init Wi-Fi
    ESP_LOGI(TAG, "Setting up as a 'slave', how dignified...");
    // You'd actually set up Wi-Fi here, but let's skip that part
    start_webserver();
}

