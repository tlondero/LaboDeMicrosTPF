################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../drivers/fsl_adc16.c \
../drivers/fsl_clock.c \
../drivers/fsl_common.c \
../drivers/fsl_gpio.c \
../drivers/fsl_i2c.c \
../drivers/fsl_smc.c \
../drivers/fsl_uart.c 

OBJS += \
./drivers/fsl_adc16.o \
./drivers/fsl_clock.o \
./drivers/fsl_common.o \
./drivers/fsl_gpio.o \
./drivers/fsl_i2c.o \
./drivers/fsl_smc.o \
./drivers/fsl_uart.o 

C_DEPS += \
./drivers/fsl_adc16.d \
./drivers/fsl_clock.d \
./drivers/fsl_common.d \
./drivers/fsl_gpio.d \
./drivers/fsl_i2c.d \
./drivers/fsl_smc.d \
./drivers/fsl_uart.d 


# Each subdirectory must supply rules for building sources it contributes
drivers/%.o: ../drivers/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DCPU_MK64FN1M0VLL12 -DCPU_MK64FN1M0VLL12_cm4 -DFSL_RTOS_BM -DSDK_OS_BAREMETAL -DSDK_DEBUGCONSOLE=0 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -DSERIAL_PORT_TYPE_UART=1 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -D__REDLIB__ -I"/home/mrtbuntu/Documents/MCUXpresso_11.2.1_4149/workspace/convolution/board" -I"/home/mrtbuntu/Documents/MCUXpresso_11.2.1_4149/workspace/convolution/source" -I"/home/mrtbuntu/Documents/MCUXpresso_11.2.1_4149/workspace/convolution" -I"/home/mrtbuntu/Documents/MCUXpresso_11.2.1_4149/workspace/convolution/drivers" -I"/home/mrtbuntu/Documents/MCUXpresso_11.2.1_4149/workspace/convolution/device" -I"/home/mrtbuntu/Documents/MCUXpresso_11.2.1_4149/workspace/convolution/CMSIS" -I"/home/mrtbuntu/Documents/MCUXpresso_11.2.1_4149/workspace/convolution/component/uart" -I"/home/mrtbuntu/Documents/MCUXpresso_11.2.1_4149/workspace/convolution/utilities" -I"/home/mrtbuntu/Documents/MCUXpresso_11.2.1_4149/workspace/convolution/component/serial_manager" -I"/home/mrtbuntu/Documents/MCUXpresso_11.2.1_4149/workspace/convolution/component/lists" -O0 -fno-common -g3 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmerge-constants -fmacro-prefix-map="../$(@D)/"=. -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


