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
#define MAX_IHU_MSG_BUF_LENGTH_CLOUD MAX_IHU_MSG_BODY_LENGTH-6
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
	MSG_ID_DIDO_CCL_EVENT_LOCK_TRIGGER,
	MSG_ID_DIDO_CCL_EVENT_FAULT_TRIGGER,
	MSG_ID_DIDO_CCL_EVENT_STATUS_UPDATE,
	MSG_ID_DIDO_CCL_DOOR_OPEN_EVENT,
	MSG_ID_DIDO_CCL_LOCK_C_DOOR_C_EVENT,

	//SPS
	MSG_ID_SPS_L2FRAME_SEND,
	MSG_ID_SPS_L2FRAME_RCV,
	MSG_ID_SPS_CCL_CLOUD_FB,
	MSG_ID_SPS_CCL_SENSOR_STATUS_RESP,
	MSG_ID_SPS_CCL_EVENT_REPORT_CFM,
	MSG_ID_SPS_CCL_FAULT_REPORT_CFM,
	MSG_ID_SPS_CCL_CLOSE_REPORT_CFM,	
	
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
	MSG_ID_CCL_SPS_OPEN_AUTH_INQ,  	   //后台查询
	MSG_ID_CCL_COM_SENSOR_STATUS_REQ,
	MSG_ID_CCL_COM_CTRL_CMD,
	MSG_ID_CCL_SPS_EVENT_REPORT_SEND,  //周期汇报
	MSG_ID_CCL_SPS_FAULT_REPORT_SEND,  //故障汇报
	MSG_ID_CCL_SPS_CLOSE_REPORT_SEND,  //正常一次开关报告

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
	UINT8 lockTongueState[IHU_CCL_SENSOR_LOCK_NUMBER_MAX];
	UINT8 lockiTriggerState[IHU_CCL_SENSOR_LOCK_NUMBER_MAX];
	UINT8 lockoEnableState[IHU_CCL_SENSOR_LOCK_NUMBER_MAX];
	UINT8 doorState[IHU_CCL_SENSOR_LOCK_NUMBER_MAX];
	UINT8 cameraState[IHU_CCL_SENSOR_LOCK_NUMBER_MAX];
	UINT8 smokeState;
	UINT8 waterState;
	UINT8 fallState;	
	UINT8 shakeState;
	UINT8 batteryState;
	INT16 batteryValue;
	INT16 tempValue;
	INT16 humidValue;
	INT16 rssiValue;
	INT16 rsv1Value;  //两个预留传感器之一
	INT16 rsv2Value;	 //两个预留传感器之一
}com_sensor_status_t;

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

//DIDO
//MSG_ID_DIDO_CCL_SENSOR_STATUS_RESP
typedef struct msg_struct_dido_ccl_sensor_status_rep
{
	UINT8 cmdid;
	com_sensor_status_t sensor;
	UINT16 length;
}msg_struct_dido_ccl_sensor_status_rep_t;

//MSG_ID_DIDO_CCL_EVENT_LOCK_TRIGGER
typedef struct msg_struct_dido_ccl_event_lock_trigger
{
	UINT8 cmdid;
	UINT8 lockid;  //指示是哪一个锁触发的
	UINT16 length;
}msg_struct_dido_ccl_event_lock_trigger_t;

//MSG_ID_DIDO_CCL_EVENT_FAULT_TRIGGER
typedef struct msg_struct_dido_ccl_event_fault_trigger
{
	UINT8 cmdid;
	com_sensor_status_t sensor;
	UINT16 length;
}msg_struct_dido_ccl_event_fault_trigger_t;

//MSG_ID_DIDO_CCL_EVENT_STATUS_UPDATE
typedef struct msg_struct_dido_ccl_status_update
{
	UINT8 cmdid;
	UINT16 length;
}msg_struct_dido_ccl_status_update_t;

