################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
D:/IHUPRJ/ihu/DA1468x_SDK_BTLE_v_1.0.6/sdk/bsp/adapters/src/ad_battery.c \
D:/IHUPRJ/ihu/DA1468x_SDK_BTLE_v_1.0.6/sdk/bsp/adapters/src/ad_flash.c \
D:/IHUPRJ/ihu/DA1468x_SDK_BTLE_v_1.0.6/sdk/bsp/adapters/src/ad_gpadc.c \
D:/IHUPRJ/ihu/DA1468x_SDK_BTLE_v_1.0.6/sdk/bsp/adapters/src/ad_i2c.c \
D:/IHUPRJ/ihu/DA1468x_SDK_BTLE_v_1.0.6/sdk/bsp/adapters/src/ad_keyboard_scanner.c \
D:/IHUPRJ/ihu/DA1468x_SDK_BTLE_v_1.0.6/sdk/bsp/adapters/src/ad_nvms.c \
D:/IHUPRJ/ihu/DA1468x_SDK_BTLE_v_1.0.6/sdk/bsp/adapters/src/ad_nvms_direct.c \
D:/IHUPRJ/ihu/DA1468x_SDK_BTLE_v_1.0.6/sdk/bsp/adapters/src/ad_nvms_ves.c \
D:/IHUPRJ/ihu/DA1468x_SDK_BTLE_v_1.0.6/sdk/bsp/adapters/src/ad_nvparam.c \
D:/IHUPRJ/ihu/DA1468x_SDK_BTLE_v_1.0.6/sdk/bsp/adapters/src/ad_rf.c \
D:/IHUPRJ/ihu/DA1468x_SDK_BTLE_v_1.0.6/sdk/bsp/adapters/src/ad_spi.c \
D:/IHUPRJ/ihu/DA1468x_SDK_BTLE_v_1.0.6/sdk/bsp/adapters/src/ad_temp_sens.c \
D:/IHUPRJ/ihu/DA1468x_SDK_BTLE_v_1.0.6/sdk/bsp/adapters/src/ad_uart.c \
D:/IHUPRJ/ihu/DA1468x_SDK_BTLE_v_1.0.6/sdk/bsp/adapters/src/ad_uart_ble.c 

OBJS += \
./sdk/adapters/src/ad_battery.o \
./sdk/adapters/src/ad_flash.o \
./sdk/adapters/src/ad_gpadc.o \
./sdk/adapters/src/ad_i2c.o \
./sdk/adapters/src/ad_keyboard_scanner.o \
./sdk/adapters/src/ad_nvms.o \
./sdk/adapters/src/ad_nvms_direct.o \
./sdk/adapters/src/ad_nvms_ves.o \
./sdk/adapters/src/ad_nvparam.o \
./sdk/adapters/src/ad_rf.o \
./sdk/adapters/src/ad_spi.o \
./sdk/adapters/src/ad_temp_sens.o \
./sdk/adapters/src/ad_uart.o \
./sdk/adapters/src/ad_uart_ble.o 

C_DEPS += \
./sdk/adapters/src/ad_battery.d \
./sdk/adapters/src/ad_flash.d \
./sdk/adapters/src/ad_gpadc.d \
./sdk/adapters/src/ad_i2c.d \
./sdk/adapters/src/ad_keyboard_scanner.d \
./sdk/adapters/src/ad_nvms.d \
./sdk/adapters/src/ad_nvms_direct.d \
./sdk/adapters/src/ad_nvms_ves.d \
./sdk/adapters/src/ad_nvparam.d \
./sdk/adapters/src/ad_rf.d \
./sdk/adapters/src/ad_spi.d \
./sdk/adapters/src/ad_temp_sens.d \
./sdk/adapters/src/ad_uart.d \
./sdk/adapters/src/ad_uart_ble.d 


# Each subdirectory must supply rules for building sources it contributes
sdk/adapters/src/ad_battery.o: D:/IHUPRJ/ihu/DA1468x_SDK_BTLE_v_1.0.6/sdk/bsp/adapters/src/ad_battery.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0 -mthumb -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -Ddg_configBLACK_ORCA_IC_REV=BLACK_ORCA_IC_REV_A -Ddg_configBLACK_ORCA_IC_STEP=BLACK_ORCA_IC_STEP_E -DRELEASE_BUILD -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\adapters\include" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\memory\include" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\config" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\include" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\osal" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\peripherals\include" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\free_rtos\include" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\system\sys_man\include" -include"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\projects\dk_apps\templates\freertos_retarget\config\custom_config_qspi.h" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

