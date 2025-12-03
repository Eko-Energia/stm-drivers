/**
 * @file error_handler.h
 * @brief Error handling and reporting for PERLA CAN network
 * @author AGH EKO-ENERGIA
 *
 * @details This module provides standardized error reporting across all CAN nodes.
 *          Errors are transmitted as CAN frames with NODE_ID as the frame ID.
 *
 *          Supports all STM32 CAN architectures:
 *          - Single bxCAN (CAN)
 *          - Dual bxCAN (CAN1, CAN2)
 *          - FDCAN
 *
 * @note Severity levels determine the weight of errors in the system context,
 *       NOT whether the node should continue operating. That decision belongs
 *       to the local node logic.
 */

#ifndef INC_ERROR_HANDLER_H_
#define INC_ERROR_HANDLER_H_

#include "main.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

/* ============================================================================
 * CAN Architecture Detection
 * ============================================================================ */

/* Detect CAN type based on HAL definitions */
#if defined(HAL_FDCAN_MODULE_ENABLED)
#define ERROR_HANDLER_USE_FDCAN 1
#define ERROR_HANDLER_USE_BXCAN 0
#elif defined(HAL_CAN_MODULE_ENABLED)
#define ERROR_HANDLER_USE_FDCAN 0
#define ERROR_HANDLER_USE_BXCAN 1
#else
#error "No CAN module enabled. Enable HAL_CAN_MODULE or HAL_FDCAN_MODULE."
#endif

/* ============================================================================
 * Configuration
 * ============================================================================ */

/** @brief Error frame DLC (8 bytes payload) */
#define ERROR_FRAME_DLC 8

/** @brief Safe State frame ID (highest priority) */
#define SAFE_STATE_FRAME_ID 0x000

/** @brief Maximum diagnostic data bytes */
#define ERROR_SPECIFIC_DATA_SIZE 5

  /* ============================================================================
   * Severity Levels
   * ============================================================================ */

  /**
   * @brief Error severity levels
   *
   * These levels indicate the weight of an error in the context of the entire
   * vehicle's electronic system. They do NOT dictate whether a node should
   * stop or continue - that decision belongs to local node logic.
   */
  typedef enum
  {
    SEVERITY_SAFE_STATE = 0, /**< Critical: requires emergency power disconnect */
    SEVERITY_ERROR = 1,      /**< Pit stop: requires immediate service intervention */
    SEVERITY_WARNING = 2,    /**< Something should be fixed, but safe to continue driving */
    SEVERITY_INFO = 3,       /**< Maintenance info, good to know, no immediate action needed */
                             /* 4-7 reserved for future use */
  } ErrorSeverity_t;

  /* ============================================================================
   * Error Frame Flags (Byte 2 bitfield)
   * ============================================================================ */

  /**
   * @brief Error flags bitfield structure
   *
   * Bit layout (Byte 2):
   *   Bit 0 (MSB): HALTED - 1 = Node halted (needs restart), 0 = Node continues
   *   Bits 1-3:    SEVERITY - Error severity level (0-7)
   *   Bits 4-7:    RESERVED - For future use, set to 0
   */
  typedef struct
  {
    uint8_t halted : 1;   /**< Bit 0: Node halted flag */
    uint8_t severity : 3; /**< Bits 1-3: Severity level */
    uint8_t reserved : 4; /**< Bits 4-7: Reserved, set to 0 */
  } ErrorFlags_t;

  /**
   * @brief Error frame payload structure
   *
   * Total: 8 bytes
   *   Bytes 0-1: Error code (uint16_t, Little Endian)
   *   Byte 2:    Flags (halted + severity)
   *   Bytes 3-7: Specific diagnostic data
   */
  typedef struct
  {
    uint16_t error_code;                             /**< Bytes 0-1: Unique error code (LE) */
    ErrorFlags_t flags;                              /**< Byte 2: Status flags */
    uint8_t specific_data[ERROR_SPECIFIC_DATA_SIZE]; /**< Bytes 3-7: Debug/diagnostic data */
  } ErrorFramePayload_t;

  /* ============================================================================
   * Initialization
   * ============================================================================ */

#if ERROR_HANDLER_USE_BXCAN
  /**
   * @brief Initialize error handler with bxCAN
   *
   * @param hcan      Pointer to CAN handle
   * @param node_id   Node ID (used as error frame ID)
   */
  void ErrorHandler_Init(CAN_HandleTypeDef *hcan, uint16_t node_id);

