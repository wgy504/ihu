################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
D:/IHUSRC/ihu/SrcBxxh/l2frame/l2adclibra.c \
D:/IHUSRC/ihu/SrcBxxh/l2frame/l2didocap.c \
D:/IHUSRC/ihu/SrcBxxh/l2frame/l2ethorion.c \
D:/IHUSRC/ihu/SrcBxxh/l2frame/l2gpiocancer.c \
D:/IHUSRC/ihu/SrcBxxh/l2frame/l2i2caries.c \
D:/IHUSRC/ihu/SrcBxxh/l2frame/l2ledpisces.c \
D:/IHUSRC/ihu/SrcBxxh/l2frame/l2pwmtaurus.c \
D:/IHUSRC/ihu/SrcBxxh/l2frame/l2spileo.c \
D:/IHUSRC/ihu/SrcBxxh/l2frame/l2spsvirgo.c 

OBJS += \
./l2frame/l2adclibra.o \
./l2frame/l2didocap.o \
./l2frame/l2ethorion.o \
./l2frame/l2gpiocancer.o \
./l2frame/l2i2caries.o \
./l2frame/l2ledpisces.o \
./l2frame/l2pwmtaurus.o \
./l2frame/l2spileo.o \
./l2frame/l2spsvirgo.o 

C_DEPS += \
./l2frame/l2adclibra.d \
./l2frame/l2didocap.d \
./l2frame/l2ethorion.d \
./l2frame/l2gpiocancer.d \
./l2frame/l2i2caries.d \
./l2frame/l2ledpisces.d \
./l2frame/l2pwmtaurus.d \
./l2frame/l2spileo.d \
./l2frame/l2spsvirgo.d 


# Each subdirectory must supply rules for building sources it contributes
l2frame/l2adclibra.o: D:/IHUSRC/ihu/SrcBxxh/l2frame/l2adclibra.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -Ddg_configBLACK_ORCA_IC_REV=BLACK_ORCA_IC_REV_A -Ddg_configBLACK_ORCA_IC_STEP=BLACK_ORCA_IC_STEP_E -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\adapters\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\memory\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\config" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\osal" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\peripherals\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\free_rtos\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\system\sys_man\include" -I"//SrcBxxh/l1vmlayer" -I"D:\IHUSRC\ihu\SrcBxxh\l0comvm" -I"D:\IHUSRC\ihu\SrcBxxh\l1vmlayer" -I"D:\IHUSRC\ihu\SrcBxxh\l2frame" -I"D:\IHUSRC\ihu\SrcBxxh\l3appl" -include"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\projects\dk_apps\templates\freertos_retarget\config\custom_config_qspi.h" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

l2frame/l2didocap.o: D:/IHUSRC/ihu/SrcBxxh/l2frame/l2didocap.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -Ddg_configBLACK_ORCA_IC_REV=BLACK_ORCA_IC_REV_A -Ddg_configBLACK_ORCA_IC_STEP=BLACK_ORCA_IC_STEP_E -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\adapters\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\memory\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\config" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\osal" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\peripherals\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\free_rtos\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\system\sys_man\include" -I"//SrcBxxh/l1vmlayer" -I"D:\IHUSRC\ihu\SrcBxxh\l0comvm" -I"D:\IHUSRC\ihu\SrcBxxh\l1vmlayer" -I"D:\IHUSRC\ihu\SrcBxxh\l2frame" -I"D:\IHUSRC\ihu\SrcBxxh\l3appl" -include"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\projects\dk_apps\templates\freertos_retarget\config\custom_config_qspi.h" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

l2frame/l2ethorion.o: D:/IHUSRC/ihu/SrcBxxh/l2frame/l2ethorion.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -Ddg_configBLACK_ORCA_IC_REV=BLACK_ORCA_IC_REV_A -Ddg_configBLACK_ORCA_IC_STEP=BLACK_ORCA_IC_STEP_E -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\adapters\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\memory\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\config" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\osal" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\peripherals\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\free_rtos\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\system\sys_man\include" -I"//SrcBxxh/l1vmlayer" -I"D:\IHUSRC\ihu\SrcBxxh\l0comvm" -I"D:\IHUSRC\ihu\SrcBxxh\l1vmlayer" -I"D:\IHUSRC\ihu\SrcBxxh\l2frame" -I"D:\IHUSRC\ihu\SrcBxxh\l3appl" -include"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\projects\dk_apps\templates\freertos_retarget\config\custom_config_qspi.h" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

l2frame/l2gpiocancer.o: D:/IHUSRC/ihu/SrcBxxh/l2frame/l2gpiocancer.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -Ddg_configBLACK_ORCA_IC_REV=BLACK_ORCA_IC_REV_A -Ddg_configBLACK_ORCA_IC_STEP=BLACK_ORCA_IC_STEP_E -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\adapters\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\memory\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\config" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\osal" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\peripherals\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\free_rtos\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\system\sys_man\include" -I"//SrcBxxh/l1vmlayer" -I"D:\IHUSRC\ihu\SrcBxxh\l0comvm" -I"D:\IHUSRC\ihu\SrcBxxh\l1vmlayer" -I"D:\IHUSRC\ihu\SrcBxxh\l2frame" -I"D:\IHUSRC\ihu\SrcBxxh\l3appl" -include"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\projects\dk_apps\templates\freertos_retarget\config\custom_config_qspi.h" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

