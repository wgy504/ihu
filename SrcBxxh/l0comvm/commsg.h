/*
 * commsg.h
 *
 *  Created on: 2016年1月3日
 *      Author: test
 */

#ifndef L0COMVM_COMMSG_H_
#define L0COMVM_COMMSG_H_

#include "comtype.h"
#include "sysdim.h"

#include <stdio.h>

#include "rwip_config.h"
#include "ke_msg.h"
#include <stdint.h>
#include "ke_task.h"
#include "time.h"
//#include "da14580_config.h"

//2. 公共消息结构体定义
//Under normal case, 1024Byte shall be enough for internal message communciation purpose.
//If not enough, need modify here to enlarge
#define MAX_IHU_MSG_BUF_LENGTH MAX_IHU_MSG_BODY_LENGTH-5
typedef struct IhuMsgSruct
{
	UINT16 msgType;
	UINT8 dest_id;
	UINT8 src_id;
	UINT8 msgLen;
	INT8 msgBody[MAX_IHU_MSG_BODY_LENGTH];
}IhuMsgSruct_t;


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
	MSG_ID_COM_MIN = KE_FIRST_MSG(TASK_VMDA1458X), //Starting point

	//VMDA-BLE所需要的消息
	MSG_BLEAPP_VMDASHELL_INIT,  //ZJL, add to trigger VMDASHELL init
	MSG_BLEAPP_VMDASHELL_DISCONNECT,  //ZJL, add to trigger VMDA SHELL disconnect
	MSG_SPS_VMDASHELL_BLE_DATA_DL_RCV,  //send BLE received data to VMDASHELL task
	MSG_APP_SERVER_UART1_DATA_RECEIVED,  //APP_SPS_SCHEDULER send UART1 data to MODBUS	
	
	//正常的消息ID
	MSG_ID_COM_INIT,
	MSG_ID_COM_INIT_FB,
	MSG_ID_COM_RESTART,  //L1->重新初始化上下文数据
	MSG_ID_COM_TIME_OUT,
	MSG_ID_COM_STOP,
	
	//VMDASHELL
	MSG_ID_VMDASHELL_TIMER_1S_PERIOD,
	MSG_ID_VMDASHELL_ASYLIBRA_BLE_CON,
	MSG_ID_VMDASHELL_ASYLIBRA_BLE_DISC,
	MSG_ID_VMDASHELL_ASYLIBRA_DATA_DL,
	
	//ASYLIBRA任务
	MSG_ID_ASYLIBRA_EMC_DISC,
	MSG_ID_ASYLIBRA_EMC_CON,
	MSG_ID_ASYLIBRA_EMC_DLK_BLOCK,
	MSG_ID_ASYLIBRA_EMC_DLK_UNBLOCK,
	MSG_ID_ASYLIBRA_EMC_PUSH_CMD,
	MSG_ID_ASYLIBRA_VMDASHELL_DATA_UL,

	//AIRKISS任务模块
	
	//ADCARIES任务模块
	
	//EMC任务模块
	MSG_ID_EMC_ASYLIBRA_DATA_REQ,
	//MSG_ID_EMC_ASYLIBRA_DATA_REPORT,	
	//MSG_ID_EMC_ASYLIBRA_TIME_SYNC_REQ,
	//MSG_ID_EMC_ASYLIBRA_EQU_INFO_SYNC,


	//老旧消息的定义
	//INIT, SPS -> ASYLIBRA
	MSG_ASYLIBRA_INIT,	
	//BLE APP/SPS -> ASYLIBRA
	MSG_ASYLIBRA_BLE_CONNNECTD,  
	//AUTH REQ, ASYLIBRA->ASYLIBRA
	MSG_ASYLIBRA_AUTH_REQ,
	//AUTH RESP, ASYLIBRA->ASYLIBRA
	MSG_ASYLIBRA_AUTH_RESP,
	//INIT REQ, ASYLIBRA->ASYLIBRA
	MSG_ASYLIBRA_INIT_REQ,
	//INIT RESP, ASYLIBRA->ASYLIBRA
	MSG_ASYLIBRA_INIT_RESP,
	//ASYLIBRA CONNECTION READY, ASYLIBRA -> L3 WXAPP
	MSG_ASYLIBRA_CONNECT_READY,
	//DATA PUSH, ASYLIBRA -> L3 WXAPP
	MSG_ASYLIBRA_DATA_PUSH,
	//DATA REQ, L3 WXAPP -> ASYLIBRA
	MSG_ASYLIBRA_DATA_REQ,
	//DATA RESP, ASYLIBRA -> L3 WXAPP
	MSG_ASYLIBRA_DATA_RESP,
	//AUTH WAIT TIMER, ASYLIBRA->ASYLIBRA
	MSG_ASYLIBRA_AUTH_WAIT_TIMER,
	MSG_ASYLIBRA_DISCONNECT_TIGGER_L3,
	MSG_ASYLIBRA_SEND_BLE_DATA_TIMER,

	//INIT, ASYLIBRA -> WXAPP
	MSG_WXAPP_3MIN_TIMEOUT,
	MSG_WXAPP_5SECOND_TIMEOUT, //用户FLASH数据的发送
	//DATA_REQ, WXAPP -> ASYLIBRA
	MSG_WXAPP_DATA_REQ_EMC,
	MSG_WXAPP_PERIOD_REPORT_EMC_TRIGGER,
	MSG_WXAPP_DATA_REQ_TIME_SYNC,  //时钟同步消息
	MSG_WXAPP_EQUIPMENT_INFO_SYNC, //设备软硬件及版本信息
	
	//END FLAG
	MSG_ID_COM_MAX, //Ending point

}; //end of IHU_INTER_TASK_MSG_ID
#define MSG_ID_END 0xFF  //跟MASK_MSGID_NUM_IN_ONE_TASK设置息息相关，不能随便改动
#define MSG_ID_INVALID 0xFFFFFFFF

