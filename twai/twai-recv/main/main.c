
#include <stdio.h>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include "esp_log.h"
#include "esp_system.h"
#include "driver/twai.h"
#include "stdbool.h"

/* CAN PINS */
#define TX_PIN GPIO_NUM_27
#define RX_PIN GPIO_NUM_14

#define MAX_NUM_CAS 16
#define TTLTIMEOUT_MS 10000

xQueueHandle twaiSndQueue;
static uint8_t twaiSndQueueLen = 5;

/** @note
    ittr_for_num_of_cas;
    is used to keep track of which position of cas_id_array should be
    used to add new device.
*/
uint8_t ittr_for_num_of_cas = 0;
uint8_t num_of_cas_alive = 0;
uint8_t default_src_id = 10;

/* Stores uid, src id, ttl value */
typedef struct
{
    uint16_t cas_uid;
    uint8_t src_id;
    uint16_t time_to_live;
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

    printf("Sending ack to CAS of %i with src id %i \n", uid, src_id);
    can_message_array[0] = 0x112;
    can_message_array[1] = cas_uid.bytes[0];
    can_message_array[2] = cas_uid.bytes[1];
    can_message_array[3] = cas_sum.bytes[0];
    can_message_array[4] = cas_sum.bytes[1];
    can_message_array[5] = cas_sum.bytes[2];
    can_message_array[6] = num_of_cas_alive;
    can_message_array[7] = src_id;
    can_message_array[8] = 1;
    xQueueSend(twaiSndQueue, can_message_array, portMAX_DELAY);
}

void gen_can_msg(uint16_t uid, uint8_t src_id)
{
    uint16_t can_message_array[9] = {0};
    // Cas_uid cas_uid;
    // cas_uid.num = uid;

    printf("Sending ack to CAS of %i with src id %i \n", uid, src_id);
    can_message_array[0] = 0x300;
    can_message_array[1] = src_id;
    // can_message_array[2] = cas_uid.bytes[1];
    // // can_message_array[3] = cas_sum.bytes[0];
    // // can_message_array[4] = cas_sum.bytes[1];
    // // can_message_array[5] = cas_sum.bytes[2];
    // // can_message_array[6] = num_of_cas_alive;
    // can_message_array[7] = src_id;
    // can_message_array[8] = 1;
    xQueueSend(twaiSndQueue, can_message_array, portMAX_DELAY);
}
uint32_t sum_device_id_list(int total_num_of_cas)
{
    uint32_t sum = 0;

    for (int i = 0; i < MAX_NUM_CAS; i++)
        if ((cas_id_array[i].time_to_live) < 6)
        {
            /* TODO add a counter for num of cas*/
            printf("summing %i Bcz time-to-live is < 6 = %i \n",
                   cas_id_array[i].cas_uid, cas_id_array[i].time_to_live);
            sum += cas_id_array[i].cas_uid;
            --num_of_cas_alive;
        }
        else
            printf("Not summing %i Bcz time-to-live is > 6 =  %i \n",
                   cas_id_array[i].cas_uid, cas_id_array[i].time_to_live);
    return sum;
}

void update_time_to_live()
{
    for (int i = 0; i < MAX_NUM_CAS; i++)
        cas_id_array[i].time_to_live++;
}

void twai_receive_task(void *pvParameters)
{
    bool add_cas_to_list = true;
    TickType_t prevTime = xTaskGetTickCount();
    for (;;)
    {
        uint32_t elapsed_time = xTaskGetTickCount() - prevTime;
        if (elapsed_time >= TTLTIMEOUT_MS / portTICK_PERIOD_MS)
        {
            printf("Updated ttl\n");
            update_time_to_live();
            prevTime = xTaskGetTickCount();
        }

        if (elapsed_time >= 9000 / portTICK_PERIOD_MS)
        {

        //     for (int i = 0; i < MAX_NUM_CAS; i++)
        //     {
        //         if (cas_id_array[i].cas_uid == 65533)
        //         {
        //             printf("***** TRIG OTA for 65533 ****\n");
        //             gen_can_msg(cas_id_array[i].cas_uid, cas_id_array[i].src_id);
        //         }
        //     }
        //     prevTime = xTaskGetTickCount();
        // }

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

            if (uid.num < 1000 || uid.num > 65535)
                continue;

            /* loop through the list to check if the id is same as the received id */
            for (int i = 0; i < MAX_NUM_CAS; i++)
            {
                if (cas_id_array[i].cas_uid == uid.num)
                {
                    // printf("Same Device Found %i , with ttl %i\n",
                    //        cas_id_array[i].cas_uid, cas_id_array[i].time_to_live);
                    cas_id_array[i].time_to_live = 0;
                    cas_sum.num = sum_device_id_list(MAX_NUM_CAS);
                    /* TODO update the num of cas */
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
                cas_id_array[ittr_for_num_of_cas].cas_uid = uid.num;
                cas_id_array[ittr_for_num_of_cas].src_id = (default_src_id + ittr_for_num_of_cas);

                /* Print the curent list of cas nodes available */
                printf("Current List [");
                for (int i = 0; i < MAX_NUM_CAS; i++)
                    printf("%i, ", cas_id_array[i].cas_uid);
                printf("]\n");

                /* Sum the devices id and gen a ack msg */
                cas_sum.num = sum_device_id_list(MAX_NUM_CAS);
                gen_can_msg_for_ack(cas_id_array[ittr_for_num_of_cas].cas_uid, cas_sum, cas_id_array[ittr_for_num_of_cas].src_id);
                ++ittr_for_num_of_cas;
                ++num_of_cas_alive;

                if (ittr_for_num_of_cas > 16)
                    ittr_for_num_of_cas = 0;
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
