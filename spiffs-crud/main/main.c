#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include <esp_err.h>
#include <esp_log.h>
#include <esp_spiffs.h>

static const char *TAG = "spiffs";

void app_main(void)
{
    ESP_LOGI(TAG, "init spiffs");
    esp_vfs_spiffs_conf_t config = {
         .base_path = "/spiffs",
         .partition_label = NULL,
         .max_files = 5,
         .format_if_mount_failed = true
    };
    esp_err_t ret = esp_vfs_spiffs_register(&config);

    if(ret != ESP_OK)
    {
        if(ret == ESP_FAIL)
        {
            ESP_LOGE(TAG, "Failed to init or format vfs");
        }
        else if(ret == ESP_ERR_NOT_FOUND)
        {
            ESP_LOGE(TAG, "Failed to find SPIFFS ");
        }
        else
        {
            ESP_LOGE(TAG, "Failed to init SPIFFS (%s)",esp_err_to_name(ret));
        }
        return;
    }

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(NULL, &total, &used);
    
    if(ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to get SPIFFS partion info(%s)",esp_err_to_name(ret));
    }
    else
    {
        ESP_LOGI(TAG, "Partion Size: total : %d, used: %d",total,used);
    }

    ESP_LOGI(TAG, "Opening File");
    FILE *file = fopen("/spiffs/hello.txt","w");
    if(file == NULL)
    {
        ESP_LOGE(TAG, "failed to open for write ");
        return;
    }
    fprintf(file,"Hello World\n");
    fclose(file);
    ESP_LOGI(TAG, "File writen");

    struct stat st;
    if(stat("/spiffs/foo.txt",&st) == 0)
    {
        unlink("/spiffs/foo.txt");
    }
    ESP_LOGI(TAG, "renaming file");
    if (rename("/spiffs/hello.txt","/spiffs/foo.txt") != 0)
    {
        ESP_LOGE(TAG, "rename failed");
        return;
    }

    ESP_LOGI(TAG, "reading file");
    file = fopen("/spiffs/foo.txt", "r");
    
    if (file == NULL)
    {
        ESP_LOGE(TAG, "failed to open file for reading");
        return;
    }
    char line[64];
    fgets(line, sizeof(line), file);
    fclose(file);

    char *pos = strchr(line, '\n');
    {
        if(pos)
        {
            *pos = '\0';
        }
        ESP_LOGI(TAG, "Read from file : (%s)",line);
        esp_vfs_spiffs_unregister(NULL);
        ESP_LOGI(TAG, "SPIFFS unmounted");
    }

}
