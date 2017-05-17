/*
 * commsgbfsc.h
 *
 *  Created on: 2016年1月3日
 *      Author: test
 */

#ifndef L0COMVM_COMMSGBFSC_H_
#define L0COMVM_COMMSGBFSC_H_

#include "comtype.h"
#include "sysdim.h"
#include "sysengpar.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <assert.h>
#include <signal.h>
#include <ctype.h>
#include <limits.h>
#include <locale.h>
#include <stdbool.h>
//#include <system.h>
//#include <sys/ipc.h>


//遗留特殊的定义
typedef UINT8           		BOOL;  

//2. 公共消息结构体定义
//Under normal case, 1024Byte shall be enough for internal message communciation purpose.
//If not enough, need modify here to enlarge
#define IHU_SYSMSG_COM_BODY_LEN_MAX 		IHU_SYSDIM_MSG_BODY_LEN_MAX+6
#define IHU_SYSMSG_BH_BUF_BODY_LEN_MAX 	IHU_SYSDIM_MSG_BODY_LEN_MAX-6
typedef struct IhuMsgSruct
{
	UINT16 msgType;
	UINT8 dest_id;
	UINT8 src_id;
	UINT16 msgLen;
	INT8 msgBody[IHU_SYSDIM_MSG_BODY_LEN_MAX];
}IhuMsgSruct_t;

//所有任务模块的优先级是一样的
#define  IHU_SYSMSG_TASK_THREAD_PRIO  10          //priority of the main loop de 1 a 99 max

/*
 *
 * 每定义一个新消息，请去修改vmlayer.c中的变量zIhuMsgNameList[]，不然TRACE会出现消息内容解析的错误
 *
 */

//4. 新消息新程序结构体定义部分
//复用下位机的消息定义
#define MSG_ID_ENTRY 0
enum IHU_INTER_TASK_MSG_ID
{
	//START FLAG
	MSG_ID_COM_MIN = 1, //Starting point
	
	//正常的消息ID
	MSG_ID_COM_INIT,
	MSG_ID_COM_INIT_FB,
	MSG_ID_COM_RESTART,  //L1->重新初始化上下文数据
	MSG_ID_COM_TIME_OUT,
	MSG_ID_COM_STOP,
	MSG_ID_COM_HEART_BEAT,
	MSG_ID_COM_HEART_BEAT_FB,
	
	//VMFO
	MSG_ID_VMFO_TIMER_1S_PERIOD,
	
	//ADC
	MSG_ID_ADC_NEW_MATERIAL_WS,
	MSG_ID_ADC_MATERIAL_DROP,
	MSG_ID_ADC_L3BFSC_MEAS_CMD_RESP,

	//LED
	
	//UART
	MSG_ID_SPS_L2FRAME_SEND,
	MSG_ID_SPS_L2FRAME_RCV,
	
	//SPI
	MSG_ID_SPI_L2FRAME_SEND,
	MSG_ID_SPI_L2FRAME_RCV,
		
	//I2C
	MSG_ID_I2C_L2FRAME_SEND,
	MSG_ID_I2C_L2FRAME_RCV,
	MSG_ID_I2C_L3BFSC_MOTO_CMD_RESP,
	
	//CAN
	MSG_ID_CAN_L2FRAME_SEND,
	MSG_ID_CAN_L2FRAME_RCV,
	MSG_ID_CAN_L3BFSC_CMD_CTRL,
	MSG_ID_CAN_L3BFSC_INIT_REQ,
	MSG_ID_CAN_L3BFSC_ROLL_OUT_REQ,
	MSG_ID_CAN_L3BFSC_GIVE_UP_REQ,
	MSG_ID_CAN_ADC_WS_MAN_SET_ZERO,   //这个是需要在任何状态下都支持的，所以需要在初始化之外穿越到目标

	//L3BFSC
	MSG_ID_L3BFSC_CAN_INIT_RESP,
	MSG_ID_L3BFSC_CAN_NEW_WS_EVENT,
	MSG_ID_L3BFSC_CAN_ROLL_OUT_RESP,
	MSG_ID_L3BFSC_CAN_GIVE_UP_RESP,
	MSG_ID_L3BFSC_CAN_ERROR_STATUS_REPORT,
	MSG_ID_L3BFSC_ADC_WS_CMD_CTRL,
	MSG_ID_L3BFSC_I2C_MOTO_CMD_CTRL,
	MSG_ID_L3BFSC_CAN_CMD_RESP,
	
