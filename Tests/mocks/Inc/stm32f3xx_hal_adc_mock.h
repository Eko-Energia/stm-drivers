#ifndef STM32F3XX_HAL_ADC_MOCK_H
#define STM32F3XX_HAL_ADC_MOCK_H

#include <stdint.h>
#include "stm32f3xx_hal_def_mock.h"

/* Rejestry ADC */
typedef struct {
    volatile uint32_t ISR;
    volatile uint32_t IER;
    volatile uint32_t CR;
    volatile uint32_t CFGR;
    volatile uint32_t SMPR1;
    volatile uint32_t SMPR2;
    volatile uint32_t DR;
} ADC_TypeDef;

#define ADC1    ((ADC_TypeDef *) 0x50000000U)
#define ADC2    ((ADC_TypeDef *) 0x50000100U)

/* Konfiguracja kanałów */
typedef struct {
    uint32_t Channel;
    uint32_t Rank;
    uint32_t SamplingTime;
    uint32_t SingleDiff;
    uint32_t OffsetNumber;
    uint32_t Offset;
} ADC_ChannelConfTypeDef;

/* Struktura Init */
typedef struct {
    uint32_t ClockPrescaler;
    uint32_t Resolution;
    uint32_t DataAlign;
    uint32_t ScanConvMode;
    uint32_t ContinuousConvMode;
    uint32_t DiscontinuousConvMode;
    uint32_t ExternalTrigConv;
    uint32_t ExternalTrigConvEdge;
    uint32_t DMAContinuousRequests;
    uint32_t Overrun;
} ADC_InitTypeDef;

/* Handle ADC */
typedef struct {
    ADC_TypeDef      *Instance;
    ADC_InitTypeDef  Init;
    void             *DMA_Handle; // Wskaźnik na DMA_HandleTypeDef
    uint32_t         State;
    uint32_t         ErrorCode;
} ADC_HandleTypeDef;

/* Definicje stałych (przykładowe) */
#define ADC_RESOLUTION_12B      0x00000000U
#define ADC_DATAALIGN_RIGHT     0x00000000U
#define ADC_SCAN_ENABLE         1U

/* Prototypy */
HAL_StatusTypeDef HAL_ADC_Init(ADC_HandleTypeDef* hadc);
HAL_StatusTypeDef HAL_ADC_ConfigChannel(ADC_HandleTypeDef* hadc, ADC_ChannelConfTypeDef* sConfig);
HAL_StatusTypeDef HAL_ADC_Start_DMA(ADC_HandleTypeDef* hadc, uint32_t* pData, uint32_t Length);

#endif