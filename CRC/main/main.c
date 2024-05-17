#include "esp_crc.h"
#include <stdio.h>
#include <string.h>

void calculate_crc32()
{
    const char *data = "change";
    size_t length = strlen(data);

    uint32_t crc = esp_crc32_le(0, (const uint8_t *)data, length);

    printf("crc32 of '%s' is 0x%08x\n", data, crc);
}

void app_main()
{
    calculate_crc32();
}
