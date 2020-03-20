################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include startup/subdir.mk
-include src/subdir.mk
-include Utilities/STM32L476G-Discovery/subdir.mk
-include Utilities/Components/st7789h2/subdir.mk
-include Utilities/Components/mfxstm32l152/subdir.mk
-include Utilities/Components/lps22hb/subdir.mk
-include Utilities/Components/lis3mdl/subdir.mk
-include Utilities/Components/hx8347g/subdir.mk
-include Utilities/Components/cs42l51/subdir.mk
-include HAL_Driver/Src/Legacy/subdir.mk
-include HAL_Driver/Src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
80MHzUART.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
80MHzUART.list \

OBJCOPY_BIN += \
80MHzUART.bin \


# All Target
all: 80MHzUART.elf secondary-outputs

# Tool invocations
80MHzUART.elf: $(OBJS) $(USER_OBJS) C:\Users\booth\Documents\ENEE408A\ComponentTests\80MHzUART\LinkerScript.ld
	arm-none-eabi-gcc -o "80MHzUART.elf" @"objects.list" $(USER_OBJS) $(LIBS) -T"C:\Users\booth\Documents\ENEE408A\ComponentTests\80MHzUART\LinkerScript.ld" --specs=nosys.specs -Wl,-Map="80MHzUART.map" -Wl,--gc-sections -static -L"C:\Users\booth\Documents\ENEE408A\ComponentTests\80MHzUART\inc" -L"C:\Users\booth\Documents\ENEE408A\ComponentTests\80MHzUART\HAL_Driver\Inc" --specs=nano.specs -mfloat-abi=soft -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

80MHzUART.list: $(EXECUTABLES)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "80MHzUART.list"
	@echo 'Finished building: $@'
	@echo ' '

80MHzUART.bin: $(EXECUTABLES)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "80MHzUART.bin"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) *
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_BIN)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
