#include "stm32f3xx_hal_adc_mock.h"

HAL_StatusTypeDef HAL_ADC_Init(ADC_HandleTypeDef* hadc) {
    return HAL_OK;
}

HAL_StatusTypeDef HAL_ADC_ConfigChannel(ADC_HandleTypeDef* hadc, ADC_ChannelConfTypeDef* sConfig) {
    return HAL_OK;
}

HAL_StatusTypeDef HAL_ADC_Start_DMA(ADC_HandleTypeDef* hadc, uint32_t* pData, uint32_t Length) {
    /* W testach możesz tutaj zasymulować wpisanie danych do pData, 
       jeśli chcesz sprawdzić logikę przetwarzania wyników. */
    return HAL_OK;
}

HAL_StatusTypeDef HAL_ADC_Stop_DMA(ADC_HandleTypeDef* hadc) {
    return HAL_OK;
}