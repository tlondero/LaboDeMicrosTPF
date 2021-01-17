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
	arm-none-eabi-gcc -std=gnu99 -D__REDLIB__ -DCPU_MK64FN1M0VLL12 -DCPU_MK64FN1M0VLL12_cm4 -DFRDM_K64F -DFREEDOM -DSERIAL_PORT_TYPE_UART=1 -DSDK_DEBUGCONSOLE=0 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -DFSL_RTOS_BM -DSDK_OS_BAREMETAL -I"C:\Users\Usuario\Documents\GitHub\LaboMicrosTPF\TestS\DAC_Test\dacdmatest_dac_continuous_pdb_edma\drivers" -I"C:\Users\Usuario\Documents\GitHub\LaboMicrosTPF\TestS\DAC_Test\dacdmatest_dac_continuous_pdb_edma\device" -I"C:\Users\Usuario\Documents\GitHub\LaboMicrosTPF\TestS\DAC_Test\dacdmatest_dac_continuous_pdb_edma\component\serial_manager" -I"C:\Users\Usuario\Documents\GitHub\LaboMicrosTPF\TestS\DAC_Test\dacdmatest_dac_continuous_pdb_edma\CMSIS" -I"C:\Users\Usuario\Documents\GitHub\LaboMicrosTPF\TestS\DAC_Test\dacdmatest_dac_continuous_pdb_edma\utilities" -I"C:\Users\Usuario\Documents\GitHub\LaboMicrosTPF\TestS\DAC_Test\dacdmatest_dac_continuous_pdb_edma\component\lists" -I"C:\Users\Usuario\Documents\GitHub\LaboMicrosTPF\TestS\DAC_Test\dacdmatest_dac_continuous_pdb_edma\component\uart" -I"C:\Users\Usuario\Documents\GitHub\LaboMicrosTPF\TestS\DAC_Test\dacdmatest_dac_continuous_pdb_edma\board" -I"C:\Users\Usuario\Documents\GitHub\LaboMicrosTPF\TestS\DAC_Test\dacdmatest_dac_continuous_pdb_edma\source" -I"C:\Users\Usuario\Documents\GitHub\LaboMicrosTPF\TestS\DAC_Test\dacdmatest_dac_continuous_pdb_edma" -I"C:\Users\Usuario\Documents\GitHub\LaboMicrosTPF\TestS\DAC_Test\dacdmatest_dac_continuous_pdb_edma\drivers" -I"C:\Users\Usuario\Documents\GitHub\LaboMicrosTPF\TestS\DAC_Test\dacdmatest_dac_continuous_pdb_edma\device" -I"C:\Users\Usuario\Documents\GitHub\LaboMicrosTPF\TestS\DAC_Test\dacdmatest_dac_continuous_pdb_edma\CMSIS" -I"C:\Users\Usuario\Documents\GitHub\LaboMicrosTPF\TestS\DAC_Test\dacdmatest_dac_continuous_pdb_edma\utilities" -I"C:\Users\Usuario\Documents\GitHub\LaboMicrosTPF\TestS\DAC_Test\dacdmatest_dac_continuous_pdb_edma\component\serial_manager" -I"C:\Users\Usuario\Documents\GitHub\LaboMicrosTPF\TestS\DAC_Test\dacdmatest_dac_continuous_pdb_edma\component\lists" -I"C:\Users\Usuario\Documents\GitHub\LaboMicrosTPF\TestS\DAC_Test\dacdmatest_dac_continuous_pdb_edma\component\uart" -O0 -fno-common -g3 -Wall -c  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin -fmerge-constants -fmacro-prefix-map="../$(@D)/"=. -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