//MSG_ID_DIDO_CCL_DOOR_OPEN_EVENT
typedef struct msg_struct_dido_door_open_event
{
	UINT8 cmdid;
	UINT16 length;
}msg_struct_dido_ccl_door_open_event_t;

//MSG_ID_DIDO_CCL_LOCK_C_DOOR_C_EVENT
typedef struct msg_struct_dido_ccl_lock_c_door_c_event
{
	UINT8 cmdid;
	UINT16 length;
}msg_struct_dido_ccl_lock_c_door_c_event_t;

//SPS消息定义
typedef struct msg_struct_spsvirgo_l2frame_send
{
	UINT8 data[IHU_MSG_BODY_L2FRAME_MAX_LEN];
	UINT16 length;
}msg_struct_spsvirgo_l2frame_send_t;
typedef struct msg_struct_spsvirgo_l2frame_rcv
{
	UINT16 length;
	UINT8 data[IHU_MSG_BODY_L2FRAME_MAX_LEN];
}msg_struct_spsvirgo_l2frame_rcv_t;

//MSG_ID_SPS_CCL_CLOUD_FB
typedef struct msg_struct_spsvirgo_ccl_cloud_fb
{
	UINT8 cmdid;
	UINT8 authResult;
	UINT16 length;
}msg_struct_spsvirgo_ccl_cloud_fb_t;

//MSG_ID_SPS_CCL_SENSOR_STATUS_RESP
typedef struct msg_struct_sps_ccl_sensor_status_rep
{
	UINT8 cmdid;
	com_sensor_status_t sensor;
	UINT16 length;
}msg_struct_sps_ccl_sensor_status_rep_t;

//MSG_ID_SPS_CCL_EVENT_REPORT_CFM
typedef struct msg_struct_sps_ccl_event_report_cfm
{
	UINT8 actionFlag;
	UINT16 length;
}msg_struct_sps_ccl_event_report_cfm_t;
#define IHU_CCL_EVENT_REPORT_SEND_SUCCESS 1
#define IHU_CCL_EVENT_REPORT_SEND_FAILURE 2

//MSG_ID_SPS_CCL_FAULT_REPORT_CFM
typedef struct msg_struct_sps_ccl_fault_report_cfm
{
	UINT8 actionFlag;
	UINT16 length;
}msg_struct_sps_ccl_fault_report_cfm_t;
#define IHU_CCL_EVENT_FAULT_SEND_SUCCESS 1
#define IHU_CCL_EVENT_FAULT_SEND_FAILURE 2

//MSG_ID_SPS_CCL_CLOSE_REPORT_CFM
typedef struct msg_struct_sps_ccl_close_report_cfm
{
	UINT8 actionFlag;
	//UINT32 test;
	UINT16 length;
}msg_struct_sps_ccl_close_report_cfm_t;
#define IHU_CCL_EVENT_CLOSE_SEND_SUCCESS 1
#define IHU_CCL_EVENT_CLOSE_SEND_FAILURE 2

//SPI消息定义
//MSG_ID_SPI_L2FRAME_SEND
typedef struct msg_struct_spileo_l2frame_send
{
	UINT8 data[MAX_IHU_MSG_BODY_LENGTH-2];
	UINT16 length;
}msg_struct_spileo_l2frame_send_t;

//MSG_ID_SPI_L2FRAME_RCV
typedef struct msg_struct_spileo_l2frame_rcv
{
	UINT8 data[IHU_MSG_BODY_L2FRAME_MAX_LEN];
	UINT16 length;
}msg_struct_spileo_l2frame_rcv_t;

//I2C消息定义
typedef struct msg_struct_i2caries_l2frame_send
{
	UINT8 data[IHU_MSG_BODY_L2FRAME_MAX_LEN];
	UINT16 length;
}msg_struct_i2caries_l2frame_send_t;
typedef struct msg_struct_i2caries_l2frame_rcv
{
	UINT8 data[IHU_MSG_BODY_L2FRAME_MAX_LEN];
	UINT16 length;
}msg_struct_i2caries_l2frame_rcv_t;

