/* ESP BLE READ ONLY */
#include <stdio.h>
#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_nimble_hci.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"

#define TAG "BLE"

uint8_t ble_addr_type;
void ble_app_advertise(void);

static int device_info(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    os_mbuf_append(ctxt->om, "ESP32-BLE", strlen("ESP32-BLE"));
    return 0;
}

static const struct ble_gatt_svc_def gatt_svr_svcs[] = {
    {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID16_DECLARE(0x180A),
        .characteristics = (struct ble_gatt_chr_def[]){
            {
                .uuid = BLE_UUID16_DECLARE(0x2A29),
                .flags = BLE_GATT_CHR_F_READ,
                .access_cb = device_info,
            },
            {
                0, /* No more characteristics in this service */
            },
        },
    },
    {
        0, /* No more services */
    }

};

static int ble_gap_event(struct ble_gap_event *event, void *arg)
{
    switch (event->type)
    {
    case BLE_GAP_EVENT_CONNECT:
        ESP_LOGI(TAG, "BLE_GAP_EVENT_CONNECT %s", event->connect.status == 0 ? "OK" : "ERROR");
        if (event->connect.status != 0)
        {
            ble_app_advertise();
        }
        break;

    case BLE_GAP_EVENT_DISCONNECT:
        ESP_LOGI(TAG, "BLE_GAP_EVENT_DISCONNECT");
        ble_app_advertise();
        break;

    case BLE_GAP_EVENT_ADV_COMPLETE:
        ESP_LOGI(TAG, "BLE_GAP_EVENT_ADV_COMPLETE");
        ble_app_advertise();
        break;

    case BLE_GAP_EVENT_SUBSCRIBE:
        ESP_LOGI(TAG, "BLE_GAP_EVENT_SUBSCRIBE");
        break;

    default:
        ESP_LOGI(TAG, "Unknown GAP event: %d", event->type);
        break;
    }
    return 0;
}

void ble_app_advertise(void)
{
    struct ble_hs_adv_fields fields;
    memset(&fields, 0, sizeof(fields));

    fields.flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_DISC_LTD;
    fields.tx_pwr_lvl_is_present = 1;
    fields.tx_pwr_lvl = BLE_HS_ADV_TX_PWR_LVL_AUTO;

    fields.name = (uint8_t *)ble_svc_gap_device_name();
    fields.name_len = strlen(ble_svc_gap_device_name());
    fields.name_is_complete = 1;

    ble_gap_adv_set_fields(&fields);

    struct ble_gap_adv_params adv_params;
    memset(&adv_params, 0, sizeof(adv_params));
    adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
    adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;

    ble_gap_adv_start(ble_addr_type, NULL, BLE_HS_FOREVER, &adv_params, ble_gap_event, NULL);
}

void bleprph_on_sync(void)
{
    ESP_LOGI(TAG, "BLE Host synced");
    ble_hs_id_infer_auto(0, &ble_addr_type);
    ble_app_advertise();
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

    // Initialize NimBLE
    err = esp_nimble_hci_and_controller_init();
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "esp_nimble_hci_and_controller_init() failed: %s", esp_err_to_name(err));
        return;
    }

    nimble_port_init();
    ble_svc_gap_device_name_set("ESP32 BLE");
    ble_svc_gap_init();
    ble_svc_gatt_init();
    ble_gatts_count_cfg(gatt_svr_svcs);
    ble_gatts_add_svcs(gatt_svr_svcs);

    ble_hs_cfg.sync_cb = bleprph_on_sync;
    nimble_port_freertos_init(bleHostTask);
}
