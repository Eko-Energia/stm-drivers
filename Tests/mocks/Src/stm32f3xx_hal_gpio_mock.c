#include "stm32f3xx_hal_gpio_mock.h"

GPIO_PinState HAL_GPIO_ReadPin(void* GPIOx, uint16_t GPIO_Pin) {
    // Na razie zwracamy 0, żeby test przeszedł
    return GPIO_PIN_RESET; 
}

void HAL_GPIO_WritePin(void* GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState) {
    // Pusta funkcja, żeby linker był szczęśliwy
}