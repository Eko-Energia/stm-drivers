/**
 * @file can_driver.h
 * @brief CAN bus driver for PERLA
 * @author AGH EKO-ENERGIA
 * @author Kacper Lasota
 */

#ifndef INC_CAN_DRIVER_H_
#define INC_CAN_DRIVER_H_

#include "main.h"
#include "can_id_list.h"
#include <stdio.h>

/* Select CAN implementation based on HAL modules */
#if defined(HAL_FDCAN_MODULE_ENABLED)
#include "stm32u5xx_hal_fdcan.h"
#define CAN_USE_FDCAN 1
#define CAN_USE_BXCAN 0
#elif defined(HAL_CAN_MODULE_ENABLED)
#include "stm32fxx_hal_can.h"
#define CAN_USE_FDCAN 0
#define CAN_USE_BXCAN 1
#else
#error "No CAN HAL module enabled. Define HAL_FDCAN_MODULE_ENABLED or HAL_CAN_MODULE_ENABLED."
#endif

/**
 * Defines
 */

#define CAN_MAX_DLC 8
#define CAN_MAX_MSG 32

/**
 * Node ID - Used for error frame transmission
 * Set via CAN_Init()
 */
extern uint32_t CAN_NodeId;

/**
 * Periodic CAN message
 */
typedef struct
{
#if CAN_USE_FDCAN
	FDCAN_TxHeaderTypeDef header;
#else
	CAN_TxHeaderTypeDef header;
#endif
	uint32_t period_ms;
	uint32_t last_tick;
	void (*GetData)(uint8_t *data);
} CAN_ScheduledMsg;

/**
 * Periodic CAN message list used for automation
 */
typedef struct
{
	CAN_ScheduledMsg list[CAN_MAX_MSG];
	uint8_t size;
	uint32_t txMailbox;
} CAN_ScheduledMsgList;

/**
 * Setup functions
 */

/**
 * @brief Initialize CAN with Node ID for error handling
 *
 * This function initializes the CAN peripheral and sets the Node ID
 * which is used for error frame transmission.
 * Also initializes the error handler module.
 *
 * @param hcan      Pointer to CAN handle
 * @param node_id   Node ID for this device (uint32_t)
 */
#if CAN_USE_FDCAN
void CAN_Init(FDCAN_HandleTypeDef *hfdcan, uint32_t node_id);
#else
void CAN_Init(CAN_HandleTypeDef *hcan, uint32_t node_id);
#endif

/**
 * @brief Get configured Node ID
 * @return Current Node ID
 */
uint32_t CAN_GetNodeId(void);

/**
 * Functions for scheduled messages
 */
HAL_StatusTypeDef CAN_AddScheduledMessage(CAN_ScheduledMsg, CAN_ScheduledMsgList *);
HAL_StatusTypeDef CAN_RemoveScheduledMessage(uint32_t, CAN_ScheduledMsgList *);

#if CAN_USE_FDCAN
void CAN_HandleScheduled(FDCAN_HandleTypeDef *hfdcan, CAN_ScheduledMsgList *);
#else
void CAN_HandleScheduled(CAN_HandleTypeDef *hcan, CAN_ScheduledMsgList *);
#endif

/**
 * Functions for received messages
 */
#if CAN_USE_FDCAN
void CAN_HandleReceived(FDCAN_HandleTypeDef *hfdcan, uint8_t fifo);
#else
void CAN_HandleReceived(CAN_HandleTypeDef *hcan, uint8_t fifo);
#endif

#endif /* INC_CAN_DRIVER_H_ */
