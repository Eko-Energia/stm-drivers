
/**
  ******************************************************************************
  * @file      adc_driver.c
  * @author    Bartosz Rychlicki
  * @Title     Universal driver for ADC peripheral
  * @brief     This file contains common functions' bodies with ADC Driver's functions implementation
  ******************************************************************************
  * @attention Error codes are called when exact incorrect use of function is made
  *
  * Copyright (c) 2026 AGH Eko-Energy.
  * All rights reserved.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/

#include "adc_driver.h"


/* Functions' bodies ------------------------------------------------------------------------------------ */

static HAL_StatusTypeDef ADC_Init_NoDMA_Independent(ADC_HandleTypeDef* hadc, ADC_ChannelsConfigTypeDefs* cadc){


    // Checking of correct number of discontinuous conversion is set
	if(hadc->Init.NbrOfDiscConversion == 1){
		return HAL_OK;
	}

	return HAL_ERROR;
}



static HAL_StatusTypeDef ADC_ReadChannel_NoDMA_Independent(ADC_HandleTypeDef* hadc, ADC_ChannelsConfigTypeDefs* cadc, uint8_t* rank, uint16_t* retval){

	// Declaration of variable which will temporarily store sampled value
	uint16_t tempValue = 0, endValue = 0;

    // iterating through all ranks
    for(uint8_t i = 0; i < cadc->numberOfSelectedChannels; ++i){

    	// Starting sequencer for ADC
    	if(HAL_ADC_Start(hadc) != HAL_OK){
    	    return HAL_ERROR;
    	}

    	// Starting polling
    	if(HAL_ADC_PollForConversion(hadc, ADC_POLLING_TIMEOUT) == HAL_OK){

    		// reading value to temporary variable
            tempValue = HAL_ADC_GetValue(hadc);

    		// checking if current iteration is co-related to rank for given channel
            if(*rank == i){

            	// assigning to end value of sampled ADC value
            	endValue = tempValue;
            }
    	}else{

    		// if polling crashes then return error status
    		return HAL_ERROR;
    	}

    }

    // Stopping ADC to reset sequencer
    if(HAL_ADC_Stop(hadc) != HAL_OK){
    	return HAL_ERROR;
    }

    // checking if end value is valid
    if(endValue >= 0 && endValue <= ADC_Resolution(hadc)){

    	*retval = endValue;

    	// returning OK status
        return HAL_OK;
    }

    // returning Error status when sampled value is not in safe bounds
    return HAL_ERROR;
}


/*
 * @brief Function read ADC register associated with L data - number of channels in use
 */
static HAL_StatusTypeDef ADC_ChannelsNumberConfig(ADC_HandleTypeDef* hadc, ADC_ChannelsConfigTypeDefs* cadc){

	// Implementation for F1 family
    #if defined(STM32F1_FAMILY) || defined(STM32F3_FAMILY)

        // reading number of channels in used from SQR1
	    cadc->numberOfSelectedChannels = (((hadc->Instance->SQR1 >> ADC_SQR1_L_Pos) & 0xF) + 1);

	    // if read number of selected channels in not 0, then return OK status
	    if(cadc->numberOfSelectedChannels != 0){
	    	return HAL_OK;
	    }


    #endif

	return HAL_ERROR;
}

/*
 * @brief Function read ADC register associated with ranks configuration - SQR1, SQR2, SQR3 and in some families SQR4
 */
static HAL_StatusTypeDef ADC_ChannelsConfig(ADC_HandleTypeDef* hadc, ADC_ChannelsConfigTypeDefs* cadc){

	// checking if correct number of channels has been set
    if(0 == cadc->numberOfSelectedChannels){
    	return HAL_ERROR;
    }

    // Executing operation for F1 family
    #if defined(STM32F1_FAMILY)

	    for(uint8_t i = 0; i < cadc->numberOfSelectedChannels; ++i){

	    	// Execution of read operation of SQR3 register
	    	if(i < ADC_SQR3_2_OFFSET){

                 cadc->ranks[i] = (uint8_t)(hadc->Instance->SQR3 >> (i * ADC_SQR_DATA_RES)) & ADC_SQR_DATA_Msk;

	    	}
	    	// Execution of read operation of SQR2 register
	    	else if(i < ADC_SQR3_1_OFFSET){

	    		 /* Subtracting ADC_SQR3_2_OFFSET because at this point i is no lower than 7, in bit shifting we need to start from 0*/
	    		 cadc->ranks[i] = (uint8_t)(hadc->Instance->SQR2 >> ((i - ADC_SQR3_2_OFFSET)  * ADC_SQR_DATA_RES)) & ADC_SQR_DATA_Msk;

	    	}
	    	// Execution of read operation of SQR1 register
	    	else{

	    		 /* Subtracting ADC_SQR3_1_OFFSET because at this point i is no lower than 13, in bit shifting we need to start from 0*/
	    		 cadc->ranks[i] = (uint8_t)(hadc->Instance->SQR1 >> ((i - ADC_SQR3_1_OFFSET)  * ADC_SQR_DATA_RES)) & ADC_SQR_DATA_Msk;

	    	}

	    }

	    // returning OK status
	    return HAL_OK;

    #elif defined(STM32F3_FAMILY)

	    for(uint8_t i = 0; i < cadc->numberOfSelectedChannels; ++i){

	    	// reading channels from SQR1
            if(i < ADC_SQR1_2_OFFSET){

            	cadc->ranks[i] = (uint8_t)((hadc->Instance->SQR1 >> ((i + ADC_SQR1_L_OFFSET) * ADC_SQR_DATA_RES)) & ADC_SQR_DATA_Msk);

            }
            // Reading SQR2
            else if(i < ADC_SQR1_3_OFFSET){

            	cadc->ranks[i] = (uint8_t)((hadc->Instance->SQR2 >> ((i - ADC_SQR1_2_OFFSET) * ADC_SQR_DATA_RES)) & ADC_SQR_DATA_Msk);

            }
            // Reading SQR3
            else if(i < ADC_SQR1_4_OFFSET){

            	cadc->ranks[i] = ((hadc->Instance->SQR3 >> ((i - ADC_SQR1_3_OFFSET) * ADC_SQR_DATA_RES)) & ADC_SQR_DATA_Msk);

            }
            // Reading SQR4
            else{

                cadc->ranks[i] = ((hadc->Instance->SQR4 >> ((i - ADC_SQR1_4_OFFSET) * ADC_SQR_DATA_RES)) & ADC_SQR_DATA_Msk);

            }

		}

        // returning OK status
	    return HAL_OK;
    #endif

    // returning OK status
	return HAL_ERROR;
}

