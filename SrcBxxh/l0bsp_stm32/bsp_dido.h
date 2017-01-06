#ifndef __BSP_STM32_DIDO_H
#define __BSP_STM32_DIDO_H

#ifdef __cplusplus
extern "C" {
#endif

/////////////////////////////////////////////////////////////////////////////////////////////
#include "stm32f2xx_hal.h"
#include "stdio.h"
#include "string.h"
#include "sysdim.h"
#include "vmfreeoslayer.h"
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)
	#include "commsgccl.h"
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID)
	#include "commsgbfsc.h"
#else
#endif

//不能在这里出现管脚的任何配置和初始化，必须在STM32CubeMX中完成，这里使用STM32CubeMX给出的端口俗名

/************************ DHT11 数据类型定义******************************/
typedef struct StrDht11DataTypeDef
{
	uint8_t  humi_high8bit;		//原始数据：湿度高8位
	uint8_t  humi_low8bit;	 	//原始数据：湿度低8位
	uint8_t  temp_high8bit;	 	//原始数据：温度高8位
	uint8_t  temp_low8bit;	 	//原始数据：温度高8位
	uint8_t  check_sum;	 	    //校验和
  float    humidity;        //实际湿度
  float    temperature;     //实际温度  
}StrDht11DataTypeDef_t;	
	

