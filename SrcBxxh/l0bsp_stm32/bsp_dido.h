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
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_IAP_ID)
	#include "commsgiap.h"	
#else
	#error Un-correct constant definition
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

extern int dido_time_base;

#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)
//本地定义的交换矩阵	
	#define BSP_STM32_DIDO_DHT11_GPIO_PIN										CUBEMX_PIN_F2_GPIO_DI_DHT11_Pin
	#define BSP_STM32_DIDO_DHT11_GPIO												CUBEMX_PIN_F2_GPIO_DI_DHT11_GPIO_Port                         
	#define BSP_STM32_DIDO_DHT11_Dout_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOD_CLK_ENABLE()
//	#define BSP_STM32_DIDO_FALL_GPIO_PIN										CUBEMX_PIN_F2_GPIO_DI_FALL_SNESOR_Pin                
//	#define BSP_STM32_DIDO_FALL_GPIO												CUBEMX_PIN_F2_GPIO_DI_FALL_SNESOR_GPIO_Port          
	#define BSP_STM32_DIDO_SHAKE_GPIO_PIN										CUBEMX_PIN_F2_GPIO_DI_SHAKE_SENSOR_Pin               
	#define BSP_STM32_DIDO_SHAKE_GPIO												CUBEMX_PIN_F2_GPIO_DI_SHAKE_SENSOR_GPIO_Port         
	#define BSP_STM32_DIDO_SMOKE_GPIO_PIN										CUBEMX_PIN_F2_GPIO_DI_SMOKE_SENSOR_Pin               
	#define BSP_STM32_DIDO_SMOKE_GPIO												CUBEMX_PIN_F2_GPIO_DI_SMOKE_SENSOR_GPIO_Port         
	#define BSP_STM32_DIDO_WATER_GPIO_PIN										CUBEMX_PIN_F2_GPIO_DI_WATER_SENSOR_Pin               
	#define BSP_STM32_DIDO_WATER_GPIO												CUBEMX_PIN_F2_GPIO_DI_WATER_SENSOR_GPIO_Port
	#define BSP_STM32_DIDO_BLE_ATCMD_CTRL_GPIO_PIN					CUBEMX_PIN_F2_DO_BLE_ATCMD_Pin                         
	#define BSP_STM32_DIDO_BLE_ATCMD_CTRL_GPIO							CUBEMX_PIN_F2_DO_BLE_ATCMD_GPIO_Port    
	#define BSP_STM32_DIDO_DOOR1_RESTRICTION_GPIO_PIN				CUBEMX_PIN_F2_GPIO_DI_DOOR1_RESTRICTION_Pin          
	#define BSP_STM32_DIDO_DOOR1_RESTRICTION_GPIO						CUBEMX_PIN_F2_GPIO_DI_DOOR1_RESTRICTION_GPIO_Port    
	#define BSP_STM32_DIDO_DOOR2_RESTRICTION_GPIO_PIN				CUBEMX_PIN_F2_GPIO_DI_DOOR2_RESTRICTION_Pin          
	#define BSP_STM32_DIDO_DOOR2_RESTRICTION_GPIO						CUBEMX_PIN_F2_GPIO_DI_DOOR2_RESTRICTION_GPIO_Port    
	#define BSP_STM32_DIDO_DOOR3_RESTRICTION_GPIO_PIN				CUBEMX_PIN_F2_GPIO_DI_DOOR3_RESTRICTION_Pin          
	#define BSP_STM32_DIDO_DOOR3_RESTRICTION_GPIO						CUBEMX_PIN_F2_GPIO_DI_DOOR3_RESTRICTION_GPIO_Port    
	#define BSP_STM32_DIDO_DOOR4_RESTRICTION_GPIO_PIN				CUBEMX_PIN_F2_GPIO_DI_DOOR4_RESTRICTION_Pin          
	#define BSP_STM32_DIDO_DOOR4_RESTRICTION_GPIO						CUBEMX_PIN_F2_GPIO_DI_DOOR4_RESTRICTION_GPIO_Port     
	#define BSP_STM32_DIDO_LOCK_ACT_FLAG_PIN								CUBEMX_PIN_F2_DI_LOCK_ACT_FLAG_Pin          
	#define BSP_STM32_DIDO_LOCK_ACT_FLAG_GPIO								CUBEMX_PIN_F2_DI_LOCK_ACT_FLAG_GPIO_Port

	//PWR CONTROL
	#define BSP_STM32_DIDO_CPU_PWR_CTRL_GPIO_PIN					  CUBEMX_PIN_F2_DO_CPU_PWR_Pin                      
	#define BSP_STM32_DIDO_CPU_PWR_CTRL_GPIO							  CUBEMX_PIN_F2_DO_CPU_PWR_GPIO_Port   	
	#define BSP_STM32_DIDO_MQ2_CAM_PWR_CTRL_GPIO_PIN				CUBEMX_PIN_F2_DO_MQ2_CAM_PWR_Pin                      
	#define BSP_STM32_DIDO_MQ2_CAM_PWR_CTRL_GPIO						CUBEMX_PIN_F2_DO_MQ2_CAM_PWR_GPIO_Port
	#define BSP_STM32_DIDO_GPRSMOD_PWR_SUPPLY_GPIO_PIN			CUBEMX_PIN_F2_DO_GPRSMOD_PWR_SUPPLY_Pin
  #define BSP_STM32_DIDO_GPRSMOD_PWR_SUPPLY_GPIO					CUBEMX_PIN_F2_DO_GPRSMOD_PWR_SUPPLY_GPIO_Port               
	#define BSP_STM32_DIDO_GPRSMOD_PWR_KEY_GPIO_PIN				  CUBEMX_PIN_F2_DO_GPRSMOD_PWR_KEY_Pin
  #define BSP_STM32_DIDO_GPRSMOD_PWR_KEY_GPIO						  CUBEMX_PIN_F2_DO_GPRSMOD_PWR_KEY_GPIO_Port               
	#define BSP_STM32_DIDO_SENSOR_PWR_CTRL_GPIO_PIN					CUBEMX_PIN_F2_DO_SENSOR_PWR_Pin                      
	#define BSP_STM32_DIDO_SENSOR_PWR_CTRL_GPIO							CUBEMX_PIN_F2_DO_SENSOR_PWR_GPIO_Port    

  //新板子的方案：LOCK_POWER_CTRL
	#define BSP_STM32_DIDO_LOCK1_DO1_GPIO_PIN								CUBEMX_PIN_F2_GPIO_DO_LOCK1_SWITCH_Pin                  
	#define BSP_STM32_DIDO_LOCK1_DO1_GPIO										CUBEMX_PIN_F2_GPIO_DO_LOCK1_SWITCH_GPIO_Port            
	#define BSP_STM32_DIDO_LOCK2_DO1_GPIO_PIN								CUBEMX_PIN_F2_GPIO_DO_LOCK2_SWITCH_Pin                  
	#define BSP_STM32_DIDO_LOCK2_DO1_GPIO										CUBEMX_PIN_F2_GPIO_DO_LOCK2_SWITCH_GPIO_Port        
	#define BSP_STM32_DIDO_LOCK3_DO1_GPIO_PIN								CUBEMX_PIN_F2_GPIO_DO_LOCK3_SWITCH_Pin                  
	#define BSP_STM32_DIDO_LOCK3_DO1_GPIO										CUBEMX_PIN_F2_GPIO_DO_LOCK3_SWITCH_GPIO_Port   
	#define BSP_STM32_DIDO_LOCK4_DO1_GPIO_PIN								CUBEMX_PIN_F2_GPIO_DO_LOCK4_SWITCH_Pin                  
	#define BSP_STM32_DIDO_LOCK4_DO1_GPIO										CUBEMX_PIN_F2_GPIO_DO_LOCK4_SWITCH_GPIO_Port


	//有效的函数体
	#define BSP_STM32_DIDO_DHT11_READ								HAL_GPIO_ReadPin(BSP_STM32_DIDO_DHT11_GPIO, BSP_STM32_DIDO_DHT11_GPIO_PIN)
	#define BSP_STM32_DIDO_DHT11_Dout_LOW()         HAL_GPIO_WritePin(BSP_STM32_DIDO_DHT11_GPIO,BSP_STM32_DIDO_DHT11_GPIO_PIN,GPIO_PIN_RESET) 
	#define BSP_STM32_DIDO_DHT11_Dout_HIGH()        HAL_GPIO_WritePin(BSP_STM32_DIDO_DHT11_GPIO,BSP_STM32_DIDO_DHT11_GPIO_PIN,GPIO_PIN_SET)
	#define BSP_STM32_DIDO_DHT11_Data_IN()	        HAL_GPIO_ReadPin(BSP_STM32_DIDO_DHT11_GPIO,BSP_STM32_DIDO_DHT11_GPIO_PIN)	
