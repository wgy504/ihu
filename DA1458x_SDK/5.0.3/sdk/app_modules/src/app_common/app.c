/**
 ****************************************************************************************
 *
 * @file app.c
 *
 * @brief Application entry point
 *
 * Copyright (C) RivieraWaves 2009-2013
 *
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup APP
 * @{
 ****************************************************************************************
 */


/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"        // SW configuration

#if BLE_APP_PRESENT
    #include "app_task.h"           // Application task Definition
    #include "app.h"                // Application Definition
    #include "gapm_task.h"          // GAP Manager Task API
    #include "gapc_task.h"          // GAP Controller Task API
    #include "co_math.h"            // Common Maths Definition
    #include "app_api.h"            // Application task Definition
    #include "app_prf_types.h"
    #include "app_prf_perm_types.h"

    #include "app_security.h"       // Application security Definition
    #include "nvds.h"               // NVDS Definitions

    #include "user_callback_config.h"
    #include "app_default_handlers.h"

    #include "app_mid.h"
    #include "ke_mem.h"
    #include "app_adv_data.h"
    #if BLE_CUSTOM_SERVER
        #include "user_custs_config.h"
    #endif


/*
 * DEFINES
 ****************************************************************************************
 */

#define APP_EASY_GAP_MAX_CONNECTION     APP_EASY_MAX_ACTIVE_CONNECTION

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/// Application Environment Structure
struct app_env_tag app_env[APP_EASY_MAX_ACTIVE_CONNECTION] __attribute__((section("retention_mem_area0"),zero_init)); //@RETENTION MEMORY

// Array that holds the service access rights set by user for the included profiles. The default value is "ENABLE"
app_prf_srv_sec_t app_prf_srv_perm[PRFS_TASK_ID_MAX] __attribute__((section("retention_mem_area0"),zero_init)); //@RETENTION MEMORY

const struct prf_func_callbacks prf_funcs[] =
{
    #if BLE_PROX_REPORTER
    {TASK_PROXR,     app_proxr_create_db, app_proxr_enable},
    #endif
    
    #if BLE_BAS_SERVER
    {TASK_BASS,      app_bass_create_db, app_bass_enable},
    #endif
    
    #if BLE_FINDME_TARGET
    {TASK_FINDT,     NULL, app_findt_enable},
    #endif
    
    #if BLE_FINDME_LOCATOR
    {TASK_FINDL,     NULL, app_findl_enable},
    #endif
    
    #if BLE_DIS_SERVER
    {TASK_DISS,      app_diss_create_db, app_diss_enable},
    #endif
    
    #if BLE_SPOTA_RECEIVER
    {TASK_SPOTAR,    app_spotar_create_db, app_spotar_enable},
    #endif
    
    {TASK_NONE,    NULL, NULL},   // DO NOT MOVE. Mast always be last
};


