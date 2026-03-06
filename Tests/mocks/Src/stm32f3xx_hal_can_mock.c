#include "stm32f3xx_hal_can_mock.h"

HAL_StatusTypeDef HAL_CAN_Start(CAN_HandleTypeDef *hcan) {
    return HAL_OK;
}

HAL_StatusTypeDef HAL_CAN_ActivateNotification(CAN_HandleTypeDef *hcan, uint32_t ActiveITs) {
    return HAL_OK;
}

HAL_StatusTypeDef HAL_CAN_AddTxMessage(CAN_HandleTypeDef *hcan, CAN_TxHeaderTypeDef *pHeader, 
                                       uint8_t aData[], uint32_t *pTxMailbox) {
    return HAL_OK;
}

HAL_StatusTypeDef HAL_CAN_GetRxMessage(CAN_HandleTypeDef *hcan, uint32_t RxFifo, 
                                       CAN_RxHeaderTypeDef *pHeader, uint8_t aData[]) {
    return HAL_OK;
}