// TODO if you inti you should be able to destroy
// TODO make a end_ota()
// TODO check version if same dont upload
// TODO do you need to implement httpd clean up ?
// TODO send more & descriptive msg to client ?
// TODO version control 


#include "ota.h"
#include "wifi_connect.h"

/* A macro for logging  */
static string SERVER_TAG = "[SERVER]";
static string SPIFFS_OTA = "[SPIFFS OTA]";
static string OTA_TAG = "[OTA]";

/* server handle */
static httpd_handle_t server = NULL;

static const char *mDNS_name = NULL;


/* event handler for the default URL */
static esp_err_t on_default_url(httpd_req_t *req)
{
    esp_err_t err;

    /* LOG URL of req */
    ESP_LOGI(SERVER_TAG, "URL %s: ", req->uri);

    /* config spiffs for file reading*/
    esp_vfs_spiffs_conf_t esp_vfs_spiffs_config = {
        .base_path = "/spiffs",
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = true
    };
    err = esp_vfs_spiffs_register(&esp_vfs_spiffs_config);
    if(err != ESP_OK)
        TASK_ERROR_FATALE("Unable to register spiffs", SERVER_TAG, err);


    /* determine the file path based on the URL */
    char path[600];

    /* if default url gets hit the load the html file from spiffs*/
    if(strcmp(req->uri, "/") ==0)
        strcpy(path, "/spiffs/index.html");
    else
        sprintf(path,"/spiffs%s",req->uri);

    /* determine the file extension */
    char *ext = strrchr(path,'.');

    /* Set the MINE type based on the file extension */
    if(strcmp(ext, ".css") == 0)
        httpd_resp_set_type(req, "text/css");

    if(strcmp(ext, ".js") == 0)
        httpd_resp_set_type(req, "text/javascript");

    if(strcmp(ext, ".png") == 0)
        httpd_resp_set_type(req, "image/png");

    /* open the file for reading */
    FILE *file = fopen(path, "r");
    if(file == NULL)
    {
        /* if file does not open send 404 error and call TASK_ERROR_FATALE */
        httpd_resp_send_404(req);
        esp_vfs_spiffs_unregister(NULL);
        TASK_ERROR_FATALE("Unable to open spiffs for loading website ",SERVER_TAG ,ESP_FAIL);
    }

    /* Read the file in chunks and send it to client */
    char lineRead[256];
    while(fgets(lineRead, sizeof(lineRead), file))
        httpd_resp_sendstr_chunk(req, lineRead);

    /* send NULL to say everything has been send */
    httpd_resp_sendstr_chunk(req, NULL);

    /* close the file and unregister the spiffs */
    fclose(file);
    esp_vfs_spiffs_unregister(NULL);
    
    /* return ESP_OK if every goes well*/
    return ESP_OK;
}


/* SPIFFS UPDATE handler function */
static esp_err_t on_spiffs_update(httpd_req_t *req)
{
    esp_err_t err;
    /* config spiffs for file reading*/
    esp_vfs_spiffs_conf_t esp_vfs_spiffs_config = {
        .base_path = "/spiffs",
        .partition_label = NULL,
        .max_files = 1,
        .format_if_mount_failed = true
    };
    err = esp_vfs_spiffs_register(&esp_vfs_spiffs_config);
    if(err != ESP_OK)
        TASK_ERROR_FATALE("Unable to register spiffs", SPIFFS_OTA, err);
    
    /* Check if the request is a POST request */
    if (req->method != HTTP_POST) 
    {
        httpd_resp_send_err(req, HTTPD_405_METHOD_NOT_ALLOWED, "Method not allowed");
        TASK_ERROR_FATALE("Method not allowed", SERVER_TAG, ESP_FAIL);
    }

    /* Open a file for writing the firmware image */
    FILE *spiffs_file = fopen("/spiffs/spiffs.txt", "w");
    if (spiffs_file == NULL)
    {
        ESP_LOGE(SPIFFS_OTA, "Error opening file for writing");
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Error opening file for writing");
        return ESP_FAIL;
    }

    /* Read the firmware image data from the request body */
    char buf[500];
    int received = 0;
    while (received < req->content_len) 
    {
        /* receive data from http in chunks  */
        int ret = httpd_req_recv(req, buf, sizeof(buf));
        if (ret <= 0)
        {
            fclose(spiffs_file);
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Error receiving request body");
            TASK_ERROR_FATALE("Error receiving request body",SPIFFS_OTA, ESP_FAIL);
        }
        received += ret;
        /* write it into the file */
        fwrite(buf, 1, ret, spiffs_file);
    }

    /* Close the spiffs file */
    fclose(spiffs_file);
    
    /* send data to client saying received file  */
    httpd_resp_sendstr(req, "File Received");
    ESP_LOGI(SPIFFS_OTA, "File Received");
    
    return ESP_OK;
}