/*
 * LOCAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/// Application Task Descriptor
static const struct ke_task_desc TASK_DESC_APP = {NULL,
                                                  &app_default_handler,
                                                  app_state,
                                                  APP_STATE_MAX,
                                                  APP_IDX_MAX};

static struct gapc_param_update_cmd *param_update_cmd [APP_EASY_GAP_MAX_CONNECTION] __attribute__((section("retention_mem_area0"),zero_init)); //@RETENTION MEMORY

static struct gapm_set_dev_config_cmd *set_dev_config_cmd __attribute__((section("retention_mem_area0"),zero_init));

static struct gapm_start_advertise_cmd *adv_cmd __attribute__((section("retention_mem_area0"),zero_init));

static struct gapm_start_connection_cmd *start_connection_cmd __attribute__((section("retention_mem_area0"),zero_init));

static timer_hnd adv_timer_id __attribute__((section("retention_mem_area0"),zero_init)); //@RETENTION MEMORY

static void (*adv_timeout_callback) (void) __attribute__((section("retention_mem_area0"),zero_init)); //@RETENTION MEMORY

//The following structures are only used as default options for the easy API 
//if the user does not define anything.                                                  
static const struct gapm_start_advertise_cmd default_undir_adv_cmd = {
    .op = {
        .code = GAPM_ADV_UNDIRECT,
        .addr_src = GAPM_PUBLIC_ADDR,
    },
    .intv_min    = MS_TO_DOUBLESLOTS(200),
    .intv_max    = MS_TO_DOUBLESLOTS(200),
    .channel_map = 0x7,
    .info={
        .host = {
            .mode           = GAP_GEN_DISCOVERABLE,
            .adv_data_len = 19,
            .adv_data = "\x09"\
                        ADV_TYPE_COMPLETE_LIST_16BIT_SERVICE_IDS\
                        ADV_UUID_LINK_LOSS_SERVICE\
                        ADV_UUID_TX_POWER_SERVICE\
                        ADV_UUID_SPOTAR_SERVICE\
                        "\0x08"\
                        ADV_TYPE_COMPLETE_LOCAL_NAME\
                        "DA1458x",
            .scan_rsp_data_len = 10,
            .scan_rsp_data = "\x0a"\
                               ADV_TYPE_MANUFACTURER_SPECIFIC_DATA\
                               ADV_DIALOG_MANUFACTURER_CODE\
                               "DLG-BLE",
        }
    }
};

static const struct gapm_start_advertise_cmd default_non_connectable_adv_cmd = {
    .op = {
        .code = ADV_NON_CONN,
        .addr_src = GAPM_PUBLIC_ADDR,
    },
    .intv_min    = MS_TO_DOUBLESLOTS(200),
    .intv_max    = MS_TO_DOUBLESLOTS(200),
    .channel_map = 0x7,
    .info = {
        .host= {
            .mode           = GAP_GEN_DISCOVERABLE,
            .adv_data_len = 19,
            .adv_data = "\x09"\
                        ADV_TYPE_COMPLETE_LIST_16BIT_SERVICE_IDS\
                        ADV_UUID_LINK_LOSS_SERVICE\
                        ADV_UUID_IMMEDIATE_ALERT_SERVICE\
                        ADV_UUID_SPOTAR_SERVICE\
                        "\0x08"\
                        ADV_TYPE_COMPLETE_LOCAL_NAME\
                        "DA1458x",
            .scan_rsp_data_len = 0,
            .scan_rsp_data = "",
        }
    }
};

static const struct gapm_start_advertise_cmd default_dir_adv_cmd = {
    .op = {
        .code=GAPM_ADV_DIRECT,
        .addr_src=GAPM_PUBLIC_ADDR,
    },
    .intv_min    = MS_TO_DOUBLESLOTS(200),
    .intv_max    = MS_TO_DOUBLESLOTS(200),
    .channel_map = 0x7,
    .info = {
        .direct= {
            .addr= {
                .addr = {0x0,0x1,0x2,0x3,0x4,0x5}
            },
            /// Address type of the device 0=public/1=private random
            .addr_type = 0,
          }
    }
};

/// Perform update of connection parameters command
static const struct gapc_param_update_cmd default_param_update_cmd = {
    .operation = GAPC_UPDATE_PARAMS,
    #ifndef __DA14581__
    .params = {
    #endif
        .intv_min = MS_TO_DOUBLESLOTS(10),            /// Connection interval minimum
        .intv_max = MS_TO_DOUBLESLOTS(20),           /// Connection interval maximum
        .latency = 0,             /// Latency
        .time_out = MS_TO_TIMERUNITS(1000),          /// Supervision timeout
    #ifndef __DA14581__
    },
    #else
    .ce_len_min = MS_TO_DOUBLESLOTS(0),          /// Minimum Connection Event Duration 
    .ce_len_max = MS_TO_DOUBLESLOTS(0)           /// Maximum Connection Event Duration 
#endif
};

static const struct gapm_start_connection_cmd default_start_connection_cmd =
{
    .op = {
        .code = GAPM_CONNECTION_DIRECT,
        .addr_src = GAPM_PUBLIC_ADDR,
        .state = 0,
        .renew_dur = 100,
        .addr = 0,
    },
    .scan_interval = 0,
    .scan_window = 0,
    .con_intv_min = 0,
    .con_intv_max = 0,
    .con_latency = 0,
    .superv_to = 0,
    .ce_len_min = 0,
    .ce_len_max = 0,
    .nb_peers = 0,
};

static const struct gapm_set_dev_config_cmd default_set_dev_config =
{
    .operation = GAPM_SET_DEV_CONFIG,
    .role = GAP_PERIPHERAL_SLV,
    #if BLE_APP_HT
    .appearance = 728,
    #else
    // Device Appearance
    .appearance = 0x0000,
    #endif
    .appearance_write_perm = GAPM_WRITE_DISABLE,
    .name_write_perm = GAPM_WRITE_DISABLE,
    .max_mtu = 23,
    .con_intv_min = MS_TO_DOUBLESLOTS(10),
    .con_intv_max = MS_TO_DOUBLESLOTS(20),
    .con_latency  = 0,
    // Slave preferred Link supervision timeout
    .superv_to    = 100,
    .irk = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    .flags = 0
};


/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Check if the task_id has an entry in the user_prf_func 
 * @param[in] task_id The task_id to check
 * @return true if the task_id has an entry in the user_prf_func.
 ****************************************************************************************
 */

