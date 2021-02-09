################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../device/system_MK64F12.c 

OBJS += \
./device/system_MK64F12.o 

C_DEPS += \
./device/system_MK64F12.d 


# Each subdirectory must supply rules for building sources it contributes
device/%.o: ../device/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DCPU_MK64FN1M0VLL12 -DCPU_MK64FN1M0VLL12_cm4 -DFSL_RTOS_BM -DSDK_OS_BAREMETAL -DSDK_DEBUGCONSOLE=0 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -DSERIAL_PORT_TYPE_UART=1 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -D__REDLIB__ -I"/home/mrtbuntu/Documents/MCUXpresso_11.2.1_4149/workspace/convolution/board" -I"/home/mrtbuntu/Documents/MCUXpresso_11.2.1_4149/workspace/convolution/source" -I"/home/mrtbuntu/Documents/MCUXpresso_11.2.1_4149/workspace/convolution" -I"/home/mrtbuntu/Documents/MCUXpresso_11.2.1_4149/workspace/convolution/drivers" -I"/home/mrtbuntu/Documents/MCUXpresso_11.2.1_4149/workspace/convolution/device" -I"/home/mrtbuntu/Documents/MCUXpresso_11.2.1_4149/workspace/convolution/CMSIS" -I"/home/mrtbuntu/Documents/MCUXpresso_11.2.1_4149/workspace/convolution/component/uart" -I"/home/mrtbuntu/Documents/MCUXpresso_11.2.1_4149/workspace/convolution/utilities" -I"/home/mrtbuntu/Documents/MCUXpresso_11.2.1_4149/workspace/convolution/component/serial_manager" -I"/home/mrtbuntu/Documents/MCUXpresso_11.2.1_4149/workspace/convolution/component/lists" -O0 -fno-common -g3 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmerge-constants -fmacro-prefix-map="../$(@D)/"=. -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


