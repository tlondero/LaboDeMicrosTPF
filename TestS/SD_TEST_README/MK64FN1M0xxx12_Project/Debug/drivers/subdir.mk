################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../drivers/fsl_adc16.c \
../drivers/fsl_clock.c \
../drivers/fsl_common.c \
../drivers/fsl_dspi.c \
../drivers/fsl_gpio.c \
../drivers/fsl_i2c.c \
../drivers/fsl_sdhc.c \
../drivers/fsl_smc.c \
../drivers/fsl_sysmpu.c \
../drivers/fsl_uart.c 

OBJS += \
./drivers/fsl_adc16.o \
./drivers/fsl_clock.o \
./drivers/fsl_common.o \
./drivers/fsl_dspi.o \
./drivers/fsl_gpio.o \
./drivers/fsl_i2c.o \
./drivers/fsl_sdhc.o \
./drivers/fsl_smc.o \
./drivers/fsl_sysmpu.o \
./drivers/fsl_uart.o 

C_DEPS += \
./drivers/fsl_adc16.d \
./drivers/fsl_clock.d \
./drivers/fsl_common.d \
./drivers/fsl_dspi.d \
./drivers/fsl_gpio.d \
./drivers/fsl_i2c.d \
./drivers/fsl_sdhc.d \
./drivers/fsl_smc.d \
./drivers/fsl_sysmpu.d \
./drivers/fsl_uart.d 