static bool app_task_in_user_app(enum KE_TASK_TYPE task_id)
{
    uint8_t i=0;
    
    while( user_prf_funcs[i].task_id != TASK_NONE )
    {
        if (user_prf_funcs[i].task_id == task_id) 
            return(true);
        i++;
    }
     return(false);
}


/**
 ****************************************************************************************
 * @brief Calls all the enable function of the profile registered in prf_func, 
 *        custs_prf_func and user_prf_func.
 * @param[in] conhdl The connection handle
 * @return void
 ****************************************************************************************
 */

void app_prf_enable (uint16_t conhdl)
 {
     uint8_t i=0;

     while( user_prf_funcs[i].task_id != TASK_NONE )
     {
        if( user_prf_funcs[i].enable_func != NULL )
        {
            user_prf_funcs[i++].enable_func(conhdl);         
        }
            else i++;
     }
     
     i=0;
     
        /*--------------------------------------------------------------
        * ENABLE REQUIRED PROFILES
        *-------------------------------------------------------------*/
        while( prf_funcs[i].task_id != TASK_NONE )
        {
            if(( prf_funcs[i].enable_func != NULL ) && (!app_task_in_user_app(prf_funcs[i].task_id)))
            {
                prf_funcs[i++].enable_func(conhdl);         
            }
            else i++;
        }
        
        i=0;
#if BLE_CUSTOM_SERVER
        while( cust_prf_funcs[i].task_id != TASK_NONE )
        {
            if( cust_prf_funcs[i].enable_func != NULL )
            {
                cust_prf_funcs[i++].enable_func(conhdl);         
            }
            else i++; 
        }
#endif
}


/**
 ****************************************************************************************
 * @brief Initialize the database for all the included profiles.
 * @return true if succeeded, else false
 ****************************************************************************************
 */
static bool app_db_init_next(void)
{
    static uint8_t i __attribute__((section("retention_mem_area0"),zero_init)); //@RETENTION MEMORY;    
    static uint8_t k __attribute__((section("retention_mem_area0"),zero_init)); //@RETENTION MEMORY;    
    
    // initialise the databases for all the included profiles
    while( user_prf_funcs[k].task_id != TASK_NONE )
    {
        if ( user_prf_funcs[k].db_create_func != NULL )
        {
            user_prf_funcs[k++].db_create_func();
            return false;
        }
        else k++;
    }

    // initialise the databases for all the included profiles
    while( prf_funcs[i].task_id != TASK_NONE )
    {
        if (( prf_funcs[i].db_create_func != NULL ) \
            && (!app_task_in_user_app(prf_funcs[i].task_id)))    //case that the this task has an entry in the user_prf as well
        {
            prf_funcs[i++].db_create_func();
            return false;
        }
        else i++;
    }

    
    #if BLE_CUSTOM1_SERVER
    {
        static uint8_t j __attribute__((section("retention_mem_area0"),zero_init)); //@RETENTION MEMORY;    
        while( cust_prf_funcs[j].task_id != TASK_NONE )
        {
            if( cust_prf_funcs[j].db_create_func != NULL )
            {
                cust_prf_funcs[j++].db_create_func();
                return false;
            }
            else j++;
        }
        j = 0;
    }
    #endif
    
    k = 0;
    
    i = 0;
    return true;
}

/**
 ****************************************************************************************
 * @brief Read advertising data from NDVS memory.
 * @param[in] len              Expected length of the TAG
 * @param[out] data            A pointer to the buffer allocated by the caller to be filled with
 *                             the DATA part of the TAG
 * @param[in] permissible_size The permissible size of the TAG
 * @param[in] max_size         The max size of the buffer to be filled with TAG data
 * @param[in] tag              TAG to look for whose DATA is to be retrieved
 * @return void
 ****************************************************************************************
 */
static void app_easy_gap_adv_read_from_NVDS(uint8_t *len, uint8_t *data, const uint8_t permissible_size,\
                                                       const uint8_t max_size, const uint8_t tag)
{
    uint8_t temp_data[max_size];
    *len=max_size;
    if(nvds_get(tag, len, temp_data) != NVDS_OK)
    {
        ASSERT_WARNING(0);
        *len = 0;
    }
    else
    {
        ASSERT_WARNING(*len<permissible_size);
        *len = co_min(*len, permissible_size-1);
         memcpy(data, temp_data, *len);
    }
}