	//WMC <-> AWS //MYC
	MSG_ID_L3BFSC_WMC_STARTUP_IND,
	MSG_ID_L3BFSC_WMC_SET_CONFIG_REQ,
	MSG_ID_L3BFSC_WMC_SET_CONFIG_RESP,
	MSG_ID_L3BFSC_WMC_GET_CONFIG_REQ,
	MSG_ID_L3BFSC_WMC_GET_CONFIG_RESP,
	MSG_ID_L3BFSC_WMC_START_REQ,
	MSG_ID_L3BFSC_WMC_START_RESP,
	MSG_ID_L3BFSC_WMC_STOP_REQ,
	MSG_ID_L3BFSC_WMC_STOP_RESP,
	MSG_ID_L3BFSC_WMC_WEIGHT_IND,
	MSG_ID_L3BFSC_WMC_COMBIN_REQ,
	MSG_ID_L3BFSC_WMC_COMBIN_RESP,
	MSG_ID_L3BFSC_WMC_FAULT_IND,
	MSG_ID_L3BFSC_WMC_COMMAND_REQ,
	MSG_ID_L3BFSC_WMC_COMMAND_RESP,
	
	//END FLAG
	MSG_ID_COM_MAX, //Ending point

}; //end of IHU_INTER_TASK_MSG_ID
#define MSG_ID_END 0xFF  //跟MASK_MSGID_NUM_IN_ONE_TASK设置息息相关，不能随便改动
#define MSG_ID_INVALID 0xFFFFFFFF

//公共结构定义
typedef struct com_gps_pos //
{
	UINT32 gpsx;
	UINT32 gpsy;
	UINT32 gpsz;
}com_gps_pos_t;

typedef struct com_ws_and_moto_cmd //
{
	UINT8 prefixcmdid;
	UINT8 optid;
	UINT8 optpar;
	INT32 modbusVal;
}com_ws_and_moto_cmd_t;

//公共消息定义
//MSG_ID_COM_INIT
typedef struct msg_struct_com_init //
{
	UINT16 length;
}msg_struct_com_init_t;

//MSG_ID_COM_INIT_FB
typedef struct msg_struct_com_init_fb //
{
	UINT16 length;
}msg_struct_com_init_fb_t;

//MSG_ID_COM_RESTART
typedef struct  msg_struct_com_restart //
{
	UINT16 length;
}msg_struct_com_restart_t;

//MSG_ID_COM_TIME_OUT
typedef struct  msg_struct_com_time_out //
{
	UINT32 timeId;
	UINT8  timeRes;
	UINT16 length;
}msg_struct_com_time_out_t;

//MSG_ID_COM_STOP
typedef struct  msg_struct_com_stop //
{
	UINT16 length;
}msg_struct_com_stop_t;

//MSG_ID_COM_HEART_BEAT
typedef struct  msg_struct_com_heart_beat //
{
	UINT16 length;
}msg_struct_com_heart_beat_t;

//MSG_ID_COM_HEART_BEAT_FB
typedef struct  msg_struct_com_heart_beat_fb //
{
	UINT16 length;
}msg_struct_com_heart_beat_fb_t;

//VMFO消息定义
typedef struct msg_struct_vmfo_1s_period_timtout
{
	UINT16 length;
}msg_struct_vmfo_1s_period_timtout_t;

//ADC
//MSG_ID_ADC_L3BFSC_MEAS_CMD_RESP
typedef struct msg_struct_adclibra_l3bfsc_meas_cmd_resp
{
	UINT8 cmdid;
	com_ws_and_moto_cmd_t cmd;
	UINT16 length;
}msg_struct_adclibra_l3bfsc_meas_cmd_resp_t;
#define IHU_SYSMSG_ADC_BFSC_WS_CMD_TYPE_NONE 0
#define IHU_SYSMSG_ADC_BFSC_WS_CMD_TYPE_START_CFM 1
#define IHU_SYSMSG_ADC_BFSC_WS_CMD_TYPE_STOP_CFM 2
#define IHU_SYSMSG_ADC_BFSC_WS_CMD_TYPE_RESP 3

//MSG_ID_ADC_NEW_MATERIAL_WS,
typedef struct msg_struct_adc_new_material_ws
{
	INT32 wsValue;
	UINT16 length;
}msg_struct_adc_new_material_ws_t;

//MSG_ID_ADC_MATERIAL_DROP,
typedef struct msg_struct_adc_material_drop
{
	UINT16 length;
}msg_struct_adc_material_drop_t;


