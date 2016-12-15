/*
 * commsgccl.h
 *
 *  Created on: 2016年1月3日
 *      Author: test
 */

#ifndef L0COMVM_COMMSGCCL_H_
#define L0COMVM_COMMSGCCL_H_

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
typedef struct IhuMsgSruct
{
	UINT16 msgType;
	UINT8 dest_id;
	UINT8 src_id;
	UINT16 msgLen;
	INT8 msgBody[MAX_IHU_MSG_BODY_LENGTH];
}IhuMsgSruct_t;

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
	
	//LED

	//DIDO
	MSG_ID_DIDO_CCL_SENSOR_STATUS_RESP,
	MSG_ID_DIDO_EVENT_LOCK_TRIGGER,
	MSG_ID_DIDO_EVENT_FAULT_TRIGGER,
	MSG_ID_DIDO_LOCK_TRIGGER_EVENT,
	MSG_ID_DIDO_DOOR_ILG_OPEN_EVENT,
	MSG_ID_DIDO_LOCK_ILG_OPEN_EVENT,
	MSG_ID_DIDO_DOOR_OPEN_EVENT,
	MSG_ID_DIDO_LOCK_C_DOOR_C_EVENT,
	MSG_ID_DIDO_SENSOR_WARNING_EVENT,

	//SPS
	MSG_ID_SPS_L2FRAME_SEND,
	MSG_ID_SPS_L2FRAME_RCV,
	MSG_ID_SPS_TO_CCL_CLOUD_FB,
	MSG_ID_SPS_CCL_SENSOR_STATUS_RESP,
	MSG_ID_SPS_CCL_EVENT_REPORT_CFM,
	
	//SPI
	MSG_ID_SPI_L2FRAME_SEND,
	MSG_ID_SPI_L2FRAME_RCV,
		
	//I2C
	MSG_ID_I2C_L2FRAME_SEND,
	MSG_ID_I2C_L2FRAME_RCV,
	MSG_ID_I2C_CCL_SENSOR_STATUS_RESP,
	
	//CAN
	MSG_ID_CAN_L2FRAME_SEND,
	MSG_ID_CAN_L2FRAME_RCV,

	//DCMI
	MSG_ID_DCMI_CCL_SENSOR_STATUS_RESP,
	
	//CCL
	MSG_ID_CCL_TO_SPS_OPEN_AUTH_INQ,  	  //Back hawl
	MSG_ID_CCL_DIDO_SENSOR_STATUS_REQ,   
	MSG_ID_CCL_SPS_SENSOR_STATUS_REQ,   
	MSG_ID_CCL_I2C_SENSOR_STATUS_REQ,   
	MSG_ID_CCL_DCMI_SENSOR_STATUS_REQ,
	MSG_ID_CCL_DIDO_CTRL_CMD,
	MSG_ID_CCL_SPS_CTRL_CMD,
	MSG_ID_CCL_I2C_CTRL_CMD,
	MSG_ID_CCL_DCMI_CTRL_CMD,
	MSG_ID_CCL_SPS_EVENT_REPORT_SEND,

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

typedef struct com_sensor_status //
{
	UINT8 doorState[IHU_CCL_SENSOR_NUMBER_MAX];
	UINT8 lockTongueState[IHU_CCL_SENSOR_NUMBER_MAX];
	UINT8 lockiTriggerState[IHU_CCL_SENSOR_NUMBER_MAX];
	UINT8 lockoEnableState[IHU_CCL_SENSOR_NUMBER_MAX];
	UINT8 cameraState[IHU_CCL_SENSOR_NUMBER_MAX];
	UINT8 smokeState;
	UINT8 waterState;
	UINT8 fallState;	
	UINT8 shakeState;
	INT16 batteryValue;
	INT16 tempValue;
	INT16 humidValue;
	INT16 rssiValue;
	INT16 rsv1Value;  //两个预留传感器之一
	INT16 rsv2Value;	 //两个预留传感器之一
}com_sensor_status_t;
#define IHU_CCL_SENSOR_STATE_NONE 0
#define IHU_CCL_SENSOR_STATE_CLOSE 1
#define IHU_CCL_SENSOR_STATE_OPEN 2
#define IHU_CCL_SENSOR_STATE_DEACTIVE 1
#define IHU_CCL_SENSOR_STATE_ACTIVE 2


