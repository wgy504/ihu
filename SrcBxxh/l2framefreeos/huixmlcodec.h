/**
 ****************************************************************************************
 *
 * @file huixmlcodec.h
 *
 * @brief HUIXMLCODEC
 *
 * BXXH team
 * Created by ZJL, 20160918
 *
 ****************************************************************************************
 */

#ifndef L2FRAME_HUIXMLCODEC_H_
#define L2FRAME_HUIXMLCODEC_H_

#include "sysdim.h"
#if ((IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_DA_EMC68X_ID) ||\
	(IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID) ||\
	(IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID))
	#include "vmfreeoslayer.h"
	#include "l1comdef_freeos.h"
	#include "l1timer_freeos.h"	
	#include "huitp.h"	
	
#elif ((IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_SCYCB_ID) ||\
	(IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_PLCCB_ID) ||\
	(IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_PLCSB_ID))
	#include "vmucoslayer.h"
	#include "l1comdef_ucos.h"
	#include "l1timer_ucos.h"
#else
#endif

//XML message pack/unpack
extern OPSTAT func_cloud_standard_xml_pack(StrCloudBhItfStdHuixml_t *xmlFormat, CloudDataSendBuf_t *buf);
extern OPSTAT func_cloud_standard_xml_unpack(msg_struct_ccl_com_cloud_data_rx_t *rcv);
extern OPSTAT func_cloud_standard_xml_heart_beat_msg_unpack(msg_struct_ccl_com_cloud_data_rx_t *rcv);
extern OPSTAT func_cloud_standard_xml_emc_msg_unpack(msg_struct_ccl_com_cloud_data_rx_t *rcv);


//Extern API
extern OPSTAT func_cloud_heart_beat_received_handle(void);
extern OPSTAT func_cloud_sw_download(char *filename);

#endif /* L2FRAME_HUIXMLCODEC_H_ */
