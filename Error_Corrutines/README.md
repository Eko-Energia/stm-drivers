# Error Handler Module

Error handling and reporting module for PERLA CAN network. Provides standardized error reporting across all CAN nodes.

## Features

- **Multi-architecture support**: Works with bxCAN (single/dual) and FDCAN
- **Automatic CAN type detection**: Uses HAL defines to select the correct implementation
- **Flexible severity levels**: INFO, WARNING, ERROR, SAFE_STATE
- **Separation of concerns**: Severity indicates system-wide importance, node decides whether to halt

## API

### Initialization

```c
// Single CAN
CAN_InitWithNodeId(&hcan, NODE_ID);  // Recommended - initializes both CAN and error handler

// Dual CAN (CAN1 + CAN2)
CAN_InitDualWithNodeId(&hcan1, &hcan2, NODE_ID);

// Or initialize error handler separately
ErrorHandler_Init(&hcan, NODE_ID);
ErrorHandler_InitDual(&hcan1, &hcan2, NODE_ID);
```

### Error Reporting

```c
// Report error - node continues operation
ERROR_REPORT(error_code, SEVERITY_WARNING);

// Report error and halt - node stops (requires restart)
ERROR_STOP(error_code, SEVERITY_ERROR);

// Trigger system-wide safe state (emergency power disconnect)
TRIGGER_SAFE_STATE(reason_code);

// Extended versions with diagnostic data
uint8_t debug_data[5] = {temp, voltage_l, voltage_h, state, 0};
ERROR_REPORT_EX(error_code, SEVERITY_ERROR, debug_data, 5);
ERROR_STOP_EX(error_code, SEVERITY_ERROR, debug_data, 5);
```

## Severity Levels

| Level                 | Value | Description                                        |
| --------------------- | ----- | -------------------------------------------------- |
| `SEVERITY_SAFE_STATE` | 0     | Critical - requires emergency power disconnect     |
| `SEVERITY_ERROR`      | 1     | Pit stop - requires immediate service intervention |
| `SEVERITY_WARNING`    | 2     | Something should be fixed, but safe to continue    |
| `SEVERITY_INFO`       | 3     | Maintenance info, no immediate action needed       |

## CAN Frame Format

### Error Frame (ID = NODE_ID + 1)

| Bytes | Field         | Type       | Description                       |
| ----- | ------------- | ---------- | --------------------------------- |
| 0-1   | ERROR CODE    | uint16_t   | Unique error code (Little Endian) |
| 2     | FLAGS         | bitfield   | Status flags (see below)          |
| 3-7   | SPECIFIC DATA | uint8_t[5] | Diagnostic data snapshot          |

**FLAGS bitfield (Byte 2):**
| Bits | Name | Description |
|------|------|-------------|
| 0 | HALTED | 1 = Node halted (needs restart), 0 = continues |
| 1-3 | SEVERITY | Severity level (0-7) |
| 4-7 | RESERVED | Reserved, set to 0 |

### Safe State Frame (ID = 0x000)

| Bytes | Field       | Type     | Description                       |
| ----- | ----------- | -------- | --------------------------------- |
| 0-1   | REASON      | uint16_t | Safe state trigger reason         |
| 2-3   | SOURCE NODE | uint16_t | Node ID that triggered safe state |
| 4-7   | RESERVED    | -        | Reserved                          |

## Usage Example

```c
#include "can_driver.h"
#include "error_handler.h"

#define MY_NODE_ID  128  // Big_BMS

// Error codes (define in your project)
#define ERR_LOW_VOLTAGE     0x0001
#define ERR_COMM_FAILURE    0x0002
#define ERR_THERMAL_RUNAWAY 0x0003

int main(void) {
    // Initialize CAN with Node ID
    CAN_InitWithNodeId(&hcan, MY_NODE_ID);

    while (1) {
        // Check for low voltage
        if (battery_voltage < MIN_VOLTAGE) {
            // Report error but continue monitoring
            ERROR_REPORT(ERR_LOW_VOLTAGE, SEVERITY_ERROR);
        }

        // Check for communication errors
        if (comm_errors > MAX_ERRORS) {
            // Stop node - too risky to continue
            ERROR_STOP(ERR_COMM_FAILURE, SEVERITY_WARNING);
            // Never reaches here - node is halted
        }

        // Check for thermal runaway (confirmed by multiple sensors)
        if (thermal_runaway_confirmed) {
            // Trigger system-wide safe state
            TRIGGER_SAFE_STATE(ERR_THERMAL_RUNAWAY);
        }
    }
}
```

## Philosophy

> **Severity indicates the weight of an error in the system context - NOT whether the node should stop.**

The decision to halt belongs to local node logic. A battery monitoring node may report a severe error but continue running to monitor other cells. A communication node may stop on a warning-level error if continued operation risks sending corrupt data.

## Integration

1. Copy `Error_Corrutines/Inc/error_handler.h` and `Error_Corrutines/Src/error_handler.c` to your project
2. Add the include path to your build
3. Use `CAN_InitWithNodeId()` instead of `CAN_Init()` in your initialization
4. Include `error_handler.h` where you need to report errors

The error handler auto-detects the CAN type from HAL definitions:

- `HAL_CAN_MODULE_ENABLED` → uses bxCAN API
- `HAL_FDCAN_MODULE_ENABLED` → uses FDCAN API
