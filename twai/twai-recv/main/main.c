/* TODO Track src id of each casnode with ttl(time to live) */
/* TODO implement Time to live for each node */
/* TODO use a struct rather than a 2d array */
/* TODO Check if the cas sum is correct*/
/* TODO gen a crc */

#include <stdio.h>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "esp_log.h"
#include "esp_system.h"
#include "driver/twai.h"
#include "stdbool.h"
#include <freertos/queue.h>

#define TAG "twai"

// #define TX_PIN GPIO_NUM_5
// #define RX_PIN GPIO_NUM_4

#define TX_PIN GPIO_NUM_27
#define RX_PIN GPIO_NUM_14

/* TODO should it be 16 or 15 ? */
#define MAX_NUM_CAS 16

xQueueHandle twaiSndQueue;
static uint8_t twaiSndQueueLen = 5;

uint16_t casnode_list[4];
uint8_t num_of_cas_counter = 0;
bool add_cas_to_list = true;
uint8_t default_src_id = 30;

/* this is where each device data will be stored */
typedef struct
{
    uint16_t cas_uid;
    uint8_t src_id;
    uint8_t time_to_live;
} Cas_id_t;

Cas_id_t cas_id_array[MAX_NUM_CAS];

typedef union
{
    uint32_t num;
    uint8_t bytes[2];
} Cas_uid;
Cas_uid uid;

typedef union
{
    uint32_t num;
    uint8_t bytes[3];
} Cas_sum;
Cas_sum cas_sum;

void print_can_msg_in_cool_8t(uint8_t array[], int num_of_element)
{
    int i = 0;
    printf("[");
    for (i = 0; i < num_of_element; i++)
    {
        if (i == (num_of_element - 1))
        {
            printf("0x%02x", array[i]);
            break;
        }
        printf("0x%02x, ", array[i]);
    }
    printf("]\n");
}

void print_can_msg_in_cool_16t(uint16_t array[], int num_of_element)
{
    int i = 0;
    printf("[");
    for (i = 0; i < num_of_element; i++)
    {
        if (i == (num_of_element - 1))
        {
            printf("0x%02x", array[i]);
            break;
        }
        printf("0x%02x, ", array[i]);
    }
    printf("]\n");
}

void gen_can_msg_for_ack(uint16_t uid, Cas_sum cas_sum, uint8_t src_id)
{

    uint16_t can_message_array[9] = {0};

    Cas_uid cas_uid;
    cas_uid.num = uid;

    printf("Sending ack to CAS of %i \n", uid);
    can_message_array[0] = 0x112;
    can_message_array[1] = cas_uid.bytes[0];
    can_message_array[2] = cas_uid.bytes[1];
    can_message_array[3] = cas_sum.bytes[0];
    can_message_array[4] = cas_sum.bytes[1];
    can_message_array[5] = cas_sum.bytes[2];
    can_message_array[6] = num_of_cas_counter;
    can_message_array[7] = src_id;
    can_message_array[8] = 1;
    xQueueSend(twaiSndQueue, can_message_array, portMAX_DELAY);
}

uint32_t sum_device_id_list(int total_num_of_cas)
{
    uint32_t sum = 0;
    for (int i = 0; i < total_num_of_cas; i++)
    {
        printf("%i, ", casnode_list[i]);
        sum += casnode_list[i];
    }
    return sum;
}

