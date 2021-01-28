################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../board/board.c \
../board/clock_config.c \
../board/pin_mux.c 

OBJS += \
./board/board.o \
./board/clock_config.o \
./board/pin_mux.o 

C_DEPS += \
./board/board.d \
./board/clock_config.d \
./board/pin_mux.d 


# Each subdirectory must supply rules for building sources it contributes
board/%.o: ../board/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=gnu99 -D__REDLIB__ -DCPU_MK64FN1M0VLL12 -DCPU_MK64FN1M0VLL12_cm4 -DFRDM_K64F -DFREEDOM -DSERIAL_PORT_TYPE_UART=1 -DSDK_DEBUGCONSOLE=0 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -DFSL_RTOS_BM -DSDK_OS_BAREMETAL -I"C:\Users\Guido\Documents\GitHub\LaboDeMicrosTPF\TestS\DAC_Test\frdmk64f_pit\drivers" -I"C:\Users\Guido\Documents\GitHub\LaboDeMicrosTPF\TestS\DAC_Test\frdmk64f_pit\CMSIS" -I"C:\Users\Guido\Documents\GitHub\LaboDeMicrosTPF\TestS\DAC_Test\frdmk64f_pit\component\uart" -I"C:\Users\Guido\Documents\GitHub\LaboDeMicrosTPF\TestS\DAC_Test\frdmk64f_pit\component\serial_manager" -I"C:\Users\Guido\Documents\GitHub\LaboDeMicrosTPF\TestS\DAC_Test\frdmk64f_pit\utilities" -I"C:\Users\Guido\Documents\GitHub\LaboDeMicrosTPF\TestS\DAC_Test\frdmk64f_pit\component\lists" -I"C:\Users\Guido\Documents\GitHub\LaboDeMicrosTPF\TestS\DAC_Test\frdmk64f_pit\device" -I"C:\Users\Guido\Documents\GitHub\LaboDeMicrosTPF\TestS\DAC_Test\frdmk64f_pit\drivers" -I"C:\Users\Guido\Documents\GitHub\LaboDeMicrosTPF\TestS\DAC_Test\frdmk64f_pit\CMSIS" -I"C:\Users\Guido\Documents\GitHub\LaboDeMicrosTPF\TestS\DAC_Test\frdmk64f_pit\component\uart" -I"C:\Users\Guido\Documents\GitHub\LaboDeMicrosTPF\TestS\DAC_Test\frdmk64f_pit\component\serial_manager" -I"C:\Users\Guido\Documents\GitHub\LaboDeMicrosTPF\TestS\DAC_Test\frdmk64f_pit\utilities" -I"C:\Users\Guido\Documents\GitHub\LaboDeMicrosTPF\TestS\DAC_Test\frdmk64f_pit\component\lists" -I"C:\Users\Guido\Documents\GitHub\LaboDeMicrosTPF\TestS\DAC_Test\frdmk64f_pit\device" -I"C:\Users\Guido\Documents\GitHub\LaboDeMicrosTPF\TestS\DAC_Test\frdmk64f_pit\board" -I"C:\Users\Guido\Documents\GitHub\LaboDeMicrosTPF\TestS\DAC_Test\frdmk64f_pit\source" -I"C:\Users\Guido\Documents\GitHub\LaboDeMicrosTPF\TestS\DAC_Test\frdmk64f_pit" -O0 -fno-common -g3 -Wall -c  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin -fmerge-constants -fmacro-prefix-map="../$(@D)/"=. -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


