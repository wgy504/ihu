/**
 ****************************************************************************************
 *
 * @file l3ccl.h
 *
 * @brief L3 CCL
 *
 * BXXH team
 * Created by ZJL, 20160918
 *
 ****************************************************************************************
 */

#ifndef L3APPL_L3CCL_H_
#define L3APPL_L3CCL_H_

#include "l1comdef_freeos.h"
#include "vmfreeoslayer.h"
#include "l1timer_freeos.h"
#include "l1vmfo.h"
#include "l2i2caries.h"
#include "l2spsvirgo.h"
#include "l2didocap.h"
#include "l2ledpisces.h"
#include "l2dcmiaris.h"
#include "huixmlcodec.h"
#include "bsp_dido.h"
#include "bsp_pwr_sav.h"

//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
//#define FSM_STATE_COMMON  0x02
enum FSM_STATE_CCL
{
	FSM_STATE_CCL_INITED = FSM_STATE_COMMON + 1,  //IHU被唤醒，开始工作。开的原因可能是触发电源，也可能是定时触发
	FSM_STATE_CCL_ACTIVE,
	FSM_STATE_CCL_CLOUD_INQUERY,
	FSM_STATE_CCL_TO_OPEN_DOOR,
	FSM_STATE_CCL_DOOR_OPEN,
	FSM_STATE_CCL_FATAL_FAULT,
	FSM_STATE_CCL_EVENT_REPORT,
	//FSM_STATE_CCL_SLEEP,
	FSM_STATE_CCL_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF


//CONST常量
#define IHU_CCL_ALARM_FAULT_PERIOD_DURATION    	1 //10   	//In minutes  正常为10分钟，未来待修改
#define IHU_CCL_ALARM_NORMAL_PERIOD_DURATION    3 //8*60  //In minutes  正常为8小时，未来待修改



//Global variables
extern IhuFsmStateItem_t IhuFsmCcl[];

//Local Variables
//传感器在CCL上，由于涉及到消息数据结构，必须固定住，不然数据结构没法弄
//DIDO: 门限DOOR，锁舌LOCKI，锁控LOCKO，温湿度数据DHT，水浸WATER，烟感SMOKE，电量数据BAT
//SPS：将通过串口将RSSI数据报上来
//I2C：暂时预留两路空余的传感器
//GPRS-SPS
//BLE-SPS
//RFID-SPS
//LED+BEEP: GPIO，都混装在LED模块中

//本地需要用到的核心参数
#define  IHU_CCL_PICTURE_BUFFER_SIZE_MAX 10000 //10KB, in BYTES
typedef struct strIhuCclCtrlContext
{
	com_sensor_status_t sensor;
	UINT16 faultReportCnt;  //防止向后台送报告过于频繁，这里做一个分频控制
	UINT8  picBuf[IHU_CCL_PICTURE_BUFFER_SIZE_MAX];
	UINT32 picActualPkgSize;
	bool   handActFlag;
}strIhuCclTaskContext_t;
#define IHU_CCL_FALULT_REPORT_TIMES_MAX 200

extern strIhuCclTaskContext_t zIhuCclTaskContext;

//API
extern OPSTAT fsm_ccl_task_entry(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_ccl_init(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_ccl_restart(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_ccl_stop_rcv(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_ccl_time_out(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_ccl_dido_sensor_status_resp(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_ccl_sps_sensor_status_resp(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_ccl_i2c_sensor_status_resp(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_ccl_dcmi_sensor_status_resp(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_ccl_sps_event_report_cfm(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
	
extern OPSTAT fsm_ccl_sps_cloud_fb(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_ccl_dido_door_open_event(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_ccl_door_close_event(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
//extern OPSTAT fsm_ccl_dido_event_status_update(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_ccl_sps_close_door_report_cfm(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
	
extern OPSTAT fsm_ccl_hand_active_trigger_to_work(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
//extern OPSTAT fsm_ccl_event_fault_trigger_to_stop(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_ccl_sps_fault_report_cfm(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);

extern OPSTAT fsm_ccl_period_report_trigger(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_ccl_fault_state_trigger(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);


//External APIs
extern int8_t func_vmmw_rtc_pcf8563_set_alarm_process(int16_t duration);
extern bool func_vmmw_rtc_pcf8563_judge_alarm_happen(void);
extern int16_t func_vmmw_rtc_pcf8563_get_alarm_duration(void);
extern OPSTAT ihu_dcmiaris_take_picture(UINT8 cameraId);
extern void func_vmmw_rtc_pcf8563_clear_af_and_power_off_cpu(void);


//Local API
OPSTAT func_ccl_hw_init(void);
void func_ccl_time_out_period_event_report(void);
//void func_ccl_time_out_event_report_period_scan(void);
OPSTAT func_ccl_time_out_lock_work_active(void);
OPSTAT func_ccl_time_out_lock_work_wait_door_for_open(void);
void func_ccl_close_all_sensor_power(void);
void func_ccl_open_all_sensor_power(void);
void func_cccl_cpu_power_off(void);
void func_ccl_stm_main_recovery_from_fault(void);  //提供了一种比RESTART更低层次的状态恢复方式



//高级定义，简化程序的可读性，包括return IHU_FAILURE在内的宏定义，没搞定。。。
#define IHU_ERROR_PRINT_CCL_WO_RETURN zIhuSysStaPm.taskRunErrCnt[TASK_ID_CCL]++; func_ccl_stm_main_recovery_from_fault(); IhuErrorPrint
#define IHU_ERROR_PRINT_CCL_RECOVERY(...)	do{zIhuSysStaPm.taskRunErrCnt[TASK_ID_CCL]++;  func_ccl_stm_main_recovery_from_fault(); IhuErrorPrint(__VA_ARGS__);  return IHU_FAILURE;}while(0)	

#endif /* L3APPL_L3CCL_H_ */