#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)
//本地定义的交换矩阵	
	#define BSP_STM32_DIDO_DHT11_GPIO_PIN										CUBEMX_PIN_F2_GPIO_DI_DHT11_Pin
	#define BSP_STM32_DIDO_DHT11_GPIO												CUBEMX_PIN_F2_GPIO_DI_DHT11_GPIO_Port                         
	#define BSP_STM32_DIDO_DHT11_Dout_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOD_CLK_ENABLE()
	#define BSP_STM32_DIDO_FALL_GPIO_PIN										CUBEMX_PIN_F2_GPIO_DI_FALL_SNESOR_Pin                
	#define BSP_STM32_DIDO_FALL_GPIO												CUBEMX_PIN_F2_GPIO_DI_FALL_SNESOR_GPIO_Port          
	#define BSP_STM32_DIDO_SHAKE_GPIO_PIN										CUBEMX_PIN_F2_GPIO_DI_SHAKE_SENSOR_Pin               
	#define BSP_STM32_DIDO_SHAKE_GPIO												CUBEMX_PIN_F2_GPIO_DI_SHAKE_SENSOR_GPIO_Port         
	#define BSP_STM32_DIDO_SMOKE_GPIO_PIN										CUBEMX_PIN_F2_GPIO_DI_SMOKE_SENSOR_Pin               
	#define BSP_STM32_DIDO_SMOKE_GPIO												CUBEMX_PIN_F2_GPIO_DI_SMOKE_SENSOR_GPIO_Port         
	#define BSP_STM32_DIDO_WATER_GPIO_PIN										CUBEMX_PIN_F2_GPIO_DI_WATER_SENSOR_Pin               
	#define BSP_STM32_DIDO_WATER_GPIO												CUBEMX_PIN_F2_GPIO_DI_WATER_SENSOR_GPIO_Port         
	#define BSP_STM32_DIDO_LOCK1_DI1_TRIGGER_GPIO_PIN				CUBEMX_PIN_F2_GPIO_DI_LOCK1_DI1_TRIGGER_Pin          
	#define BSP_STM32_DIDO_LOCK1_DI1_TRIGGER_GPIO						CUBEMX_PIN_F2_GPIO_DI_LOCK1_DI1_TRIGGER_GPIO_Port    
	#define BSP_STM32_DIDO_LOCK1_DI2_TONGUE_GPIO_PIN				CUBEMX_PIN_F2_GPIO_DI_LOCK1_DI2_TONGUE_Pin           
	#define BSP_STM32_DIDO_LOCK1_DI2_TONGUE_GPIO						CUBEMX_PIN_F2_GPIO_DI_LOCK1_DI2_TONGUE_GPIO_Port     
	#define BSP_STM32_DIDO_LOCK1_DO1_GPIO_PIN								CUBEMX_PIN_F2_GPIO_DO_LOCK1_DO1_Pin                  
	#define BSP_STM32_DIDO_LOCK1_DO1_GPIO										CUBEMX_PIN_F2_GPIO_DO_LOCK1_DO1_GPIO_Port            
	#define BSP_STM32_DIDO_DOOR1_RESTRICTION_GPIO_PIN				CUBEMX_PIN_F2_GPIO_DI_DOOR1_RESTRICTION_Pin          
	#define BSP_STM32_DIDO_DOOR1_RESTRICTION_GPIO						CUBEMX_PIN_F2_GPIO_DI_DOOR1_RESTRICTION_GPIO_Port    
	#define BSP_STM32_DIDO_LOCK2_DI1_TRIGGER_GPIO_PIN				CUBEMX_PIN_F2_GPIO_DI_LOCK2_DI1_TRIGGER_Pin          
	#define BSP_STM32_DIDO_LOCK2_DI1_TRIGGER_GPIO						CUBEMX_PIN_F2_GPIO_DI_LOCK2_DI1_TRIGGER_GPIO_Port    
	#define BSP_STM32_DIDO_LOCK2_DI2_TONGUE_GPIO_PIN				CUBEMX_PIN_F2_GPIO_DI_LOCK2_DI2_TONGUE_Pin           
	#define BSP_STM32_DIDO_LOCK2_DI2_TONGUE_GPIO						CUBEMX_PIN_F2_GPIO_DI_LOCK2_DI2_TONGUE_GPIO_Port     
	#define BSP_STM32_DIDO_LOCK2_DO1_GPIO_PIN								CUBEMX_PIN_F2_GPIO_DO_LOCK2_DO1_Pin                  
	#define BSP_STM32_DIDO_LOCK2_DO1_GPIO										CUBEMX_PIN_F2_GPIO_DO_LOCK2_DO1_GPIO_Port            
	#define BSP_STM32_DIDO_DOOR2_RESTRICTION_GPIO_PIN				CUBEMX_PIN_F2_GPIO_DI_DOOR2_RESTRICTION_Pin          
	#define BSP_STM32_DIDO_DOOR2_RESTRICTION_GPIO						CUBEMX_PIN_F2_GPIO_DI_DOOR2_RESTRICTION_GPIO_Port    
	#define BSP_STM32_DIDO_LOCK3_DI1_TRIGGER_GPIO_PIN				CUBEMX_PIN_F2_GPIO_DI_LOCK3_DI1_TRIGGER_Pin          
	#define BSP_STM32_DIDO_LOCK3_DI1_TRIGGER_GPIO						CUBEMX_PIN_F2_GPIO_DI_LOCK3_DI1_TRIGGER_GPIO_Port    
	#define BSP_STM32_DIDO_LOCK3_DI2_TONGUE_GPIO_PIN				CUBEMX_PIN_F2_GPIO_DI_LOCK3_DI2_TONGUE_Pin           
	#define BSP_STM32_DIDO_LOCK3_DI2_TONGUE_GPIO						CUBEMX_PIN_F2_GPIO_DI_LOCK3_DI2_TONGUE_GPIO_Port     
	#define BSP_STM32_DIDO_LOCK3_DO1_GPIO_PIN								CUBEMX_PIN_F2_GPIO_DO_LOCK3_DO1_Pin                  
	#define BSP_STM32_DIDO_LOCK3_DO1_GPIO										CUBEMX_PIN_F2_GPIO_DO_LOCK3_DO1_GPIO_Port            
	#define BSP_STM32_DIDO_DOOR3_RESTRICTION_GPIO_PIN				CUBEMX_PIN_F2_GPIO_DI_DOOR3_RESTRICTION_Pin          
	#define BSP_STM32_DIDO_DOOR3_RESTRICTION_GPIO						CUBEMX_PIN_F2_GPIO_DI_DOOR3_RESTRICTION_GPIO_Port    
	#define BSP_STM32_DIDO_LOCK4_DI1_TRIGGER_GPIO_PIN				CUBEMX_PIN_F2_GPIO_DI_LOCK4_DI1_TRIGGER_Pin          
	#define BSP_STM32_DIDO_LOCK4_DI1_TRIGGER_GPIO						CUBEMX_PIN_F2_GPIO_DI_LOCK4_DI1_TRIGGER_GPIO_Port    
	#define BSP_STM32_DIDO_LOCK4_DI2_TONGUE_GPIO_PIN				CUBEMX_PIN_F2_GPIO_DI_LOCK4_DI2_TONGUE_Pin           
	#define BSP_STM32_DIDO_LOCK4_DI2_TONGUE_GPIO						CUBEMX_PIN_F2_GPIO_DI_LOCK4_DI2_TONGUE_GPIO_Port     
	#define BSP_STM32_DIDO_LOCK4_DO1_GPIO_PIN								CUBEMX_PIN_F2_GPIO_DO_LOCK4_DO1_Pin                  
	#define BSP_STM32_DIDO_LOCK4_DO1_GPIO										CUBEMX_PIN_F2_GPIO_DO_LOCK4_DO1_GPIO_Port            
	#define BSP_STM32_DIDO_DOOR4_RESTRICTION_GPIO_PIN				CUBEMX_PIN_F2_GPIO_DI_DOOR4_RESTRICTION_Pin          
	#define BSP_STM32_DIDO_DOOR4_RESTRICTION_GPIO						CUBEMX_PIN_F2_GPIO_DI_DOOR4_RESTRICTION_GPIO_Port    
	#define BSP_STM32_DIDO_GPRSMOD_PWR_CTRL_GPIO_PIN				CUBEMX_PIN_F2_DO_GPRSMOD_PWR_Pin                     
	#define BSP_STM32_DIDO_GPRSMOD_PWR_CTRL_GPIO						CUBEMX_PIN_F2_DO_GPRSMOD_PWR_GPIO_Port               
	#define BSP_STM32_DIDO_BLE_PWR_CTRL_GPIO_PIN						CUBEMX_PIN_F2_DO_BLE_PWR_Pin                         
	#define BSP_STM32_DIDO_BLE_PWR_CTRL_GPIO								CUBEMX_PIN_F2_DO_BLE_PWR_GPIO_Port                   
	#define BSP_STM32_DIDO_RFID_PWR_CTRL_GPIO_PIN						CUBEMX_PIN_F2_DO_RFID_PWR_Pin                        
	#define BSP_STM32_DIDO_RFID_PWR_CTRL_GPIO								CUBEMX_PIN_F2_DO_RFID_PWR_GPIO_Port                  
	#define BSP_STM32_DIDO_SENSOR_PWR_CTRL_GPIO_PIN					CUBEMX_PIN_F2_DO_SENSOR_PWR_Pin                      
	#define BSP_STM32_DIDO_SENSOR_PWR_CTRL_GPIO							CUBEMX_PIN_F2_DO_SENSOR_PWR_GPIO_Port                

