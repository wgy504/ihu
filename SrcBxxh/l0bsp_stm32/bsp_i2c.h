#ifndef __BSP_STM32_I2C_H
#define __BSP_STM32_I2C_H

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
	#error Un-correct constant definition
#endif

//不能在这里出现管脚的任何配置和初始化，必须在STM32CubeMX中完成，这里使用STM32CubeMX给出的端口俗名

#define IHU_BSP_STM32_I2C_TIMEOUT_TX_MAX                   300

	
//IHU_SYSDIM_MSG_BODY_LEN_MAX-2是因为发送到上层SPSVIRGO的数据缓冲区受到消息结构msg_struct_spsvirgo_l2frame_rcv_t的影响
#define IHU_BSP_STM32_I2C1_REC_MAX_LEN 					IHU_SYSDIM_L2FRAME_MSG_BODY_LEN_MAX	//最大接收数据长度
#define IHU_BSP_STM32_I2C2_REC_MAX_LEN 					IHU_SYSDIM_L2FRAME_MSG_BODY_LEN_MAX	//最大接收数据长度

//发送和接受数据的延迟时间长度
#define IHU_BSP_STM32_I2C_TX_MAX_DELAY_DURATION 100
#define IHU_BSP_STM32_I2C_RX_MAX_DELAY_DURATION 100

//因为是互斥的，所以以下必须通过开关量来判定，callback函数到底该采用哪种方式进行工作
#define BSP_STM32_I2C_WORK_MODE_IAU 1
#define BSP_STM32_I2C_WORK_MODE_MPU6050 2
#define BSP_STM32_I2C_WORK_MODE_CCL_SENSOR 3
#define BSP_STM32_I2C_WORK_MODE_CHOICE BSP_STM32_I2C_WORK_MODE_MPU6050

//本地定义的交换矩阵
//IAU模块，工作在SLAVE模式下
#define IHU_BSP_STM32_I2C_IAU_HANDLER							hi2c1
#define IHU_BSP_STM32_I2C_IAU_HANDLER_ID  				1
#define IHU_BSP_STM32_I2C_SPARE1_HANDLER					hi2c1
#define IHU_BSP_STM32_I2C_SPARE1_HANDLER_ID  			2

//MPU6050陀螺仪的I2C控制传感器，工作在MASTER模式下
#define IHU_BSP_STM32_I2C_MPU6050_HANDLER					hi2c1
#define IHU_BSP_STM32_I2C_MPU6050_HANDLER_ID  		1
////被控从模式设备的地址：当AD0接地或者悬空时，地址为0x68，接VCC时，地址为0x69
#define IHU_BSP_STM32_I2C_MPU6050_SENSOR_SLAVE_ADDRESS	    0x68  //0x30F  
#define IHU_BSP_STM32_I2C_MPU6050_FIX_FRAME_LEN	    0x10  //通信帧长度，用于接收帧控制

//CCL所对应的I2C控制传感器，工作在MASTER模式下
#define IHU_BSP_STM32_I2C_CCL_SENSOR_HANDLER			hi2c1
#define IHU_BSP_STM32_I2C_CCL_SENSOR_HANDLER_ID  	1
#define IHU_BSP_STM32_I2C_CCL_SENSOR_SLAVE_ADDRESS	    0x30F    //被控从模式设备的地址
#define IHU_BSP_STM32_I2C_CCL_FIX_FRAME_LEN	    0x10  //通信帧长度，用于接收帧控制

//全局函数
extern int ihu_bsp_stm32_i2c_slave_hw_init(void);
extern int ihu_bsp_stm32_i2c_iau_send_data(uint8_t* buff, uint16_t len);
extern int ihu_bsp_stm32_i2c_iau_rcv_data(uint8_t* buff, uint16_t len);	
extern int ihu_bsp_stm32_i2c_spare1_send_data(uint8_t* buff, uint16_t len);
extern int ihu_bsp_stm32_i2c_spare1_rcv_data(uint8_t* buff, uint16_t len);
//重载接收函数，以便通过IT中断方式搞定接收通信，否则需要通过轮询或者单独线程搞定，更加麻烦
void HAL_I2C_RxCpltCallback(I2C_HandleTypeDef *I2cHandle);


//CCL传感器发送控制数据
extern int ihu_bsp_stm32_i2c_ccl_send_data(uint8_t* buff, uint16_t len);
extern int ihu_bsp_stm32_i2c_ccl_sensor_rcv_data(uint8_t* buff, uint16_t len);


//向上提供的API
int8_t             ihu_bsp_stm32_i2c_mpu6050_write_data(uint16_t Addr, uint8_t Reg, uint8_t Value);
HAL_StatusTypeDef  ihu_bsp_stm32_i2c_mpu6050_write_buffer(uint16_t Addr, uint8_t Reg, uint16_t RegSize, uint8_t *pBuffer, uint16_t Length);
uint8_t            ihu_bsp_stm32_i2c_mpu6050_read_data(uint16_t Addr, uint8_t Reg);
HAL_StatusTypeDef  ihu_bsp_stm32_i2c_mpu6050_read_buffer(uint16_t Addr, uint8_t Reg, uint16_t RegSize, uint8_t *pBuffer, uint16_t Length);
HAL_StatusTypeDef  ihu_bsp_stm32_i2c_mpu6050_is_device_ready(uint16_t DevAddress, uint32_t Trials);


#ifdef __cplusplus
}
#endif
#endif  /* __BSP_STM32_I2C_H */


