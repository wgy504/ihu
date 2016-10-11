/**
 ****************************************************************************************
 *
 * @file rwble.c
 *
 * @brief Entry points the BLE software
 *
 * Copyright (C) RivieraWaves 2009-2014
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup ROOT
 * @{
 ****************************************************************************************
 */


/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include <string.h>
#include "co_version.h"
#include "rwble.h"
#if HCI_PRESENT
#include "hci.h"
#endif //HCI_PRESENT
#include "ke_event.h"
#include "ke_timer.h"
#include "co_buf.h"
#include "ea.h"
#include "lld.h"
#include "llc.h"
#include "llm.h"
#include "dbg.h"
#include "ea.h"
#include "reg_blecore.h"
#include "osal.h"
#if (dg_configBLE_ADV_STOP_DELAY_ENABLE == 1)
#include "ad_ble.h"
#endif

#if (dg_configBLE_SKIP_LATENCY_API == 1) && (RWBLE_SW_VERSION_MAJOR >= 8)
        #include "ble_mgr.h"
        #include "reg_ble_em_cs.h"
        #include "reg_ble_em_rx_desc.h"

        bool lld_evt_continue(struct ea_elt_tag *elt);
        void lld_evt_priority_reset(struct ea_elt_tag *elt);
        void lld_evt_deferred_elt_push(struct ea_elt_tag *elt, uint8_t type, uint8_t rx_desc_cnt);
        void lld_evt_restart(struct ea_elt_tag *elt, bool rejected);
        uint8_t gapc_get_conidx(uint16_t conhdl);

        static void lld_evt_end_skip_latency(struct ea_elt_tag *elt);
#endif /* (dg_configBLE_SKIP_LATENCY_API == 1) && (RWBLE_SW_VERSION_MAJOR >= 8) */

#include "window_stats.h"

#if (NVDS_SUPPORT)
#include "nvds.h"         // NVDS definitions
#endif // NVDS_SUPPORT

#define RF_DCOFFSET_LOW 5
#define RF_DCOFFSET_HIGH 250

/* Forward declarations for Notif Hooks */
#ifdef dg_configBLE_EVENT_NOTIF_HOOK_END_EVENT
        void dg_configBLE_EVENT_NOTIF_HOOK_END_EVENT(void);
#endif
#ifdef dg_configBLE_EVENT_NOTIF_HOOK_CSCNT_EVENT
        void dg_configBLE_EVENT_NOTIF_HOOK_CSCNT_EVENT(void);
#endif
#ifdef dg_configBLE_EVENT_NOTIF_HOOK_FINE_EVENT
        void dg_configBLE_EVENT_NOTIF_HOOK_FINE_EVENT(void);
#endif

/*
 * EXTERNAL FUNCTIONS DEFINITIONS
 ****************************************************************************************
 */

void rf_reinit_func(void);

extern bool rf_dcoffset_failure;

#define RF_CHECK_DCOFFSET() \
        do { \
                uint8_t q = REG_GETF(DEM, RF_DC_OFFSET_RESULT_REG, DCOFFSET_Q_RD); \
                uint8_t i = REG_GETF(DEM, RF_DC_OFFSET_RESULT_REG, DCOFFSET_I_RD); \
                if (q < RF_DCOFFSET_LOW || q > RF_DCOFFSET_HIGH || \
                        i < RF_DCOFFSET_LOW || i > RF_DCOFFSET_HIGH) \
                        rf_dcoffset_failure = true; \
        } while(0)

/*
 * LOCAL FUNCTIONS DEFINITIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initializes the diagnostic port.
 ****************************************************************************************
 */
static void rwble_diagport_init(void)
{
    #if (NVDS_SUPPORT)
    uint8_t diag_cfg[NVDS_LEN_DIAG_BLE_HW];
    uint8_t length = NVDS_LEN_DIAG_BLE_HW;

    // Read diagport configuration from NVDS
    if(nvds_get(NVDS_TAG_DIAG_BLE_HW, &length, diag_cfg) == NVDS_OK)
    {
        ble_diagcntl_pack(1, diag_cfg[3], 1, diag_cfg[2], 1, diag_cfg[1], 1, diag_cfg[0]);
    }
    else
    #endif // NVDS_SUPPORT
    {
        ble_diagcntl_set(0);
    }
}


