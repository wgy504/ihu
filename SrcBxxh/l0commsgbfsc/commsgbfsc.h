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
#include <huitp.h>


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
//	MSG_ID_L3BFSC_CAN_ERROR_STATUS_REPORT,
//	MSG_ID_L3BFSC_ADC_WS_CMD_CTRL,
//	MSG_ID_L3BFSC_I2C_MOTO_CMD_CTRL,
//	MSG_ID_L3BFSC_CAN_CMD_RESP,
	
	//WMC <-> AWS //MYC
	MSG_ID_L3BFSC_WMC_STARTUP_IND,      
	MSG_ID_L3BFSC_WMC_SET_CONFIG_REQ,    //0x24
	MSG_ID_L3BFSC_WMC_SET_CONFIG_RESP,
	MSG_ID_L3BFSC_WMC_START_REQ,
	MSG_ID_L3BFSC_WMC_START_RESP,
	MSG_ID_L3BFSC_WMC_STOP_REQ,
	MSG_ID_L3BFSC_WMC_STOP_RESP,
	MSG_ID_L3BFSC_WMC_NEW_WS_EVENT,
	MSG_ID_L3BFSC_WMC_REPEAT_WS_EVENT,
	MSG_ID_L3BFSC_WMC_COMBIN_REQ,
	MSG_ID_L3BFSC_WMC_COMBIN_RESP,
	MSG_ID_L3BFSC_WMC_FAULT_IND,
	MSG_ID_L3BFSC_WMC_COMMAND_REQ,
	MSG_ID_L3BFSC_WMC_COMMAND_RESP,
	MSG_ID_L3BFSC_WMC_WEIGHT_IND,
	MSG_ID_L3BFSC_WMC_ERR_INQ_CMD_REQ,
	MSG_ID_L3BFSC_WMC_ERR_INQ_CMD_RESP,
	
	MSG_ID_L3BFSC_WMC_HEART_BEAT_REPORT,
	MSG_ID_L3BFSC_WMC_HEART_BEAT_CONFIRM,
	//HUITP_MSGID_sui_bfsc_heart_beat_report           = 0x3BA0,
	//HUITP_MSGID_sui_bfsc_heart_beat_confirm          = 0x3B20,
	
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

typedef struct msg_struct_l3bfsc_wmc_resp
{
	UINT16	msgid;
	UINT16  length;
  UINT8  validFlag;  //是否执行成功
	UINT8  spare1;
	UINT16  errCode;
	//WmcId_t wmc_id;               /* 0 ~ 15 is the DIP defined, ID 16 is the main rolling */
  //WmcErrorCode_t result;
}msg_struct_l3bfsc_wmc_resp_t;


typedef struct msg_struct_l3bfsc_wmc_startup_ind
{
	UINT16 msgid;
	UINT16 length;
	WmcInventory_t wmc_inventory;
}msg_struct_l3bfsc_wmc_startup_ind_t;


typedef struct msg_struct_l3bfsc_wmc_set_config_req
{
	UINT16 msgid;
	UINT16 length;
	WeightSensorParamaters_t weight_sensor_param;
	MotorControlParamaters_t motor_control_param;
}msg_struct_l3bfsc_wmc_set_config_req_t;


typedef struct msg_struct_l3bfsc_set_config_resp
{
	UINT16	msgid;
	UINT16  length;
  UINT8  validFlag;  //是否执行成功
	UINT8  spare1;
	UINT16  errCode;
}msg_struct_l3bfsc_set_config_resp_t;

/*
**	MSG_ID_L3BFSC_WMC_START_REQ
*/
typedef struct msg_struct_l3bfsc_wmc_start_req
{
	UINT16 msgid;
	UINT16 length;
}msg_struct_l3bfsc_wmc_start_req_t;


typedef struct msg_struct_l3bfsc_start_resp
{
	UINT16	msgid;
	UINT16  length;
  UINT8  validFlag;  //是否执行成功
	UINT8  spare1;
	UINT16  errCode;
}msg_struct_l3bfsc_start_resp_t;

