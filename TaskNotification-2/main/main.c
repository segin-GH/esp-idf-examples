#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static TaskHandle_t receiverHandler = NULL;

void sender(void *params)
{
  while (true)
  {
    xTaskNotify(receiverHandler, (1 << 0), eSetValueWithOverwrite);
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    xTaskNotify(receiverHandler, (0x02), eSetValueWithOverwrite);
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    xTaskNotify(receiverHandler, (0b0011), eSetValueWithOverwrite);
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    xTaskNotify(receiverHandler, (0x04), eSetValueWithOverwrite);
    vTaskDelay(5000 / portTICK_PERIOD_MS);
  }
}

void receiver(void *params)
{
  uint state;
  while (true)
  {
    xTaskNotifyWait(0, 0, &state, portMAX_DELAY);
    printf("received state %d times\n", state);
  }
}

void app_main(void)
{
  xTaskCreate(
      &receiver,
      "sender",
      2048,
      NULL,
      2,
      &receiverHandler);

  xTaskCreate(
      &sender,
      "receiver",
      2048,
      NULL,
      2,
      NULL);
}