/* OTA update handler function */
static esp_err_t on_ota_update(httpd_req_t *req)
{
    /* Check if the request is a POST request */
    if (req->method != HTTP_POST) {
        httpd_resp_send_err(req, HTTPD_405_METHOD_NOT_ALLOWED, "Method not allowed");
        return ESP_FAIL;
    }
    esp_err_t err;
    
    esp_ota_handle_t ota_handle = 0;
    const esp_partition_t *update_partition = NULL;

    ESP_LOGI(OTA_TAG, "Starting OTA...");

    const esp_partition_t *configured = esp_ota_get_boot_partition();
    const esp_partition_t *running = esp_ota_get_running_partition();

    if( configured != running)
    {
        ESP_LOGE(OTA_TAG,"This can happen if either the OTA boot\
            data or preferred boot image become corrupted somehow.");
    }
    
    ESP_LOGI(OTA_TAG, "Running partition type %d subtype %d (offset 0x%08"PRIx32")",
            running->type, running->subtype, running->address);

    update_partition = esp_ota_get_next_update_partition(NULL);
    if(update_partition == NULL)
        TASK_ERROR_FATALE("Error getting OTA update partition",OTA_TAG, ESP_FAIL);

    ESP_LOGI(OTA_TAG, "Writing to partition subtype %d at offset 0x%"PRIx32,
             update_partition->subtype, update_partition->address);

    err = esp_ota_begin(update_partition, OTA_SIZE_UNKNOWN, &ota_handle);
    if(err != ESP_OK)
        TASK_ERROR_FATALE("Error starting OTA update", OTA_TAG, err);

    /* Read the firmware image data from the request body and write it to the OTA partition */
    char buf[128];
    int received = 0;
    while(received < req->content_len)
    {
        int ret = httpd_req_recv(req, buf, sizeof(buf));
        if (ret <= 0)
        {
            esp_ota_end(ota_handle);
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Error receiving request body");
            TASK_ERROR_FATALE("Error receiving request body",OTA_TAG,ESP_FAIL);
        }
        received += ret;
        err = esp_ota_write(ota_handle, (const void *)buf, ret);
        if(err != ESP_OK)
        {
            esp_ota_end(ota_handle);
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Error writing to OTA partition");
            TASK_ERROR_FATALE("Error writing to OTA partition", OTA_TAG, err);
        }
    }
    
    /* Finalize the OTA update process */
    err = esp_ota_end(ota_handle);
    if(err != ESP_OK)
        TASK_ERROR_FATALE("Error finalizing OTA update", OTA_TAG, err);

    /* Set the OTA partition as the active partition */
    err = esp_ota_set_boot_partition(update_partition);
    if(err != ESP_OK)
        TASK_ERROR_FATALE("Error setting OTA partition as active", OTA_TAG, err);

    ESP_LOGI(OTA_TAG, "OTA update successfully");
    ESP_LOGI(OTA_TAG, "Restarting in 2 seconds.....");
    vTaskDelay(2000/portTICK_PERIOD_MS);
    esp_restart();

    return ESP_OK;
}


/* function to start mDNS service */
static void start_mdns_service()
{
    /* initialize mDNS */
    mdns_init();
    /* set host name */
    mdns_hostname_set(mDNS_name);        /* new addr will be http://esp-server.local/ */
    /* set instance name */
    mdns_instance_name_set("esp-server-hspl");
}

void set_mDNS_name(const char * name)
{
    if(name != NULL)
        mDNS_name = name;
    else
        mDNS_name = "esp-server";
}

/* init server */
static void init_server()
{
    esp_err_t err;
    /* set server as default config */
    httpd_config_t serverConfig = HTTPD_DEFAULT_CONFIG();

    /* Set the URI matching function to use wildcards */
    serverConfig.uri_match_fn = httpd_uri_match_wildcard;

    /* start the server */
    err = httpd_start(&server, &serverConfig);
    if(err != ESP_OK)
        TASK_ERROR_FATALE("Unable to start httpd server",SERVER_TAG, err);

    /* register a handler for default URL */
    httpd_uri_t default_url = {
        .uri = "/*",
        .method = HTTP_GET,
        .handler = on_default_url
    };
    err = httpd_register_uri_handler(server, &default_url);
    if(err != ESP_OK)
        TASK_ERROR_FATALE("Unable to register default_url ",SERVER_TAG, err);

    /* register a handler for spiffs URL */
    httpd_uri_t ota_spiffs_update_url = {
        .uri = "/spiffs",
        .method = HTTP_POST,
        .handler = on_spiffs_update 
    };
    err = httpd_register_uri_handler(server, &ota_spiffs_update_url);
    if(err != ESP_OK)
        TASK_ERROR_FATALE("Unable to register ota_spiffs_update_url ",SERVER_TAG, err);

    /* register a handler for OTA URL */
    httpd_uri_t ota_update_url = {
        .uri = "/ota",
        .method = HTTP_POST,
        .handler = on_ota_update
    };
    err = httpd_register_uri_handler(server, &ota_update_url);
    if(err != ESP_OK)
        TASK_ERROR_FATALE("Unable to register ota_update_url ",SERVER_TAG, err);
}


/* a function which handles fatale errors and restarts the chip */
void __attribute__((noreturn)) task_fatal_error( string exit_msg, string tag, esp_err_t err, bool reset)
{
    ESP_LOGE(tag, "%s", exit_msg);
    ESP_LOGE(tag, "The Reason for Error maybe %s",esp_err_to_name(err));
    if(reset != false)
    {
        ESP_LOGE(tag, "Restarting due to fatal error...");
        esp_restart();
    }

    for(;;) { /* never get out of this loop */ }

}


void init_ota(void)
{
    ESP_LOGI(OTA_TAG,"INVOKING OTA");
    nvs_flash_init();
    start_mdns_service();
    init_server();
}