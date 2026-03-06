#ifndef STM32F3XX_HAL_RCC_MOCK_H
#define STM32F3XX_HAL_RCC_MOCK_H

#include <stdint.h>
#include "stm32f3xx_hal_def_mock.h"

/** * @brief  RCC Internal/External Oscillator Config structure definition  
  */
typedef struct {
  uint32_t OscillatorType;       
  uint32_t HSEState;            
  uint32_t HSEPredivValue;      
  uint32_t LSEState;            
  uint32_t HSIState;            
  uint32_t HSICalibrationValue; 
  uint32_t LSIState;            
  uint32_t PLL_State; 
} RCC_OscInitTypeDef;

/** * @brief  RCC System, AHB and APB busses clock configuration structure definition  
  */
typedef struct {
  uint32_t ClockType;             
  uint32_t SYSCLKSource;          
  uint32_t AHBCLKDivider;         
  uint32_t APB1CLKDivider;        
  uint32_t APB2CLKDivider;        
} RCC_ClkInitTypeDef;


HAL_StatusTypeDef HAL_RCC_OscConfig(RCC_OscInitTypeDef  *RCC_OscInitStruct);
HAL_StatusTypeDef HAL_RCC_ClockConfig(RCC_ClkInitTypeDef  *RCC_ClkInitStruct, uint32_t FLatency);
uint32_t HAL_RCC_GetSysClockFreq(void);


#define __HAL_RCC_GPIOA_CLK_ENABLE()   do { } while(0)
#define __HAL_RCC_GPIOB_CLK_ENABLE()   do { } while(0)
#define __HAL_RCC_ADC12_CLK_ENABLE()   do { } while(0)

#endif