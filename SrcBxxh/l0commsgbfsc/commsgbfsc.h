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
//typedef UINT8           		bool;
typedef UINT8           		BOOL;  

//2. 公共消息结构体定义
//Under normal case, 1024Byte shall be enough for internal message communciation purpose.
//If not enough, need modify here to enlarge
#define MAX_IHU_MSG_BUF_LENGTH MAX_IHU_MSG_BODY_LENGTH+6
#define MAX_IHU_MSG_BUF_LENGTH_CLOUD MAX_IHU_MSG_BODY_LENGTH-6
typedef struct IhuMsgSruct
{
	UINT16 msgType;
	UINT8 dest_id;
	UINT8 src_id;
	UINT16 msgLen;
	INT8 msgBody[MAX_IHU_MSG_BODY_LENGTH];
}IhuMsgSruct_t;

//所有任务模块的优先级是一样的
#define  IHU_THREAD_PRIO  10          //priority of the main loop de 1 a 99 max

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
	UINT8 length;
}msg_struct_com_init_t;

//MSG_ID_COM_INIT_FB
typedef struct msg_struct_com_init_fb //
{
	UINT8 length;
}msg_struct_com_init_fb_t;

//MSG_ID_COM_RESTART
typedef struct  msg_struct_com_restart //
{
	UINT8 length;
}msg_struct_com_restart_t;

//MSG_ID_COM_TIME_OUT
typedef struct  msg_struct_com_time_out //
{
	UINT32 timeId;
	UINT8  timeRes;
	UINT8 length;
}msg_struct_com_time_out_t;

//MSG_ID_COM_STOP
typedef struct  msg_struct_com_stop //
{
	UINT8 length;
}msg_struct_com_stop_t;

//MSG_ID_COM_HEART_BEAT
typedef struct  msg_struct_com_heart_beat //
{
	UINT8 length;
}msg_struct_com_heart_beat_t;

//MSG_ID_COM_HEART_BEAT_FB
typedef struct  msg_struct_com_heart_beat_fb //
{
	UINT8 length;
}msg_struct_com_heart_beat_fb_t;

//VMFO消息定义
typedef struct msg_struct_vmfo_1s_period_timtout
{
	UINT8 length;
}msg_struct_vmfo_1s_period_timtout_t;

//ADC
//MSG_ID_ADC_L3BFSC_MEAS_CMD_RESP
typedef struct msg_struct_adclibra_l3bfsc_meas_cmd_resp
{
	UINT8 cmdid;
	com_ws_and_moto_cmd_t cmd;
	UINT8 length;
}msg_struct_adclibra_l3bfsc_meas_cmd_resp_t;
#define IHU_ADC_BFSC_WS_CMD_TYPE_NONE 0
#define IHU_ADC_BFSC_WS_CMD_TYPE_START_CFM 1
#define IHU_ADC_BFSC_WS_CMD_TYPE_STOP_CFM 2
#define IHU_ADC_BFSC_WS_CMD_TYPE_RESP 3

//MSG_ID_ADC_NEW_MATERIAL_WS,
typedef struct msg_struct_adc_new_material_ws
{
	INT32 wsValue;
	UINT8 length;
}msg_struct_adc_new_material_ws_t;

//MSG_ID_ADC_MATERIAL_DROP,
typedef struct msg_struct_adc_material_drop
{
	UINT8 length;
}msg_struct_adc_material_drop_t;


//UART消息定义
typedef struct msg_struct_spsvirgo_l2frame_send
{
	UINT8 data[MAX_IHU_MSG_BODY_LENGTH-1];
	UINT8 length;
}msg_struct_spsvirgo_l2frame_send_t;
typedef struct msg_struct_spsvirgo_l2frame_rcv
{
	UINT8 data[MAX_IHU_MSG_BODY_LENGTH-1];
	UINT8 length;
}msg_struct_spsvirgo_l2frame_rcv_t;

//SPI消息定义
typedef struct msg_struct_spileo_l2frame_send
{
	UINT8 data[MAX_IHU_MSG_BODY_LENGTH-1];
	UINT8 length;
}msg_struct_spileo_l2frame_send_t;
typedef struct msg_struct_spileo_l2frame_rcv
{
	UINT8 data[MAX_IHU_MSG_BODY_LENGTH-1];
	UINT8 length;
}msg_struct_spileo_l2frame_rcv_t;

//I2C消息定义
//MSG_ID_I2C_L2FRAME_SEND
typedef struct msg_struct_i2caries_l2frame_send
{
	UINT8 data[MAX_IHU_MSG_BODY_LENGTH-1];	
	UINT8 length;
}msg_struct_i2caries_l2frame_send_t;
//MSG_ID_I2C_L2FRAME_RCV
typedef struct msg_struct_i2caries_l2frame_rcv
{
	UINT8 data[MAX_IHU_MSG_BODY_LENGTH-1];
	UINT8 length;
}msg_struct_i2caries_l2frame_rcv_t;

//MSG_ID_I2C_L3BFSC_MOTO_CMD_RESP
typedef struct msg_struct_i2caries_l3bfsc_cmd_resp
{
	UINT8 cmdid;
	com_ws_and_moto_cmd_t cmd;	
	UINT8 length;
}msg_struct_i2caries_l3bfsc_cmd_resp_t;
#define IHU_I2C_BFSC_WS_CMD_TYPE_NONE 0
#define IHU_I2C_BFSC_WS_CMD_TYPE_START_CFM 1
#define IHU_I2C_BFSC_WS_CMD_TYPE_STOP_CFM 2
#define IHU_I2C_BFSC_WS_CMD_TYPE_RESP 3

