#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "esp_err.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "app_adc.h"


// ADC Calibration
#if CONFIG_IDF_TARGET_ESP32
#define ADC_EXAMPLE_CALI_SCHEME ESP_ADC_CAL_VAL_EFUSE_VREF
#elif CONFIG_IDF_TARGET_ESP32S2
#define ADC_EXAMPLE_CALI_SCHEME ESP_ADC_CAL_VAL_EFUSE_TP
#elif CONFIG_IDF_TARGET_ESP32C3
#define ADC_EXAMPLE_CALI_SCHEME ESP_ADC_CAL_VAL_EFUSE_TP
#elif CONFIG_IDF_TARGET_ESP32S3
#define ADC_EXAMPLE_CALI_SCHEME ESP_ADC_CAL_VAL_EFUSE_TP_FIT
#endif

#define  ADC_MAX 3

static const char TAG[] = "adc";
static uint8_t adc_ch[ADC_MAX];
static esp_adc_cal_characteristics_t adc1_chars;

esp_err_t app_adc_init(uint8_t *const ptr_pines)
{
    esp_err_t esp_err = app_adc_calib();

    adc_ch[0] = ptr_pines[0];
    adc_ch[1] = ptr_pines[1];
    adc_ch[2] = ptr_pines[2];
    return esp_err;
}

esp_err_t app_adc_read_milli_voltage(uint32_t *const ptr_voltage)
{
    esp_err_t esp_err;

    esp_err = esp_adc_cal_get_voltage(adc_ch[0], &adc1_chars, &ptr_voltage[0]);
    if (esp_err != ESP_OK)
    {
        printf("app_adc_read_voltage: Error (%s) reading pin %d\n", esp_err_to_name(esp_err), adc_ch[0]);
        return esp_err;
    }

    esp_err = esp_adc_cal_get_voltage(adc_ch[1], &adc1_chars, &ptr_voltage[1]);
    if (esp_err != ESP_OK)
    {
        printf("app_adc_read_voltage: Error (%s) reading pin %d\n", esp_err_to_name(esp_err), adc_ch[1]);
        return esp_err;
    }
    
    esp_err = esp_adc_cal_get_voltage(adc_ch[2], &adc1_chars, &ptr_voltage[2]);
    if (esp_err != ESP_OK)
    {
        printf("app_adc_read_voltage: Error (%s) reading pin %d\n", esp_err_to_name(esp_err), adc_ch[2]);
        return esp_err;
    }
    
    return ESP_OK;
}


esp_err_t app_adc_calib(void)
{
    esp_err_t esp_err;

    ESP_LOGI(TAG, "app_adc_calib");
    esp_err = esp_adc_cal_check_efuse(ADC_EXAMPLE_CALI_SCHEME);

    if (esp_err == ESP_ERR_NOT_SUPPORTED)
    {
        printf("app_adc_calib: Error (%s)\n", esp_err_to_name(esp_err));
        return esp_err;
    }
    else if (esp_err == ESP_ERR_INVALID_VERSION)
    {
        printf("app_adc_calib: Error (%s) eFuse not burnt, skip software calibration\n", esp_err_to_name(esp_err));
        return esp_err;
    }
    else if (esp_err == ESP_OK)
    {
        esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_DEFAULT, 0, &adc1_chars);
        return esp_err;
    }
    else
    {
        printf("app_adc_calib: Error (%s). Invalid arg\n",  esp_err_to_name(esp_err));
        return (esp_err = ESP_FAIL);
    }
}