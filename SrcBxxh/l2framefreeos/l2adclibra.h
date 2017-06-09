/**
 ****************************************************************************************
 *
 * @file l2adclibra.h
 *
 * @brief L2 ADCLIBRA
 *
 * BXXH team
 * Created by ZJL, 20160918
 *
 ****************************************************************************************
 */

#ifndef L2FRAME_L2ADCLIBRA_H_
#define L2FRAME_L2ADCLIBRA_H_

#include "sysdim.h"
#if ((IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_DA_EMC68X_ID) ||\
	(IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID) ||\
	(IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID))
	#include "vmfreeoslayer.h"
	#include "l1comdef_freeos.h"
	#include "l1timer_freeos.h"	
	#include "bsp_adc.h"	
	
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
enum FSM_STATE_ADCLIBRA
{
	FSM_STATE_ADCLIBRA_INITED = FSM_STATE_COMMON + 1,
	FSM_STATE_ADCLIBRA_ACTIVED,
	FSM_STATE_ADCLIBRA_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Global variables
extern IhuFsmStateItem_t IhuFsmAdclibra[];

//本地需要用到的核心参数
typedef struct strIhuBfscAdcWeightPar
{
	INT32 WeightExistCnt;  //用于BFSC项目中判定扫描到的重物，是否一直有重量的情形
	INT32 Weightvalue;  //用于BFSC项目中称重结果
	INT32 WeightBasket;  //皮重
	INT32 WeightManSetZero;  //手动清零重量
	UINT8 WeightZeroTrackMode;  //这个值的设定，将会使得每一次称重，自动清零，判定的标准是单体重量小于设定目标重量的10%s
	INT32 WeightMaxScale;   //重量最大量程
	INT32 WeightMinSens;    //最小灵敏度值
	INT32 WeightStaticRange; //静态量程范围
	INT32 WeightStaticDur;  //静态时间时长
	INT32 WeightCal0Kg;        //0KG的校准
	INT32 WeightCal1Kg;         //1KG的校准
	UINT8 WeightWorkingMode;  //秤工作模式
}strIhuBfscAdcWeightPar_t;

#define IHU_BFSC_ADC_WEIGHT_ZERO_TRACK_MODE_NONE 0
#define IHU_BFSC_ADC_WEIGHT_ZERO_TRACK_MODE_ACTIVE 1

#define IHU_BFSC_ADC_WEIGHT_WORKING_MODE_NONE 0
#define IHU_BFSC_ADC_WEIGHT_WORKING_MODE_CAL 1
#define IHU_BFSC_ADC_WEIGHT_WORKING_MODE_NORMAL 2
#define IHU_BFSC_ADC_WEIGHT_WORKING_MODE_STOP 3

//ADC工作状态
#define IHU_CCL_ADC_WORKING_MODE_NONE 0
#define IHU_CCL_ADC_WORKING_MODE_SLEEP 1
#define IHU_CCL_ADC_WORKING_MODE_ACTIVE 2
#define IHU_CCL_ADC_WORKING_MODE_FAULT 3
#define IHU_CCL_ADC_WORKING_MODE_INVALID 0xFF

//本地需要用到的核心参数
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)
	typedef struct strIhuCclAdcPar
	{	
		UINT8 cclAdcWorkingMode;
		com_sensor_status_t sensor;
	}strIhuCclAdcPar_t;
#endif

//CCL项目中所需要使用到的电池告警范围	
#define IHU_CCL_ADC_BATTERY_WARNING_MIN_VALUE			420 //对应4.2v的水平
#define IHU_CCL_ADC_BATTERY_WARNING_MAX_VALUE			800 //对应8v的水平	

//控制定时器定时扫描是否启动
#define IHU_ADCLIBRA_PERIOD_TIMER_ACTIVE 1
#define IHU_ADCLIBRA_PERIOD_TIMER_DEACTIVE 2
#define IHU_ADCLIBRA_PERIOD_TIMER_SET IHU_ADCLIBRA_PERIOD_TIMER_ACTIVE

	
//API
extern OPSTAT fsm_adclibra_task_entry(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_adclibra_init(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_adclibra_restart(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_adclibra_stop_rcv(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_adclibra_time_out(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_adclibra_l3bfsc_ws_cmd_ctrl(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_adclibra_canvela_ws_man_set_zero(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);

extern bool ihu_adclibra_ccl_scan_battery_warning_level(void);	
	
//Local API
OPSTAT func_adclibra_hw_init(void);
void func_adclibra_time_out_period_scan(void);
OPSTAT func_adclibra_time_out_bfsc_read_weight_scan(void);
INT32 func_adclibra_bfsc_read_weight(void);
INT32 func_adclibra_bfsc_read_origin(void);

//=======================================================
//START: Local API from Xiong Puhui, for ADC Weight Filter
//=======================================================

#define WIGHT_SENSOR_CMD_TYPE_STOP 0
#define WIGHT_SENSOR_CMD_TYPE_START 1

typedef struct weight_sensor_cmd_s
{
  uint32_t valid:1;
  uint32_t type:7;
  uint32_t reserved:24;
}weight_sensor_cmd_t;

typedef struct weight_sensor_filter_s
{
  int32_t adc_filtered[2];
  uint32_t beta_num[2];
  uint32_t stable_thresh;
  uint32_t change_thresh;
}weight_sensor_filter_t;

int weight_sensor_map_adc_to_weight(uint32_t adc_value);
int weight_sensor_send_cmd(uint32_t type);
int weight_sensor_recv_cmd(weight_sensor_cmd_t *command);
uint32_t weight_sensor_read_and_filtering(weight_sensor_filter_t *wsf);
void weight_sensor_task(void const *param);
//=======================================================
//END: Local API from Xiong Puhui, for ADC Weight Filter
//=======================================================

#endif /* L2FRAME_L2ADCLIBRA_H_ */

