/**
 * @file can_driver.h
 * @brief CAN bus driver for PERLA
 * @author AGH EKO-ENERGIA
 * @author Kacper Lasota
 */

#ifndef CAN_DRIVER_H
#define CAN_DRIVER_H

#include "main.h"
#include "can_id_list.h"
#include <stdio.h>

/**
 * Defines
 */

#define CAN_MAX_DLC (8)
#define CAN_MAX_MSG (32)

/**
 * Periodic CAN message
 */
struct CAN_scheduledMsg
{
	CAN_TxHeaderTypeDef header;     // frame header
	uint32_t periodMs;              // period of this message
	uint32_t lastTick;              // time stamp of the last message
	void (*getData)(uint8_t *data, void *context); // fetches data
	void *context;                  // user callback context
};

/**
 * Periodic CAN message list used for automation
 */
struct CAN_scheduledMsgList
{
	struct CAN_scheduledMsg list[CAN_MAX_MSG];
	uint8_t size;
	uint32_t txMailbox;
};

/**
 * Setup functions
 */

/**
 * @brief Initialize CAN
 *
 * This function initializes the CAN peripheral.
 *
 * @param hcan      Pointer to CAN handle
 */
void CAN_init(CAN_HandleTypeDef *hcan);

/**
 * @brief Get configured Node ID
 * @return Current Node ID
 */
uint32_t CAN_getNodeId(void);

/**
 * Functions for scheduled messages
 */
HAL_StatusTypeDef CAN_addScheduledMessage(struct CAN_scheduledMsg msg, struct CAN_scheduledMsgList *buffer);

HAL_StatusTypeDef CAN_removeScheduledMessage(uint32_t id, struct CAN_scheduledMsgList *buffer);

void CAN_handleScheduled(CAN_HandleTypeDef *hcan, struct CAN_scheduledMsgList *buffer);

/**
 * @brief Process all scheduled CAN messages (call in main loop)
 * Uses the scheduler list provided during initialization.
 * @param hcan      Pointer to CAN handle
 * @param scheduler Pointer to the CAN scheduled message list (buffer)
 */
void CAN_process(CAN_HandleTypeDef *hcan, struct CAN_scheduledMsgList *scheduler);

/**
 * Functions for received messages
 */
void CAN_handleReceived(CAN_HandleTypeDef *hcan, uint8_t fifo);

#endif // CAN_DRIVER_H
