################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../drivers/fsl_clock.c \
../drivers/fsl_common.c \
../drivers/fsl_dac.c \
../drivers/fsl_dmamux.c \
../drivers/fsl_edma.c \
../drivers/fsl_ftfx_cache.c \
../drivers/fsl_ftfx_controller.c \
../drivers/fsl_ftfx_flash.c \
../drivers/fsl_ftfx_flexnvm.c \
../drivers/fsl_gpio.c \
../drivers/fsl_pdb.c \
../drivers/fsl_pit.c \
../drivers/fsl_smc.c \
../drivers/fsl_uart.c 

OBJS += \
./drivers/fsl_clock.o \
./drivers/fsl_common.o \
./drivers/fsl_dac.o \
./drivers/fsl_dmamux.o \
./drivers/fsl_edma.o \
./drivers/fsl_ftfx_cache.o \
./drivers/fsl_ftfx_controller.o \
./drivers/fsl_ftfx_flash.o \
./drivers/fsl_ftfx_flexnvm.o \
./drivers/fsl_gpio.o \
./drivers/fsl_pdb.o \
./drivers/fsl_pit.o \
./drivers/fsl_smc.o \
./drivers/fsl_uart.o 

C_DEPS += \
./drivers/fsl_clock.d \
./drivers/fsl_common.d \
./drivers/fsl_dac.d \
./drivers/fsl_dmamux.d \
./drivers/fsl_edma.d \
./drivers/fsl_ftfx_cache.d \
./drivers/fsl_ftfx_controller.d \
./drivers/fsl_ftfx_flash.d \
./drivers/fsl_ftfx_flexnvm.d \
./drivers/fsl_gpio.d \
./drivers/fsl_pdb.d \
./drivers/fsl_pit.d \
./drivers/fsl_smc.d \
./drivers/fsl_uart.d 


# Each subdirectory must supply rules for building sources it contributes
drivers/%.o: ../drivers/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=gnu99 -D__REDLIB__ -DCPU_MK64FN1M0VLL12 -DCPU_MK64FN1M0VLL12_cm4 -DFRDM_K64F -DFREEDOM -DSERIAL_PORT_TYPE_UART=1 -DSDK_DEBUGCONSOLE=0 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -DFSL_RTOS_BM -DSDK_OS_BAREMETAL -I"C:\Users\Acer\Documents\MCUXpressoIDE_11.2.1_4149\workspace\dacdmatest_dac_continuous_pdb_edma\drivers" -I"C:\Users\Acer\Documents\MCUXpressoIDE_11.2.1_4149\workspace\dacdmatest_dac_continuous_pdb_edma\device" -I"C:\Users\Acer\Documents\MCUXpressoIDE_11.2.1_4149\workspace\dacdmatest_dac_continuous_pdb_edma\component\serial_manager" -I"C:\Users\Acer\Documents\MCUXpressoIDE_11.2.1_4149\workspace\dacdmatest_dac_continuous_pdb_edma\CMSIS" -I"C:\Users\Acer\Documents\MCUXpressoIDE_11.2.1_4149\workspace\dacdmatest_dac_continuous_pdb_edma\utilities" -I"C:\Users\Acer\Documents\MCUXpressoIDE_11.2.1_4149\workspace\dacdmatest_dac_continuous_pdb_edma\component\lists" -I"C:\Users\Acer\Documents\MCUXpressoIDE_11.2.1_4149\workspace\dacdmatest_dac_continuous_pdb_edma\component\uart" -I"C:\Users\Acer\Documents\MCUXpressoIDE_11.2.1_4149\workspace\dacdmatest_dac_continuous_pdb_edma\board" -I"C:\Users\Acer\Documents\MCUXpressoIDE_11.2.1_4149\workspace\dacdmatest_dac_continuous_pdb_edma\source" -I"C:\Users\Acer\Documents\MCUXpressoIDE_11.2.1_4149\workspace\dacdmatest_dac_continuous_pdb_edma" -I"C:\Users\Acer\Documents\MCUXpressoIDE_11.2.1_4149\workspace\dacdmatest_dac_continuous_pdb_edma\drivers" -I"C:\Users\Acer\Documents\MCUXpressoIDE_11.2.1_4149\workspace\dacdmatest_dac_continuous_pdb_edma\device" -I"C:\Users\Acer\Documents\MCUXpressoIDE_11.2.1_4149\workspace\dacdmatest_dac_continuous_pdb_edma\CMSIS" -I"C:\Users\Acer\Documents\MCUXpressoIDE_11.2.1_4149\workspace\dacdmatest_dac_continuous_pdb_edma\utilities" -I"C:\Users\Acer\Documents\MCUXpressoIDE_11.2.1_4149\workspace\dacdmatest_dac_continuous_pdb_edma\component\serial_manager" -I"C:\Users\Acer\Documents\MCUXpressoIDE_11.2.1_4149\workspace\dacdmatest_dac_continuous_pdb_edma\component\lists" -I"C:\Users\Acer\Documents\MCUXpressoIDE_11.2.1_4149\workspace\dacdmatest_dac_continuous_pdb_edma\component\uart" -O0 -fno-common -g3 -Wall -c  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin -fmerge-constants -fmacro-prefix-map="../$(@D)/"=. -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


