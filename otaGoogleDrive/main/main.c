#include <stdio.h>
#include <string.h>
#include <esp_wifi.h>
#include <esp_event.h>
#include <nvs_flash.h>
#include <esp_http_client.h>
#include <esp_https_ota.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <esp_log.h>
#include <driver/gpio.h>
#include <wifi_connect.h>

const char *wifiName = "Segin";
const char *password = "2003sejin";

const char *otaTag = "<OTA>";

const int softwareVersion = 5;

xSemaphoreHandle ota_sem;

extern const uint8_t server_cert_pem_start[] asm("_binary_google_cer_start");

esp_err_t client_event_handler(esp_http_client_event_t *event)
{
    return ESP_OK;
}

void run_ota(void *parms)
{
    while(true)
    {
        xSemaphoreTake(ota_sem, portMAX_DELAY);
        ESP_LOGI(otaTag, "Invoking OTA");

        nvs_flash_init();
        esp_event_loop_create_default();
        wifi_init();
        wifi_connect_sta(wifiName, password, 10000);
        vTaskDelay(1000/portMAX_DELAY);

// https://drive.google.com/u/0/uc?id=1_nKkU2iqrFe_A5swSz8sitzL4QaiNcey&export=download
// https://drive.google.com/u/0/uc?id=1_nKkU2iqrFe_A5swSz8sitzL4QaiNcey&export=download

        esp_http_client_config_t clientConfig = {
            .url = "https://drive.google.com/u/0/uc?id=1_nKkU2iqrFe_A5swSz8sitzL4QaiNcey&export=download", // ota location
            .event_handler = client_event_handler,
            .cert_pem = (char *)server_cert_pem_start
        };

        if(esp_https_ota(&clientConfig) == ESP_OK)
        {
            ESP_LOGI(otaTag, "OTA FLASH successfully for version %i.", softwareVersion);
            printf("restarting in 5 sec\n");
            esp_restart();
        }
        ESP_LOGE(otaTag, "FAILED TO UPDATE THROUGH OTA");
    }
}


void on_btn_pushed(void *parms)
{
    xSemaphoreGiveFromISR(ota_sem, pdFALSE);
}

void app_main(void)
{
    printf("SOFTWARE VERSION %i\n", softwareVersion);
    gpio_config_t gpioConfig ={
        .pin_bit_mask = 1ULL << GPIO_NUM_0,
        .mode = GPIO_MODE_DEF_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_NEGEDGE
    };
    gpio_config(&gpioConfig);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(GPIO_NUM_0, on_btn_pushed, NULL);

    ota_sem = xSemaphoreCreateBinary();

    xTaskCreatePinnedToCore(
        run_ota,
        "run_ota",
        1024*8,
        NULL,
        2,
        NULL,
        APP_CPU_NUM
    );

}