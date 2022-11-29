/**
 * @brief SPI SLAVE 
 * @author segin 
 */

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "esp_system.h"
#include "esp_event.h"
#include "driver/spi_slave.h"
#include "esp_log.h"
#include "driver/gpio.h"

#define GPIO_MOSI 23
#define GPIO_MISO 19
#define GPIO_SCLK 18
#define GPIO_CS 21

#define RCV_HOST HSPI_HOST

xQueueHandle queue;
static const int queue_len = 10;
char dataBuff[129]="";

void sendDataThroughSPI(void *args)
{
    int n=0;
    esp_err_t ret;

    //Configuration for the SPI bus
    spi_bus_config_t buscfg={
        .mosi_io_num=GPIO_MOSI,
        .miso_io_num=GPIO_MISO,
        .sclk_io_num=GPIO_SCLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
    };

    //Configuration for the SPI slave interface
    spi_slave_interface_config_t slvcfg={
        .mode=0,
        .spics_io_num=-1, // -1 no chip select pin bcz i implemented my own slave select 
        .queue_size=3,
        .flags=0,
        .post_setup_cb=NULL,
        .post_trans_cb=NULL
    };
    //Enable pull-ups on SPI lines so we don't detect rogue pulses when no master is connected.
    gpio_set_pull_mode(GPIO_MOSI, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(GPIO_SCLK, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(GPIO_CS, GPIO_PULLUP_ONLY);

    //Initialize SPI slave interface
    ret=spi_slave_initialize(RCV_HOST, &buscfg, &slvcfg, SPI_DMA_CH_AUTO);
    assert(ret==ESP_OK);
    gpio_set_direction(GPIO_CS,GPIO_MODE_INPUT);
    WORD_ALIGNED_ATTR char sendbuf[129]="";
    WORD_ALIGNED_ATTR char recvbuf[129]="";
    memset(recvbuf, 0, sizeof(recvbuf));
    spi_slave_transaction_t t;
    memset(&t, 0, sizeof(t));

    while(1)
     {
        //Clear receive buffer, set send buffer to something sane
        memset(recvbuf, 0, sizeof(sendbuf));
        if(xQueueReceive(queue,&sendbuf,5000/portTICK_PERIOD_MS))
        {
            // sprintf(sendbuf, "This is the receiver %i",n);
            t.length=128*8;
            t.tx_buffer=sendbuf;
            t.rx_buffer=recvbuf;
            if(gpio_get_level(GPIO_CS) == 0)
            {
                ret=spi_slave_transmit(RCV_HOST, &t, portMAX_DELAY);
                printf("ReceivedbyslaveONE: %s\n", recvbuf);
                n++;
            }
        }
        vTaskDelay(900/portTICK_PERIOD_MS);
    }
}

void logWithUART(void *args)
{
    esp_err_t err;
    int count = 0;
    while(true)
    {
        memset(dataBuff,0,sizeof(dataBuff));
        sprintf(dataBuff,"Data UART %i",count);
        err = xQueueSend(queue, &dataBuff, 1000/portTICK_PERIOD_MS);
        if(!err)
        {
            printf("[queue] Could not add to queue\n.");
        }
        memset(dataBuff,0,sizeof(dataBuff));
        ++count;
        vTaskDelay(600/portTICK_PERIOD_MS);
    }
}

void app_main(void)
{
    queue = xQueueCreate(queue_len, sizeof(dataBuff)); /* it should be sizeof(dataBuff)*/
    xTaskCreatePinnedToCore(
        sendDataThroughSPI,
        "sendDataThroughSPI",
        2048,
        NULL,
        2,
        NULL,
        APP_CPU_NUM
    );

    xTaskCreatePinnedToCore(
        logWithUART,
        "logWithUART",
        2048,
        NULL,
        2,
        NULL,
        APP_CPU_NUM
    );
}    