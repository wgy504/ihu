/*
 * vmfreeoslayer.h
 *
 *  Created on: 2016年9月18日
 *      Author: test
 */

#ifndef L1VMFREEOS_VMFREEOSLAYER_H_
#define L1VMFREEOS_VMFREEOSLAYER_H_

//自行标准配置
#include "comtype.h"
#include "sysdim.h"
#include "commsgscycb.h"
#include "sysconfig.h"
#include "sysengpar.h"
#include "sysversion.h"


//标准库
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

/*
 *	
 *  定义结构体
 *
 */

//定义TASK对应的名字
//!!!!!!!!!!!!!!!!ATTENTION!!!!!!!!!!!!!!!!
//Whenever the Task Id is changed, QueID and vmlayer.c/zIhuTaskNameList array should be updated, otherwise error will happen!!!
//不是任务的任务，比如TRACE/CONFIG/3G/GPIO等等，留待清理，简化任务列表
/*
 *
 *   【增加任务】，必须同时修改四个地方：
 *   - IHU_TASK_NAME_ID
 *   - IHU_TASK_QUEUE_ID
 *   - zIhuTaskNameList
 *   - 还要修改可能的本地配置文件，或者sysengpar.h的固定工参配置信息
 *
 */
enum IHU_TASK_NAME_ID
{
	TASK_ID_MIN = 0,
	TASK_ID_VMFO,
	TASK_ID_TIMER,
	TASK_ID_ADCLIBRA,
	TASK_ID_SPILEO,
	TASK_ID_I2CARIES,
	TASK_ID_PWMTAURUS,
	TASK_ID_SPSVIRGO,
	TASK_ID_GPIOCANCER,
	TASK_ID_DIDOCAP,
	TASK_ID_LEDPISCES,
	TASK_ID_ETHORION,	
	TASK_ID_SCYCB,
	TASK_ID_MAX,
	TASK_ID_INVALID = 0xFF,
}; //end of IHU_TASK_NAME_ID

//定义TASK对应的MESSAGE_QUEUE的ID
enum IHU_TASK_QUEUE_ID
{
	TASK_QUE_ID_MIN = IHU_TASK_QUEUE_ID_START,
	TASK_QUE_ID_VMFO,
	TASK_QUE_ID_TIMER,
	TASK_QUE_ID_ADCLIBRA,
	TASK_QUE_ID_SPILEO,
	TASK_QUE_ID_I2CARIES,
	TASK_QUE_ID_PWMTAURUS,
	TASK_QUE_ID_SPSVIRGO,
	TASK_QUE_ID_GPIOCANCER,
	TASK_QUE_ID_DIDOCAP,
	TASK_QUE_ID_LEDPISCES,
	TASK_QUE_ID_ETHORION,	
	TASK_QUE_ID_SCYCB,
	TASK_QUE_ID_MAX,
	TASK_QUE_ID_INVALID = 0xFF,
}; //end of IHU_TASK_QUEUE_ID


/*
** Fsm INFORMATION structure.
*/
#define FSM_STATE_ENTRY  0x00
#define FSM_STATE_IDLE  0x01
#define FSM_STATE_END   0xFE
#define FSM_STATE_INVALID 0xFF



//FSM的基础结构定义
typedef struct FsmStateItem
{
	UINT16 msg_id;
	UINT8 state;
	OPSTAT (*stateFunc)(UINT8 dest_id, UINT8 src_id, void *param_ptr, UINT16 param_len);
}FsmStateItem_t;

typedef struct IhuTaskTag
{
	UINT32 TaskId;
	UINT8  pnpState;
	INT32  QueId;
	UINT8  state;
	char   TaskName[TASK_NAME_MAX_LENGTH];
	FsmStateItem_t *fsmPtr;
	UINT8 QueFullFlag;
}IhuTaskTag_t;
#define IHU_TASK_PNP_ON 2
#define IHU_TASK_PNP_OFF 1
#define IHU_TASK_PNP_INVALID 0xFF
#define IHU_TASK_QUEUE_FULL_TRUE 2
#define IHU_TASK_QUEUE_FULL_FALSE 1
#define IHU_TASK_QUEUE_FULL_INVALID 0xFF

typedef struct FsmArrayElement
{
	OPSTAT (*stateFunc)(UINT8 dest_id, UINT8 src_id, void *param_ptr, UINT16 param_len);
}FsmArrayElement_t;

typedef struct FsmCtrlTable
{
	UINT8 numOfFsmArrayElement;  //每一个具体任务TASK中，定义了多少个STATE-MSGID映射表单
	UINT8 taskId;
	FsmArrayElement_t pFsmArray[MAX_STATE_NUM_IN_ONE_TASK][MAX_MSGID_NUM_IN_ONE_TASK];
}FsmCtrlTable_t;