//有效的函数体
	#define BSP_STM32_DIDO_DHT11_READ								HAL_GPIO_ReadPin(BSP_STM32_DIDO_DHT11_GPIO, BSP_STM32_DIDO_DHT11_GPIO_PIN)
	#define BSP_STM32_DIDO_DHT11_Dout_LOW()         HAL_GPIO_WritePin(BSP_STM32_DIDO_DHT11_GPIO,BSP_STM32_DIDO_DHT11_GPIO_PIN,GPIO_PIN_RESET) 
	#define BSP_STM32_DIDO_DHT11_Dout_HIGH()        HAL_GPIO_WritePin(BSP_STM32_DIDO_DHT11_GPIO,BSP_STM32_DIDO_DHT11_GPIO_PIN,GPIO_PIN_SET)
	#define BSP_STM32_DIDO_DHT11_Data_IN()	        HAL_GPIO_ReadPin(BSP_STM32_DIDO_DHT11_GPIO,BSP_STM32_DIDO_DHT11_GPIO_PIN)	
	#define BSP_STM32_DIDO_FALL_READ								HAL_GPIO_ReadPin(BSP_STM32_DIDO_FALL_GPIO, BSP_STM32_DIDO_FALL_GPIO_PIN)
	#define BSP_STM32_DIDO_SHAKE_READ								HAL_GPIO_ReadPin(BSP_STM32_DIDO_SHAKE_GPIO, BSP_STM32_DIDO_SHAKE_GPIO_PIN)
	#define BSP_STM32_DIDO_SMOKE_READ								HAL_GPIO_ReadPin(BSP_STM32_DIDO_SMOKE_GPIO, BSP_STM32_DIDO_SMOKE_GPIO_PIN)
	#define BSP_STM32_DIDO_WATER_READ								HAL_GPIO_ReadPin(BSP_STM32_DIDO_WATER_GPIO, BSP_STM32_DIDO_WATER_GPIO_PIN)
	#define BSP_STM32_DIDO_LOCK1_DI1_TRIGGER_READ		HAL_GPIO_ReadPin(BSP_STM32_DIDO_LOCK1_DI1_TRIGGER_GPIO, BSP_STM32_DIDO_LOCK1_DI1_TRIGGER_GPIO_PIN)
	#define BSP_STM32_DIDO_LOCK1_DI2_TONGUE_READ		HAL_GPIO_ReadPin(BSP_STM32_DIDO_LOCK1_DI2_TONGUE_GPIO, BSP_STM32_DIDO_LOCK1_DI2_TONGUE_GPIO_PIN)
	#define BSP_STM32_DIDO_LOCK1_DO1_ON             HAL_GPIO_WritePin(BSP_STM32_DIDO_LOCK1_DO1_GPIO, BSP_STM32_DIDO_LOCK1_DO1_GPIO_PIN, GPIO_PIN_SET)    	// 输出高电平
	#define BSP_STM32_DIDO_LOCK1_DO1_OFF            HAL_GPIO_WritePin(BSP_STM32_DIDO_LOCK1_DO1_GPIO, BSP_STM32_DIDO_LOCK1_DO1_GPIO_PIN, GPIO_PIN_RESET)  	// 输出低电平
	#define BSP_STM32_DIDO_DOOR1_RESTRICTION_READ		HAL_GPIO_ReadPin(BSP_STM32_DIDO_DOOR1_RESTRICTION_GPIO, BSP_STM32_DIDO_DOOR1_RESTRICTION_GPIO_PIN)
	#define BSP_STM32_DIDO_LOCK2_DI1_TRIGGER_READ		HAL_GPIO_ReadPin(BSP_STM32_DIDO_LOCK2_DI1_TRIGGER_GPIO, BSP_STM32_DIDO_LOCK2_DI1_TRIGGER_GPIO_PIN)
	#define BSP_STM32_DIDO_LOCK2_DI2_TONGUE_READ		HAL_GPIO_ReadPin(BSP_STM32_DIDO_LOCK2_DI2_TONGUE_GPIO, BSP_STM32_DIDO_LOCK2_DI2_TONGUE_GPIO_PIN)
	#define BSP_STM32_DIDO_LOCK2_DO1_ON             HAL_GPIO_WritePin(BSP_STM32_DIDO_LOCK2_DO1_GPIO, BSP_STM32_DIDO_LOCK2_DO1_GPIO_PIN, GPIO_PIN_SET)    	// 输出高电平
	#define BSP_STM32_DIDO_LOCK2_DO1_OFF            HAL_GPIO_WritePin(BSP_STM32_DIDO_LOCK2_DO1_GPIO, BSP_STM32_DIDO_LOCK2_DO1_GPIO_PIN, GPIO_PIN_RESET)  	// 输出低电平
	#define BSP_STM32_DIDO_DOOR2_RESTRICTION_READ		HAL_GPIO_ReadPin(BSP_STM32_DIDO_DOOR2_RESTRICTION_GPIO, BSP_STM32_DIDO_DOOR2_RESTRICTION_GPIO_PIN)
	#define BSP_STM32_DIDO_LOCK3_DI1_TRIGGER_READ		HAL_GPIO_ReadPin(BSP_STM32_DIDO_LOCK3_DI1_TRIGGER_GPIO, BSP_STM32_DIDO_LOCK3_DI1_TRIGGER_GPIO_PIN)
	#define BSP_STM32_DIDO_LOCK3_DI2_TONGUE_READ		HAL_GPIO_ReadPin(BSP_STM32_DIDO_LOCK3_DI2_TONGUE_GPIO, BSP_STM32_DIDO_LOCK3_DI2_TONGUE_GPIO_PIN)
	#define BSP_STM32_DIDO_LOCK3_DO1_ON             HAL_GPIO_WritePin(BSP_STM32_DIDO_LOCK3_DO1_GPIO, BSP_STM32_DIDO_LOCK3_DO1_GPIO_PIN, GPIO_PIN_SET)    	// 输出高电平
	#define BSP_STM32_DIDO_LOCK3_DO1_OFF            HAL_GPIO_WritePin(BSP_STM32_DIDO_LOCK3_DO1_GPIO, BSP_STM32_DIDO_LOCK3_DO1_GPIO_PIN, GPIO_PIN_RESET)  	// 输出低电平
	#define BSP_STM32_DIDO_DOOR3_RESTRICTION_READ		HAL_GPIO_ReadPin(BSP_STM32_DIDO_DOOR3_RESTRICTION_GPIO, BSP_STM32_DIDO_DOOR3_RESTRICTION_GPIO_PIN)
	#define BSP_STM32_DIDO_LOCK4_DI1_TRIGGER_READ		HAL_GPIO_ReadPin(BSP_STM32_DIDO_LOCK4_DI1_TRIGGER_GPIO, BSP_STM32_DIDO_LOCK4_DI1_TRIGGER_GPIO_PIN)
	#define BSP_STM32_DIDO_LOCK4_DI2_TONGUE_READ		HAL_GPIO_ReadPin(BSP_STM32_DIDO_LOCK4_DI2_TONGUE_GPIO, BSP_STM32_DIDO_LOCK4_DI2_TONGUE_GPIO_PIN)
	#define BSP_STM32_DIDO_LOCK4_DO1_ON             HAL_GPIO_WritePin(BSP_STM32_DIDO_LOCK4_DO1_GPIO, BSP_STM32_DIDO_LOCK4_DO1_GPIO_PIN, GPIO_PIN_SET)    	// 输出高电平
	#define BSP_STM32_DIDO_LOCK4_DO1_OFF            HAL_GPIO_WritePin(BSP_STM32_DIDO_LOCK4_DO1_GPIO, BSP_STM32_DIDO_LOCK4_DO1_GPIO_PIN, GPIO_PIN_RESET)  	// 输出低电平
	#define BSP_STM32_DIDO_DOOR4_RESTRICTION_READ		HAL_GPIO_ReadPin(BSP_STM32_DIDO_DOOR4_RESTRICTION_GPIO, BSP_STM32_DIDO_DOOR4_RESTRICTION_GPIO_PIN)
	#define BSP_STM32_DIDO_GPRSMOD_PWR_CTRL_ON      HAL_GPIO_WritePin(BSP_STM32_DIDO_GPRSMOD_PWR_CTRL_GPIO, BSP_STM32_DIDO_GPRSMOD_PWR_CTRL_GPIO_PIN, GPIO_PIN_SET)    	// 输出高电平
	#define BSP_STM32_DIDO_GPRSMOD_PWR_CTRL_OFF     HAL_GPIO_WritePin(BSP_STM32_DIDO_GPRSMOD_PWR_CTRL_GPIO, BSP_STM32_DIDO_GPRSMOD_PWR_CTRL_GPIO_PIN, GPIO_PIN_RESET)  	// 输出低电平
	#define BSP_STM32_DIDO_BLE_PWR_CTRL_ON      		HAL_GPIO_WritePin(BSP_STM32_DIDO_BLE_PWR_CTRL_GPIO, BSP_STM32_DIDO_BLE_PWR_CTRL_GPIO_PIN, GPIO_PIN_SET)    	// 输出高电平
	#define BSP_STM32_DIDO_BLE_PWR_CTRL_OFF     		HAL_GPIO_WritePin(BSP_STM32_DIDO_BLE_PWR_CTRL_GPIO, BSP_STM32_DIDO_BLE_PWR_CTRL_GPIO_PIN, GPIO_PIN_RESET)  	// 输出低电平
	#define BSP_STM32_DIDO_RFID_PWR_CTRL_ON      		HAL_GPIO_WritePin(BSP_STM32_DIDO_RFID_PWR_CTRL_GPIO, BSP_STM32_DIDO_RFID_PWR_CTRL_GPIO_PIN, GPIO_PIN_SET)    	// 输出高电平
	#define BSP_STM32_DIDO_RFID_PWR_CTRL_OFF     		HAL_GPIO_WritePin(BSP_STM32_DIDO_RFID_PWR_CTRL_GPIO, BSP_STM32_DIDO_RFID_PWR_CTRL_GPIO_PIN, GPIO_PIN_RESET)  	// 输出低电平
	#define BSP_STM32_DIDO_SENSOR_PWR_CTRL_ON      	HAL_GPIO_WritePin(BSP_STM32_DIDO_SENSOR_PWR_CTRL_GPIO, BSP_STM32_DIDO_SENSOR_PWR_CTRL_GPIO_PIN, GPIO_PIN_SET)    	// 输出高电平
	#define BSP_STM32_DIDO_SENSOR_PWR_CTRL_OFF     	HAL_GPIO_WritePin(BSP_STM32_DIDO_SENSOR_PWR_CTRL_GPIO, BSP_STM32_DIDO_SENSOR_PWR_CTRL_GPIO_PIN, GPIO_PIN_RESET)  	// 输出低电平