//跟CCL独特相关的所有CMDID统一编码
#define IHU_CCL_DH_CMDID_NONE 0
#define IHU_CCL_DH_CMDID_REQ_STATUS_DOOR 1
#define IHU_CCL_DH_CMDID_REQ_STATUS_LOCKI 2
#define IHU_CCL_DH_CMDID_REQ_STATUS_RSSI 2
#define IHU_CCL_DH_CMDID_REQ_STATUS_BATTERY 3
#define IHU_CCL_DH_CMDID_REQ_STATUS_DIDO 4
#define IHU_CCL_DH_CMDID_REQ_STATUS_SPS 5
#define IHU_CCL_DH_CMDID_REQ_STATUS_I2C 6
#define IHU_CCL_DH_CMDID_REQ_STATUS_DCMI 7
#define IHU_CCL_DH_CMDID_RESP_STATUS_DOOR 11
#define IHU_CCL_DH_CMDID_RESP_STATUS_LOCKI 12
#define IHU_CCL_DH_CMDID_RESP_STATUS_RSSI 12
#define IHU_CCL_DH_CMDID_RESP_STATUS_BATTERY 13
#define IHU_CCL_DH_CMDID_RESP_STATUS_DIDO 14
#define IHU_CCL_DH_CMDID_RESP_STATUS_SPS 15
#define IHU_CCL_DH_CMDID_RESP_STATUS_I2C 16
#define IHU_CCL_DH_CMDID_RESP_STATUS_DCMI 17

#define IHU_CCL_DH_CMDID_EVENT_REPORT 20
#define IHU_CCL_DH_CMDID_EVENT_IND_DOOR_C_TO_O 21
#define IHU_CCL_DH_CMDID_EVENT_IND_DOOR_O_TO_C 22
#define IHU_CCL_DH_CMDID_EVENT_IND_LOCK_C_TO_O 23
#define IHU_CCL_DH_CMDID_EVENT_IND_LOCK_O_TO_C 24
#define IHU_CCL_DH_CMDID_EVENT_IND_LOCK_TRIGGER 30    //触发CCL进入工作状态
#define IHU_CCL_DH_CMDID_EVENT_IND_SHAKE_TRIGGER 31   //触发CCL进入工作状态
#define IHU_CCL_DH_CMDID_EVENT_IND_LOCK_AND_SHAKE 32  //触发CCL进入工作状态

#define IHU_CCL_DH_CMDID_EVENT_IND_FAULT_DOOR_O 40    //触发CCL进入错误模式
#define IHU_CCL_DH_CMDID_EVENT_IND_FAULT_LOCK_O 41    //触发CCL进入错误模式
#define IHU_CCL_DH_CMDID_EVENT_IND_FAULT_BAT_WARNING 42    //触发CCL进入错误模式
#define IHU_CCL_DH_CMDID_EVENT_IND_FAULT_FALL_WARNING 43    //触发CCL进入错误模式
#define IHU_CCL_DH_CMDID_EVENT_IND_FAULT_SMOKE_WARNING 44    //触发CCL进入错误模式
#define IHU_CCL_DH_CMDID_EVENT_IND_FAULT_WATER_WARNING 45    //触发CCL进入错误模式

#define IHU_CCL_DH_CMDID_HEART_BEAT 0xFE
#define IHU_CCL_DH_CMDID_INVAID 0xFF

#define IHU_CCL_DIDO_WORKING_MODE_NONE 0
#define IHU_CCL_DIDO_WORKING_MODE_SLEEP 1
#define IHU_CCL_DIDO_WORKING_MODE_ACTIVE 2
#define IHU_CCL_DIDO_WORKING_MODE_FAULT 3
#define IHU_CCL_DIDO_WORKING_MODE_INVALID 0xFF

#define IHU_CCL_ADC_WORKING_MODE_NONE 0
#define IHU_CCL_ADC_WORKING_MODE_SLEEP 1
#define IHU_CCL_ADC_WORKING_MODE_ACTIVE 2
#define IHU_CCL_ADC_WORKING_MODE_FAULT 3
#define IHU_CCL_ADC_WORKING_MODE_INVALID 0xFF

