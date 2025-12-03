/**
 * @file demo_usage.c
 * @brief Simple demo of Error Handler
 */

#include "main.h"
#include "can_driver.h"
#include "error_handler.h"

#define MY_NODE_ID 128

void app_main(void)
{
  extern CAN_HandleTypeDef hcan;
  CAN_Init(&hcan, MY_NODE_ID);

  uint32_t start = HAL_GetTick();
  uint8_t counter = 0;

  while (1)
  {
    uint32_t elapsed = HAL_GetTick() - start;

    // After 10 seconds - die
    if (elapsed >= 10000)
    {
      ERROR_STOP(0x00FF, SEVERITY_ERROR);
    }

    // Every 2 seconds - report error with debug data
    if (elapsed >= (counter + 1) * 2000)
    {
      counter++;
      uint8_t debug[5] = {counter, (uint8_t)(elapsed & 0xFF), (uint8_t)(elapsed >> 8), 0, 0};

      ERROR_REPORT_EX(0x0001, SEVERITY_INFO, debug, 5);
      ERROR_REPORT(0x0002, SEVERITY_WARNING);
      ERROR_REPORT(0x0003, SEVERITY_ERROR);
      TRIGGER_SAFE_STATE(0x0004);
    }
  }
}