/*
 * EXPORTED FUNCTIONS DEFINITIONS
 ****************************************************************************************
 */

void rwble_init(void)
{
    #if RW_BLE_SUPPORT
    // Initialize buffers
    co_buf_init();
    #endif // #if RW_BLE_SUPPORT

    // Initialize the Link Layer Driver
    lld_init(false);

    #if (BLE_CENTRAL || BLE_PERIPHERAL)
    // Initialize the Link Layer Controller
    llc_init();
    #endif /* #if (BLE_CENTRAL || BLE_PERIPHERAL) */
    // Initialize the Link Layer Manager
    llm_init(false);
    #if (RW_DEBUG)
    #if RW_BLE_SUPPORT
    // Initialize the debug process
    dbg_init();
    #endif //RW_BLE_SUPPORT
    #endif //RW_DEBUG
    // Initialize diagport - for test and debug only
    rwble_diagport_init();

    #if BLE_HOST_PRESENT
    // Signal the completion of the boot process to the host layers
    llm_ble_ready();
    #endif //BLE_HOST_PRESENT
}

void rwble_reset(void)
{
    // Disable interrupts before reset procedure is completed
    GLOBAL_INT_DISABLE();

    // Reset the BLE core
    lld_reset();

    // init the link layer driver
    lld_init(true);

    #if (BLE_CENTRAL || BLE_PERIPHERAL)
    // Reset the link layer controller
    llc_reset();
    #endif /* #if (BLE_CENTRAL || BLE_PERIPHERAL) */

    // init the link layer manager
    llm_init(true);

    // Initialize Descriptors and buffers
    co_buf_init();

    // Restore interrupts after reset procedure is completed
    GLOBAL_INT_RESTORE();
}

void rwble_version(uint8_t* fw_version, uint8_t* hw_version)
{
    // FW version
    *(fw_version+3) = RWBLE_SW_VERSION_MAJOR;
    *(fw_version+2) = RWBLE_SW_VERSION_MINOR;
    *(fw_version+1) = RWBLE_SW_VERSION_BUILD;
    *(fw_version) = RWBLE_SW_VERSION_SUB_BUILD;

    // HW version 
    *(hw_version+3) = ble_typ_getf();
    *(hw_version+2) = ble_rel_getf();
    *(hw_version+1) = ble_upg_getf();
    *(hw_version) = ble_build_getf();
}

#if RW_BLE_SUPPORT

void rwble_send_message(uint32_t error)
{
    if(error == RESET_TO_ROM || error == RESET_AND_LOAD_FW)
    {
        // Send platform reset command complete if requested by user
        dbg_platform_reset_complete(error);
    }
    else
    {
        // Allocate a message structure for hardware error event
        struct hci_hw_err_evt *evt = KE_MSG_ALLOC(HCI_EVENT, 0, HCI_HW_ERR_EVT_CODE, hci_hw_err_evt);

        // Fill the HW error code
        switch(error)
        {
            case RESET_MEM_ALLOC_FAIL: evt->hw_code = CO_ERROR_HW_MEM_ALLOC_FAIL; break;
            default: ASSERT_INFO(0, error, 0); break;
        }

        // Send the message
        hci_send_2_host(evt);
    }
}

#endif //RW_BLE_SUPPORT

#if ((dg_configBLE_EVENT_COUNTER_ENABLE == 1) || (dg_configBLE_ADV_STOP_DELAY_ENABLE == 1))

/*
 * \brief Enable Event Counters in BLE ISR
 *
 * If the application has not defined #dg_configBLE_EVENT_COUNTER_ENABLE in its custom_config
 * file, this is defined to the default value of 0 to disable the Event Counters in BLE stack ISR.
 *
 */
#include "osal.h"

#if (dg_configBLE_ADV_STOP_DELAY_ENABLE == 1)
PRIVILEGED_DATA static bool rwble_evt_end_adv_ntf_en;
#endif

