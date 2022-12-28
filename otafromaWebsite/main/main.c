#include <stdio.h>
#include <ota.h>


void app_main(void)
{
    set_mDNS_name(NULL); // new addr will be http://pdb-esp.local/
    init_ota();
}