//MsgQueue的基础结构定义
typedef struct FsmQueueElement
{
	UINT8 msgQue[MAX_IHU_MSG_BODY_LENGTH];
	bool useFlag;
}FsmQueueElement_t;

typedef struct FsmQueueListTable
{
	FsmQueueElement_t queList[MAX_QUEUE_NUM_IN_ONE_TASK];
	UINT8 queIndex;
}FsmQueueListTable_t;

//FSM的总结构定义
typedef struct FsmTable
{
	UINT8 numOfFsmCtrlTable;  //Number of running (Task + Instance)
	UINT8 currentTaskId;  //transfer task_id to launched FSM machine, then useless
	FsmCtrlTable_t  pFsmCtrlTable[MAX_TASK_NUM_IN_ONE_IHU];  //所有任务的状态机总控表
	FsmQueueListTable_t taskQue[MAX_TASK_NUM_IN_ONE_IHU];  //所有任务的消息队列总控表
}FsmTable_t;

//任务模块RESTART的一些全局定义
#define IHU_RUN_ERROR_LEVEL_0_WARNING 10
#define IHU_RUN_ERROR_LEVEL_1_MINOR 100
#define IHU_RUN_ERROR_LEVEL_2_MAJOR 1000
#define IHU_RUN_ERROR_LEVEL_3_CRITICAL 10000
#define IHU_RUN_ERROR_LEVEL_4_DEAD 100000

//全局Counter，用于性能指标统计之用
typedef struct IhuGlobalCounter
{
	UINT32 errCnt[MAX_TASK_NUM_IN_ONE_IHU];  //以每个任务为单位
	UINT32 restartCnt;
}IhuGlobalCounter_t;
extern IhuGlobalCounter_t zIhuGlobalCounter;


/*
 *	
 *  全局API
 *  UINT16 param_len: 考虑到单片机下的实时操作系统，U16长度是最为合适的，如果没有意外，以后不得修改
 *
 */
//全局API，不依赖具体操作系统
extern void IhuDebugPrint(char *format, ...);
extern void IhuErrorPrint(char *format, ...);
extern void ihu_vm_system_init(void);  //系统级别的初始化
extern void ihu_sleep(UINT32 second);
extern void ihu_usleep(UINT32 usecond);  //resulution 10^(-6)s = 1 microsecond
uint16_t b2l_uint16(uint16_t in);
extern OPSTAT ihu_taskid_to_string(UINT8 id, char *string);
extern OPSTAT ihu_msgid_to_string(UINT16 id, char *string);


//VM FSM related APIs，状态机核心部分，不依赖具体操作系统
extern OPSTAT FsmInit(void);
extern OPSTAT FsmAddNew(UINT8 task_id, FsmStateItem_t* pFsmStateItem);
extern OPSTAT FsmRemove(UINT8 task_id);
extern OPSTAT FsmRunEngine(UINT16 msg_id, UINT8 dest_id, UINT8 src_id, void *param_ptr, UINT16 param_len);
extern OPSTAT FsmProcessingLaunch(void);
extern OPSTAT FsmProcessingLaunchEntry(UINT8 task_id);
extern OPSTAT FsmProcessingLaunchExecute(UINT8 task_id);
extern OPSTAT FsmSetState(UINT8 task_id, UINT8 newState);
extern UINT8  FsmGetState(UINT8 task_id);


//Global VM layer basic API and functions，跟具体操作系统相关的API部分
extern OPSTAT ihu_msgque_create(UINT8 task_id);
extern OPSTAT ihu_msgque_delete(UINT8 task_id);
UINT32 ihu_msgque_inquery(UINT8 task_id);
extern OPSTAT ihu_msgque_resync(void);
extern OPSTAT ihu_message_queue_clean(UINT8 dest_id);
extern OPSTAT ihu_task_create(UINT8 task_id, void *(*task_func)(void *), void *arg, int prio);
extern OPSTAT ihu_task_delete(UINT8 task_id);

extern OPSTAT ihu_message_rcv(UINT8 dest_id, IhuMsgSruct_t *msg);
extern OPSTAT ihu_message_send(UINT16 msg_id, UINT8 dest_id, UINT8 src_id, void *param_ptr, UINT16 param_len); //message send

