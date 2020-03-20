################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Utilities/Components/st7789h2/st7789h2.c 

OBJS += \
./Utilities/Components/st7789h2/st7789h2.o 

C_DEPS += \
./Utilities/Components/st7789h2/st7789h2.d 


# Each subdirectory must supply rules for building sources it contributes
Utilities/Components/st7789h2/st7789h2.o: ../Utilities/Components/st7789h2/st7789h2.c
	arm-none-eabi-gcc "$<" -std=gnu11 -g3 -DSTM32L476xx -c -I"C:/Users/booth/Documents/ENEE408A/ComponentTests/80MHzUART/inc" -I"C:/Users/booth/Documents/ENEE408A/ComponentTests/80MHzUART/HAL_Driver/Inc" -I"C:/Users/booth/Documents/ENEE408A/ComponentTests/80MHzUART/CMSIS/core" -I"C:/Users/booth/Documents/ENEE408A/ComponentTests/80MHzUART/CMSIS/device" -I"C:/Users/booth/Documents/ENEE408A/ComponentTests/80MHzUART/Utilities/STM32L476G-Discovery" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Utilities/Components/st7789h2/st7789h2.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