/*
 *	
*  SPS3.150.2例子，为了适应VMDA，需要定义的结构体
 *
 */
// keep same structure as gattc_write_cmd_ind, to reduce memory copy
typedef struct msg_struct_sps_vmda1458x_ble_data_dl_rcv
{
    /// Handle of the attribute that has to be written
    uint16_t handle;
    /// Data length to be written
    uint32_t length;
    /// offset at which the data has to be written
    uint16_t offset;
    /// Destination task shall send back a write response command if true.
    bool response;
    ///Informs that it's last request of a multiple prepare write request.
    bool last;
    /// Data to be written in attribute database
    uint8_t  value[__ARRAY_EMPTY];
}msg_struct_sps_vmda1458x_ble_data_dl_rcv_t;

//UART1½ÓÊÕµ½µÄÊý¾Ý£¬·¢ËÍ¸øMODBUS
#define APP_SPS_UART1_DATA_LENGTH_MAX_BUF 50
#define APP_SPS_UART1_DATA_LENGTH_MAX_MSG 8
struct msg_struct_app_server_uart1_data_received
{
    uint32_t length;
		uint8_t data[APP_SPS_UART1_DATA_LENGTH_MAX_MSG];	
};
typedef struct {
	int    	size;
	int	    readIdx;
	int	    writeIdx;
	uint8_t data[APP_SPS_UART1_DATA_LENGTH_MAX_BUF];
}UartRingBuffer;

typedef struct msg_struct_bleapp_vmdashell_init
{
    uint32_t length;
}msg_struct_bleapp_vmdashell_init_t;
typedef struct msg_struct_bleapp_vmda1458x_disconnect
{
    /// length
    uint32_t length;
}msg_struct_bleapp_vmda1458x_disconnect_t;


/*
 *	
 *  VMDASHELL与底层BLEAPP相关的消息结构体定义
 *
 */
//VMDA消息定义
typedef struct msg_struct_vmdashell_1s_period_timtout
{
	UINT8 length;
}msg_struct_vmdashell_1s_period_timtout_t;

/*
 *	
 *  VMDA以及之上的消息结构体定义
 *
 */

//公共结构定义
typedef struct com_gps_pos //
{
	UINT32 gpsx;
	UINT32 gpsy;
	UINT32 gpsz;
}com_gps_pos_t;

typedef struct  sensor_emc_data_element //
{
	UINT32 equipid;
	UINT8  dataFormat;
	UINT32 emcValue;
	com_gps_pos_t gps;
	UINT32 timeStamp;
	UINT32 nTimes;
	UINT8 onOffLineFlag;
}sensor_emc_data_element_t;

typedef struct IhuL3DataFormatTag
{
	UINT8 UserCmdId;
	struct timeSync_t
	{
		UINT32 timeStamp;
	}timeSync;
	struct equInfo_t
	{
		UINT8 mac[6];
		UINT8 hwType;
		UINT16 hwVer;
		UINT8 swRel;
		UINT16 swVer;
	}equInfo;
	struct emcInfo_t
	{
		UINT16 emcValue;
		UINT8 timeYmd[6];
		UINT16 nTimes;
	}emcInfo;	
}IhuL3DataFormatTag_t;

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

