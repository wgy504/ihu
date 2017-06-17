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

#define WS_BETA_DEN   4  // DEN=1<<4
#define WS_BETA_NUM1  15
#define WS_BETA_NUM2  10

int weight_sensor_map_adc_to_weight(uint32_t adc_value);
int weight_sensor_send_cmd(uint32_t type);
int weight_sensor_recv_cmd(weight_sensor_cmd_t *command);
uint32_t weight_sensor_read_and_filtering(weight_sensor_filter_t *wsf);
void weight_sensor_task(void const *param);

// ======= ADDED BY MA YUCHU =======
// Make sure in the vmfreeoslayer.c have the following line:
//  {TIMER_ID_10MS_BFSC_ADCLIBRA_SCAN_TIMER,         "TID_10MS_BFSC_ADCLIBRA_SCAN_TIMER",      20,      TIMER_RESOLUTION_10MS}, 
//                                                                                             ^^
//	UINT32	WeightSensorLoadDetectionTimeMs;
//	UINT32	WeightSensorLoadThread;
//	UINT32	WeightSensorEmptyThread;
//	UINT32	WeightSensorEmptyDetectionTimeMs;
//	UINT32	MaxAllowedWeight;										

//	UINT32	WeightSensorStaticZeroValue;				
//	UINT32	WeightSensorTailorValue;						
//	UINT32	WeightSensorDynamicZeroThreadValue;	
//	UINT32	WeightSensorDynamicZeroHysteresisMs;

#define 	WEIGHT_SENSOR_ADC_READ_TICK_MS										(200)     // MAKE SURE   200/10 = 20   || 
#define 	WEIGHT_SENSOR_MOVING_AVERAGE_TICKS								(5)
#define 	WEIGHT_SENSOR_MAX_TICKS_SAVED											(64)


#define		WEIGHT_SENSOR_LOAD_DETECTION_TIME_MS							(1000)		
#define		WEIGHT_SENSOR_LOAD_THREDSHOLD											(200)			// for 2g with 0.01 granuality
#define		WEIGHT_SENSOR_EMPTY_DETECTION_TIME_MS							(1000)		
#define		WEIGHT_SENSOR_EMPTY_DETECTION_THREDSHOLD					(200)			// for 2g with 0.01 granuality
#define		WEIGHT_SENSOR_MAX_ALLOWED_WEIGHT									(1000000)	// 10kg

#define		WEIGHT_SENSOR_STATIC_ZERO_VALUE										(0)
#define		WEIGHT_SENSOR_TAILOR_VALUE												(0)

#define		WEIGHT_SENSOR_DYNAMIC_ZERO_THREDSHOLD_VALUE				(0)			// 1g
#define		WEIGHT_SENSOR_DYNAMIC_ZERO_HYSTERESIS_MS					(10000)		// 5s

/*
** Paramter checking !!!!! make sure these parameter relationship: 
** (WEIGHT_SENSOR_DYNAMIC_ZERO_HYSTERESIS_MS / WEIGHT_SENSOR_ADC_READ_TICK_MS) < WEIGHT_SENSOR_MAX_TICKS_SAVED
** (WEIGHT_SENSOR_LOAD_DETECTION_TIME_MS / WEIGHT_SENSOR_ADC_READ_TICK_MS) < WEIGHT_SENSOR_MAX_TICKS_SAVED
** (WEIGHT_SENSOR_EMPTY_DETECTION_TIME_MS / WEIGHT_SENSOR_ADC_READ_TICK_MS) < WEIGHT_SENSOR_MAX_TICKS_SAVED
** !!!! If these parameter are not checked, memory will be corrupt !!!!
*/


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
	//ADDED BY MA YUCHU
	INT32		WeightValueRaw[WEIGHT_SENSOR_MOVING_AVERAGE_TICKS];
	UINT32 	WeightValueCurrentIndexMovingAverage;
	INT32 	WeightValueEvaluated[WEIGHT_SENSOR_MAX_TICKS_SAVED];
	INT32 	WeightValueAdjusted[WEIGHT_SENSOR_MAX_TICKS_SAVED];
	INT32 	WeightDynamicZeroValue;
	
	UINT32 	WeightValueLastLoadTicks;
	INT32 	WeightValueLastLoadValue;
	UINT32 	WeightValueCurrLoadTicks;
	INT32 	WeightValueCurrLoadValue;
	
	UINT32 	WeightValueLastEmptyTicks;
	INT32 	WeightValueLastEmptyValue;
	UINT32 	WeightValueCurrtEmptyTicks;
	INT32 	WeightValueCurrEmptyValue;

//#define 	WEIGHT_EVENT_ID_LOAD						(0)
//#define 	WEIGHT_EVENT_ID_EMPTY						(1)
//#define 	WEIGHT_EVENT_ID_PICKUP						(2)
	UINT32 	WeightLastEventType;
	UINT32 	WeightLastEventTicks;
	INT32 	WeightLastValue;	
	UINT32 	WeightCurrEventType;
	UINT32 	WeightCurrEventTicks;
	INT32 	WeightCurrValue;
	
	UINT32 	WeightCurrentTicks;  // Dimension: 2^32 = 4294967296*0.2s = 858993459.2s = 238609.29 hour = 9942 days = 27.2 years
	UINT32 	SysTicksMs;
	UINT32 	RepeatTimes;
	UINT32 	ConsecutiveTimes;
	
	UINT32	WeightSensorLoadDetectionTicks; // = WeightSensorLoadDetectionTimeMs / WEIGHT_SENSOR_ADC_READ_TICK_MS;
	UINT32 	WeightSensorEmptyDetectionTicks; // = WeightSensorEmptyDetectionTimeMs / WEIGHT_SENSOR_ADC_READ_TICK_MS;
	UINT32	WeightSensorDynamicZeroHysteresisTicks; // = WeightSensorDynamicZeroHysteresisMs / WEIGHT_SENSOR_ADC_READ_TICK_MS;
	
}strIhuBfscAdcWeightPar_t;

//=======================================================
//END: Local API from Xiong Puhui, for ADC Weight Filter
//=======================================================

#endif /* L2FRAME_L2ADCLIBRA_H_ */

