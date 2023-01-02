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
    nvs_flash_init();
    wifi_init();
    wifi_connect_sta(&wifi_cred);
    set_spiffs_path("/spiffs/setings.txt");
    set_mDNS_name(NULL); // new addr will be http://pdb-esp.local/
    init_ota();
}