/*
**	MSG_ID_L3BFSC_WMC_STOP_REQ
*/
typedef struct msg_struct_l3bfsc_wmc_stop_req
{
	UINT16 msgid;
	UINT16 length;
}msg_struct_l3bfsc_wmc_stop_req_t;


typedef struct msg_struct_l3bfsc_stop_resp
{
	UINT16	msgid;
	UINT16  length;
  UINT8  validFlag;  //是否执行成功
	UINT8  spare1;
	UINT16  errCode;
}msg_struct_l3bfsc_stop_resp_t;

// MSG_ID_L3BFSC_WMC_WEIGHT_IND -> This is pure for internal message between Weight Thread and L3BFSC thread
typedef struct msg_struct_l3bfsc_weight_ind
{
	UINT32 adc_filtered;
	UINT32 average_weight;
	UINT32 weight_event;
  UINT32 repeat_times;
}msg_struct_l3bfsc_weight_ind_t;

typedef struct msg_struct_l3bfsc_wmc_ws_event
{
	UINT16 msgid;
	UINT16 length;
	WmcId_t wmc_id;               /* 0 ~ 15 is the DIP defined, ID 16 is the main rolling */
	WeightIndication_t weight_ind;
  CombineType_t weight_combin_type;
}msg_struct_l3bfsc_wmc_ws_event_t;

typedef struct msg_struct_l3bfsc_new_ws_event
{
	UINT16 msgid;
	UINT16 length;
	WmcId_t wmc_id;               /* 0 ~ 15 is the DIP defined, ID 16 is the main rolling */
	WeightIndication_t weight_ind;
  CombineType_t weight_combin_type;
}msg_struct_l3bfsc_new_ws_event_t;

typedef struct msg_struct_l3bfsc_repeat_ws_event
{
	UINT16 msgid;
	UINT16 length;
	WmcId_t wmc_id;               /* 0 ~ 15 is the DIP defined, ID 16 is the main rolling */
	WeightIndication_t weight_ind;
  CombineType_t weight_combin_type;
}msg_struct_l3bfsc_repeat_ws_event_t;

typedef struct msg_struct_l3bfsc_wmc_combin_out_req
{
	UINT16 msgid;
	UINT16 length;
	CombineType_t weight_combin_type;
}msg_struct_l3bfsc_wmc_combin_out_req_t;


typedef struct msg_struct_l3bfsc_wmc_combin_out_resp
{
	UINT16 msgid;
	UINT16 length;
	CombineType_t weight_combin_type;
}msg_struct_l3bfsc_wmc_combin_out_resp_t;


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
//#define SENSOR_COMMAND_ID_WEITGH_READ (0x0001)
//#define MOTOR_COMMAND_ID (0x0002)
//#define LED1_COMMAND_ID (0x0004)
//#define LED2_COMMAND_ID (0x0008)
//#define LED3_COMMAND_ID (0x0010)
//#define LED4_COMMAND_ID (0x0020)

//#define 	LED_COMMNAD_ON										(1)
//#define 	LED_COMMNAD_OFF									  (2)
//#define 	LED_COMMNAD_BINKING_HIGHSPEED		  (3)
//#define 	LED_COMMNAD_BINKING_LOWSPEED			(4)

typedef struct msg_struct_l3bfsc_wmc_command_req
{
	UINT16 msgid;
	UINT16 length;
	UINT32 comand_flags;
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
	UINT8  validFlag;  //是否执行成功
	UINT8  spare1;
	UINT16 spare2;
}msg_struct_l3bfsc_wmc_command_resp_t;


typedef struct msg_struct_l3bfsc_wmc_err_inq_req
{
	UINT16 	msgid;
	UINT16 	length;
	WmcId_t wmc_id;               /* 0 ~ 15 is the DIP defined, ID 16 is the main rolling */
	UINT16	error_code;
}msg_struct_l3bfsc_wmc_err_inq_req_t;


