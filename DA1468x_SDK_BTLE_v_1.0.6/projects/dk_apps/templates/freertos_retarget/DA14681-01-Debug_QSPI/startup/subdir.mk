################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
D:/IHUSRC/ihu/DA1468x_SDK_BTLE_v_1.0.6/sdk/bsp/startup/config.c \
D:/IHUSRC/ihu/DA1468x_SDK_BTLE_v_1.0.6/sdk/bsp/startup/system_ARMCM0.c 

S_UPPER_SRCS += \
D:/IHUSRC/ihu/DA1468x_SDK_BTLE_v_1.0.6/sdk/bsp/startup/startup_ARMCM0.S \
D:/IHUSRC/ihu/DA1468x_SDK_BTLE_v_1.0.6/sdk/bsp/startup/vector_table.S 

OBJS += \
./startup/config.o \
./startup/startup_ARMCM0.o \
./startup/system_ARMCM0.o \
./startup/vector_table.o 

S_UPPER_DEPS += \
./startup/startup_ARMCM0.d \
./startup/vector_table.d 

C_DEPS += \
./startup/config.d \
./startup/system_ARMCM0.d 


# Each subdirectory must supply rules for building sources it contributes
startup/config.o: D:/IHUSRC/ihu/DA1468x_SDK_BTLE_v_1.0.6/sdk/bsp/startup/config.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -Ddg_configBLACK_ORCA_IC_REV=BLACK_ORCA_IC_REV_A -Ddg_configBLACK_ORCA_IC_STEP=BLACK_ORCA_IC_STEP_E -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\adapters\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\memory\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\config" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\osal" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\peripherals\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\free_rtos\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\system\sys_man\include" -I"//SrcBxxh/l1vmlayer" -I"D:\IHUSRC\ihu\SrcBxxh\l0comvm" -I"D:\IHUSRC\ihu\SrcBxxh\l1vmlayer" -I"D:\IHUSRC\ihu\SrcBxxh\l2frame" -I"D:\IHUSRC\ihu\SrcBxxh\l3appl" -include"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\projects\dk_apps\templates\freertos_retarget\config\custom_config_qspi.h" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

startup/startup_ARMCM0.o: D:/IHUSRC/ihu/DA1468x_SDK_BTLE_v_1.0.6/sdk/bsp/startup/startup_ARMCM0.S
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU Assembler'
	arm-none-eabi-gcc -mcpu=cortex-m0 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -x assembler-with-cpp -Ddg_configBLACK_ORCA_IC_REV=BLACK_ORCA_IC_REV_A -Ddg_configBLACK_ORCA_IC_STEP=BLACK_ORCA_IC_STEP_E -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\config" -include"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\projects\dk_apps\templates\freertos_retarget\config\custom_config_qspi.h" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

startup/system_ARMCM0.o: D:/IHUSRC/ihu/DA1468x_SDK_BTLE_v_1.0.6/sdk/bsp/startup/system_ARMCM0.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -Ddg_configBLACK_ORCA_IC_REV=BLACK_ORCA_IC_REV_A -Ddg_configBLACK_ORCA_IC_STEP=BLACK_ORCA_IC_STEP_E -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\adapters\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\memory\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\config" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\osal" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\peripherals\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\free_rtos\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\system\sys_man\include" -I"//SrcBxxh/l1vmlayer" -I"D:\IHUSRC\ihu\SrcBxxh\l0comvm" -I"D:\IHUSRC\ihu\SrcBxxh\l1vmlayer" -I"D:\IHUSRC\ihu\SrcBxxh\l2frame" -I"D:\IHUSRC\ihu\SrcBxxh\l3appl" -include"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\projects\dk_apps\templates\freertos_retarget\config\custom_config_qspi.h" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

startup/vector_table.o: D:/IHUSRC/ihu/DA1468x_SDK_BTLE_v_1.0.6/sdk/bsp/startup/vector_table.S
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU Assembler'
	arm-none-eabi-gcc -mcpu=cortex-m0 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -x assembler-with-cpp -Ddg_configBLACK_ORCA_IC_REV=BLACK_ORCA_IC_REV_A -Ddg_configBLACK_ORCA_IC_STEP=BLACK_ORCA_IC_STEP_E -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\config" -include"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\projects\dk_apps\templates\freertos_retarget\config\custom_config_qspi.h" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


