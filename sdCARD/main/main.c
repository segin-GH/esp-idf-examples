#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "esp_log.h"
#include <errno.h>

static const char *TAG = "example";
static sdmmc_card_t *card = NULL;
static sdmmc_host_t host = SDSPI_HOST_DEFAULT();

#define MOUNT_POINT "/sdcard"

#define PIN_NUM_MISO 19
#define PIN_NUM_MOSI 21
#define PIN_NUM_CLK 14
#define PIN_NUM_CS 33

esp_err_t initialize_sd_card(void)
{
    ESP_LOGI(TAG, "Initializing SD card");

    esp_err_t err;
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = true,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024};

    spi_bus_config_t bus_cfg = {
        .mosi_io_num = PIN_NUM_MOSI,
        .miso_io_num = PIN_NUM_MISO,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4000,
    };
    err = spi_bus_initialize(host.slot, &bus_cfg, SDSPI_DEFAULT_DMA);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to initialize spi bus for SD card.");
        return err;
    }

    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = PIN_NUM_CS;
    slot_config.host_id = host.slot;

    const char mount_point[] = MOUNT_POINT;

    ESP_LOGI(TAG, "Mounting filesystem");
    err = esp_vfs_fat_sdspi_mount(mount_point, &host, &slot_config, &mount_config, &card);

    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to mount filesystem. If you want the card to be formatted, set appropriate menuconfig options.");
        spi_bus_free(host.slot);
        return err;
    }

    sdmmc_card_print_info(stdout, card);
    return ESP_OK;
}

// Function to write to a file
esp_err_t write_to_file(const char *filename, const char *content)
{
    FILE *f = fopen(filename, "w");
    if (f == NULL)
    {
        ESP_LOGE(TAG, "Failed to open file for writing");
        return ESP_FAIL;
    }
    fprintf(f, "%s\n", content);
    fclose(f);
    ESP_LOGI(TAG, "File written");
    return ESP_OK;
}

// Function to read from a file
esp_err_t read_from_file(const char *filename, char *buffer, size_t buffer_size)
{
    FILE *f = fopen(filename, "r");
    if (f == NULL)
    {
        ESP_LOGE(TAG, "Failed to open file for reading");
        return ESP_FAIL;
    }
    fgets(buffer, buffer_size, f);
    fclose(f);

    // Strip newline
    char *pos = strchr(buffer, '\n');
    if (pos)
    {
        *pos = '\0';
    }
    ESP_LOGI(TAG, "Read from file: '%s'", buffer);
    return ESP_OK;
}

// Function to delete a file
esp_err_t delete_file(const char *filename)
{
    struct stat st;
    if (stat(filename, &st) == 0)
    { // Check if file exists
        if (unlink(filename) != 0)
        {
            ESP_LOGE(TAG, "Failed to delete file %s", filename);
            return ESP_FAIL; // Return failure if unable to delete
        }
    }
    return ESP_OK; // Return success if file doesn't exist or is deleted
}

// Function to clean up resources
void cleanup_resources(const char *mount_point, sdmmc_card_t *card, sdmmc_host_t host)
{
    esp_vfs_fat_sdcard_unmount(mount_point, card);
    ESP_LOGI(TAG, "Card unmounted");
    spi_bus_free(host.slot);
}

void app_main(void)
{
    // Initialization
    esp_err_t err = initialize_sd_card();
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "SD card initialization failed");
        return;
    }

    // Writing to file
    const char *file_hello = MOUNT_POINT "/hello.txt";
    err = write_to_file(file_hello, "Hello world!");
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to write to file %s", file_hello);
        cleanup_resources(MOUNT_POINT, card, host);
        return;
    }

    // Check if file_foo exists and remove it
    const char *file_foo = MOUNT_POINT "/foo.txt";
    err = delete_file(file_foo);
    if (err != ESP_OK)
    {
        cleanup_resources(MOUNT_POINT, card, host);
        return;
    }

    // Renaming file
    file_foo = MOUNT_POINT "/foo.txt";
    if (rename(file_hello, file_foo) != 0)
    {
        ESP_LOGE(TAG, "Rename failed with error code: %d", errno);
        cleanup_resources(MOUNT_POINT, card, host);
        return;
    }

    // Reading from file
    char line[64];
    read_from_file(file_foo, line, sizeof(line));

    // Cleanup
    cleanup_resources(MOUNT_POINT, card, host);
}
