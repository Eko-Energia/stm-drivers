# STM32 Peripheral ADC Driver

## Title

**STM32 Peripheral ADC Driver (TDD Implementation)**

## Description

This directory contains source and header files for the ADC driver, designed to optimize the workflow with Analog-to-Digital Converters in the **Perła** racing car projects.

The main purpose of this ADC driver is to provide a clear API and macros that simplify the ADC workflow while ensuring firmware universality across various STM32 cores (for now, it supports F1 and F3 cores - in future implementation will support wider range of MCUs). The modular structure facilitates locating, analyzing, and debugging software errors at the low-level layer.

> **NOTE:** The driver is built using direct register access and low-level features of the ADC controller embedded in STM32 cores.

---

## Status & Development (TDD)

The project is being developed using the **TDD (Test-Driven Development)** methodology. This ensures high reliability through a suite of diagnostic functions that verify the driver's correctness within the dedicated hardware solution.

- **Current Support:** At this stage of implementation, the driver supports **multi-channel readout** without **DMA** support.
- **Documentation:** The user manual will be provided and updated continuously with each new functional update.

---

## Project Structure

The project architecture is divided into modules to ensure a transparent flow and separation of logic:

### 📂 Inc (Headers)

- **adc_driver.h**: API function prototypes, macro definitions, and data structures supporting a logical workflow.
- **adc_utils.h**: Declarations of low-level utilities operating directly on ADC registers.
- **adc_conf.h**: Used for parameters that require manual user configuration.

### 📂 Src (Sources)

- **adc_driver.c**: Implementation of core API functions and driver variable definitions.
- **adc_utils.c**: Operational functions extracting and processing data directly from the ADC controller registers.

### 📂 Tests (Quality Assurance)

- **Src/adc_ut.c**: Unit testing modules. Currently, these contain on-target **static tests**. In the final phase of implementation, they will be expanded into **diagnostic functions** to confirm on-target operational correctness.

---

## Application

In `.ioc` file, in section for `ADC`, please make sure that `DISCONTINUOUS` flag is set to `ENABLED`. Also ensure that `Number of DIscontinuous Conversion` is set to `1`.
While working wwith multiple channels, ensures correct `Number of Conversion`, which value should be equal to currently set channels. `.ioc file` whill automatically increase `number of ranks` - make sure, that every rank is assigned to `different channel`.

To ensure stable operation and prevent `HardFault` errors, please call the **Init** function before working with the ADC. If Dual Mode is required, call the `Init_Multimode` function within the ADC Slave initialization (MX_ADC_Init).

## Manual

_The detailed manual will be expanded as development progresses._

1.  For a proper workflow, initialize the `ADC_ChannelsConfigTypeDef` object for each ADC instance.

```c
ADC_ChannelsConfigTypeDefs cadc1;         //< Channels struct object - stores rank configurations for ADC1
ADC_ChannelsConfigTypeDefs cadc2;         //< Channels struct object - stores rank configurations for ADC2
```

2.  Call the `ADC_Init` function within each `MX_ADC_Init` function for all ADCs you are working with.

```c
/*
 * @brief  ADC initialization function, initialize type of conversion, set correct ADC's regs' values
 * @param  hadc - handle to ADC instance
 * @retval status of HAL's operation
 */
HAL_StatusTypeDef ADC_Init(ADC_HandleTypeDef* hadc, ADC_ChannelsConfigTypeDefs* cadc);
```

3.  In **adc_conf.h** change `ADCx_USED_CHANNELS` stored number, to amount of selected channels in **.ioc file**.
    > **NOTE:** In `ADCx_USED_CHANNELS`, **x** stand for index of ADC instance - can be `ADC1_..` or `ADC2_...`.

```c
#define ADC1_USED_CHANNELS (3)  //< Macro defines number of channels for ADC1,
#define ADC2_USED_CHANNELS (3)  //< Macro defines number of channels for ADC2,
```

---

## Author

**Bartosz Rychlicki** [bartoszrychl@student.agh.edu.pl](mailto:bartoszrychl@student.agh.edu.pl)

**AGH Eko-Energy** Software Department
