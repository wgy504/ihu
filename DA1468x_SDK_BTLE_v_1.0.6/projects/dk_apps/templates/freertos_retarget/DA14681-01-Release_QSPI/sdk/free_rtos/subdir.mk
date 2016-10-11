################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
D:/IHUSRC/ihu/DA1468x_SDK_BTLE_v_1.0.6/sdk/bsp/free_rtos/croutine.c \
D:/IHUSRC/ihu/DA1468x_SDK_BTLE_v_1.0.6/sdk/bsp/free_rtos/event_groups.c \
D:/IHUSRC/ihu/DA1468x_SDK_BTLE_v_1.0.6/sdk/bsp/free_rtos/list.c \
D:/IHUSRC/ihu/DA1468x_SDK_BTLE_v_1.0.6/sdk/bsp/free_rtos/queue.c \
D:/IHUSRC/ihu/DA1468x_SDK_BTLE_v_1.0.6/sdk/bsp/free_rtos/tasks.c \
D:/IHUSRC/ihu/DA1468x_SDK_BTLE_v_1.0.6/sdk/bsp/free_rtos/timers.c 

OBJS += \
./sdk/free_rtos/croutine.o \
./sdk/free_rtos/event_groups.o \
./sdk/free_rtos/list.o \
./sdk/free_rtos/queue.o \
./sdk/free_rtos/tasks.o \
./sdk/free_rtos/timers.o 

C_DEPS += \
./sdk/free_rtos/croutine.d \
./sdk/free_rtos/event_groups.d \
./sdk/free_rtos/list.d \
./sdk/free_rtos/queue.d \
./sdk/free_rtos/tasks.d \
./sdk/free_rtos/timers.d 


# Each subdirectory must supply rules for building sources it contributes
sdk/free_rtos/croutine.o: D:/IHUSRC/ihu/DA1468x_SDK_BTLE_v_1.0.6/sdk/bsp/free_rtos/croutine.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0 -mthumb -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -Ddg_configBLACK_ORCA_IC_REV=BLACK_ORCA_IC_REV_A -Ddg_configBLACK_ORCA_IC_STEP=BLACK_ORCA_IC_STEP_E -DRELEASE_BUILD -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\adapters\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\memory\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\config" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\osal" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\peripherals\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\free_rtos\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\system\sys_man\include" -I"D:\IHUSRC\ihu\SrcBxxh\l0comvm" -I"D:\IHUSRC\ihu\SrcBxxh\l1vmlayer" -I"D:\IHUSRC\ihu\SrcBxxh\l2frame" -I"D:\IHUSRC\ihu\SrcBxxh\l3appl" -include"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\projects\dk_apps\templates\freertos_retarget\config\custom_config_qspi.h" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

sdk/free_rtos/event_groups.o: D:/IHUSRC/ihu/DA1468x_SDK_BTLE_v_1.0.6/sdk/bsp/free_rtos/event_groups.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0 -mthumb -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -Ddg_configBLACK_ORCA_IC_REV=BLACK_ORCA_IC_REV_A -Ddg_configBLACK_ORCA_IC_STEP=BLACK_ORCA_IC_STEP_E -DRELEASE_BUILD -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\adapters\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\memory\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\config" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\osal" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\peripherals\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\free_rtos\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\system\sys_man\include" -I"D:\IHUSRC\ihu\SrcBxxh\l0comvm" -I"D:\IHUSRC\ihu\SrcBxxh\l1vmlayer" -I"D:\IHUSRC\ihu\SrcBxxh\l2frame" -I"D:\IHUSRC\ihu\SrcBxxh\l3appl" -include"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\projects\dk_apps\templates\freertos_retarget\config\custom_config_qspi.h" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

sdk/free_rtos/list.o: D:/IHUSRC/ihu/DA1468x_SDK_BTLE_v_1.0.6/sdk/bsp/free_rtos/list.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0 -mthumb -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -Ddg_configBLACK_ORCA_IC_REV=BLACK_ORCA_IC_REV_A -Ddg_configBLACK_ORCA_IC_STEP=BLACK_ORCA_IC_STEP_E -DRELEASE_BUILD -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\adapters\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\memory\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\config" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\osal" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\peripherals\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\free_rtos\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\system\sys_man\include" -I"D:\IHUSRC\ihu\SrcBxxh\l0comvm" -I"D:\IHUSRC\ihu\SrcBxxh\l1vmlayer" -I"D:\IHUSRC\ihu\SrcBxxh\l2frame" -I"D:\IHUSRC\ihu\SrcBxxh\l3appl" -include"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\projects\dk_apps\templates\freertos_retarget\config\custom_config_qspi.h" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

sdk/free_rtos/queue.o: D:/IHUSRC/ihu/DA1468x_SDK_BTLE_v_1.0.6/sdk/bsp/free_rtos/queue.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0 -mthumb -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -Ddg_configBLACK_ORCA_IC_REV=BLACK_ORCA_IC_REV_A -Ddg_configBLACK_ORCA_IC_STEP=BLACK_ORCA_IC_STEP_E -DRELEASE_BUILD -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\adapters\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\memory\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\config" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\osal" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\peripherals\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\free_rtos\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\system\sys_man\include" -I"D:\IHUSRC\ihu\SrcBxxh\l0comvm" -I"D:\IHUSRC\ihu\SrcBxxh\l1vmlayer" -I"D:\IHUSRC\ihu\SrcBxxh\l2frame" -I"D:\IHUSRC\ihu\SrcBxxh\l3appl" -include"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\projects\dk_apps\templates\freertos_retarget\config\custom_config_qspi.h" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

sdk/free_rtos/tasks.o: D:/IHUSRC/ihu/DA1468x_SDK_BTLE_v_1.0.6/sdk/bsp/free_rtos/tasks.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0 -mthumb -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -Ddg_configBLACK_ORCA_IC_REV=BLACK_ORCA_IC_REV_A -Ddg_configBLACK_ORCA_IC_STEP=BLACK_ORCA_IC_STEP_E -DRELEASE_BUILD -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\adapters\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\memory\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\config" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\osal" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\peripherals\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\free_rtos\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\system\sys_man\include" -I"D:\IHUSRC\ihu\SrcBxxh\l0comvm" -I"D:\IHUSRC\ihu\SrcBxxh\l1vmlayer" -I"D:\IHUSRC\ihu\SrcBxxh\l2frame" -I"D:\IHUSRC\ihu\SrcBxxh\l3appl" -include"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\projects\dk_apps\templates\freertos_retarget\config\custom_config_qspi.h" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

sdk/free_rtos/timers.o: D:/IHUSRC/ihu/DA1468x_SDK_BTLE_v_1.0.6/sdk/bsp/free_rtos/timers.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0 -mthumb -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -Ddg_configBLACK_ORCA_IC_REV=BLACK_ORCA_IC_REV_A -Ddg_configBLACK_ORCA_IC_STEP=BLACK_ORCA_IC_STEP_E -DRELEASE_BUILD -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\adapters\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\memory\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\config" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\osal" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\peripherals\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\free_rtos\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\system\sys_man\include" -I"D:\IHUSRC\ihu\SrcBxxh\l0comvm" -I"D:\IHUSRC\ihu\SrcBxxh\l1vmlayer" -I"D:\IHUSRC\ihu\SrcBxxh\l2frame" -I"D:\IHUSRC\ihu\SrcBxxh\l3appl" -include"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\projects\dk_apps\templates\freertos_retarget\config\custom_config_qspi.h" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