void twai_receive_task(void *pvParameters)
{
    for (;;)
    {
        // Wait for a message to be received
        twai_message_t message;
        if (twai_receive(&message, pdMS_TO_TICKS(1000)) != ESP_OK)
            continue;

        printf("[0x%02x]", message.identifier);
        print_can_msg_in_cool_8t(message.data, 8);

        if (message.identifier == 0x399)
        {
            memset(&uid, 0, sizeof(uid));
            uid.bytes[0] = message.data[0];
            uid.bytes[1] = message.data[1];
            printf("DEVICE ID: %i\n", uid.num);
            
            for (int i = 0; i < MAX_NUM_CAS; i++)
            {
                // if (casnode_list[i] == uid.num)

                if (cas_id_array[i].cas_uid == uid.num)
                {
                    // cas_sum.num = sum_device_id_list(4);

                    gen_can_msg_for_ack(cas_id_array[i].cas_uid, cas_sum, cas_id_array[i].src_id);
                    printf("Device already exists ignoring\n");
                    add_cas_to_list = false;
                    break;
                }
                add_cas_to_list = true;
            }
            if (add_cas_to_list)
            {
                printf("Got a new device in bus appending to list\n");
                // casnode_list[num_of_cas_counter] = uid.num;
                printf("num of cas :: %i\n", num_of_cas_counter);
                cas_id_array[num_of_cas_counter].cas_uid = uid.num;
                cas_id_array[num_of_cas_counter].src_id = (default_src_id + num_of_cas_counter);
                printf("Current List [");
                // uint32_t sum = 0;
                for (int i = 0; i < MAX_NUM_CAS; i++)
                {
                    printf("%i, ", cas_id_array[i].cas_uid);
                    // sum += casnode_list[i];
                }
                printf("]\n");
                // cas_sum.num = sum_device_id_list(4);
                gen_can_msg_for_ack(cas_id_array[num_of_cas_counter].cas_uid, cas_sum, cas_id_array[num_of_cas_counter].src_id);
                add_cas_to_list = true;
                ++num_of_cas_counter;
                printf("Total num of CAS :: %i\n", num_of_cas_counter);

                if(num_of_cas_counter > 16)
                    num_of_cas_counter = 0;
            }
        }
    }
}

void twai_send_task(void *parms)
{
    twai_message_t msg;
    uint16_t recvDataArray[9] = {0};
    for (;;)
    {
        xQueueReceive(twaiSndQueue, recvDataArray, portMAX_DELAY);
        print_can_msg_in_cool_16t(recvDataArray, 9);
        msg.identifier = recvDataArray[0];
        msg.extd = 1;
        msg.data_length_code = 8;

        msg.data[0] = recvDataArray[1];
        msg.data[1] = recvDataArray[2];
        msg.data[2] = recvDataArray[3];
        msg.data[3] = recvDataArray[4];
        msg.data[4] = recvDataArray[5];
        msg.data[5] = recvDataArray[6];
        msg.data[6] = recvDataArray[7];
        msg.data[7] = recvDataArray[8];

        ESP_LOGI("TWAI", "Sending message...");
        if (twai_transmit(&msg, pdMS_TO_TICKS(1000)) == ESP_OK)
            ESP_LOGI("TWAI", "Message queued for transmission\n");
        else
            ESP_LOGE("TWAI", "Failed to queue message for transmission\n");
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

void app_main()
{
    twaiSndQueue = xQueueCreate(twaiSndQueueLen, sizeof(uint16_t) * 10);
    // Configure TWAI module
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(TX_PIN, RX_PIN, TWAI_MODE_NORMAL);
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_125KBITS();
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

    if (twai_driver_install(&g_config, &t_config, &f_config) == ESP_OK)
    {
        printf("Driver installed\n");
        printf("Driver BAUDRATE 125k\n");
    }
    else
    {
        printf("Failed to install driver\n");
        return;
    }
    if (twai_start() == ESP_OK)
    {
        printf("Driver started\n");
    }
    else
    {
        printf("Failed to start driver\n");
        return;
    }

    // Create task for receiving messages
    xTaskCreatePinnedToCore(
        twai_receive_task,
        "twai_receive_task",
        4096,
        NULL,
        10,
        NULL,
        APP_CPU_NUM);

    xTaskCreatePinnedToCore(
        twai_send_task,
        "twai_send_task",
        5000,
        NULL,
        10,
        NULL,
        APP_CPU_NUM);
}
