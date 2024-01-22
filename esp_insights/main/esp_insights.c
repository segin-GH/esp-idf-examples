#include <esp_insights.h>
#define ESP_INSIGHTS_AUTH_KEY "eyJhbGciOiJSUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1c2VyIjoiMjkyNzYwMjUtOTE5MC00ZDk2LTk1OTEtNzAxMjA1N2VkYWQwIiwiaXNzIjoiZTMyMmI1OWMtNjNjYy00ZTQwLThlYTItNGU3NzY2NTQ1Y2NhIiwic3ViIjoiMjA4MWNhMTEtMGNjNS00ZDRjLTk2ZTAtZTNiMDg4M2ViZTllIiwiZXhwIjoyMDIxMjY3Mjc5LCJpYXQiOjE3MDU5MDcyNzl9.ZdXMEYOd24UobcgwmouepEMfJf8eH0qurdafTADjsriZ8iumX2s4Fdoi8Ssza3BgO2-iXoFma8owwp7vOzTgoAZOhIMVe6y4bdjNykQ4GzLO4SfEuNUjfKdM_VqrzAcPG0QReoEY6uW5GzyLPTqfPmL1uPkNkSAUXq2mHegI0xdXIvOWnc1bocqT6L_i1uoCzngQJevWULUYlTPupw4Pk4F3tc3wc3ed-Sg3xZLhOJlN47WfcvfsUKH0X4LNELuHPfoog3h1GMsVDNRs9MeMIQArS8vv-GJk1LQZqDTU6DD9I_fpddtFMnhd_a2ib_iV3X4mcVqUleEFh0vnmQiNzg"

{
    esp_insights_config_t config = {
        .log_type = ESP_DIAG_LOG_TYPE_ERROR,
        .auth_key = ESP_INSIGHTS_AUTH_KEY,
    };

    esp_insights_init(&config);
}

void app_main(void)
{
    esp_insights_log("Hello World!");
}