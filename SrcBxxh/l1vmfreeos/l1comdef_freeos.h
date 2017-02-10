/*
 * l1comdef_freeos.h
 *
 *  Created on: 2016年1月3日
 *      Author: test
 */

#ifndef L1VMFREEOS_L1COMDEF_FREEOS_H_
#define L1VMFREEOS_L1COMDEF_FREEOS_H_

#include "vmfreeoslayer.h"

//自定义存储数据结构
//DISC data, 只存储周期性数据
#define IHU_DISC_DATA_SAMPLE_STORAGE_NBR_MAX 5
typedef struct IhuDiscDataSampleStorageArray
{
	UINT32 sid;
	UINT8  sensortype;  //0x20->EMC, 0x25->PM1025, L3UserCmdId
	UINT8  dataFormat;
	UINT32 emcValue;
	UINT8  equipid;
	UINT8  onOffLine;
	UINT32 gpsx;
	UINT32 gpsy;
	UINT32 gpsz;
	UINT32 timestamp;
}IhuDiscDataSampleStorageArray_t;
#define IHU_DISC_DATA_SAMPLE_ONLINE 1
#define IHU_DISC_DATA_SAMPLE_OFFLINE 2
typedef struct IhuDiscDataSampleStorage
{
	UINT32 rdCnt;
	UINT32 wrtCnt;
	UINT32 recordNbr;
	UINT32 offlineNbr;
	UINT32 lastSid;
	IhuDiscDataSampleStorageArray_t recordItem[IHU_DISC_DATA_SAMPLE_STORAGE_NBR_MAX];
}IhuDiscDataSampleStorage_t;

/**************************************************************************************
 *   HUITP标准链路层L2FRAME定义                                                           *
 *************************************************************************************/
//帧结构定义
typedef struct StrHuiFrame
{
    UINT8 	start; //0xFE
    UINT8 	checksum1; //加总头的结果
    UINT16 	len; //0-1500，原则上不能超过系统定义的消息体长度
    UINT8 	msgBody[];
}StrHuiFrame_t;

//BFSC特例
typedef enum
{
	L3PO_bfsc_min = 0,
	L3PO_bfsc_none = 0,
	L3PO_bfsc_data_req = 0x01, //Data Request
	L3PO_bfsc_start_cmd = 0x02, //start
	L3PO_bfsc_stop_cmd = 0x03, //stop
	L3PO_bfsc_data_report = 0x81, //Data Report
	L3PO_bfsc_start_resp = 0x82, //start
	L3PO_bfsc_stop_resp = 0x83, //stop
	L3PO_bfsc_max,
}L3BfscOptIdDef;

/**************************************************************************************
 *   CANITF接口的FRAME格式定义：因为CLOUD/L3BFSC/CANVELA都要用到这个定义                   *
 *   所以必须放在这个进行公共定义                                                          *
 *************************************************************************************/
/*
叶工，测试指令和之前你的测试平台使用一致，没有改变：

重量读取：23 00 65 02 00 00 00 00
自动0点跟踪：23 00 65 09 xx xx xx xx （值设定及含义同modbus协议）
最小灵敏度：23 00 65 0a xx xx xx xx （值设定及含义同modbus协议）
手动清零：23 00 65 0c xx xx xx xx (值设定及含义同modbus协议)
静止检测范围：23 00 65 0d xx xx xx xx （值设定及含义同modbus协议）
静止检测时间：23 00 65 0e xx xx xx xx （值设定及含义同modbus协议）
称量0校准：23 00 65 10 01 00 00 00
称量1kg校准：23 00 65 10 02 00 00 00
称量退出校准：23 00 65 10 00 00 00 00
退出校正的指令要改成：23 00 65 10 03 00 00 00
电机正转：23 00 65 20 01 00 00 00
电机反转：23 00 65 20 02 00 00 00
电机停止：23 00 65 20 03 00 00 00
电机速度：23 00 65 21 xx xx xx xx
量程：23 00 65 25 xx xx xx xx

校准顺序是，首先空称台，然后点零载，
放砝码，然后点满载。然后点退出校准保存。
32位的数据低位在前，例如，23 00 65 0d 01 00 00 00
 */

typedef struct strIhuCanvelaCmdFrame
{
	UINT8 bfscCmdPrefixH;
	UINT8 bfscCmdPrefixL;
	UINT8 bfscCmdId;
	UINT8 bfscOptId;
	UINT8 bfscOptPar;
	UINT8 bfscPar1;
	UINT8 bfscPar2;
	UINT8 bfscPar3;
}strIhuCanvelaCmdFrame_t; 

#define IHU_CANVELA_CMD_PREFIXL 0x00
#define IHU_CANVELA_CMD_BFSC_ID 0x65
typedef enum
{
	IHU_CANVELA_PREFIXH_none = 0,
	IHU_CANVELA_PREFIXH_min = 0x22,
	IHU_CANVELA_PREFIXH_motor_ctrl = 0x23,
	IHU_CANVELA_PREFIXH_motor_resp = 0x60,
	IHU_CANVELA_PREFIXH_ws_ctrl = 0x40,
	IHU_CANVELA_PREFIXH_ws_resp = 0x43,
	IHU_CANVELA_PREFIXH_node_ctrl = 0x50,	
	IHU_CANVELA_PREFIXH_node_resp = 0x51,	
	IHU_CANVELA_PREFIXH_heart_beat = 0x7F,  //心跳
	IHU_CANVELA_PREFIXH_max,
	IHU_CANVELA_PREFIXH_invalid = 0xFF,
}IhuCanvelaPrefixHCmdidEnmu;

