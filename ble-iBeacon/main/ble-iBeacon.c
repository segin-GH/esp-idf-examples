#include <stdio.h>
#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_nimble_hci.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "services/gap/ble_svc_gap.h"

#define TAG "BLE"

void bleprph_on_sync(void)
{
    ESP_LOGI(TAG, "BLE Host synced");
    ble_addr_t addr;
    ble_hs_id_infer_auto(1, &addr);
    ble_hs_id_set_rnd(addr.val);

    /* Simple Beacon */
    /* TODO: add a better way for uuid */

    uint8_t uuid128[16];
    memset(uuid128, 0x11, sizeof(uuid128));
    ble_ibeacon_set_adv_data(uuid128, 2, 10, -50);

    /* Eddystone Beacon */
    /*     struct ble_hs_adv_fields fields = (struct ble_hs_adv_fields){0};
        ble_eddystone_set_adv_data_url(
            &fields,
            BLE_EDDYSTONE_URL_SCHEME_HTTPS,
            "www.google.com",
            strlen("www.google.com"),
            BLE_EDDYSTONE_URL_SUFFIX_COM,
            -30); */

    struct ble_gap_adv_params adv_params = (struct ble_gap_adv_params){0};
    ble_gap_adv_start(BLE_OWN_ADDR_RANDOM, NULL, BLE_HS_FOREVER, &adv_params, NULL, NULL);
}

void bleHostTask(void *param)
{
    ESP_LOGI(TAG, "BLE Host Task started");
    nimble_port_run();
}

void app_main(void)
{
    esp_err_t err;

    // Initialize NVS
    err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
        ESP_LOGI(TAG, "NVS init: %s", esp_err_to_name(err));
    }

    err = esp_nimble_hci_and_controller_init();
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "esp_nimble_hci_and_controller_init() failed: %s", esp_err_to_name(err));
        return;
    }

    nimble_port_init();

    ble_hs_cfg.sync_cb = bleprph_on_sync;
    nimble_port_freertos_init(bleHostTask);
}