//	#define BSP_STM32_DIDO_FALL_READ								HAL_GPIO_ReadPin(BSP_STM32_DIDO_FALL_GPIO, BSP_STM32_DIDO_FALL_GPIO_PIN)
	#define BSP_STM32_DIDO_SHAKE_READ								HAL_GPIO_ReadPin(BSP_STM32_DIDO_SHAKE_GPIO, BSP_STM32_DIDO_SHAKE_GPIO_PIN)
	#define BSP_STM32_DIDO_SMOKE_READ								HAL_GPIO_ReadPin(BSP_STM32_DIDO_SMOKE_GPIO, BSP_STM32_DIDO_SMOKE_GPIO_PIN)
	#define BSP_STM32_DIDO_WATER_READ								HAL_GPIO_ReadPin(BSP_STM32_DIDO_WATER_GPIO, BSP_STM32_DIDO_WATER_GPIO_PIN)
	#define BSP_STM32_DIDO_LOCK1_DO1_ON             HAL_GPIO_WritePin(BSP_STM32_DIDO_LOCK1_DO1_GPIO, BSP_STM32_DIDO_LOCK1_DO1_GPIO_PIN, GPIO_PIN_SET)    	// 输出高电平
	#define BSP_STM32_DIDO_LOCK1_DO1_OFF            HAL_GPIO_WritePin(BSP_STM32_DIDO_LOCK1_DO1_GPIO, BSP_STM32_DIDO_LOCK1_DO1_GPIO_PIN, GPIO_PIN_RESET)  	// 输出低电平
	#define BSP_STM32_DIDO_DOOR1_RESTRICTION_READ		HAL_GPIO_ReadPin(BSP_STM32_DIDO_DOOR1_RESTRICTION_GPIO, BSP_STM32_DIDO_DOOR1_RESTRICTION_GPIO_PIN)
	#define BSP_STM32_DIDO_LOCK2_DO1_ON             HAL_GPIO_WritePin(BSP_STM32_DIDO_LOCK2_DO1_GPIO, BSP_STM32_DIDO_LOCK2_DO1_GPIO_PIN, GPIO_PIN_SET)    	// 输出高电平
	#define BSP_STM32_DIDO_LOCK2_DO1_OFF            HAL_GPIO_WritePin(BSP_STM32_DIDO_LOCK2_DO1_GPIO, BSP_STM32_DIDO_LOCK2_DO1_GPIO_PIN, GPIO_PIN_RESET)  	// 输出低电平
	#define BSP_STM32_DIDO_DOOR2_RESTRICTION_READ		HAL_GPIO_ReadPin(BSP_STM32_DIDO_DOOR2_RESTRICTION_GPIO, BSP_STM32_DIDO_DOOR2_RESTRICTION_GPIO_PIN)
	#define BSP_STM32_DIDO_LOCK3_DO1_ON             HAL_GPIO_WritePin(BSP_STM32_DIDO_LOCK3_DO1_GPIO, BSP_STM32_DIDO_LOCK3_DO1_GPIO_PIN, GPIO_PIN_SET)    	// 输出高电平
	#define BSP_STM32_DIDO_LOCK3_DO1_OFF            HAL_GPIO_WritePin(BSP_STM32_DIDO_LOCK3_DO1_GPIO, BSP_STM32_DIDO_LOCK3_DO1_GPIO_PIN, GPIO_PIN_RESET)  	// 输出低电平
	#define BSP_STM32_DIDO_DOOR3_RESTRICTION_READ		HAL_GPIO_ReadPin(BSP_STM32_DIDO_DOOR3_RESTRICTION_GPIO, BSP_STM32_DIDO_DOOR3_RESTRICTION_GPIO_PIN)
	#define BSP_STM32_DIDO_LOCK_ACT_FLAG_READ				HAL_GPIO_ReadPin(BSP_STM32_DIDO_LOCK_ACT_FLAG_GPIO, BSP_STM32_DIDO_LOCK_ACT_FLAG_PIN)
	#define BSP_STM32_DIDO_LOCK4_DO1_ON             HAL_GPIO_WritePin(BSP_STM32_DIDO_LOCK4_DO1_GPIO, BSP_STM32_DIDO_LOCK4_DO1_GPIO_PIN, GPIO_PIN_SET)    	// 输出高电平
	#define BSP_STM32_DIDO_LOCK4_DO1_OFF            HAL_GPIO_WritePin(BSP_STM32_DIDO_LOCK4_DO1_GPIO, BSP_STM32_DIDO_LOCK4_DO1_GPIO_PIN, GPIO_PIN_RESET)  	// 输出低电平
	#define BSP_STM32_DIDO_DOOR4_RESTRICTION_READ		HAL_GPIO_ReadPin(BSP_STM32_DIDO_DOOR4_RESTRICTION_GPIO, BSP_STM32_DIDO_DOOR4_RESTRICTION_GPIO_PIN)
	#define BSP_STM32_DIDO_BLE_ATCMD_CTRL_ON      	HAL_GPIO_WritePin(BSP_STM32_DIDO_BLE_ATCMD_CTRL_GPIO, BSP_STM32_DIDO_BLE_ATCMD_CTRL_GPIO_PIN, GPIO_PIN_SET)    	// 输出高电平
	#define BSP_STM32_DIDO_BLE_ATCMD_CTRL_OFF     	HAL_GPIO_WritePin(BSP_STM32_DIDO_BLE_ATCMD_CTRL_GPIO, BSP_STM32_DIDO_BLE_ATCMD_CTRL_GPIO_PIN, GPIO_PIN_RESET)  	// 输出低电平

	//POWER CONTROL
	#define BSP_STM32_DIDO_CPU_PWR_CTRL_ON        	HAL_GPIO_WritePin(BSP_STM32_DIDO_CPU_PWR_CTRL_GPIO, BSP_STM32_DIDO_CPU_PWR_CTRL_GPIO_PIN, GPIO_PIN_SET)    	// 输出高电平
	#define BSP_STM32_DIDO_CPU_PWR_CTRL_OFF       	HAL_GPIO_WritePin(BSP_STM32_DIDO_CPU_PWR_CTRL_GPIO, BSP_STM32_DIDO_CPU_PWR_CTRL_GPIO_PIN, GPIO_PIN_RESET)  	// 输出低电平
	#define BSP_STM32_DIDO_MQ2_CAM_PWR_CTRL_ON      HAL_GPIO_WritePin(BSP_STM32_DIDO_MQ2_CAM_PWR_CTRL_GPIO, BSP_STM32_DIDO_MQ2_CAM_PWR_CTRL_GPIO_PIN, GPIO_PIN_RESET)  	// 输出低电平
	#define BSP_STM32_DIDO_MQ2_CAM_PWR_CTRL_OFF      HAL_GPIO_WritePin(BSP_STM32_DIDO_MQ2_CAM_PWR_CTRL_GPIO, BSP_STM32_DIDO_MQ2_CAM_PWR_CTRL_GPIO_PIN, GPIO_PIN_SET)    	// 输出高电平
	#define BSP_STM32_DIDO_GPRSMOD_PWR_SUPPLY_ON    HAL_GPIO_WritePin(BSP_STM32_DIDO_GPRSMOD_PWR_SUPPLY_GPIO, BSP_STM32_DIDO_GPRSMOD_PWR_SUPPLY_GPIO_PIN, GPIO_PIN_SET)    	// 输出高电平
	#define BSP_STM32_DIDO_GPRSMOD_PWR_SUPPLY_OFF   HAL_GPIO_WritePin(BSP_STM32_DIDO_GPRSMOD_PWR_SUPPLY_GPIO, BSP_STM32_DIDO_GPRSMOD_PWR_SUPPLY_GPIO_PIN, GPIO_PIN_RESET)  	// 输出低电平
	#define BSP_STM32_DIDO_GPRSMOD_PWR_KEY_ON       HAL_GPIO_WritePin(BSP_STM32_DIDO_GPRSMOD_PWR_KEY_GPIO, BSP_STM32_DIDO_GPRSMOD_PWR_KEY_GPIO_PIN, GPIO_PIN_SET)    	// 输出高电平
	#define BSP_STM32_DIDO_GPRSMOD_PWR_KEY_OFF      HAL_GPIO_WritePin(BSP_STM32_DIDO_GPRSMOD_PWR_KEY_GPIO, BSP_STM32_DIDO_GPRSMOD_PWR_KEY_GPIO_PIN, GPIO_PIN_RESET)  	// 输出低电平
	#define BSP_STM32_DIDO_SENSOR_PWR_CTRL_ON     	HAL_GPIO_WritePin(BSP_STM32_DIDO_SENSOR_PWR_CTRL_GPIO, BSP_STM32_DIDO_SENSOR_PWR_CTRL_GPIO_PIN, GPIO_PIN_RESET)  	// 输出低电平
	#define BSP_STM32_DIDO_SENSOR_PWR_CTRL_OFF      HAL_GPIO_WritePin(BSP_STM32_DIDO_SENSOR_PWR_CTRL_GPIO, BSP_STM32_DIDO_SENSOR_PWR_CTRL_GPIO_PIN, GPIO_PIN_SET)    	// 输出高电平