//UART消息定义
typedef struct msg_struct_spsvirgo_l2frame_send
{
	UINT8 data[IHU_SYSDIM_L2FRAME_MSG_BODY_LEN_MAX];
	UINT16 length;
}msg_struct_spsvirgo_l2frame_send_t;
typedef struct msg_struct_spsvirgo_l2frame_rcv
{
	UINT8 data[IHU_SYSDIM_L2FRAME_MSG_BODY_LEN_MAX];
	UINT16 length;
}msg_struct_spsvirgo_l2frame_rcv_t;

//SPI消息定义
typedef struct msg_struct_spileo_l2frame_send
{
	UINT8 data[IHU_SYSDIM_L2FRAME_MSG_BODY_LEN_MAX];
	UINT16 length;
}msg_struct_spileo_l2frame_send_t;
typedef struct msg_struct_spileo_l2frame_rcv
{
	UINT8 data[IHU_SYSDIM_L2FRAME_MSG_BODY_LEN_MAX];
	UINT16 length;
}msg_struct_spileo_l2frame_rcv_t;

//I2C消息定义
//MSG_ID_I2C_L2FRAME_SEND
typedef struct msg_struct_i2caries_l2frame_send
{
	UINT8 data[IHU_SYSDIM_L2FRAME_MSG_BODY_LEN_MAX];	
	UINT16 length;
}msg_struct_i2caries_l2frame_send_t;
//MSG_ID_I2C_L2FRAME_RCV
typedef struct msg_struct_i2caries_l2frame_rcv
{
	UINT8 data[IHU_SYSDIM_L2FRAME_MSG_BODY_LEN_MAX];
	UINT16 length;
}msg_struct_i2caries_l2frame_rcv_t;

//MSG_ID_I2C_L3BFSC_MOTO_CMD_RESP
typedef struct msg_struct_i2caries_l3bfsc_cmd_resp
{
	UINT8 cmdid;
	com_ws_and_moto_cmd_t cmd;	
	UINT16 length;
}msg_struct_i2caries_l3bfsc_cmd_resp_t;
#define IHU_SYSMSG_I2C_BFSC_WS_CMD_TYPE_NONE 0
#define IHU_SYSMSG_I2C_BFSC_WS_CMD_TYPE_START_CFM 1
#define IHU_SYSMSG_I2C_BFSC_WS_CMD_TYPE_STOP_CFM 2
#define IHU_SYSMSG_I2C_BFSC_WS_CMD_TYPE_RESP 3

//CAN消息定义
//MSG_ID_CAN_L2FRAME_SEND
typedef struct msg_struct_canvela_l2frame_send
{
	UINT8 data[IHU_SYSDIM_L2FRAME_MSG_BODY_LEN_MAX];
	UINT16 length;
}msg_struct_canvela_l2frame_send_t;

//MSG_ID_CAN_L2FRAME_RCV
typedef struct msg_struct_canvela_l2frame_rcv
{
	UINT8 data[IHU_SYSDIM_L2FRAME_MSG_BODY_LEN_MAX];
	UINT16 length;
}msg_struct_canvela_l2frame_rcv_t;

//MSG_ID_CAN_L3BFSC_CMD_CTRL
typedef struct msg_struct_canvela_l3bfsc_cmd_ctrl
{
	com_ws_and_moto_cmd_t cmd;	
	UINT16 length;
}msg_struct_canvela_l3bfsc_cmd_ctrl_t;

//MSG_ID_CAN_L3BFSC_INIT_REQ
typedef struct msg_struct_canvela_l3bfsc_init_req
{
	UINT16 length;
}msg_struct_canvela_l3bfsc_init_req_t;

//MSG_ID_CAN_L3BFSC_ROLL_OUT_REQ,
typedef struct msg_struct_canvela_l3bfsc_roll_out_req
{
	UINT16 length;
}msg_struct_canvela_l3bfsc_roll_out_req_t;

//MSG_ID_CAN_L3BFSC_GIVE_UP_REQ,
typedef struct msg_struct_canvela_l3bfsc_give_up_req
{
	UINT16 length;
}msg_struct_canvela_l3bfsc_give_up_req_t;

//MSG_ID_CAN_ADC_WS_MAN_SET_ZERO
typedef struct msg_struct_canvela_adc_ws_man_set_zero
{
	UINT16 length;
}msg_struct_canvela_adc_ws_man_set_zero_t;

