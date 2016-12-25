/*
 * l1comdef_freeos.h
 *
 *  Created on: 2016年1月3日
 *      Author: test
 */

#ifndef L1VMFREEOS_L1COMDEF_FREEOS_H_
#define L1VMFREEOS_L1COMDEF_FREEOS_H_

#include "vmfreeoslayer.h"
#include "l1huitp_freeos.h"


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

//为各个业务独自定义的方式
//老旧定义方式，未来需要改进去掉
//标准命令字统一定义
typedef enum
{
	L3CI_none = 0,
	L3CI_blood_glucose = 1,  //血糖
	L3CI_single_sports = 2,  //单次运动
	L3CI_single_sleep = 3, //单次睡眠
	L3CI_body_fat = 4,  //体脂
	L3CI_blood_pressure = 5, //血压
	L3CI_runner_machine_report = 0x0A, //跑步机数据上报
	L3CI_runner_machine_control = 0x0B, //跑步机任务控制
	L3CI_gps = 0x0C, //GPS地址
	L3CI_Ihu_iau_control = 0x10, //IHU与IAU之间控制命令
	L3CI_emc = 0x20, //电磁辐射强度
	L3CI_emc_accumulation = 0x21, //电磁辐射剂量
	L3CI_emc_indicator = 0x20, //电磁辐射强度
	L3CI_co = 0x22, //一氧化碳
	L3CI_formaldehyde = 0x23, //甲醛HCHO
	L3CI_alcohol = 0x24, //酒精
	L3CI_pm25 = 0x25, //PM1/2.5/10
	L3CI_windspd = 0x26, //风速Wind Speed
	L3CI_winddir = 0x27, //风向Wind Direction
	L3CI_temp = 0x28, //温度Temperature
	L3CI_humid = 0x29, //湿度Humidity
	L3CI_airprs = 0x2A, //气压Air pressure
	L3CI_noise = 0x2B, //噪声Noise
	L3CI_hsmmp = 0x2C, //相机Camer or audio high speed
	L3CI_audio = 0x2D, //声音
	L3CI_video = 0x2E, //视频
	L3CI_picture = 0x2F, //图片
	L3CI_lock = 0x30, //云控锁
	L3CI_water_meter = 0x31, //水表
	L3CI_heat_meter = 0x32, //热表
	L3CI_gas_meter = 0x33, //气表
	L3CI_power_meter = 0x34, //电表
	L3CI_light_strength = 0x35, //光照强度
	L3CI_toxicgas = 0x36, //有毒气体VOC
	L3CI_altitude = 0x37, //海拔高度
	L3CI_moto = 0x38, //马达
	L3CI_switch = 0x39, //继电器
	L3CI_transporter = 0x3A, //导轨传送带
	L3CI_bfsc_comb_scale = 0x3B, //组合秤
	L3CI_ccl_lock = 0x40,  //智能锁
	L3CI_ccl_door = 0x41, //光交箱门
	L3CI_ccl_rfid = 0x42, //光交箱RFID模块
	L3CI_ccl_ble = 0x43, //光交箱BLE模块
	L3CI_ccl_gprs = 0x44, //光交箱GPRS模块
	L3CI_ccl_battery = 0x45, //光交箱电池模块
	L3CI_ccl_shake = 0x46, //光交箱震动
	L3CI_ccl_smoke = 0x47, //光交箱烟雾
	L3CI_ccl_water = 0x48, //光交箱水浸
	L3CI_ccl_temp = 0x49, //光交箱温度
	L3CI_ccl_humid = 0x4A, //光交箱湿度
	L3CI_ccl_fall = 0x4B, //倾倒
	L3CI_ccl_state = 0x4C, //状态聚合
	L3CI_itf_sps = 0x50, //串口读取命令/返回结果
	L3CI_itf_adc = 0x51, //ADC读取命令/返回结果
	L3CI_itf_dac = 0x52, //DAC读取命令/返回结果
	L3CI_itf_i2c = 0x53, //I2C读取命令/返回结果
	L3CI_itf_pwm = 0x54, //PWM读取命令/返回结果
	L3CI_itf_di = 0x55, //DI读取命令/返回结果
	L3CI_itf_do = 0x56, //DO读取命令/返回结果
	L3CI_itf_can = 0x57, //CAN读取命令/返回结果
	L3CI_itf_spi = 0x58, //SPI读取命令/返回结果
	L3CI_itf_usb = 0x59, //USB读取命令/返回结果
	L3CI_itf_eth = 0x5A, //网口读取命令/返回结果
	L3CI_itf_485 = 0x5B, //485读取命令/返回结果
	L3CI_Ihu_inventory= 0xA0,	//软件清单
	L3CI_sw_package = 0xA1,	//软件版本体
	L3CI_alarm_info = 0xB0, //for alarm report
	L3CI_performance_info = 0xB1, // or PM report
	L3CI_equipment_info = 0xF0,	//设备基本信息
	L3CI_personal_info = 0xF1,	//个人基本信息
	L3CI_time_sync = 0xF2,	//时间同步
	L3CI_read_data = 0xF3,	//读取数据
	L3CI_clock_timeout = 0xF4,	//定时闹钟及久坐提醒
	L3CI_sync_charging = 0xF5,	//同步充电，双击情况
	L3CI_sync_trigger = 0xF6,	//同步通知信息
	L3CI_cmd_control = 0xFD,  //for cmd control by Shanchun
	L3CI_heart_beat = 0xFE, //心跳
	L3CI_null = 0xFF, //无效
}L3StandardHuiCmdDefId;

