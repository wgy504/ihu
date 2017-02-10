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

#define IHU_BSP_STM32_I2C_TIMEOUT_TX_MAX                   3000

	
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
#define BSP_STM32_I2C_WORK_MODE_CHOICE BSP_STM32_I2C_WORK_MODE_CCL_SENSOR

//本地定义的交换矩阵
//IAU模块，工作在SLAVE模式下
#define IHU_BSP_STM32_I2C_IAU_HANDLER							hi2c1
#define IHU_BSP_STM32_I2C_IAU_HANDLER_ID  				1
#define IHU_BSP_STM32_I2C_SPARE1_HANDLER					hi2c1
#define IHU_BSP_STM32_I2C_SPARE1_HANDLER_ID  			2

//MPU6050陀螺仪的I2C控制传感器，工作在MASTER模式下
#define IHU_BSP_STM32_I2C_MPU6050_HANDLER					hi2c1
#define IHU_BSP_STM32_I2C_MPU6050_HANDLER_ID  		1
#define IHU_BSP_STM32_I2C_MPU6050_SENSOR_SLAVE_ADDRESS	    0x30F  //被控从模式设备的地址
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
//MPU6050必须先初始化，然后才能读取加速度/陀螺仪位置/温度数据
extern void 	ihu_bsp_stm32_i2c_mpu6050_init(void);
extern int8_t ihu_bsp_stm32_i2c_mpu6050_acc_read(int16_t *accData);
extern int8_t ihu_bsp_stm32_i2c_mpu6050_gyro_read(int16_t *gyroData);
extern int8_t ihu_bsp_stm32_i2c_mpu6050_temp_read(int16_t tempData);
//CCL传感器发送控制数据
extern int ihu_bsp_stm32_i2c_ccl_send_data(uint8_t* buff, uint16_t len);
extern int ihu_bsp_stm32_i2c_ccl_sensor_rcv_data(uint8_t* buff, uint16_t len);


//Local APIs
void func_bsp_stm32_i2c_mpu6050_write_data(uint16_t Addr, uint8_t Reg, uint8_t Value);
HAL_StatusTypeDef  func_bsp_stm32_i2c_mpu6050_write_buffer(uint16_t Addr, uint8_t Reg, uint16_t RegSize, uint8_t *pBuffer, uint16_t Length);
uint8_t func_bsp_stm32_i2c_mpu6050_read_data(uint16_t Addr, uint8_t Reg);
HAL_StatusTypeDef  func_bsp_stm32_i2c_mpu6050_read_buffer(uint16_t Addr, uint8_t Reg, uint16_t RegSize, uint8_t *pBuffer, uint16_t Length);
HAL_StatusTypeDef  func_bsp_stm32_i2c_mpu6050_is_device_ready(uint16_t DevAddress, uint32_t Trials);
void func_bsp_stm32_i2c_mpu6050_read_temp(int16_t *tempData);
void func_bsp_stm32_i2c_mpu6050_read_gyro(int16_t *gyroData);
void func_bsp_stm32_i2c_mpu6050_read_acc(int16_t *accData);
void func_bsp_stm32_i2c_mpu6050_return_temp(int16_t*Temperature);	
uint8_t func_bsp_stm32_i2c_mpu6050_return_id(void);