//VMDA消息定义
typedef struct  msg_struct_vmdashell_asylibra_ble_con //
{
	UINT8 length;
}msg_struct_vmdashell_asylibra_ble_con_t;
typedef struct  msg_struct_vmdashell_asylibra_ble_disc //
{
	UINT8 length;
}msg_struct_vmdashell_asylibra_ble_disc_t;

#define MSG_BUF_MAX_LENGTH_ASY_DATA_GENERAL MAX_IHU_MSG_BODY_LENGTH-4
typedef struct  msg_struct_vmdashell_asylibra_data_dl //
{
	UINT8 length;
	INT8 dataBuf[MSG_BUF_MAX_LENGTH_ASY_DATA_GENERAL];
}msg_struct_vmdashell_asylibra_data_dl_t;

//ASYLIBRA消息定义
typedef struct  msg_struct_asylibra_emc_disc //
{
	UINT8 length;
}msg_struct_asylibra_emc_disc_t;
typedef struct  msg_struct_asylibra_emc_con //
{
	UINT8 length;
}msg_struct_asylibra_emc_con_t;
typedef struct  msg_struct_asylibra_emc_dlk_block //
{
	UINT8 length;
}msg_struct_asylibra_emc_dlk_block_t;
typedef struct  msg_struct_asylibra_emc_dlk_unblock //
{
	UINT8 length;
}msg_struct_asylibra_emc_dlk_unblock_t;
typedef struct  msg_struct_asylibra_emc_push_cmd //
{
	UINT16 EmDeviceDataType;
	UINT8 length;
	UINT8 l3dataLen;
	UINT8 l3Data[MSG_BUF_MAX_LENGTH_ASY_DATA_GENERAL-10];
}msg_struct_asylibra_emc_push_cmd_t;

typedef struct  msg_struct_vmdashell_asylibra_data_ul //
{
	UINT8 length;
	INT8 dataBuf[MSG_BUF_MAX_LENGTH_ASY_DATA_GENERAL];
}msg_struct_vmdashell_asylibra_data_ul_t;

//EMC消息定义
typedef struct  msg_struct_emc_asylibra_data_req //
{
	UINT16 EmDeviceDataType;
	UINT8 length;
	UINT8 l3dataLen;
	UINT8 l3Data[MSG_BUF_MAX_LENGTH_ASY_DATA_GENERAL-4];
}msg_struct_emc_asylibra_data_req_t;
//typedef struct  msg_struct_emc_asylibra_data_report //
//{
//	UINT8  usercmdid;
//	UINT8  useroptid;
//	UINT8  cmdIdBackType; //指明是瞬时，还是周期性读数
//	sensor_emc_data_element_t emc;
//	UINT16 EmDeviceDataType;
//	UINT8 length;
//}msg_struct_emc_asylibra_data_report_t;
//typedef struct  msg_struct_emc_asylibra_time_sync_req //
//{
//	UINT8 length;
//}msg_struct_emc_asylibra_time_sync_req_t;
//typedef struct  msg_struct_emc_asylibra_equ_info_sync //
//{
//	UINT8 hw_uuid[6];
//	UINT8 hw_type;
//	UINT8 hw_version;
//	UINT8 sw_release;
//	UINT8 sw_delivery;		
//	UINT8 length;
//}msg_struct_emc_asylibra_equ_info_sync_t;













typedef struct msg_struct_asylibra_ble_connected //用于定时结束时，指示BLE连接态
{
	uint16_t length;
}msg_struct_asylibra_ble_connected_t;

//struct msg_struct_asylibra_auth_resp
//{
//	/// Handle of the attribute that has to be written
//	uint16_t handle;
//	/// Data length to be written
//	uint16_t length;
//	/// offset at which the data has to be written
//	uint16_t offset;
//	/// Destination task shall send back a write response command if true.
//	bool response;
//	///Informs that it's last request of a multiple prepare write request.
//	bool last;
//	/// Data to be written in attribute database
//	uint8_t  value[__ARRAY_EMPTY];
//};
//struct msg_struct_asylibra_init_resp
//{
//	/// Handle of the attribute that has to be written
//	uint16_t handle;
//	/// Data length to be written
//	uint16_t length;
//	/// offset at which the data has to be written
//	uint16_t offset;
//	/// Destination task shall send back a write response command if true.
//	bool response;
//	///Informs that it's last request of a multiple prepare write request.
//	bool last;
//	/// Data to be written in attribute database
//	uint8_t  value[__ARRAY_EMPTY];
//};