//HUITP_MSGID_sui_bfsc_err_inq_cmd_resp            = 0x3B9A,
typedef struct msg_struct_l3bfsc_wmc_err_inq_resp
{
	UINT16 	msgid;
	UINT16 	length;
	WmcId_t wmc_id;               /* 0 ~ 15 is the DIP defined, ID 16 is the main rolling */
	UINT16	error_code;
	UINT32  average_weight;
	UINT8  validFlag;  //是否执行成功
	UINT8  spare1;
	UINT16 spare2;
}msg_struct_l3bfsc_wmc_err_inq_resp_t;

//心跳过程
//HUITP_MSGID_sui_bfsc_heart_beat_report           = 0x3BA0,
typedef struct msg_struct_l3bfsc_wmc_heart_beat_report
{
	UINT16 	msgid;
	UINT16 	length;
	WmcId_t wmc_id;               /* 0 ~ 15 is the DIP defined, ID 16 is the main rolling */
}msg_struct_l3bfsc_wmc_heart_beat_report_t;

//HUITP_MSGID_sui_bfsc_heart_beat_confirm          = 0x3B20,
typedef struct msg_struct_l3bfsc_wmc_heart_beat_confirm
{
	UINT16 	msgid;
	UINT16 	length;
	WmcId_t wmc_id;               /* 0 ~ 15 is the DIP defined, ID 16 is the main rolling */
	UINT8   wmcState;
	UINT8   spare1;
	UINT16  spare2;
}msg_struct_l3bfsc_wmc_heart_beat_confirm_t;

/*
**	MSG_ID_L3BFSC_WMC_MSH_HEADER
*/
typedef struct msg_struct_l3bfsc_wmc_msg_header
{
	UINT16 msgid;
	UINT16 length;
}msg_struct_l3bfsc_wmc_msg_header_t;



/* Message Length definition */
#define 	MSG_SIZE_L3BFSC_WMC_STARTUP_IND					(sizeof(msg_struct_l3bfsc_wmc_startup_ind_t))
#define 	MSG_SIZE_L3BFSC_WMC_SET_CONFIG_REQ			(sizeof(msg_struct_l3bfsc_wmc_set_config_req_t))
#define 	MSG_SIZE_L3BFSC_WMC_SET_CONFIG_RESP			(sizeof(msg_struct_l3bfsc_wmc_resp_t))
#define 	MSG_SIZE_L3BFSC_WMC_START_REQ						(sizeof(msg_struct_l3bfsc_wmc_start_req_t))
#define 	MSG_SIZE_L3BFSC_WMC_START_RESP					(sizeof(msg_struct_l3bfsc_wmc_resp_t))
#define 	MSG_SIZE_L3BFSC_WMC_STOP_REQ						(sizeof(msg_struct_l3bfsc_wmc_stop_req_t))
#define 	MSG_SIZE_L3BFSC_WMC_STOP_RESP						(sizeof(msg_struct_l3bfsc_wmc_resp_t))
#define 	MSG_SIZE_L3BFSC_WMC_NEW_WS_EVENT				(sizeof(msg_struct_l3bfsc_wmc_ws_event_t))
#define 	MSG_SIZE_L3BFSC_WMC_REPEAT_WS_EVENT			(sizeof(msg_struct_l3bfsc_wmc_ws_event_t))	
#define 	MSG_SIZE_L3BFSC_WMC_COMBIN_REQ					(sizeof(msg_struct_l3bfsc_wmc_combin_out_req_t))
#define 	MSG_SIZE_L3BFSC_WMC_COMBIN_RESP					(sizeof(msg_struct_l3bfsc_wmc_combin_out_resp_t))
#define 	MSG_SIZE_L3BFSC_WMC_FAULT_IND						(sizeof(msg_struct_l3bfsc_wmc_fault_ind_t))
#define 	MSG_SIZE_L3BFSC_WMC_COMMAND_REQ					(sizeof(msg_struct_l3bfsc_wmc_command_req_t))
#define 	MSG_SIZE_L3BFSC_WMC_COMMAND_RESP				(sizeof(msg_struct_l3bfsc_wmc_command_resp_t))
#define 	MSG_SIZE_L3BFSC_WMC_ERR_INQ_REQ					(sizeof(msg_struct_l3bfsc_wmc_err_inq_req_t))
#define 	MSG_SIZE_L3BFSC_WMC_ERR_INQ_RESP				(sizeof(msg_struct_l3bfsc_wmc_err_inq_resp_t))
#define 	MSG_SIZE_L3BFSC_WMC_HEART_BEAT_REP			(sizeof(msg_struct_l3bfsc_wmc_heart_beat_report_t))
#define 	MSG_SIZE_L3BFSC_WMC_HEART_BEAT_CONF			(sizeof(msg_struct_l3bfsc_wmc_heart_beat_confirm_t))

