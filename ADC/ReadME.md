# ⚠️ STM32 Peripheral ADC Driver (v2.0 - Work in Progress)

**Status:** New Development Version (Under Construction).  
This driver is currently under active development. The current implementation focuses on providing a stable foundation for high-precision measurements in specific operating modes.

---

## 🚀 Key Features (Current Support)

The current version (v2.0-alpha) supports the following configuration:
* **Target Auto-Sensing**: Automatically detects the specific STM32 microcontroller family and ensures correct register mappings.
* **Independent Mode**: Full support for standalone ADC operations.
* **Non-Continuous / Discontinuous Mode**: Support for sequential conversion groups.
* **Polling Mode**: Data retrieval via register/flag monitoring (No DMA).
* **Built-in Calibration**: Automated hardware calibration and rank detection during initialization.

---

## 🧪 Test-Driven Development (TDD)

This driver is implemented using the **TDD (Test-Driven Development)** methodology. Every feature is preceded by unit tests to ensure high code reliability and maintainability.

* **Current Test Scope**: Full Unit Tests are provided for **Independent, Non-Continuous mode without DMA support**.
* **Future Scope**: Every supported operating mode will include a dedicated suite of unit tests.
* **Cross-Platform Validation**: Tests are being designed to ensure functional parity not only on the **F3** family but also for **F1, F2, and F4** series in the future.

---

---

## 🛠 Integration Manual

Follow these steps to ensure a stable implementation and avoid `HardFault` exceptions.

### STEP 1: Global Declarations
For now, there is no need to declare objects globally. In future, when driver will be supporting DMA usage, global DMA buffer will be provided and manual to its implementation. 


### STEP 2: Configuration Check
Before starting conversions, you must verify the following macro in `adc_driver.h`:

```c
#define ADC_USED_CHANNELS // Must equal the total number of channels enabled for that specific ADC.
```
---

## 📂 File Structure

1.  **`Inc/adc_driver.h`**: Function prototypes, macros, and configuration structures.
2.  **`Inc/stm32_family.h`**: STM32 family definitions for cross-platform portability.
3.  **`Src/adc_driver.c`**: Core driver logic and variable definitions.

---
### 🚀 Future Roadmap

* **Advanced Conversion Modes**: Implementation of Continuous conversion logic and Synchronized Dual Mode (Master/Slave) support.
* **Enhanced DMA Integration**: Optimization for both **Normal** and **Circular** DMA modes to ensure high-speed, buffered data transfer.
* **Intelligent Automation**: 
    * **Auto-Target & Rank Detection**: Automated peripheral and sequence discovery.
    * **Auto-Restart Logic**: Implementation of an automatic driver recovery mechanism to handle peripheral desynchronization or bus errors without requiring a full system reset.
* **Cross-Family Expansion**: Extending full unit test coverage and register mapping for **STM32 F1, F2, and F4** families to ensure universal compatibility.

---

## ✍️ Author
**Bartosz Rychlicki** AGH Eko-Energy — Software Department  
Email: bartoszrychl@student.agh.edu.pl