//External APIs
	//DHT11必须先初始化以后，才能单独被调用使用
	extern void    ihu_bsp_stm32_dido_f2board_dht11_init( void );
	extern int16_t ihu_bsp_stm32_dido_f2board_dht11_temp_read(void);
	extern int16_t ihu_bsp_stm32_dido_f2board_dht11_humid_read(void);
	extern int16_t ihu_bsp_stm32_dido_f2board_shake_read(void);
	extern int16_t ihu_bsp_stm32_dido_f2board_smoke_read(void);
	extern int16_t ihu_bsp_stm32_dido_f2board_water_read(void);
	extern int16_t ihu_bsp_stm32_dido_f2board_lock1_di1_trigger_read(void);
	extern int16_t ihu_bsp_stm32_dido_f2board_lock1_di2_tongue_read(void);
	extern void ihu_bsp_stm32_dido_f2board_lock1_do1_on(void);
	extern void ihu_bsp_stm32_dido_f2board_lock1_do1_off(void);
	extern int16_t ihu_bsp_stm32_dido_f2board_door1_restriction_read(void);
	extern int16_t ihu_bsp_stm32_dido_f2board_lock2_di1_trigger_read(void);
	extern int16_t ihu_bsp_stm32_dido_f2board_lock2_di2_tongue_read(void);
	extern void ihu_bsp_stm32_dido_f2board_lock2_do1_on(void);
	extern void ihu_bsp_stm32_dido_f2board_lock2_do1_off(void);
	extern int16_t ihu_bsp_stm32_dido_f2board_door2_restriction_read(void);
	extern int16_t ihu_bsp_stm32_dido_f2board_lock3_di1_trigger_read(void);
	extern int16_t ihu_bsp_stm32_dido_f2board_lock3_di2_tongue_read(void);
	extern void ihu_bsp_stm32_dido_f2board_lock3_do1_on(void);
	extern void ihu_bsp_stm32_dido_f2board_lock3_do1_off(void);
	extern int16_t ihu_bsp_stm32_dido_f2board_door3_restriction_read(void);
	extern int16_t ihu_bsp_stm32_dido_f2board_lock4_di1_trigger_read(void);
	extern int16_t ihu_bsp_stm32_dido_f2board_lock4_di2_tongue_read(void);
	extern void ihu_bsp_stm32_dido_f2board_lock4_do1_on(void);
	extern void ihu_bsp_stm32_dido_f2board_lock4_do1_off(void);
	extern int16_t ihu_bsp_stm32_dido_f2board_door4_restriction_read(void);
	extern void ihu_bsp_stm32_dido_f2board_gprsmod_power_ctrl_on(void);
	extern void ihu_bsp_stm32_dido_f2board_gprsmod_power_ctrl_off(void);
	extern void ihu_bsp_stm32_dido_f2board_ble_power_ctrl_on(void);
	extern void ihu_bsp_stm32_dido_f2board_ble_power_ctrl_off(void);
	extern void ihu_bsp_stm32_dido_f2board_rfid_power_ctrl_on(void);
	extern void ihu_bsp_stm32_dido_f2board_rfid_power_ctrl_off(void);
	extern void ihu_bsp_stm32_dido_f2board_sensor_power_ctrl_on(void);
	extern void ihu_bsp_stm32_dido_f2board_sensor_power_ctrl_off(void);

//Local APIs
static void func_bsp_stm32_dido_dht11_mode_IPU(void);
static void func_bsp_stm32_dido_dht11_mode_out_PP(void);
static uint8_t func_bsp_stm32_dido_dht11_read_byte(void);
uint8_t func_bsp_stm32_dido_dht11_read_temp_and_humidity(StrDht11DataTypeDef_t * DHT11_Data);

#endif //(IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)

//如果是其它项目，同样做类似的定义。复用比较困难，因为差异较大，但代码拷贝的概率较大



#ifdef __cplusplus
}
#endif
#endif  // __BSP_STM32_DIDO_H

