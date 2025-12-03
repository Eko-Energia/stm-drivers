/**
 * @file error_handler.c
 * @brief Error handling and reporting implementation for PERLA CAN network
 * @author AGH EKO-ENERGIA
 *
 * @details Implements error reporting via CAN frames.
 *          Supports bxCAN (single/dual) and FDCAN architectures.
 */

#include "error_handler.h"
#include <string.h>

/* ============================================================================
 * Private Variables
 * ============================================================================ */

/** @brief Initialization flag */
static uint8_t s_initialized = 0;

/** @brief Configured Node ID */
static uint16_t s_node_id = 0;

/** @brief Error frame ID (equals node_id) */
static uint32_t s_error_frame_id = 0;

#if ERROR_HANDLER_USE_BXCAN
/** @brief Primary CAN handle */
static CAN_HandleTypeDef *s_hcan = NULL;

/** @brief TX mailbox for error frames */
static uint32_t s_tx_mailbox = 0;

#elif ERROR_HANDLER_USE_FDCAN
/** @brief Primary FDCAN handle */
static FDCAN_HandleTypeDef *s_hfdcan = NULL;

#endif

/* ============================================================================
 * Private Function Prototypes
 * ============================================================================ */

static void SendErrorFrame(uint16_t error_code, uint8_t severity, uint8_t halted,
                           const uint8_t *data, uint8_t data_len);
static void HaltNode(void);

/* ============================================================================
 * Initialization Functions
 * ============================================================================ */

#if ERROR_HANDLER_USE_BXCAN

void ErrorHandler_Init(CAN_HandleTypeDef *hcan, uint16_t node_id)
{
  if (hcan == NULL)
  {
    return;
  }

  s_hcan = hcan;
  s_node_id = node_id;
  s_error_frame_id = (uint32_t)node_id;
  s_initialized = 1;
}

#elif ERROR_HANDLER_USE_FDCAN

void ErrorHandler_Init(FDCAN_HandleTypeDef *hfdcan, uint16_t node_id)
{
  if (hfdcan == NULL)
  {
    return;
  }

  s_hfdcan = hfdcan;
  s_node_id = node_id;
  s_error_frame_id = (uint32_t)node_id;
  s_initialized = 1;
}

#endif

/* ============================================================================
 * Public API Functions
 * ============================================================================ */

void ERROR_REPORT(uint16_t error_code, uint8_t severity)
{
  SendErrorFrame(error_code, severity, 0, NULL, 0);
}

void ERROR_STOP(uint16_t error_code, uint8_t severity)
{
  SendErrorFrame(error_code, severity, 1, NULL, 0);
  HaltNode();
}

void TRIGGER_SAFE_STATE(uint16_t reason)
{
  /* Send error frame with SEVERITY_SAFE_STATE and reason as error code */
  SendErrorFrame(reason, SEVERITY_SAFE_STATE, 0, NULL, 0);
  /* Optionally halt after triggering safe state */
  /* HaltNode(); */
}

void ERROR_REPORT_EX(uint16_t error_code, uint8_t severity, const uint8_t *data, uint8_t data_len)
{
  SendErrorFrame(error_code, severity, 0, data, data_len);
}

void ERROR_STOP_EX(uint16_t error_code, uint8_t severity, const uint8_t *data, uint8_t data_len)
{
  SendErrorFrame(error_code, severity, 1, data, data_len);
  HaltNode();
}

uint16_t ErrorHandler_GetNodeId(void)
{
  return s_node_id;
}

uint8_t ErrorHandler_IsInitialized(void)
{
  return s_initialized;
}

/* ============================================================================
 * Private Functions - bxCAN Implementation
 * ============================================================================ */

#if ERROR_HANDLER_USE_BXCAN