#define IHU_CCL_I2C_WORKING_MODE_NONE 0
#define IHU_CCL_I2C_WORKING_MODE_SLEEP 1
#define IHU_CCL_I2C_WORKING_MODE_ACTIVE 2
#define IHU_CCL_I2C_WORKING_MODE_FAULT 3
#define IHU_CCL_I2C_WORKING_MODE_INVALID 0xFF

#define IHU_CCL_SPS_WORKING_MODE_NONE 0
#define IHU_CCL_SPS_WORKING_MODE_SLEEP 1
#define IHU_CCL_SPS_WORKING_MODE_ACTIVE 2
#define IHU_CCL_SPS_WORKING_MODE_INVALID 0xFF

#define IHU_CCL_DCMI_WORKING_MODE_NONE 0
#define IHU_CCL_DCMI_WORKING_MODE_SLEEP 1
#define IHU_CCL_DCMI_WORKING_MODE_ACTIVE 2
#define IHU_CCL_DCMI_WORKING_MODE_FAULT 3
#define IHU_CCL_DCMI_WORKING_MODE_INVALID 0xFF


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

//DIDO

//MSG_ID_DIDO_CCL_SENSOR_STATUS_RESP
typedef struct msg_struct_dido_ccl_sensor_status_rep
{
	UINT8 cmdid;
	com_sensor_status_t sensor;
	UINT8 length;
}msg_struct_dido_ccl_sensor_status_rep_t;

//MSG_ID_DIDO_EVENT_LOCK_TRIGGER
typedef struct msg_struct_dido_event_lock_trigger
{
	UINT8 cmdid;
	UINT8 lockid;  //指示是哪一个锁触发的
	UINT8 length;
}msg_struct_dido_event_lock_trigger_t;

//MSG_ID_DIDO_EVENT_FAULT_TRIGGER
typedef struct msg_struct_dido_event_fault_trigger
{
	UINT8 cmdid;
	UINT8 lockid;  //指示是哪一个锁触发的
	com_sensor_status_t sensor;
	UINT8 length;
}msg_struct_dido_event_fault_trigger_t;

//
typedef struct msg_struct_dido_lock_trigger_event
{
	UINT8 cmdid;
	UINT8 length;
}msg_struct_dido_lock_trigger_event_t;
typedef struct msg_struct_dido_door_ilg_open_event
{
	UINT8 cmdid;
	UINT8 length;
}msg_struct_dido_door_ilg_open_event_t;
typedef struct msg_struct_dido_lock_ilg_open_event
{
	UINT8 cmdid;
	UINT8 length;
}msg_struct_dido_lock_ilg_open_event_t;
typedef struct msg_struct_dido_door_open_event
{
	UINT8 cmdid;
	UINT8 length;
}msg_struct_dido_door_open_event_t;
typedef struct msg_struct_dido_lock_c_door_c_event
{
	UINT8 cmdid;
	UINT8 length;
}msg_struct_dido_lock_c_door_c_event_t;
typedef struct msg_struct_dido_sensor_warning_event
{
	UINT8 cmdid;
	UINT16 waterThred;
	UINT16 shakeThred;
	UINT16 smokeThred;
	UINT16 tempThred;
	UINT16 humidThred;
	UINT16 d3axisThre; //三轴传感器表达倾斜
	UINT16 powerThred;
	UINT8 length;
}msg_struct_dido_sensor_warning_event_t;


//SPS消息定义
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
typedef struct msg_struct_spsvirgo_to_ccl_cloud_fb
{
	UINT8 cmdid;
	UINT8 length;
}msg_struct_spsvirgo_to_ccl_cloud_fb_t;

//MSG_ID_SPS_CCL_SENSOR_STATUS_RESP
typedef struct msg_struct_sps_ccl_sensor_status_rep
{
	UINT8 cmdid;
	com_sensor_status_t sensor;
	UINT8 length;
}msg_struct_sps_ccl_sensor_status_rep_t;

//MSG_ID_SPS_CCL_EVENT_REPORT_CFM
typedef struct msg_struct_sps_ccl_event_report_cfm
{
	UINT8 cmdid;
	UINT8 length;
}msg_struct_sps_ccl_event_report_cfm;

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
typedef struct msg_struct_i2caries_l2frame_send
{
	UINT8 data[MAX_IHU_MSG_BODY_LENGTH-1];
	UINT8 length;
}msg_struct_i2caries_l2frame_send_t;
typedef struct msg_struct_i2caries_l2frame_rcv
{
	UINT8 data[MAX_IHU_MSG_BODY_LENGTH-1];
	UINT8 length;
}msg_struct_i2caries_l2frame_rcv_t;