sdk/adapters/src/ad_flash.o: D:/IHUPRJ/ihu/DA1468x_SDK_BTLE_v_1.0.6/sdk/bsp/adapters/src/ad_flash.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0 -mthumb -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -Ddg_configBLACK_ORCA_IC_REV=BLACK_ORCA_IC_REV_A -Ddg_configBLACK_ORCA_IC_STEP=BLACK_ORCA_IC_STEP_E -DRELEASE_BUILD -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\adapters\include" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\memory\include" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\config" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\include" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\osal" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\peripherals\include" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\free_rtos\include" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\system\sys_man\include" -include"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\projects\dk_apps\templates\freertos_retarget\config\custom_config_qspi.h" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

sdk/adapters/src/ad_gpadc.o: D:/IHUPRJ/ihu/DA1468x_SDK_BTLE_v_1.0.6/sdk/bsp/adapters/src/ad_gpadc.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0 -mthumb -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -Ddg_configBLACK_ORCA_IC_REV=BLACK_ORCA_IC_REV_A -Ddg_configBLACK_ORCA_IC_STEP=BLACK_ORCA_IC_STEP_E -DRELEASE_BUILD -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\adapters\include" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\memory\include" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\config" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\include" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\osal" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\peripherals\include" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\free_rtos\include" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\system\sys_man\include" -include"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\projects\dk_apps\templates\freertos_retarget\config\custom_config_qspi.h" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

sdk/adapters/src/ad_i2c.o: D:/IHUPRJ/ihu/DA1468x_SDK_BTLE_v_1.0.6/sdk/bsp/adapters/src/ad_i2c.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0 -mthumb -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -Ddg_configBLACK_ORCA_IC_REV=BLACK_ORCA_IC_REV_A -Ddg_configBLACK_ORCA_IC_STEP=BLACK_ORCA_IC_STEP_E -DRELEASE_BUILD -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\adapters\include" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\memory\include" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\config" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\include" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\osal" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\peripherals\include" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\free_rtos\include" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\system\sys_man\include" -include"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\projects\dk_apps\templates\freertos_retarget\config\custom_config_qspi.h" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

sdk/adapters/src/ad_keyboard_scanner.o: D:/IHUPRJ/ihu/DA1468x_SDK_BTLE_v_1.0.6/sdk/bsp/adapters/src/ad_keyboard_scanner.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0 -mthumb -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -Ddg_configBLACK_ORCA_IC_REV=BLACK_ORCA_IC_REV_A -Ddg_configBLACK_ORCA_IC_STEP=BLACK_ORCA_IC_STEP_E -DRELEASE_BUILD -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\adapters\include" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\memory\include" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\config" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\include" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\osal" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\peripherals\include" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\free_rtos\include" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\system\sys_man\include" -include"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\projects\dk_apps\templates\freertos_retarget\config\custom_config_qspi.h" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

sdk/adapters/src/ad_nvms.o: D:/IHUPRJ/ihu/DA1468x_SDK_BTLE_v_1.0.6/sdk/bsp/adapters/src/ad_nvms.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0 -mthumb -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -Ddg_configBLACK_ORCA_IC_REV=BLACK_ORCA_IC_REV_A -Ddg_configBLACK_ORCA_IC_STEP=BLACK_ORCA_IC_STEP_E -DRELEASE_BUILD -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\adapters\include" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\memory\include" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\config" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\include" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\osal" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\peripherals\include" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\free_rtos\include" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\system\sys_man\include" -include"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\projects\dk_apps\templates\freertos_retarget\config\custom_config_qspi.h" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

sdk/adapters/src/ad_nvms_direct.o: D:/IHUPRJ/ihu/DA1468x_SDK_BTLE_v_1.0.6/sdk/bsp/adapters/src/ad_nvms_direct.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0 -mthumb -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -Ddg_configBLACK_ORCA_IC_REV=BLACK_ORCA_IC_REV_A -Ddg_configBLACK_ORCA_IC_STEP=BLACK_ORCA_IC_STEP_E -DRELEASE_BUILD -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\adapters\include" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\memory\include" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\config" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\include" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\osal" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\peripherals\include" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\free_rtos\include" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\system\sys_man\include" -include"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\projects\dk_apps\templates\freertos_retarget\config\custom_config_qspi.h" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

sdk/adapters/src/ad_nvms_ves.o: D:/IHUPRJ/ihu/DA1468x_SDK_BTLE_v_1.0.6/sdk/bsp/adapters/src/ad_nvms_ves.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0 -mthumb -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -Ddg_configBLACK_ORCA_IC_REV=BLACK_ORCA_IC_REV_A -Ddg_configBLACK_ORCA_IC_STEP=BLACK_ORCA_IC_STEP_E -DRELEASE_BUILD -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\adapters\include" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\memory\include" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\config" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\include" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\osal" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\peripherals\include" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\free_rtos\include" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\system\sys_man\include" -include"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\projects\dk_apps\templates\freertos_retarget\config\custom_config_qspi.h" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

