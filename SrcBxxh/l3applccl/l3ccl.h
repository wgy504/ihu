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

//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
enum FSM_STATE_CCL
{
	FSM_STATE_CCL_INITED = 0x02,  //IHU被唤醒，开始工作。开的原因可能是触发电源，也可能是定时触发
	FSM_STATE_CCL_CLOUD_INQUERY,
	FSM_STATE_CCL_TO_OPEN_DOOR,
	FSM_STATE_CCL_DOOR_OPEN,
	FSM_STATE_CCL_FATAL_FAULT,
	FSM_STATE_CCL_EVENT_REPORT,
	FSM_STATE_CCL_SLEEP,
	FSM_STATE_CCL_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Global variables
extern FsmStateItem_t FsmCcl[];

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
typedef struct strIhuCclCtrlPar
{
	com_sensor_status_t sensor;
	UINT16  faultReportCnt;  //防止向后台送报告过于频繁，这里做一个分频控制
}strIhuCclCtrlPar_t;
#define IHU_CCL_FALULT_REPORT_TIMES_MAX 200

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
extern OPSTAT fsm_ccl_lock_and_door_close_event(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_ccl_dido_event_status_update(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_ccl_sps_close_door_report_cfm(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
	
extern OPSTAT fsm_ccl_event_lock_trigger_to_work(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_ccl_event_fault_trigger_to_stop(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_ccl_sps_fault_report_cfm(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);

//Local API
OPSTAT func_ccl_hw_init(void);
void func_ccl_time_out_period_event_report(void);
void func_ccl_time_out_event_report_period_scan(void);
OPSTAT func_ccl_time_out_lock_work_active(void);
OPSTAT func_ccl_time_out_lock_work_wait_door_for_open(void);
void func_ccl_close_all_sensor(void);
void func_ccl_open_all_sensor(void);

	

#endif /* L3APPL_L3CCL_H_ */

