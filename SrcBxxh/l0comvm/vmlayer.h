/*
 * vmlayer.h
 *
 *  Created on: 2016年1月3日
 *      Author: test
 */

#ifndef L0COMVM_VMLAYER_H_
#define L0COMVM_VMLAYER_H_

//自行标准配置
#include "commsg.h"
#include "sysdim.h"
#include "sysconfig.h"
#include "sysengpar.h"

//标准库
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

//DA系统配置文件
#include "rwip_config.h"
//#include "da14580_config.h"
//#include "app_console.h"

//DA系统核心API
#include "ke_msg.h"
#include "ke_task.h"
#include "gpio.h"
#include "nvds.h"
#include "adc.h"

//DA系统应用API
//#include "sps_server_task.h"
#include "prf_types.h"
//#include "app_sps_scheduler.h"
//#include "sps_server_task.h"  //MSG_SPS_VMDA_BLE_DATA_RCV message incoming
#include "app_task.h"  //APPBLE_VMDA_INIT message incoming
#include "app.h"
#include "app_api.h"  //APPBLE_VMDA_INIT message incoming
#include "user_periph_setup.h"

//自定义应用API
//#include "i2c_led.h"

//Google/Tecent库
#include "epb_MmBp.h"
#include "crc32.h"
#include "aes_crypt.h"

/*
 *	
 *  定义结构体
 *
 */

// 定义TASK对应的名字
// [增加任务]，必须同时修改2个地方：IHU_TASK_NAME_ID，zIhuTaskNameList
enum IHU_TASK_NAME_ID
{
	TASK_ID_MIN = 0,
	TASK_ID_VMDASHELL,
	TASK_ID_TIMER,
	TASK_ID_ASYLIBRA,
	//TASK_ID_AKSLEO,
	TASK_ID_ADCARIES,
	TASK_ID_EMC,
	TASK_ID_MAX,
	TASK_ID_INVALID = 0xFF,
}; //end of IHU_TASK_NAME_ID

typedef struct IhuTaskTag
{
	UINT32 TaskId;
	UINT8  pnpState;
	UINT8  state;
	char   TaskName[TASK_NAME_MAX_LENGTH];
}IhuTaskTag_t;


//HW INVENTORY对应的信息
typedef struct IhuHwInvInfoTag
{
	bool led_on_off;
}IhuHwInvInfoTag_t;

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
	OPSTAT (*stateFunc)(UINT8 dest_id, UINT8 src_id, void *param_ptr, UINT8 param_len);
}FsmStateItem_t;
typedef struct FsmArrayElement
{
	OPSTAT (*stateFunc)(UINT8 dest_id, UINT8 src_id, void *param_ptr, UINT8 param_len);
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

//Local variables
#define VM_SLEEP_INIT_1SECOND_CNT 5000 //计数器，用于调节发送INIT消息的时间
#define VM_SLEEP_INIT_1SECOND_SEED 0

/*
 *	
 *  全局API
 *
 */

//跟DA14580 BLE底层交互的API
extern void bxxh_vm_init_hook(void);  //用于给DA148x的主程序挂载初始化的入口函数
extern void bxxh_vm_task_entry_hook(void);  //用于给DA148x的主程序挂载循环主任务的入口函数
extern void vmda1458x_message_send(ke_msg_id_t const msg_id, ke_task_id_t const dest_id, ke_task_id_t const src_id, void const *param_ptr, uint16_t const param_len); //message send
extern void vmda1458x_timer_set(ke_msg_id_t const timerid, ke_task_id_t const taskid, uint16_t delay);
extern void vmda1458x_timer_clear(ke_msg_id_t const timerid, ke_task_id_t const taskid);
extern void vmda1458x_data_send_to_ble(unsigned char *p, int n);
extern void vmda1458x_data_send_to_uart(unsigned char *p, int n);
extern int  vmda1458x_emc_sample(void);  //ADC0读数，可以通过被动式获取
extern void vmda1458x_led_flair(void);
extern void vmda1458x_init_msg_to_vmdashell(void);
extern OPSTAT vmda1458x_mac_addr_get(UINT8* mac, UINT8 len);


//外部引用的API
extern _ARMABI int sprintf(char * __restrict /*s*/, const char * __restrict /*format*/, ...) __attribute__((__nonnull__(1,2)));
extern int arch_printf(const char *fmt, ...);
//extern void bxxh_task_vmda_main_entry(void);
extern void ihu_task_vmdashell_init(void);
extern void ihu_task_vmdashell_mainloop(void);

//VM FSM related APIs
extern OPSTAT FsmInit(void);
extern OPSTAT FsmAddNew(UINT8 task_id, FsmStateItem_t* pFsmStateItem);
extern OPSTAT FsmRunEngine(UINT16 msg_id, UINT8 dest_id, UINT8 src_id, void *param_ptr, UINT8 param_len);
extern OPSTAT FsmProcessingLaunchEntry(UINT8 task_id);
extern OPSTAT FsmProcessingLaunchExecute(UINT8 task_id);
extern OPSTAT FsmSetState(UINT8 task_id, UINT8 newState);
extern UINT8  FsmGetState(UINT8 task_id);
extern OPSTAT fsm_com_do_nothing(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT8 param_len);

//Global VM layer basic API and functions
extern OPSTAT ihu_message_rcv(UINT8 dest_id, IhuMsgSruct_t *msg);
extern OPSTAT ihu_message_send(UINT16 msg_id, UINT8 dest_id, UINT8 src_id, void *param_ptr, UINT8 param_len); //message send
extern OPSTAT ihu_taskid_to_string(UINT8 id, char *string);
extern OPSTAT ihu_msgid_to_string(UINT16 id, char *string);
extern OPSTAT ihu_message_queue_clean(UINT8 dest_id);
extern OPSTAT ihu_system_task_init_call(UINT8 task_id, FsmStateItem_t *p);
extern OPSTAT ihu_system_task_execute(UINT8 task_id, FsmStateItem_t *p);
extern OPSTAT ihu_task_create_and_run(UINT8 task_id, FsmStateItem_t* pFsmStateItem);
extern void   ihu_task_create_all(void);
extern void   ihu_task_execute_all(void);
extern void ihu_timer_set(UINT16 const timerid, UINT16 const taskid, UINT16 delay);
extern void ihu_timer_clear(UINT16 const timerid, UINT16 const taskid);

//VMDA内部以及上层使用的API
extern void IhuDebugPrint(char *p);
extern void IhuErrorPrint(char *p);
uint16_t b2l_uint16(uint16_t in);
extern void ihu_sw_restart(void);
extern OPSTAT ihu_sleep(UINT32 cntDuration, UINT8 task_id, UINT8 seed);
extern void ihu_task_create_all(void);
extern struct tm ihu_clock_unix_to_ymd(time_t t_unix);
extern UINT16 ihu_emc_adc_read(void);
extern OPSTAT ihu_get_mac_addr(UINT8* mac);

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
extern IhuHwInvInfoTag_t zIhuHwInvInfo;                     //硬件清单
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

//外部引用API
extern OPSTAT ihu_timer_start(UINT8 task_id, UINT8 timer_id, UINT32 t_dur, UINT8 t_type, UINT8 t_res);
extern OPSTAT ihu_timer_stop(UINT8 task_id, UINT8 timer_id, UINT8 t_res);
extern void ihu_timer_routine_handler_1s(void);
extern void ihu_timer_routine_handler_10ms(void);
extern void vmda_app_uart_push_zjl(unsigned char *p, int n);
extern void vmda_app_ble_push(unsigned char *p, int n);

#endif /* L0COMVM_VMLAYER_H_ */
