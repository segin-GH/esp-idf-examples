#include <stdio.h>
#include <driver/gpio.h>
#include <driver/adc.h>
#include <esp_adc_cal.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

void app_main()
{ 
    while(true)
    {
        // Initialize ADC peripheral
        adc1_config_width(ADC_WIDTH_BIT_12);
        adc1_config_channel_atten(ADC1_CHANNEL_7, ADC_ATTEN_DB_11);

        // Read the voltage of the internal reference
        uint32_t internal_voltage = adc1_get_raw(ADC1_CHANNEL_7);

        vTaskDelay(100 / portTICK_PERIOD_MS);
        esp_adc_cal_characteristics_t adc_chars;
        // Characterize the ADC
        esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 3300, &adc_chars);

        vTaskDelay(100 / portTICK_PERIOD_MS);
        // Convert the 12-bit ADC value to voltage (in mV)
        internal_voltage = esp_adc_cal_raw_to_voltage(internal_voltage, &adc_chars);

        vTaskDelay(100 / portTICK_PERIOD_MS);
        // Calculate the voltage of the 3V3 pin
        uint32_t vdd33_voltage = internal_voltage * 3.3 / 1.1;
        printf("3V3 voltage: %d mV\n", vdd33_voltage);
    }
}