static void SendErrorFrame(uint16_t error_code, uint8_t severity, uint8_t halted,
                           const uint8_t *data, uint8_t data_len)
{
  if (!s_initialized || s_hcan == NULL)
  {
    return;
  }

  /* Build payload */
  uint8_t payload[ERROR_FRAME_DLC] = {0};

  /* Bytes 0-1: Error code (Little Endian) */
  payload[0] = (uint8_t)(error_code & 0xFF);
  payload[1] = (uint8_t)((error_code >> 8) & 0xFF);

  /* Byte 2: Flags
   * Bit 0 (MSB): HALTED
   * Bits 1-3: SEVERITY
   * Bits 4-7: RESERVED (0)
   */
  payload[2] = ((halted & 0x01) << 0) |  /* Bit 0: halted */
               ((severity & 0x07) << 1); /* Bits 1-3: severity */

  /* Bytes 3-7: Specific data */
  if (data != NULL && data_len > 0)
  {
    uint8_t copy_len = (data_len > ERROR_SPECIFIC_DATA_SIZE) ? ERROR_SPECIFIC_DATA_SIZE : data_len;
    memcpy(&payload[3], data, copy_len);
  }

  /* Setup TX header */
  CAN_TxHeaderTypeDef tx_header;
  tx_header.StdId = s_error_frame_id;
  tx_header.ExtId = 0;
  tx_header.IDE = CAN_ID_STD;
  tx_header.RTR = CAN_RTR_DATA;
  tx_header.DLC = ERROR_FRAME_DLC;
  tx_header.TransmitGlobalTime = DISABLE;

  /* Send on primary CAN */
  HAL_CAN_AddTxMessage(s_hcan, &tx_header, payload, &s_tx_mailbox);
}

#elif ERROR_HANDLER_USE_FDCAN

/* ============================================================================
 * Private Functions - FDCAN Implementation
 * ============================================================================ */

static void SendErrorFrame(uint16_t error_code, uint8_t severity, uint8_t halted,
                           const uint8_t *data, uint8_t data_len)
{
  if (!s_initialized || s_hfdcan == NULL)
  {
    return;
  }

  /* Build payload */
  uint8_t payload[ERROR_FRAME_DLC] = {0};

  /* Bytes 0-1: Error code (Little Endian) */
  payload[0] = (uint8_t)(error_code & 0xFF);
  payload[1] = (uint8_t)((error_code >> 8) & 0xFF);

  /* Byte 2: Flags
   * Bit 0 (MSB): HALTED
   * Bits 1-3: SEVERITY
   * Bits 4-7: RESERVED (0)
   */
  payload[2] = ((halted & 0x01) << 0) |
               ((severity & 0x07) << 1);

  /* Bytes 3-7: Specific data */
  if (data != NULL && data_len > 0)
  {
    uint8_t copy_len = (data_len > ERROR_SPECIFIC_DATA_SIZE) ? ERROR_SPECIFIC_DATA_SIZE : data_len;
    memcpy(&payload[3], data, copy_len);
  }

  /* Setup FDCAN TX header */
  FDCAN_TxHeaderTypeDef tx_header;
  tx_header.Identifier = s_error_frame_id;
  tx_header.IdType = FDCAN_STANDARD_ID;
  tx_header.TxFrameType = FDCAN_DATA_FRAME;
  tx_header.DataLength = FDCAN_DLC_BYTES_8;
  tx_header.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
  tx_header.BitRateSwitch = FDCAN_BRS_OFF;
  tx_header.FDFormat = FDCAN_CLASSIC_CAN;
  tx_header.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
  tx_header.MessageMarker = 0;

  /* Send on primary FDCAN */
  HAL_FDCAN_AddMessageToTxFifoQ(s_hfdcan, &tx_header, payload);
}

#endif

/* ============================================================================
 * Common Private Functions
 * ============================================================================ */

/**
 * @brief Halt the node in an infinite loop
 *
 * This function never returns. A hardware reset is required to recover.
 */
static void HaltNode(void)
{
  /* Disable interrupts to prevent further execution */
  __disable_irq();

  /* Infinite loop - node is halted */
  while (1)
  {
    /* Optional: Toggle LED or watchdog to indicate halted state */
    /* HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin); */
    /* HAL_Delay(500); */
  }
}
