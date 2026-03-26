/**
  ******************************************************************************
  * @file    adc_driver.h
  * @author  Bartosz Rychlicki
  * @author  AGH Eko-Energia

  * @Title   Universal driver for ADC peripheral (Built for F1, F2, F3 and F4 families, but can be implemented for all families)

  * @brief   This file contains common defines, flags and macros that are used to prevent high quality of driver's functionalities.
  * 		 All flags, macros and typedefs are built to sense ADC settings and its linked DMA settings
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 AGH Eko-Energy.
  * All rights reserved.
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef INC_ADC_DRIVER_H_
#define INC_ADC_DRIVER_H_

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include <stdint.h>
#include "stm32_family.h"
#include <assert.h>

/* Private constant macros -------------------------------------------------------------------------------------------- */
#define ADC_RESOLUTION 		   (4095)
#define ADC_USED_CHANNELS 	   (1) 											//< Macro defines amount of channels that ADC use to convert analog signals on
#define ADC_CONVERTED_MEASURES (1)  										//< Macro defines amount of measures, that will be used to average value converted by ADC on exact channel
#define ADC_BUFFER_SIZE 	   (ADC_USED_CHANNELS * ADC_CONVERTED_MEASURES) //< Macro stores data buffer length

#define ADC_POLLING_TIMEOUT    (100)										 //< Macro stores max ADC polling timeout, to prevent endless blocking by polling

#define STM32_VCC              (3.3f)										 //< STM32 supply voltage for reading ADC's pins' voltages


/* Functions' prototypes ------------------------------------------------------------------------------------ */
/*
 * @brief  ADC initialization function, initialize type of conversion, set correct ADC's regs' values
 * @param  hadc - handle to ADC instance
 * @retval status of HAL's operation
 */
HAL_StatusTypeDef ADC_Init(ADC_HandleTypeDef* hadc);


/*
 * @brief  ADC reading channel's value function.
 * @param  hadc - handle to ADC instance
 * @param  rank - rank to which channel is assigned
 * @param  *retval - pointer to value, which stores converted value
 * @retval status of HAL's operation
 */
HAL_StatusTypeDef ADC_ReadChannel(ADC_HandleTypeDef* hadc, uint8_t rank, uint16_t* retval);


/*
 * @brief  Read and scale ADC conversion result for the specified rank into assigned to rank STM32 pin;s voltage.
 * @param  hadc     - handle to ADC instance
 * @param  rank     - conversion rank to which the measured channel is assigned
 * @param  *retval  - pointer to variable that will store pin voltage for future operations
 */
HAL_StatusTypeDef ADC_GetPinVoltage(ADC_HandleTypeDef* hadc, uint8_t rank, float* retval);


#ifdef __cplusplus
}
#endif

#endif /* INC_ADC_DRIVER_H_ */