//External APIs
	//DHT11必须先初始化以后，才能单独被调用使用
	extern void    ihu_bsp_stm32_dido_f2board_dht11_init( void );
	extern int16_t ihu_bsp_stm32_dido_f2board_dht11_temp_read(void);
	extern int16_t ihu_bsp_stm32_dido_f2board_dht11_humid_read(void);
//	extern int16_t ihu_bsp_stm32_dido_f2board_fall_read(void);
	extern int16_t ihu_bsp_stm32_dido_f2board_shake_read(void);
	extern int16_t ihu_bsp_stm32_dido_f2board_smoke_read(void);
	extern int16_t ihu_bsp_stm32_dido_f2board_water_read(void);
//	extern int16_t ihu_bsp_stm32_dido_f2board_lock1_di1_trigger_read(void);
//	extern int16_t ihu_bsp_stm32_dido_f2board_lock1_di2_tongue_read(void);
	extern void ihu_bsp_stm32_dido_f2board_lock1_do1_on(void);
	extern void ihu_bsp_stm32_dido_f2board_lock1_do1_off(void);
	extern int16_t ihu_bsp_stm32_dido_f2board_door1_restriction_read(void);
//	extern int16_t ihu_bsp_stm32_dido_f2board_lock2_di1_trigger_read(void);
//	extern int16_t ihu_bsp_stm32_dido_f2board_lock2_di2_tongue_read(void);
	extern void ihu_bsp_stm32_dido_f2board_lock2_do1_on(void);
	extern void ihu_bsp_stm32_dido_f2board_lock2_do1_off(void);
	extern int16_t ihu_bsp_stm32_dido_f2board_door2_restriction_read(void);
