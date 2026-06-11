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

/* Include error handler if available */
#if __has_include("error_handler.h")
#include "error_handler.h"
#define ERROR_HANDLER_AVAILABLE (1)
#else
#define ERROR_HANDLER_AVAILABLE (0)
#endif

void CAN_Init(CAN_HandleTypeDef *hcanPtr)
{
	if (HAL_CAN_ActivateNotification(hcanPtr, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
	{
		Error_Handler();
	}

	CAN_FilterTypeDef filterConfig;

	filterConfig.FilterBank = 0;
	filterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
	filterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
	filterConfig.FilterIdHigh = 0x0000;
	filterConfig.FilterIdLow = 0x0000;
	filterConfig.FilterMaskIdHigh = 0x0000;
	filterConfig.FilterMaskIdLow = 0x0000;
	filterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
	filterConfig.FilterActivation = ENABLE;
	filterConfig.SlaveStartFilterBank = 14;

	if (HAL_CAN_ConfigFilter(hcanPtr, &filterConfig) != HAL_OK)
	{
		/* Filter configuration Error */
		Error_Handler();
	}

	if (HAL_CAN_Start(hcanPtr) != HAL_OK)
	{
		Error_Handler();
	}
}

HAL_StatusTypeDef CAN_AddScheduledMsg(struct CAN_scheduledMsg *msg, struct CAN_scheduledMsgList *buffer)
{
	// basic error checking
	if (buffer->size >= CAN_MAX_MSG)
	{
		return HAL_ERROR;
	}
	if (msg->periodMs == 0)
	{
		return HAL_ERROR;
	}

	struct CAN_scheduledMsg tempMsg = *msg;
	tempMsg.lastTick = HAL_GetTick();

	// check if id already exists in the buffer
	for (uint8_t i = 0; i < buffer->size; i++)
	{
		if ((buffer->list[i].header.IDE == CAN_ID_STD && buffer->list[i].header.StdId == tempMsg.header.StdId) ||
			(buffer->list[i].header.IDE == CAN_ID_EXT && buffer->list[i].header.ExtId == tempMsg.header.ExtId))
		{
			return HAL_ERROR;
		}
	}

	buffer->list[buffer->size] = tempMsg;
	buffer->size++;
	return HAL_OK;
}

HAL_StatusTypeDef CAN_RemoveScheduledMsg(uint32_t id, struct CAN_scheduledMsgList *buffer)
{
	for (uint8_t i = 0; i < buffer->size; i++)
	{
		if ((buffer->list[i].header.IDE == CAN_ID_STD && buffer->list[i].header.StdId == id) ||
			(buffer->list[i].header.IDE == CAN_ID_EXT && buffer->list[i].header.ExtId == id))
		{
			while (i + 1 < buffer->size)
			{
				buffer->list[i] = buffer->list[i + 1];
				i++;
			}
			buffer->size--;
			return HAL_OK;
		}
	}

	return HAL_ERROR;
}

void CAN_HandleScheduled(CAN_HandleTypeDef *hcanPtr, struct CAN_scheduledMsgList *scheduler)
{
	if (hcanPtr == NULL || scheduler == NULL)
	{
		return;
	}

	uint32_t currentTick = HAL_GetTick();
	for (uint8_t i = 0; i < scheduler->size; i++)
	{
		struct CAN_scheduledMsg *msg = &scheduler->list[i];
		if (currentTick > msg->lastTick + msg->periodMs)
		{
			uint8_t data[CAN_MAX_DLC];
			// Initialize data to 0 to be safe
			for (uint8_t k = 0; k < CAN_MAX_DLC; k++)
			{
				data[k] = 0;
			}
			
			if (msg->getData != NULL)
			{
				msg->getData(data, msg->context);
			}
			
			if (HAL_CAN_AddTxMessage(hcanPtr, &msg->header, data, &scheduler->txMailbox) != HAL_OK)
			{
				return;
			}

			msg->lastTick = HAL_GetTick();
		}
	}
}

static uint32_t CAN_GetIncomingMsgId(const CAN_RxHeaderTypeDef *header)
{
	if (header->IDE == CAN_ID_STD)
	{
		return header->StdId;
	}

	return header->ExtId;
}

HAL_StatusTypeDef CAN_AddIncomingMsg(struct CAN_IncomingMsgList *buffer, CAN_RxHeaderTypeDef *header, uint8_t *data)
{
	if (buffer == NULL || header == NULL || data == NULL)
	{
		return HAL_ERROR;
	}

	if (buffer->count >= CAN_MAX_MSG)
	{
		return HAL_ERROR;
	}

	struct CAN_IncomingMsg *dst = &buffer->list[buffer->count];
	dst->header = *header;
	memcpy(dst->data, data, CAN_MAX_DLC);

	buffer->count++;
	buffer->receiveFlag = 1;

	return HAL_OK;
}

HAL_StatusTypeDef CAN_GetLatestMessage(struct CAN_IncomingMsgList *buffer, struct CAN_IncomingMsg *msg)
{
	if (buffer == NULL || msg == NULL)
	{
		return HAL_ERROR;
	}

	if (buffer->count == 0)
	{
		return HAL_ERROR;
	}

	uint8_t lowestIdx = 0;
	uint32_t lowestId = CAN_GetIncomingMsgId(&buffer->list[0].header);

	for (uint8_t i = 1; i < buffer->count; i++)
	{
		uint32_t id = CAN_GetIncomingMsgId(&buffer->list[i].header);
		if (id < lowestId)
		{
			lowestId = id;
			lowestIdx = i;
		}
	}

	*msg = buffer->list[lowestIdx];
	buffer->list[lowestIdx] = buffer->list[buffer->count - 1];
	buffer->count--;

	if (buffer->count == 0)
	{
		buffer->receiveFlag = 0;
	}

	return HAL_OK;
}
