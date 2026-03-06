#include "stm32f3xx_hal_dma_mock.h"

HAL_StatusTypeDef HAL_DMA_Init(DMA_HandleTypeDef* hdma) {
    return HAL_OK;
}

HAL_StatusTypeDef HAL_DMA_DeInit(DMA_HandleTypeDef* hdma) {
    return HAL_OK;
}

HAL_StatusTypeDef HAL_DMA_Start_IT(DMA_HandleTypeDef* hdma, uint32_t SrcAddress, uint32_t DstAddress, uint32_t DataLength) {
    return HAL_OK;
}