//不同的控制命令
typedef enum
{
	IHU_CANVELA_OPTID_none = 0,
	IHU_CANVELA_OPTID_min = 0x01,
	IHU_CANVELA_OPTID_wegith_read = 0x02,  //重量读取
	IHU_CANVELA_OPTID_auto_zero_track = 0x09,  //（值设定及含义同modbus协议） 自动0点跟踪
	IHU_CANVELA_OPTID_min_sensitivity = 0x0A,  //（值设定及含义同modbus协议） 最小灵敏度
	IHU_CANVELA_OPTID_manual_set_zero = 0x0C,  //（值设定及含义同modbus协议）  手动清零
	IHU_CANVELA_OPTID_static_detect_range = 0x0D, //（值设定及含义同modbus协议） 静止检测范围
	IHU_CANVELA_OPTID_static_detect_duration = 0x0E, //（值设定及含义同modbus协议） 静止检测时间
	IHU_CANVELA_OPTID_weight_scale_calibration = 0x10, //称量校准
	IHU_CANVELA_OPTID_motor_turn_around = 0x20,  //电机转动
	IHU_CANVELA_OPTID_motor_speed = 0x21,  //电机设置速度
	IHU_CANVELA_OPTID_scale_range = 0x25,  //量程
	IHU_CANVELA_OPTID_node_set = 0x26,  //整个节点
	IHU_CANVELA_OPTID_material_out_normal = 0x27,  //正常出料
	IHU_CANVELA_OPTID_material_give_up = 0x27,  //放弃出料
	IHU_CANVELA_OPTID_error_status = 0x28,  //错误状态	
	IHU_CANVELA_OPTID_max,
	IHU_CANVELA_OPTID_invalid = 0xFF,
}IhuCanvelaOptidEnmu;
typedef enum
{
	IHU_CANVELA_OPTPAR_none = 0,
	IHU_CANVELA_OPTPAR_motor_turn_around_normal = 0x01,
	IHU_CANVELA_OPTPAR_motor_turn_around_reverse = 0x02,
	IHU_CANVELA_OPTPAR_motor_turn_around_stop = 0x03,
	IHU_CANVELA_OPTPAR_weight_scale_calibration_0 = 0x01,
	IHU_CANVELA_OPTPAR_weight_scale_calibration_1kg = 0x02,
	IHU_CANVELA_OPTPAR_weight_scale_calibration_exit = 0x03,
	IHU_CANVELA_OPTPAR_invalid = 0xFF,
}IhuCanvelaOptParEnmu;
//如果是32为量值数据，则采用little_endian_windows的风格，低位在前

//I2C-MOTO控制的L2FRAME格式
typedef struct strIhuI2cariesMotoFrame
{
	UINT8 bfscI2cId;
	UINT8 len;
	UINT16 bfscI2cValue;	
}strIhuI2cariesMotoFrame_t;


//CCL项目下定义的工作状态
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
#define IHU_CCL_DH_CMDID_EVENT_IND_DOOR_LOCK_ALL_C 25
#define IHU_CCL_DH_CMDID_EVENT_IND_LOCK_TRIGGER 30   	//触发CCL进入工作状态
#define IHU_CCL_DH_CMDID_EVENT_IND_SHAKE_TRIGGER 31   //触发CCL进入工作状态
#define IHU_CCL_DH_CMDID_EVENT_IND_LOCK_AND_SHAKE 32  //触发CCL进入工作状态
#define IHU_CCL_DH_CMDID_EVENT_IND_FAULT_MULTI 40   //触发CCL进入错误模式
#define IHU_CCL_DH_CMDID_EVENT_IND_FAULT_RECOVER 41    //触发CCL进入恢复正常
#define IHU_CCL_DH_CMDID_WORK_MODE_SLEEP 60   	//强制进入新状态
#define IHU_CCL_DH_CMDID_WORK_MODE_ACTIVE 61   	//强制进入新状态
#define IHU_CCL_DH_CMDID_WORK_MODE_FAULT 62   	//强制进入新状态
#define IHU_CCL_DH_CMDID_WORK_MODE_CTRL 63   		//用于发送指令

#define IHU_CCL_DH_CMDID_CMD_ENABLE_LOCK  70        //用于激活其它锁具 
#define IHU_CCL_DH_CMDID_CMD_DOOR_OPEN  71        //用于激活其它锁具 
#define IHU_CCL_DH_CMDID_CMD_DOOR_CLOSE  72        //用于激活其它锁具 

#define IHU_CCL_DH_CMDID_HEART_BEAT 0xFE
#define IHU_CCL_DH_CMDID_INVAID 0xFF

#define IHU_CCL_LOCK_AUTH_RESULT_OK  1
#define IHU_CCL_LOCK_AUTH_RESULT_NOK  2


//后台接口协议定义
typedef struct CloudDataSendBuf
{
	UINT16 bufferLen;
	//char的意义是，底层均为字符串，而不是十六进制数据
	char buf[IHU_SYSMSG_BH_BUF_BODY_LEN_MAX];  //内部还包括格式化的参数部分，这里需要再仔细考虑一下
}CloudDataSendBuf_t;

//VMMW_GPRSMOD基站定位输出数据结构
typedef struct StrVmmwGprsmodBasestationPosition
{
	float latitude;
	float longitude;
	char  date[20];
	char  time[20];
}StrVmmwGprsmodBasestationPosition_t;



#endif /* L1VMFREEOS_L1COMDEF_FREEOS_H_ */