//L3BFSC消息
//MSG_ID_L3BFSC_CAN_INIT_RESP
typedef struct msg_struct_l3bfsc_canvela_init_resp
{
	UINT16 length;
}msg_struct_l3bfsc_canvela_init_resp_t;

//MSG_ID_L3BFSC_CAN_NEW_WS_EVENT
typedef struct msg_struct_l3bfsc_canvela_new_ws_event
{
	INT32 wsValue;
	UINT16 length;
}msg_struct_l3bfsc_canvela_new_ws_event_t;

//MSG_ID_L3BFSC_CAN_ROLL_OUT_RESP,
typedef struct msg_struct_l3bfsc_canvela_roll_out_resp
{
	UINT16 length;
}msg_struct_l3bfsc_canvela_roll_out_resp_t;

//MSG_ID_L3BFSC_CAN_GIVE_UP_RESP,
typedef struct msg_struct_l3bfsc_canvela_give_up_resp
{
	UINT16 length;
}msg_struct_l3bfsc_canvela_give_up_resp_t;

//MSG_ID_L3BFSC_CAN_ERROR_STATUS_REPORT
typedef struct msg_struct_l3bfsc_canvela_error_status_report
{
	UINT32 errId;
	UINT32 errCause;
	UINT32 errSource;
	UINT16 length;
}msg_struct_l3bfsc_canvela_error_status_report_t;

//MSG_ID_L3BFSC_ADC_WS_CMD_CTRL
typedef struct msg_struct_l3bfsc_adc_ws_cmd_ctrl
{
	UINT8 cmdid;
	com_ws_and_moto_cmd_t cmd;
	UINT16 length;
}msg_struct_l3bfsc_adc_ws_cmd_ctrl_t;
#define IHU_SYSMSG_BFSC_ADC_WS_CMD_TYPE_NONE 0
#define IHU_SYSMSG_BFSC_ADC_WS_CMD_TYPE_START 1
#define IHU_SYSMSG_BFSC_ADC_WS_CMD_TYPE_STOP 2
#define IHU_SYSMSG_BFSC_ADC_WS_CMD_TYPE_CTRL 3

//MSG_ID_L3BFSC_I2C_MOTO_CMD_CTRL
typedef struct msg_struct_l3bfsc_i2c_moto_cmd_ctrl
{
	UINT8 cmdid;
	com_ws_and_moto_cmd_t cmd;	
	UINT16 length;
}msg_struct_l3bfsc_i2c_moto_cmd_ctrl_t;
#define IHU_SYSMSG_BFSC_I2C_MOTO_CMD_TYPE_NONE 0
#define IHU_SYSMSG_BFSC_I2C_MOTO_CMD_TYPE_START 1
#define IHU_SYSMSG_BFSC_I2C_MOTO_CMD_TYPE_STOP 2
#define IHU_SYSMSG_BFSC_I2C_MOTO_CMD_TYPE_CTRL 3

//MSG_ID_L3BFSC_CAN_CMD_RESP
typedef struct msg_struct_l3bfsc_canvela_cmd_resp
{
	UINT8 cmdid;
	com_ws_and_moto_cmd_t cmd;	
	UINT16 length;
}msg_struct_l3bfsc_canvela_cmd_resp_t;
#define IHU_SYSMSG_BFSC_CAN_CMD_TYPE_NONE 0
#define IHU_SYSMSG_BFSC_CAN_CMD_TYPE_START_CFM 1
#define IHU_SYSMSG_BFSC_CAN_CMD_TYPE_STOP_CFM 2
#define IHU_SYSMSG_BFSC_CAN_CMD_TYPE_RESP 3

/* 
** ====================================================================
** ============================ MYC START =============================
** ====================================================================
*/

/*
** 	//WMC <-> AWS
**	MSG_ID_L3BFSC_WMC_STARTUP_IND,
**	MSG_ID_L3BFSC_WMC_SET_CONFIG_REQ,
**	MSG_ID_L3BFSC_WMC_SET_CONFIG_RESP,
**	MSG_ID_L3BFSC_WMC_GET_CONFIG_REQ,
**	MSG_ID_L3BFSC_WMC_GET_CONFIG_RESP,
**	MSG_ID_L3BFSC_WMC_START_REQ,
**	MSG_ID_L3BFSC_WMC_START_RESP,
**	MSG_ID_L3BFSC_WMC_STOP_REQ,
**	MSG_ID_L3BFSC_WMC_STOP_RESP,
**	MSG_ID_L3BFSC_WMC_WEIGHT_IND,
**	MSG_ID_L3BFSC_WMC_COMBIN_REQ,
**	MSG_ID_L3BFSC_WMC_COMBIN_RESP,
**	MSG_ID_L3BFSC_WMC_FAULT_IND,
**	MSG_ID_L3BFSC_WMC_COMMAND_REQ,
**	MSG_ID_L3BFSC_WMC_COMMAND_RESP,
*/

