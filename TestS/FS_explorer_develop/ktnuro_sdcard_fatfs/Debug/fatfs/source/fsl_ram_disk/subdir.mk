################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../fatfs/source/fsl_ram_disk/fsl_ram_disk.c 

OBJS += \
./fatfs/source/fsl_ram_disk/fsl_ram_disk.o 

C_DEPS += \
./fatfs/source/fsl_ram_disk/fsl_ram_disk.d 


# Each subdirectory must supply rules for building sources it contributes
fatfs/source/fsl_ram_disk/%.o: ../fatfs/source/fsl_ram_disk/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=gnu99 -D__REDLIB__ -DCPU_MK64FN1M0VLL12 -DCPU_MK64FN1M0VLL12_cm4 -DFRDM_K64F -DFREEDOM -DSD_ENABLED -DSERIAL_PORT_TYPE_UART=1 -DSDK_DEBUGCONSOLE=0 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -I"C:\Users\Guido\Documents\GitHub\LaboDeMicrosTPF\TestS\FS_explorer_develop\ktnuro_sdcard_fatfs\board" -I"C:\Users\Guido\Documents\GitHub\LaboDeMicrosTPF\TestS\FS_explorer_develop\ktnuro_sdcard_fatfs\source" -I"C:\Users\Guido\Documents\GitHub\LaboDeMicrosTPF\TestS\FS_explorer_develop\ktnuro_sdcard_fatfs" -I"C:\Users\Guido\Documents\GitHub\LaboDeMicrosTPF\TestS\FS_explorer_develop\ktnuro_sdcard_fatfs\drivers" -I"C:\Users\Guido\Documents\GitHub\LaboDeMicrosTPF\TestS\FS_explorer_develop\ktnuro_sdcard_fatfs\device" -I"C:\Users\Guido\Documents\GitHub\LaboDeMicrosTPF\TestS\FS_explorer_develop\ktnuro_sdcard_fatfs\CMSIS" -I"C:\Users\Guido\Documents\GitHub\LaboDeMicrosTPF\TestS\FS_explorer_develop\ktnuro_sdcard_fatfs\sdmmc\inc" -I"C:\Users\Guido\Documents\GitHub\LaboDeMicrosTPF\TestS\FS_explorer_develop\ktnuro_sdcard_fatfs\sdmmc\host" -I"C:\Users\Guido\Documents\GitHub\LaboDeMicrosTPF\TestS\FS_explorer_develop\ktnuro_sdcard_fatfs\sdmmc\osa" -I"C:\Users\Guido\Documents\GitHub\LaboDeMicrosTPF\TestS\FS_explorer_develop\ktnuro_sdcard_fatfs\component\osa" -I"C:\Users\Guido\Documents\GitHub\LaboDeMicrosTPF\TestS\FS_explorer_develop\ktnuro_sdcard_fatfs\component\lists" -I"C:\Users\Guido\Documents\GitHub\LaboDeMicrosTPF\TestS\FS_explorer_develop\ktnuro_sdcard_fatfs\fatfs\source" -I"C:\Users\Guido\Documents\GitHub\LaboDeMicrosTPF\TestS\FS_explorer_develop\ktnuro_sdcard_fatfs\fatfs\source\fsl_ram_disk" -I"C:\Users\Guido\Documents\GitHub\LaboDeMicrosTPF\TestS\FS_explorer_develop\ktnuro_sdcard_fatfs\fatfs\source\fsl_sd_disk" -I"C:\Users\Guido\Documents\GitHub\LaboDeMicrosTPF\TestS\FS_explorer_develop\ktnuro_sdcard_fatfs\utilities" -I"C:\Users\Guido\Documents\GitHub\LaboDeMicrosTPF\TestS\FS_explorer_develop\ktnuro_sdcard_fatfs\component\serial_manager" -I"C:\Users\Guido\Documents\GitHub\LaboDeMicrosTPF\TestS\FS_explorer_develop\ktnuro_sdcard_fatfs\component\uart" -O0 -fno-common -g3 -Wall -c  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin -fmerge-constants -fmacro-prefix-map="../$(@D)/"=. -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


