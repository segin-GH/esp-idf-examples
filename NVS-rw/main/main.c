#include "nvs_flash.h"
#include "nvs.h"
#include "cJSON.h"

#define MAX_JSON_SIZE 1024

void app_main()
{
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Open
    nvs_handle_t my_handle;
    ret = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (ret != ESP_OK)
    {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(ret));
        return;
    }

    // Write
    const char *key = "IMPSTA";
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "key", "NOIMPACT");
    char json_string[MAX_JSON_SIZE];
    snprintf(json_string, MAX_JSON_SIZE, "%s", cJSON_Print(root));

    ret = nvs_set_str(my_handle, key, json_string);
    printf((ret != ESP_OK) ? "Failed to write!\n" : "Set Done\n");

    // Commit written value.
    ret = nvs_commit(my_handle);
    printf((ret != ESP_OK) ? "Failed to commit!\n" : "Commit Done\n");

    // Close
    nvs_close(my_handle);

    // Reopen for reading
    ret = nvs_open("storage", NVS_READONLY, &my_handle);
    if (ret != ESP_OK)
    {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(ret));
        return;
    }
    else
    {
        printf("Done\n");
    }

    // Read
    size_t required_size = MAX_JSON_SIZE;
    ret = nvs_get_str(my_handle, key, json_string, &required_size);
    if (ret != ESP_OK && ret != ESP_ERR_NVS_NOT_FOUND)
        return;

    // Parse JSON value
    cJSON *parsed_json = cJSON_Parse(json_string);
    char *read_key = cJSON_GetObjectItem(parsed_json, "key")->valuestring;

    printf("Key: %s\n", read_key);

    // Don't forget to free up the resources
    cJSON_Delete(root);
    cJSON_Delete(parsed_json);

    // Close
    nvs_close(my_handle);
}
