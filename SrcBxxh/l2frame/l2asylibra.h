/**
 ****************************************************************************************
 *
 * @file l2asylibra.h
 *
 * @brief L2 ASYLIBRA
 *
 * BXXH team
 * Created by ZJL, 20160127
 *
 ****************************************************************************************
 */

#ifndef L2FRAME_L2ASYLIBRA_H_
#define L2FRAME_L2ASYLIBRA_H_

#include "vmdalayer.h"
#include "l1comdef.h"
#include "i2c_led.h"
#include "epb_MmBp.h"

//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
enum FSM_STATE_ASYLIBRA
{
	FSM_STATE_ASYLIBRA_INITED = 0x02,
	FSM_STATE_ASYLIBRA_BLE_CONNECTED,
	FSM_STATE_ASYLIBRA_AUTH_OK,
	FSM_STATE_ASYLIBRA_DATA_READY,
	FSM_STATE_ASYLIBRA_DATA_WF_CONF,	
	FSM_STATE_ASYLIBRA_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//为了方便程序的可读性和维护性，对于链路变化的结果用灯表达出来，这里进行了必要的重定义
enum ASYLIBRA_LED_IND_BY_STATE
{
	ASYLIBRA_LED_STATE_ENTRY = IHU_LED_BLINK_MODE_ALWAYS_OFF,
	ASYLIBRA_LED_STATE_IDLE = IHU_LED_BLINK_MODE_ALWAYS_OFF,
	ASYLIBRA_LED_STATE_INITED = IHU_LED_BLINK_MODE_ALWAYS_OFF,
	ASYLIBRA_LED_STATE_BLE_CONNECTED = IHU_LED_BLINK_MODE_ALWAYS_ON,
	ASYLIBRA_LED_STATE_AUTH_OK = IHU_LED_BLINK_MODE_ONCE_OFF_ON,
	ASYLIBRA_LED_STATE_DATA_READY = IHU_LED_BLINK_MODE_HIGH_SPEED,
	ASYLIBRA_LED_STATE_DATA_WF_CONF = IHU_LED_BLINK_MODE_LOW_SPEED,	
};
#define ASYLIBRA_LED_IND_NO LED_ID_7

//Global variables
extern FsmStateItem_t FsmAsylibra[];

//Local variables
#define ASYLIBRA_TIMER_DURATION_AUTH_RESP_WAIT 3  //in second，这是采用了VM以上层的时钟形式
#define ASYLIBRA_TIMER_DURATION_INIT_RESP_WAIT 3  //in second，这是采用了VM以上层的时钟形式
#define ASYLIBRA_TIMER_DURATION_DATA_RESP_WAIT 10  //in second，这是采用了VM以上层的时钟形式

//用于存放IhuAsylibra中链路环境变量
typedef struct IhuAsylibraAuthReqTag
{
	uint8_t md5_type_id[16];
	uint8_t device_key[16];
	char device_name[15];
	char device_id[12];
	char timezone[12];
	char language[12];	
}IhuAsylibraAuthReqTag_t;
typedef struct IhuAsylibraInitRespFbTag
{
	bool flag;
	uint32_t user_id_high;              //2=微信用户高32位
	uint32_t user_id_low;               //3=微信用户低32位
	uint32_t challeange_answer;         //4=手机回复的挑战值，为设备生成字节的CRC32
	uint8_t init_scence;                //5=微信连上设备时，处于什么情景
	uint32_t auto_sync_max_duration_second;  //6=自动同步持续多长时间，微信就会关闭。 0xffffffff表示无限长时间
	char user_nick_name[15];            //11=微信用户昵称
	uint16_t platform_type;             //12=手机平台
	char model[15];                     //13=手机硬件型号 
	char os[15];                        //14=手机OS版本
	int32_t time;                       //15=手机当前时间
	int32_t time_zone;                  //16=手机当前时区
	char time_string[15];               //17=手机当前时间，格式为201402281005285，2014(Y)02(M)28(D)10(H)05(M)28(S)5(W)
}IhuAsylibraInitRespFbTag_t;

typedef struct IhuAsylibraLinkCtrlTable
{	
	UINT16 seqId;
	IhuAsylibraAuthReqTag_t	authReq;
	UINT8 initReqChallenge[4];
	UINT8 resp_field_filter[1];
	IhuAsylibraInitRespFbTag_t initRespFb;
}IhuAsylibraLinkCtrlTable_t;

typedef struct auth_buf
{
	uint32_t random;
	uint32_t seq;
	uint32_t crc;
}auth_buf_t;


//API
extern OPSTAT fsm_asylibra_task_entry(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT8 param_len);
extern OPSTAT fsm_asylibra_init(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT8 param_len);
extern OPSTAT fsm_asylibra_restart(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT8 param_len);
extern OPSTAT fsm_asylibra_stop_rcv(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT8 param_len);
extern OPSTAT fsm_asylibra_ble_con_rcv(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT8 param_len);
extern OPSTAT fsm_asylibra_ble_disc_rcv(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT8 param_len);
extern OPSTAT fsm_asylibra_ble_data_dl_rcv(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT8 param_len);
extern OPSTAT fsm_asylibra_time_out(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT8 param_len);
extern OPSTAT fsm_asylibra_emc_data_req(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT8 param_len);


//Local API
OPSTAT func_asylibra_hw_init(void);
OPSTAT func_asylibra_pack_auth_req(INT8 *buf, UINT8 len);
OPSTAT func_asylibra_pack_init_req(INT8 *buf, UINT8 len);

OPSTAT func_asylibra_unpack_ble_data_dl_head(INT8 *buf, UINT8 len, UINT16 nCmd);
OPSTAT func_asylibra_rcv_auth_resp_processing(INT8 *buf, UINT8 len);
OPSTAT func_asylibra_rcv_init_resp_processing(INT8 *buf, UINT8 len);
OPSTAT func_asylibra_rcv_data_resp_processing(INT8 *buf, UINT8 len);
OPSTAT func_asylibra_rcv_data_push_processing(INT8 *buf, UINT8 len);
OPSTAT func_asylibra_rcv_push_switch_view_processing(INT8 *buf, UINT8 len);
OPSTAT func_asylibra_rcv_push_switch_background_processing(INT8 *buf, UINT8 len);
OPSTAT func_asylibra_rcv_err_decode_processing(INT8 *buf, UINT8 len);
OPSTAT func_asylibra_time_out_auth_resp_wait(void);
OPSTAT func_asylibra_time_out_init_resp_wait(void);
OPSTAT func_asylibra_time_out_data_resp_wait(void);


//备份API过程，未来很可能不需要
//extern OPSTAT fsm_asylibra_emc_data_report(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT8 param_len);
//extern OPSTAT fsm_asylibra_emc_time_sync_req(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT8 param_len);
//extern OPSTAT fsm_asylibra_emc_equ_info_sync(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT8 param_len);
//OPSTAT func_asylibra_pack_data_req(INT8 *buf, UINT8 len, INT8 *input, UINT8 inputLen);
//OPSTAT func_asylibra_pack_time_sync_req(INT8 *buf, UINT8 len, INT8 *input, UINT8 inputLen);
//OPSTAT func_asylibra_pack_equ_info(INT8 *buf, UINT8 len, INT8 *input, UINT8 inputLen);


#endif /* L2FRAME_L2ASYLIBRA_H_ */