/**
 ****************************************************************************************
 * @brief Inject the device name in advertising data.
 * @param[out] len        The Advertising data length
 * @param[in] name_length The device name length
 * @param[out] data       Pointer to the buffer which will carry the device name
 * @param[in] name_data   The device name
 * @return void
 ****************************************************************************************
 */
static void app_easy_gap_place_name_ad_struct(uint8_t *len, uint8_t const name_length,\
                                                            uint8_t *data, uint8_t const *name_data)
{
    // Fill Length
    data[0] = name_length + 1;
    // Fill Device Name Flag
    data[1] = GAP_AD_TYPE_COMPLETE_NAME;
    // Copy device name
    memcpy(&data[2], name_data, name_length);
    // Update Advertising Data Length
    *len += name_length + 2;
}

/**
 ****************************************************************************************
 * @brief Create advertising message for non-connectable peripheral.
 * @return gapm_start_advertise_cmd Pointer to the advertising message
 ****************************************************************************************
 */
static struct gapm_start_advertise_cmd* app_easy_gap_non_connectable_advertise_start_create_msg(void)
{
    // Allocate a message for GAP
    if (adv_cmd==NULL)
    {
        struct gapm_start_advertise_cmd *cmd;
        cmd = app_advertise_start_msg_create();
        adv_cmd=cmd;
        if (USER_CONFIG)
        {
            cmd->op.code = user_non_connectable_advertise_conf.advertise_operation;
            cmd->op.addr_src = user_non_connectable_advertise_conf.address_src;
            cmd->intv_max = user_non_connectable_advertise_conf.intv;
            cmd->intv_min = user_non_connectable_advertise_conf.intv;
            cmd->channel_map = user_non_connectable_advertise_conf.channel_map;
            ASSERT_WARNING(user_non_connectable_advertise_conf.advertise_operation == GAPM_ADV_NON_CONN);
            {
                cmd->info.host.mode=user_non_connectable_advertise_mode;
                // Advertising Data
                // Carefull to get data from the NVDS you need to have an array
                // of at least 32 positions
                app_easy_gap_adv_read_from_NVDS (&cmd->info.host.adv_data_len,&cmd->info.host.adv_data[0],
                                                 APP_ADV_DATA_MAX_SIZE, ADV_DATA_LEN+1, NVDS_TAG_APP_BLE_ADV_DATA);

                {
                    // Scan Response Data
                    cmd->info.host.scan_rsp_data_len  = 0;
                    memset(&cmd->info.host.scan_rsp_data[0], 0, SCAN_RSP_DATA_LEN);
                }

                // Get remaining space in the Advertising Data - 2 bytes are used for name length/flag
                int16_t device_name_adv_avail_space = APP_ADV_DATA_MAX_SIZE - adv_cmd->info.host.adv_data_len - 2;
                uint8_t device_name_length = 0;
                uint8_t device_name_temp_buf[NVDS_LEN_DEVICE_NAME];
                // Check if data can be added to the Advertising data
                if (device_name_adv_avail_space > 0)
                {
                // Get the Device Name to add in the Advertising Data
                // Get default Device Name (No name if not enough space)
                   device_name_length = NVDS_LEN_DEVICE_NAME;
                   if (nvds_get(NVDS_TAG_DEVICE_NAME, &device_name_length, &device_name_temp_buf[0]) != NVDS_OK)
                    {
                        // Restore the default value
                        ASSERT_WARNING(0);
                        device_name_length = 0;
                    }
                }

                //place the NAME to advertise or in the scan response.
                if (device_name_adv_avail_space>=device_name_length)
                {
                    app_easy_gap_place_name_ad_struct(&cmd->info.host.adv_data_len, device_name_length, \
                    &cmd->info.host.adv_data[cmd->info.host.adv_data_len], device_name_temp_buf);
                }
            }
        }
        else
            memcpy((void*)cmd, (void*)&default_non_connectable_adv_cmd, sizeof(default_non_connectable_adv_cmd));
    }

    return(adv_cmd);
}

/**
 ****************************************************************************************
 * @brief Create advertising message for undirected connection.
 * @return Pointer to the advertising message
 ****************************************************************************************
 */