//	//HCU-IHU SUI新增内容
//	//上电过程
//	HUITP_MSGID_sui_bfsc_startup_ind                 = 0x3B90,
//	//配置过程
//	HUITP_MSGID_sui_bfsc_set_config_req              = 0x3B11,
//	HUITP_MSGID_sui_bfsc_set_config_resp             = 0x3B91,
//	//启动过程
//	HUITP_MSGID_sui_bfsc_start_req                   = 0x3B12,
//	HUITP_MSGID_sui_bfsc_start_resp                  = 0x3B92,
//	HUITP_MSGID_sui_bfsc_stop_req                    = 0x3B13,
//	HUITP_MSGID_sui_bfsc_stop_resp                   = 0x3B93,
//	//重量汇报过程
//	HUITP_MSGID_sui_bfsc_new_ws_event                = 0x3B94,
//	//HUITP_MSGID_sui_bfsc_repeat_ws_event             = 0x3B94,  //TO BE REMOVE
//	//组合出料过程
//	HUITP_MSGID_sui_bfsc_ws_comb_out_req             = 0x3B15,
//	HUITP_MSGID_sui_bfsc_ws_comb_out_resp            = 0x3B95,
/*
**	Command Response Structure
*/


/*
**	HUITP_MSGID_sui_bfsc_startup_ind
*/
typedef struct WmcErrorCode
{
	UINT16 error_code;           
	UINT16 spare1;               
}WmcErrorCode_t;

typedef struct WmcId
{
	UINT8 wmc_id;               /* 0 ~ 15 is the DIP defined, ID 16 is the main rolling */
	UINT8 spare1;               /* 0 ~ 15 is the DIP defined, ID 16 is the main rolling */
	UINT8 spare2;               /* 0 ~ 15 is the DIP defined, ID 16 is the main rolling */
	UINT8 spare3;               /* 0 ~ 15 is the DIP defined, ID 16 is the main rolling */	
}WmcId_t;

typedef struct msg_struct_l3bfsc_wmc_resp
{
	UINT16	msgid;
	UINT16  length;
	WmcId_t wmc_id;               /* 0 ~ 15 is the DIP defined, ID 16 is the main rolling */
  WmcErrorCode_t result;
}msg_struct_l3bfsc_wmc_resp_t;


typedef struct WmcInventory
{
	UINT32 hw_inventory_id;
	UINT32 sw_inventory_id;
	UINT32 stm32_cpu_id;
	UINT32 weight_sensor_type;
	UINT32 motor_type;
	WmcId_t wmc_id;               /* 0 ~ 15 is the DIP defined, ID 16 is the main rolling */
	UINT32 spare1;
	UINT32 spare2;
}WmcInventory_t;


typedef struct msg_struct_l3bfsc_wmc_startup_ind
{
	UINT16 msgid;
	UINT16 length;
	WmcInventory_t wmc_inventory;
}msg_struct_l3bfsc_wmc_startup_ind_t;

/*
**	MSG_ID_L3BFSC_WMC_SET_CONFIG_REQ
*/
typedef struct CombinationAlgorithmParamaters
{
	UINT32	MinScaleNumberCombination;				//组合搜索的最小Scale的个数
	UINT32	MaxScaleNumberCombination;				//组合搜索的最大Scale的个数
	UINT32	MinScaleNumberStartCombination;		//开始查找的最小个数，就是说大于这个个数就开始搜索
	UINT32	TargetCombinationWeight;				  //组合目标重量
	UINT32	TargetCombinationUpperWeight;			//组合目标重量上限
	UINT32	IsPriorityScaleEnabled;					  // 1: Enable, 0: Disable
	UINT32	IsProximitCombinationMode;				// 1: AboveUpperLimit, 2: BelowLowerLimit, 0: Disable
	UINT32	CombinationBias;						      //每个Scale要求放几个物品
	UINT32	IsRemainDetectionEnable;				  //Scale处于LAOD状态超过remainDetectionTimeS, 被认为是Remain状态，提示要将物品拿走: 1:Enable， 0：Disble
	UINT32	RemainDetectionTimeSec;					  // RemainDetionTime in Seconds
	UINT32	RemainScaleTreatment;					    // 1: Discharge (提示用户移走），0：Enforce（强制进行组合）
	UINT32	CombinationSpeedMode;					    // 0：SpeedPriority，1: PrecisePriority
	UINT32	CombinationAutoMode;					    // 0: Auto, 1: Manual
	UINT32	MovingAvrageSpeedCount;					  //计算平均速度的时候使用最近多少个组合做统计
	UINT32	spare1;
	UINT32	spare2;
	UINT32	spare3;
	UINT32	spare4;
}CombinationAlgorithmParamaters_t;