sdk/adapters/src/ad_nvparam.o: D:/IHUPRJ/ihu/DA1468x_SDK_BTLE_v_1.0.6/sdk/bsp/adapters/src/ad_nvparam.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0 -mthumb -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -Ddg_configBLACK_ORCA_IC_REV=BLACK_ORCA_IC_REV_A -Ddg_configBLACK_ORCA_IC_STEP=BLACK_ORCA_IC_STEP_E -DRELEASE_BUILD -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\adapters\include" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\memory\include" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\config" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\include" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\osal" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\peripherals\include" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\free_rtos\include" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\system\sys_man\include" -include"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\projects\dk_apps\templates\freertos_retarget\config\custom_config_qspi.h" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

sdk/adapters/src/ad_rf.o: D:/IHUPRJ/ihu/DA1468x_SDK_BTLE_v_1.0.6/sdk/bsp/adapters/src/ad_rf.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0 -mthumb -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -Ddg_configBLACK_ORCA_IC_REV=BLACK_ORCA_IC_REV_A -Ddg_configBLACK_ORCA_IC_STEP=BLACK_ORCA_IC_STEP_E -DRELEASE_BUILD -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\adapters\include" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\memory\include" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\config" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\include" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\osal" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\peripherals\include" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\free_rtos\include" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\system\sys_man\include" -include"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\projects\dk_apps\templates\freertos_retarget\config\custom_config_qspi.h" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

sdk/adapters/src/ad_spi.o: D:/IHUPRJ/ihu/DA1468x_SDK_BTLE_v_1.0.6/sdk/bsp/adapters/src/ad_spi.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0 -mthumb -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -Ddg_configBLACK_ORCA_IC_REV=BLACK_ORCA_IC_REV_A -Ddg_configBLACK_ORCA_IC_STEP=BLACK_ORCA_IC_STEP_E -DRELEASE_BUILD -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\adapters\include" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\memory\include" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\config" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\include" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\osal" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\peripherals\include" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\free_rtos\include" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\system\sys_man\include" -include"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\projects\dk_apps\templates\freertos_retarget\config\custom_config_qspi.h" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

sdk/adapters/src/ad_temp_sens.o: D:/IHUPRJ/ihu/DA1468x_SDK_BTLE_v_1.0.6/sdk/bsp/adapters/src/ad_temp_sens.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0 -mthumb -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -Ddg_configBLACK_ORCA_IC_REV=BLACK_ORCA_IC_REV_A -Ddg_configBLACK_ORCA_IC_STEP=BLACK_ORCA_IC_STEP_E -DRELEASE_BUILD -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\adapters\include" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\memory\include" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\config" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\include" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\osal" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\peripherals\include" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\free_rtos\include" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\system\sys_man\include" -include"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\projects\dk_apps\templates\freertos_retarget\config\custom_config_qspi.h" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

sdk/adapters/src/ad_uart.o: D:/IHUPRJ/ihu/DA1468x_SDK_BTLE_v_1.0.6/sdk/bsp/adapters/src/ad_uart.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0 -mthumb -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -Ddg_configBLACK_ORCA_IC_REV=BLACK_ORCA_IC_REV_A -Ddg_configBLACK_ORCA_IC_STEP=BLACK_ORCA_IC_STEP_E -DRELEASE_BUILD -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\adapters\include" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\memory\include" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\config" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\include" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\osal" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\peripherals\include" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\free_rtos\include" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\system\sys_man\include" -include"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\projects\dk_apps\templates\freertos_retarget\config\custom_config_qspi.h" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

sdk/adapters/src/ad_uart_ble.o: D:/IHUPRJ/ihu/DA1468x_SDK_BTLE_v_1.0.6/sdk/bsp/adapters/src/ad_uart_ble.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0 -mthumb -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -Ddg_configBLACK_ORCA_IC_REV=BLACK_ORCA_IC_REV_A -Ddg_configBLACK_ORCA_IC_STEP=BLACK_ORCA_IC_STEP_E -DRELEASE_BUILD -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\adapters\include" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\memory\include" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\config" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\include" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\osal" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\peripherals\include" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\free_rtos\include" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\system\sys_man\include" -include"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\projects\dk_apps\templates\freertos_retarget\config\custom_config_qspi.h" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