HAL_StatusTypeDef ADC_Init(ADC_HandleTypeDef* hadc, ADC_ChannelsConfigTypeDefs* cadc){

	// checking if user passed null pointer to ADC handle or cadc structure
	if(NULL == hadc || NULL == cadc){
		return HAL_ERROR;
	}

	// Initializing channels configuration operation
	if(ADC_Get_ChannelsConfiguration(hadc, cadc) != HAL_OK){
		return HAL_ERROR;
	}


	// Init ADC for correct work mode
	if(ADC_Discontinuous(hadc) == ENABLE){

		// Checking if User made correct config in .ioc file
		if(ADC_Init_NoDMA_Independent(hadc, cadc) != HAL_OK){
			return HAL_ERROR;
		}
	}


	// Enabling clock for ADC1
	__HAL_RCC_ADC1_CLK_ENABLE();

	return HAL_OK;
}


HAL_StatusTypeDef ADC_ReadChannel(ADC_HandleTypeDef* hadc, ADC_ChannelsConfigTypeDefs* cadc, uint8_t channel, uint16_t* retval){

	if(hadc == NULL || retval == NULL){
		return HAL_ERROR;
	}

    // Declaration of rank for given channel
	uint8_t rank = 0;

	// Reading rank
	if(ADC_Get_ChannelRank(hadc, cadc, channel, &rank) != HAL_OK){
		return HAL_ERROR;
	}

	// Checking of firmware correctly sensed rank for passed channel | value can be set to one of values from [0, 15]
	if(rank >= 16){
		return HAL_ERROR;
	}

	// Executing reading, with calling correct to .ioc config, private function
    if(DISABLE == ADC_DMA_ENABLED(hadc)){

    	// Executing reading channel for mode without DMA
    	if(ADC_Discontinuous(hadc) == ENABLE){

    		if(ADC_ReadChannel_NoDMA_Independent(hadc, cadc, &rank, retval) != HAL_OK){
    			return HAL_ERROR;
    		}
        // Driver does not support different reading mode without DMA usage, so it returns error
    	}else{

    		return HAL_ERROR;

    	}
    }else{

    	// For now return error status, cause there is no implementation for reading with DMA support
        return HAL_ERROR;
    }



	return HAL_OK;
}

HAL_StatusTypeDef ADC_Get_PinVoltage(ADC_HandleTypeDef* hadc, ADC_ChannelsConfigTypeDefs* cadc, uint8_t channel, float* retval){

	// checking if user gave null pointers
	if(NULL == hadc || NULL == retval){
		return HAL_ERROR;
	}

	// Declaration of variables to store temporary data
	uint16_t binaryValue = 0;
	float tempValue = 0.0f;

	// Reading channel's sampled value
	if(ADC_ReadChannel(hadc, cadc, channel, &binaryValue) != HAL_OK){
		return HAL_ERROR;
	}

	// Calculating voltage on pin
	tempValue = (float)(binaryValue/ADC_Resolution(hadc) * STM32_VCC);

	// Security check
	if(tempValue >= STM32_GND || tempValue <= STM32_VCC){

		// assigning calculated value to return value
		*retval = tempValue;

		// returning OK status
		return HAL_OK;

	}

	return HAL_ERROR;
}


HAL_StatusTypeDef ADC_Get_ChannelRank(ADC_HandleTypeDef* hadc, ADC_ChannelsConfigTypeDefs* cadc, uint8_t channel, uint8_t* rank){

	// iterating through table of channels in cadc
	for(uint8_t i = 0; i < cadc->numberOfSelectedChannels; ++i){
		if(channel == cadc->ranks[i]){

			// overwriting parameter's value - rank - with i value, rank to which channel is assigned
			*rank = i;

			return HAL_OK;
		}
	}

	return HAL_ERROR;
}


HAL_StatusTypeDef ADC_Get_ChannelsConfiguration(ADC_HandleTypeDef* hadc, ADC_ChannelsConfigTypeDefs* cadc){

	// reading number of channels to be used
	if(ADC_ChannelsNumberConfig(hadc, cadc) != HAL_OK){
		return HAL_ERROR;
	}

	// reading channels numbers assigned to ADC's ranks
	if(ADC_ChannelsConfig(hadc, cadc) != HAL_OK){
		return HAL_ERROR;
	}

	return HAL_OK;
}
