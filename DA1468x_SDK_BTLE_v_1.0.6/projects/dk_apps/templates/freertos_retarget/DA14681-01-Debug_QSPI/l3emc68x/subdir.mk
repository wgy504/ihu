################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
D:/IHUPRJ/ihu/SrcBxxh/l3appl/l3emc68x.c 

OBJS += \
./l3emc68x/l3emc68x.o 

C_DEPS += \
./l3emc68x/l3emc68x.d 


# Each subdirectory must supply rules for building sources it contributes
l3emc68x/l3emc68x.o: D:/IHUPRJ/ihu/SrcBxxh/l3appl/l3emc68x.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -Ddg_configBLACK_ORCA_IC_REV=BLACK_ORCA_IC_REV_A -Ddg_configBLACK_ORCA_IC_STEP=BLACK_ORCA_IC_STEP_E -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\adapters\include" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\memory\include" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\config" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\include" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\osal" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\peripherals\include" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\free_rtos\include" -I"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\sdk\bsp\system\sys_man\include" -I"//SrcBxxh/l1vmlayer" -I"D:\IHUPRJ\ihu\SrcBxxh\l0comvm" -I"D:\IHUPRJ\ihu\SrcBxxh\l1vmlayer" -I"D:\IHUPRJ\ihu\SrcBxxh\l2frame" -I"D:\IHUPRJ\ihu\SrcBxxh\l3appl" -include"D:\IHUPRJ\ihu\DA1468x_SDK_BTLE_v_1.0.6\projects\dk_apps\templates\freertos_retarget\config\custom_config_qspi.h" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


