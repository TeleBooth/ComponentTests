################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_UPPER_SRCS += \
../startup/startup_stm32l476xx.S 

OBJS += \
./startup/startup_stm32l476xx.o 


# Each subdirectory must supply rules for building sources it contributes
startup/%.o: ../startup/%.S
	arm-none-eabi-gcc -g3 -c -x assembler-with-cpp --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@" "$<"

