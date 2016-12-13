/**
 ****************************************************************************************
 *
 * @file l2ledpisces.h
 *
 * @brief L2 LEDPISCES
 *
 * BXXH team
 * Created by ZJL, 20160918
 *
 ****************************************************************************************
 */

#ifndef L2FRAME_L2LEDPISCES_H_
#define L2FRAME_L2LEDPISCES_H_

#include "sysdim.h"
#if ((IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_DA_EMC68X_ID) ||\
	(IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID) ||\
	(IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID))
	#include "vmfreeoslayer.h"
	#include "l1comdef_freeos.h"
	#include "l1timer_freeos.h"		
#elif ((IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_SCYCB_ID) ||\
	(IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_PLCCB_ID) ||\
	(IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_PLCSB_ID))
	#include "vmucoslayer.h"
	#include "l1comdef_ucos.h"
	#include "l1timer_ucos.h"
	#include "l1hd_led.h"
#else
#endif


//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
enum FSM_STATE_LEDPISCES
{
	FSM_STATE_LEDPISCES_INITED = 0x02,
	FSM_STATE_LEDPISCES_ACTIVED,
	FSM_STATE_LEDPISCES_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Global variables
extern FsmStateItem_t FsmLedpisces[];

//Local variables
typedef enum{
  BEEPState_OFF = 0,
	BEEPState_ON,
}StrBeepStateTypoeDef_t;

//纯测试目的，未来需要弄到CubeMX中去
//参看：硬石YS-F1Pro开发板开发手册（HAL库版本）
#define IHU_IS_BEEP_STATE(STATE) (((STATE) == BEEPState_OFF) || ((STATE) == BEEPState_ON))
#define IHU_BEEP_RCC_CLK_ENABLE() 					_HAL_RCC_GPIOD_CLK_ENABLE()
#define BEEP_GPIO_PIN 					GPIO_PIN_7
#define BEEP_GPIO 							GPIOD
#define IHU_BEEP_ON 						HAL_GPIO_WritePin(BEEP_GPIO,BEEP_GPIO_PIN,GPIO_PIN_SET) // 输出高电平
#define IHU_BEEP_OFF 						HAL_GPIO_WritePin(BEEP_GPIO,BEEP_GPIO_PIN,GPIO_PIN_RESET) // 输出低电平
#define IHU_BEEP_TOGGLE 				HAL_GPIO_TogglePin(BEEP_GPIO,BEEP_GPIO_PIN) // 输出反转

//API
extern OPSTAT fsm_ledpisces_task_entry(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_ledpisces_init(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_ledpisces_restart(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_ledpisces_stop_rcv(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_ledpisces_time_out(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);

//Local API
OPSTAT func_ledpisces_hw_init(void);
void func_ledpisces_time_out_period_scan(void);

#endif /* L2FRAME_L2LEDPISCES_H_ */