//MSG_ID_I2C_CCL_SENSOR_STATUS_RESP
typedef struct msg_struct_i2c_ccl_sensor_status_rep
{
	UINT8 cmdid;
	com_sensor_status_t sensor;
	UINT16 length;
}msg_struct_i2c_ccl_sensor_status_rep_t;

//CAN消息定义
typedef struct msg_struct_canvela_l2frame_send
{
	UINT8 data[IHU_MSG_BODY_L2FRAME_MAX_LEN];
	UINT16 length;
}msg_struct_canvela_l2frame_send_t;
typedef struct msg_struct_canvela_l2frame_rcv
{
	UINT8 data[IHU_MSG_BODY_L2FRAME_MAX_LEN];
	UINT16 length;
}msg_struct_canvela_l2frame_rcv_t;

//DCMI
//MSG_ID_DCMI_CCL_SENSOR_STATUS_RESP
typedef struct msg_struct_dcmi_ccl_sensor_status_rep
{
	UINT8 cmdid;
	com_sensor_status_t sensor;
	UINT16 length;
}msg_struct_dcmi_ccl_sensor_status_rep_t;

//CCL
//MSG_ID_CCL_SPS_OPEN_AUTH_INQ
#define IHU_CCL_BH_CTRL_CMD_BUF_LEN 20
typedef struct msg_struct_ccl_sps_open_auth_inq
{
	UINT8 cmdid;
	UINT8 dataBuf[IHU_CCL_BH_CTRL_CMD_BUF_LEN];
	UINT16 length;
}msg_struct_ccl_sps_open_auth_inq;

//MSG_ID_CCL_COM_SENSOR_STATUS_REQ
typedef struct msg_struct_ccl_com_sensor_status_req
{
	UINT8 cmdid;
	UINT16 length;
}msg_struct_ccl_com_sensor_status_req_t;

//MSG_ID_CCL_COM_CTRL_CMD
typedef struct msg_struct_ccl_com_ctrl_cmd
{
	UINT8 workmode;
	UINT8 cmdid;
	UINT8 lockid;
	UINT16 length;
}msg_struct_ccl_com_ctrl_cmd_t;

//MSG_ID_CCL_SPS_EVENT_REPORT_SEND
typedef struct msg_struct_ccl_sps_event_report_send
{
	UINT8 cmdid;
	com_sensor_status_t sensor;
	UINT16 length;
}msg_struct_ccl_sps_event_report_send_t;

//MSG_ID_CCL_SPS_FAULT_REPORT_SEND
typedef struct msg_struct_ccl_sps_fault_report_send
{
	UINT8 cause;
	com_sensor_status_t sensor;
	UINT16 length;
}msg_struct_ccl_sps_fault_report_send_t;
#define IHU_CCL_FAULT_CAUSE_SENSOR_WARNING 1
#define IHU_CCL_FAULT_CAUSE_CLOSE_DOOR_TIME_OUT 2

//MSG_ID_CCL_SPS_CLOSE_REPORT_SEND
typedef struct msg_struct_ccl_sps_close_report_send
{
	UINT8 cause;
	UINT16 length;
}msg_struct_ccl_sps_close_report_send_t;
#define IHU_CCL_CLOSE_DOOR_NORMAL 1
#define IHU_CCL_CLOSE_DOOR_NOT_YET_OPEN 2
#define IHU_CCL_CLOSE_DOOR_BY_FAULT 3

//来自底层的SPS数据结构部分
//Used to common all different received data buffer
typedef struct  msg_struct_ccl_com_cloud_data_rx //
{
	UINT16 length;
	char buf[MAX_IHU_MSG_BUF_LENGTH_CLOUD];
}msg_struct_ccl_com_cloud_data_rx_t;


#endif /* L0COMVM_COMMSGCCL_H_ */
