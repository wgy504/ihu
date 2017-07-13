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

#pragma pack (1) //强制1字节对齐

//遗留特殊的定义
//typedef UINT8           		bool;
typedef UINT8           		BOOL;  

//2. 公共消息结构体定义
//Under normal case, 1024Byte shall be enough for internal message communciation purpose.
//If not enough, need modify here to enlarge
#define IHU_SYSMSG_COM_BODY_LEN_MAX 			IHU_SYSDIM_MSG_BODY_LEN_MAX+6
#define IHU_SYSMSG_BH_BUF_BODY_LEN_MAX 		IHU_SYSDIM_MSG_BODY_LEN_MAX-4  //跟L2FRAME长度保持同步，都是比消息BUFFER小4字节，以便容纳进消息BODY之中
typedef struct IhuMsgSruct
{
	UINT16 msgType;
	UINT8 dest_id;
	UINT8 src_id;
	UINT16 msgLen;
	INT8 msgBody[IHU_SYSDIM_MSG_BODY_LEN_MAX];
}IhuMsgSruct_t;

#define IHU_SYSMSG_TASK_THREAD_PRIO  10          //priority of the main loop de 1 a 99 max

/*
 *
 * 每定义一个新消息，请去修改vmlayer.c中的变量zIhuVmCtrMsgStaticCfg.name[]，不然TRACE会出现消息内容解析的错误
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

	//SPS
	MSG_ID_SPS_L2FRAME_SEND,
	MSG_ID_SPS_L2FRAME_RCV,
	
	//CAN
	MSG_ID_CAN_L2FRAME_SEND,
	MSG_ID_CAN_L2FRAME_RCV,

	//L3IAP
	MSG_ID_IAP_SW_INVENTORY_REPORT,   		// IHU -> HCU
	MSG_ID_IAP_SW_INVENTORY_CONFIRM,  		// HCU -> IHU
	MSG_ID_IAP_SW_PACKAGE_REPORT,   	    // IHU -> HCU
	MSG_ID_IAP_SW_PACKAGE_CONFIRM,  	    // HCU -> IHU
	
	//END FLAG
	MSG_ID_COM_MAX, //Ending point

}; //end of IHU_INTER_TASK_MSG_ID
#define MSG_ID_END 0xFF  //跟IHU_SYSDIM_MSGID_MASK_SET设置息息相关，不能随便改动
#define MSG_ID_INVALID 0xFFFFFFFF

//公共结构定义
typedef struct com_gps_pos //
{
	UINT32 gpsx;
	UINT32 gpsy;
	UINT32 gpsz;
}com_gps_pos_t;


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


//SPS消息定义
typedef struct msg_struct_spsvirgo_l2frame_send
{
	UINT8 data[IHU_SYSDIM_L2FRAME_MSG_BODY_LEN_MAX];
	UINT16 length;
}msg_struct_spsvirgo_l2frame_send_t;
typedef struct msg_struct_spsvirgo_l2frame_rcv
{
	UINT16 length;
	UINT8 data[IHU_SYSDIM_L2FRAME_MSG_BODY_LEN_MAX];
}msg_struct_spsvirgo_l2frame_rcv_t;

//CAN消息定义
//MSG_ID_CAN_L2FRAME_SEND,
typedef struct msg_struct_canvela_l2frame_send
{
	UINT8 data[IHU_SYSDIM_L2FRAME_MSG_BODY_LEN_MAX];
	UINT16 length;
}msg_struct_canvela_l2frame_send_t;

//MSG_ID_CAN_L2FRAME_RCV,
typedef struct msg_struct_canvela_l2frame_rcv
{
	UINT8 data[IHU_SYSDIM_L2FRAME_MSG_BODY_LEN_MAX];
	UINT16 length;
}msg_struct_canvela_l2frame_rcv_t;


//L3IAP
//MSG_ID_L3IAP_CANVELA_INVENTORY_REPORT,
typedef struct msg_struct_l3iap_canvela_inventory_report
{
	UINT16 length;
}msg_struct_l3iap_canvela_inventory_report_t;

//MSG_ID_L3IAP_CANVELA_SW_PACKAGE_REPORT,
typedef struct msg_struct_l3iap_canvela_sw_package_report
{
	UINT16 length;
}msg_struct_l3iap_canvela_sw_package_report_t;

//MSG_ID_CANVELA_L3IAP_INVENTORY_CONFIRM,
typedef struct msg_struct_canvela_l3iap_inventory_confirm
{
	UINT16 length;
}msg_struct_canvela_l3iap_inventory_confirm_t;

//MSG_ID_CANVELA_L3IAP_SW_PACKAGE_CONFIRM,
typedef struct msg_struct_canvela_l3iap_sw_package_confirm
{
	UINT16 length;
}msg_struct_canvela_l3iap_sw_package_confirm_t;

/*
** IAP System Message inside L2 Frame
*/

