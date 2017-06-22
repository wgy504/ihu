/**
 ****************************************************************************************
 *
 * @file l3iap.h
 *
 * @brief L3 IAP
 *
 * BXXH team
 * Created by ZJL, 20160918
 *
 ****************************************************************************************
 */

#ifndef L3APPL_L3IAP_H_
#define L3APPL_L3IAP_H_

#include "l1comdef_freeos.h"
#include "vmfreeoslayer.h"
#include "l1timer_freeos.h"
#include "l1vmfo.h"
#include "l2spsvirgo.h"
#include "l2ledpisces.h"
#include "huixmlcodec.h"
#include "bsp_dido.h"

//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
//#define FSM_STATE_COMMON  0x02
enum FSM_STATE_IAP
{
	FSM_STATE_IAP_INITED = FSM_STATE_COMMON + 1, 
	FSM_STATE_IAP_ACTIVE,
	FSM_STATE_IAP_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF


//CONST常量


//Global variables
extern IhuFsmStateItem_t IhuFsmIap[];

//本地需要用到的核心参数
typedef struct strIhuIapCtrlContext
{
	bool   testFlag;
}strIhuIapTaskContext_t;

extern strIhuIapTaskContext_t zIhuIapTaskContext;

//API
extern OPSTAT fsm_iap_task_entry(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_iap_init(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_iap_restart(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_iap_stop_rcv(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_iap_time_out(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);


//External APIs


//Local API
OPSTAT func_iap_hw_init(void);
void func_iap_time_out_period_event_report(void);
void func_iap_stm_main_recovery_from_fault(void);  //提供了一种比RESTART更低层次的状态恢复方式


//高级定义，简化程序的可读性，包括return IHU_FAILURE在内的宏定义，没搞定。。。
#define IHU_ERROR_PRINT_IAP_WO_RETURN zIhuSysStaPm.taskRunErrCnt[TASK_ID_IAP]++; func_iap_stm_main_recovery_from_fault(); IhuErrorPrint
#define IHU_ERROR_PRINT_IAP_RECOVERY(...)	do{zIhuSysStaPm.taskRunErrCnt[TASK_ID_IAP]++;  func_iap_stm_main_recovery_from_fault(); IhuErrorPrint(__VA_ARGS__);  return IHU_FAILURE;}while(0)	

#endif /* L3APPL_L3IAP_H_ */

