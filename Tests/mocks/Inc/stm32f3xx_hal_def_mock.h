#ifndef STM32F3XX_HAL_DEF_H
#define STM32F3XX_HAL_DEF_H

#include <stdint.h>

/* Definicja statusów używana przez wszystkie moduły HAL */
typedef enum {
  HAL_OK       = 0x00U,
  HAL_ERROR    = 0x01U,
  HAL_BUSY     = 0x02U,
  HAL_TIMEOUT  = 0x03U
} HAL_StatusTypeDef;

#define UNUSED(x) ((void)(x))

#endif