/**
 ****************************************************************************************
 *
 * @file l2spsvirgo.h
 *
 * @brief L2 SPSVIRGO
 *
 * BXXH team
 * Created by ZJL, 20160918
 *
 ****************************************************************************************
 */

#ifndef L2FRAME_L2SPSVIRGO_H_
#define L2FRAME_L2SPSVIRGO_H_

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
	#include "l1hd_usart.h"
#else
	#error Un-correct constant definition
#endif

#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)
	#include "huitp.h"	
	#include "huixmlcodec.h"
#endif

//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
enum FSM_STATE_SPSVIRGO
{
	FSM_STATE_SPSVIRGO_INITED = 0x02,
	FSM_STATE_SPSVIRGO_ACTIVED,
	FSM_STATE_SPSVIRGO_COMMU,   //与后台的通信状态
	FSM_STATE_SPSVIRGO_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Global variables
extern FsmStateItem_t FsmSpsvirgo[];

#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)
	//本地需要用到的核心参数
	typedef struct strIhuCclSpsPar
	{	
		UINT8 cclSpsWorkingMode;
		com_sensor_status_t sensor;
	}strIhuCclSpsPar_t;
#endif

//SPS工作状态
#define IHU_CCL_SPS_WORKING_MODE_NONE 0
#define IHU_CCL_SPS_WORKING_MODE_SLEEP 1
#define IHU_CCL_SPS_WORKING_MODE_ACTIVE 2
#define IHU_CCL_SPS_WORKING_MODE_FAULT 3
#define IHU_CCL_SPS_WORKING_MODE_INVALID 0xFF

//控制定时器定时扫描是否启动
#define IHU_SPSVIRGO_PERIOD_TIMER_ACTIVE 1
#define IHU_SPSVIRGO_PERIOD_TIMER_DEACTIVE 2
#define IHU_SPSVIRGO_PERIOD_TIMER_SET IHU_SPSVIRGO_PERIOD_TIMER_DEACTIVE

//API
extern OPSTAT fsm_spsvirgo_task_entry(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_spsvirgo_init(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_spsvirgo_restart(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_spsvirgo_stop_rcv(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_spsvirgo_time_out(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_spsvirgo_l2frame_rcv(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_spsvirgo_ccl_open_auth_inq(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_spsvirgo_ccl_sensor_status_req(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_spsvirgo_ccl_ctrl_cmd(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_spsvirgo_ccl_event_report_send(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_spsvirgo_ccl_fault_report_send(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);
extern OPSTAT fsm_spsvirgo_ccl_close_door_report_send(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len);

//接收到的消息处理函数
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)
	extern OPSTAT func_cloud_spsvirgo_ccl_msg_heart_beat_req_received_handle(StrMsg_HUITP_MSGID_uni_heart_beat_req_t *rcv);
	extern OPSTAT func_cloud_spsvirgo_ccl_msg_heart_beat_confirm_received_handle(StrMsg_HUITP_MSGID_uni_heart_beat_confirm_t *rcv);
	extern OPSTAT func_cloud_spsvirgo_ccl_msg_ccl_lock_req_received_handle(StrMsg_HUITP_MSGID_uni_ccl_lock_req_t *rcv);
	extern OPSTAT func_cloud_spsvirgo_ccl_msg_ccl_lock_confirm_received_handle(StrMsg_HUITP_MSGID_uni_ccl_lock_confirm_t *rcv);
	extern OPSTAT func_cloud_spsvirgo_ccl_msg_ccl_lock_auth_resp_received_handle(StrMsg_HUITP_MSGID_uni_ccl_lock_auth_resp_t *rcv);
	extern OPSTAT func_cloud_spsvirgo_ccl_msg_ccl_door_req_received_handle(StrMsg_HUITP_MSGID_uni_ccl_door_req_t *rcv);
	extern OPSTAT func_cloud_spsvirgo_ccl_msg_ccl_door_confirm_received_handle(StrMsg_HUITP_MSGID_uni_ccl_door_confirm_t *rcv);
	extern OPSTAT func_cloud_spsvirgo_ccl_msg_ccl_rfid_req_received_handle(StrMsg_HUITP_MSGID_uni_ccl_rfid_req_t *rcv);
	extern OPSTAT func_cloud_spsvirgo_ccl_msg_ccl_rfid_confirm_received_handle(StrMsg_HUITP_MSGID_uni_ccl_rfid_confirm_t *rcv);
	extern OPSTAT func_cloud_spsvirgo_ccl_msg_ccl_ble_req_received_handle(StrMsg_HUITP_MSGID_uni_ccl_ble_req_t *rcv);
	extern OPSTAT func_cloud_spsvirgo_ccl_msg_ccl_ble_confirm_received_handle(StrMsg_HUITP_MSGID_uni_ccl_ble_confirm_t *rcv);
	extern OPSTAT func_cloud_spsvirgo_ccl_msg_ccl_gprs_req_received_handle(StrMsg_HUITP_MSGID_uni_ccl_gprs_req_t *rcv);
	extern OPSTAT func_cloud_spsvirgo_ccl_msg_ccl_gprs_confirm_received_handle(StrMsg_HUITP_MSGID_uni_ccl_gprs_confirm_t *rcv);
	extern OPSTAT func_cloud_spsvirgo_ccl_msg_ccl_battery_req_received_handle(StrMsg_HUITP_MSGID_uni_ccl_battery_req_t *rcv);
	extern OPSTAT func_cloud_spsvirgo_ccl_msg_ccl_battery_confirm_received_handle(StrMsg_HUITP_MSGID_uni_ccl_battery_confirm_t *rcv);
	extern OPSTAT func_cloud_spsvirgo_ccl_msg_ccl_shake_req_received_handle(StrMsg_HUITP_MSGID_uni_ccl_shake_req_t *rcv);
	extern OPSTAT func_cloud_spsvirgo_ccl_msg_ccl_shake_confirm_received_handle(StrMsg_HUITP_MSGID_uni_ccl_shake_confirm_t *rcv);
	extern OPSTAT func_cloud_spsvirgo_ccl_msg_ccl_smoke_req_received_handle(StrMsg_HUITP_MSGID_uni_ccl_smoke_req_t *rcv);
	extern OPSTAT func_cloud_spsvirgo_ccl_msg_ccl_smoke_confirm_received_handle(StrMsg_HUITP_MSGID_uni_ccl_smoke_confirm_t *rcv);
	extern OPSTAT func_cloud_spsvirgo_ccl_msg_ccl_water_req_received_handle(StrMsg_HUITP_MSGID_uni_ccl_water_req_t *rcv);
	extern OPSTAT func_cloud_spsvirgo_ccl_msg_ccl_water_confirm_received_handle(StrMsg_HUITP_MSGID_uni_ccl_water_confirm_t *rcv);
	extern OPSTAT func_cloud_spsvirgo_ccl_msg_ccl_temp_req_received_handle(StrMsg_HUITP_MSGID_uni_ccl_temp_req_t *rcv);
	extern OPSTAT func_cloud_spsvirgo_ccl_msg_ccl_temp_confirm_received_handle(StrMsg_HUITP_MSGID_uni_ccl_temp_confirm_t *rcv);
	extern OPSTAT func_cloud_spsvirgo_ccl_msg_ccl_humid_req_received_handle(StrMsg_HUITP_MSGID_uni_ccl_humid_req_t *rcv);
	extern OPSTAT func_cloud_spsvirgo_ccl_msg_ccl_humid_confirm_received_handle(StrMsg_HUITP_MSGID_uni_ccl_humid_confirm_t *rcv);
	extern OPSTAT func_cloud_spsvirgo_ccl_msg_ccl_fall_req_received_handle(StrMsg_HUITP_MSGID_uni_ccl_fall_req_t *rcv);
	extern OPSTAT func_cloud_spsvirgo_ccl_msg_ccl_fall_confirm_received_handle(StrMsg_HUITP_MSGID_uni_ccl_fall_confirm_t *rcv);
	extern OPSTAT func_cloud_spsvirgo_ccl_msg_ccl_state_req_received_handle(StrMsg_HUITP_MSGID_uni_ccl_state_req_t *rcv);
	extern OPSTAT func_cloud_spsvirgo_ccl_msg_ccl_state_confirm_received_handle(StrMsg_HUITP_MSGID_uni_ccl_state_confirm_t *rcv);
	extern OPSTAT func_cloud_spsvirgo_ccl_msg_inventory_req_received_handle(StrMsg_HUITP_MSGID_uni_inventory_req_t *rcv);
	extern OPSTAT func_cloud_spsvirgo_ccl_msg_inventory_confirm_received_handle(StrMsg_HUITP_MSGID_uni_inventory_confirm_t *rcv);
	extern OPSTAT func_cloud_spsvirgo_ccl_msg_sw_package_req_received_handle(StrMsg_HUITP_MSGID_uni_sw_package_req_t *rcv);
	extern OPSTAT func_cloud_spsvirgo_ccl_msg_sw_package_confirm_received_handle(StrMsg_HUITP_MSGID_uni_sw_package_confirm_t *rcv);

	extern OPSTAT func_cloud_standard_xml_pack(UINT8 msgType, char *funcFlag, UINT16 msgId, StrMsg_HUITP_MSGID_uni_general_message_t *inputPar, UINT16 inputLen, CloudDataSendBuf_t *output);
	extern OPSTAT func_cloud_standard_xml_unpack(msg_struct_ccl_com_cloud_data_rx_t *rcv, int expectMsgId);
#endif

extern INT16 ihu_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_rssi_value(void);

//Local API
OPSTAT func_spsvirgo_hw_init(void);
void func_spsvirgo_time_out_period_scan(void);	


//External API
extern bool ihu_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_door_open_state(UINT8 doorid);
extern bool ihu_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_lock_open_state(UINT8 lockid);
extern bool ihu_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_water_state(void);
extern bool ihu_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_fall_state(void);
extern bool ihu_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_smoke_state(void);
extern bool ihu_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_shake_state(void);
extern bool ihu_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_bat_state(void);
extern INT16 ihu_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_temp_value(void);
extern INT16 ihu_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_humid_value(void);
extern INT16 ihu_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_bat_value(void);
extern INT16 ihu_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_rsv1_value(void);
extern INT16 ihu_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_rsv2_value(void);
extern INT16 ihu_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_dcmi_value(void);

//GPRSMOD的模组操作
extern OPSTAT ihu_vmmw_gprsmod_http_data_transmit_with_receive(char *input, int16_t inlen, char *output, uint16_t *outlen);
extern OPSTAT ihu_vmmw_gprsmod_tcp_text_data_transmit_with_receive(char *input, int16_t inlen, char *output, uint16_t *outlen);
extern OPSTAT ihu_vmmw_gprsmod_tcp_u8_data_transmit_with_receive(int8_t *input, int16_t inlen, int8_t *output, uint16_t *outlen);
extern int16_t ihu_vmmw_gprsmod_get_rssi_value(void);
extern OPSTAT ihu_vmmw_blemod_hc05_uart_fetch_mac_addr_official(uint8_t *macAddr, uint8_t len);
extern OPSTAT ihu_vmmw_blemod_hc05_uart_fetch_mac_addr_test_mode(uint8_t *macAddr, uint8_t len);


#endif /* L2FRAME_L2SPSVIRGO_H_ */

