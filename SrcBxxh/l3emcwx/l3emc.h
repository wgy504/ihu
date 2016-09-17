/**
 ****************************************************************************************
 *
 * @file l3emc.h
 *
 * @brief L3 EMC
 *
 * BXXH team
 * Created by ZJL, 20160127
 *
 ****************************************************************************************
 */

#ifndef L3APPL_L3EMC_H_
#define L3APPL_L3EMC_H_

#include "vmlayer.h"
#include "l1comdef.h"
#include "epb_MmBp.h"

//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
enum FSM_STATE_EMC
{
	FSM_STATE_EMC_INITED = 0x02,
	FSM_STATE_EMC_OFFLINE,
	FSM_STATE_EMC_ONLINE,
	FSM_STATE_EMC_ONLINE_BLOCK,	  //链路层不可重入	
	FSM_STATE_EMC_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Global variables
extern FsmStateItem_t FsmEmc[];
extern IhuDiscDataSampleStorage_t zIhuMemStorageBuf;

//Local variables
#define EMC_TIMER_DURATION_PERIOD_READ 60  //in second，这是采用了VM以上层的时钟形式


//API
extern OPSTAT fsm_emc_task_entry(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT8 param_len);
extern OPSTAT fsm_emc_init(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT8 param_len);
extern OPSTAT fsm_emc_restart(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT8 param_len);
extern OPSTAT fsm_emc_stop_rcv(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT8 param_len);
extern OPSTAT fsm_emc_disc_rcv(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT8 param_len);
extern OPSTAT fsm_emc_con_rcv(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT8 param_len);
extern OPSTAT fsm_emc_dlk_block_rcv(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT8 param_len);
extern OPSTAT fsm_emc_dlk_unblock_rcv(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT8 param_len);
extern OPSTAT fsm_emc_time_out(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT8 param_len);
extern OPSTAT fsm_emc_push_cmd_rcv(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT8 param_len);
extern OPSTAT ihu_save_to_storage_mem(IhuDiscDataSampleStorageArray_t *record);
extern OPSTAT ihu_read_from_storage_mem(IhuDiscDataSampleStorageArray_t *record);
extern OPSTAT ihu_save_to_storage_disc(void *dataBuffer, UINT32 dataLen);
extern OPSTAT ihu_read_from_storage_disc(void *dataBuffer, UINT32 dataLen);
extern OPSTAT ihu_unpack_l3_data_from_cloud(UINT8 *buf, UINT8 len, IhuL3DataFormatTag_t *output);
extern OPSTAT ihu_pack_l3_data_to_cloud(IhuL3DataFormatTag_t *input, UINT8 *buf, UINT8 len);


//Local API
OPSTAT func_emc_hw_init(void);
OPSTAT func_emc_send_out_data_online(void);
OPSTAT func_emc_store_data_offline(void);

#endif /* L3APPL_L3EMC_H_ */

