#include <stdio.h>
#include <ota.h>
#include <nvs_flash.h>
#include "wifi_connect.h"


wifi_cred_t wifi_cred = {
    .wifi_name = "Segin",
    .wifi_pass = "2003sejin",
    .k_timeout = 10000
};

void app_main(void)
{
    /* config spiffs for file reading*/

    // esp_err_t err;
    // esp_vfs_spiffs_conf_t esp_vfs_spiffs_config = {
    //     .base_path = "/spiffs",
    //     .partition_label = NULL,
    //     .max_files = 1,
    //     .format_if_mount_failed = true
    // };
    // err = esp_vfs_spiffs_register(&esp_vfs_spiffs_config);
    // if(err != ESP_OK)
    //     TASK_ERROR_FATALE("Unable to register spiffs", SPIFFS_OTA, err);
    
    nvs_flash_init();
    wifi_init();
    wifi_connect_sta(&wifi_cred);
    set_spiffs_path("/spiffs/setings.txt");
    set_mDNS_name(NULL); // new addr will be http://pdb-esp.local/
    init_ota();
}