struct msg_struct_asylibra_connect_ready
{
    uint16_t length;
};
#define ASY_DATA_PUSH_LENGTH_MAX 20
struct msg_struct_asylibra_data_push
{
	uint8_t UserCmdId;
	uint16_t EmDeviceDataType;
	uint16_t length;
	uint8_t data[ASY_DATA_PUSH_LENGTH_MAX];
};
//该消息将保留在ASYLIBRA内部，不再用于WXAPP使用
//struct msg_struct_asylibra_data_resp
//{
//	/// Handle of the attribute that has to be written
//	uint16_t handle;
//	/// Data length to be written
//	uint16_t length;
//	/// offset at which the data has to be written
//	uint16_t offset;
//	/// Destination task shall send back a write response command if true.
//	bool response;
//	///Informs that it's last request of a multiple prepare write request.
//	bool last;
//	/// Data to be written in attribute database
//	uint8_t  value[__ARRAY_EMPTY];
//};

///////////////////////////////////////////////////////////////////////////////////////
/* MYC 2015/10/25, simplify the BLE to AirSync Message, after using Buffer_Man APIs */
struct msg_struct_asylibra_auth_resp
{
    uint8_t    start;
	  uint8_t		 version;
		uint16_t	 len;
	  uint16_t   msgId;
	  uint16_t   seqNum;
		/// Data to be written in attribute database
		uint8_t  value[__ARRAY_EMPTY];
};
struct msg_struct_asylibra_init_resp
{
    uint8_t    start;
	  uint8_t		 version;
		uint16_t	 len;
	  uint16_t   msgId;
	  uint16_t   seqNum;
		uint8_t  		value[__ARRAY_EMPTY];
};

struct msg_struct_asylibra_data_resp
{
    uint8_t    start;
	  uint8_t		 version;
		uint16_t	 len;
	  uint16_t   msgId;
	  uint16_t   seqNum;
		uint8_t  value[__ARRAY_EMPTY];
};
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////


struct msg_struct_asylibra_disconnect_tigger_l3
{
    uint16_t length;
};
struct msg_struct_asylibra_send_ble_data_timer
{
    uint16_t length;
};

//All L3 message structure defined here
struct msg_struct_3min_timtout
{
    uint16_t length;
};
struct msg_struct_5second_timtout
{
    uint16_t length;
};
struct msg_struct_period_report_emc_trigger
{
    uint16_t length;
};
//以解开的原始信息进行组织，二进制码流的事情，留待ASYLIBRA完成
struct msg_struct_wxapp_data_req_emc
{
	uint8_t usercmdid;
  uint16_t length;
	uint16_t emc_value;
	uint8_t year;
	uint8_t month;
	uint8_t day;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
	unsigned int gps_x;
	unsigned int gps_y;
	uint16_t ntimes;
	uint16_t EmDeviceDataType;
};
//时钟请求
struct msg_struct_wxapp_data_req_time_sync
{
  uint16_t length;
};
//设备信息同步
struct msg_struct_wxapp_equipment_info_sync
{
  uint16_t length;
	uint8_t hw_uuid[6];
	uint8_t hw_type;
	uint16_t hw_version;
	uint8_t sw_release;
	uint16_t sw_delivery;	
};

//这个长度应该跟ASYLIBRA中的DATA_PUSH消息最长长度保持一致
#define WXAPP_DATA_PUSH_LENGTH_MAX 20
struct msg_struct_wxapp_modbus_data_push
{
	uint8_t UserCmdId;
	uint16_t EmDeviceDataType;
	uint16_t length;
	uint8_t data[WXAPP_DATA_PUSH_LENGTH_MAX];	
};
struct msg_struct_wxapp_modbus_disconnect
{
  uint16_t length;
};


