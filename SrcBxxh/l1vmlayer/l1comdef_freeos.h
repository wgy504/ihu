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

//自定义，服从自定义XML规范
//CLOUD<->HCU之间自行定义的命令字
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
  L3CI_hcu_iau_control = 0x10, //HCU与IAU之间控制命令
  L3CI_emc = 0x20, //电磁辐射强度
  L3CI_emc_accumulation = 0x21, //电磁辐射剂量
  L3CI_emc_indicator = 0x20, //电磁辐射强度
  L3CI_co = 0x22, //一氧化碳
  L3CI_formaldehyde = 0x23, //甲醛HCHO
  L3CI_alcohol = 0x24, //酒精
  L3CI_pm25 = 0x25, //PM1/2.5/10
  L3CI_windspd = 0x26, //Wind Speed
  L3CI_winddir = 0x27, //Wind Direction
  L3CI_temp = 0x28, //Temperature
  L3CI_humid = 0x29, //Humidity
  L3CI_airprs = 0x2A, //Air pressure
  L3CI_noise = 0x2B, //Noise
  L3CI_hsmmp = 0x2C, //Camer or audio high speed
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
  L3CI_itf_sps = 0x40, //串口读取命令/返回结果
  L3CI_itf_adc = 0x41, //ADC读取命令/返回结果
  L3CI_itf_dac = 0x42, //DAC读取命令/返回结果
  L3CI_itf_i2c = 0x43, //I2C读取命令/返回结果
  L3CI_itf_pwm = 0x44, //PWM读取命令/返回结果
  L3CI_itf_di = 0x45, //DI读取命令/返回结果
  L3CI_itf_do = 0x46, //DO读取命令/返回结果
  L3CI_itf_can = 0x47, //CAN读取命令/返回结果
  L3CI_itf_spi = 0x48, //SPI读取命令/返回结果
  L3CI_itf_usb = 0x49, //USB读取命令/返回结果
  L3CI_itf_eth = 0x4A, //网口读取命令/返回结果
  L3CI_itf_485 = 0x4B, //485读取命令/返回结果
  L3CI_sw_inventory = 0xA0, //软件清单
  L3CI_sw_package = 0xA1, //软件版本体
  L3CI_equipment_info = 0xF0, //设备基本信息
  L3CI_personal_info = 0xF1,  //个人基本信息
  L3CI_time_sync = 0xF2,  //时间同步
  L3CI_read_data = 0xF3,  //读取数据
  L3CI_clock_timeout = 0xF4,  //定时闹钟及久坐提醒
  L3CI_sync_charging = 0xF5,  //同步充电，双击情况
  L3CI_sync_trigger = 0xF6, //同步通知信息
  L3CI_heart_beat = 0xFE, //心跳
  L3CI_null = 0xFF, //无效
}L3UserCmdIdDef;
//HCU<->IAU HUITP接口之间定义的操作字
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

#endif /* L1VMFREEOS_L1COMDEF_FREEOS_H_ */
