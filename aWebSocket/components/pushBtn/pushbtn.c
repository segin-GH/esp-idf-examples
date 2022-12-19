#include "pushbtn.h"

#define BTN 0

static xSemaphoreHandle btn_sem;

static void IRAM_ATTR on_btn_pushed(void *args)
{
    xSemaphoreGiveFromISR(btn_sem,NULL);
}

static void when_btn_pushed(void *args)
{
    for(;;)
    {
        xSemaphoreTake(btn_sem, portMAX_DELAY);
        cJSON *payload = cJSON_CreateObject();
        cJSON_AddBoolToObject(payload, "btn_state", gpio_get_level(BTN));
        char *msg = cJSON_Print(payload);
        printf("msg: %s\n", msg);

        cJSON_Delete(payload);
        free(msg);
    }
}


void init_btn_onboard()
{
    xTaskCreatePinnedToCore(
        when_btn_pushed,
        "when_btn_pushed",
        2048,
        NULL,
        5,
        NULL,
        APP_CPU_NUM
    );
    btn_sem = xSemaphoreCreateBinary();
    
    gpio_pad_select_gpio(BTN);
    gpio_set_direction(BTN, GPIO_MODE_INPUT);
    gpio_set_intr_type(BTN, GPIO_INTR_ANYEDGE);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(BTN, on_btn_pushed, NULL);
}