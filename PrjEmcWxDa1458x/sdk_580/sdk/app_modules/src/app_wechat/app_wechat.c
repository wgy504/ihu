/**
 ****************************************************************************************
 *
 * @file app_wechat.c
 *
 * @brief Wechat Service Application entry point
 *
* Copyright (C) 2012. Dialog Semiconductor Ltd, unpublished work. This computer 
* program includes Confidential, Proprietary Information and is a Trade Secret of 
* Dialog Semiconductor Ltd.  All use, disclosure, and/or reproduction is prohibited 
* unless authorized in writing. All Rights Reserved.
*
* <bluetooth.support@diasemi.com> and contributors.
*
****************************************************************************************
*/

/**
 ****************************************************************************************
 * @addtogroup APP
 * @{
 ****************************************************************************************
 */
#include "ke_mem.h"
#include "rwip_config.h"     // SW configuration
#include "arch_console.h"

#if (BLE_WECHAT)

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "app_wechat.h"                 // Wechat Service Application Definitions
#include "app_wechat_task.h"            // Wechat Service Application Task API
#include "app.h"                        // Application Definitions
#include "app_task.h"                   // Application Task Definitions
#include "wechat_task.h"                // Wechat Functions
#include "app_prf_perm_types.h"
#include "mpbledemo2.h"
/*
 * LOCAL VARIABLES DECLARATIONS
 ****************************************************************************************
 */
/*****************************************************************************
* data handle
*****************************************************************************/
data_info g_send_data  ;
static data_info g_rcv_data   ;

data_handler* pDataHandler ;

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */
//function for register all products
static void register_all_products(void) 
{
	REGISTER(mpbledemo2);
	// could register more products if defined
}        

void app_wechat_init(void)
{
    register_all_products();
    
    data_handler_init(&m_mpbledemo2_handler, PRODUCT_TYPE_MPBLEDEMO2);
	m_mpbledemo2_handler->m_data_init_func();
    m_mpbledemo2_handler->m_data_init_peripheral_func();
    
    app_wechat_SetDatahandler(m_mpbledemo2_handler);  
    
    // Initialise service access write permitions for all the included profiles
    prf_init_srv_perm();
    
    arch_set_sleep_mode(app_default_sleep_mode);
}

/**
 ****************************************************************************************
 * @brief enable Wechat profile.
 *
 * @return void.
 ****************************************************************************************
 */
static uint16_t active_conhdl __attribute__((section("retention_mem_area0"),zero_init));
void app_wechat_enable(uint16_t conhdl)
{
    // Allocate the message
    struct wechat_enable_req* req = KE_MSG_ALLOC(WECHAT_ENABLE_REQ, TASK_WECHAT, TASK_APP,
                                                 wechat_enable_req);

    req->conhdl = conhdl;
    req->sec_lvl = PERM(SVC, ENABLE);
    req->feature = 0x00; // Client Configuration, notifications and indications are both disabled
    
    active_conhdl = conhdl;
    
    // Send the message
    ke_msg_send(req);
}

/**
 ****************************************************************************************
 * @brief Create Wechat profile database.
 *
 * @return void.
 ****************************************************************************************
 */
void app_wechat_create_db(void)
{
    struct wechat_create_db_req *req = KE_MSG_ALLOC(WECHAT_CREATE_DB_REQ,
                                                  TASK_WECHAT, TASK_APP,
                                                  wechat_create_db_req);

    req->features = 0; // DUMMY NOT USED

    ke_msg_send(req);
}

int ble_wechat_indicate_data_chunk(void)
{
	uint16_t chunk_len = 0;
	
	chunk_len = g_send_data.len - g_send_data.offset;
	chunk_len = chunk_len > BLE_WECHAT_MAX_DATA_LEN?BLE_WECHAT_MAX_DATA_LEN:chunk_len;

	if (chunk_len == 0) 
    {
        app_wechat_datahandler()->m_data_free_func(g_send_data.data, g_send_data.len);
		g_send_data.data = NULL;
		g_send_data.len = 0;
		g_send_data.offset = 0;
		return 0;
	}

    struct wechat_indication_req *req = KE_MSG_ALLOC(WECHAT_SEND_INDICATION_REQ,
                                                  TASK_WECHAT, TASK_APP,
                                                  wechat_indication_req);
    req->conhdl = active_conhdl;
    memcpy(req->value, g_send_data.data + g_send_data.offset, chunk_len);
    req->length = chunk_len;

    ke_msg_send(req);

	g_send_data.offset += chunk_len;
	return 1;
}

bool ble_wechat_is_last_data_sent_out(void)
{
    return (g_send_data.len == 0 && g_send_data.offset == 0) ||  \
        (g_send_data.len != 0 && g_send_data.len == g_send_data.offset);
}

//device sent data on the indicate characteristic
int ble_wechat_indicate_data(uint8_t *data, int len)
{
	if (data == NULL || len == 0) 
    {
		return 0;	
	}
    
	if (!ble_wechat_is_last_data_sent_out()) 
    {
    #ifdef CATCH_LOG
		arch_printf("\r\n offset:%d %d",g_send_data.offset,g_send_data.len);
    #endif
		return 0;
	}
    
	g_send_data.data = data;
	g_send_data.len = len;
	g_send_data.offset = 0;
	return (ble_wechat_indicate_data_chunk());
}

void app_wechat_SetDatahandler(data_handler* pHandler)
{
    pDataHandler = pHandler;
}

data_handler* app_wechat_datahandler(void)
{
    return pDataHandler; 
}

void ble_wechat_process_received_data(uint8_t* pData, uint32_t length)
{
	int error_code;
	int chunk_size = 0;
	if (length <= BLE_WECHAT_MAX_DATA_LEN) 
	{
		if (g_rcv_data.len == 0) 
		{
            BpFixHead *fix_head = (BpFixHead *)pData;
			g_rcv_data.len = ntohs(fix_head->nLength);
			g_rcv_data.offset = 0;
			g_rcv_data.data = (uint8_t *)ke_malloc(g_rcv_data.len, KE_MEM_NON_RETENTION);
		}
        
		chunk_size = g_rcv_data.len - g_rcv_data.offset;
		chunk_size = chunk_size < length ? chunk_size : length;
		memcpy(g_rcv_data.data + g_rcv_data.offset, pData, chunk_size);
		g_rcv_data.offset += chunk_size;
        
		if (g_rcv_data.len <= g_rcv_data.offset) 
		{
			error_code = app_wechat_datahandler()->m_data_consume_func(g_rcv_data.data, g_rcv_data.len);
			app_wechat_datahandler()->m_data_free_func(g_rcv_data.data,g_rcv_data.len);
			wechat_error_chack(app_wechat_datahandler(), error_code);
			g_rcv_data.len = 0;
			g_rcv_data.offset = 0;	

            app_wechat_datahandler()->m_data_main_process_func();
		}	
	}        
}

#endif //BLE_WECHAT

/// @} APP