# Each subdirectory must supply rules for building sources it contributes
drivers/%.o: ../drivers/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DCPU_MK64FN1M0VLL12 -DCPU_MK64FN1M0VLL12_cm4 -DFSL_RTOS_BM -DSDK_OS_BAREMETAL -DSDK_DEBUGCONSOLE=0 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -DSERIAL_PORT_TYPE_UART=1 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -I"C:\Users\Guido\Desktop\PORONGA MICROS\sd_sdk_ktn_parte2\MK64FN1M0xxx12_Project\usb\host\class" -I"C:\Users\Guido\Desktop\PORONGA MICROS\sd_sdk_ktn_parte2\MK64FN1M0xxx12_Project\sdmmc\inc" -I"C:\Users\Guido\Desktop\PORONGA MICROS\sd_sdk_ktn_parte2\MK64FN1M0xxx12_Project\usb\host" -I"C:\Users\Guido\Desktop\PORONGA MICROS\sd_sdk_ktn_parte2\MK64FN1M0xxx12_Project\CMSIS" -I"C:\Users\Guido\Desktop\PORONGA MICROS\sd_sdk_ktn_parte2\MK64FN1M0xxx12_Project\drivers" -I"C:\Users\Guido\Desktop\PORONGA MICROS\sd_sdk_ktn_parte2\MK64FN1M0xxx12_Project\fatfs\source\fsl_ram_disk" -I"C:\Users\Guido\Desktop\PORONGA MICROS\sd_sdk_ktn_parte2\MK64FN1M0xxx12_Project\component\osa" -I"C:\Users\Guido\Desktop\PORONGA MICROS\sd_sdk_ktn_parte2\MK64FN1M0xxx12_Project\sdmmc\osa" -I"C:\Users\Guido\Desktop\PORONGA MICROS\sd_sdk_ktn_parte2\MK64FN1M0xxx12_Project\fatfs\source\fsl_sdspi_disk" -I"C:\Users\Guido\Desktop\PORONGA MICROS\sd_sdk_ktn_parte2\MK64FN1M0xxx12_Project\sdmmc\host" -I"C:\Users\Guido\Desktop\PORONGA MICROS\sd_sdk_ktn_parte2\MK64FN1M0xxx12_Project\component\uart" -I"C:\Users\Guido\Desktop\PORONGA MICROS\sd_sdk_ktn_parte2\MK64FN1M0xxx12_Project\component\serial_manager" -I"C:\Users\Guido\Desktop\PORONGA MICROS\sd_sdk_ktn_parte2\MK64FN1M0xxx12_Project\fatfs\source\fsl_usb_disk" -I"C:\Users\Guido\Desktop\PORONGA MICROS\sd_sdk_ktn_parte2\MK64FN1M0xxx12_Project\source\generated" -I"C:\Users\Guido\Desktop\PORONGA MICROS\sd_sdk_ktn_parte2\MK64FN1M0xxx12_Project\fatfs\source\fsl_mmc_disk" -I"C:\Users\Guido\Desktop\PORONGA MICROS\sd_sdk_ktn_parte2\MK64FN1M0xxx12_Project\usb\include" -I"C:\Users\Guido\Desktop\PORONGA MICROS\sd_sdk_ktn_parte2\MK64FN1M0xxx12_Project\device" -I"C:\Users\Guido\Desktop\PORONGA MICROS\sd_sdk_ktn_parte2\MK64FN1M0xxx12_Project\component\lists" -I"C:\Users\Guido\Desktop\PORONGA MICROS\sd_sdk_ktn_parte2\MK64FN1M0xxx12_Project\fatfs\source\fsl_sd_disk" -I"C:\Users\Guido\Desktop\PORONGA MICROS\sd_sdk_ktn_parte2\MK64FN1M0xxx12_Project\utilities" -I"C:\Users\Guido\Desktop\PORONGA MICROS\sd_sdk_ktn_parte2\MK64FN1M0xxx12_Project\fatfs\source" -I"C:\Users\Guido\Desktop\PORONGA MICROS\sd_sdk_ktn_parte2\MK64FN1M0xxx12_Project\source" -I"C:\Users\Guido\Desktop\PORONGA MICROS\sd_sdk_ktn_parte2\MK64FN1M0xxx12_Project\board" -I"C:\Users\Guido\Desktop\PORONGA MICROS\sd_sdk_ktn_parte2\MK64FN1M0xxx12_Project\source" -I"C:\Users\Guido\Desktop\PORONGA MICROS\sd_sdk_ktn_parte2\MK64FN1M0xxx12_Project" -I"C:\Users\Guido\Desktop\PORONGA MICROS\sd_sdk_ktn_parte2\MK64FN1M0xxx12_Project\drivers" -I"C:\Users\Guido\Desktop\PORONGA MICROS\sd_sdk_ktn_parte2\MK64FN1M0xxx12_Project\device" -I"C:\Users\Guido\Desktop\PORONGA MICROS\sd_sdk_ktn_parte2\MK64FN1M0xxx12_Project\CMSIS" -I"C:\Users\Guido\Desktop\PORONGA MICROS\sd_sdk_ktn_parte2\MK64FN1M0xxx12_Project\component\uart" -I"C:\Users\Guido\Desktop\PORONGA MICROS\sd_sdk_ktn_parte2\MK64FN1M0xxx12_Project\utilities" -I"C:\Users\Guido\Desktop\PORONGA MICROS\sd_sdk_ktn_parte2\MK64FN1M0xxx12_Project\component\serial_manager" -I"C:\Users\Guido\Desktop\PORONGA MICROS\sd_sdk_ktn_parte2\MK64FN1M0xxx12_Project\component\lists" -I"C:\Users\Guido\Desktop\PORONGA MICROS\sd_sdk_ktn_parte2\MK64FN1M0xxx12_Project\fatfs\source" -I"C:\Users\Guido\Desktop\PORONGA MICROS\sd_sdk_ktn_parte2\MK64FN1M0xxx12_Project\fatfs\source\fsl_ram_disk" -I"C:\Users\Guido\Desktop\PORONGA MICROS\sd_sdk_ktn_parte2\MK64FN1M0xxx12_Project\fatfs\source\fsl_mmc_disk" -I"C:\Users\Guido\Desktop\PORONGA MICROS\sd_sdk_ktn_parte2\MK64FN1M0xxx12_Project\sdmmc\inc" -I"C:\Users\Guido\Desktop\PORONGA MICROS\sd_sdk_ktn_parte2\MK64FN1M0xxx12_Project\sdmmc\host" -I"C:\Users\Guido\Desktop\PORONGA MICROS\sd_sdk_ktn_parte2\MK64FN1M0xxx12_Project\fatfs\source\fsl_sd_disk" -I"C:\Users\Guido\Desktop\PORONGA MICROS\sd_sdk_ktn_parte2\MK64FN1M0xxx12_Project\fatfs\source\fsl_sdspi_disk" -I"C:\Users\Guido\Desktop\PORONGA MICROS\sd_sdk_ktn_parte2\MK64FN1M0xxx12_Project\fatfs\source\fsl_usb_disk" -I"C:\Users\Guido\Desktop\PORONGA MICROS\sd_sdk_ktn_parte2\MK64FN1M0xxx12_Project\usb\host\class" -I"C:\Users\Guido\Desktop\PORONGA MICROS\sd_sdk_ktn_parte2\MK64FN1M0xxx12_Project\usb\host" -I"C:\Users\Guido\Desktop\PORONGA MICROS\sd_sdk_ktn_parte2\MK64FN1M0xxx12_Project\component\osa" -I"C:\Users\Guido\Desktop\PORONGA MICROS\sd_sdk_ktn_parte2\MK64FN1M0xxx12_Project\source\generated" -I"C:\Users\Guido\Desktop\PORONGA MICROS\sd_sdk_ktn_parte2\MK64FN1M0xxx12_Project\usb\include" -I"C:\Users\Guido\Desktop\PORONGA MICROS\sd_sdk_ktn_parte2\MK64FN1M0xxx12_Project\sdmmc\osa" -O0 -fno-common -g3 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmerge-constants -fmacro-prefix-map="../$(@D)/"=. -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