//IHU<->IAU HUITP接口之间定义的操作字
//所有的操作字，需要极度的丰富化，以形成完整的处理任务模块
typedef enum
{
  L3PO_min = 0,
  L3PO_none = 0,
  L3PO_data_req = 0x01, //Data Request
  L3PO_set_switch = 0x02,
  L3PO_set_modbus_address =0x03,
  L3PO_set_work_cycle = 0x04, //In second
  L3PO_set_sample_cycle = 0x05, //In second
  L3PO_set_sample_number = 0x06,
  L3PO_data_report = 0x81, //Data Report
  L3PO_read_switch = 0x82,
  L3PO_read_modbus_address = 0x83,
  L3PO_read_work_cycle = 0x84, //In second
  L3PO_read_sample_cycle = 0x85, //In second
  L3PO_read_sample_number = 0x86,
  L3PO_max,
}L3HuitpOptIdDef;

//L3PO的通用定义，如果不属于任何传感器，则可以使用这个定义
typedef enum
{
	L3PO_generic_min = 0,
	L3PO_generic_none = 0,
	L3PO_generic_data_req = 0x01, //Data Request
	L3PO_generic_set_switch = 0x02,
	L3PO_generic_set_modbus_address =0x03,
	L3PO_generic_set_work_cycle = 0x04, //In second
	L3PO_generic_set_sample_cycle = 0x05, //In second
	L3PO_generic_set_sample_number = 0x06,
	L3PO_generic_data_report = 0x81, //Data Report
	L3PO_generic_read_switch = 0x82,
	L3PO_generic_read_modbus_address = 0x83,
	L3PO_generic_read_work_cycle = 0x84, //In second
	L3PO_generic_read_sample_cycle = 0x85, //In second
	L3PO_generic_read_sample_number = 0x86,
	L3PO_generic_max,
}L3GenericOptIdDef;

//EMC特例
typedef enum
{
	L3PO_emc_min = 0,
	L3PO_emc_none = 0,
	L3PO_emc_data_req = 0x01, //Data Request
	L3PO_emc_set_switch = 0x02,
	L3PO_emc_set_modbus_address =0x03,
	L3PO_emc_set_work_cycle = 0x04, //In second
	L3PO_emc_set_sample_cycle = 0x05, //In second
	L3PO_emc_set_sample_number = 0x06,
	L3PO_emc_data_report = 0x81, //Data Report
	L3PO_emc_read_switch = 0x82,
	L3PO_emc_read_modbus_address = 0x83,
	L3PO_emc_read_work_cycle = 0x84, //In second
	L3PO_emc_read_sample_cycle = 0x85, //In second
	L3PO_emc_read_sample_number = 0x86,
	L3PO_emc_max,
}L3EmcOptIdDef;

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