l2frame/l2i2caries.o: D:/IHUSRC/ihu/SrcBxxh/l2frame/l2i2caries.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -Ddg_configBLACK_ORCA_IC_REV=BLACK_ORCA_IC_REV_A -Ddg_configBLACK_ORCA_IC_STEP=BLACK_ORCA_IC_STEP_E -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\adapters\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\memory\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\config" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\osal" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\peripherals\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\free_rtos\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\system\sys_man\include" -I"//SrcBxxh/l1vmlayer" -I"D:\IHUSRC\ihu\SrcBxxh\l0comvm" -I"D:\IHUSRC\ihu\SrcBxxh\l1vmlayer" -I"D:\IHUSRC\ihu\SrcBxxh\l2frame" -I"D:\IHUSRC\ihu\SrcBxxh\l3appl" -include"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\projects\dk_apps\templates\freertos_retarget\config\custom_config_qspi.h" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

l2frame/l2ledpisces.o: D:/IHUSRC/ihu/SrcBxxh/l2frame/l2ledpisces.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -Ddg_configBLACK_ORCA_IC_REV=BLACK_ORCA_IC_REV_A -Ddg_configBLACK_ORCA_IC_STEP=BLACK_ORCA_IC_STEP_E -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\adapters\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\memory\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\config" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\osal" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\peripherals\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\free_rtos\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\system\sys_man\include" -I"//SrcBxxh/l1vmlayer" -I"D:\IHUSRC\ihu\SrcBxxh\l0comvm" -I"D:\IHUSRC\ihu\SrcBxxh\l1vmlayer" -I"D:\IHUSRC\ihu\SrcBxxh\l2frame" -I"D:\IHUSRC\ihu\SrcBxxh\l3appl" -include"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\projects\dk_apps\templates\freertos_retarget\config\custom_config_qspi.h" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

l2frame/l2pwmtaurus.o: D:/IHUSRC/ihu/SrcBxxh/l2frame/l2pwmtaurus.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -Ddg_configBLACK_ORCA_IC_REV=BLACK_ORCA_IC_REV_A -Ddg_configBLACK_ORCA_IC_STEP=BLACK_ORCA_IC_STEP_E -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\adapters\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\memory\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\config" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\osal" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\peripherals\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\free_rtos\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\system\sys_man\include" -I"//SrcBxxh/l1vmlayer" -I"D:\IHUSRC\ihu\SrcBxxh\l0comvm" -I"D:\IHUSRC\ihu\SrcBxxh\l1vmlayer" -I"D:\IHUSRC\ihu\SrcBxxh\l2frame" -I"D:\IHUSRC\ihu\SrcBxxh\l3appl" -include"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\projects\dk_apps\templates\freertos_retarget\config\custom_config_qspi.h" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

l2frame/l2spileo.o: D:/IHUSRC/ihu/SrcBxxh/l2frame/l2spileo.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -Ddg_configBLACK_ORCA_IC_REV=BLACK_ORCA_IC_REV_A -Ddg_configBLACK_ORCA_IC_STEP=BLACK_ORCA_IC_STEP_E -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\adapters\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\memory\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\config" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\osal" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\peripherals\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\free_rtos\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\system\sys_man\include" -I"//SrcBxxh/l1vmlayer" -I"D:\IHUSRC\ihu\SrcBxxh\l0comvm" -I"D:\IHUSRC\ihu\SrcBxxh\l1vmlayer" -I"D:\IHUSRC\ihu\SrcBxxh\l2frame" -I"D:\IHUSRC\ihu\SrcBxxh\l3appl" -include"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\projects\dk_apps\templates\freertos_retarget\config\custom_config_qspi.h" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

l2frame/l2spsvirgo.o: D:/IHUSRC/ihu/SrcBxxh/l2frame/l2spsvirgo.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -Ddg_configBLACK_ORCA_IC_REV=BLACK_ORCA_IC_REV_A -Ddg_configBLACK_ORCA_IC_STEP=BLACK_ORCA_IC_STEP_E -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\adapters\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\memory\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\config" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\osal" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\peripherals\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\free_rtos\include" -I"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\system\sys_man\include" -I"//SrcBxxh/l1vmlayer" -I"D:\IHUSRC\ihu\SrcBxxh\l0comvm" -I"D:\IHUSRC\ihu\SrcBxxh\l1vmlayer" -I"D:\IHUSRC\ihu\SrcBxxh\l2frame" -I"D:\IHUSRC\ihu\SrcBxxh\l3appl" -include"D:\IHUSRC\ihu\DA1468x_SDK_BTLE_v_1.0.6\projects\dk_apps\templates\freertos_retarget\config\custom_config_qspi.h" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


