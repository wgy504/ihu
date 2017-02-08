/**
 ****************************************************************************************
 *
 * @file l1timer_freeos.h
 *
 * @brief L1 TIMER FreeRTOS
 *
 * BXXH team
 * Created by ZJL, 20160127
 *
 ****************************************************************************************
 */

#ifndef L1VMFREEOS_L1TIMER_FREEOS_H_
#define L1VMFREEOS_L1TIMER_FREEOS_H_

#include "vmfreeoslayer.h"
#include "l1comdef_freeos.h"
	
//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
enum FSM_STATE_TIMER
{
	FSM_STATE_TIMER_INITED = 0x02,
	FSM_STATE_TIMER_AVTIVE,
	FSM_STATE_TIMER_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF


/*
 *	
 *  全局TIMER常量定义
 *
 */
//当前就是1ms的定时时钟！！！
//Granularity of 1second and 10ms timer set
#define IHU_TIMER_TICK_1_SEC 1000      //in second
#define IHU_TIMER_TICK_10_MS 10  //in ms second
#define IHU_TIMER_TICK_1_MS 1  //in ms second
#define IHU_TIMER_TICK_10_MS_IN_NS 10000000  //in n second
#define IHU_TIMER_TICK_1_MS_IN_NS 1000000  //in n second
#define IHU_TIMER_TICK_100_MS_IN_NS 100000000  //in n second

//Timer Id definition：后面统一使用节省内存的方式进行定义
//#define MAX_TIMER_NUM_IN_ONE_IHU_1S 40  //Normal situation
//#define MAX_TIMER_NUM_IN_ONE_IHU_10MS 4 //Frame usage
//#define MAX_TIMER_NUM_IN_ONE_IHU_1MS 4  //Real-time usage
#define MAX_TIMER_SET_DURATION 100000

#define MAX_TIMER_NUM_IN_ONE_IHU_1S TIMER_ID_1S_MAX - TIMER_ID_1S_MIN + 1  //Normal situation
#define MAX_TIMER_NUM_IN_ONE_IHU_10MS TIMER_ID_10MS_MAX - TIMER_ID_10MS_MIN + 1  //Frame usage
#define MAX_TIMER_NUM_IN_ONE_IHU_1MS TIMER_ID_1MS_MAX - TIMER_ID_1MS_MIN + 1  //Real-time usage

//System level timer global variables
typedef struct IhuTimerElement
{
	UINT8  timerId;
	UINT8  taskId;
	UINT32 tDuration;
	UINT8  timerType;
	UINT8  timerRes; //Timer Resolution
	UINT32 elapse;
	UINT8  status;
}IhuTimerElement_t;
typedef struct IhuTimerTable
{
	IhuTimerElement_t timer1s[MAX_TIMER_NUM_IN_ONE_IHU_1S];
	IhuTimerElement_t timer10ms[MAX_TIMER_NUM_IN_ONE_IHU_10MS];
	IhuTimerElement_t timer1ms[MAX_TIMER_NUM_IN_ONE_IHU_1MS];
}IhuTimerTable_t;

#define TIMER_STATUS_DEACTIVE 1
#define TIMER_STATUS_ACTIVE 2
#define TIMER_STATUS_START 0xFD
#define TIMER_STATUS_STOP 0xFE
#define TIMER_STATUS_INVALID 0xFF

#define TIMER_TYPE_ONE_TIME 1
#define TIMER_TYPE_PERIOD 2
#define TIMER_TYPE_INVALID 0xFF

//Global variables
extern IhuFsmStateItem_t IhuFsmTimer[];
extern IhuTimerTable_t zIhuTimerTable;

//API
extern OPSTAT fsm_timer_task_entry(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_timer_init(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_timer_restart(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_timer_stop_rcv(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
//用法中，TIMER_DUR就是多少秒为单位
extern OPSTAT ihu_timer_start(UINT8 task_id, UINT8 timer_id, UINT32 t_dur, UINT8 t_type, UINT8 t_res);
extern OPSTAT ihu_timer_stop(UINT8 task_id, UINT8 timer_id, UINT8 t_res);

//Local API
OPSTAT func_timer_hw_init(void);
void func_timer_routine_handler_1s(OS_TIMER timerid);
void func_timer_routine_handler_10ms(OS_TIMER timerid);
void func_timer_routine_handler_1ms(OS_TIMER timerid);

#endif /* L1VMFREEOS_L1TIMER_FREEOS_H_ */

