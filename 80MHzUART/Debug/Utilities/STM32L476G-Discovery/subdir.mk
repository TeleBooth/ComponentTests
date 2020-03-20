################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Utilities/STM32L476G-Discovery/stm32l476g_discovery.c \
../Utilities/STM32L476G-Discovery/stm32l476g_discovery_audio.c \
../Utilities/STM32L476G-Discovery/stm32l476g_discovery_compass.c \
../Utilities/STM32L476G-Discovery/stm32l476g_discovery_glass_lcd.c \
../Utilities/STM32L476G-Discovery/stm32l476g_discovery_gyroscope.c \
../Utilities/STM32L476G-Discovery/stm32l476g_discovery_idd.c \
../Utilities/STM32L476G-Discovery/stm32l476g_discovery_qspi.c 

OBJS += \
./Utilities/STM32L476G-Discovery/stm32l476g_discovery.o \
./Utilities/STM32L476G-Discovery/stm32l476g_discovery_audio.o \
./Utilities/STM32L476G-Discovery/stm32l476g_discovery_compass.o \
./Utilities/STM32L476G-Discovery/stm32l476g_discovery_glass_lcd.o \
./Utilities/STM32L476G-Discovery/stm32l476g_discovery_gyroscope.o \
./Utilities/STM32L476G-Discovery/stm32l476g_discovery_idd.o \
./Utilities/STM32L476G-Discovery/stm32l476g_discovery_qspi.o 

C_DEPS += \
./Utilities/STM32L476G-Discovery/stm32l476g_discovery.d \
./Utilities/STM32L476G-Discovery/stm32l476g_discovery_audio.d \
./Utilities/STM32L476G-Discovery/stm32l476g_discovery_compass.d \
./Utilities/STM32L476G-Discovery/stm32l476g_discovery_glass_lcd.d \
./Utilities/STM32L476G-Discovery/stm32l476g_discovery_gyroscope.d \
./Utilities/STM32L476G-Discovery/stm32l476g_discovery_idd.d \
./Utilities/STM32L476G-Discovery/stm32l476g_discovery_qspi.d 