extern OPSTAT ihu_system_task_init_call(UINT8 task_id, FsmStateItem_t *p);
extern OPSTAT ihu_system_task_execute(UINT8 task_id, FsmStateItem_t *p);
extern OPSTAT ihu_task_create_and_run(UINT8 task_id, FsmStateItem_t* pFsmStateItem);
extern void   ihu_task_create_all(void);
extern void   ihu_task_execute_all(void);
extern OPSTAT fsm_com_do_nothing(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern void ihu_sw_restart(void);
extern struct tm ihu_clock_unix_to_ymd(time_t t_unix);








/*
 *	
 *  全局TIMER定义
 *
 */
//Granularity of 1second and 10ms timer set
#define IHU_TIMER_TICK_1_SEC 1      //in second
#define IHU_TIMER_TICK_10_MS 10000  //in u second
#define IHU_TIMER_TICK_1_MS 1000  //in u second
#define IHU_TIMER_TICK_10_MS_IN_NS 10000000  //in n second
#define IHU_TIMER_TICK_1_MS_IN_NS 1000000  //in n second
#define IHU_TIMER_TICK_100_MS_IN_NS 100000000  //in n second

//Timer Id definition
enum IHU_TIMER_ID_1S_ALL
{
	TIMER_ID_1S_MIN = 0,
	TIMER_ID_1S_VMDA_INIT_FB,
	TIMER_ID_1S_ASYLIBRA_AUTH_RESP_WAIT,
	TIMER_ID_1S_ASYLIBRA_INIT_RESP_WAIT,
	TIMER_ID_1S_ASYLIBRA_DATA_RESP_WAIT,	
	TIMER_ID_1S_EMC_PERIOD_READ,
	TIMER_ID_1S_MAX,
};
enum IHU_TIMER_ID_10MS_ALL
{
	TIMER_ID_10MS_MIN = 0,
	TIMER_ID_10MS_VMDA_TEST,
	TIMER_ID_10MS_MAX,
};
enum IHU_TIMER_ID_1MS_ALL
{
	TIMER_ID_1MS_MIN = 0,
	TIMER_ID_1MS_VMDA_TEST,
	TIMER_ID_1MS_MAX,
};

//System level timer global variables
typedef struct IhuTimerElement
{
	UINT32 timerId;
	UINT32 taskId;
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

#define TIMER_RESOLUTION_1S 1
#define TIMER_RESOLUTION_10MS 2
#define TIMER_RESOLUTION_1MS 3
#define TIMER_RESOLUTION_INVALID 0xFF


/*
 *	
 *  外部引用的全局变量
 *
 */

//Global variables
extern IhuTaskTag_t zIhuTaskInfo[MAX_TASK_NUM_IN_ONE_IHU];  //任务控制总表
extern UINT32 zIhuRunErrCnt[MAX_TASK_NUM_IN_ONE_IHU];       //差错表
extern FsmTable_t zIhuFsmTable;                             //状态机总表
extern char *zIhuTaskNameList[MAX_TASK_NUM_IN_ONE_IHU];     //任务名字符串
extern char *zIhuMsgNameList[MAX_MSGID_NUM_IN_ONE_TASK];    //消息名字符串
extern IhuSysEngParTable_t zIhuSysEngPar;                   //工参
extern IhuTimerTable_t zIhuTimerTable;                      //定时器
extern time_t zIhuSystemTimeUnix;                           //系统时钟TimeStamp
extern struct tm zIhuSystemTimeYmd;                        	//系统时钟YMD
extern UINT32 zIhuSleepCnt[MAX_TASK_NUM_IN_ONE_IHU][MAX_SLEEP_NUM_IN_ONE_TASK];  //睡眠控制表
extern FsmStateItem_t FsmVmdashell[];                            //状态机
extern FsmStateItem_t FsmTimer[];                           //状态机
extern FsmStateItem_t FsmAsylibra[];                        //状态机
extern FsmStateItem_t FsmAdcaries[];                        //状态机
extern FsmStateItem_t FsmEmc[];                             //状态机

//外部引用API，来自于TIMER任务模块。TIMER任务模块的机制是，必须将VM启动起来，然后TIMER上层任务模块才能被激活，并产生自定义的TIME_OUT消息
extern OPSTAT ihu_timer_start(UINT8 task_id, UINT8 timer_id, UINT32 t_dur, UINT8 t_type, UINT8 t_res);
extern OPSTAT ihu_timer_stop(UINT8 task_id, UINT8 timer_id, UINT8 t_res);
extern void ihu_timer_routine_handler_1s(void);
extern void ihu_timer_routine_handler_10ms(void);

//extern int sprintf(char * __restrict /*s*/, const char * __restrict /*format*/, ...) __attribute__((__nonnull__(1,2)));
//extern int arch_printf(const char *fmt, ...);

#endif /* L1VMFREEOS_VMFREEOSLAYER_H_ */