//UINT8  cmdIdBackType; //指明是瞬时，还是周期性读数，针对读数到底是周期性的还是一次性的
//内部定义，内部使用
typedef enum
{
	L3CI_cmdid_back_type_none = 0,
	L3CI_cmdid_back_type_instance = 1,
	L3CI_cmdid_back_type_period = 2,
	L3CI_cmdid_back_type_control = 3,
	L3CI_cmdid_back_type_invalid = 0xFF,
}L3UserCmdIdBackTypeDef;

//ASYLIBRA的定义
#define IHU_ASY_HEADER_MAGICNUMBER  0xFE
#define IHU_ASY_HEADER_BVERSION  0x01

//Defined at epb_MmBp.c, coming from Tecent or Google protobuf protocal!
#define TAG_BaseResponse_ErrCode												0x08
#define TAG_BaseResponse_ErrMsg													0x12
#define TAG_AuthRequest_BaseRequest											0x0a
#define TAG_AuthRequest_Md5DeviceTypeAndDeviceId				0x12
#define TAG_AuthRequest_ProtoVersion										0x18
#define TAG_AuthRequest_AuthProto												0x20
#define TAG_AuthRequest_AuthMethod											0x28
#define TAG_AuthRequest_AesSign													0x32
#define TAG_AuthRequest_MacAddress											0x3a
#define TAG_AuthRequest_TimeZone												0x52
#define TAG_AuthRequest_Language												0x5a
#define TAG_AuthRequest_DeviceName											0x62
#define TAG_AuthResponse_BaseResponse										0x0a
#define TAG_AuthResponse_AesSessionKey									0x12
#define TAG_InitRequest_BaseRequest											0x0a
#define TAG_InitRequest_RespFieldFilter									0x12
#define TAG_InitRequest_Challenge												0x1a
#define TAG_InitResponse_BaseResponse										0x0a
#define TAG_InitResponse_UserIdHigh											0x10
#define TAG_InitResponse_UserIdLow											0x18
#define TAG_InitResponse_ChalleangeAnswer								0x20
#define TAG_InitResponse_InitScence											0x28
#define TAG_InitResponse_AutoSyncMaxDurationSecond			0x30
#define TAG_InitResponse_UserNickName										0x5a
#define TAG_InitResponse_PlatformType										0x60
#define TAG_InitResponse_Model													0x6a
#define TAG_InitResponse_Os															0x72
#define TAG_InitResponse_Time														0x78
#define TAG_InitResponse_TimeZone												0x8001
#define TAG_InitResponse_TimeString											0x8a01
#define TAG_SendDataRequest_BaseRequest									0x0a
#define TAG_SendDataRequest_Data												0x12
#define TAG_SendDataRequest_Type												0x18
#define TAG_SendDataResponse_BaseResponse								0x0a
#define TAG_SendDataResponse_Data												0x12
#define TAG_RecvDataPush_BasePush												0x0a
#define TAG_RecvDataPush_Data														0x12
#define TAG_RecvDataPush_Type														0x18
#define TAG_SwitchViewPush_BasePush											0x0a
#define TAG_SwitchViewPush_SwitchViewOp									0x10
#define TAG_SwitchViewPush_ViewId												0x18
#define TAG_SwitchBackgroudPush_BasePush								0x0a
#define TAG_SwitchBackgroudPush_SwitchBackgroundOp			0x10
/*typedef enum
{
	ECI_none = 0,
	ECI_req_auth = 10001,
	ECI_req_sendData = 10002,
	ECI_req_init = 10003,
	ECI_resp_auth = 20001,
	ECI_resp_sendData = 20002,
	ECI_resp_init = 20003,
	ECI_push_recvData = 30001,
	ECI_push_switchView = 30002,
	ECI_push_switchBackgroud = 30003,
	ECI_err_decode = 29999
} EmCmdId;
*/