typedef struct WeightSensorParamaters
{
	UINT32	WeightSensorLoadDetectionTimeMs;		//称台稳定的判断时间
	UINT32	WeightSensorLoadThread;							//称台稳定门限，如果在WeightSensorLoadDetectionTime内，重量变化都小于WeightSensorLoadThread
	UINT32	WeightSensorEmptyThread;
	UINT32	WeightSensorEmptyDetectionTimeMs;
	UINT32	WeightSensorPickupThread;						// NOT for GUI
	UINT32	WeightSensorPickupDetectionTimeMs;	// NOT for GUI
	UINT32	StardardReadyTimeMs;								//???
	UINT32	MaxAllowedWeight;										//如果发现超过这个最大值，说明Sensor出错
	
	UINT32	WeightSensorInitOrNot;							// NOT for GUI
	UINT32	WeightSensorAdcSampleFreq;					
	UINT32	WeightSensorAdcGain;
	UINT32	WeightSensorAdcBitwidth;						// NOT for GUI
	UINT32  WeightSensorAdcValue;								// NOT for GUI
	UINT32	WeightSensorCalibrationZeroAdcValue;// NOT for GUI
	UINT32	WeightSensorCalibrationFullAdcValue;// NOT for GUI
	UINT32	WeightSensorCalibrationFullWeight;	
	double	WeightSensorCalibrationK;						// NOT for GUI
	UINT32	WeightSensorCalibrationB;						// NOT for GUI
	UINT32	WeightSensorStaticZeroValue;				
	UINT32	WeightSensorTailorValue;						
	UINT32	WeightSensorDynamicZeroThreadValue;	
	UINT32	WeightSensorDynamicZeroHysteresisMs;
	//UINT32  WeightSensorFilterCoeff[32];				// NOT for GUI
	//UINT32  WeightSensorOutputValue[32];				// NOT for GUI
}WeightSensorParamaters_t;

typedef struct MotorControlParamaters
{
	UINT32	MotorSpeed;
	UINT32	MotorDirection;									//0: Clockwise; 1: Counter-Clockwise
	UINT32	MotorRollingStartMs;						//how long do the motor rolling for start action
	UINT32	MotorRollingStopMs;							//how long do the motor rolling for stop action
	UINT32	MotorRollingInveralMs;					//If the motor is rolling, how long the motor will stay in still before roll back (stop action).
	UINT32	MotorFailureDetectionVaration;	// % of the MotorSpeed
	UINT32	MotorFailureDetectionTimeMs;		// within TimeMs, 如果速度都在外面，认为故障
	UINT32	spare1;
	UINT32	spare2;
	UINT32	spare3;
	UINT32	spare4;
}MotorControlParamaters_t;

typedef struct msg_struct_l3bfsc_wmc_set_config_req
{
	UINT16 msgid;
	UINT16 length;
	WeightSensorParamaters_t weight_sensor_param;
	MotorControlParamaters_t motor_control_param;
}msg_struct_l3bfsc_wmc_set_config_req_t;

/*
**	MSG_ID_L3BFSC_WMC_START_REQ
*/
typedef struct msg_struct_l3bfsc_wmc_start_req
{
	UINT16 msgid;
	UINT16 length;
	
}msg_struct_l3bfsc_wmc_start_req_t;

/*
**	MSG_ID_L3BFSC_WMC_STOP_REQ
*/
typedef struct msg_struct_l3bfsc_wmc_stop_req
{
	UINT16 msgid;
	UINT16 length;
}msg_struct_l3bfsc_wmc_stop_req_t;

/*
**	MSG_ID_L3BFSC_WMC_WEIGHT_IND
*/
#define 	WEIGHT_EVENT_ID_LOAD						(0)
#define 	WEIGHT_EVENT_ID_EMPTY						(1)
#define 	WEIGHT_EVENT_ID_PICKUP					(2)

