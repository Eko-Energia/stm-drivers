#ifndef STM32F3XX_HAL_CAN_MOCK_H
#define STM32F3XX_HAL_CAN_MOCK_H

#include <stdint.h>
#include "stm32f3xx_hal_def_mock.h"

typedef struct {
  uint32_t StdId;
  uint32_t ExtId;
  uint32_t IDE;
  uint32_t RTR;
  uint32_t DLC;
} CAN_TxHeaderTypeDef;

typedef struct {
  uint32_t StdId;
  uint32_t ExtId;
  uint32_t IDE;
  uint32_t RTR;
  uint32_t DLC;
  uint32_t Timestamp;
} CAN_RxHeaderTypeDef;

typedef struct {
  void *Instance;
  /* Dodaj pola Init jeśli Twój kod je sprawdza */
} CAN_HandleTypeDef;

#define CAN_ID_STD          0x00000000U
#define CAN_RTR_DATA        0x00000000U

HAL_StatusTypeDef HAL_CAN_AddTxMessage(CAN_HandleTypeDef *hcan, CAN_TxHeaderTypeDef *pHeader, uint8_t aData[], uint32_t *pTxMailbox);
HAL_StatusTypeDef HAL_CAN_GetRxMessage(CAN_HandleTypeDef *hcan, uint32_t RxFifo, CAN_RxHeaderTypeDef *pHeader, uint8_t aData[]);

#endif