//统一定义汇报给后台的数据格式
typedef enum
{
	CLOUD_SENSOR_DATA_FORMAT_NULL = 0,
	CLOUD_SENSOR_DATA_FOMAT_INT_ONLY = 1,
	CLOUD_SENSOR_DATA_FOMAT_FLOAT_WITH_NF1 = 2,
	CLOUD_SENSOR_DATA_FOMAT_FLOAT_WITH_NF2 = 3,
	CLOUD_SENSOR_DATA_FOMAT_FLOAT_WITH_NF3 = 4,
	CLOUD_SENSOR_DATA_FOMAT_FLOAT_WITH_NF4 = 5,
	CLOUD_SENSOR_DATA_FOMAT_MAX = 0x10,
	CLOUD_SENSOR_DATA_FOMAT_INVALID = 0xFF,
}CloudDataFormatDef;

//设备地址，用于支持多个设备的同时工作
#define IHU_EMC_EQUID_DEFAULT 1


/**************************************************************************************
 *   HUITP标准链路层FRAME定义                                                           *
 *************************************************************************************/
//帧结构定义
typedef struct StrHuiFrame
{
    UINT8 	start; //0xFE
    UINT8 	checksum1; //加总头的结果
    UINT16 	len; //0-1500，原则上不能超过系统定义的消息体长度
    UINT8 	msgBody[];
}StrHuiFrame_t;


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
//#define IHU_CCL_DH_CMDID_EVENT_IND_FAULT_DOOR_O 40    //触发CCL进入错误模式
//#define IHU_CCL_DH_CMDID_EVENT_IND_FAULT_LOCK_O 41    //触发CCL进入错误模式
//#define IHU_CCL_DH_CMDID_EVENT_IND_FAULT_BAT_WARNING 42    //触发CCL进入错误模式
//#define IHU_CCL_DH_CMDID_EVENT_IND_FAULT_FALL_WARNING 43    //触发CCL进入错误模式
//#define IHU_CCL_DH_CMDID_EVENT_IND_FAULT_SMOKE_WARNING 44    //触发CCL进入错误模式
//#define IHU_CCL_DH_CMDID_EVENT_IND_FAULT_WATER_WARNING 45    //触发CCL进入错误模式
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
	char buf[MAX_IHU_MSG_BUF_LENGTH_CLOUD];  //内部还包括格式化的参数部分，这里需要再仔细考虑一下
}CloudDataSendBuf_t;

//跟后台的通信接口中，需要定义一些全局消息的结构体类型
enum CloudBhMsgTypeEnum
{
	IHU_CLOUD_BH_MSG_TYPE_MIN = 0x00,
	IHU_CLOUD_BH_MSG_TYPE_DEVICE_REPORT_UINT8,
	IHU_CLOUD_BH_MSG_TYPE_DEVICE_CONTROL_UINT8,
	IHU_CLOUD_BH_MSG_TYPE_HEAT_BEAT_UINT8,
	IHU_CLOUD_BH_MSG_TYPE_BIZ_ITG_UINT8,
	IHU_CLOUD_BH_MSG_TYPE_ALARM_REPORT_UINT8,//for alarm report
	IHU_CLOUD_BH_MSG_TYPE_PM_REPORT_UINT8,//for pm report
	IHU_CLOUD_BH_MSG_TYPE_MAX,
};
#define IHU_CLOUD_BH_MSG_TYPE_DEVICE_REPORT_STRING  "hcu_text"  //"hcu_text"
#define IHU_CLOUD_BH_MSG_TYPE_DEVICE_CONTROL_STRING  "hcu_command" //"hcu_command"
#define IHU_CLOUD_BH_MSG_TYPE_HEAT_BEAT_STRING "hcu_heart_beat"   //"hcu_heart_beat"  //心跳协议，里面的数据内容是空的
#define IHU_CLOUD_BH_MSG_TYPE_BIZ_ITG_STRING "hcu_biz_itg"  //业务智能 hcu_biz_inteligence
#define IHU_CLOUD_BH_MSG_TYPE_ALARM_REPORT_STRING "hcu_alarm"  //for alarm report
#define IHU_CLOUD_BH_MSG_TYPE_PM_REPORT_STRING "hcu_pm"  //for pm report

