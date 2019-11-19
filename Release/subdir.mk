################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
..\hyperlight.cpp \
..\sloeber.ino.cpp 

LINK_OBJ += \
.\hyperlight.cpp.o \
.\sloeber.ino.cpp.o 

CPP_DEPS += \
.\hyperlight.cpp.d \
.\sloeber.ino.cpp.d 


# Each subdirectory must supply rules for building sources it contributes
hyperlight.cpp.o: ..\hyperlight.cpp
	@echo 'Building file: $<'
	@echo 'Starting C++ compile'
	"E:\Sloeber\arduinoPlugin\packages\STM32\tools\arm-none-eabi-gcc\8.2.1-1.7/bin/arm-none-eabi-g++" -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb "@C:/Users/ahoel/Documents/sloeber-workspace/hyperlight_dma_sloeber/Release/sketch/build_opt.h" -c -g -Og -Wall -Wextra -std=gnu++14 -ffunction-sections -fdata-sections -nostdlib -fno-threadsafe-statics --param max-inline-insns-single=500 -fno-rtti -fno-exceptions -fno-use-cxa-atexit -MMD "-I" "-IE:\Sloeber\arduinoPlugin\packages\STM32\hardware\stm32\1.7.0\cores\arduino/avr" "-IE:\Sloeber\arduinoPlugin\packages\STM32\hardware\stm32\1.7.0\cores\arduino/stm32" "-IE:\Sloeber\arduinoPlugin\packages\STM32\hardware\stm32\1.7.0\cores\arduino/stm32/LL" "-IE:\Sloeber\arduinoPlugin\packages\STM32\hardware\stm32\1.7.0\cores\arduino/stm32/usb" "-IE:\Sloeber\arduinoPlugin\packages\STM32\hardware\stm32\1.7.0\cores\arduino/stm32/usb/hid" "-IE:\Sloeber\arduinoPlugin\packages\STM32\hardware\stm32\1.7.0\cores\arduino/stm32/usb/cdc" "-IE:\Sloeber\/arduinoPlugin/packages/STM32/hardware/stm32/1.7.0/system/Drivers/STM32F4xx_HAL_Driver/Inc/" "-IE:\Sloeber\/arduinoPlugin/packages/STM32/hardware/stm32/1.7.0/system/Drivers/STM32F4xx_HAL_Driver/Src/" "-IE:\Sloeber\/arduinoPlugin/packages/STM32/hardware/stm32/1.7.0/system/STM32F4xx/" "-IE:\Sloeber\/arduinoPlugin/packages/STM32/hardware/stm32/1.7.0/system/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" "-IE:\Sloeber\/arduinoPlugin/packages/STM32/hardware/stm32/1.7.0/system/Middlewares/ST/STM32_USB_Device_Library/Core/Src" -DSTM32F4xx -DARDUINO=10802 -DARDUINO_Hyperlight -DARDUINO_ARCH_STM32 -DBOARD_NAME="Hyperlight"  -DSTM32F407xx  -DHAL_UART_MODULE_ENABLED  "-IE:\Sloeber\arduinoPlugin\packages\STM32\tools\CMSIS\5.5.1/CMSIS/Core/Include/" "-IE:\Sloeber\/arduinoPlugin/packages/STM32/hardware/stm32/1.7.0/system/Drivers/CMSIS/Device/ST/STM32F4xx/Include/" "-IE:\Sloeber\/arduinoPlugin/packages/STM32/hardware/stm32/1.7.0/system/Drivers/CMSIS/Device/ST/STM32F4xx/Source/Templates/gcc/"   -I"E:\Sloeber\arduinoPlugin\packages\STM32\hardware\stm32\1.7.0\cores\arduino" -I"E:\Sloeber\arduinoPlugin\packages\STM32\hardware\stm32\1.7.0\variants\Hyperlight" -I"E:\Sloeber\arduinoPlugin\packages\STM32\hardware\stm32\1.7.0\libraries\IWatchdog\src" -I"E:\Sloeber\arduinoPlugin\packages\STM32\hardware\stm32\1.7.0\libraries\SPI\src" -I"C:\Users\ahoel\Documents\Arduino\libraries\Artnet" -I"E:\Sloeber\arduinoPlugin\libraries\Ethernet\2.0.0\src" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -D__IN_ECLIPSE__=1 -x c++ "$<"  -o  "$@"
	@echo 'Finished building: $<'
	@echo ' '