# Each subdirectory must supply rules for building sources it contributes
Utilities/STM32L476G-Discovery/stm32l476g_discovery.o: ../Utilities/STM32L476G-Discovery/stm32l476g_discovery.c
	arm-none-eabi-gcc "$<" -std=gnu11 -g3 -DSTM32L476xx -c -I"C:/Users/booth/Documents/ENEE408A/ComponentTests/80MHzUART/inc" -I"C:/Users/booth/Documents/ENEE408A/ComponentTests/80MHzUART/HAL_Driver/Inc" -I"C:/Users/booth/Documents/ENEE408A/ComponentTests/80MHzUART/CMSIS/core" -I"C:/Users/booth/Documents/ENEE408A/ComponentTests/80MHzUART/CMSIS/device" -I"C:/Users/booth/Documents/ENEE408A/ComponentTests/80MHzUART/Utilities/STM32L476G-Discovery" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Utilities/STM32L476G-Discovery/stm32l476g_discovery.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Utilities/STM32L476G-Discovery/stm32l476g_discovery_audio.o: ../Utilities/STM32L476G-Discovery/stm32l476g_discovery_audio.c
	arm-none-eabi-gcc "$<" -std=gnu11 -g3 -DSTM32L476xx -c -I"C:/Users/booth/Documents/ENEE408A/ComponentTests/80MHzUART/inc" -I"C:/Users/booth/Documents/ENEE408A/ComponentTests/80MHzUART/HAL_Driver/Inc" -I"C:/Users/booth/Documents/ENEE408A/ComponentTests/80MHzUART/CMSIS/core" -I"C:/Users/booth/Documents/ENEE408A/ComponentTests/80MHzUART/CMSIS/device" -I"C:/Users/booth/Documents/ENEE408A/ComponentTests/80MHzUART/Utilities/STM32L476G-Discovery" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Utilities/STM32L476G-Discovery/stm32l476g_discovery_audio.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Utilities/STM32L476G-Discovery/stm32l476g_discovery_compass.o: ../Utilities/STM32L476G-Discovery/stm32l476g_discovery_compass.c
	arm-none-eabi-gcc "$<" -std=gnu11 -g3 -DSTM32L476xx -c -I"C:/Users/booth/Documents/ENEE408A/ComponentTests/80MHzUART/inc" -I"C:/Users/booth/Documents/ENEE408A/ComponentTests/80MHzUART/HAL_Driver/Inc" -I"C:/Users/booth/Documents/ENEE408A/ComponentTests/80MHzUART/CMSIS/core" -I"C:/Users/booth/Documents/ENEE408A/ComponentTests/80MHzUART/CMSIS/device" -I"C:/Users/booth/Documents/ENEE408A/ComponentTests/80MHzUART/Utilities/STM32L476G-Discovery" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Utilities/STM32L476G-Discovery/stm32l476g_discovery_compass.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Utilities/STM32L476G-Discovery/stm32l476g_discovery_glass_lcd.o: ../Utilities/STM32L476G-Discovery/stm32l476g_discovery_glass_lcd.c
	arm-none-eabi-gcc "$<" -std=gnu11 -g3 -DSTM32L476xx -c -I"C:/Users/booth/Documents/ENEE408A/ComponentTests/80MHzUART/inc" -I"C:/Users/booth/Documents/ENEE408A/ComponentTests/80MHzUART/HAL_Driver/Inc" -I"C:/Users/booth/Documents/ENEE408A/ComponentTests/80MHzUART/CMSIS/core" -I"C:/Users/booth/Documents/ENEE408A/ComponentTests/80MHzUART/CMSIS/device" -I"C:/Users/booth/Documents/ENEE408A/ComponentTests/80MHzUART/Utilities/STM32L476G-Discovery" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Utilities/STM32L476G-Discovery/stm32l476g_discovery_glass_lcd.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Utilities/STM32L476G-Discovery/stm32l476g_discovery_gyroscope.o: ../Utilities/STM32L476G-Discovery/stm32l476g_discovery_gyroscope.c
	arm-none-eabi-gcc "$<" -std=gnu11 -g3 -DSTM32L476xx -c -I"C:/Users/booth/Documents/ENEE408A/ComponentTests/80MHzUART/inc" -I"C:/Users/booth/Documents/ENEE408A/ComponentTests/80MHzUART/HAL_Driver/Inc" -I"C:/Users/booth/Documents/ENEE408A/ComponentTests/80MHzUART/CMSIS/core" -I"C:/Users/booth/Documents/ENEE408A/ComponentTests/80MHzUART/CMSIS/device" -I"C:/Users/booth/Documents/ENEE408A/ComponentTests/80MHzUART/Utilities/STM32L476G-Discovery" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Utilities/STM32L476G-Discovery/stm32l476g_discovery_gyroscope.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Utilities/STM32L476G-Discovery/stm32l476g_discovery_idd.o: ../Utilities/STM32L476G-Discovery/stm32l476g_discovery_idd.c
	arm-none-eabi-gcc "$<" -std=gnu11 -g3 -DSTM32L476xx -c -I"C:/Users/booth/Documents/ENEE408A/ComponentTests/80MHzUART/inc" -I"C:/Users/booth/Documents/ENEE408A/ComponentTests/80MHzUART/HAL_Driver/Inc" -I"C:/Users/booth/Documents/ENEE408A/ComponentTests/80MHzUART/CMSIS/core" -I"C:/Users/booth/Documents/ENEE408A/ComponentTests/80MHzUART/CMSIS/device" -I"C:/Users/booth/Documents/ENEE408A/ComponentTests/80MHzUART/Utilities/STM32L476G-Discovery" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Utilities/STM32L476G-Discovery/stm32l476g_discovery_idd.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Utilities/STM32L476G-Discovery/stm32l476g_discovery_qspi.o: ../Utilities/STM32L476G-Discovery/stm32l476g_discovery_qspi.c
	arm-none-eabi-gcc "$<" -std=gnu11 -g3 -DSTM32L476xx -c -I"C:/Users/booth/Documents/ENEE408A/ComponentTests/80MHzUART/inc" -I"C:/Users/booth/Documents/ENEE408A/ComponentTests/80MHzUART/HAL_Driver/Inc" -I"C:/Users/booth/Documents/ENEE408A/ComponentTests/80MHzUART/CMSIS/core" -I"C:/Users/booth/Documents/ENEE408A/ComponentTests/80MHzUART/CMSIS/device" -I"C:/Users/booth/Documents/ENEE408A/ComponentTests/80MHzUART/Utilities/STM32L476G-Discovery" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Utilities/STM32L476G-Discovery/stm32l476g_discovery_qspi.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

