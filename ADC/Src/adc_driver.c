
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

static HAL_StatusTypeDef ADC_Init_NoDMA_Indenepdent_Discontinous(ADC_HandleTypeDef* hadc){

	// ADC currently started

	return HAL_OK;
}

static HAL_StatusTypeDef ADC_ReadChannel_NoDMA_Indenepdent_Discontinuous(ADC_HandleTypeDef* hadc, uint32_t rank, uint16_t* retval){

	volatile uint16_t binaryType = 0;


	for(volatile uint8_t i = 0; i <= rank; ++i){

		if(HAL_ADC_Start(hadc) != HAL_OK){
				return HAL_ERROR;
		}

		if(HAL_ADC_PollForConversion(hadc, HAL_MAX_DELAY) != HAL_OK){
			return HAL_ERROR;
		}

		binaryType = (uint16_t)HAL_ADC_GetValue(hadc);
	}

	if(binaryType > hadc->Init.Resolution){
		return HAL_ERROR;
	}

	*retval = binaryType;



	return HAL_OK;
}

HAL_StatusTypeDef ADC_Init(ADC_HandleTypeDef* hadc){

	// Initializing ADC
	if(HAL_ADC_Init(hadc) != HAL_OK){
		return HAL_ERROR;
	}

	// start ADC's conversion in independent/no DMA/discintinous mode
	if(ADC_Init_NoDMA_Indenepdent_Discontinous(hadc) != HAL_OK){
		return HAL_ERROR;
	}

	return HAL_OK;
}


HAL_StatusTypeDef ADC_ReadChannel(ADC_HandleTypeDef* hadc, uint32_t rank, uint16_t* retval){

	if(ADC_ReadChannel_NoDMA_Indenepdent_Discontinuous(hadc, rank, retval) != HAL_OK){
		return HAL_ERROR;
	}

	return HAL_OK;
}

HAL_StatusTypeDef ADC_GetValue(ADC_HandleTypeDef* hadc, uint32_t rank, float maxValue, float* retval){

	volatile float realValue = 0;
	uint16_t binaryType = 0;

	if(ADC_ReadChannel(hadc, rank, &binaryType) != HAL_OK){
		return HAL_ERROR;
	}

	realValue = (float)binaryType / hadc->Init.Resolution * maxValue;

	if(realValue > maxValue){
		return HAL_ERROR;
	}

	*retval = realValue;

	return HAL_OK;
}
