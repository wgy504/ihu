/**
 ****************************************************************************************
 *
 * @file l2i2caries.h
 *
 * @brief L2 I2CARIES
 *
 * BXXH team
 * Created by ZJL, 20160918
 *
 ****************************************************************************************
 */

#ifndef L2FRAME_L2I2CARIES_H_
#define L2FRAME_L2I2CARIES_H_

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
#else
	#error Un-correct constant definition
#endif


//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
//#define FSM_STATE_COMMON  0x02
enum FSM_STATE_I2CARIES
{
	FSM_STATE_I2CARIES_INITED = FSM_STATE_COMMON + 1,
	FSM_STATE_I2CARIES_ACTIVED,
	FSM_STATE_I2CARIES_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Global variables
extern IhuFsmStateItem_t IhuFsmI2caries[];

//本地需要用到的核心参数
typedef struct strIhuBfscI2cMotoPar
{
	INT32 speed;  //设置的速度
	UINT8 turnDir;  //转动的方向
	UINT8 turnMode;  //是否转动
}strIhuBfscI2cMotoPar_t;

#define IHU_BFSC_I2C_MOTO_TURN_DIRECTION_NONE 0
#define IHU_BFSC_I2C_MOTO_TURN_DIRECTION_NOR 1
#define IHU_BFSC_I2C_MOTO_TURN_DIRECTION_REV 2
#define IHU_BFSC_I2C_MOTO_TURN_MODE_NONE 0
#define IHU_BFSC_I2C_MOTO_TURN_MODE_START 1
#define IHU_BFSC_I2C_MOTO_TURN_MODE_STOP 2

//I2C工作状态
#define IHU_CCL_I2C_WORKING_MODE_NONE 0
#define IHU_CCL_I2C_WORKING_MODE_SLEEP 1
#define IHU_CCL_I2C_WORKING_MODE_ACTIVE 2
#define IHU_CCL_I2C_WORKING_MODE_FAULT 3
#define IHU_CCL_I2C_WORKING_MODE_INVALID 0xFF

//本地需要用到的核心参数
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)
	typedef struct strIhuCclI2cPar
	{	
		UINT8 cclI2cWorkingMode;
		com_sensor_status_t sensor;
	}strIhuCclI2cPar_t;
#endif

//控制定时器定时扫描是否启动
#define IHU_I2CARIES_PERIOD_TIMER_ACTIVE 1
#define IHU_I2CARIES_PERIOD_TIMER_DEACTIVE 2
#define IHU_I2CARIES_PERIOD_TIMER_SET IHU_I2CARIES_PERIOD_TIMER_DEACTIVE	
	
//API
extern OPSTAT fsm_i2caries_task_entry(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_i2caries_init(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_i2caries_restart(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_i2caries_stop_rcv(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_i2caries_time_out(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_i2caries_bfsc_moto_cmd_ctrl(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_i2caries_bfsc_l2frame_rcv(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_i2caries_ccl_sensor_status_req(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_i2caries_ccl_ctrl_cmd(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);

//全局函数
extern INT16 ihu_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_rsv1_value(void);
extern INT16 ihu_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_rsv2_value(void);
	
	
//Local API
OPSTAT func_i2caries_hw_init(void);
void func_i2caries_time_out_period_scan(void);
void func_i2caries_bfsc_moto_control(INT8 mode, INT8 dir);
void func_i2caries_bfsc_moto_set_speed(INT32 speed);
void func_i2caries_bfsc_frame_send(strIhuI2cariesMotoFrame_t *frame);


//GPRS/BLE/RFIDMOD/NAVIGMOD的模组操作
extern int8_t  ihu_vmmw_navig_mpu6050_init(void);
extern float   ihu_wmmw_navig_mpu6050_axis_z_angle_caculate_by_static_method(void);


//高级定义，简化程序的可读性
#define IHU_ERROR_PRINT_I2CARIES(...)	do{zIhuSysStaPm.taskRunErrCnt[TASK_ID_I2CARIES]++;  IhuErrorPrint(__VA_ARGS__);  return IHU_FAILURE;}while(0)	


#endif /* L2FRAME_L2I2CARIES_H_ */

