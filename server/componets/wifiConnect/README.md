# WIFI CONNECT

 A wrapper lib over wifi to make life easy.

## Prerequisites

-   ESP-IDF v4.4  (instructions for installation can be found [here](https://docs.espressif.com/projects/esp-idf/en/stable/get-started/index.html#setup-toolchain))
-   A working ESP32 development board

## Set up
Instruction can be found [here](https://github.com/HacklabEmbedded/components)
 ## Folder content

Below is a tree of the files in the project folder

```
├── CMakeLists.txt
├── include
│   └── wifi_connect.h
├── README.md
└── wifi_connect.c
```

#  API

### `esp_err_t wifi_init(void)`
Initializes WiFi.

**Returns:**
- `ESP_OK`: WiFi was successfully initialized.
- `ESP_FAIL`: An error occurred while initializing WiFi.
---
### `esp_err_t wifi_connect_sta(wifi_cred_t *cred)`
Connects to a WiFi network in station mode using the specified credentials.

**Parameters:**
- `cred`: Pointer to a `wifi_cred_t` struct containing the WiFi name and password.

**Returns:**
- `ESP_OK`: WiFi was successfully connected.
- `ESP_FAIL`: An error occurred while connecting to WiFi.
---
### `esp_err_t wifi_connect_ap(wifi_cred_t *cred)`
Connects to a WiFi network in access point mode using the specified credentials.

**Parameters:**
- `cred`: Pointer to a `wifi_cred_t` struct containing the WiFi name and password.

**Returns:**
- `ESP_OK`: WiFi was successfully connected.
- `ESP_FAIL`: An error occurred while connecting to WiFi.
---
### `esp_err_t wifi_disconnect_sta(wifi_cred_t *cred)`
Disconnects from a WiFi network in station mode.

**Parameters:**
- `cred`: Pointer to a `wifi_cred_t` struct containing the WiFi name and password.

**Returns:**
- `ESP_OK`: WiFi was successfully disconnected.
- `ESP_FAIL`: An error occurred while disconnecting from WiFi.

### `esp_err_t deinit_wifi(void)`
Deinitializes WiFi.

**Returns:**
- `ESP_OK`: WiFi was successfully deinitialized.
- `ESP_FAIL`: An error occurred while deinitializing WiFi.
---
## Structures

### `wifi_cred_t`
Contains WiFi credentials for connecting to a WiFi network.

**Members:**
- `char wifi_name[16]`: Name of the WiFi network.
- `char wifi_pass[16]`: Password for the WiFi network.
- `int k_timeout`: Timeout value (in ms) for WiFi connection or disconnection.
---
A full example how to use this api can be found [here](https://github.com/HacklabEmbedded/EspIdfComponentTestCodes/tree/main/ota)



---
#### TODO
* check if the return value of api is correct.

