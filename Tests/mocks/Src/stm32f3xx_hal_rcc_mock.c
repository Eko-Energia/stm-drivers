#include "stm32f3xx_hal_rcc_mock.h"

HAL_StatusTypeDef HAL_RCC_OscConfig(RCC_OscInitTypeDef  *RCC_OscInitStruct) {
    return HAL_OK;
}

HAL_StatusTypeDef HAL_RCC_ClockConfig(RCC_ClkInitTypeDef  *RCC_ClkInitStruct, uint32_t FLatency) {
    return HAL_OK;
}

uint32_t HAL_RCC_GetSysClockFreq(void) {
    return 72000000U; // Zwracamy standardowe 72MHz dla F3
}