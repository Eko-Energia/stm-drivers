################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../EKO_Drivers/CAN/Src/can_driver.c 

OBJS += \
./EKO_Drivers/CAN/Src/can_driver.o 

C_DEPS += \
./EKO_Drivers/CAN/Src/can_driver.d 


# Each subdirectory must supply rules for building sources it contributes
EKO_Drivers/CAN/Src/%.o EKO_Drivers/CAN/Src/%.su EKO_Drivers/CAN/Src/%.cyclo: ../EKO_Drivers/CAN/Src/%.c EKO_Drivers/CAN/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F303x8 -c -I../Core/Inc -I../Drivers/STM32F3xx_HAL_Driver/Inc -I../Drivers/STM32F3xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Include -I../EKO_Drivers/ADC/Inc -I../EKO_Drivers/CAN/Inc -I../PCBCells_driver/ADC/Inc -I../PCBCells_driver/CAN/Inc -I../PCBCells_driver/PCB_Cells/Inc -I../Drivers/CMSIS/Device/ST/STM32F3xx/Include -O0 -ffunction-sections -fdata-sections -Wall -Werror -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-EKO_Drivers-2f-CAN-2f-Src

clean-EKO_Drivers-2f-CAN-2f-Src:
	-$(RM) ./EKO_Drivers/CAN/Src/can_driver.cyclo ./EKO_Drivers/CAN/Src/can_driver.d ./EKO_Drivers/CAN/Src/can_driver.o ./EKO_Drivers/CAN/Src/can_driver.su

.PHONY: clean-EKO_Drivers-2f-CAN-2f-Src

