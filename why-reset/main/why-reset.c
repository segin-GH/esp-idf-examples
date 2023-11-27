#include <stdio.h>
#include "esp_system.h"

void app_main(void)
{
    esp_reset_reason_t reason = esp_reset_reason();
    switch (reason)
    {
    case ESP_RST_POWERON:
        printf("Power-on reset\n");
        break;
    case ESP_RST_EXT:
        printf("External reset\n");
        break;
    case ESP_RST_SW:
        printf("Software reset\n");
        break;
    case ESP_RST_PANIC:
        printf("Exception/panic reset\n");
        break;
    case ESP_RST_INT_WDT:
        printf("Interrupt watchdog reset\n");
        break;
    case ESP_RST_TASK_WDT:
        printf("Task watchdog reset\n");
        break;
    case ESP_RST_WDT:
        printf("Other watchdog reset\n");
        break;
    case ESP_RST_DEEPSLEEP:
        printf("Deep sleep reset\n");
        break;
    case ESP_RST_BROWNOUT:
        printf("Brownout reset\n");
        break;
    case ESP_RST_SDIO:
        printf("SDIO reset\n");
        break;

    default:
        printf("Unknown reset reason\n");
        break;
    }
}