################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Utilities/Components/ov9655/ov9655.c 

OBJS += \
./Utilities/Components/ov9655/ov9655.o 

C_DEPS += \
./Utilities/Components/ov9655/ov9655.d 


# Each subdirectory must supply rules for building sources it contributes
Utilities/Components/ov9655/%.o: ../Utilities/Components/ov9655/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DSTM32 -DSTM32L4 -DSTM32L476VGTx -DSTM32L476G_DISCO -DDEBUG -DSTM32L476xx -DUSE_HAL_DRIVER -I"C:/Users/pablo/Google Drive/College/ENEE 408A/ComponentTests/UART/Utilities/Components/m24sr" -I"C:/Users/pablo/Google Drive/College/ENEE 408A/ComponentTests/UART/Utilities/Components/cs43l22" -I"C:/Users/pablo/Google Drive/College/ENEE 408A/ComponentTests/UART/Utilities/Components/mx25lm51245g" -I"C:/Users/pablo/Google Drive/College/ENEE 408A/ComponentTests/UART/Utilities/Components/mfxstm32l152" -I"C:/Users/pablo/Google Drive/College/ENEE 408A/ComponentTests/UART/CMSIS/device" -I"C:/Users/pablo/Google Drive/College/ENEE 408A/ComponentTests/UART/Utilities/Components/cs42l51" -I"C:/Users/pablo/Google Drive/College/ENEE 408A/ComponentTests/UART/Utilities/Components/n25q128a" -I"C:/Users/pablo/Google Drive/College/ENEE 408A/ComponentTests/UART/Utilities/Components/st7735" -I"C:/Users/pablo/Google Drive/College/ENEE 408A/ComponentTests/UART/Utilities/Components/ft3x67" -I"C:/Users/pablo/Google Drive/College/ENEE 408A/ComponentTests/UART/Utilities/Components/rk043fn48h" -I"C:/Users/pablo/Google Drive/College/ENEE 408A/ComponentTests/UART/Utilities/Components/hx8347g" -I"C:/Users/pablo/Google Drive/College/ENEE 408A/ComponentTests/UART/HAL_Driver/Inc/Legacy" -I"C:/Users/pablo/Google Drive/College/ENEE 408A/ComponentTests/UART/Utilities/Components/ov9655" -I"C:/Users/pablo/Google Drive/College/ENEE 408A/ComponentTests/UART/Utilities/Components/stmpe1600" -I"C:/Users/pablo/Google Drive/College/ENEE 408A/ComponentTests/UART/Utilities/Components/Common" -I"C:/Users/pablo/Google Drive/College/ENEE 408A/ComponentTests/UART/Utilities/Components/l3gd20" -I"C:/Users/pablo/Google Drive/College/ENEE 408A/ComponentTests/UART/Utilities/Components/lsm6dsl" -I"C:/Users/pablo/Google Drive/College/ENEE 408A/ComponentTests/UART/HAL_Driver/Inc" -I"C:/Users/pablo/Google Drive/College/ENEE 408A/ComponentTests/UART/Utilities/Components/stmpe811" -I"C:/Users/pablo/Google Drive/College/ENEE 408A/ComponentTests/UART/Utilities/Components/wm8994" -I"C:/Users/pablo/Google Drive/College/ENEE 408A/ComponentTests/UART/Utilities/Components/ft5336" -I"C:/Users/pablo/Google Drive/College/ENEE 408A/ComponentTests/UART/Utilities/Components/n25q256a" -I"C:/Users/pablo/Google Drive/College/ENEE 408A/ComponentTests/UART/inc" -I"C:/Users/pablo/Google Drive/College/ENEE 408A/ComponentTests/UART/Utilities/Components/hts221" -I"C:/Users/pablo/Google Drive/College/ENEE 408A/ComponentTests/UART/Utilities/Components/ls016b8uy" -I"C:/Users/pablo/Google Drive/College/ENEE 408A/ComponentTests/UART/Utilities/Components/ft6x06" -I"C:/Users/pablo/Google Drive/College/ENEE 408A/ComponentTests/UART/Utilities/Components/lis3mdl" -I"C:/Users/pablo/Google Drive/College/ENEE 408A/ComponentTests/UART/Utilities/Components/lps22hb" -I"C:/Users/pablo/Google Drive/College/ENEE 408A/ComponentTests/UART/Utilities/Components/st7789h2" -I"C:/Users/pablo/Google Drive/College/ENEE 408A/ComponentTests/UART/Utilities/Components/lsm303c" -I"C:/Users/pablo/Google Drive/College/ENEE 408A/ComponentTests/UART/Utilities/Components/iss66wvh8m8" -I"C:/Users/pablo/Google Drive/College/ENEE 408A/ComponentTests/UART/Utilities/Components/lsm303dlhc" -I"C:/Users/pablo/Google Drive/College/ENEE 408A/ComponentTests/UART/CMSIS/core" -I"C:/Users/pablo/Google Drive/College/ENEE 408A/ComponentTests/UART/Utilities/Components/mx25r6435f" -I"C:/Users/pablo/Google Drive/College/ENEE 408A/ComponentTests/UART/Utilities/STM32L476G-Discovery" -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