//HUIXML格式定义
//主体内容中，各个不同的字段完全按照这个优先级进行编码，解码时将由各种操作字的隐含必选关系进行解码，从而简化编码函数
#define IHU_CLOUD_BH_ITF_STD_XML_HEAD_MAX_LENGTH IHU_FILE_NAME_LENGTH_MAX  //要传递HTTP名字会后台，只能扩大该域的长度
//固定头
typedef struct CloudBhItfHuixmlFixHead
{
	char xml_l[6];
	char ToUserName_l[22];
	char ToUserName[IHU_CLOUD_BH_ITF_STD_XML_HEAD_MAX_LENGTH+1];
	char ToUserName_r[17];
	char FromUserName_l[24];
	char FromUserName[IHU_CLOUD_BH_ITF_STD_XML_HEAD_MAX_LENGTH+1];
	char FromUserName_r[19];
	char CreateTime_l[13];
	char CreateTime[IHU_CLOUD_BH_ITF_STD_XML_HEAD_MAX_LENGTH+1];
	char CreateTime_r[14];
	char MsgType_l[19];
	char MsgType[IHU_CLOUD_BH_ITF_STD_XML_HEAD_MAX_LENGTH+1];
	char MsgType_r[14];
	char Content_l[19];
	char conCmdId[3];
	char conLen[3];  //1B
	char conOptId[3]; //1B	
}CloudBhItfHuixmlFixHead_t;

//固定尾
typedef struct CloudBhItfHuixmlFixTail
{
	char Content_r[14];
	char FuncFlag_l[11];
	char FuncFlag[IHU_CLOUD_BH_ITF_STD_XML_HEAD_MAX_LENGTH+1];
	char FuncFlag_r[12];
	char xml_r[7];
}CloudBhItfHuixmlFixTail_t;

//软件清单结构定义

