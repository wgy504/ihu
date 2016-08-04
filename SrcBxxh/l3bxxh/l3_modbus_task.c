/**
 ****************************************************************************************
 *
 * @file L3_modbus_task.c
 *
 * @brief Layer 3 modbus task
 *
 * BXXH team
 * Created by ZJL, 20150819
 *
 ****************************************************************************************
 */
#if (CFG_MODBUS)
#include "bxxh_adapt.h"
#include "L3_modbus_task.h"
#include "l3_asylibra_task.h"
#include "l3_wxapp_task.h"
#include "prf_types.h"
#include "app_sps_scheduler.h"
#include "sps_server_task.h"
#include "app_console.h"
#include "adc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "epb_MmBp.h"
#include "crc32.h"
#include "time.h"
//#include "aes_crypt.h"

static int handler_modbus_disabled_init(ke_msg_id_t const msgid, struct msg_struct_wxapp_modbus_connect_ready const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
static int handler_modbus_10min_timtout(ke_msg_id_t const msgid, struct msg_struct_mobus_10min_timtout const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
static int handler_modbus_5second_timtout(ke_msg_id_t const msgid, struct msg_struct_modbus_5second_timtout const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
static int handler_modbus_data_push(ke_msg_id_t const msgid, struct msg_struct_wxapp_modbus_data_push const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
static int handler_modbus_period_report_pm1025(ke_msg_id_t const msgid, struct msg_struct_modbus_period_report_pm1025_trigger *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
static int handler_modbus_period_report_wind_speed(ke_msg_id_t const msgid, struct msg_struct_modbus_period_report_wind_speed_trigger *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
static int handler_modbus_period_report_wind_direction(ke_msg_id_t const msgid, struct msg_struct_modbus_period_report_wind_direction_trigger *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
static int handler_modbus_period_report_temperature(ke_msg_id_t const msgid, struct msg_struct_modbus_period_report_temperature_trigger *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
static int handler_modbus_period_report_humidity(ke_msg_id_t const msgid, struct msg_struct_modbus_period_report_humidity_trigger *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
static int handler_modbus_disconnect(ke_msg_id_t const msgid, struct msg_struct_wxapp_modbus_disconnect const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
static int handler_modbus_uart1_data_received(ke_msg_id_t const msgid, struct msg_struct_app_server_uart1_data_received const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
static int handler_modbus_uart1_frame_timtout(ke_msg_id_t const msgid, struct msg_struct_modbus_uart1_frame_timtout const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);

const struct ke_msg_handler fsm_modbus_default[] =
{
	{MSG_WXAPP_MODBUS_CONNECT_READY,									(ke_msg_func_t)handler_modbus_disabled_init},
	{MSG_MODBUS_10MIN_TIMEOUT, 												(ke_msg_func_t)handler_modbus_10min_timtout},
	{MSG_MODBUS_5SECOND_TIMEOUT, 											(ke_msg_func_t)handler_modbus_5second_timtout},
	{MSG_WXAPP_MODBUS_DATA_PUSH, 											(ke_msg_func_t)handler_modbus_data_push},	
	{MSG_MODBUS_PERIOD_REPORT_PM1025_TRIGGER,         (ke_msg_func_t)handler_modbus_period_report_pm1025},
	{MSG_MODBUS_PERIOD_REPORT_WIND_SPEED_TRIGGER,     (ke_msg_func_t)handler_modbus_period_report_wind_speed},
	{MSG_MODBUS_PERIOD_REPORT_WIND_DIRECTION_TRIGGER,	(ke_msg_func_t)handler_modbus_period_report_wind_direction},
	{MSG_MODBUS_PERIOD_REPORT_TEMPERATURE_TRIGGER,    (ke_msg_func_t)handler_modbus_period_report_temperature},
	{MSG_MODBUS_PERIOD_REPORT_HUMIDITY_TRIGGER,       (ke_msg_func_t)handler_modbus_period_report_humidity},	
	{MSG_WXAPP_MODBUS_DISCONNECT, 										(ke_msg_func_t)handler_modbus_disconnect},
	{MSG_APP_SERVER_UART1_DATA_RECEIVED, 							(ke_msg_func_t)handler_modbus_uart1_data_received},
	{MSG_MODBUS_UART1_FRAME_TIMEOUT, 									(ke_msg_func_t)handler_modbus_uart1_frame_timtout},	

};
/// Specifies the message handlers that are common to all states.
const struct ke_state_handler handler_modbus_default = KE_STATE_HANDLER(fsm_modbus_default); //KE_STATE_HANDLER_NONE;
/// Defines the placeholder for the states of all the task instances.
ke_state_t modbus_state[MODBUS_IDX_MAX];
/// MODBUS task descriptor
static const struct ke_task_desc TASK_DESC_MODBUS = {NULL, &handler_modbus_default, modbus_state, STATE_MODBUS_MAX, MODBUS_IDX_MAX};
struct modbus_env_tag
{
	//connection information
	struct prf_con_info con_info;
	/// Application Task Id
	ke_task_id_t appid;
	/// Connection handle
	uint16_t conhdl;
	/// DB Handle
	uint16_t shdl;
}modbus_env;

//本任务模块中的全局数据变量
unsigned int modbusPm1025RequestCount;
unsigned int modbusPm1025ResponseCount;
uint16_t modbusZmanufatureSvr;
bool modbusPm1025InstanceFlag; //instance->true, period -> false
UartRingBuffer modbusUart1Buffer;
uint8_t zCycleReadModbusSensor;
bool zModbusTemperatureOrHumidity;

//参考WXAPP的全局变量
extern time_t z_system_time_unix;
extern struct tm *z_system_time_ymd;
extern struct flash_data_sample_storage z_flash_data_sample[FLASH_DATA_SAMPLE_STORAGE_NBR_MAX];
extern uint8_t z_flash_data_sample_write;  //存储的起点
extern uint8_t z_flash_data_sample_read; //读取的起点
extern uint8_t z_flash_data_sample_nbr;  //存入有效多少记录


//任务程序初始化
void task_modbus_init(void)
{
	// Reset the wxapp_receivererometer environment
	memset(&modbus_env, 0, sizeof(modbus_env));
	
	// Create MODBUS task
	ke_task_create(TASK_MODBUS, &TASK_DESC_MODBUS);

	// Go to IDLE state
	ke_state_set(TASK_MODBUS, STATE_MODBUS_DISABLED);
	//Init
	modbusPm1025RequestCount = 0;
	modbusPm1025ResponseCount = 0;
	modbusPm1025InstanceFlag = false;
	//初始化跟UART1通信的缓存数据区
	modbusUart1Buffer.size = APP_SPS_UART1_DATA_LENGTH_MAX_BUF;
	modbusUart1Buffer.readIdx = 0;
	modbusUart1Buffer.writeIdx = 0;
	//从第一个传感器开始轮循
	zCycleReadModbusSensor = 0;
	zModbusTemperatureOrHumidity = true;
}

//Mainloop entry, give chance to get control rights at anytime
void task_modbus_mainloop(void)
{	

}

//初始化到正常状态，判定是从DISABLE还是OFFLINE来的，从而进行不同的处理
static int handler_modbus_disabled_init(ke_msg_id_t const msgid, struct msg_struct_wxapp_modbus_connect_ready const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
	//第一次上电初始化后，启动定时器
	if (ke_state_get(TASK_MODBUS) == STATE_MODBUS_DISABLED)
	{
		//正式进入CONNECT状态后，PM1025定时读取数据便开始了
		bxxh_timer_set(MSG_MODBUS_10MIN_TIMEOUT, TASK_MODBUS, TIMER_MODBUS_10MIN);
	}
	//重现BLE上线
	else if (ke_state_get(TASK_MODBUS) == STATE_WXAPP_OFFLINE)
	{
		//将FLASH存储的历史数据都发到后台
		//妥善处理nTimes的参数问题，以表征时间的问题
		//FLASH数据由WXAPP统一处理，这里不再单独处理了
		bxxh_print_console("MODBUS: FLASH storage data will be handled by WXAPP. \r\n");
	}
	
	ke_state_set(TASK_MODBUS, STATE_MODBUS_CONNECTED);
	return (KE_MSG_CONSUMED);
}

//定时启动，读取传感器数据
static int handler_modbus_10min_timtout(ke_msg_id_t const msgid, struct msg_struct_mobus_10min_timtout const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
	zCycleReadModbusSensor = zCycleReadModbusSensor % 5;
	switch (zCycleReadModbusSensor)
	{
		case 0:
		{
			struct msg_struct_modbus_period_report_pm1025_trigger m;
			m.length = sizeof(m);
			bxxh_message_send(MSG_MODBUS_PERIOD_REPORT_PM1025_TRIGGER, TASK_MODBUS, TASK_MODBUS, &m, m.length);
			break;			
		}
		case 1:
		{
			struct msg_struct_modbus_period_report_wind_speed_trigger m;
			m.length = sizeof(m);
			bxxh_message_send(MSG_MODBUS_PERIOD_REPORT_WIND_SPEED_TRIGGER, TASK_MODBUS, TASK_MODBUS, &m, m.length);		
			break;			
		}
		case 2:
		{
			struct msg_struct_modbus_period_report_wind_direction_trigger m;
			m.length = sizeof(m);
			bxxh_message_send(MSG_MODBUS_PERIOD_REPORT_WIND_DIRECTION_TRIGGER, TASK_MODBUS, TASK_MODBUS, &m, m.length);	
			break;			
		}
		case 3:
		{
			struct msg_struct_modbus_period_report_temperature_trigger m;
			m.length = sizeof(m);
			bxxh_message_send(MSG_MODBUS_PERIOD_REPORT_TEMPERATURE_TRIGGER, TASK_MODBUS, TASK_MODBUS, &m, m.length);
			zModbusTemperatureOrHumidity = true;
			break;			
		}
		case 4:
		{
			struct msg_struct_modbus_period_report_humidity_trigger m;
			m.length = sizeof(m);
			bxxh_message_send(MSG_MODBUS_PERIOD_REPORT_HUMIDITY_TRIGGER, TASK_MODBUS, TASK_MODBUS, &m, m.length);
			zModbusTemperatureOrHumidity = false;			
			break;			
		}
		default:
			break;
	}

	//最后启动定时器，以便循环读取
	bxxh_timer_set(MSG_MODBUS_10MIN_TIMEOUT, TASK_MODBUS, TIMER_MODBUS_10MIN);	
	zCycleReadModbusSensor++;  //下一次继续轮到下一个传感器
	//No state change
	return (KE_MSG_CONSUMED);
}

//FLASH离线数据存储
//离线这里并不需要处理，全部交给了WXAPP进行处理
static int handler_modbus_5second_timtout(ke_msg_id_t const msgid, struct msg_struct_modbus_5second_timtout const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
	//no state change
	return (KE_MSG_CONSUMED);
}

//处理后台命令，瞬时读取
//GPS地址暂时保留，但ASYLIBRA并不发送给后台
static int handler_modbus_data_push(ke_msg_id_t const msgid, struct msg_struct_wxapp_modbus_data_push const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
	uint8_t UserCmdid = param->UserCmdId;
	if (UserCmdid == L3CI_pm1025)
	{
		uint8_t UserOpid = param->data[0];
		//瞬时读取数据
		if (UserOpid == L3PO_pm1025_req)
		{
			//先置状态
			modbusPm1025RequestCount++;
			modbusPm1025InstanceFlag = true;
			modbusUart1Buffer.readIdx = 0;
			modbusUart1Buffer.writeIdx = 0;
			//再发送REQ功能字给MODBUS
			uint8_t Pm1025DataReq[]   = {0x01,0x03,0x00,0x04,0x00,0x06,0x84,0x09};
			//重新赋值EQUIPMENT ID
			Pm1025DataReq[0] = MODBUS_PM1025_RTU_EQUIPMENT_ID;
			Pm1025DataReq[1] = MODBUS_PM1025_FUNC_DATA_INQUERY;
			//重新赋值初始寄存器(40004 -> PM1.0, 40006 -> PM2.5, 40008 -> PM10)
			//重新计算CRC
			bxxh_data_send_to_uart(Pm1025DataReq, sizeof(Pm1025DataReq));
			modbusZmanufatureSvr = param->EmDeviceDataType; //保存全局变量，以便后面发送时带回给ASYLIBRA并成帧
			
			//后面需要删掉，暂时保留，以便在MODBUS可用之前，用于汇报DUMMY的数据
//			struct msg_struct_modbus_data_req_pm1025_report m;
//			m.usercmdid = L3CI_pm1025;
//			m.length = sizeof(m);
//			m.useroptid = L3PO_pm1025_report;
//			m.pm1d0 = 1;
//			m.pm2d5 = 2;
//			m.pm10 = 3;
//			m.gps_x = 210;
//			m.gps_y = 211;
//			m.equipid = 1;
//			m.ntimes = 0;
//			m.EmDeviceDataType = param->EmDeviceDataType;
//			bxxh_message_send(MSG_MODBUS_DATA_REQ_PM1025_REPORT, TASK_ASYLIBRA, TASK_MODBUS, &m, m.length);
//			bxxh_led_flair();
			//删除结束
			bxxh_print_console("MODBUS: Send out instance command to UART1 and get PM1025 data! \r\n");
		}
		else
		{
			bxxh_print_console("MODBUS: Receive un-implemented user OperationId! \r\n");
		}
	} // UserCmdid = L3CI_pm1025
	else
	{
		//其它OPERATION命令, 暂时不考虑
		bxxh_print_console("MODBUS: Receive undefined user command! \r\n");
	}	
	//依然在连接态，故而继续保持，确保状态的稳定性
	ke_state_set(TASK_MODBUS, STATE_MODBUS_CONNECTED);
	return (KE_MSG_CONSUMED);
}

//定时到达，处理读取数据，送到ASYLIBRA，周期报告
//GPS地址暂时保留，但ASYLIBRA并不发送给后台
static int handler_modbus_period_report_pm1025(ke_msg_id_t const msgid, struct msg_struct_modbus_period_report_pm1025_trigger *param, ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
	if (ke_state_get(TASK_MODBUS) == STATE_MODBUS_CONNECTED)
	{
		//先置状态
		modbusPm1025RequestCount++;
		modbusPm1025InstanceFlag = false;
		modbusUart1Buffer.readIdx = 0;
		modbusUart1Buffer.writeIdx = 0;
		//再发送REQ功能字给MODBUS
		uint8_t Pm1025DataReq[]   = {0x01,0x03,0x00,0x04,0x00,0x06,0x84,0x09};
		//重新赋值EQUIPMENT ID
		Pm1025DataReq[0] = MODBUS_PM1025_RTU_EQUIPMENT_ID;
		Pm1025DataReq[1] = MODBUS_PM1025_FUNC_DATA_INQUERY;
		//重新赋值初始寄存器(40004 -> PM1.0, 40006 -> PM2.5, 40008 -> PM10)
		//重新计算CRC
		bxxh_data_send_to_uart(Pm1025DataReq, sizeof(Pm1025DataReq));
		
		//UART1外设工作正常后，需要删掉，暂时保留，以便在MODBUS可用之前，用于汇报DUMMY的数据
//		struct msg_struct_modbus_data_req_pm1025_report m;
//		m.usercmdid = L3CI_pm1025;
//		m.length = sizeof(m);
//		m.useroptid = L3PO_pm1025_report;
//		m.pm1d0 = 0;
//		m.pm2d5 = 0;
//		m.pm10 = 0;
//		m.gps_x = 110;
//		m.gps_y = 111;
//		m.equipid = 1;
//		m.ntimes = 1;
//		m.EmDeviceDataType = EDDT_manufatureSvr;
//		bxxh_message_send(MSG_MODBUS_DATA_REQ_PM1025_REPORT, TASK_ASYLIBRA, TASK_MODBUS, &m, m.length);
//		bxxh_led_flair();
		//删除结束
		bxxh_print_console("MODBUS: Send out period command to UART1 and get PM1025 data! \r\n");
	}
	else if (ke_state_get(TASK_MODBUS) == STATE_MODBUS_OFFLINE)
	{
//		//UART1外设工作正常后，需要删掉，暂时保留，以便在MODBUS可用之前，用于汇报DUMMY的数据
//		//存储到FLASH，贯穿写入数据
//		//离线数据启动UART1的逻辑，需要再处理
//		z_flash_data_sample[z_flash_data_sample_write].sensortype = L3CI_pm1025;
//		z_flash_data_sample[z_flash_data_sample_write].pm1d0 = 11;
//		z_flash_data_sample[z_flash_data_sample_write].pm2d5 = 12;
//		z_flash_data_sample[z_flash_data_sample_write].pm10 = 13;
//		z_flash_data_sample[z_flash_data_sample_write].timestamp = z_system_time_unix;
//		z_flash_data_sample[z_flash_data_sample_write].equipid = 1;
//		z_flash_data_sample_write++;
//		z_flash_data_sample_write = z_flash_data_sample_write % FLASH_DATA_SAMPLE_STORAGE_NBR_MAX;
//		//判断是否满，从而回写
//		if (z_flash_data_sample_nbr >= FLASH_DATA_SAMPLE_STORAGE_NBR_MAX)
//		{
//			z_flash_data_sample_nbr = FLASH_DATA_SAMPLE_STORAGE_NBR_MAX;
//			z_flash_data_sample_read++;
//			z_flash_data_sample_read = z_flash_data_sample_read % FLASH_DATA_SAMPLE_STORAGE_NBR_MAX;
//		}else{
//			z_flash_data_sample_nbr++;
//		}
//		//删除结束
	}
	//no state change
	return (KE_MSG_CONSUMED);
}

static int handler_modbus_period_report_wind_speed(ke_msg_id_t const msgid, struct msg_struct_modbus_period_report_wind_speed_trigger *param, ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
	//忽略COUNT读取是否一致的检查
	//modbusPm1025RequestCount++;
	//modbusPm1025InstanceFlag = false;
	modbusUart1Buffer.readIdx = 0;
	modbusUart1Buffer.writeIdx = 0;
	//再发送REQ功能字给MODBUS
	uint8_t WindSpeedDataReq[]   = {0x02,0x03,0x00,0x00,0x00,0x01,0x84,0x39};
	//重新赋值EQUIPMENT ID
	WindSpeedDataReq[0] = MODBUS_WIND_SPEED_RTU_EQUIPMENT_ID;
	WindSpeedDataReq[1] = MODBUS_GENERIC_FUNC_DATA_INQUERY;
	//重新计算CRC
	bxxh_data_send_to_uart(WindSpeedDataReq, sizeof(WindSpeedDataReq));
	bxxh_print_console("MODBUS: Send out period command to UART1 and get wind speed data! \r\n");	
	
	//no state change
	return (KE_MSG_CONSUMED);
}

static int handler_modbus_period_report_wind_direction(ke_msg_id_t const msgid, struct msg_struct_modbus_period_report_wind_direction_trigger *param, ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
	//忽略COUNT读取是否一致的检查
	modbusUart1Buffer.readIdx = 0;
	modbusUart1Buffer.writeIdx = 0;
	//再发送REQ功能字给MODBUS
	uint8_t WindDirectionDataReq[]   = {0x03,0x03,0x00,0x00,0x00,0x01,0x85,0xE8};
	//重新赋值EQUIPMENT ID
	WindDirectionDataReq[0] = MODBUS_WIND_DIRECTION_RTU_EQUIPMENT_ID;
	WindDirectionDataReq[1] = MODBUS_GENERIC_FUNC_DATA_INQUERY;
	//重新计算CRC
	bxxh_data_send_to_uart(WindDirectionDataReq, sizeof(WindDirectionDataReq));
	bxxh_print_console("MODBUS: Send out period command to UART1 and get wind direction data! \r\n");	
	
	//no state change
	return (KE_MSG_CONSUMED);
}

static int handler_modbus_period_report_temperature(ke_msg_id_t const msgid, struct msg_struct_modbus_period_report_temperature_trigger *param, ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
	//忽略COUNT读取是否一致的检查
	modbusUart1Buffer.readIdx = 0;
	modbusUart1Buffer.writeIdx = 0;
	//再发送REQ功能字给MODBUS
	uint8_t TemperatureDataReq[]   = {0x06,0x03,0x00,0x00,0x00,0x02,0xC5,0xBC};
	//重新赋值EQUIPMENT ID
	TemperatureDataReq[0] = MODBUS_TEMPERATURE_RTU_EQUIPMENT_ID;
	TemperatureDataReq[1] = MODBUS_GENERIC_FUNC_DATA_INQUERY;
	//重新计算CRC
	bxxh_data_send_to_uart(TemperatureDataReq, sizeof(TemperatureDataReq));
	bxxh_print_console("MODBUS: Send out period command to UART1 and get temperature data! \r\n");	
	
	//no state change
	return (KE_MSG_CONSUMED);
}

static int handler_modbus_period_report_humidity(ke_msg_id_t const msgid, struct msg_struct_modbus_period_report_humidity_trigger *param, ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
	//忽略COUNT读取是否一致的检查
	modbusUart1Buffer.readIdx = 0;
	modbusUart1Buffer.writeIdx = 0;
	//再发送REQ功能字给MODBUS
	uint8_t HumidityDataReq[]   = {0x06,0x03,0x00,0x00,0x00,0x02,0xC5,0xBC};
	//重新赋值EQUIPMENT ID
	HumidityDataReq[0] = MODBUS_HUMIDITY_RTU_EQUIPMENT_ID;
	HumidityDataReq[1] = MODBUS_GENERIC_FUNC_DATA_INQUERY;
	//重新计算CRC
	bxxh_data_send_to_uart(HumidityDataReq, sizeof(HumidityDataReq));
	bxxh_print_console("MODBUS: Send out period command to UART1 and get temperature data! \r\n");	
	
	//no state change
	return (KE_MSG_CONSUMED);
}

//链路终端，进入OFFLINE状态
static int handler_modbus_disconnect(ke_msg_id_t const msgid, struct msg_struct_wxapp_modbus_disconnect const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
	//计数器停止，FLASH数据不能接着回传了
	//10MINUTES定时器不停，因为离线后依然在采样数据，只是无法发送出去而已
	bxxh_timer_clear(MSG_MODBUS_5SECOND_TIMEOUT, TASK_MODBUS);
	//设置到初始状态
	ke_state_set(TASK_MODBUS, STATE_MODBUS_OFFLINE);	
	return (KE_MSG_CONSUMED);
}

//收到APP_SPS_SCHEDULER发来的UART1数据，进行处理
static int handler_modbus_uart1_data_received(ke_msg_id_t const msgid, struct msg_struct_app_server_uart1_data_received const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id)
{	
	bxxh_timer_set(MSG_MODBUS_UART1_FRAME_TIMEOUT, TASK_MODBUS, TIMER_MODBUS_UART1_FRAME);	
	return (KE_MSG_CONSUMED);
}

//UART1成帧，数据收全后，进行统一处理
//这是未能实现SLEEP的代价
static int handler_modbus_uart1_frame_timtout(ke_msg_id_t const msgid, struct msg_struct_modbus_uart1_frame_timtout const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
	//重置状态
	modbusUart1Buffer.readIdx = 0;
	modbusUart1Buffer.writeIdx = 0;
	//解码接收数据
	unsigned int pm1d0, pm2d5, pm10;
	uint16_t windspeed, winddirection, temperature, humidity, equipid;
	//打印完整的接收BUFFER
	bxxh_print_console("MODBUS: Data received from UART1, buffer = ");	
	int i=0;
	for (i=0; i<20; i++)  //应该使用，这里只输出一部分而已，APP_SPS_UART1_DATA_LENGTH_MAX_BUF
	{
		sprintf(pDebugPrintBuf, "%02X ", modbusUart1Buffer.data[i]);
		bxxh_print_console(pDebugPrintBuf);
	}
	bxxh_print_console("\r\n");	
	//判定CRC
	//判定功能码是否合法
	if (modbusUart1Buffer.data[1] != MODBUS_PM1025_FUNC_DATA_INQUERY)
	{
		bxxh_print_console("MODBUS: Error function code of data received from UART1. \r\n");		
		return (KE_MSG_CONSUMED);
	}
	//判定长度是否合法，肯定不能超过数据缓存长度，不然收到的数据就不全
	if (modbusUart1Buffer.data[2] >= APP_SPS_UART1_DATA_LENGTH_MAX_BUF)
	{
		bxxh_print_console("MODBUS: Error length of data received from UART1. \r\n");
		return (KE_MSG_CONSUMED);
	}
	//参考接收数据格式：
	// 01 03 0C x1(04H) x2(04L) x3(05H) x4(05L) y1(06H) y2(06L) y3(07H) y4(07L) z1(08H) z2(08L) z3(09H) z4(09L) CRC1 CRC2

	//通过从地址进行判定分类，其中PM1025进行完整的处理，而气象4参数将进行简化处理
	switch (modbusUart1Buffer.data[0])
	{
		case MODBUS_PM1025_RTU_EQUIPMENT_ID:
		{
			bxxh_print_console("MODBUS: Received right PM1025 data from UART1! \r\n");
			//然后再进入数据的处理过程，这里的数据采集命令是固定的，只采集连续6个地址的数据
			equipid = modbusUart1Buffer.data[0];
			unsigned int t0, t1, t2, t3;
			t0 = modbusUart1Buffer.data[3];
			t1 = modbusUart1Buffer.data[4];
			t2 = modbusUart1Buffer.data[5];
			t3 = modbusUart1Buffer.data[6];
			t0 = (t0 <<8) & 0xFF00;
			t1 = t1 & 0xFF;
			t2 = (t2 <<24) & 0xFF000000;
			t3 = (t3 << 16) & 0xFF0000;
			pm1d0 = t0 + t1 + t2 + t3;
			t0 = modbusUart1Buffer.data[7];
			t1 = modbusUart1Buffer.data[8];
			t2 = modbusUart1Buffer.data[9];
			t3 = modbusUart1Buffer.data[10];
			t0 = (t0 <<8) & 0xFF00;
			t1 = t1 & 0xFF;
			t2 = (t2 <<24) & 0xFF000000;
			t3 = (t3 << 16) & 0xFF0000;
			pm2d5 = t0 + t1 + t2 + t3;
			t0 = modbusUart1Buffer.data[11];
			t1 = modbusUart1Buffer.data[12];
			t2 = modbusUart1Buffer.data[13];
			t3 = modbusUart1Buffer.data[14];
			t0 = (t0 <<8) & 0xFF00;
			t1 = t1 & 0xFF;
			t2 = (t2 <<24) & 0xFF000000;
			t3 = (t3 << 16) & 0xFF0000;
			pm10 = t0 + t1 + t2 + t3;	
			//判断收发成对
			modbusPm1025ResponseCount++;
			if (modbusPm1025ResponseCount != modbusPm1025RequestCount)
			{
				bxxh_print_console("MODBUS: Receive data from UART1 error with response-id not equval to request-id. \r\n");
				modbusPm1025ResponseCount = modbusPm1025RequestCount;
			}		
			//如果是连接态，瞬时发送
			if ((ke_state_get(TASK_MODBUS) == STATE_MODBUS_CONNECTED) && (modbusPm1025InstanceFlag == true))
			{
				struct msg_struct_modbus_data_req_pm1025_report m;
				m.usercmdid = L3CI_pm1025;
				m.length = sizeof(m);
				m.useroptid = L3PO_pm1025_report;
				m.pm1d0 = pm1d0;
				m.pm2d5 = pm2d5;
				m.pm10 = pm10;
				m.gps_x = 210;
				m.gps_y = 211;
				m.equipid = equipid;
				m.ntimes = 0;
				m.EmDeviceDataType = modbusZmanufatureSvr;
				bxxh_message_send(MSG_MODBUS_DATA_REQ_PM1025_REPORT, TASK_ASYLIBRA, TASK_MODBUS, &m, m.length);
				//bxxh_led_flair();
			}
			//如果是连接态，周期发送
			else if ((ke_state_get(TASK_MODBUS) == STATE_MODBUS_CONNECTED) && (modbusPm1025InstanceFlag == false))
			{
				struct msg_struct_modbus_data_req_pm1025_report m;
				m.usercmdid = L3CI_pm1025;
				m.length = sizeof(m);
				m.useroptid = L3PO_pm1025_report;
				m.pm1d0 = pm1d0;
				m.pm2d5 = pm2d5;
				m.pm10 = pm10;
				m.gps_x = 110;
				m.gps_y = 111;
				m.equipid = equipid;
				m.ntimes = 1;
				m.EmDeviceDataType = EDDT_manufatureSvr; //周期发送，肯定不是命令来的
				bxxh_message_send(MSG_MODBUS_DATA_REQ_PM1025_REPORT, TASK_ASYLIBRA, TASK_MODBUS, &m, m.length);
				//bxxh_led_flair();
			}
			else if (ke_state_get(TASK_MODBUS) == STATE_MODBUS_OFFLINE)
			{
				//存储到FLASH，贯穿写入数据
				z_flash_data_sample[z_flash_data_sample_write].sensortype = L3CI_pm1025;
				z_flash_data_sample[z_flash_data_sample_write].pm1d0 = pm1d0;
				z_flash_data_sample[z_flash_data_sample_write].pm2d5 = pm2d5;
				z_flash_data_sample[z_flash_data_sample_write].pm10 = pm10;
				z_flash_data_sample[z_flash_data_sample_write].timestamp = z_system_time_unix;
				z_flash_data_sample[z_flash_data_sample_write].equipid = equipid;
				z_flash_data_sample_write++;
				z_flash_data_sample_write = z_flash_data_sample_write % FLASH_DATA_SAMPLE_STORAGE_NBR_MAX;
				//判断是否满，从而回写
				if (z_flash_data_sample_nbr >= FLASH_DATA_SAMPLE_STORAGE_NBR_MAX)
				{
					z_flash_data_sample_nbr = FLASH_DATA_SAMPLE_STORAGE_NBR_MAX;
					z_flash_data_sample_read++;
					z_flash_data_sample_read = z_flash_data_sample_read % FLASH_DATA_SAMPLE_STORAGE_NBR_MAX;
				}else{
					z_flash_data_sample_nbr++;
				}
			}
			break;
		}
		
		case MODBUS_WIND_SPEED_RTU_EQUIPMENT_ID:
		{
			bxxh_print_console("MODBUS: Received right wind speed data from UART1! \r\n");
			//然后再进入数据的处理过程,CRC暂时忽略
			equipid = modbusUart1Buffer.data[0];
			uint16_t t0, t1;
			t0 = modbusUart1Buffer.data[3];
			t1 = modbusUart1Buffer.data[4];
			t0 = (t0 <<8) & 0xFF00;
			t1 = t1 & 0xFF;
			windspeed = t0 + t1;
			//如果是连接态，周期发送，而且是特别假设为周期发送，因为不支持瞬时模式
			if (ke_state_get(TASK_MODBUS) == STATE_MODBUS_CONNECTED)
			{
				struct msg_struct_modbus_data_req_wind_speed_report m;
				m.usercmdid = L3CI_wind_speed;
				m.length = sizeof(m);
				m.useroptid = L3PO_pm1025_report;  //复用PM1025的操作字
				m.wind_speed = windspeed;
				m.equipid = equipid;
				m.ntimes = 1;
				m.EmDeviceDataType = EDDT_manufatureSvr; //周期发送，肯定不是命令来的
				bxxh_message_send(MSG_MODBUS_DATA_REQ_WIND_SPEED_REPORT, TASK_ASYLIBRA, TASK_MODBUS, &m, m.length);
				//bxxh_led_flair();
			}//online
			else if (ke_state_get(TASK_MODBUS) == STATE_MODBUS_OFFLINE)
			{
				//存储到FLASH，贯穿写入数据
				z_flash_data_sample[z_flash_data_sample_write].sensortype = L3CI_wind_speed;
				z_flash_data_sample[z_flash_data_sample_write].wind_speed = windspeed;
				z_flash_data_sample[z_flash_data_sample_write].timestamp = z_system_time_unix;
				z_flash_data_sample[z_flash_data_sample_write].equipid = equipid;
				z_flash_data_sample_write++;
				z_flash_data_sample_write = z_flash_data_sample_write % FLASH_DATA_SAMPLE_STORAGE_NBR_MAX;
				//判断是否满，从而回写
				if (z_flash_data_sample_nbr >= FLASH_DATA_SAMPLE_STORAGE_NBR_MAX)
				{
					z_flash_data_sample_nbr = FLASH_DATA_SAMPLE_STORAGE_NBR_MAX;
					z_flash_data_sample_read++;
					z_flash_data_sample_read = z_flash_data_sample_read % FLASH_DATA_SAMPLE_STORAGE_NBR_MAX;
				}else{
					z_flash_data_sample_nbr++;
				}
			}//offline
		
			break;
		}
		case MODBUS_WIND_DIRECTION_RTU_EQUIPMENT_ID:
		{
			bxxh_print_console("MODBUS: Received right wind direction data from UART1! \r\n");
			//然后再进入数据的处理过程,CRC暂时忽略
			equipid = modbusUart1Buffer.data[0];
			uint16_t t0, t1;
			t0 = modbusUart1Buffer.data[3];
			t1 = modbusUart1Buffer.data[4];
			t0 = (t0 <<8) & 0xFF00;
			t1 = t1 & 0xFF;
			winddirection = t0 + t1;
			//如果是连接态，周期发送，而且是特别假设为周期发送，因为不支持瞬时模式
			if (ke_state_get(TASK_MODBUS) == STATE_MODBUS_CONNECTED)
			{
				struct msg_struct_modbus_data_req_wind_direction_report m;
				m.usercmdid = L3CI_wind_direction;
				m.length = sizeof(m);
				m.useroptid = L3PO_pm1025_report;  //复用PM1025的操作字
				m.wind_direction = winddirection;
				m.equipid = equipid;
				m.ntimes = 1;
				m.EmDeviceDataType = EDDT_manufatureSvr; //周期发送，肯定不是命令来的
				bxxh_message_send(MSG_MODBUS_DATA_REQ_WIND_DIRECTION_REPORT, TASK_ASYLIBRA, TASK_MODBUS, &m, m.length);
				//bxxh_led_flair();
			}//online
			else if (ke_state_get(TASK_MODBUS) == STATE_MODBUS_OFFLINE)
			{
				//存储到FLASH，贯穿写入数据
				z_flash_data_sample[z_flash_data_sample_write].sensortype = L3CI_wind_direction;
				z_flash_data_sample[z_flash_data_sample_write].wind_direction = winddirection;
				z_flash_data_sample[z_flash_data_sample_write].timestamp = z_system_time_unix;
				z_flash_data_sample[z_flash_data_sample_write].equipid = equipid;
				z_flash_data_sample_write++;
				z_flash_data_sample_write = z_flash_data_sample_write % FLASH_DATA_SAMPLE_STORAGE_NBR_MAX;
				//判断是否满，从而回写
				if (z_flash_data_sample_nbr >= FLASH_DATA_SAMPLE_STORAGE_NBR_MAX)
				{
					z_flash_data_sample_nbr = FLASH_DATA_SAMPLE_STORAGE_NBR_MAX;
					z_flash_data_sample_read++;
					z_flash_data_sample_read = z_flash_data_sample_read % FLASH_DATA_SAMPLE_STORAGE_NBR_MAX;
				}else{
					z_flash_data_sample_nbr++;
				}
			}//offline
			break;
		}
		case MODBUS_TEMPERATURE_RTU_EQUIPMENT_ID:  //same as humidity
		{
			if (zModbusTemperatureOrHumidity == true) //温度读取
			{
				bxxh_print_console("MODBUS: Received right temperature data from UART1! \r\n");
				//然后再进入数据的处理过程,CRC暂时忽略
				equipid = modbusUart1Buffer.data[0];
				uint16_t t0, t1;
				t0 = modbusUart1Buffer.data[5];
				t1 = modbusUart1Buffer.data[6];
				t0 = (t0 <<8) & 0xFF00;
				t1 = t1 & 0xFF;
				temperature = t0 + t1;
				//如果是连接态，周期发送，而且是特别假设为周期发送，因为不支持瞬时模式
				if (ke_state_get(TASK_MODBUS) == STATE_MODBUS_CONNECTED)
				{
					struct msg_struct_modbus_data_req_temperature_report m;
					m.usercmdid = L3CI_temperature;
					m.length = sizeof(m);
					m.useroptid = L3PO_pm1025_report;  //复用PM1025的操作字
					m.temperature = temperature;
					m.equipid = equipid;
					m.ntimes = 1;
					m.EmDeviceDataType = EDDT_manufatureSvr; //周期发送，肯定不是命令来的
					bxxh_message_send(MSG_MODBUS_DATA_REQ_TEMPERATURE_REPORT, TASK_ASYLIBRA, TASK_MODBUS, &m, m.length);
//					bxxh_led_flair();
				}//online
				else if (ke_state_get(TASK_MODBUS) == STATE_MODBUS_OFFLINE)
				{
					//存储到FLASH，贯穿写入数据
					z_flash_data_sample[z_flash_data_sample_write].sensortype = L3CI_temperature;
					z_flash_data_sample[z_flash_data_sample_write].temperature = temperature;
					z_flash_data_sample[z_flash_data_sample_write].timestamp = z_system_time_unix;
					z_flash_data_sample[z_flash_data_sample_write].equipid = equipid;
					z_flash_data_sample_write++;
					z_flash_data_sample_write = z_flash_data_sample_write % FLASH_DATA_SAMPLE_STORAGE_NBR_MAX;
					//判断是否满，从而回写
					if (z_flash_data_sample_nbr >= FLASH_DATA_SAMPLE_STORAGE_NBR_MAX)
					{
						z_flash_data_sample_nbr = FLASH_DATA_SAMPLE_STORAGE_NBR_MAX;
						z_flash_data_sample_read++;
						z_flash_data_sample_read = z_flash_data_sample_read % FLASH_DATA_SAMPLE_STORAGE_NBR_MAX;
					}else{
						z_flash_data_sample_nbr++;
					}
				}//offline					
			}//温度
			else if (zModbusTemperatureOrHumidity == false) //湿度读取
			{
				bxxh_print_console("MODBUS: Received right humidity data from UART1! \r\n");
				//然后再进入数据的处理过程,CRC暂时忽略
				equipid = modbusUart1Buffer.data[0];
				uint16_t t0, t1;
				t0 = modbusUart1Buffer.data[3];
				t1 = modbusUart1Buffer.data[4];
				t0 = (t0 <<8) & 0xFF00;
				t1 = t1 & 0xFF;
				humidity = t0 + t1;
				//如果是连接态，周期发送，而且是特别假设为周期发送，因为不支持瞬时模式
				if (ke_state_get(TASK_MODBUS) == STATE_MODBUS_CONNECTED)
				{
					struct msg_struct_modbus_data_req_humidity_report m;
					m.usercmdid = L3CI_humidity;
					m.length = sizeof(m);
					m.useroptid = L3PO_pm1025_report;  //复用PM1025的操作字
					m.humidity = humidity;
					m.equipid = equipid;
					m.ntimes = 1;
					m.EmDeviceDataType = EDDT_manufatureSvr; //周期发送，肯定不是命令来的
					bxxh_message_send(MSG_MODBUS_DATA_REQ_HUMIDITY_REPORT, TASK_ASYLIBRA, TASK_MODBUS, &m, m.length);
//					bxxh_led_flair();
				}//online
				else if (ke_state_get(TASK_MODBUS) == STATE_MODBUS_OFFLINE)
				{
					//存储到FLASH，贯穿写入数据
					z_flash_data_sample[z_flash_data_sample_write].sensortype = L3CI_humidity;
					z_flash_data_sample[z_flash_data_sample_write].humidity = humidity;
					z_flash_data_sample[z_flash_data_sample_write].timestamp = z_system_time_unix;
					z_flash_data_sample[z_flash_data_sample_write].equipid = equipid;
					z_flash_data_sample_write++;
					z_flash_data_sample_write = z_flash_data_sample_write % FLASH_DATA_SAMPLE_STORAGE_NBR_MAX;
					//判断是否满，从而回写
					if (z_flash_data_sample_nbr >= FLASH_DATA_SAMPLE_STORAGE_NBR_MAX)
					{
						z_flash_data_sample_nbr = FLASH_DATA_SAMPLE_STORAGE_NBR_MAX;
						z_flash_data_sample_read++;
						z_flash_data_sample_read = z_flash_data_sample_read % FLASH_DATA_SAMPLE_STORAGE_NBR_MAX;
					}else{
						z_flash_data_sample_nbr++;
					}
				}//offline									
			}//湿度
			break;
		}
		default:
		{
			bxxh_print_console("MODBUS: Error equipment ID of data received from UART1. \r\n");		
			//return (KE_MSG_CONSUMED);  //到达不了的语句，优化掉
			break;
		}
	}
	
	return (KE_MSG_CONSUMED);	
}
#endif