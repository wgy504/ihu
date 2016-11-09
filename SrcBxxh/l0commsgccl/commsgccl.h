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
	MSG_ID_ADC_UL_DATA_PULL_BWD,
	MSG_ID_ADC_DL_CTRL_CMD_REQ,
	MSG_ID_ADC_UL_CTRL_CMD_RESP,
	
	//LED
	
	//UART
	MSG_ID_SPS_L2FRAME_SEND,
	MSG_ID_SPS_L2FRAME_RCV,
	
	//SPI
	MSG_ID_SPI_L2FRAME_SEND,
	MSG_ID_SPI_L2FRAME_RCV,
	MSG_ID_SPI_DL_DATA_PUSH_FWD,
	MSG_ID_SPI_UL_DATA_PULL_BWD,
	MSG_ID_SPI_DL_CTRL_CMD_REQ,
	MSG_ID_SPI_UL_CTRL_CMD_RESP,
		
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

//公共消息定义
typedef struct msg_struct_com_init //
{
	UINT8 length;
}msg_struct_com_init_t;
typedef struct msg_struct_com_init_fb //
{
	UINT8 length;
}msg_struct_com_init_fb_t;
typedef struct  msg_struct_com_restart //
{
	UINT8 length;
}msg_struct_com_restart_t;
typedef struct  msg_struct_com_time_out //
{
	UINT32 timeId;
	UINT8  timeRes;
	UINT8 length;
}msg_struct_com_time_out_t;
typedef struct  msg_struct_com_stop //
{
	UINT8 length;
}msg_struct_com_stop_t;

//VMFO消息定义
typedef struct msg_struct_vmfo_1s_period_timtout
{
	UINT8 length;
}msg_struct_vmfo_1s_period_timtout_t;

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








#endif /* L0COMVM_COMMSGCCL_H_ */