PRIVILEGED_DATA uint32_t adv_evt_cntr=0;
PRIVILEGED_DATA uint32_t slave_con_evt_cntr=0;
PRIVILEGED_DATA uint32_t master_con_evt_cntr=0;
static void count_events(void)
{
    // Pick the element in the programmed environment queue
    struct ea_elt_tag *elt = (struct ea_elt_tag *) co_list_pick(&lld_evt_env.elt_prog);
    // Get the event
    struct lld_evt_tag *evt = LLD_EVT_ENV_ADDR_GET(elt);
    // Update the counters
    if (evt->mode == LLD_EVT_ADV_MODE) {
#if (dg_configBLE_ADV_STOP_DELAY_ENABLE == 1)
        if (rwble_evt_end_adv_ntf_en) {
            ad_ble_task_notify_from_isr(mainBIT_EVENT_ADV_END);
        }
#endif
#if (dg_configBLE_EVENT_COUNTER_ENABLE == 1)
        adv_evt_cntr++;
#endif
    }
    if (evt->mode == LLD_EVT_SLV_MODE) {
#if (dg_configBLE_EVENT_COUNTER_ENABLE == 1)
        slave_con_evt_cntr++;
#endif
    }
    if (evt->mode == LLD_EVT_MST_MODE) {
#if (dg_configBLE_EVENT_COUNTER_ENABLE == 1)
        master_con_evt_cntr++;
#endif
    }
}
#endif

void ad_ble_cscnt_serviced(void);
void ad_ble_finegtim_serviced(void);

#if (dg_configIMAGE_SETUP == DEVELOPMENT_MODE)
extern uint32_t retained_slp_duration;
extern uint32_t ble_wakeups_cnt;
__RETAINED uint32_t ble_slp_delays_cnt;
__RETAINED uint32_t ble_slp_delays_max;
#endif

__RETAINED_CODE static void patched_lld_evt_end_isr(bool apfm)
{
    // Pop the element in the programmed environment queue
    struct ea_elt_tag *elt = (struct ea_elt_tag *)co_list_pop_front(&lld_evt_env.elt_prog);
    // Get BLE event environments
    struct lld_evt_tag *evt = LLD_EVT_ENV_ADDR_GET(elt);
    /// Instant of the next action (in events)
    uint16_t instant = evt->instant;
    /// Describe the action to be done when instant occurs
    uint8_t instant_action = evt->instant_action;
    /// Update offset
    uint16_t update_offset = evt->update_offset;
    /// Update window size
    uint8_t update_size = evt->update_size;
    bool restore_instant = false;

    if (LLD_EVT_FLAG_GET(evt, WAITING_INSTANT) && evt->counter != (evt->instant - 1) && evt->counter != (evt->instant))
        restore_instant = true;

    ea_elt_remove(elt);
    if (apfm)
    {
        // Get the associated BLE event
        struct lld_evt_tag *evt = LLD_EVT_ENV_ADDR_GET(elt);
        // Report the APFM error in element internal status
        LLD_EVT_FLAG_SET(evt, APFM);
    }
    else
    {
        // Clear ASAP flag if needed
        if(EA_ASAP_FIELD_GET_TYPE(elt->asap_field) != EA_FLAG_NO_ASAP)
        {
            EA_ASAP_FIELD_CLEAR(elt->asap_field);
        }
    }

#if (dg_configBLE_SKIP_LATENCY_API == 1) && (RWBLE_SW_VERSION_MAJOR >= 8)
    lld_evt_end_skip_latency(elt);
#else
    // Call back the end of event function
    lld_evt_end(elt);
#endif /* (dg_configBLE_SKIP_LATENCY_API == 1) && (RWBLE_SW_VERSION_MAJOR >= 8) */

    if(restore_instant && evt && !LLD_EVT_FLAG_GET(evt, DELETE))
    {
        evt->instant        = instant;
        evt->instant_action = instant_action;
        evt->update_offset  = update_offset;
        evt->update_size    = update_size;
    }
}



