#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "driver/dac.h"
#include "app_adc.h"
#include "esp_adc_cal.h"

static esp_adc_cal_characteristics_t adc1_chars; //Almacenamiento de las características ADC
uint32_t adc_cal[3]; //Vector que almacena los voltajes de entrada ADC (en mV)
uint8_t dac1_val, dac2_val = 0; //Valores auxiliares dac output (en Bits)
uint8_t comp1, comp2 = 0; //Variables de compensación (en Bits)

void app_main(void)
{
    //Configurción y caracterización de ADCs:
    adc1_config_channel_atten(ADC1_CHANNEL_3, ADC_ATTEN_DB_11);
    adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_DB_11);
    adc1_config_channel_atten(ADC1_CHANNEL_7, ADC_ATTEN_DB_11);
    adc1_config_width(ADC_WIDTH_BIT_12);
    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 0, &adc1_chars);

    //Habilitar DACs:
    dac_output_enable(DAC_CHANNEL_1);
    dac_output_enable(DAC_CHANNEL_2);

    while (1)
    {
        //Obtener voltaje de los ADCs (en mV):
        esp_adc_cal_get_voltage(ADC1_CHANNEL_3, &adc1_chars, &adc_cal[0]);
        esp_adc_cal_get_voltage(ADC1_CHANNEL_6, &adc1_chars, &adc_cal[1]);
        esp_adc_cal_get_voltage(ADC1_CHANNEL_7, &adc1_chars, &adc_cal[2]);

        //Conversión de mV a Bits y almacenamiento en variables auxiliares:
        dac1_val = (adc_cal[1] * 256) / 3100;
        dac2_val = (adc_cal[2] * 256) / 3100;

        if (adc_cal[1] && adc_cal[2] < 2500) //El rango de error es mayor entre 0 y 2.5 V, por eso la compensación es mayor.
        {
            //Compensación negativa para mejorar resoución:
            comp1 = dac1_val - 5; 
            comp2 = dac2_val - 8; 

            //Asignación del valor ajustado (auxiliar-compensación) en la salida de los DACs:
            dac_output_voltage(DAC_CHANNEL_1, comp1);
            dac_output_voltage(DAC_CHANNEL_2, comp2);
        }

        else if (adc_cal[1] && adc_cal[2] > 2500) //En el rango de 2.5 a 3.1 V, tenemos que reducir la compensación.
        {
            //Compensación negativa para mejorar resoución:
            comp1 = dac1_val - 4; 
            comp2 = dac2_val - 7; 

            //Asignación del valor ajustado (auxiliar-compensación) en la salida de los DACs:
            dac_output_voltage(DAC_CHANNEL_1, comp1);
            dac_output_voltage(DAC_CHANNEL_2, comp2);
        }
        
        //Imprimir voltajes de los ADCs (en mV).
        printf("ADC1: %d \t ADC2: %d \t RETRO: %d\n", adc_cal[1], adc_cal[2], adc_cal[0]); 

        //Imprimir valores de salida en los DACs (en Bits).
        printf("DAC1: %d \t DAC2 %d \n\n", dac1_val, dac2_val); 

        //Delay
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}