#elif ERROR_HANDLER_USE_FDCAN
/**
 * @brief Initialize error handler with FDCAN
 *
 * @param hfdcan    Pointer to FDCAN handle
 * @param node_id   Node ID (used as error frame ID)
 */
void ErrorHandler_Init(FDCAN_HandleTypeDef *hfdcan, uint16_t node_id);

#endif

  /* ============================================================================
   * Error Reporting Functions
   * ============================================================================ */

  /**
   * @brief Report an error without stopping the node
   *
   * Sends an error frame over CAN with HALTED=0, indicating the node continues
   * operation. Use this when the node can safely continue despite the error.
   *
   * @param error_code    Unique error code (see error codes CSV in CAN-DATABASE)
   * @param severity      Error severity level (SEVERITY_ERROR, SEVERITY_WARNING, SEVERITY_INFO)
   *
   * @example
   *   // Battery node detects low voltage but continues monitoring
   *   ERROR_REPORT(ERR_LOW_VOLTAGE, SEVERITY_ERROR);
   */
  void ERROR_REPORT(uint16_t error_code, uint8_t severity);

  /**
   * @brief Report an error and halt the node
   *
   * Sends an error frame over CAN with HALTED=1, indicating the node has stopped.
   * After sending, enters an infinite loop (node requires restart).
   *
   * Use this when the node must stop due to the error condition.
   *
   * @param error_code    Unique error code (see error codes CSV in CAN-DATABASE)
   * @param severity      Error severity level
   *
   * @warning This function does NOT return! The node will be halted.
   *
   * @example
   *   // Communication error requires node to stop
   *   ERROR_STOP(ERR_COMM_FAILURE, SEVERITY_WARNING);
   */
  void ERROR_STOP(uint16_t error_code, uint8_t severity);

  /**
   * @brief Trigger system-wide Safe State
   *
   * Sends an error frame with SEVERITY_SAFE_STATE and the reason as error code.
   * This triggers emergency power disconnect affecting the entire vehicle.
   *
   * Use only for critical situations confirmed by multiple sensors/systems.
   *
   * @param reason    Reason code (sent as error_code with SEVERITY_SAFE_STATE)
   *
   * @warning This triggers vehicle-wide emergency power disconnect!
   *
   * @example
   *   // Critical thermal runaway detected by multiple sensors
   *   TRIGGER_SAFE_STATE(REASON_THERMAL_RUNAWAY);
   */
  void TRIGGER_SAFE_STATE(uint16_t reason);

  /* ============================================================================
   * Extended Error Reporting (with diagnostic data)
   * ============================================================================ */

  /**
   * @brief Report an error with additional diagnostic data
   *
   * Same as ERROR_REPORT but allows attaching up to 5 bytes of diagnostic data
   * (e.g., sensor readings, state snapshot at the moment of error).
   *
   * @param error_code    Unique error code
   * @param severity      Error severity level
   * @param data          Pointer to diagnostic data (up to 5 bytes)
   * @param data_len      Length of diagnostic data (0-5)
   */
  void ERROR_REPORT_EX(uint16_t error_code, uint8_t severity, const uint8_t *data, uint8_t data_len);

  /**
   * @brief Report an error with diagnostic data and halt the node
   *
   * Same as ERROR_STOP but allows attaching diagnostic data.
   *
   * @param error_code    Unique error code
   * @param severity      Error severity level
   * @param data          Pointer to diagnostic data (up to 5 bytes)
   * @param data_len      Length of diagnostic data (0-5)
   *
   * @warning This function does NOT return!
   */
  void ERROR_STOP_EX(uint16_t error_code, uint8_t severity, const uint8_t *data, uint8_t data_len);

  /* ============================================================================
   * Utility Functions
   * ============================================================================ */

  /**
   * @brief Get the configured Node ID
   * @return Current node ID
   */
  uint16_t ErrorHandler_GetNodeId(void);

  /**
   * @brief Check if error handler is initialized
   * @return 1 if initialized, 0 otherwise
   */
  uint8_t ErrorHandler_IsInitialized(void);

#ifdef __cplusplus
}
#endif

#endif /* INC_ERROR_HANDLER_H_ */
