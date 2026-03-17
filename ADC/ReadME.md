# STM32 Peripheral ADC Driver

A high-performance, low-level driver designed to simplify and standardize the workflow with ADC peripherals across the STM32 family. This driver serves as a universal abstraction layer, making firmware more maintainable and easier to debug by providing a consistent API for both simple and complex ADC configurations.

---

## 🚀 Key Features

* **Target Auto-Sensing**: The driver automatically detects the specific STM32 microcontroller family used in the project, ensuring correct register mappings without manual configuration.
* **Universal Compatibility**: Designed to be compatible with almost all STM32 cores by utilizing low-level features and direct register access.
* **Automatic Channel Detection**: Features auto-detection of the number of channels enabled for conversion.
* **Synchronized Dual Mode**: Full support for **Multimode (Master/Slave)** conversions.
* **DMA Support**: Optimized for both **Normal** and **Circular** DMA modes.
* **Flexible Conversion**: Supports both **Continuous** and **Non-Continuous** conversion modes.
* **Built-in Calibration**: Automatically handles ADC calibration and rank detection during initialization.

---

## 🛠 Integration Manual

Follow these steps to ensure a stable implementation and avoid `HardFault` exceptions.

### STEP 1: Global Declarations
Declare objects to store channel configurations and quantized values globally.

```c
#include "adc_driver.h"

/* --- ADC1 Configuration Objects --- */
ADC_ChannelsTypeDef cadc1; // Object storing ADC1 channel config
ADC_BufferTypeDef   badc1; // Object storing ADC1 quantized values

/* --- ADC2 Configuration Objects --- */
ADC_ChannelsTypeDef cadc2; 
ADC_BufferTypeDef   badc2;
```


### STEP 2: Independent Mode Setup
Inside the `MX_ADC_Init` function, call the initialization. This enables auto-detection and calibration.

```c
/* USER CODE BEGIN ADC1_Init 2 */

if (ADC_Init(&hadc1, &badc1, &cadc1) == HAL_OK) 
{
    /* ADC initialized and calibrated successfully */
}
else 
{
    Error_Handler(); 
}

/* USER CODE END ADC1_Init 2 */
```


### STEP 3: Dual Mode Setup (DUAL MODE)
If using two ADCs in Dual Mode, call the multimode initialization within the **ADC Slave's** init function.


```c
/* USER CODE BEGIN ADC2_Init 2 */

/* Slave shares the Master's buffer for synchronized data */
if (ADC_InitMultimode(&hadc1, &badc1) != HAL_OK) 
{
    Error_Handler();
}

/* USER CODE END ADC2_Init 2 */
```


### STEP 4: Configuration Check
Before starting conversions, you must verify the following macro in `adc_driver.h`:

```c
#define ADC_MAX_CHANNELS // Must equal the total number of channels enabled for that specific ADC.
```
---

## 📂 File Structure

1.  **`Inc/adc_driver.h`**: Function prototypes, macros, and configuration structures.
2.  **`Inc/stm32_family.h`**: STM32 family definitions for cross-platform portability.
3.  **`Src/adc_driver.c`**: Core driver logic and variable definitions.

---

## 📊 Project Status

* **Core Logic**: ✅ DONE (Register-based implementation).
* **Testing**: ✅ DONE (Validation and code tests completed).

### 🚀 Future Roadmap
* **Auto-Restart Logic**: Implementation of an automatic driver recovery mechanism to handle peripheral desynchronization or bus errors without requiring a full system reset.

---

## ✍️ Author
**Bartosz Rychlicki** AGH Eko-Energy — Software Department  
Email: bartoszrychl@student.agh.edu.pl