__RETAINED_CODE static void slp_isr(void)
{
#if (dg_configIMAGE_SETUP == DEVELOPMENT_MODE)
        uint32_t slp_period;
#endif


        //Sample the base time count
        ble_sampleclk_set(BLE_SAMP_BIT);
        while (ble_sampleclk_get());

        DBG_SWDIAG(BLE_ISR, SLPINT, 1);

        // Clear the interrupt
        ble_intack_clear(BLE_SLPINTACK_BIT);

        // Handle wake-up
        rwip_wakeup();


        DBG_SET_LOW(BLE_USE_TIMING_DEBUG, CPMDBG_BLE_IRQ);

        REG_CLR_BIT(GPREG, GP_CONTROL_REG, BLE_WAKEUP_REQ);


        // Check if the BLE was delayed
#if (dg_configIMAGE_SETUP == DEVELOPMENT_MODE)
        slp_period = ble_deepslstat_get();

        if ((retained_slp_duration != 0) && (retained_slp_duration < slp_period)) {
                ble_slp_delays_cnt++;

                ASSERT_WARNING(ble_slp_delays_cnt < (BLE_MAX_DELAYS_ALLOWED + 1));
        }

        if (ble_wakeups_cnt == (BLE_WAKEUP_MONITOR_PERIOD - 1)) {
                if (ble_slp_delays_cnt > ble_slp_delays_max) {
                        ble_slp_delays_max = ble_slp_delays_cnt;
                }
                ble_slp_delays_cnt = 0;
        }
#endif

        /*
         * Radio Subsystem initialization. Execute here after making sure that BLE core is awake.
         */
        rf_reinit_func();

        if (BLE_RX_EN_DEBUG == 1) {
                RFCU_POWER->RF_PORT_EN_REG |= 0x10;

                // Set to match the BLE_RADIOPWRUPDN_REG:RXPWRUP setting
                REG_SETF(RFCU_POWER, RF_CNTRL_TIMER_29_REG, SET_OFFSET, 0x54);
        }

        DBG_SWDIAG(BLE_ISR, SLPINT, 0);
}


__attribute__((noinline)) static void cscnt_isr(void);

__RETAINED_CODE static void cscnt_isr(void)
{
        DBG_SWDIAG(BLE_ISR, CSCNTINT, 1);

        // Handle end of wake-up
        rwip_wakeup_end();

        // Try to schedule immediately
        ea_finetimer_isr();

        // Notify Adapter
        ad_ble_cscnt_serviced();

        // Clear the interrupt
        ble_intack_clear(BLE_CSCNTINTACK_BIT);

#ifdef dg_configBLE_EVENT_NOTIF_HOOK_CSCNT_EVENT
        dg_configBLE_EVENT_NOTIF_HOOK_CSCNT_EVENT();
#endif


        DBG_SWDIAG(BLE_ISR, CSCNTINT, 0);
}


__attribute__((noinline)) static void finegt_isr(void);

static void finegt_isr(void)
{
        DBG_SWDIAG(BLE_ISR, FINETGTIMINT, 1);

        // Clear the interrupt
        ble_intack_clear(BLE_FINETGTIMINTACK_BIT);

        CHECK_AND_CALL_VOID(custom_pti_set);

        ea_finetimer_isr();

        // Notify Adapter
        ad_ble_finegtim_serviced();

#ifdef dg_configBLE_EVENT_NOTIF_HOOK_FINE_EVENT
        dg_configBLE_EVENT_NOTIF_HOOK_FINE_EVENT();
#endif

        DBG_SWDIAG(BLE_ISR, FINETGTIMINT, 0);
}


__attribute__((noinline)) static void rx_isr(void);

static void rx_isr(void)
{
        DBG_SWDIAG(BLE_ISR, RXINT, 1);

        ble_intack_clear(BLE_RXINTSTAT_BIT);

        lld_evt_rx_isr();

        DBG_SWDIAG(BLE_ISR, RXINT, 0);
}


__attribute__((noinline)) static void event_isr(void);

