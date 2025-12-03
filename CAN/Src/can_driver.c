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
#define ERROR_HANDLER_AVAILABLE 1
#else
#define ERROR_HANDLER_AVAILABLE 0
#endif

/**
 * @brief Node ID for error frame transmission
 */
uint32_t CAN_NodeId = 0;

/**
 * @brief Initialize CAN/FDCAN with Node ID and error handler
 */
#if CAN_USE_FDCAN
void CAN_Init(FDCAN_HandleTypeDef *hfdcan, uint32_t node_id)
{
	CAN_NodeId = node_id;

	/* Configure a permissive global filter before start */
	if (HAL_FDCAN_ConfigGlobalFilter(hfdcan,
																	 FDCAN_ACCEPT_IN_RX_FIFO0,
																	 FDCAN_ACCEPT_IN_RX_FIFO0,
																	 DISABLE,
																	 DISABLE) != HAL_OK)
	{
		Error_Handler();
	}

	if (HAL_FDCAN_Start(hfdcan) != HAL_OK)
	{
		Error_Handler();
	}

	if (HAL_FDCAN_ActivateNotification(hfdcan, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0) != HAL_OK)
	{
		Error_Handler();
	}

#if ERROR_HANDLER_AVAILABLE
	ErrorHandler_Init(hfdcan, (uint16_t)node_id);
#endif
}
#else
void CAN_Init(CAN_HandleTypeDef *hcan, uint32_t node_id)
{
	CAN_NodeId = node_id;

	if (HAL_CAN_ActivateNotification(hcan, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
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

	if (HAL_CAN_Start(hcan) != HAL_OK)
	{
		Error_Handler();
	}

#if ERROR_HANDLER_AVAILABLE
	ErrorHandler_Init(hcan, (uint16_t)node_id);
#endif
}
#endif

/**
 * @brief Get configured Node ID
 */
uint32_t CAN_GetNodeId(void)
{
	return CAN_NodeId;
}

/**
 * @brief Add new message to the periodic buffer
 */
HAL_StatusTypeDef CAN_AddScheduledMessage(CAN_ScheduledMsg msg, CAN_ScheduledMsgList *buffer)
{
	// basic error checking
	if (buffer->size >= CAN_MAX_MSG - 1)
		Error_Handler();
	if (msg.period_ms == 0)
		Error_Handler();

	msg.last_tick = HAL_GetTick();

	// check if id already exists in the buffer
	for (int i = 0; i < buffer->size; i++)
	{
		if ((buffer->list[i].header.IDE == CAN_ID_STD && buffer->list[i].header.StdId == msg.header.StdId) || (buffer->list[i].header.IDE == CAN_ID_EXT && buffer->list[i].header.ExtId == msg.header.ExtId))
			return HAL_ERROR;
	}

	buffer->list[buffer->size] = msg;
	buffer->size++;
	return HAL_OK;
}

/*
 * @brief Remove message from the periodic buffer
 */
HAL_StatusTypeDef CAN_RemoveScheduledMessage(uint32_t id, CAN_ScheduledMsgList *buffer)
{
	for (uint8_t i = 0; i < buffer->size; i++)
	{
		if ((buffer->list[i].header.IDE == CAN_ID_STD && buffer->list[i].header.StdId == id) || (buffer->list[i].header.IDE == CAN_ID_EXT && buffer->list[i].header.ExtId == id))
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

/**
 * @brief This function sends regular messages
 */
#if CAN_USE_FDCAN
void CAN_HandleScheduled(FDCAN_HandleTypeDef *hfdcan, CAN_ScheduledMsgList *buffer)
#else
void CAN_HandleScheduled(CAN_HandleTypeDef *hcan, CAN_ScheduledMsgList *buffer)
#endif
{
	uint32_t currentTick = HAL_GetTick();
	for (uint8_t i = 0; i < buffer->size; i++)
	{
		CAN_ScheduledMsg *msg = &buffer->list[i];
		if (currentTick > msg->last_tick + msg->period_ms)
		{

#if CAN_USE_FDCAN
			uint8_t data[8] = {0};
			msg->GetData(data);
			if (HAL_FDCAN_AddMessageToTxFifoQ(hfdcan, &msg->header, data) != HAL_OK)
#else
			uint8_t data[msg->header.DLC];
			msg->GetData(data);
			if (HAL_CAN_AddTxMessage(hcan, &msg->header, data, &buffer->txMailbox) != HAL_OK)
#endif
			{
				return;
			}

			msg->last_tick = HAL_GetTick();
		}
	}
}

/**
 * @brief	Basic functionality only handles safe state and error MSG
 * 			Put this into HAL_CAN_RxFifo0MsgPendingCallback
 * @param	hcan pointer to a CAN_HandleTypeDef structure that contains
 *         	the configuration information for the specified CAN.
 * @param	fifo Fifo number of the received message to be read.
 * 			This parameter can be the value of @arg CAN_receive_FIFO_number
 */
#if CAN_USE_FDCAN
void CAN_HandleReceived(FDCAN_HandleTypeDef *hfdcan, uint8_t fifo)
#else
void CAN_HandleReceived(CAN_HandleTypeDef *hcan, uint8_t fifo)
#endif
{
#if CAN_USE_FDCAN
	FDCAN_RxHeaderTypeDef rxHeader;
	uint8_t rxData[CAN_MAX_DLC];

	if (HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &rxHeader, rxData) != HAL_OK)
	{
		return;
	}

	switch (rxHeader.Identifier)
#else
	/* NOTE : This function should not be modified, when it is needed,
			the CAN_HandleReceived could be implemented in the
			user file
	*/
	CAN_RxHeaderTypeDef rxHeader;
	uint8_t rxData[CAN_MAX_DLC];

	HAL_CAN_GetRxMessage(hcan, fifo, &rxHeader, rxData);

	// highest to lowest priority (lowest to highest ID)
	switch (rxHeader.ExtId)
#endif
	{
	case SAFE_STATE_ID:
		// tutaj raczej od razu w interrupcie funkcja
		printf("Safe STATE!\n");
		break;
	case ERROR_MSG_ID:
		// a tutaj raczej flaga i ogarniamy to w while'u
		printf("Error!\n");
		break;
	case 403105268:

		break;
	default:
		printf("Unknown frame ID\n");
		break;
	}
}