//MSG_ID_I2C_CCL_SENSOR_STATUS_RESP
typedef struct msg_struct_i2c_ccl_sensor_status_rep
{
	UINT8 cmdid;
	com_sensor_status_t sensor;
	UINT8 length;
}msg_struct_i2c_ccl_sensor_status_rep_t;

//CAN消息定义
typedef struct msg_struct_canvela_l2frame_send
{
	UINT8 data[MAX_IHU_MSG_BODY_LENGTH-1];
	UINT8 length;
}msg_struct_canvela_l2frame_send_t;
typedef struct msg_struct_canvela_l2frame_rcv
{
	UINT8 data[MAX_IHU_MSG_BODY_LENGTH-1];
	UINT8 length;
}msg_struct_canvela_l2frame_rcv_t;

//DCMI
//MSG_ID_DCMI_CCL_SENSOR_STATUS_RESP
typedef struct msg_struct_dcmi_ccl_sensor_status_rep
{
	UINT8 cmdid;
	com_sensor_status_t sensor;
	UINT8 length;
}msg_struct_dcmi_ccl_sensor_status_rep_t;

//CCL
//MSG_ID_CCL_TO_SPS_OPEN_AUTH_INQ
#define IHU_CCL_BH_CTRL_CMD_BUF_LEN 20
typedef struct msg_struct_ccl_to_sps_open_auth_inq
{
	UINT8 cmdid;
	UINT8 dataBuf[IHU_CCL_BH_CTRL_CMD_BUF_LEN];
	UINT8 length;
}msg_struct_ccl_to_sps_open_auth_inq;
//MSG_ID_CCL_TO_DIDO_CTRL_CMD


//MSG_ID_CCL_DIDO_SENSOR_STATUS_REQ
typedef struct msg_struct_ccl_dido_sensor_status_req
{
	UINT8 cmdid;
	UINT8 length;
}msg_struct_ccl_dido_sensor_status_req_t;

//MSG_ID_CCL_SPS_SENSOR_STATUS_REQ
typedef struct msg_struct_ccl_sps_sensor_status_req
{
	UINT8 cmdid;
	UINT8 length;
}msg_struct_ccl_sps_sensor_status_req_t;

//MSG_ID_CCL_I2C_SENSOR_STATUS_REQ
typedef struct msg_struct_ccl_i2c_sensor_status_req
{
	UINT8 cmdid;
	UINT8 length;
}msg_struct_ccl_i2c_sensor_status_req_t;

//MSG_ID_CCL_DCMI_SENSOR_STATUS_REQ
typedef struct msg_struct_ccl_dcmi_sensor_status_req
{
	UINT8 cmdid;
	UINT8 length;
}msg_struct_ccl_dcmi_sensor_status_req_t;

//MSG_ID_CCL_DIDO_CTRL_CMD
typedef struct msg_struct_ccl_dido_ctrl_cmd
{
	UINT8 cmdid;
	UINT8 length;
}msg_struct_ccl_dido_ctrl_cmd_t;

//MSG_ID_CCL_SPS_CTRL_CMD
typedef struct msg_struct_ccl_sps_ctrl_cmd
{
	UINT8 cmdid;
	UINT8 length;
}msg_struct_ccl_sps_ctrl_cmd_t;

//MSG_ID_CCL_I2C_CTRL_CMD
typedef struct msg_struct_ccl_i2c_ctrl_cmd
{
	UINT8 cmdid;
	UINT8 length;
}msg_struct_ccl_i2c_ctrl_cmd_t;

//MSG_ID_CCL_DCMI_CTRL_CMD
typedef struct msg_struct_ccl_dcmi_ctrl_cmd
{
	UINT8 cmdid;
	UINT8 length;
}msg_struct_ccl_dcmi_ctrl_cmd_t;

//MSG_ID_CCL_SPS_EVENT_REPORT_SEND
typedef struct msg_struct_ccl_sps_event_report_send
{
	UINT8 cmdid;
	com_sensor_status_t sensor;
	UINT8 length;
}msg_struct_ccl_sps_event_report_send_t;



#endif /* L0COMVM_COMMSGCCL_H_ */
