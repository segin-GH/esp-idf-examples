#include <stdio.h>
#include <string.h>
#include "driver/uart.h"
#include "driver/uart.h"
#include "esp_log.h"

#define COMMAND_SUFFIX "\r\n" // Because apparently we still live in the world of typewriters
#define UART_BUF_SIZE (1024)

void init_uart()
{
    uart_config_t uart_config = {
        .baud_rate = 115200, // because why not? everyone loves 115200
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };
    uart_param_config(UART_NUM_2, &uart_config);
    uart_set_pin(UART_NUM_2, 17, 16, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(UART_NUM_2, 1024 * 2, 0, 0, NULL, 0);
}

void send_command(const char *command)
{
    if (!command || *command == '\0')
    {
        ESP_LOGE("UART_SEND", "Trying to send an empty command. Maybe try whispering to it next.");
        return;
    }

    // Append the magic carriage return and newline, because we're all about that base, no treble
    char full_command[strlen(command) + strlen(COMMAND_SUFFIX) + 1]; // +1 for the null terminator, obviously
    sprintf(full_command, "%s%s", command, COMMAND_SUFFIX);          // Concatenating strings like it's 1999

    // Now, for the grand reveal, send the command to UART
    uart_write_bytes(UART_NUM_2, full_command, strlen(full_command)); // Look at it go!
    uart_flush(UART_NUM_2);                                           // Flush, like we're playing poker

    printf("%s\n", full_command);
}

char *read_response(int timeout_ms)
{
    static char data[UART_BUF_SIZE]; // Static buffer to hold the response
    memset(data, 0, UART_BUF_SIZE);  // Clear buffer

    int len = uart_read_bytes(UART_NUM_2, (uint8_t *)data, UART_BUF_SIZE - 1, pdMS_TO_TICKS(timeout_ms));
    if (len > 0)
    {
        data[len] = '\0'; // Ensure null-termination
        printf("%s\n", data);
    }
    else
    {
        ESP_LOGE("UART_READ", "No data or read timeout.");
    }

    return data; // Return the response for further processing
}

void parse_httpaction_response(const char *response, int *status, int *size)
{
    // Assuming response format: +HTTPACTION: <method>,<status_code>,<data_len>
    if (sscanf(response, "+HTTPACTION: %*d,%d,%d", status, size) == 2)
    {
        return;
    }
    else
    {
        ESP_LOGE("PARSE", "Failed to parse HTTPACTION response.");
    }
}

int find_data_start(uint8_t *source)
{
    // Our trusty start marker
    char *start_marker = "+HTTPREAD: DATA,";
    char *start = strstr((char *)source, start_marker);
    if (!start)
        return -1; // No marker, no party

    start += strlen(start_marker); // Jump past the marker

    // Here's the clever bit: find the next "\r\n"
    char *data_start = strstr(start, "\r\n");
    if (!data_start)
        return -1; // No "\r\n", no data

    data_start += 2;                    // Skip over the "\r\n", right into the data's loving arms
    return data_start - (char *)source; // The offset of our data's starting point
}

int find_data_end(uint8_t *source, int start_offset)
{
    char *end_marker = "\r\n+HTTPREAD: 0";
    char *start = (char *)source + start_offset;
    char *end = strstr(start, end_marker);
    if (!end)
        return -1; // End marker not found, might as well go home

    return end - (char *)source; // Return the offset from the beginning
}

int parse_and_clean_data(uint8_t *source, char *dest, int dest_len)
{
    if (!source || !dest || dest_len <= 0)
    {
        return 1;
    }

    int data_start = find_data_start(source);
    if (data_start < 0)
    {
        return 2;
    }

    int data_end = find_data_end(source, data_start);
    if (data_end < 0)
    {
        return 3;
    }

    int data_len = data_end - data_start;
    if (data_len <= 0)
    {
        return 4;
    }
    else if (data_len > dest_len)
    {
        return 5;
    }

    // Copy the binary data as is, no strings attached
    memcpy(dest, source + data_start, data_len);

    // Return the length of the data, because we're honest like that
    return data_len;
}

void print_data_in_hex(char *data, int length)
{
    for (int i = 0; i < length; i++)
    {
        printf("%02x ", (unsigned char)data[i]);
        if ((i + 1) % 16 == 0)
        {
            printf("\n");
        }
    }
    if (length % 16 != 0)
    {
        printf("\n"); // Ensure we start a new line after the last line of data.
    }
}

void app_main()
{

    init_uart();

    // Command 1: The Opening Act
    send_command("AT");
    vTaskDelay(100 / portTICK_PERIOD_MS); // The briefest of pauses, a mere moment to catch one's breath
    read_response(2000);                  // And the audience waits with bated breath for the reply

    // Command 2: Rising Action
    send_command("AT+CGSOCKCONT=1,\"IPV4V6\",\"jionet\"");
    vTaskDelay(100 / portTICK_PERIOD_MS); // A slightly longer interlude, the plot thickens
    read_response(2000);                  // The response, eagerly anticipated

    // Command 3: The Plot Twist
    send_command("AT+HTTPINIT");
    vTaskDelay(100 / portTICK_PERIOD_MS); // The tension builds, a longer pause to reflect
    read_response(2000);                  // The climax approaches, what will the response reveal?

    // Command 4: The Climax
    send_command("AT+HTTPPARA=\"URL\",\"https://raw.githubusercontent.com/segin-hls/bin/main/ota.bin\"");
    vTaskDelay(100 / portTICK_PERIOD_MS); // The grand pause, the moment of truth
    read_response(2000);                  // The revelation, the turning point

    // Command 5: The Denouement
    send_command("AT+HTTPACTION=0");
    vTaskDelay(5000 / portTICK_PERIOD_MS); // Wait for action to complete

    char *response = read_response(2000); // Read the response, potentially with a longer timeout

    // serrch for +HTTPACTION:

    response = strstr(response, "+HTTPACTION:");

    int status = 0, size = 0;
    parse_httpaction_response(response, &status, &size); // Parse the +HTTPACTION response

    printf("HTTP Status: %d, Size: %d\n", status, size);

    // loop util the size is same as writtenre
    while (1)
    {
        send_command("AT+HTTPREAD=0,16");
        vTaskDelay(100 / portTICK_PERIOD_MS); // The grand pause, the moment of truth
        response = read_response(100);       // Read the response, potentially with a longer timeout

        int data_start = find_data_start((uint8_t *)response);

        printf("Binary Data >>>>>>>>>>>> \n");
        print_data_in_hex(response + data_start, 1024);
    }
}