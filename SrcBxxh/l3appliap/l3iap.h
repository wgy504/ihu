/**
 ****************************************************************************************
 *
 * @file l3iap.h
 *
 * @brief L3IAP
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
enum FSM_STATE_L3IAP
{
	FSM_STATE_L3IAP_INITED = FSM_STATE_COMMON + 1, 
	FSM_STATE_L3IAP_ACTIVE,
	FSM_STATE_L3IAP_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF


//CONST常量


//Global variables
extern IhuFsmStateItem_t IhuFsmL3iap[];

//本地需要用到的核心参数
//typedef struct strIhuL3iapCtrlContext
//{
//	bool   testFlag;
//}strIhuL3iapTaskContext_t;

//extern strIhuL3iapTaskContext_t zIhuL3iapTaskContext;

//API
extern OPSTAT fsm_l3iap_task_entry(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_l3iap_init(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_l3iap_restart(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_l3iap_stop_rcv(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_l3iap_time_out(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);

extern OPSTAT fsm_l3iap_sw_inventory_confirm(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);    
extern OPSTAT fsm_l3iap_sw_inventory_timeout(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_l3iap_sw_package_confirm(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_l3iap_sw_package_timeout(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);

//External APIs


//Local API
OPSTAT func_l3iap_hw_init(void);
void func_l3iap_time_out_period_event_report(void);
void func_l3iap_stm_main_recovery_from_fault(void);  //提供了一种比RESTART更低层次的状态恢复方式


//高级定义，简化程序的可读性，包括return IHU_FAILURE在内的宏定义，没搞定。。。
#define IHU_ERROR_PRINT_L3IAP_WO_RETURN zIhuSysStaPm.taskRunErrCnt[TASK_ID_L3IAP]++; func_l3iap_stm_main_recovery_from_fault(); IhuErrorPrint
#define IHU_ERROR_PRINT_L3IAP_RECOVERY(...)	do{zIhuSysStaPm.taskRunErrCnt[TASK_ID_L3IAP]++;  func_l3iap_stm_main_recovery_from_fault(); IhuErrorPrint(__VA_ARGS__);  return IHU_FAILURE;}while(0)	

/* 
** COMMOND DEFINES FOR IAP
*/
#define L3IAP_MAX_RETRANSMISSION_TIME              (8)  // TO BE UPDATE
#define L3IAP_RETRANSMISSION_TIMEOUT_MS            (5000)  // TO BE UPDATE
#define L3IAP_SEGMENT_REQUEST_INTERVAL_MS          (0)  // TO BE UPDATE
#define L3IAP_MAX_LOAD_SIZE_IN_BYTES               (200000)  // TO BE UPDATE

typedef struct strIhuL3iapCtrlContext
{
    UINT32    CurrentSegmentIndex;
    UINT32    CurrentSegmentTransTimes;
    UINT32    swTotalSegmentNumber;
    UINT32    swLengthInBytesPerSegment;
    UINT32    swLengthInBytesLastSegment;
    UINT32    SegmentRequestIntervalMs;
  	UINT16    swCalculatedCheckSum;

	  UINT16 	  swUpgradeType;					       //APP or FACTORY, or ... same as msg_struct_l3iap_iap_sw_upgrade_req
	  UINT16 	  swRelId;
	  UINT16 	  swVerId;
	  UINT16 	  swCheckSum;
	  UINT32    swTotalLengthInBytes;

	  UINT32    NextFlashSaveAddress;
}strIhuL3iapTaskContext_t;
/*
** Memory Map for 207VG with 1Mbytes FLASH
** 
** Sector 0    0x0800 0000 - 0x0800 3FFFF        16 kBytes      16 kbytes
** Sector 1    0x0800 4000 - 0x0800 7FFFF        16 kBytes      32 kbytes
** Sector 2    0x0800 8000 - 0x0800 BFFFF        16 kBytes      48 kbytes
** Sector 3    0x0800 C000 - 0x0800 FFFFF        16 kBytes      64 kbytes
**
** Sector 4    0x0801 0000 - 0x0800 FFFFF        64 kBytes     128 kbytes
**
** Sector 5    0x0802 0000 - 0x0803 FFFFF        128 kBytes    256 kbytes
** Sector 6    0x0804 0000 - 0x0805 FFFFF        128 kBytes    384 kbytes
** Sector 7    0x0806 0000 - 0x0807 FFFFF        128 kBytes    512 kbytes
** Sector 8    0x0808 0000 - 0x0809 FFFFF        128 kBytes    640 kbytes
** Sector 9    0x080A 0000 - 0x080B FFFFF        128 kBytes    768 kbytes
** Sector 10   0x080C 0000 - 0x080D FFFFF        128 kBytes    896 kbytes
** Sector 11   0x080E 0000 - 0x080F FFFFF        128 kBytes   1024 kbytes
**
*/
#define FLASH_ADDRESS_SW_CONTROL_TABLE             (0x080E0000)  // TO BE UPDATE
#define FLASH_ADDRESS_IAP_LOAD                     (0x08000000)
#define FLASH_ADDRESS_FACTORY_LOAD                 (0x08020000)  // TO BE UPDATE
#define FLASH_ADDRESS_APP_LOAD                     (0x08080000)  // TO BE UPDATE

extern strIhuL3iapTaskContext_t zIhuL3iapTaskContext;
extern SysEngParElementHwBurnPhyIdAddr_t zIhuSysEngElementHwBurnContext;


void ihu_iap_sw_jump_to_app_load(void);
void ihu_iap_sw_jump_to_factory_load(void);
void ihu_send_iap_sw_inventory_report(void);
void ihu_send_iap_sw_package_report(void);

void ihu_iap_sw_save_hw_sw_control_table(void);
UINT16 CalculateCheckSum(UINT8 *pdata, UINT32 length_in_bytes);

extern OPSTAT LoadIhuSysEngElementHwBurnContext(SysEngParElementHwBurnPhyIdAddr_t *psepehb, UINT32 SysEngElementHwBurnContextAddress);

#endif /* L3APPL_L3IAP_H_ */