sloeber.ino.cpp.o: ..\sloeber.ino.cpp
	@echo 'Building file: $<'
	@echo 'Starting C++ compile'
	"E:\Sloeber\arduinoPlugin\packages\STM32\tools\arm-none-eabi-gcc\8.2.1-1.7/bin/arm-none-eabi-g++" -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb "@C:/Users/ahoel/Documents/sloeber-workspace/hyperlight_dma_sloeber/Release/sketch/build_opt.h" -c -g -Og -Wall -Wextra -std=gnu++14 -ffunction-sections -fdata-sections -nostdlib -fno-threadsafe-statics --param max-inline-insns-single=500 -fno-rtti -fno-exceptions -fno-use-cxa-atexit -MMD "-I" "-IE:\Sloeber\arduinoPlugin\packages\STM32\hardware\stm32\1.7.0\cores\arduino/avr" "-IE:\Sloeber\arduinoPlugin\packages\STM32\hardware\stm32\1.7.0\cores\arduino/stm32" "-IE:\Sloeber\arduinoPlugin\packages\STM32\hardware\stm32\1.7.0\cores\arduino/stm32/LL" "-IE:\Sloeber\arduinoPlugin\packages\STM32\hardware\stm32\1.7.0\cores\arduino/stm32/usb" "-IE:\Sloeber\arduinoPlugin\packages\STM32\hardware\stm32\1.7.0\cores\arduino/stm32/usb/hid" "-IE:\Sloeber\arduinoPlugin\packages\STM32\hardware\stm32\1.7.0\cores\arduino/stm32/usb/cdc" "-IE:\Sloeber\/arduinoPlugin/packages/STM32/hardware/stm32/1.7.0/system/Drivers/STM32F4xx_HAL_Driver/Inc/" "-IE:\Sloeber\/arduinoPlugin/packages/STM32/hardware/stm32/1.7.0/system/Drivers/STM32F4xx_HAL_Driver/Src/" "-IE:\Sloeber\/arduinoPlugin/packages/STM32/hardware/stm32/1.7.0/system/STM32F4xx/" "-IE:\Sloeber\/arduinoPlugin/packages/STM32/hardware/stm32/1.7.0/system/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" "-IE:\Sloeber\/arduinoPlugin/packages/STM32/hardware/stm32/1.7.0/system/Middlewares/ST/STM32_USB_Device_Library/Core/Src" -DSTM32F4xx -DARDUINO=10802 -DARDUINO_Hyperlight -DARDUINO_ARCH_STM32 -DBOARD_NAME="Hyperlight"  -DSTM32F407xx  -DHAL_UART_MODULE_ENABLED  "-IE:\Sloeber\arduinoPlugin\packages\STM32\tools\CMSIS\5.5.1/CMSIS/Core/Include/" "-IE:\Sloeber\/arduinoPlugin/packages/STM32/hardware/stm32/1.7.0/system/Drivers/CMSIS/Device/ST/STM32F4xx/Include/" "-IE:\Sloeber\/arduinoPlugin/packages/STM32/hardware/stm32/1.7.0/system/Drivers/CMSIS/Device/ST/STM32F4xx/Source/Templates/gcc/"   -I"E:\Sloeber\arduinoPlugin\packages\STM32\hardware\stm32\1.7.0\cores\arduino" -I"E:\Sloeber\arduinoPlugin\packages\STM32\hardware\stm32\1.7.0\variants\Hyperlight" -I"E:\Sloeber\arduinoPlugin\packages\STM32\hardware\stm32\1.7.0\libraries\IWatchdog\src" -I"E:\Sloeber\arduinoPlugin\packages\STM32\hardware\stm32\1.7.0\libraries\SPI\src" -I"C:\Users\ahoel\Documents\Arduino\libraries\Artnet" -I"E:\Sloeber\arduinoPlugin\libraries\Ethernet\2.0.0\src" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -D__IN_ECLIPSE__=1 -x c++ "$<"  -o  "$@"

	@echo 'Finished building: $<'
	@echo ' '