//	extern int16_t ihu_bsp_stm32_dido_f2board_lock3_di1_trigger_read(void);
//	extern int16_t ihu_bsp_stm32_dido_f2board_lock3_di2_tongue_read(void);
	extern void ihu_bsp_stm32_dido_f2board_lock3_do1_on(void);
	extern void ihu_bsp_stm32_dido_f2board_lock3_do1_off(void);
	extern int16_t ihu_bsp_stm32_dido_f2board_door3_restriction_read(void);
//	extern int16_t ihu_bsp_stm32_dido_f2board_lock4_di1_trigger_read(void);
//	extern int16_t ihu_bsp_stm32_dido_f2board_lock4_di2_tongue_read(void);
	extern void ihu_bsp_stm32_dido_f2board_lock4_do1_on(void);
	extern void ihu_bsp_stm32_dido_f2board_lock4_do1_off(void);
	extern int16_t ihu_bsp_stm32_dido_f2board_door4_restriction_read(void);
	extern void ihu_bsp_stm32_dido_f2board_cpu_power_ctrl_on(void);
	extern void ihu_bsp_stm32_dido_f2board_cpu_power_ctrl_off(void);
	extern void ihu_bsp_stm32_dido_f2board_mq2_cam_power_ctrl_on(void);
	extern void ihu_bsp_stm32_dido_f2board_mq2_cam_power_ctrl_off(void);
	extern void ihu_bsp_stm32_dido_f2board_gprsmod_power_supply_on(void);
	extern void ihu_bsp_stm32_dido_f2board_gprsmod_power_supply_off(void);
	extern void ihu_bsp_stm32_dido_f2board_gprsmod_power_key_on(void);
	extern void ihu_bsp_stm32_dido_f2board_gprsmod_power_key_off(void);
	extern void ihu_bsp_stm32_dido_f2board_sensor_power_ctrl_on(void);
	extern void ihu_bsp_stm32_dido_f2board_sensor_power_ctrl_off(void);
	extern void ihu_bsp_stm32_dido_f2board_ble_atcmd_mode_ctrl_on(void);
	extern void ihu_bsp_stm32_dido_f2board_ble_atcmd_mode_ctrl_off(void);
	extern int16_t ihu_bsp_stm32_dido_f2board_lock_act_flag_read(void);