/* Can ID for communication between AWS and WMC */
/* 1: AWS to WMC: 1 to n, n = 0 ... 15       */
/* Node ID bitmap: CAN ID = 0x0010 XXXX    */
/* CAN ID          direction => ^  ~~~~ <= 1111 1111 1111 1111, */
//#define AWS_TO_WMC_CAN_ID_PREFIX		(0x00100000U)
//#define AWS_TO_WMC_CAN_ID_SUFFIX		(mwc_id_bitmap)

/* 2: AWS to WMC-S: 1 to 1  */
/* CAN ID: 0x0010 0000 */
//#define AWS_TO_WMCS_CAN_ID				(0x00110000U)

/* 3: WMC to AWS: */
/* CAN ID: 0x0030 0000 (WMC Node ID 0) to 0x0030 000F (WMC Node ID 15) */
//#define WMC_TO_AWS_CAN_ID_PREFIX		(0x00200000U)
//#define WMC_TO_AWS_CAN_ID_SUFFIX		(mwc_id)

/* 4: WMC-S to AWS: */
/* CAN ID: 0x0030 0010 (Node ID 0) */
//#define WMCS_TO_AWS_CAN_ID				(0x00200010U)

/* Can ID for communication between AWS and WMC */

#define		WMC_CAN_ID_SUFFIX												((zWmcInvenory.wmc_id.wmc_id&0x2F))
#define		AWS_CAN_ID															((WMC_TO_AWS_CAN_ID_PREFIX)|(WMC_CAN_ID_SUFFIX))
#define		WMC_CAN_ID															((AWS_TO_WMC_CAN_ID_PREFIX)|(1<<WMC_CAN_ID_SUFFIX))



/* CAN Msg Lenth */
#define		MAX_WMC_CONTROL_MSG_LEN									(256U)
#define		MAX_WMC_CONTROL_MSG_HEADER_LEN					(sizeof(IHU_HUITP_L2FRAME_STD_frame_header_t))
#define		MAX_WMC_CONTROL_MSG_BODY_LEN						(MAX_WMC_CONTROL_MSG_LEN - MAX_WMC_CONTROL_MSG_HEADER_LEN)

///* ERROR CODE */
//typedef enum IHU_ERROR_CODE
//{
//	//ERROR CODE ID
//	ERROR_CODE_NO_ERROR = 0, //Starting point
//	
//	//COMMON ERROR CODE
//	ERROR_CODE_CALLING_ERROR,
//	ERROR_CODE_INPUT_PARAMETER_KO,
//	ERROR_CODE_WRONG_WMC_STATE,
//	ERROR_CODE_UNKNOWN,
//	
//	/* TO BE ADDED FOR EACH OF THE MESSAGE */
//	//**	MSG_ID_L3BFSC_WMC_SET_CONFIG_REQ,
//	
//	//**	MSG_ID_L3BFSC_WMC_GET_CONFIG_REQ,
//	
//	//**	MSG_ID_L3BFSC_WMC_START_REQ,
//	
//	//**	MSG_ID_L3BFSC_WMC_STOP_REQ,
//	
//	//**	MSG_ID_L3BFSC_WMC_COMBIN_REQ,
//	
//	//**	MSG_ID_L3BFSC_WMC_COMMAND_REQ,
//	
//	ERROR_CODE_MAX, //Ending point

//}error_code_t; //end of IHU_INTER_TASK_MSG_ID

extern WmcInventory_t										zWmcInvenory;
/* 
** ====================================================================
** ============================= MYC END ==============================
** ====================================================================
*/


#endif /* L0COMVM_COMMSGBFSC_H_ */