//CAN消息定义
//MSG_ID_CAN_L2FRAME_SEND
typedef struct msg_struct_canvela_l2frame_send
{
	UINT8 data[MAX_IHU_MSG_BODY_LENGTH-1];
	UINT8 length;
}msg_struct_canvela_l2frame_send_t;

//MSG_ID_CAN_L2FRAME_RCV
typedef struct msg_struct_canvela_l2frame_rcv
{
	UINT8 data[MAX_IHU_MSG_BODY_LENGTH-1];
	UINT8 length;
}msg_struct_canvela_l2frame_rcv_t;

//MSG_ID_CAN_L3BFSC_CMD_CTRL
typedef struct msg_struct_canvela_l3bfsc_cmd_ctrl
{
	com_ws_and_moto_cmd_t cmd;	
	UINT8 length;
}msg_struct_canvela_l3bfsc_cmd_ctrl_t;

//MSG_ID_CAN_L3BFSC_INIT_REQ
typedef struct msg_struct_canvela_l3bfsc_init_req
{
	UINT8 length;
}msg_struct_canvela_l3bfsc_init_req_t;

//MSG_ID_CAN_L3BFSC_ROLL_OUT_REQ,
typedef struct msg_struct_canvela_l3bfsc_roll_out_req
{
	UINT8 length;
}msg_struct_canvela_l3bfsc_roll_out_req_t;

//MSG_ID_CAN_L3BFSC_GIVE_UP_REQ,
typedef struct msg_struct_canvela_l3bfsc_give_up_req
{
	UINT8 length;
}msg_struct_canvela_l3bfsc_give_up_req_t;

//MSG_ID_CAN_ADC_WS_MAN_SET_ZERO
typedef struct msg_struct_canvela_adc_ws_man_set_zero
{
	UINT8 length;
}msg_struct_canvela_adc_ws_man_set_zero_t;

//L3BFSC消息
//MSG_ID_L3BFSC_CAN_INIT_RESP
typedef struct msg_struct_l3bfsc_canvela_init_resp
{
	UINT8 length;
}msg_struct_l3bfsc_canvela_init_resp_t;

//MSG_ID_L3BFSC_CAN_NEW_WS_EVENT
typedef struct msg_struct_l3bfsc_canvela_new_ws_event
{
	INT32 wsValue;
	UINT8 length;
}msg_struct_l3bfsc_canvela_new_ws_event_t;

//MSG_ID_L3BFSC_CAN_ROLL_OUT_RESP,
typedef struct msg_struct_l3bfsc_canvela_roll_out_resp
{
	UINT8 length;
}msg_struct_l3bfsc_canvela_roll_out_resp_t;

//MSG_ID_L3BFSC_CAN_GIVE_UP_RESP,
typedef struct msg_struct_l3bfsc_canvela_give_up_resp
{
	UINT8 length;
}msg_struct_l3bfsc_canvela_give_up_resp_t;

//MSG_ID_L3BFSC_CAN_ERROR_STATUS_REPORT
typedef struct msg_struct_l3bfsc_canvela_error_status_report
{
	UINT32 errId;
	UINT32 errCause;
	UINT32 errSource;
	UINT8 length;
}msg_struct_l3bfsc_canvela_error_status_report_t;

//MSG_ID_L3BFSC_ADC_WS_CMD_CTRL
typedef struct msg_struct_l3bfsc_adc_ws_cmd_ctrl
{
	UINT8 cmdid;
	com_ws_and_moto_cmd_t cmd;
	UINT8 length;
}msg_struct_l3bfsc_adc_ws_cmd_ctrl_t;
#define IHU_BFSC_ADC_WS_CMD_TYPE_NONE 0
#define IHU_BFSC_ADC_WS_CMD_TYPE_START 1
#define IHU_BFSC_ADC_WS_CMD_TYPE_STOP 2
#define IHU_BFSC_ADC_WS_CMD_TYPE_CTRL 3

//MSG_ID_L3BFSC_I2C_MOTO_CMD_CTRL
typedef struct msg_struct_l3bfsc_i2c_moto_cmd_ctrl
{
	UINT8 cmdid;
	com_ws_and_moto_cmd_t cmd;	
	UINT8 length;
}msg_struct_l3bfsc_i2c_moto_cmd_ctrl_t;
#define IHU_BFSC_I2C_MOTO_CMD_TYPE_NONE 0
#define IHU_BFSC_I2C_MOTO_CMD_TYPE_START 1
#define IHU_BFSC_I2C_MOTO_CMD_TYPE_STOP 2
#define IHU_BFSC_I2C_MOTO_CMD_TYPE_CTRL 3

//MSG_ID_L3BFSC_CAN_CMD_RESP
typedef struct msg_struct_l3bfsc_canvela_cmd_resp
{
	UINT8 cmdid;
	com_ws_and_moto_cmd_t cmd;	
	UINT8 length;
}msg_struct_l3bfsc_canvela_cmd_resp_t;
#define IHU_BFSC_CAN_CMD_TYPE_NONE 0
#define IHU_BFSC_CAN_CMD_TYPE_START_CFM 1
#define IHU_BFSC_CAN_CMD_TYPE_STOP_CFM 2
#define IHU_BFSC_CAN_CMD_TYPE_RESP 3

#endif /* L0COMVM_COMMSGBFSC_H_ */