static struct gapm_start_advertise_cmd* app_easy_gap_undirected_advertise_start_create_msg(void)
{
    // Allocate a message for GAP
    if (adv_cmd == NULL)
    {
        struct gapm_start_advertise_cmd *cmd;
        cmd = app_advertise_start_msg_create();
        adv_cmd = cmd;
        if (USER_CONFIG)
        {
            cmd->op.code = user_undirected_advertise_conf.advertise_operation;
            cmd->op.addr_src = user_undirected_advertise_conf.address_src;
            cmd->intv_max = user_undirected_advertise_conf.intv;
            cmd->intv_min = user_undirected_advertise_conf.intv;
            cmd->channel_map = user_undirected_advertise_conf.channel_map;
            ASSERT_WARNING(user_undirected_advertise_conf.advertise_operation == GAPM_ADV_UNDIRECT);
            {
                cmd->info.host.mode = user_undirected_advertise_mode;
                // Advertising Data
                // Carefull to get data from the NVDS you need to have an array
                // of at least 32 positions
                app_easy_gap_adv_read_from_NVDS (&cmd->info.host.adv_data_len,&cmd->info.host.adv_data[0], \
                          APP_ADV_DATA_MAX_SIZE, ADV_DATA_LEN+1, NVDS_TAG_APP_BLE_ADV_DATA);

                // Scan Response Data
                // Carefull to get data from the NVDS you need to have an array
                // of at least 32 positions
                //Although Supported by NVDS you should not store in NVDS mode than  SCAN_RSP_DATA_LEN data
                app_easy_gap_adv_read_from_NVDS (&cmd->info.host.scan_rsp_data_len,&cmd->info.host.scan_rsp_data[0], \
                          SCAN_RSP_DATA_LEN, SCAN_RSP_DATA_LEN+1, NVDS_TAG_APP_BLE_SCAN_RESP_DATA);

                // Get remaining space in the Advertising Data - 2 bytes are used for name length/flag
                int16_t device_name_adv_avail_space = APP_ADV_DATA_MAX_SIZE - adv_cmd->info.host.adv_data_len - 2;
                int16_t device_name_scan_avail_space = SCAN_RSP_DATA_LEN - adv_cmd->info.host.scan_rsp_data_len - 2;
                uint8_t device_name_length = 0;
                uint8_t device_name_temp_buf[NVDS_LEN_DEVICE_NAME];
                // Check if data can be added to the Advertising data
                if ((device_name_adv_avail_space > 0) || (device_name_scan_avail_space > 0))
                {
                // Get the Device Name to add in the Advertising Data
                // Get default Device Name (No name if not enough space)
                   device_name_length = NVDS_LEN_DEVICE_NAME;
                   if (nvds_get(NVDS_TAG_DEVICE_NAME, &device_name_length, &device_name_temp_buf[0]) != NVDS_OK)
                    {
                        // Restore the default value
                        ASSERT_WARNING(0);
                        device_name_length = 0;
                    }
                }

                //place the NAME to advertise or in the scan response.

                if(device_name_length > 0)
                {
                    //select where to place the
                    //place the NAME to advertise or in the scan response.
                    if(device_name_adv_avail_space >= device_name_length)
                    {
                        app_easy_gap_place_name_ad_struct(&cmd->info.host.adv_data_len, device_name_length, \
                        &cmd->info.host.adv_data[cmd->info.host.adv_data_len], device_name_temp_buf);
                    }
                    else
                        if(device_name_scan_avail_space >= device_name_length)
                    {

                        app_easy_gap_place_name_ad_struct(&cmd->info.host.scan_rsp_data_len, device_name_length, \
                        &cmd->info.host.scan_rsp_data[cmd->info.host.scan_rsp_data_len], device_name_temp_buf);
                    }
                 }
            }
        }
        else
            memcpy((void*)cmd, (void*)&default_undir_adv_cmd, sizeof(default_undir_adv_cmd));
    }

    return(adv_cmd);
}

/**
 ****************************************************************************************
 * @brief Create advertising message for directed connection.
 * @return gapm_start_advertise_cmd Pointer to the advertising message
 ****************************************************************************************
 */
static struct gapm_start_advertise_cmd* app_easy_gap_directed_advertise_start_create_msg(void)
{
    // Allocate a message for GAP
    if (adv_cmd == NULL)
    {
        struct gapm_start_advertise_cmd *cmd;
        cmd = app_advertise_start_msg_create();
        adv_cmd = cmd;
        if (USER_CONFIG)
        {
            cmd->op.code = user_directed_advertise_conf.advertise_operation;
            cmd->op.addr_src = user_directed_advertise_conf.address_src;
            cmd->channel_map = user_directed_advertise_conf.channel_map;
            ASSERT_WARNING(user_directed_advertise_conf.advertise_operation == GAPM_ADV_DIRECT);
            cmd->info.direct.addr_type = user_directed_advertise_target_address_conf.addr_type;
            memcpy(cmd->info.direct.addr.addr, user_directed_advertise_target_address_conf.addr,BD_ADDR_LEN);
        }
        else
            memcpy((void*)cmd, (void*)&default_dir_adv_cmd, sizeof(default_dir_adv_cmd));
    }
    return(adv_cmd);
}

