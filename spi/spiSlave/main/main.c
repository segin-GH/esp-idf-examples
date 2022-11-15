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



void app_main(void)
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

    WORD_ALIGNED_ATTR char sendbuf[129]="";
    WORD_ALIGNED_ATTR char recvbuf[129]="";
    memset(recvbuf, 0, sizeof(recvbuf));
    spi_slave_transaction_t t;
    memset(&t, 0, sizeof(t));

    while(1)
     {
        //Clear receive buffer, set send buffer to something sane
        
        memset(recvbuf, 0, sizeof(sendbuf));
        sprintf(sendbuf, "This is the receiver %i",n);
        vTaskDelay(1000/portTICK_PERIOD_MS);
        
        t.length=128*8;
        t.tx_buffer=sendbuf;
        t.rx_buffer=recvbuf;
        
        ret=spi_slave_transmit(RCV_HOST, &t, portMAX_DELAY);

        printf("Received: %s\n", recvbuf);
        n++;
    }

}