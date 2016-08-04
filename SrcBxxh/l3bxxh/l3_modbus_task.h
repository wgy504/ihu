/**
 ****************************************************************************************
 *
 * @file L3_modbus_task.h
 *
 * @brief Layer 3 modbus task
 *
 * BXXH team
 * Created by ZJL, 20150819
 *
 ****************************************************************************************
 */
#if (CFG_MODBUS)
#include <stdint.h>
#include "ke_task.h"
#include "time.h"

void task_modbus_init(void);
void task_modbus_mainloop(void);

/// Number of WXAPP Task Instances
#define MODBUS_IDX_MAX                 (1)

/// States of MODBUS task
enum MODBUS_STATE
{
	/// Disabled State
	STATE_MODBUS_DISABLED,
	/// Connectable state
	STATE_MODBUS_OFFLINE,
	/// Security state,
	//STATE_MODBUS_SECURITY,
	/// Connected state
	STATE_MODBUS_CONNECTED,
	/// Number of defined states.
	STATE_MODBUS_MAX,
};


//Internal procedure
#define TIMER_MODBUS_10MIN       1200	//实质为1分钟，缺省工作时间并非10分钟，5种传感器一起轮，总共1分钟轮到一次
#define TIMER_MODBUS_5SECOND     200	//实质为2s，用于FLASH数据的回传
#define TIMER_MODBUS_UART1_FRAME  50	//500ms，用于UART1数据的成帧
//MODBUS从地址定义，在本程序中全部设定死
#define MODBUS_PM1025_RTU_EQUIPMENT_ID  1
#define MODBUS_WIND_SPEED_RTU_EQUIPMENT_ID  2
#define MODBUS_WIND_DIRECTION_RTU_EQUIPMENT_ID  3
#define MODBUS_TEMPERATURE_RTU_EQUIPMENT_ID  6
#define MODBUS_HUMIDITY_RTU_EQUIPMENT_ID  6
#define MODBUS_GENERIC_FUNC_DATA_INQUERY  0x03
#define MODBUS_PM1025_FUNC_DATA_INQUERY  0x03

//Modbus PM1025用户自定义操作字
//目前，气象4参数共享PM1025的所有自定义操作字，但控制字肯定是不一样的
typedef enum
{
	L3PO_none = 0,
	L3PO_pm1025_req = 0x01, //PM3 Request
	L3PO_set_pm1025_switch = 0x02,
	L3PO_set_modbus_address =0x03,
	L3PO_set_work_cycle = 0x04, //In second
	L3PO_set_sample_cycle = 0x05, //In second
	L3PO_set_sample_number = 0x06,
	L3PO_pm1025_report = 0x81,
	L3PO_read_pm1025_switch = 0x82,
	L3PO_read_modbus_address = 0x83,
	L3PO_read_work_cycle = 0x84, //In second
	L3PO_read_sample_cycle = 0x85, //In second
	L3PO_read_sample_number = 0x86,	
}L3Pm1025OptId;

#endif