typedef struct WeightIndication
{
	UINT32 weight_event;		//LOAD, EMPTY, PICKUP(FFS)
	UINT32 average_weight;	//average value in the detect window  // <--- MUST
}WeightIndication_t;

typedef struct msg_struct_l3bfsc_wmc_weight_ind
{
	UINT16 msgid;
	UINT16 length;
	WmcId_t wmc_id;               /* 0 ~ 15 is the DIP defined, ID 16 is the main rolling */
	WeightIndication_t weight_ind;
}msg_struct_l3bfsc_wmc_weight_ind_t;

/*
**	MSG_ID_L3BFSC_WMC_COMBIN_OUT_REQ    //bfsc_ws_comb_out_req
*/
//#define 	LED_COMMNAD_ID_IGNORE								(0)//MUSR BE 0
//#define 	LED_COMMNAD_ID_ON										(1)
//#define 	LED_COMMNAD_ID_OFF									(2)
//#define 	LED_COMMNAD_ID_BINKING_HIGHSPEED		(3)
//#define 	LED_COMMNAD_ID_BINKING_LOWSPEED			(4)

//#define 	MOTOR_COMMAND_ID_IGORE						(0) //MUSR BE 0
//#define 	MOTOR_COMMAND_ID_START						(1)
//#define 	MOTOR_COMMAND_ID_STOP							(2)
//#define 	MOTOR_COMMAND_ID_ROLLONCE					(3)
//#define 	MOTOR_COMMAND_ID_SPEED_READ				(4)

// CombineType Defination
// COMNINETPYE_ROOLOUT_START
// COMNINETPYE_ROOLOUT_COMPLETE
// COMNINETPYE_DROP_START
// COMNINETPYE_DROP_COMPLETE
// COMNINETPYE_WARNING_START
// COMNINETPYE_WARNING_COMPLETE
// COMNINETPYE_ERROR_START
// COMNINETPYE_ERROR_COMPLETE

typedef struct CombineType
{
	UINT32	WeightCombineType;
	UINT32	ActionDelayMs;
}CombineType_t;

typedef struct msg_struct_l3bfsc_wmc_combin_out_req
{
	UINT16 msgid;
	UINT16 length;
	CombineType_t weight_combin_type;
}msg_struct_l3bfsc_wmc_combin_out_req_t;

/*
**	MSG_ID_L3BFSC_WMC_FAULT_IND
*/
typedef struct msg_struct_l3bfsc_fault_ind
{
	UINT16 	msgid;
	UINT16 	length;
	WmcId_t wmc_id;               /* 0 ~ 15 is the DIP defined, ID 16 is the main rolling */
	UINT16	error_code;
}msg_struct_l3bfsc_wmc_fault_ind_t;

/*
**	MSG_ID_L3BFSC_WMC_COMMAND_REQ
*/
#define 	SESOR_COMMAND_ID_IGORE							(0) //MUSR BE 0
#define 	SESOR_COMMAND_ID_WEITGH_READ				(3)

typedef struct msg_struct_l3bfsc_wmc_command_req
{
	UINT16 msgid;
	UINT16 length;
	UINT16 spare;
	UINT32 wmc_state;
	UINT8 led1_command;
	UINT8 led2_command;
	UINT8 led3_command;
	UINT8 led4_command;
	UINT32 motor_command;
	UINT32 sensor_command;
}msg_struct_l3bfsc_wmc_command_req_t;

/*
**	MSG_ID_L3BFSC_WMC_COMMAND_RESP
*/
typedef struct msg_struct_l3bfsc_wmc_command_resp
{
	UINT16	msgid;
	UINT16 length;
	WmcId_t wmc_id;               /* 0 ~ 15 is the DIP defined, ID 16 is the main rolling */
  WmcErrorCode_t result;
	UINT32 	motor_speed;
	UINT32 	sensor_weight;
}msg_struct_l3bfsc_wmc_command_resp_t;

/*
**	MSG_ID_L3BFSC_WMC_MSH_HEADER
*/
typedef struct msg_struct_l3bfsc_wmc_msg_header
{
	UINT32 msgid;
	UINT16 length;
	UINT16 spare;
	UINT32 wmc_state;
}msg_struct_l3bfsc_wmc_msg_header_t;