//来源于epb_MmBp.c，定义了所有ASYLIBRA中IE的标识
//如果改变这里，需要考虑一旦复用了epb_MmBp.c，则有编解码不一致的风险
#define TAG_BaseResponse_ErrCode												  0x08
#define TAG_BaseResponse_ErrMsg													  0x12
#define TAG_AuthRequest_BaseRequest												0x0a
#define TAG_AuthRequest_Md5DeviceTypeAndDeviceId					0x12
#define TAG_AuthRequest_ProtoVersion											0x18
#define TAG_AuthRequest_AuthProto												  0x20
#define TAG_AuthRequest_AuthMethod												0x28
#define TAG_AuthRequest_AesSign													  0x32
#define TAG_AuthRequest_MacAddress												0x3a
#define TAG_AuthRequest_TimeZone												  0x52
#define TAG_AuthRequest_Language												  0x5a
#define TAG_AuthRequest_DeviceName												0x62
#define TAG_AuthResponse_BaseResponse											0x0a
#define TAG_AuthResponse_AesSessionKey										0x12
#define TAG_InitRequest_BaseRequest												0x0a
#define TAG_InitRequest_RespFieldFilter										0x12
#define TAG_InitRequest_Challenge												  0x1a
#define TAG_InitResponse_BaseResponse											0x0a
#define TAG_InitResponse_UserIdHigh												0x10
#define TAG_InitResponse_UserIdLow												0x18
#define TAG_InitResponse_ChalleangeAnswer									0x20
#define TAG_InitResponse_InitScence												0x28
#define TAG_InitResponse_AutoSyncMaxDurationSecond				0x30
#define TAG_InitResponse_UserNickName											0x5a
#define TAG_InitResponse_PlatformType											0x60
#define TAG_InitResponse_Model													  0x6a
#define TAG_InitResponse_Os														    0x72
#define TAG_InitResponse_Time													    0x78
#define TAG_InitResponse_TimeZone												  0x8001
#define TAG_InitResponse_TimeString												0x8a01
#define TAG_SendDataRequest_BaseRequest									  0x0a
#define TAG_SendDataRequest_Data												  0x12
#define TAG_SendDataRequest_Type												  0x18
#define TAG_SendDataResponse_BaseResponse								  0x0a
#define TAG_SendDataResponse_Data												  0x12
#define TAG_RecvDataPush_BasePush												  0x0a
#define TAG_RecvDataPush_Data													    0x12
#define TAG_RecvDataPush_Type													    0x18
#define TAG_SwitchViewPush_BasePush												0x0a
#define TAG_SwitchViewPush_SwitchViewOp										0x10
#define TAG_SwitchViewPush_ViewId												  0x18
#define TAG_SwitchBackgroudPush_BasePush									0x0a
#define TAG_SwitchBackgroudPush_SwitchBackgroundOp				0x10

#define MSG_PAYLOAD_LEN_ECI_RESP_AUTH											(0x000E)  
#define MSG_PAYLOAD_LEN_ECI_RESP_INIT											(0x0000)
#define MSG_PAYLOAD_LEN_ECI_RESP_SENDDATA									(0x0000)
#define MSG_PAYLOAD_LEN_ECI_RESP_RECVDATA									(0x0000)

/*
**** send auth response ****
data len = 14
data dump = FE 01 00 0E 4E 21 00 01 0A 02 08 00 12 00 

**** send init request response ****
data len = 70
data dump = FE 01 00 46 4E 23 00 02 0A 02 08 00 10 B4 24 18 F8 AC 01 20 B9 AE 94 85 06 5A 14 57 65 43 68 61 74 20 54 65 73 74 20 62 79 20 6D 61 74 74 21 8A 01 14 57 65 43 68 61 74 20 54 65 73 74 20 62 79 20 6D 61 74 74 21 

**** send SendData Response(echo request) ****
data len = 36
data dump = FE 01 00 24 4E 22 12 37 0A 02 08 00 12 16 0A 00 12 10 20 0E 00 06 00 01 02 03 04 05 01 02 03 04 05 06 18 10 

**** send ManufactureSvr Push ****
data len = 22
data dump = FE 01 00 16 75 31 00 00 0A 00 12 08 31 32 33 34 35 36 37 38 18 00 

**** send Html Push ****
data len = 23
data dump = FE 01 00 17 75 31 00 00 0A 00 12 08 31 32 33 34 35 36 37 38 18 91 4E 

**** send wxWristBand Push ****
data len = 14
data dump = FE 01 00 0E 75 31 00 00 0A 00 12 00 18 01 

**** send EnterDeviceChatView Push ****
data len = 14
data dump = FE 01 00 0E 75 32 00 00 0A 00 10 01 18 01 

**** send Exit ChatView Push ****
data len = 14
data dump = FE 01 00 0E 75 32 00 00 0A 00 10 02 18 01 

**** send Enter HtmlChatView Push ****
data len = 14
data dump = FE 01 00 0E 75 32 00 00 0A 00 10 01 18 02 

**** send Exit HtmlChatView Push ****
data len = 14
data dump = FE 01 00 0E 75 32 00 00 0A 00 10 02 18 02 

**** send enterBackground Push ****
data len = 12
data dump = FE 01 00 0C 75 33 00 00 0A 00 10 01 

**** send enterForground Push ****
data len = 12
data dump = FE 01 00 0C 75 33 00 00 0A 00 10 02 

**** send enterSleep Push ****
data len = 12
data dump = FE 01 00 0C 75 33 00 00 0A 00 10 03 

*/

#endif /* L0COMVM_COMMSG_H_ */