/**
 ****************************************************************************************
 * @brief Create parameter update request message.
 * @return gapc_param_update_cmd Pointer to the parameter update request message
 ****************************************************************************************
 */
static struct gapc_param_update_cmd* app_easy_gap_param_update_msg_create(uint8_t connection_idx)
{
    // Allocate a message for GAP
    if (param_update_cmd[connection_idx] == NULL)
    {
        struct gapc_param_update_cmd* cmd;
        cmd = app_param_update_msg_create(connection_idx);
        ASSERT_WARNING(connection_idx < APP_EASY_GAP_MAX_CONNECTION);
        param_update_cmd[connection_idx] = cmd;
        if (USER_CONFIG)
        {
    #ifndef __DA14581__
            cmd->params.intv_max = user_connection_param_conf.intv_max;
            cmd->params.intv_min = user_connection_param_conf.intv_min;
            cmd->params.latency = user_connection_param_conf.latency;
            cmd->params.time_out = user_connection_param_conf.time_out;
    #else
            cmd->intv_max = user_connection_param_conf.intv_max;
            cmd->intv_min = user_connection_param_conf.intv_min;
            cmd->latency = user_connection_param_conf.latency;
            cmd->time_out = user_connection_param_conf.time_out;
            cmd->ce_len_min = user_connection_param_conf.ce_len_min;
            cmd->ce_len_max = user_connection_param_conf.ce_len_max;
    #endif
        }
        else
            memcpy((void*)cmd, (void*)&default_param_update_cmd, sizeof(struct gapc_param_update_cmd));
    }

    return (param_update_cmd[connection_idx]);
}

/**
 ****************************************************************************************
 * @brief Create connection message.
 * @return gapm_start_connection_cmd Pointer to the connection message
 ****************************************************************************************
 */
static struct gapm_start_connection_cmd* app_easy_gap_start_connection_to_msg_create(void)
{
    // Allocate a message for GAP
    if (start_connection_cmd == NULL)
    {
        start_connection_cmd = app_connect_start_msg_create();
        memcpy((void*)start_connection_cmd, (void*)&default_start_connection_cmd, sizeof(struct gapm_start_connection_cmd));
    }
    return (start_connection_cmd);
}

/**
 ****************************************************************************************
 * @brief Create device configuration message.
 * @return gapm_set_dev_config_cmd Pointer to the device configuration message
 ****************************************************************************************
 */
static struct gapm_set_dev_config_cmd* app_easy_gap_dev_config_create_msg(void)
{
    // Allocate a message for GAP
    if (set_dev_config_cmd == NULL)
    {
        struct gapm_set_dev_config_cmd* cmd;
        cmd = app_gapm_configure_msg_create();
        set_dev_config_cmd = cmd;

        if(USER_CONFIG)
        {
            cmd->operation = GAPM_SET_DEV_CONFIG;
            cmd->role = user_gapm_conf.role;
            cmd->appearance = user_gapm_conf.appearance;
            cmd->appearance_write_perm = user_gapm_conf.appearance_write_perm;
            cmd->name_write_perm = user_gapm_conf.name_write_perm;
            cmd->max_mtu = user_gapm_conf.max_mtu;
            cmd->con_intv_min = user_gapm_conf.con_intv_min;
            cmd->con_intv_max = user_gapm_conf.con_intv_max;
            cmd->con_latency = user_gapm_conf.con_latency;
            cmd->superv_to = user_gapm_conf.superv_to;
            cmd->flags = user_gapm_conf.flags;
            memcpy(cmd->irk.key,user_gapm_conf.irk,KEY_LEN);
        }
        else
            memcpy((void*)cmd, (void*)&default_set_dev_config, sizeof(struct gapm_set_dev_config_cmd));
    }
    return (set_dev_config_cmd);
}

/**
 ****************************************************************************************
 * @brief Advertising stop handler.
 * @return void
 ****************************************************************************************
 */
static void app_easy_gap_advertise_stop_handler(void)
{
    void (*timeout_callback)(void);
    app_easy_gap_advertise_stop();
    adv_timer_id=EASY_TIMER_INVALID_TIMER;
    if(adv_timeout_callback!=NULL)
    {
        timeout_callback = adv_timeout_callback;
        adv_timeout_callback = NULL;
        timeout_callback();
    }
}

int16_t active_conidx_to_conhdl(uint8_t connection_idx)
{
    ASSERT_WARNING(connection_idx < APP_EASY_MAX_ACTIVE_CONNECTION);
    if(app_env[connection_idx].connection_active == true)
        return(app_env[connection_idx].conhdl);
    else
        return(-1);
}

