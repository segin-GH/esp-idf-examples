#include <stdio.h>
#include <string.h>
#include <esp_heap_caps.h>
#include <freertos/FreeRTOS.h>
#include <esp_log.h>


# define TAG "Memory"

void app_main(void)
{ 
    ESP_LOGI(TAG, "freeRtos way : %d", xPortGetFreeHeapSize());

    int DRam = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    int IRam = heap_caps_get_free_size(MALLOC_CAP_32BIT);

    int DramFree = heap_caps_get_largest_free_block(MALLOC_CAP_8BIT);
    int stackMem = uxTaskGetStackHighWaterMark(NULL);

    ESP_LOGI(TAG, "DRam \t\t%d", DRam);
    ESP_LOGI(TAG, "IRam \t\t%d",IRam);

}