static void event_isr(void)
{
        DBG_SWDIAG(BLE_ISR, EVENTINT, 1);

        // Clear the interrupt
        ble_intack_clear(BLE_EVENTINTSTAT_BIT);

#if (BLE_WINDOW_STATISTICS == 1)
        rxwin_calculate_lag();
#endif

#if ((dg_configBLE_EVENT_COUNTER_ENABLE == 1) || (dg_configBLE_ADV_STOP_DELAY_ENABLE == 1))
        count_events();
#endif

#ifdef dg_configBLE_EVENT_NOTIF_HOOK_END_EVENT
        dg_configBLE_EVENT_NOTIF_HOOK_END_EVENT();
#endif

        patched_lld_evt_end_isr(false);
        //            // In case where the RX irq is raised at the same time, do the processing under EVENT irq
        //            if (irq_stat & BLE_RXINTSTAT_BIT)
        //            {
        //                // Clear the RX irq
        //                ble_intack_clear(BLE_RXINTSTAT_BIT);
        //                irq_stat &= ~BLE_RXINTSTAT_BIT;
        //            }

        DBG_SWDIAG(BLE_ISR, EVENTINT, 0);
}


__attribute__((noinline)) static void eventapfa_isr(void);

static void eventapfa_isr(void)
{
        // Clear the interrupt
        ble_intack_clear(BLE_EVENTAPFAINTSTAT_BIT);

        patched_lld_evt_end_isr(true);
}


__attribute__((noinline)) static void sw_isr(void);

static void sw_isr(void)
{
        // Clear the interrupt
        ble_intack_clear(BLE_SWINTSTAT_BIT);

        ea_sw_isr();
}


__attribute__((noinline)) static void grosstim_isr(void);

static void grosstim_isr(void)
{
        DBG_SWDIAG(BLE_ISR, GROSSTGTIMINT, 1);

        // Clear the interrupt
        ble_intack_clear(BLE_GROSSTGTIMINTACK_BIT);

        lld_evt_timer_isr();

        DBG_SWDIAG(BLE_ISR, GROSSTGTIMINT, 0);
}


__attribute__((noinline)) static void crypt_isr(void);

static void crypt_isr(void)
{
        DBG_SWDIAG(BLE_ISR, CRYPTINT, 1);

        ble_intack_clear(BLE_CRYPTINTSTAT_BIT);

        lld_crypt_isr();

        DBG_SWDIAG(BLE_ISR, CRYPTINT, 0);
}


__RETAINED_CODE __BLEIRQ void rwble_isr(void)
{
        // Loop until no more interrupts have to be handled
        while (1)
        {
                // Check BLE interrupt status and call the appropriate handlers
                uint32_t irq_stat = ble_intstat_get();
                if (irq_stat == 0)
                        break;

#if DEEP_SLEEP
#if RW_BLE_SUPPORT
                // End of sleep interrupt
                if (irq_stat & BLE_SLPINTSTAT_BIT)
                {
                        slp_isr();
                }

                // Slot interrupt
                if (irq_stat & BLE_CSCNTINTSTAT_BIT)
                {
                        cscnt_isr();
                }
#endif //RW_BLE_SUPPORT
#endif //DEEP_SLEEP

                // Event target interrupt
                if (irq_stat & BLE_FINETGTIMINTSTAT_BIT)
                {
                        finegt_isr();
                }

                // Rx interrupt
                if (irq_stat & BLE_RXINTSTAT_BIT)
                {
                        rx_isr();
                }

                // End of event interrupt
                if (irq_stat & BLE_EVENTINTSTAT_BIT)
                {
                        RF_CHECK_DCOFFSET();
                        event_isr();
                }

                // AFPM interrupt
                if (irq_stat & BLE_EVENTAPFAINTSTAT_BIT)
                {
                        eventapfa_isr();
                }

                // SW interrupt
                if (irq_stat & BLE_SWINTSTAT_BIT)
                {
                        sw_isr();
                }

#if (RW_BLE_SUPPORT)
                // General purpose timer interrupt
                if (irq_stat & BLE_GROSSTGTIMINTSTAT_BIT)
                {
                        grosstim_isr();
                }
#endif //(RW_BLE_SUPPORT)

                // End of encryption interrupt
                if (irq_stat & BLE_CRYPTINTSTAT_BIT)
                {
                        crypt_isr();
                }

                // Error interrupt
                if (irq_stat & BLE_ERRORINTSTAT_BIT)
                {
                        DBG_SWDIAG(BLE_ISR, ERRORINT, 1);

                        // Clear the interrupt
                        ble_intack_clear(BLE_ERRORINTSTAT_BIT);

                        DBG_SWDIAG(BLE_ISR, ERRORINT, 0);

                        ASSERT_ERROR(0);
                }
        }
}

