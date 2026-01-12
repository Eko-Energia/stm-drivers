/**
  * @file can_driver.c
  * @brief CAN bus driver for PERLA
  * @author AGH EKO-ENERGIA
  * @author Kacper Lasota
  */


/*
 * TODO
 *
 * Error handling both on bus and generic error messages
 * Filter configuration
 * Received messages handling
 *
 */

#include "can_driver.h"

/**
 * @brief initiate CAN with basic filter configuration
 */
void CAN_Init(CAN_HandleTypeDef* hcan)
{
	if(HAL_CAN_ActivateNotification(hcan, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
	{
		Error_Handler();
	}

	CAN_FilterTypeDef sFilterConfig;

	sFilterConfig.FilterBank = 0;
	sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
	sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
	sFilterConfig.FilterIdHigh = 0x0000;
	sFilterConfig.FilterIdLow = 0x0000;
	sFilterConfig.FilterMaskIdHigh = 0x0000;
	sFilterConfig.FilterMaskIdLow = 0x0000;
	sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
	sFilterConfig.FilterActivation = ENABLE;
	sFilterConfig.SlaveStartFilterBank = 14;

	if (HAL_CAN_ConfigFilter(hcan, &sFilterConfig) != HAL_OK)
	{
		/* Filter configuration Error */
		Error_Handler();
	}

	if(HAL_CAN_Start(hcan) != HAL_OK)
	{
		Error_Handler();
	}
}


/**
 * @brief Add new message to the periodic buffer
 */
HAL_StatusTypeDef CAN_AddScheduledMessage(CAN_ScheduledMsg msg, CAN_ScheduledMsgList* buffer)
{
	// basic error checking
	if(buffer->size >= CAN_MAX_MSG - 1)
		Error_Handler();
	if(msg.period_ms == 0)
		Error_Handler();

	msg.last_tick = HAL_GetTick();

	// check if id already exists in the buffer
	for(int i = 0; i < buffer->size; i++)
	{
		if((buffer->list[i].header.IDE == CAN_ID_STD && buffer->list[i].header.StdId == msg.header.StdId)
			|| (buffer->list[i].header.IDE == CAN_ID_EXT && buffer->list[i].header.ExtId == msg.header.ExtId))
			return HAL_ERROR;
	}

	buffer->list[buffer->size] = msg;
	buffer->size++;
	return HAL_OK;
}

/*
 * @brief Remove message from the periodic buffer
 */
HAL_StatusTypeDef CAN_RemoveScheduledMessage(uint32_t id, CAN_ScheduledMsgList* buffer)
{
	for(uint8_t i = 0; i < buffer->size; i++)
	{
		if((buffer->list[i].header.IDE == CAN_ID_STD && buffer->list[i].header.StdId == id)
			|| (buffer->list[i].header.IDE == CAN_ID_EXT && buffer->list[i].header.ExtId == id))
		{
			while(i + 1 < buffer->size)
			{
				buffer->list[i] = buffer->list[i+1];
				i++;
			}
			buffer->size--;
			return HAL_OK;
		}
	}

	return HAL_ERROR;
}

/**
 * @brief This function sends regular messages
 */
void CAN_HandleScheduled(CAN_HandleTypeDef *hcan, CAN_ScheduledMsgList* buffer)
{
	uint32_t currentTick = HAL_GetTick();
	for(uint8_t i = 0; i < buffer->size;i++)
	{
		CAN_ScheduledMsg *msg = &buffer->list[i];
		if(currentTick > msg->last_tick + msg->period_ms)
		{
			uint8_t data[msg->header.DLC];
			msg->GetData(data);
			if(HAL_CAN_AddTxMessage(hcan, &msg->header, data, &buffer->txMailbox) != HAL_OK)
			{
				return;
			}

			msg->last_tick = HAL_GetTick();
		}
	}
}
