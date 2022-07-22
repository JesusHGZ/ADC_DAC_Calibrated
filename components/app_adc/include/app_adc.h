#ifndef __APP_ADC_H__
#define __APP_ADC_H__

#include "driver/adc.h"
#include "esp_adc_cal.h"

/*
Config 3 pines as 3 analog pines.
@param array whith 3 uint8_t pines.
@return ESP_OK if successful.Otherwhise has an error.
@note ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_DEFAULT
*/
esp_err_t app_adc_init(uint8_t* const ptr_pines);

/*
Get the voltage for the 3 analog pines.
@param variable type uint32_t.
@return ESP_OK if successful.Otherwhise has an error. 
*/
esp_err_t app_adc_read_milli_voltage(uint32_t* const ptr_voltage);

/*
Calibrate adc channel 1.
@return ESP_OK if successful.Otherwhise has an error.
*/
esp_err_t app_adc_calib(void);

#endif // _APP_ADC_H_