//msgid:
//	MSG_ID_IAP_SW_INVENTORY_REPORT,   		// IHU -> HCU
//	MSG_ID_IAP_SW_INVENTORY_CONFIRM,  		// HCU -> IHU
//	MSG_ID_IAP_SW_PACKAGE_REPORT,   	    // IHU -> HCU
//	MSG_ID_IAP_SW_PACKAGE_CONFIRM,  	    // HCU -> IHU

/*
**	MSG_ID_L3IAP_WMC_MSH_HEADER
*/

/* 
** THIS IS DEFINED BY LI HAO C, TO BE MERGED
*/
//typedef struct SysEngParElementHwBurnPhyIdAddr
//{
//	char  	equLable[20];
//	UINT16 	hwType;
//	UINT16 	hwPemId;
//	UINT16 	swRelId;
//	UINT16 	swVerId;
//	UINT8  	swUpgradeFlag;
//  UINT32  spare1;
//	UINT32  facLoadAddr;
//  UINT16  facLoadSwRel;
//  UINT16  facLoadSwVer;
//  UINT16  facLoadCheckSum;
//  UINT16  facLoadValid;
//  UINT32  spare2;
//	UINT32  bootLoad1Addr;
//  UINT16  bootLoad1SwRel;
//  UINT16  bootLoad1SwVer;
//  UINT16  bootLoad1CheckSum;
//  UINT16  bootLoad1Valid;
//  UINT32  spare3;
//	UINT8 	rsv2;
//	UINT8 	rsv3;
//}SysEngParElementHwBurnPhyIdAddr_t;

//typedef struct SysEngParElementHwBurnPhyIdAddr
//{
//	char  	equLable[20];
//	UINT16 	hwType;
//	UINT16 	hwPemId;
//	UINT8  	swUpgradeFlag;
//	UINT8 	swUpgPollId;
//	UINT8 	bootIndex;
//	UINT8 	bootAreaMax;  
//	UINT32  HwNodeIdPart1;    //32
//	UINT32	HwNodeIdPart2;		 //32
//	UINT32  facLoadAddr; 
//	UINT16	facLoadSwRel;
//	UINT16	facLoadSwVer;	
//	UINT16 	facLoadCheckSum;
//	UINT16  facLoadValid;
//	UINT32  bootLoad1Addr;
//	UINT16 	bootLoad1RelId;
//	UINT16 	bootLoad1VerId;
//	UINT16 	bootLoad1CheckSum;
//	UINT16  bootLoad1Valid;
//	UINT32	spare3;   //32
//	UINT32  bootLoad2Addr;
//	UINT16 	bootLoad2RelId;
//	UINT16 	bootLoad2VerId;
//	UINT16 	bootLoad2CheckSum;
//	UINT16  bootLoad2Valid;
//	UINT32	spare4;
//	UINT32  bootLoad3Addr;
//	UINT16 	bootLoad3RelId;
//	UINT16 	bootLoad3VerId;
//	UINT16 	bootLoad3CheckSum;
//	UINT16  bootLoad3Valid;
//	UINT32  spare5;       //32
//	UINT8   cipherKey[16];
//	UINT8 	rsv[16];   		//32
//}SysEngParElementHwBurnPhyIdAddr_t;

typedef struct msg_struct_l3iap_msg_header
{
	UINT16 msgid;
	UINT16 length;
}msg_struct_l3iap_msg_header_t;


//MSG_ID_IAP_SW_INVENTORY_REPORT,
typedef struct msg_struct_l3iap_sw_inventory_report
{
	UINT16  msgid;
	UINT16  length;
	UINT16  hwType;
	UINT16  hwId;
	UINT16  swRel;
	UINT16  swVer;
	UINT8   upgradeFlag;
	//UINT32  timeStamp;
}msg_struct_l3iap_sw_upgrade_report_t;

//MSG_ID_IAP_SW_INVENTORY_CONFIRM,
#define IAP_SW_UPGRADE_TYPE_JUMP_TO_APP_LOAD         (1) //NO Upgrade
#define IAP_SW_UPGRADE_TYPE_JUMP_TO_FACTORY_LOAD     (2) //NO Upgrade
#define IAP_SW_UPGRADE_TYPE_UPGRADE_APP_LOAD         (3) //Upgrade AppLoad
#define IAP_SW_UPGRADE_TYPE_UPGRADE_FACTORY_LOAD     (4) //Upgrade FacoryLoad

