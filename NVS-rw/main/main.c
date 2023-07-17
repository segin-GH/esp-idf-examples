#include <stdio.h>
#include <string.h>
#include "nvs_flash.h"
#include "nvs.h"
#include "cJSON.h"

#define NVS_NAMESPACE "storage"

esp_err_t save_cjson_to_nvs()
{
    nvs_handle_t my_handle;
    esp_err_t err;

    // Open
    err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &my_handle);
    if (err != ESP_OK)
    {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
        return err;
    }

    // Create cJSON object
    cJSON *pdb_settings_json = cJSON_CreateObject();
    if (pdb_settings_json == NULL)
    {
        printf("Failed to create json object\n");
        return ESP_FAIL;
    }

    cJSON_AddItemToObject(pdb_settings_json, "NAM", cJSON_CreateString("PDB.ZORO"));
    cJSON_AddItemToObject(pdb_settings_json, "VER", cJSON_CreateString("1.0.0"));
    cJSON_AddItemToObject(pdb_settings_json, "ID", cJSON_CreateString("1234567890"));
    cJSON_AddItemToObject(pdb_settings_json, "KEY", cJSON_CreateNumber(12));

    char *stringified_json = cJSON_Print(pdb_settings_json);
    if (stringified_json == NULL)
    {
        printf("Failed to stringify json\n");
        cJSON_Delete(pdb_settings_json);
        return ESP_FAIL;
    }

    // Write
    err = nvs_set_blob(my_handle, "my_json", stringified_json, strlen(stringified_json) + 1);
    if (err != ESP_OK)
    {
        printf("Error (%s) writing!\n", esp_err_to_name(err));
    }

    // Commit
    err = nvs_commit(my_handle);
    if (err != ESP_OK)
    {
        printf("Error (%s) committing!\n", esp_err_to_name(err));
    }

    // Close
    nvs_close(my_handle);
    free(stringified_json);
    cJSON_Delete(pdb_settings_json);
    return err;
}

esp_err_t read_cjson_from_nvs()
{
    nvs_handle_t my_handle;
    esp_err_t err;
    size_t required_size = 0;

    // Open
    err = nvs_open(NVS_NAMESPACE, NVS_READONLY, &my_handle);
    if (err != ESP_OK)
    {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
        return err;
    }

    // Obtain the length of the string that needs to be stored
    err = nvs_get_blob(my_handle, "my_json", NULL, &required_size);
    if (err != ESP_OK)
    {
        printf("Failed to get the required size\n");
        nvs_close(my_handle);
        return err;
    }

    // Allocate the memory
    char *stored_stringified_json = malloc(required_size);
    if (stored_stringified_json == NULL)
    {
        printf("Failed to allocate memory\n");
        nvs_close(my_handle);
        return ESP_FAIL;
    }

    // Read
    err = nvs_get_blob(my_handle, "my_json", stored_stringified_json, &required_size);
    if (err != ESP_OK)
    {
        printf("Error (%s) reading!\n", esp_err_to_name(err));
        free(stored_stringified_json);
        nvs_close(my_handle);
        return err;
    }

    cJSON *pdb_settings_json = cJSON_Parse(stored_stringified_json);
    if (pdb_settings_json == NULL)
    {
        printf("Failed to parse json\n");
        free(stored_stringified_json);
        nvs_close(my_handle);
        return ESP_FAIL;
    }

    char *output_string = cJSON_Print(pdb_settings_json);
    printf("%s\n", output_string);

    // Close and free allocated memories
    nvs_close(my_handle);
    free(output_string);
    free(stored_stringified_json);
    cJSON_Delete(pdb_settings_json);

    return ESP_OK;
}

void app_main()
{
    esp_err_t err;

    // Initialize NVS
    err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    err = save_cjson_to_nvs();
    if (err != ESP_OK)
    {
        printf("Failed to save json to nvs\n");
        return;
    }

    err = read_cjson_from_nvs();
    if (err != ESP_OK)
    {
        printf("Failed to read json from nvs\n");
        return;
    }
}