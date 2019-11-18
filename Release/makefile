################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ..\makefile.init

RM := del 

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include libraries\IWatchdog\src\subdir.mk
-include core\variant\subdir.mk
-include core\core\stm32\usb\hid\subdir.mk
-include core\core\stm32\usb\cdc\subdir.mk
-include core\core\stm32\usb\subdir.mk
-include core\core\stm32\LL\subdir.mk
-include core\core\stm32\HAL\subdir.mk
-include core\core\stm32\subdir.mk
-include core\core\avr\subdir.mk
-include core\core\subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ..\makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
AR += \
C:\Users\ahoel\Documents\sloeber-workspace\hyperlight_dma_sloeber\Release\arduino.ar \

ELF += \
hyperlight_dma_sloeber.elf \

HEX += \
hyperlight_dma_sloeber.hex \


# All Target
all:
	$(MAKE) --no-print-directory pre-build
	$(MAKE) --no-print-directory main-build

# Main-build Target
main-build: hyperlight_dma_sloeber

# Tool invocations
hyperlight_dma_sloeber: $(HEX)
	@echo 'Building target: $@'
	@echo 'Printing size:'
	"E:\Sloeber\arduinoPlugin\packages\STM32\tools\arm-none-eabi-gcc\8.2.1-1.7/bin/arm-none-eabi-size" -A "C:/Users/ahoel/Documents/sloeber-workspace/hyperlight_dma_sloeber/Release/hyperlight_dma_sloeber.elf"
	@echo 'Finished building target: $@'
	@echo ' '

C:\Users\ahoel\Documents\sloeber-workspace\hyperlight_dma_sloeber\Release\arduino.ar: $(AR_OBJ)
	@echo 'Starting archiver'
	"E:\Sloeber\arduinoPlugin\packages\STM32\tools\arm-none-eabi-gcc\8.2.1-1.7/bin/arm-none-eabi-gcc-ar" rcs     C:/Users/ahoel/Documents/sloeber-workspace/hyperlight_dma_sloeber/Release/arduino.ar $(AR_OBJ)
	@echo 'Finished building: $@'
	@echo ' '

hyperlight_dma_sloeber.elf: $(LINK_OBJ) $(AR)
	@echo 'Starting combiner'
	"E:\Sloeber\arduinoPlugin\packages\STM32\tools\arm-none-eabi-gcc\8.2.1-1.7/bin/arm-none-eabi-gcc" -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -g -Og  -Wl,--defsym=LD_FLASH_OFFSET=0 -Wl,--defsym=LD_MAX_SIZE=524288 -Wl,--defsym=LD_MAX_DATA_SIZE=131072 -Wl,--cref -Wl,--check-sections -Wl,--gc-sections -Wl,--entry=Reset_Handler -Wl,--unresolved-symbols=report-all -Wl,--warn-common "-TE:\Sloeber\arduinoPlugin\packages\STM32\hardware\stm32\1.7.0\variants\Hyperlight/ldscript.ld" "-Wl,-Map,C:/Users/ahoel/Documents/sloeber-workspace/hyperlight_dma_sloeber/Release/hyperlight_dma_sloeber.map"   "-LE:\Sloeber\arduinoPlugin\packages\STM32\tools\CMSIS\5.5.1/CMSIS/DSP/Lib/GCC/" -larm_cortexM4l_math -o "C:/Users/ahoel/Documents/sloeber-workspace/hyperlight_dma_sloeber/Release/hyperlight_dma_sloeber.elf" "-LC:/Users/ahoel/Documents/sloeber-workspace/hyperlight_dma_sloeber/Release" -Wl,--start-group   $(LINK_OBJ)  -Wl,--whole-archive  C:/Users/ahoel/Documents/sloeber-workspace/hyperlight_dma_sloeber/Release/arduino.ar  -Wl,--no-whole-archive -lc -Wl,--end-group -lm -lgcc -lstdc++
	@echo 'Finished building: $@'
	@echo ' '

hyperlight_dma_sloeber.hex: $(ELF)
	@echo 'Do all objcopy commands'
	${A.RECIPE.OBJCOPY.BIN.PATTERN}
	${A.RECIPE.OBJCOPY.HEX.PATTERN}
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(C_UPPER_DEPS)$(PDE_DEPS)$(C_DEPS)$(AR)$(CC_DEPS)$(AR_OBJ)$(C++_DEPS)$(LINK_OBJ)$(CXX_DEPS)$(ASM_DEPS)$(HEX)$(INO_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS)$(ELF)$(CPP_DEPS)
	-@echo ' '

pre-build:
	-${A.RECIPE.HOOKS.PREBUILD.1.PATTERN}
	-@echo ' '

.PHONY: all clean dependents main-build pre-build

-include ..\makefile.targets
