#include <stdio.h>
#include <ota.h>
#include "wifi_connect.h"

wifi_cred_t wifi_cred = {
    .wifi_name = "Segin",
    .wifi_pass = "sejin2003",
    .k_timeout = 10000
};

void app_main(void)
{
    wifi_connect_sta(&wifi_cred);
    set_mDNS_name(NULL); // new addr will be http://pdb-esp.local/
    init_ota();
}