typedef struct msg_struct_l3iap_sw_inventory_confirm
{
//	UINT16  msgid;
//	UINT16  length;
//	UINT16 	swUpgradeType;					       //APP or FACTORY, or ... same as msg_struct_l3iap_iap_sw_upgrade_req
//	UINT16 	swRelId;
//	UINT16 	swVerId;
//	UINT16 	swCheckSum;
//	UINT32  swTotalLengthInBytes;
	
		UINT16  msgid;
		UINT16  length;
		UINT16  swRel;
		UINT16  swVer;
		UINT8   upgradeFlag;
		UINT16 	swCheckSum;
		UINT32  swTotalLengthInBytes;
}msg_struct_l3iap_sw_inventory_confirm_t;

//MSG_ID_IAP_SW_PACKAGE_REPORT,

#define IAP_SW_UPGRADE_PREPARE_STATUS_ABOUT_TO_JUMP_TO_APP              (1) //NO Upgrade
#define IAP_SW_UPGRADE_PREPARE_STATUS_ABOUT_TO_JUMP_TO_FACTORY_LOAD     (2) //NO Upgrade
#define IAP_SW_UPGRADE_PREPARE_STATUS_APP_LOAD_FLASH_ERASED_OK          (3) //Upgrade AppLoad
#define IAP_SW_UPGRADE_PREPARE_STATUS_FACTORY_LOAD_FLASH_ERASED_OK      (4) //Upgrade FacoryLoad

/*
Frame structure:

1. L2 CAN FRAME Header (4 bytes)
typedef struct IHU_HUITP_L2FRAME_STD_frame_header
{
		uint8_t start;
		uint8_t chksum;
		uint16_t len;     // the length including the size of header
}IHU_HUITP_L2FRAME_STD_frame_header_t;

2. L2 FRAME HEADER (4 bytes)
typedef struct msg_struct_l3iap_msg_header
{
		UINT16 msgid;
		UINT16 length;
}msg_struct_l3iap_msg_header_t;

3. Private Header for each message
	UINT16 	UpgradePrepareStatus;					//APP or FACTORY, or ...
	UINT16 	swRelId;
	UINT16 	swVerId;
	UINT16 	swCheckSum;
	UINT32  swTotalLengthInBytes;
	UINT32	swSegmentIndex;        // 0, 1, 2, ... MaxLoadSegmentIndex, start from 0, 
		
*/
typedef struct msg_struct_l3iap_sw_package_report
{
//	UINT16  msgid;
//	UINT16  length;
//	UINT16 	UpgradePrepareStatus;					//APP or FACTORY, or ...
//	UINT16 	swRelId;
//	UINT16 	swVerId;
//	UINT16 	swCheckSum;
//	UINT32  swTotalLengthInBytes;
//	UINT32	swSegmentIndex;        // 0, 1, 2, ... MaxLoadSegmentIndex, start from 0, 
	UINT16  msgid;
	UINT16  length;
	UINT16  swRelId;
	UINT16  swVerId;
	UINT8   upgradeFlag;
	UINT16  segIndex;
	UINT16  segTotal;
	UINT16  segSplitLen;
}msg_struct_l3iap_sw_package_report_t;

//MSG_ID_IAP_SW_PACKAGE_CONFIRM,
#define MAX_LEN_PER_LOAD_SEGMENT	(256-4-sizeof(msg_struct_l3iap_sw_package_report_t))      //232
	
typedef struct msg_struct_l3iap_sw_package_confirm
{
//	UINT16  msgid;
//	UINT16  length;
//	UINT16 	UpgradePrepareStatus;					//APP or FACTORY, or ...
//	UINT16 	swRelId;
//	UINT16 	swVerId;
//	UINT16 	swCheckSum;
//	UINT32  swTotalLengthInBytes;
//	UINT32	swSegmentIndex;        // 0, 1, 2, ... MaxLoadSegmentIndex, start from 0, 
//	UINT8		swPackageSegmentContent[MAX_LEN_PER_LOAD_SEGMENT];
	UINT16 msgid;
	UINT16 length;
	UINT16 swRelId;
	UINT16 swVerId;
	UINT8  upgradeFlag;
	UINT16 segIndex;
	UINT16 segTotal;
	UINT16 segSplitLen;
	UINT16 segValidLen;
	UINT16 segCheckSum;
	UINT8  swPkgBody[MAX_LEN_PER_LOAD_SEGMENT];
}msg_struct_l3iap_sw_package_confirm_t;




#endif /* L0COMVM_COMMSGCCL_H_ */
