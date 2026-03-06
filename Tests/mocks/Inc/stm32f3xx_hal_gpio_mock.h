#ifndef STM32F3XX_HAL_GPIO_MOCK_H
#define STM32F3XX_HAL_GPIO_MOCK_H

#include <stdint.h>
#include "stm32f3xx_hal_def_mock.h"

typedef struct {
  uint32_t Pin;
  uint32_t Mode;
  uint32_t Pull;
  uint32_t Speed;
} GPIO_InitTypeDef;

typedef enum {
  GPIO_PIN_RESET = 0,
  GPIO_PIN_SET
} GPIO_PinState;

/* Rejestry - udawane adresy */
#define GPIOA               ((void *)0x48000000U)
#define GPIOB               ((void *)0x48000400U)
#define GPIOC               ((void *)0x48000800U)

#define GPIO_PIN_0          ((uint16_t)0x0001)
#define GPIO_PIN_1          ((uint16_t)0x0002)
#define GPIO_PIN_2          ((uint16_t)0x0004)
/* Dodaj kolejne piny jeśli ich używasz */

GPIO_PinState HAL_GPIO_ReadPin(void* GPIOx, uint16_t GPIO_Pin);
void HAL_GPIO_WritePin(void* GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState);

#endif