//Local APIs
static void func_bsp_stm32_dido_dht11_mode_IPU(void);
static void func_bsp_stm32_dido_dht11_mode_out_PP(void);
static uint8_t func_bsp_stm32_dido_dht11_read_byte(void);
uint8_t func_bsp_stm32_dido_dht11_read_temp_and_humidity(StrDht11DataTypeDef_t * DHT11_Data);

#endif //(IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)

//如果是其它项目，同样做类似的定义。复用比较困难，因为差异较大，但代码拷贝的概率较大



/***********************   DS18B20 连接引脚定义  **************************/
//独立的绝对管脚定义，未来使用时，需要将此关键映射到MAIN.h中指定的管脚
#define BSP_STM32_DIDO_DS18B20_Dout_GPIO_CLK_ENABLE()              __HAL_RCC_GPIOD_CLK_ENABLE()
#define BSP_STM32_DIDO_DS18B20_Dout_PORT                           GPIOD
#define BSP_STM32_DIDO_DS18B20_Dout_PIN                            GPIO_PIN_3
#define BSP_STM32_DIDO_DS18B20_Dout_LOW()                          HAL_GPIO_WritePin(BSP_STM32_DIDO_DS18B20_Dout_PORT,BSP_STM32_DIDO_DS18B20_Dout_PIN,GPIO_PIN_RESET) 
#define BSP_STM32_DIDO_DS18B20_Dout_HIGH()                         HAL_GPIO_WritePin(BSP_STM32_DIDO_DS18B20_Dout_PORT,BSP_STM32_DIDO_DS18B20_Dout_PIN,GPIO_PIN_SET)
#define BSP_STM32_DIDO_DS18B20_Data_IN()	                         HAL_GPIO_ReadPin(BSP_STM32_DIDO_DS18B20_Dout_PORT,BSP_STM32_DIDO_DS18B20_Dout_PIN)