//CCL状态报告消息结构定义
typedef struct CloudBhItfHuixmlCclStateReportLock
{
	//CCL状态报告：Lock的TLV参数
	char cclLockId[3]; //1B
	char cclLockLen[3]; //1B
	char cclLockState[3]; //1B	
}CloudBhItfHuixmlCclStateReportLock_t;
typedef struct CloudBhItfHuixmlCclStateReportDoor
{
	//CCL状态报告：Door的TLV参数
	char cclDoorId[3]; //1B
	char cclDoorLen[3]; //1B
	char cclDoorState[3]; //1B
}CloudBhItfHuixmlCclStateReportDoor_t;
typedef struct CloudBhItfHuixmlCclStateReportShake
{
	//CCL状态报告：Shake的TLV参数
	char cclShakeId[3]; //1B
	char cclShakeLen[3]; //1B
	char cclShakeState[3]; //1B
}CloudBhItfHuixmlCclStateReportShake_t;
typedef struct CloudBhItfHuixmlCclStateReportSmoke
{
	//CCL状态报告：Smoke的TLV参数
	char cclSmokeId[3]; //1B
	char cclSmokeLen[3]; //1B
	char cclSmokeState[3]; //1B
}CloudBhItfHuixmlCclStateReportSmoke_t;
typedef struct CloudBhItfHuixmlCclStateReportWater
{
	//CCL状态报告：Water的TLV参数
	char cclWaterId[3]; //1B
	char cclWaterLen[3]; //1B
	char cclWaterState[3]; //1B
}CloudBhItfHuixmlCclStateReportWater_t;
typedef struct CloudBhItfHuixmlCclStateReportFall
{
	//CCL状态报告：Fall的TLV参数
	char cclFallId[3]; //1B
	char cclFallLen[3]; //1B
	char cclFallState[3]; //1B
}CloudBhItfHuixmlCclStateReportFall_t;
typedef struct CloudBhItfHuixmlCclStateReportBat
{
	//CCL状态报告：Bat的TLV参数
	char cclBatId[3]; //1B  电池告警状态
	char cclBatLen[3]; //1B
	char cclBatState[3]; //1B
}CloudBhItfHuixmlCclStateReportBat_t;
typedef struct CloudBhItfHuixmlCclStateReportTemp
{
	//CCL状态报告：Temp的TLV参数
	char cclTempId[3]; //1B
	char cclTempLen[5]; //2B
	char cclTempVal[5]; //2B
}CloudBhItfHuixmlCclStateReportTemp_t;
typedef struct CloudBhItfHuixmlCclStateReportHumid
{
	//CCL状态报告：Humid的TLV参数
	char cclHumidId[3]; //1B
	char cclHumidLen[5]; //2B
	char cclHumidVal[5]; //2B
}CloudBhItfHuixmlCclStateReportHumid_t;
typedef struct CloudBhItfHuixmlCclStateReportBattery
{
	//CCL状态报告：Humid的TLV参数
	char cclBatteryId[3]; //1B  电池剩余电量数值
	char cclBatteryLen[5]; //2B
	char cclBatteryVal[5]; //2B
}CloudBhItfHuixmlCclStateReportBattery_t;
typedef struct CloudBhItfHuixmlCclStateReportRssi
{
	//CCL状态报告：Rssi的TLV参数
	char cclRssiId[3]; //1B
	char cclRssiLen[5]; //2B
	char cclRssiVal[5]; //2B
}CloudBhItfHuixmlCclStateReportRssi_t;
typedef struct CloudBhItfHuixmlCclStateReportRsv1
{
	//CCL状态报告：Rsv1的TLV参数
	char cclRsv1Id[3]; //1B
	char cclRsv1Len[5]; //2B
	char cclRsv1Val[5]; //2B
}CloudBhItfHuixmlCclStateReportRsv1_t;
typedef struct CloudBhItfHuixmlCclStateReportRsv2
{
	//CCL状态报告：Rsv2的TLV参数
	char cclRsv2Id[3]; //1B
	char cclRsv2Len[5]; //2B
	char cclRsv2Val[5]; //2B
}CloudBhItfHuixmlCclStateReportRsv2_t;
typedef struct CloudBhItfHuixmlCclStateReport
{
	//CCL的状态报告TLV参数
	CloudBhItfHuixmlCclStateReportLock_t cclLock;
	CloudBhItfHuixmlCclStateReportDoor_t cclDoor;
	CloudBhItfHuixmlCclStateReportShake_t cclShake;
	CloudBhItfHuixmlCclStateReportSmoke_t cclSmoke;
	CloudBhItfHuixmlCclStateReportWater_t cclWater;
	CloudBhItfHuixmlCclStateReportFall_t cclFall;
	CloudBhItfHuixmlCclStateReportBat_t cclBat;
	CloudBhItfHuixmlCclStateReportTemp_t cclTemp;
	CloudBhItfHuixmlCclStateReportHumid_t cclHumid;
	CloudBhItfHuixmlCclStateReportBattery_t cclBattery;
	CloudBhItfHuixmlCclStateReportRssi_t cclRssi;
	CloudBhItfHuixmlCclStateReportRsv1_t cclRsv1;
	CloudBhItfHuixmlCclStateReportRsv2_t cclRsv2;
}CloudBhItfHuixmlCclStateReport_t;



typedef struct StrCloudBhItfStdHuixml
{
	//固定头
	CloudBhItfHuixmlFixHead_t fixHead;

	//变长部分，未来演进为TLV标准结构
	//每一个内容，都是一个标准的IE，全局唯一编码，确保编解码的简单高效
	//为了进一步提高效率，所有ID编码，包括CMDID/OPTID/IEID，均采用哈夫曼变长编码，00预留，01-EF为常用断码，F0xx - FFxx表示长码
		
	//软件清单
	//Added by Shanchun for hcu inventory
	char conHwUuid[12];//6B
	char conHwType[3];//1B
	char conHwVersion[5];//2B
	char conSwDelivery[5];//1B
	char conSwRelease[5];//2B
		
	//软件下载
	//Added by Shanchun for hcu sw download
	char conSwDownload[3];
	
	//CCL状态报告的TLV
	CloudBhItfHuixmlCclStateReport_t cclStateReport;
	
	//固定尾
	CloudBhItfHuixmlFixTail_t fixTail;
}StrCloudBhItfStdHuixml_t;


#endif /* L1VMFREEOS_L1COMDEF_FREEOS_H_ */
