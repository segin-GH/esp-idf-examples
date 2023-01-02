idf_component_register(SRCS "ota.c"
    INCLUDE_DIRS "include"
    REQUIRES wifiConnect
    PRIV_REQUIRES esp_https_server nvs_flash esp_https_ota mdns spiffs app_update
)
spiffs_create_partition_image(spiffs ./site FLASH_IN_PROJECT)
# include_directories(${IDF_PATH}/components/)
