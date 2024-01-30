#include "driver/uart.h"
#include "esp_log.h"
#include "string.h"

#define BUF_SIZE 1024

// UART2 Configuration
#define UART2_TXD 17
#define UART2_RXD 16
#define HSPL_SIM7600_UART UART_NUM_2
#define COMMAND_SUFFIX "\r\n"

void init_uart()
{
    // Initialize UART0 (Console)
    uart_config_t uart0_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };
    ESP_ERROR_CHECK(uart_driver_install(UART_NUM_0, BUF_SIZE * 2, 0, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(UART_NUM_0, &uart0_config));

    // Initialize UART2 for AT command forwarding
    uart_config_t uart2_config = {
        .baud_rate = 115200, // Match this with your device's requirements
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
    };
    ESP_ERROR_CHECK(uart_param_config(UART_NUM_2, &uart2_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_NUM_2, UART2_TXD, UART2_RXD, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    ESP_ERROR_CHECK(uart_driver_install(UART_NUM_2, BUF_SIZE * 2, 0, 0, NULL, 0));
}

void read_uart2_task(void *pvParameters)
{
    uint8_t *response = (uint8_t *)malloc(BUF_SIZE);
    if (!response)
    {
        ESP_LOGE("UART2", "Failed to malloc; try turning it off and on again");
        vTaskDelete(NULL);
    }

    while (1)
    {
        int len = uart_read_bytes(HSPL_SIM7600_UART, response, BUF_SIZE - 1, 200 / portTICK_RATE_MS);
        if (len < 0)
        {
            ESP_LOGE("UART2", "Failed to read data; try turning it off and on again");
            continue;
        }
        if (len > 0)
        {
            response[len] = '\0';
            printf("%s\n", (const char *)response);
        }
    }

    free(response);
    vTaskDelete(NULL);
}

/* AT+CGACT=1,1

AT+CGSOCKCONT=1,"IP","internet.ng.airtel.com" 
AT+CGSOCKCONT=1,"IPV4V6","IOT.COM"

AT+CGDCONT=1,"IP","IOT.COM"

AT+HTTPPARA="URL","https://app.trakr.live/home"

AT+CNMP=38

AT+HTTPPARA="CID",1

AT+HTTPREAD=0,10000


AT+COPS=1,0,"IND airtel airtel",2
 */

void app_main()
{
    init_uart(); // init uart, because obviously

    xTaskCreatePinnedToCore(read_uart2_task, "read_uart2_task", 2048, NULL, 1, NULL, 1); // create a task to read from UART2

    uint8_t *data = (uint8_t *)malloc(BUF_SIZE); // malloc, like it's a free lunch
    if (!data)
    {
        ESP_LOGE("UART0", "Failed to malloc; try turning it off and on again"); // handling errors, sort of
        return;
    }

    while (1)
    {
        int len = 0; // init len, because we're optimistic
        while (1)
        {
            int read_bytes = uart_read_bytes(UART_NUM_0, &data[len], 1, portMAX_DELAY); // reading one byte, like we've got all day
            if (read_bytes > 0)
            {
                if (data[len] == '\n' || data[len] == '\r')
                {                                            // if enter is pressed, like a true terminal aficionado
                    uart_write_bytes(UART_NUM_0, "\r\n", 2); // echo newline, because we're polite

                    // Constructing the full command
                    data[len] = '\0'; // Null-terminate the input
                    char full_command[len + strlen(COMMAND_SUFFIX) + 1];
                    int full_command_length = snprintf(full_command, sizeof(full_command), "%s%s", data, COMMAND_SUFFIX);
                    if (full_command_length < 0)
                    {
                        ESP_LOGE("UART0", "Failed to construct full command; try turning it off and on again"); // handling errors, sort of
                        return;
                    }
                    // Print the command before sending it
                    printf("> %s\n", (const char *)full_command);

                    // ESP_LOGI("UART0", "Sending command >>>>>> : %s", full_command);
                    uart_write_bytes(HSPL_SIM7600_UART, full_command, full_command_length);
                    uart_flush(HSPL_SIM7600_UART);

                    break; // break, like it's prison
                }
                else
                {
                    uart_write_bytes(UART_NUM_0, &data[len], 1); // echo back, for that authentic terminal feel
                }
                len++; // increment len, because we can
            }
        }
    }

    free(data); // free, like a bird. JK, we never get here.
}