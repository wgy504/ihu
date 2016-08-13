/**
 ****************************************************************************************
 *
 * @file vmdashell_task.h
 *
 * @brief VMDASHELL task
 *
 * BXXH team
 * Created by ZJL, 20160127
 *
 ****************************************************************************************
 */

#ifndef L0VMDA_VMDASHELL_TASK_H_
#define L0VMDA_VMDASHELL_TASK_H_

#include "vmlayer.h"

//按照DaWechat工程：Number of VMDASHELL Task Instances
#define IHU_VMDASHELL_IDX_MAX                 (1)

//按照DaWechat工程：States of VMDASHELL task
enum IHU_VMDASHELL_STATE
{
	IHU_STATE_VMDASHELL_MIN = 0,
	IHU_STATE_VMDASHELL_DISABLE,
	IHU_STATE_VMDASHELL_CONNECT,
	IHU_STATE_VMDASHELL_MAX,
};

//按照DaWechat工程：环境结构体
typedef struct ihu_vmdashell_env_tag
{
	struct prf_con_info con_info;
	ke_task_id_t appid;
	uint16_t conhdl;
	uint16_t shdl;
}ihu_vmdashell_env_tag_t;


//按照DaWechat工程：Local APIs //状态处理机
extern void ihu_task_vmdashell_init(void);
extern void ihu_task_vmdashell_mainloop(void);
OPSTAT hlFuncVmdashellBleInit(ke_msg_id_t const msgid, msg_struct_bleapp_vmda_init_t const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
OPSTAT hlFuncVmdashellSysClock1sPeriod(ke_msg_id_t const msgid, msg_struct_vmda_1s_period_timtout_t const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
OPSTAT hlFuncVmdashellSpsBleDataDlRcv(ke_msg_id_t const msgid, msg_struct_sps_vmda_ble_data_dl_rcv_t const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
OPSTAT hlFuncVmdashellBleDisc(ke_msg_id_t const msgid, msg_struct_bleapp_vmda_disconnect_t const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);


/*
 *  
 *  采用VM任务模块机制的状态转移机
 *  本模块内部的消息队列，将被VM进行持续扫描 
 *
 */
 
//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
enum FSM_STATE_VMDASHELL
{
	FSM_STATE_VMDASHELL_INITED = 0x02,
	FSM_STATE_VMDASHELL_ACTIVE,
	FSM_STATE_VMDASHELL_WF_INIT_FB,	
	FSM_STATE_VMDASHELL_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Global variables
extern FsmStateItem_t FsmVmdashell[];
//extern bool zVmdashellInitFbRcvCtrl[MAX_TASK_NUM_IN_ONE_IHU];

//Local variables
#define VMDASHELL_TIMER_1SECOND_CLOCK 100 //1s，用于时钟形成，这是采用了操作系统BXXH提供的方式
#define VMDASHELL_TIMER_DURATION_INIT_FB 3  //in second，这是采用了VM以上层的时钟形式

//API
extern OPSTAT fsm_vmdashell_task_entry(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT8 param_len);
extern OPSTAT fsm_vmdashell_task_init(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT8 param_len);
extern OPSTAT fsm_vmdashell_restart(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT8 param_len);
extern OPSTAT fsm_vmdashell_task_init_fb_rcv(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT8 param_len);
extern OPSTAT fsm_vmdashell_stop_rcv(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT8 param_len);
extern OPSTAT fsm_vmdashell_task_init_time_out(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT8 param_len);
extern OPSTAT fsm_vmdashell_data_ul_send(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT8 param_len);

//Local API
OPSTAT func_vmdashell_hw_init(void);
OPSTAT func_vmdashell_send_out_init(UINT8 task_id);
OPSTAT func_vmdashell_check_init_fb_complete(void);

#endif /* L0VMDA_VMDASHELL_TASK_H_ */
