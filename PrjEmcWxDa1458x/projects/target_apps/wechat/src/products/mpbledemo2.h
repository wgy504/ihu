
#ifndef MPBLEDEMO2
#define MPBLEDEMO2

#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "epb_MmBp.h"
#include "ble_wechat_util.h"

#define CMD_NULL    0
#define CMD_AUTH    1
#define CMD_INIT    2
#define CMD_SENDDAT 3
#define CMD_SENDDAT_TIME_SYNC   4
#define CMD_SENDDAT_EQU_INFO    5
#define CMD_SENDDAT_EMC_REPORT  6
#define CMD_SENDDAT_TMR_REPORT  7
#define CMD_SENDDAT_HMD_REPORT  8


#define DEVICE_TYPE "gh_70c714952b02" 
#define DEVICE_ID  "gh_70c714952b02_8248307502397542f48a3775bcb234d4" //"wechat_dialog"   
#define PROTO_VERSION   0x010004
#define AUTH_PROTO      1

#define MAC_ADDRESS_LENGTH  6

//#define EAM_md5AndNoEnrypt 1     
//#define EAM_md5AndAesEnrypt 1
#define EAM_macNoEncrypt 2

#define DEVICE_KEY {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f};

#ifdef EAM_macNoEncrypt
	#define AUTH_METHOD EAM_macNoEncrypt
	#define MD5_TYPE_AND_ID_LENGTH 0
	#define CIPHER_TEXT_LENGTH 0
#endif

#ifdef EAM_md5AndAesEnrypt
	#define AUTH_METHOD EAM_md5AndAesEnrypt
	#define MD5_TYPE_AND_ID_LENGTH 16
	#define CIPHER_TEXT_LENGTH 16
#endif
#ifdef EAM_md5AndNoEnrypt
	#define AUTH_METHOD             EAM_md5AndNoEnrypt
	#define MD5_TYPE_AND_ID_LENGTH  16
	#define CIPHER_TEXT_LENGTH      0
#endif

#define CHALLENAGE_LENGTH       4

#define MPBLEDEMO2_MAGICCODE_H  0xfe
#define MPBLEDEMO2_MAGICCODE_L  0xcf
#define MPBLEDEMO2_VERSION      0x01
#define SEND_HELLO_WECHAT       "Hello, WeChat!"

/* Hardware Resources define */
#define MPBLEDEMO2_LIGHT        19
#define MPBLEDEMO2_BUTTON_1     17

typedef enum
{
	errorCodeUnpackAuthResp             = 0x9990,
	errorCodeUnpackInitResp             = 0x9991,
	errorCodeUnpackSendDataResp         = 0x9992,
	errorCodeUnpackCtlCmdResp           = 0x9993,
	errorCodeUnpackRecvDataPush         = 0x9994,
	errorCodeUnpackSwitchViewPush       = 0x9995,
	errorCodeUnpackSwitchBackgroundPush = 0x9996,
	errorCodeUnpackErrorDecode          = 0x9997,
} mpbledemo2UnpackErrorCode;

typedef enum
{
	errorCodeProduce = 0x9980,
} mpbledemo2PackErrorCode;


// command ID between device and vendor server
typedef enum
{
	//原始命令
	sendTextReq     = 0x01,    //DEVICE发送到后台
	sendTextResp    = 0x1001,  //DEVICE发送到后台
	openLightPush   = 0x1101,  //后台送到DEVICE
	closeLightPush  = 0x1102,  //后台送到DEVICE
	//EMC
	readEmcInsPush  = 0x2001,  //后台送到DEVICE
	readEmcDataResp = 0x2081,
	//温度
	readTmrInsPush  = 0x2801,  //后台送到DEVICE
	readTmrDataResp = 0x2881,
	//湿度
	readHmdInsPush  = 0x2901,  //后台送到DEVICE
	readHmdDataResp = 0x2981,
	//TimeSync
	readTimeSyncReq= 0xF201,  //DEVICE发到后台
	readTimeSyncResp  = 0xF281,  
	//Equipment Info
	readEquInfoPush = 0xF001,  //后台送到DEVICE
	readEquInfoReq  = 0xF081,	
	
} BleDemo2CmdID;

typedef struct
{
	uint8_t     m_magicCode[2];
	uint16_t    m_version;
	uint16_t    m_totalLength;
	uint16_t    m_cmdid;
	uint16_t    m_seq;
	uint16_t    m_errorCode;
} BlueDemoHead;

typedef struct 
{
	int     cmd;
	CString send_msg;
} mpbledemo2_info;

 typedef struct 
{
	bool wechats_switch_state; //公众账号切换到前台的状态
	bool indication_state;
	bool auth_state;
	bool init_state;
	bool auth_send;
	bool init_send;
	unsigned short send_data_seq;
	unsigned short push_data_seq;
	unsigned short seq; 
}mpbledemo2_state;

typedef void (*mpbledemo2_handler_func)(uint8_t *ptrData, uint32_t lengthInByte);

typedef struct
{
    BleDemo2CmdID               commandCode;
    mpbledemo2_handler_func     handler; 
} MPBLEDEMO2_RECEIVED_CMD_HANDLER_T;

extern data_handler mpbledemo2_data_handler;
extern data_handler *m_mpbledemo2_handler;

extern void mpbledemo2_reset(void);
extern void mpbledemo2_indication_state(bool isEnable);
extern int32_t mpbledemo2_sendData(uint8_t* ptrData, uint32_t lengthInByte);

#endif
