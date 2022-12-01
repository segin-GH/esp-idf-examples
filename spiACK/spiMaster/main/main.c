/**
 * @brief SPI MASTER
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
#include "driver/spi_master.h"
#include "esp_log.h"

#include "driver/gpio.h"
#include "esp_intr_alloc.h"

#define GPIO_HANDSHAKE 14
#define GPIO_MOSI 23
#define GPIO_MISO 19
#define GPIO_SCLK 18
#define GPIO_CS 21


#define SENDER_HOST HSPI_HOST

void app_main(void)
{
    esp_err_t ret;
    spi_device_handle_t handle;

    //Configuration for the SPI bus
    spi_bus_config_t buscfg = {
        .mosi_io_num = GPIO_MOSI,
        .miso_io_num = GPIO_MISO,
        .sclk_io_num = GPIO_SCLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1
    };
    //Configuration for the SPI device on the other side of the bus
    spi_device_interface_config_t devcfg={
        .command_bits = 0,
        .address_bits = 0,
        .dummy_bits = 0,
        .clock_speed_hz = 6000000,
        .duty_cycle_pos = 128,        //50% duty cycle
        .mode = 0,
        .spics_io_num = -1,
        .cs_ena_posttrans = 3,        //Keep the CS low 3 cycles after transaction, to stop slave from missing the last bit when CS has less propagation delay than CLK
        .queue_size = 5
    };
    gpio_set_direction(GPIO_CS,GPIO_MODE_OUTPUT);
    int n = 0;
    char sendbuf[130] = {0};
    char recvbuf[130] = {0};
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    
    //Initialize the SPI bus and add the device we want to send stuff to.
    ret = spi_bus_initialize(SENDER_HOST, &buscfg, SPI_DMA_CH_AUTO);
    assert(ret == ESP_OK);
    ret = spi_bus_add_device(SENDER_HOST, &devcfg, &handle);
    assert(ret == ESP_OK);
    while(true)
    {
        int res = snprintf(sendbuf, sizeof(sendbuf),
                "Sender %i ;; Last time, I received: \"%s\"",n,recvbuf);
        if (res >= sizeof(sendbuf)) 
        {
            printf("Data truncated\n");
        }
        t.length=sizeof(sendbuf)*20;
        t.tx_buffer=sendbuf;
        t.rx_buffer=recvbuf;
        {
            gpio_set_level(GPIO_CS,0);
            //Wait for slave to be ready for next byte before sending
            ret=spi_device_transmit(handle, &t);
            printf("ReceivedbyMaster: %s\n", recvbuf);
            memset(&t, 0, sizeof(t));
            ++n;
            vTaskDelay(1000/portTICK_PERIOD_MS);
            gpio_set_level(GPIO_CS,1);
        }
    }
    //Never reached.
    ret=spi_bus_remove_device(handle);
    assert(ret==ESP_OK);
}