/* Message Length definition */
#define 	MSG_SIZE_L3BFSC_WMC_STARTUP_IND					(sizeof(msg_struct_l3bfsc_wmc_startup_ind_t))
#define 	MSG_SIZE_L3BFSC_WMC_SET_CONFIG_REQ			(sizeof(msg_struct_l3bfsc_wmc_set_config_req_t))
#define 	MSG_SIZE_L3BFSC_WMC_SET_CONFIG_RESP			(sizeof(msg_struct_l3bfsc_wmc_resp_t))
#define 	MSG_SIZE_L3BFSC_WMC_START_REQ						(sizeof(msg_struct_l3bfsc_wmc_start_req_t))
#define 	MSG_SIZE_L3BFSC_WMC_START_RESP					(sizeof(msg_struct_l3bfsc_wmc_resp_t))
#define 	MSG_SIZE_L3BFSC_WMC_STOP_REQ						(sizeof(msg_struct_l3bfsc_wmc_stop_req_t))
#define 	MSG_SIZE_L3BFSC_WMC_STOP_RESP						(sizeof(msg_struct_l3bfsc_wmc_resp_t))
#define 	MSG_SIZE_L3BFSC_WMC_WEIGHT_IND					(sizeof(msg_struct_l3bfsc_wmc_weight_ind_t))
#define 	MSG_SIZE_L3BFSC_WMC_COMBIN_REQ					(sizeof(msg_struct_l3bfsc_wmc_combin_out_req_t))
#define 	MSG_SIZE_L3BFSC_WMC_COMBIN_RESP					(sizeof(msg_struct_l3bfsc_wmc_resp_t))
#define 	MSG_SIZE_L3BFSC_WMC_FAULT_IND						(sizeof(msg_struct_l3bfsc_wmc_fault_ind_t))
#define 	MSG_SIZE_L3BFSC_WMC_COMMAND_REQ					(sizeof(msg_struct_l3bfsc_wmc_command_req_t))
#define 	MSG_SIZE_L3BFSC_WMC_COMMAND_RESP				(sizeof(msg_struct_l3bfsc_wmc_command_resp_t))
	
/* Can ID for communication between AWS and WMC */
#define		AWS_CAN_ID_PREFIX												(0x600U)
#define		WMC_CAN_ID_PREFIX												(0x400U)
#define		AWS_CAN_ID_SUFFIX												(0x001U)
#define		WMC_CAN_ID_SUFFIX												((zWmcInvenory.wmc_id&0x2F))
#define		AWS_CAN_ID															((AWS_CAN_ID_PREFIX)|(AWS_CAN_ID_SUFFIX))
#define		WMC_CAN_ID															((AWS_CAN_ID_PREFIX)|(AWS_CAN_ID_SUFFIX))

/* CAN Msg Lenth */
#define		MAX_WMC_CONTROL_MSG_LEN									(256U)
#define		MAX_WMC_CONTROL_MSG_HEADER_LEN					(sizeof(IHU_HUITP_L2FRAME_STD_frame_header_t))
#define		MAX_WMC_CONTROL_MSG_BODY_LEN						(MAX_WMC_CONTROL_MSG_LEN - MAX_WMC_CONTROL_MSG_HEADER_LEN)

/* ERROR CODE */
typedef enum IHU_ERROR_CODE
{
	//ERROR CODE ID
	ERROR_CODE_NO_ERROR = 0, //Starting point
	
	//COMMON ERROR CODE
	ERROR_CODE_CALLING_ERROR,
	ERROR_CODE_INPUT_PARAMETER_KO,
	ERROR_CODE_WRONG_WMC_STATE,
	ERROR_CODE_UNKNOWN,
	
	/* TO BE ADDED FOR EACH OF THE MESSAGE */
	//**	MSG_ID_L3BFSC_WMC_SET_CONFIG_REQ,
	
	//**	MSG_ID_L3BFSC_WMC_GET_CONFIG_REQ,
	
	//**	MSG_ID_L3BFSC_WMC_START_REQ,
	
	//**	MSG_ID_L3BFSC_WMC_STOP_REQ,
	
	//**	MSG_ID_L3BFSC_WMC_COMBIN_REQ,
	
	//**	MSG_ID_L3BFSC_WMC_COMMAND_REQ,
	
	ERROR_CODE_MAX, //Ending point

}error_code_t; //end of IHU_INTER_TASK_MSG_ID

/* 
** ====================================================================
** ============================= MYC END ==============================
** ====================================================================
*/


#endif /* L0COMVM_COMMSGBFSC_H_ */