////////////////////////////////////////////////////////////////////////////////////////////////////////
/* MPU6050独特的宏定义 --------------------------------------------------------------------*/
////////////////////////////////////////////////////////////////////////////////////////////////////////
#define BSP_STM32_MPU6050_ADDRESS            0x68
#define BSP_STM32_MPU6050_WHO_AM_I           0x75
#define BSP_STM32_MPU6050_SMPLRT_DIV            0  //8000Hz
#define BSP_STM32_MPU6050_DLPF_CFG              0
#define BSP_STM32_MPU6050_GYRO_OUT           0x43  //MPU6050陀螺仪数据寄存器地址
#define BSP_STM32_MPU6050_ACC_OUT            0x3B  //MPU6050加速度数据寄存器地址
#define BSP_STM32_MPU6050_SLAVE_ADDRESS      0xd0  //MPU6050器件读地址
#define BSP_STM32_MPU6050_ADDRESS_AD0_LOW     0x68 // address pin low (GND), default for InvenSense evaluation board
#define BSP_STM32_MPU6050_ADDRESS_AD0_HIGH    0x69 // address pin high (VCC)
#define BSP_STM32_MPU6050_DEFAULT_ADDRESS     BSP_STM32_MPU6050_ADDRESS_AD0_LOW
#define BSP_STM32_MPU6050_RA_XG_OFFS_TC       0x00 //[7] PWR_MODE, [6:1] XG_OFFS_TC, [0] OTP_BNK_VLD
#define BSP_STM32_MPU6050_RA_YG_OFFS_TC       0x01 //[7] PWR_MODE, [6:1] YG_OFFS_TC, [0] OTP_BNK_VLD
#define BSP_STM32_MPU6050_RA_ZG_OFFS_TC       0x02 //[7] PWR_MODE, [6:1] ZG_OFFS_TC, [0] OTP_BNK_VLD
#define BSP_STM32_MPU6050_RA_X_FINE_GAIN      0x03 //[7:0] X_FINE_GAIN
#define BSP_STM32_MPU6050_RA_Y_FINE_GAIN      0x04 //[7:0] Y_FINE_GAIN
#define BSP_STM32_MPU6050_RA_Z_FINE_GAIN      0x05 //[7:0] Z_FINE_GAIN
#define BSP_STM32_MPU6050_RA_XA_OFFS_H        0x06 //[15:0] XA_OFFS
#define BSP_STM32_MPU6050_RA_XA_OFFS_L_TC     0x07
#define BSP_STM32_MPU6050_RA_YA_OFFS_H        0x08 //[15:0] YA_OFFS
#define BSP_STM32_MPU6050_RA_YA_OFFS_L_TC     0x09
#define BSP_STM32_MPU6050_RA_ZA_OFFS_H        0x0A //[15:0] ZA_OFFS
#define BSP_STM32_MPU6050_RA_ZA_OFFS_L_TC     0x0B
#define BSP_STM32_MPU6050_RA_XG_OFFS_USRH     0x13 //[15:0] XG_OFFS_USR
#define BSP_STM32_MPU6050_RA_XG_OFFS_USRL     0x14
#define BSP_STM32_MPU6050_RA_YG_OFFS_USRH     0x15 //[15:0] YG_OFFS_USR
#define BSP_STM32_MPU6050_RA_YG_OFFS_USRL     0x16
#define BSP_STM32_MPU6050_RA_ZG_OFFS_USRH     0x17 //[15:0] ZG_OFFS_USR
#define BSP_STM32_MPU6050_RA_ZG_OFFS_USRL     0x18
#define BSP_STM32_MPU6050_RA_SMPLRT_DIV       0x19
#define BSP_STM32_MPU6050_RA_CONFIG           0x1A
#define BSP_STM32_MPU6050_RA_GYRO_CONFIG      0x1B
#define BSP_STM32_MPU6050_RA_ACCEL_CONFIG     0x1C
#define BSP_STM32_MPU6050_RA_FF_THR           0x1D
#define BSP_STM32_MPU6050_RA_FF_DUR           0x1E
#define BSP_STM32_MPU6050_RA_MOT_THR          0x1F
#define BSP_STM32_MPU6050_RA_MOT_DUR          0x20
#define BSP_STM32_MPU6050_RA_ZRMOT_THR        0x21
#define BSP_STM32_MPU6050_RA_ZRMOT_DUR        0x22
#define BSP_STM32_MPU6050_RA_FIFO_EN          0x23
#define BSP_STM32_MPU6050_RA_I2C_MST_CTRL     0x24
#define BSP_STM32_MPU6050_RA_I2C_SLV0_ADDR    0x25
#define BSP_STM32_MPU6050_RA_I2C_SLV0_REG     0x26
#define BSP_STM32_MPU6050_RA_I2C_SLV0_CTRL    0x27
#define BSP_STM32_MPU6050_RA_I2C_SLV1_ADDR    0x28
#define BSP_STM32_MPU6050_RA_I2C_SLV1_REG     0x29
#define BSP_STM32_MPU6050_RA_I2C_SLV1_CTRL    0x2A
#define BSP_STM32_MPU6050_RA_I2C_SLV2_ADDR    0x2B
#define BSP_STM32_MPU6050_RA_I2C_SLV2_REG     0x2C
#define BSP_STM32_MPU6050_RA_I2C_SLV2_CTRL    0x2D
#define BSP_STM32_MPU6050_RA_I2C_SLV3_ADDR    0x2E
#define BSP_STM32_MPU6050_RA_I2C_SLV3_REG     0x2F
#define BSP_STM32_MPU6050_RA_I2C_SLV3_CTRL    0x30
#define BSP_STM32_MPU6050_RA_I2C_SLV4_ADDR    0x31
#define BSP_STM32_MPU6050_RA_I2C_SLV4_REG     0x32
#define BSP_STM32_MPU6050_RA_I2C_SLV4_DO      0x33
#define BSP_STM32_MPU6050_RA_I2C_SLV4_CTRL    0x34
#define BSP_STM32_MPU6050_RA_I2C_SLV4_DI      0x35
#define BSP_STM32_MPU6050_RA_I2C_MST_STATUS   0x36
#define BSP_STM32_MPU6050_RA_INT_PIN_CFG      0x37
#define BSP_STM32_MPU6050_RA_INT_ENABLE       0x38
#define BSP_STM32_MPU6050_RA_DMP_INT_STATUS   0x39
#define BSP_STM32_MPU6050_RA_INT_STATUS       0x3A
#define BSP_STM32_MPU6050_RA_ACCEL_XOUT_H     0x3B
#define BSP_STM32_MPU6050_RA_ACCEL_XOUT_L     0x3C
#define BSP_STM32_MPU6050_RA_ACCEL_YOUT_H     0x3D
#define BSP_STM32_MPU6050_RA_ACCEL_YOUT_L     0x3E
#define BSP_STM32_MPU6050_RA_ACCEL_ZOUT_H     0x3F
#define BSP_STM32_MPU6050_RA_ACCEL_ZOUT_L     0x40
#define BSP_STM32_MPU6050_RA_TEMP_OUT_H       0x41
#define BSP_STM32_MPU6050_RA_TEMP_OUT_L       0x42
#define BSP_STM32_MPU6050_RA_GYRO_XOUT_H      0x43
#define BSP_STM32_MPU6050_RA_GYRO_XOUT_L      0x44
#define BSP_STM32_MPU6050_RA_GYRO_YOUT_H      0x45
#define BSP_STM32_MPU6050_RA_GYRO_YOUT_L      0x46
#define BSP_STM32_MPU6050_RA_GYRO_ZOUT_H      0x47
#define BSP_STM32_MPU6050_RA_GYRO_ZOUT_L      0x48
#define BSP_STM32_MPU6050_RA_EXT_SENS_DATA_00 0x49
#define BSP_STM32_MPU6050_RA_EXT_SENS_DATA_01 0x4A
#define BSP_STM32_MPU6050_RA_EXT_SENS_DATA_02 0x4B
#define BSP_STM32_MPU6050_RA_EXT_SENS_DATA_03 0x4C
#define BSP_STM32_MPU6050_RA_EXT_SENS_DATA_04 0x4D
#define BSP_STM32_MPU6050_RA_EXT_SENS_DATA_05 0x4E
#define BSP_STM32_MPU6050_RA_EXT_SENS_DATA_06 0x4F
#define BSP_STM32_MPU6050_RA_EXT_SENS_DATA_07 0x50
#define BSP_STM32_MPU6050_RA_EXT_SENS_DATA_08 0x51
#define BSP_STM32_MPU6050_RA_EXT_SENS_DATA_09 0x52
#define BSP_STM32_MPU6050_RA_EXT_SENS_DATA_10 0x53
#define BSP_STM32_MPU6050_RA_EXT_SENS_DATA_11 0x54
#define BSP_STM32_MPU6050_RA_EXT_SENS_DATA_12 0x55
#define BSP_STM32_MPU6050_RA_EXT_SENS_DATA_13 0x56
#define BSP_STM32_MPU6050_RA_EXT_SENS_DATA_14 0x57
#define BSP_STM32_MPU6050_RA_EXT_SENS_DATA_15 0x58
#define BSP_STM32_MPU6050_RA_EXT_SENS_DATA_16 0x59
#define BSP_STM32_MPU6050_RA_EXT_SENS_DATA_17 0x5A
#define BSP_STM32_MPU6050_RA_EXT_SENS_DATA_18 0x5B
#define BSP_STM32_MPU6050_RA_EXT_SENS_DATA_19 0x5C
#define BSP_STM32_MPU6050_RA_EXT_SENS_DATA_20 0x5D
#define BSP_STM32_MPU6050_RA_EXT_SENS_DATA_21 0x5E
#define BSP_STM32_MPU6050_RA_EXT_SENS_DATA_22 0x5F
#define BSP_STM32_MPU6050_RA_EXT_SENS_DATA_23 0x60
#define BSP_STM32_MPU6050_RA_MOT_DETECT_STATUS 0x61
#define BSP_STM32_MPU6050_RA_I2C_SLV0_DO      0x63
#define BSP_STM32_MPU6050_RA_I2C_SLV1_DO      0x64
#define BSP_STM32_MPU6050_RA_I2C_SLV2_DO      0x65
#define BSP_STM32_MPU6050_RA_I2C_SLV3_DO      0x66
#define BSP_STM32_MPU6050_RA_I2C_MST_DELAY_CTRL   0x67
#define BSP_STM32_MPU6050_RA_SIGNAL_PATH_RESET    0x68
#define BSP_STM32_MPU6050_RA_MOT_DETECT_CTRL      0x69
#define BSP_STM32_MPU6050_RA_USER_CTRL        0x6A
#define BSP_STM32_MPU6050_RA_PWR_MGMT_1       0x6B
#define BSP_STM32_MPU6050_RA_PWR_MGMT_2       0x6C
#define BSP_STM32_MPU6050_RA_BANK_SEL         0x6D
#define BSP_STM32_MPU6050_RA_MEM_START_ADDR   0x6E
#define BSP_STM32_MPU6050_RA_MEM_R_W          0x6F
#define BSP_STM32_MPU6050_RA_DMP_CFG_1        0x70
#define BSP_STM32_MPU6050_RA_DMP_CFG_2        0x71
#define BSP_STM32_MPU6050_RA_FIFO_COUNTH      0x72
#define BSP_STM32_MPU6050_RA_FIFO_COUNTL      0x73
#define BSP_STM32_MPU6050_RA_FIFO_R_W         0x74
#define BSP_STM32_MPU6050_RA_WHO_AM_I         0x75
#define BSP_STM32_MPU6050_TC_PWR_MODE_BIT     7
#define BSP_STM32_MPU6050_TC_OFFSET_BIT       6
#define BSP_STM32_MPU6050_TC_OFFSET_LENGTH    6
#define BSP_STM32_MPU6050_TC_OTP_BNK_VLD_BIT  0
#define BSP_STM32_MPU6050_VDDIO_LEVEL_VLOGIC  0
#define BSP_STM32_MPU6050_VDDIO_LEVEL_VDD     1
#define BSP_STM32_MPU6050_CFG_EXT_SYNC_SET_BIT    5
#define BSP_STM32_MPU6050_CFG_EXT_SYNC_SET_LENGTH 3
#define BSP_STM32_MPU6050_CFG_DLPF_CFG_BIT    2
#define BSP_STM32_MPU6050_CFG_DLPF_CFG_LENGTH 3
#define BSP_STM32_MPU6050_EXT_SYNC_DISABLED       0x0
#define BSP_STM32_MPU6050_EXT_SYNC_TEMP_OUT_L     0x1
#define BSP_STM32_MPU6050_EXT_SYNC_GYRO_XOUT_L    0x2
#define BSP_STM32_MPU6050_EXT_SYNC_GYRO_YOUT_L    0x3
#define BSP_STM32_MPU6050_EXT_SYNC_GYRO_ZOUT_L    0x4
#define BSP_STM32_MPU6050_EXT_SYNC_ACCEL_XOUT_L   0x5
#define BSP_STM32_MPU6050_EXT_SYNC_ACCEL_YOUT_L   0x6
#define BSP_STM32_MPU6050_EXT_SYNC_ACCEL_ZOUT_L   0x7
#define BSP_STM32_MPU6050_DLPF_BW_256         0x00
#define BSP_STM32_MPU6050_DLPF_BW_188         0x01
#define BSP_STM32_MPU6050_DLPF_BW_98          0x02
#define BSP_STM32_MPU6050_DLPF_BW_42          0x03
#define BSP_STM32_MPU6050_DLPF_BW_20          0x04
#define BSP_STM32_MPU6050_DLPF_BW_10          0x05
#define BSP_STM32_MPU6050_DLPF_BW_5           0x06
#define BSP_STM32_MPU6050_GCONFIG_FS_SEL_BIT      4
#define BSP_STM32_MPU6050_GCONFIG_FS_SEL_LENGTH   2
#define BSP_STM32_MPU6050_GYRO_FS_250         0x00
#define BSP_STM32_MPU6050_GYRO_FS_500         0x01
#define BSP_STM32_MPU6050_GYRO_FS_1000        0x02
#define BSP_STM32_MPU6050_GYRO_FS_2000        0x03
#define BSP_STM32_MPU6050_ACONFIG_XA_ST_BIT           7
#define BSP_STM32_MPU6050_ACONFIG_YA_ST_BIT           6
#define BSP_STM32_MPU6050_ACONFIG_ZA_ST_BIT           5
#define BSP_STM32_MPU6050_ACONFIG_AFS_SEL_BIT         4
#define BSP_STM32_MPU6050_ACONFIG_AFS_SEL_LENGTH      2
#define BSP_STM32_MPU6050_ACONFIG_ACCEL_HPF_BIT       2
#define BSP_STM32_MPU6050_ACONFIG_ACCEL_HPF_LENGTH    3
#define BSP_STM32_MPU6050_ACCEL_FS_2          0x00
#define BSP_STM32_MPU6050_ACCEL_FS_4          0x01
#define BSP_STM32_MPU6050_ACCEL_FS_8          0x02
#define BSP_STM32_MPU6050_ACCEL_FS_16         0x03
#define BSP_STM32_MPU6050_DHPF_RESET          0x00
#define BSP_STM32_MPU6050_DHPF_5              0x01
#define BSP_STM32_MPU6050_DHPF_2P5            0x02
#define BSP_STM32_MPU6050_DHPF_1P25           0x03
#define BSP_STM32_MPU6050_DHPF_0P63           0x04
#define BSP_STM32_MPU6050_DHPF_HOLD           0x07
#define BSP_STM32_MPU6050_TEMP_FIFO_EN_BIT    7
#define BSP_STM32_MPU6050_XG_FIFO_EN_BIT      6
#define BSP_STM32_MPU6050_YG_FIFO_EN_BIT      5
#define BSP_STM32_MPU6050_ZG_FIFO_EN_BIT      4
#define BSP_STM32_MPU6050_ACCEL_FIFO_EN_BIT   3
#define BSP_STM32_MPU6050_SLV2_FIFO_EN_BIT    2
#define BSP_STM32_MPU6050_SLV1_FIFO_EN_BIT    1
#define BSP_STM32_MPU6050_SLV0_FIFO_EN_BIT    0
#define BSP_STM32_MPU6050_MULT_MST_EN_BIT     7
#define BSP_STM32_MPU6050_WAIT_FOR_ES_BIT     6
#define BSP_STM32_MPU6050_SLV_3_FIFO_EN_BIT   5
#define BSP_STM32_MPU6050_I2C_MST_P_NSR_BIT   4
#define BSP_STM32_MPU6050_I2C_MST_CLK_BIT     3
#define BSP_STM32_MPU6050_I2C_MST_CLK_LENGTH  4
#define BSP_STM32_MPU6050_CLOCK_DIV_348       0x0
#define BSP_STM32_MPU6050_CLOCK_DIV_333       0x1
#define BSP_STM32_MPU6050_CLOCK_DIV_320       0x2
#define BSP_STM32_MPU6050_CLOCK_DIV_308       0x3
#define BSP_STM32_MPU6050_CLOCK_DIV_296       0x4
#define BSP_STM32_MPU6050_CLOCK_DIV_286       0x5
#define BSP_STM32_MPU6050_CLOCK_DIV_276       0x6
#define BSP_STM32_MPU6050_CLOCK_DIV_267       0x7
#define BSP_STM32_MPU6050_CLOCK_DIV_258       0x8
#define BSP_STM32_MPU6050_CLOCK_DIV_500       0x9
#define BSP_STM32_MPU6050_CLOCK_DIV_471       0xA
#define BSP_STM32_MPU6050_CLOCK_DIV_444       0xB
#define BSP_STM32_MPU6050_CLOCK_DIV_421       0xC
#define BSP_STM32_MPU6050_CLOCK_DIV_400       0xD
#define BSP_STM32_MPU6050_CLOCK_DIV_381       0xE
#define BSP_STM32_MPU6050_CLOCK_DIV_364       0xF
#define BSP_STM32_MPU6050_I2C_SLV_RW_BIT      7
#define BSP_STM32_MPU6050_I2C_SLV_ADDR_BIT    6
#define BSP_STM32_MPU6050_I2C_SLV_ADDR_LENGTH 7
#define BSP_STM32_MPU6050_I2C_SLV_EN_BIT      7
#define BSP_STM32_MPU6050_I2C_SLV_BYTE_SW_BIT 6
#define BSP_STM32_MPU6050_I2C_SLV_REG_DIS_BIT 5
#define BSP_STM32_MPU6050_I2C_SLV_GRP_BIT     4
#define BSP_STM32_MPU6050_I2C_SLV_LEN_BIT     3
#define BSP_STM32_MPU6050_I2C_SLV_LEN_LENGTH  4
#define BSP_STM32_MPU6050_I2C_SLV4_RW_BIT         7
#define BSP_STM32_MPU6050_I2C_SLV4_ADDR_BIT       6
#define BSP_STM32_MPU6050_I2C_SLV4_ADDR_LENGTH    7
#define BSP_STM32_MPU6050_I2C_SLV4_EN_BIT         7
#define BSP_STM32_MPU6050_I2C_SLV4_INT_EN_BIT     6
#define BSP_STM32_MPU6050_I2C_SLV4_REG_DIS_BIT    5
#define BSP_STM32_MPU6050_I2C_SLV4_MST_DLY_BIT    4
#define BSP_STM32_MPU6050_I2C_SLV4_MST_DLY_LENGTH 5
#define BSP_STM32_MPU6050_MST_PASS_THROUGH_BIT    7
#define BSP_STM32_MPU6050_MST_I2C_SLV4_DONE_BIT   6
#define BSP_STM32_MPU6050_MST_I2C_LOST_ARB_BIT    5
#define BSP_STM32_MPU6050_MST_I2C_SLV4_NACK_BIT   4
#define BSP_STM32_MPU6050_MST_I2C_SLV3_NACK_BIT   3
#define BSP_STM32_MPU6050_MST_I2C_SLV2_NACK_BIT   2
#define BSP_STM32_MPU6050_MST_I2C_SLV1_NACK_BIT   1
#define BSP_STM32_MPU6050_MST_I2C_SLV0_NACK_BIT   0
#define BSP_STM32_MPU6050_INTCFG_INT_LEVEL_BIT        7
#define BSP_STM32_MPU6050_INTCFG_INT_OPEN_BIT         6
#define BSP_STM32_MPU6050_INTCFG_LATCH_INT_EN_BIT     5
#define BSP_STM32_MPU6050_INTCFG_INT_RD_CLEAR_BIT     4
#define BSP_STM32_MPU6050_INTCFG_FSYNC_INT_LEVEL_BIT  3
#define BSP_STM32_MPU6050_INTCFG_FSYNC_INT_EN_BIT     2
#define BSP_STM32_MPU6050_INTCFG_I2C_BYPASS_EN_BIT    1
#define BSP_STM32_MPU6050_INTCFG_CLKOUT_EN_BIT        0
#define BSP_STM32_MPU6050_INTMODE_ACTIVEHIGH  0x00
#define BSP_STM32_MPU6050_INTMODE_ACTIVELOW   0x01
#define BSP_STM32_MPU6050_INTDRV_PUSHPULL     0x00
#define BSP_STM32_MPU6050_INTDRV_OPENDRAIN    0x01
#define BSP_STM32_MPU6050_INTLATCH_50USPULSE  0x00
#define BSP_STM32_MPU6050_INTLATCH_WAITCLEAR  0x01
#define BSP_STM32_MPU6050_INTCLEAR_STATUSREAD 0x00
#define BSP_STM32_MPU6050_INTCLEAR_ANYREAD    0x01
#define BSP_STM32_MPU6050_INTERRUPT_FF_BIT            7
#define BSP_STM32_MPU6050_INTERRUPT_MOT_BIT           6
#define BSP_STM32_MPU6050_INTERRUPT_ZMOT_BIT          5
#define BSP_STM32_MPU6050_INTERRUPT_FIFO_OFLOW_BIT    4
#define BSP_STM32_MPU6050_INTERRUPT_I2C_MST_INT_BIT   3
#define BSP_STM32_MPU6050_INTERRUPT_PLL_RDY_INT_BIT   2
#define BSP_STM32_MPU6050_INTERRUPT_DMP_INT_BIT       1
#define BSP_STM32_MPU6050_INTERRUPT_DATA_RDY_BIT      0
#define BSP_STM32_MPU6050_DMPINT_5_BIT            5
#define BSP_STM32_MPU6050_DMPINT_4_BIT            4
#define BSP_STM32_MPU6050_DMPINT_3_BIT            3
#define BSP_STM32_MPU6050_DMPINT_2_BIT            2
#define BSP_STM32_MPU6050_DMPINT_1_BIT            1
#define BSP_STM32_MPU6050_DMPINT_0_BIT            0
#define BSP_STM32_MPU6050_MOTION_MOT_XNEG_BIT     7
#define BSP_STM32_MPU6050_MOTION_MOT_XPOS_BIT     6
#define BSP_STM32_MPU6050_MOTION_MOT_YNEG_BIT     5
#define BSP_STM32_MPU6050_MOTION_MOT_YPOS_BIT     4
#define BSP_STM32_MPU6050_MOTION_MOT_ZNEG_BIT     3
#define BSP_STM32_MPU6050_MOTION_MOT_ZPOS_BIT     2
#define BSP_STM32_MPU6050_MOTION_MOT_ZRMOT_BIT    0
#define BSP_STM32_MPU6050_DELAYCTRL_DELAY_ES_SHADOW_BIT   7
#define BSP_STM32_MPU6050_DELAYCTRL_I2C_SLV4_DLY_EN_BIT   4
#define BSP_STM32_MPU6050_DELAYCTRL_I2C_SLV3_DLY_EN_BIT   3
#define BSP_STM32_MPU6050_DELAYCTRL_I2C_SLV2_DLY_EN_BIT   2
#define BSP_STM32_MPU6050_DELAYCTRL_I2C_SLV1_DLY_EN_BIT   1
#define BSP_STM32_MPU6050_DELAYCTRL_I2C_SLV0_DLY_EN_BIT   0
#define BSP_STM32_MPU6050_PATHRESET_GYRO_RESET_BIT    2
#define BSP_STM32_MPU6050_PATHRESET_ACCEL_RESET_BIT   1
#define BSP_STM32_MPU6050_PATHRESET_TEMP_RESET_BIT    0
#define BSP_STM32_MPU6050_DETECT_ACCEL_ON_DELAY_BIT       5
#define BSP_STM32_MPU6050_DETECT_ACCEL_ON_DELAY_LENGTH    2
#define BSP_STM32_MPU6050_DETECT_FF_COUNT_BIT             3
#define BSP_STM32_MPU6050_DETECT_FF_COUNT_LENGTH          2
#define BSP_STM32_MPU6050_DETECT_MOT_COUNT_BIT            1
#define BSP_STM32_MPU6050_DETECT_MOT_COUNT_LENGTH         2
#define BSP_STM32_MPU6050_DETECT_DECREMENT_RESET  0x0
#define BSP_STM32_MPU6050_DETECT_DECREMENT_1      0x1
#define BSP_STM32_MPU6050_DETECT_DECREMENT_2      0x2
#define BSP_STM32_MPU6050_DETECT_DECREMENT_4      0x3
#define BSP_STM32_MPU6050_USERCTRL_DMP_EN_BIT             7
#define BSP_STM32_MPU6050_USERCTRL_FIFO_EN_BIT            6
#define BSP_STM32_MPU6050_USERCTRL_I2C_MST_EN_BIT         5
#define BSP_STM32_MPU6050_USERCTRL_I2C_IF_DIS_BIT         4
#define BSP_STM32_MPU6050_USERCTRL_DMP_RESET_BIT          3
#define BSP_STM32_MPU6050_USERCTRL_FIFO_RESET_BIT         2
#define BSP_STM32_MPU6050_USERCTRL_I2C_MST_RESET_BIT      1
#define BSP_STM32_MPU6050_USERCTRL_SIG_COND_RESET_BIT     0
#define BSP_STM32_MPU6050_PWR1_DEVICE_RESET_BIT   7
#define BSP_STM32_MPU6050_PWR1_SLEEP_BIT          6
#define BSP_STM32_MPU6050_PWR1_CYCLE_BIT          5
#define BSP_STM32_MPU6050_PWR1_TEMP_DIS_BIT       3
#define BSP_STM32_MPU6050_PWR1_CLKSEL_BIT         2
#define BSP_STM32_MPU6050_PWR1_CLKSEL_LENGTH      3
#define BSP_STM32_MPU6050_CLOCK_INTERNAL          0x00
#define BSP_STM32_MPU6050_CLOCK_PLL_XGYRO         0x01
#define BSP_STM32_MPU6050_CLOCK_PLL_YGYRO         0x02
#define BSP_STM32_MPU6050_CLOCK_PLL_ZGYRO         0x03
#define BSP_STM32_MPU6050_CLOCK_PLL_EXT32K        0x04
#define BSP_STM32_MPU6050_CLOCK_PLL_EXT19M        0x05
#define BSP_STM32_MPU6050_CLOCK_KEEP_RESET        0x07
#define BSP_STM32_MPU6050_PWR2_LP_WAKE_CTRL_BIT       7
#define BSP_STM32_MPU6050_PWR2_LP_WAKE_CTRL_LENGTH    2
#define BSP_STM32_MPU6050_PWR2_STBY_XA_BIT            5
#define BSP_STM32_MPU6050_PWR2_STBY_YA_BIT            4
#define BSP_STM32_MPU6050_PWR2_STBY_ZA_BIT            3
#define BSP_STM32_MPU6050_PWR2_STBY_XG_BIT            2
#define BSP_STM32_MPU6050_PWR2_STBY_YG_BIT            1
#define BSP_STM32_MPU6050_PWR2_STBY_ZG_BIT            0
#define BSP_STM32_MPU6050_WAKE_FREQ_1P25      0x0
#define BSP_STM32_MPU6050_WAKE_FREQ_2P5       0x1
#define BSP_STM32_MPU6050_WAKE_FREQ_5         0x2
#define BSP_STM32_MPU6050_WAKE_FREQ_10        0x3
#define BSP_STM32_MPU6050_BANKSEL_PRFTCH_EN_BIT       6
#define BSP_STM32_MPU6050_BANKSEL_CFG_USER_BANK_BIT   5
#define BSP_STM32_MPU6050_BANKSEL_MEM_SEL_BIT         4
#define BSP_STM32_MPU6050_BANKSEL_MEM_SEL_LENGTH      5
#define BSP_STM32_MPU6050_WHO_AM_I_BIT        6
#define BSP_STM32_MPU6050_WHO_AM_I_LENGTH     6
#define BSP_STM32_MPU6050_DMP_MEMORY_BANKS        8
#define BSP_STM32_MPU6050_DMP_MEMORY_BANK_SIZE    256
#define BSP_STM32_MPU6050_DMP_MEMORY_CHUNK_SIZE   16	

#ifdef __cplusplus
}
#endif
#endif  /* __BSP_STM32_I2C_H */