//External APIs
extern uint8_t ihu_bsp_stm32_dido_ds18b20_init(void);
extern int16_t ihu_bsp_stm32_dido_f2board_ds18b20_temp_read(void);

//Local APIs
void func_bsp_stm32_dido_ds18b20_read_id( uint8_t * ds18b20_id);
float func_bsp_stm32_dido_ds18b20_get_temp_skip_rom(void);
float func_bsp_stm32_dido_ds18b20_get_temp_match_rom(uint8_t *ds18b20_id);
static void func_bsp_stm32_dido_ds18b20_set_mode_IPU(void);
static void func_bsp_stm32_dido_ds18b20_set_mode_out_PP(void);
static void func_bsp_stm32_dido_ds18b20_reset(void);
static uint8_t func_bsp_stm32_dido_ds18b20_presence(void);
static uint8_t func_bsp_stm32_dido_ds18b20_read_bit(void);
static uint8_t func_bsp_stm32_dido_ds18b20_read_byte(void);
static void func_bsp_stm32_dido_ds18b20_write_byte(uint8_t dat);
static void func_bsp_stm32_dido_ds18b20_skip_rom(void);
static void func_bsp_stm32_dido_ds18b20_match_rom(void);


#ifdef __cplusplus
}
#endif
#endif  // __BSP_STM32_DIDO_H

