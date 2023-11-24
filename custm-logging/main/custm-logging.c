/* #include <stdio.h>
#include <stdarg.h>
#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#include "esp_log.h"

void custom_log(const char *tag, esp_log_level_t level, const char *format, ...)
{
    char new_format[256];
    snprintf(new_format, sizeof(new_format), "[CUSTOM] %s", format);

    va_list args;
    va_start(args, format);
    esp_log_write(level, tag, new_format, args);
    va_end(args);
}

void app_main()
{
    custom_log("ExampleTag", ESP_LOG_INFO, "Logging integer: %d\n", 123);
    custom_log("ExampleTag", ESP_LOG_INFO, "Logging string: %s\n", "Hello ESP32");
    custom_log("ExampleTag", ESP_LOG_INFO, "Logging float: %.2f\n", 3.14);
} */

/* #include "esp_log.h"
#include "esp_system.h"

// Define a tag for your logs. This is typically the name of the module.
static const char *TAG = "MyModule";

void app_main()
{
    // Log at different levels
    esp_log_write(ESP_LOG_INFO, TAG, "This is an info log.\n");
    esp_log_write(ESP_LOG_ERROR, TAG, "This is an error log.\n");
    esp_log_write(ESP_LOG_WARN, TAG, "This is a warning log.\n");
    esp_log_write(ESP_LOG_DEBUG, TAG, "This is a debug log.\n");
}
 */


#include <stdio.h>
#include "esp_log.h"
#include <stdarg.h>

#define CUSTOM_LOG(tag, level, format, ...) \
    custom_log(tag, level, __FILE__, __LINE__, format, ##__VA_ARGS__)


void custom_log(const char* tag, esp_log_level_t level, const char* file, int line, const char* format, ...) {
    char new_format[512];
    snprintf(new_format, sizeof(new_format), "[CUSTOM] [%s:%d] %s", file, line, format);

    va_list args;
    va_start(args, format);
    esp_log_write(level, tag, new_format, args);
    va_end(args);
}

void app_main() {
    CUSTOM_LOG("ExampleTag", ESP_LOG_INFO, "Logging integer: %d\n", 123);
    CUSTOM_LOG("ExampleTag", ESP_LOG_INFO, "Logging string: %s\n", "Hello ESP32");
    CUSTOM_LOG("ExampleTag", ESP_LOG_INFO, "Logging float: %.2f\n", 3.14);
}


