/**
 * @file demo_usage.c
 * @brief Simple demo of Error Handler with Heartbeat and Error States
 */

#include "main.h"
#include "error_handler.h"
#include "can_driver.h"

// Pedals node - arithmetics using Kvasser ID (64 is start of block, 5 bits of messageID)
#define MY_NODE_ID (64 >> 5)

/* Global references (mocking what main.c would have) */
extern CAN_HandleTypeDef hcan;
struct CAN_scheduledMsgList schedulerList = {0};
static ERROR_HANDLER_HandleTypeDef hErrorHandler = {0};

void app_main(void)
{
	/* Initialize CAN */
	/* Configure filter, start, etc. done in CAN_init */
	/* We now pass the scheduler list to be handled internally by the driver */
	CAN_init(&hcan);

	/* Initialize Error Handler with Scheduler */
	/* This automatically adds the Heartbeat OK message (1s period) */
	ERROR_HANDLER_init(&hErrorHandler, &hcan, MY_NODE_ID, &schedulerList);

	uint32_t start = HAL_GetTick();

	/* State machine demo variables */
	uint8_t demoState = 0;
	uint32_t lastStateChange = 0;

	while (1)
	{
		/* Process queued messages (Heartbeats or Error Frames) */
		/* Now handled by the driver's process function */
		CAN_process(&hcan, &schedulerList);

		uint32_t now = HAL_GetTick();
		if (now - lastStateChange > 5000)
		{
			lastStateChange = now;
			demoState++;

			switch (demoState)
			{
			case 1:
				/* 5s: Simulate Minor Error A */
				/* Should replace Heartbeat OK with Error Message (300ms period) with Code 0x0100 */
				ERROR_HANDLER_report(&hErrorHandler, 0x0100, ERROR_SEVERITY_WARNING);
				break;

			case 2:
				/* 10s: Simulate Minor Error B (Overwrites A) */
				/* Should update Error Message to Code 0x0200 */
				ERROR_HANDLER_report(&hErrorHandler, 0x0200, ERROR_SEVERITY_ERROR);
				break;

			case 3:
				/* 15s: Clear Error A (Should fail/ignore as B is active) */
				/* Should still show Error 0x0200 */
				ERROR_HANDLER_clear(&hErrorHandler, 0x0100);
				break;

			case 4:
				/* 20s: Clear Error B (Correct) */
				/* Should return to Heartbeat OK (1s period, Code 0xFFFF). */
				ERROR_HANDLER_clear(&hErrorHandler, 0x0200);
				break;

			case 5:
				/* 25s: Simulate Critical Error (Stop) */
				/* Should send Error Message and enter infinite loop processing CAN */
				ERROR_HANDLER_stop(&hErrorHandler, 0xDEAD, ERROR_SEVERITY_ERROR);
				break;

			default:
				break;
			}
		}

		/* Mock application work */
		HAL_Delay(10);
	}
}
