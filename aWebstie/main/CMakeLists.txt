idf_component_register(SRCS "app_main.c"
                    INCLUDE_DIRS "include")

spiffs_create_partition_image(spiffs ../site FLASH_IN_PROJECT)