
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

static HAL_StatusTypeDef ADC_Init_NoDMA_Independent_Discontinuous(ADC_HandleTypeDef* hadc){

	// checking if user gave null pointers
	if(NULL == hadc){
		return HAL_ERROR;
	}


	// ADC currently started

	return HAL_OK;
}

static HAL_StatusTypeDef ADC_ReadChannel_NoDMA_Independent_Discontinuous(ADC_HandleTypeDef* hadc, uint8_t rank, uint16_t* retval){

	// checking if user gave null pointers
	if(NULL == hadc || NULL == retval){
		return HAL_ERROR;
	}

	// declaration of variable, which is in charge of storing sampled value from ADC pin
	uint16_t binaryType = 0;

	// check if user passed incorrect rank
	if(rank >= ADC_USED_CHANNELS){
		return HAL_ERROR;
	}

	// iterating to correct rank number and providing for polling through all channels, to reset reading sequence
	for(uint8_t i = 0; i < ADC_USED_CHANNELS; ++i){

		// starting ADC every polling launch
		if(HAL_ADC_Start(hadc) != HAL_OK){
				return HAL_ERROR;
		}

		// launching polling to read sampled value
		if(HAL_ADC_PollForConversion(hadc, ADC_POLLING_TIMEOUT) != HAL_OK){
			return HAL_ERROR;
		}

		{	// using scopes for providing optimized memory usage, and reading sampled value
			uint16_t tempSampledValue = (uint16_t)HAL_ADC_GetValue(hadc);

			// checking if current iteration is associated with reading selected channel
			if(i == rank){

				// assigning sampled value if current iteration is associated with reading selected channel's value
				binaryType = tempSampledValue;
			}
		}

	}

	// assigning extracted value to retval
	*retval = binaryType;



	return HAL_OK;
}

HAL_StatusTypeDef ADC_Init(ADC_HandleTypeDef* hadc){

	// checking if user passed null pointer to ADC handle
	if(NULL == hadc){
		return HAL_ERROR;
	}


	// Initializing ADC
	if(HAL_ADC_Init(hadc) != HAL_OK){
		return HAL_ERROR;
	}


	// start ADC's conversion in independent/no DMA/discontinuous mode
	if(ADC_Init_NoDMA_Independent_Discontinuous(hadc) != HAL_OK){
		return HAL_ERROR;
	}

	return HAL_OK;
}


HAL_StatusTypeDef ADC_ReadChannel(ADC_HandleTypeDef* hadc, uint8_t rank, uint16_t* retval){



	// Because driver supports independent conversion mode without DMA support for ADC in independent mode only function beloww is called
	if(ADC_ReadChannel_NoDMA_Independent_Discontinuous(hadc, rank, retval) != HAL_OK){
		return HAL_ERROR;
	}

	return HAL_OK;
}

HAL_StatusTypeDef ADC_GetPinVoltage(ADC_HandleTypeDef* hadc, uint8_t rank, float* retval){

	// checking if user gave null pointers
	if(NULL == hadc || NULL == retval){
		return HAL_ERROR;
	}

	// declaration of variables which will store end value of pin's voltage and sampled value from ADC's channel
	float pinVoltage = 0;
	uint16_t binaryType = 0;

	// reading sampled value from channel
	if(ADC_ReadChannel(hadc, rank, &binaryType) != HAL_OK){
		return HAL_ERROR;
	}

	// calculating voltage on pin, assigned to channel
	pinVoltage = (float)binaryType / (float)ADC_RESOLUTION * STM32_VCC;

	// assigning voltage value to retval
	*retval = pinVoltage;

	return HAL_OK;
}
