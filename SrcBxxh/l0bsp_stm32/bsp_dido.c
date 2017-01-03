/**
  ******************************************************************************
  * 文件名程: bsp_dido.c 
  * 作    者: BXXH
  * 版    本: V1.0
  * 编写日期: 2015-10-04
  * 功    能: 基于HAL库的板载LED灯底层驱动实现
  ******************************************************************************
  ******************************************************************************
  */
/* 包含头文件 ----------------------------------------------------------------*/
#include "bsp_dido.h"

//从MAIN.x中继承过来的函数

//本地全局变量



/* 函数体 --------------------------------------------------------------------*/
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)
//该传感器的具体读数，未来再研究
int16_t ihu_bsp_stm32_dido_f2board_dht11_temp_read(void)
{
    return BSP_STM32_DIDO_DHT11_READ;
}

//该传感器的具体读数，未来再研究
int16_t ihu_bsp_stm32_dido_f2board_dht11_humid_read(void)
{
    return BSP_STM32_DIDO_FALL_READ;
}					

int16_t ihu_bsp_stm32_dido_f2board_shake_read(void)
{
    return BSP_STM32_DIDO_SHAKE_READ;
}					

int16_t ihu_bsp_stm32_dido_f2board_smoke_read(void)
{
    return BSP_STM32_DIDO_SMOKE_READ;
}					
								
int16_t ihu_bsp_stm32_dido_f2board_water_read(void)
{
    return BSP_STM32_DIDO_WATER_READ;
}					
								
int16_t ihu_bsp_stm32_dido_f2board_lock1_di1_trigger_read(void)
{
    return BSP_STM32_DIDO_LOCK1_DI1_TRIGGER_READ;
}									

int16_t ihu_bsp_stm32_dido_f2board_lock1_di2_tongue_read(void)
{
    return BSP_STM32_DIDO_LOCK1_DI2_TONGUE_READ;
}

void ihu_bsp_stm32_dido_f2board_lock1_do1_on(void)
{
    BSP_STM32_DIDO_LOCK1_DO1_ON;
}

void ihu_bsp_stm32_dido_f2board_lock1_do1_off(void)
{
    BSP_STM32_DIDO_LOCK1_DO1_OFF;
}

int16_t ihu_bsp_stm32_dido_f2board_door1_restriction_read(void)
{
    return BSP_STM32_DIDO_DOOR1_RESTRICTION_READ;
}         
            
int16_t ihu_bsp_stm32_dido_f2board_lock2_di1_trigger_read(void)
{
    return BSP_STM32_DIDO_LOCK2_DI1_TRIGGER_READ;
}									

int16_t ihu_bsp_stm32_dido_f2board_lock2_di2_tongue_read(void)
{
    return BSP_STM32_DIDO_LOCK2_DI2_TONGUE_READ;
}

void ihu_bsp_stm32_dido_f2board_lock2_do1_on(void)
{
    BSP_STM32_DIDO_LOCK2_DO1_ON;
}

void ihu_bsp_stm32_dido_f2board_lock2_do1_off(void)
{
    BSP_STM32_DIDO_LOCK2_DO1_OFF;
}

int16_t ihu_bsp_stm32_dido_f2board_door2_restriction_read(void)
{
    return BSP_STM32_DIDO_DOOR2_RESTRICTION_READ;
}   	

int16_t ihu_bsp_stm32_dido_f2board_lock3_di1_trigger_read(void)
{
    return BSP_STM32_DIDO_LOCK3_DI1_TRIGGER_READ;
}									

int16_t ihu_bsp_stm32_dido_f2board_lock3_di2_tongue_read(void)
{
    return BSP_STM32_DIDO_LOCK3_DI2_TONGUE_READ;
}

void ihu_bsp_stm32_dido_f2board_lock3_do1_on(void)
{
    BSP_STM32_DIDO_LOCK3_DO1_ON;
}

void ihu_bsp_stm32_dido_f2board_lock3_do1_off(void)
{
    BSP_STM32_DIDO_LOCK3_DO1_OFF;
}

int16_t ihu_bsp_stm32_dido_f2board_door3_restriction_read(void)
{
    return BSP_STM32_DIDO_DOOR3_RESTRICTION_READ;
}   	

int16_t ihu_bsp_stm32_dido_f2board_lock4_di1_trigger_read(void)
{
    return BSP_STM32_DIDO_LOCK4_DI1_TRIGGER_READ;
}									

int16_t ihu_bsp_stm32_dido_f2board_lock4_di2_tongue_read(void)
{
    return BSP_STM32_DIDO_LOCK4_DI2_TONGUE_READ;
}

void ihu_bsp_stm32_dido_f2board_lock4_do1_on(void)
{
    BSP_STM32_DIDO_LOCK4_DO1_ON;
}

void ihu_bsp_stm32_dido_f2board_lock4_do1_off(void)
{
    BSP_STM32_DIDO_LOCK4_DO1_OFF;
}

int16_t ihu_bsp_stm32_dido_f2board_door4_restriction_read(void)
{
    return BSP_STM32_DIDO_DOOR4_RESTRICTION_READ;
}   	

void ihu_bsp_stm32_dido_f2board_gprsmod_power_ctrl_on(void)
{
    BSP_STM32_DIDO_GPRSMOD_PWR_CTRL_ON;
}

void ihu_bsp_stm32_dido_f2board_gprsmod_power_ctrl_off(void)
{
    BSP_STM32_DIDO_GPRSMOD_PWR_CTRL_OFF;
}

void ihu_bsp_stm32_dido_f2board_ble_power_ctrl_on(void)
{
    BSP_STM32_DIDO_BLE_PWR_CTRL_ON;
}

void ihu_bsp_stm32_dido_f2board_ble_power_ctrl_off(void)
{
    BSP_STM32_DIDO_BLE_PWR_CTRL_OFF;
}

void ihu_bsp_stm32_dido_f2board_rfid_power_ctrl_on(void)
{
    BSP_STM32_DIDO_RFID_PWR_CTRL_ON;
}

void ihu_bsp_stm32_dido_f2board_rfid_power_ctrl_off(void)
{
    BSP_STM32_DIDO_RFID_PWR_CTRL_OFF;
}

void ihu_bsp_stm32_dido_f2board_sensor_power_ctrl_on(void)
{
    BSP_STM32_DIDO_SENSOR_PWR_CTRL_ON;
}

void ihu_bsp_stm32_dido_f2board_sensor_power_ctrl_off(void)
{
    BSP_STM32_DIDO_SENSOR_PWR_CTRL_OFF;
}      		

#endif //(IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)


