#ifndef STM32F3XX_HAL_DMA_MOCK_H
#define STM32F3XX_HAL_DMA_MOCK_H

#include <stdint.h>
#include "stm32f3xx_hal_def_mock.h"

typedef struct {
    uint32_t Direction;
    uint32_t PeriphInc;
    uint32_t MemInc;
    uint32_t PeriphDataAlignment;
    uint32_t MemDataAlignment;
    uint32_t Mode;
    uint32_t Priority;
} DMA_InitTypeDef;

typedef struct {
    void             *Instance;
    DMA_InitTypeDef  Init;
    uint32_t         State;
} DMA_HandleTypeDef;

#endif