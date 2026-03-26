
/**
  ******************************************************************************
  * @file      adc_driver.c
  * @author    Bartosz Rychlicki
  * @Title     Universal driver for ADC peripheral
  * @brief     This file contains common functions' bodies with ADC Driver's functions implementation
  ******************************************************************************
  * @attention Error codes are called when exact incorrect use of function is made
  *
  * Copyright (c) 2025 AGH Eko-Energy.
  * All rights reserved.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/

#include "adc_driver.h"

static HAL_StatusTypeDef ADC_Init_NoDMA_Indepedent_Discontinuous(ADC_HandleTypeDef* hadc){

	// ADC currently started

	return HAL_OK;
}

static HAL_StatusTypeDef ADC_ReadChannel_NoDMA_Indepedent_Discontinuous(ADC_HandleTypeDef* hadc, uint8_t rank, uint16_t* retval){

	volatile uint16_t binaryType = 0;


	if(rank > 16){
		return HAL_ERROR;
	}

	for(uint8_t i = 0; i <= rank; ++i){

		if(HAL_ADC_Start(hadc) != HAL_OK){
				return HAL_ERROR;
		}

		if(HAL_ADC_PollForConversion(hadc, ADC_POLLING_TIMEOUT) != HAL_OK){
			return HAL_ERROR;
		}

		binaryType = (uint16_t)HAL_ADC_GetValue(hadc);
	}

	*retval = binaryType;



	return HAL_OK;
}

HAL_StatusTypeDef ADC_Init(ADC_HandleTypeDef* hadc){

	// Initializing ADC
	if(HAL_ADC_Init(hadc) != HAL_OK){
		return HAL_ERROR;
	}

	/* TODO: Implement calibration and configuration for
		 * independent / no-DMA / discontinuous mode.
		 * Until then, report an error instead of silently succeeding.
	*/

	// start ADC's conversion in independent/no DMA/discontinuous mode
	if(ADC_Init_NoDMA_Indepedent_Discontinuous(hadc) != HAL_OK){
		return HAL_ERROR;
	}

	return HAL_OK;
}


HAL_StatusTypeDef ADC_ReadChannel(ADC_HandleTypeDef* hadc, uint8_t rank, uint16_t* retval){

	if(ADC_ReadChannel_NoDMA_Indepedent_Discontinuous(hadc, rank, retval) != HAL_OK){
		return HAL_ERROR;
	}

	return HAL_OK;
}

HAL_StatusTypeDef ADC_GetPinVoltage(ADC_HandleTypeDef* hadc, uint8_t rank, float* retval){

	volatile float pinVoltage = 0;
	uint16_t binaryType = 0;

	if(ADC_ReadChannel(hadc, rank, &binaryType) != HAL_OK){
		return HAL_ERROR;
	}

	pinVoltage = (float)binaryType / (float)ADC_RESOLUTION * STM32_VCC;

	if(pinVoltage > STM32_VCC){
		return HAL_ERROR;
	}

	*retval = pinVoltage;

	return HAL_OK;
}