int8_t active_conhdl_to_conidx(uint16_t conhdl)
{
    uint8_t i;
        for(i=0; i < APP_EASY_MAX_ACTIVE_CONNECTION; i++)
            if (app_env[i].conhdl == conhdl)
            {
                if (app_env[i].connection_active == true)
                    return(app_env[i].conidx);
                else
                    return(GAP_INVALID_CONIDX);
            }
         return (GAP_INVALID_CONIDX);      //returns -1 if the conidx is not found
}

void app_init(void)
{
    // Reset the environment
    memset(&app_env[0], 0, sizeof(app_env));

    bool security_default_val = true;

    uint8_t length = NVDS_LEN_SECURITY_ENABLE;
    // Get the security enable from the storage
    if (nvds_get(NVDS_TAG_SECURITY_ENABLE, &length, (uint8_t *)&security_default_val) != NVDS_OK)
    {
        // Set true by default (several profiles requires security)
        security_default_val = true;
    }

    for (uint8_t i = 0; i< APP_EASY_MAX_ACTIVE_CONNECTION; i++)
    {
        // Set true by default (several profiles requires security)
        app_env[i].sec_en = true;
    }

    // Create APP task
    ke_task_create(TASK_APP, &TASK_DESC_APP);

    // Initialize Task state
    ke_state_set(TASK_APP, APP_DISABLED);
}

void app_easy_gap_disconnect(uint8_t connection_idx)
{
    ASSERT_WARNING(connection_idx < APP_EASY_MAX_ACTIVE_CONNECTION);
    struct gapc_disconnect_cmd *cmd = app_disconnect_msg_create(connection_idx);

    cmd->reason = CO_ERROR_REMOTE_USER_TERM_CON;
    app_env[connection_idx].connection_active=false;
    
    // Send the message
    app_disconnect_msg_send(cmd);
}

void app_easy_gap_confirm(uint8_t connection_idx, enum gap_auth auth, enum gap_authz authorize)
{
    // confirm connection
    struct gapc_connection_cfm *cfm = app_connect_cfm_msg_create(connection_idx);

    cfm->auth = auth;

    cfm->authorize = authorize;

    // Send the message
    ke_msg_send(cfm);
}

struct gapm_start_advertise_cmd* app_easy_gap_undirected_advertise_get_active(void)
{
    return(app_easy_gap_undirected_advertise_start_create_msg());
}

void app_easy_gap_undirected_advertise_start(void)
{
    struct gapm_start_advertise_cmd* cmd;
    cmd = app_easy_gap_undirected_advertise_start_create_msg();

    // Send the message
    app_advertise_start_msg_send(cmd);
    adv_cmd=NULL ;

    // We are now connectable
    ke_state_set(TASK_APP, APP_CONNECTABLE);
}

struct gapm_start_advertise_cmd* app_easy_gap_directed_advertise_get_active(void)
{
    return(app_easy_gap_directed_advertise_start_create_msg());
}

void app_easy_gap_directed_advertise_start(void)
{
    struct gapm_start_advertise_cmd* cmd;
    cmd = app_easy_gap_undirected_advertise_start_create_msg();

    // Send the message
    app_advertise_start_msg_send(cmd);
    adv_cmd = NULL ;

    // We are now connectable
    ke_state_set(TASK_APP, APP_CONNECTABLE);
}

struct gapm_start_advertise_cmd* app_easy_gap_non_connectable_advertise_get_active(void)
{
    return(app_easy_gap_non_connectable_advertise_start_create_msg());
}

void app_easy_gap_non_connectable_advertise_start(void)
{
    struct gapm_start_advertise_cmd* cmd;
    cmd = app_easy_gap_non_connectable_advertise_start_create_msg();

    // Send the message
    app_advertise_start_msg_send(cmd);
    adv_cmd = NULL ;
    
    // We are now connectable
    ke_state_set(TASK_APP, APP_CONNECTABLE);
}

void app_easy_gap_advertise_stop(void)
{
    // Disable Advertising
    struct gapm_cancel_cmd *cmd = app_gapm_cancel_msg_create();
    // Send the message
    app_gapm_cancel_msg_send(cmd);
}

void app_easy_gap_undirected_advertise_with_timeout_start(uint16_t delay, void (*timeout_callback)(void))
{
    //stop the current running timer
    if(adv_timer_id != EASY_TIMER_INVALID_TIMER)
        app_easy_timer_cancel(adv_timer_id);
    if(timeout_callback != NULL)
        adv_timeout_callback = timeout_callback;
    adv_timer_id = app_easy_timer(delay, app_easy_gap_advertise_stop_handler);
    app_easy_gap_undirected_advertise_start();
}