#if (dg_configBLE_ADV_STOP_DELAY_ENABLE == 1)
void rwble_evt_end_adv_ntf_set(bool enable)
{
        rwble_evt_end_adv_ntf_en = enable;
}
#endif


#if (dg_configBLE_SKIP_LATENCY_API == 1) && (RWBLE_SW_VERSION_MAJOR >= 8)
__RETAINED_CODE static void lld_evt_check_md_bit(struct lld_evt_tag *evt, uint8_t rx_desc_cnt)
{
    bool skip_latency_enabled = ble_mgr_skip_latency_get_from_isr(gapc_get_conidx(evt->conhdl));

    if (skip_latency_enabled) {
            /*
             * Force MD bit to 1
             */
            evt->last_md_rx = 1;
    } else {
            /*
             * Read MD bit from the last Rx packet
             */
            uint8_t current_rx_hdl = (co_buf_rx_current_get() + (rx_desc_cnt - 1))% BLE_RX_BUFFER_CNT;
            evt->last_md_rx = ble_rxmd_getf(current_rx_hdl);
    }
}

__RETAINED_CODE static void lld_evt_end_skip_latency(struct ea_elt_tag *elt)
{
    // Get the associated BLE event
    struct lld_evt_tag *evt = LLD_EVT_ENV_ADDR_GET(elt);
    // Read total number of RX descriptor burnt during the event
    uint8_t rx_desc_cnt = ble_rxdesccnt_getf(evt->conhdl);
    // Defer element status
    bool defer_elt = true;
    // Defer element status
    bool try_to_insert_elt = true;

    // Check if the End of Event comes from APFM (smooth or immediate) abort or a normal End of Evt
    if (LLD_EVT_FLAG_GET(evt, APFM))
    {
        // Reset the flag
        LLD_EVT_FLAG_RESET(evt, APFM);

        if (!rx_desc_cnt)
        {
            // Check if the event can be restarted and no RX pending
            if(lld_evt_continue(elt) != EA_ERROR_OK)
            {
                defer_elt = false;
                try_to_insert_elt = false;
            }
        }
    }
    else
    {
        // If no APFM issue, reset the priority with the default one
        lld_evt_priority_reset(elt);
    }

    // Check if the tx/rx deferring is authorized
    if(defer_elt)
    {

        if(evt->mode != LLD_EVT_TEST_MODE)
        {
            ASSERT_INFO(rx_desc_cnt >= evt->rx_cnt, rx_desc_cnt, evt->rx_cnt);
            // Push the element in the Tx / Rx pending list
            lld_evt_deferred_elt_push(elt, RWBLE_DEFER_TYPE_END, (rx_desc_cnt - evt->rx_cnt));
            // Check the MD bit value to restart the event with or without latency
            lld_evt_check_md_bit(evt,rx_desc_cnt);
        }
        else
        {
            // Push the element in the Tx / Rx pending list
            lld_evt_deferred_elt_push(elt, RWBLE_DEFER_TYPE_TEST_END, (LLD_RX_IRQ_THRES > 0) ? (rx_desc_cnt % LLD_RX_IRQ_THRES) : 0);
        }



        // Reset RX counter stored in the event environment
        evt->rx_cnt = 0;

        // Defer the TX and RX handling
        ke_event_set(KE_EVENT_BLE_EVT_DEFER);
    }

    if (!LLD_EVT_FLAG_GET(evt, NO_RESTART) &&
            !LLD_EVT_FLAG_GET(evt, DELETE) && (try_to_insert_elt))
    {
        // Restart the event
        lld_evt_restart(elt, false);
        if (!LLD_EVT_FLAG_GET(evt, DELETE))
        {
            // Check if the element should be stopped and freed
            while (ea_elt_insert(elt) == EA_ERROR_REJECTED)
            {
                lld_evt_restart(elt, true);
                if (LLD_EVT_FLAG_GET(evt, DELETE))
                {
                    break;
                }
            }
        }
    }
    // If a deletion is requested the defer will free the element
}
#endif /* (dg_configBLE_SKIP_LATENCY_API == 1) && (RWBLE_SW_VERSION_MAJOR >= 8) */

/// @} RWBLE