void app_easy_gap_advertise_with_timeout_stop(void)
{
    //stop the current running timer
    if (adv_timer_id != EASY_TIMER_INVALID_TIMER)
        app_easy_timer_cancel(adv_timer_id);
    adv_timer_id = EASY_TIMER_INVALID_TIMER;
    adv_timeout_callback = NULL;
}

struct gapc_param_update_cmd* app_easy_gap_param_update_get_active(uint8_t connection_idx)
{
    return(app_easy_gap_param_update_msg_create(connection_idx));
}

void app_easy_gap_param_update_start(uint8_t connection_idx)
{
    struct gapc_param_update_cmd* cmd;
    cmd = app_easy_gap_param_update_msg_create(connection_idx);

    // Send the message
    app_param_update_msg_send(cmd);
    param_update_cmd[connection_idx] = NULL ;

    // We are now connectable
    ke_state_set(TASK_APP, APP_PARAM_UPD);   
}

void app_timer_set(ke_msg_id_t const timer_id, ke_task_id_t const task_id, uint16_t delay)
{
    // Delay shall not be more than maximum allowed
    if(delay > KE_TIMER_DELAY_MAX)
    {
        delay = KE_TIMER_DELAY_MAX;
    }
    // Delay should not be zero
    else if(delay == 0)
    {
        delay = 1;
    }

    ke_timer_set(timer_id, task_id, delay);
}

struct gapm_start_connection_cmd* app_easy_gap_start_connection_to_get_active(void)
{
    return(app_easy_gap_start_connection_to_msg_create());
}

void app_easy_gap_start_connection_to_set(uint8_t peer_addr_type, uint8_t *peer_addr, uint16_t intv)
{
    struct gapm_start_connection_cmd* msg;
    msg = app_easy_gap_start_connection_to_msg_create();
    msg->nb_peers = 1;
    memcpy((void *) &msg->peers[0].addr, (void *)peer_addr, BD_ADDR_LEN);
    msg->peers[0].addr_type = peer_addr_type;
    msg->con_intv_max = intv;
    msg->con_intv_min = intv;
    return;
}

void app_easy_gap_start_connection_to(void)
{
    volatile struct gapm_start_connection_cmd* msg;
    msg = app_easy_gap_start_connection_to_msg_create();
    app_connect_start_msg_send((void *) msg);
    msg = NULL;
}

bool app_db_init_start(void)
{
    // Indicate if more services need to be added in the database
    bool end_db_create = false;

    // We are now in Initialization State
    ke_state_set(TASK_APP, APP_DB_INIT);

    end_db_create = app_db_init_next();

    return end_db_create;
}

bool app_db_init(void)
{
    // Indicate if more services need to be added in the database
    bool end_db_create = false;

    end_db_create = app_db_init_next();

    return end_db_create;
}

struct gapm_set_dev_config_cmd* app_easy_gap_dev_config_get_active(void)
{
     return(app_easy_gap_dev_config_create_msg());
}

void app_easy_gap_dev_configure(void)
{
    struct gapm_set_dev_config_cmd* cmd = app_easy_gap_dev_config_create_msg();
    app_gapm_configure_msg_send(cmd);
    set_dev_config_cmd = NULL;
    return;
}

app_prf_srv_perm_t get_user_prf_srv_perm(enum KE_TASK_TYPE task_id)
{
    uint8_t i;

    for(i=0; i<PRFS_TASK_ID_MAX; i++)
    {
        if(app_prf_srv_perm[i].task_id == task_id)
            return app_prf_srv_perm[i].perm;
    }
    return SRV_PERM_ENABLE;
}

void app_set_prf_srv_perm(enum KE_TASK_TYPE task_id, app_prf_srv_perm_t srv_perm)
{
    uint8_t i;

    for(i=0; i<PRFS_TASK_ID_MAX; i++)
    {
        if((app_prf_srv_perm[i].task_id == task_id) || (app_prf_srv_perm[i].task_id == TASK_NONE))
        {
            app_prf_srv_perm[i].task_id = task_id;
            app_prf_srv_perm[i].perm = srv_perm;
        }
    }
}

void prf_init_srv_perm(void)
{
    uint8_t i;
    for(i = 0; i < PRFS_TASK_ID_MAX; i++)
    {
        app_prf_srv_perm[i].task_id = TASK_NONE;
        app_prf_srv_perm[i].perm = SRV_PERM_ENABLE;
    }
}

#endif //(BLE_APP_PRESENT)

/// @} APP
