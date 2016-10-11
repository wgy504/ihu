/**
 ****************************************************************************************
 *
 * @file arch_main.c
 *
 * @brief Main loop of the application.
 *
 * Copyright (C) RivieraWaves 2009-2012
 *
 * $Rev: 13481 $
 *
 ****************************************************************************************
 */

//0x050508030001fc0a0004700002200401020304
//0x050508030002fc0a0004700020200411121314

/*
 * INCLUDES
 ****************************************************************************************
 */
#include "ble_config.h"

#include <stdlib.h>
#include <stddef.h>     // standard definitions
#include <stdint.h>     // standard integer definition
#include <stdbool.h>    // boolean definition

#include "co_version.h"
#include "rwip_config.h"// RW SW configuration
#include "rwip.h"       // RW definitions
#include "arch.h"
#include "boot.h"       // boot definition
#include "rwip.h"       // BLE initialization
#include "em_map_ble.h"
#include "ke_mem.h"
#include "smpc.h"
#include "llc.h"
#if PLF_UART
#endif //PLF_UART
#include "nvds.h"       // NVDS initialization
#include "flash.h"      // Flash initialization
#if (BLE_EMB_PRESENT)
#include "rf.h"         // RF initialization
#endif // BLE_EMB_PRESENT

#if (BLE_HOST_PRESENT)
#include "rwble_hl.h"   // BLE HL definitions
#include "gapc.h"
#include "smpc.h"
#include "gattc.h"
#include "attc.h"
#include "atts.h"
#include "l2cc.h"
#endif //BLE_HOST_PRESENT

#if (TL_ITF)
#include "h4tl.h"
#endif //TL_ITF

#if (GTL_ITF)
#include "gtl.h"
#endif //GTL_ITF

#if (HCI_PRESENT)
#include "hci.h"             // HCI definition
#endif //HCI_PRESENT

#if (KE_SUPPORT)
#include "ke.h"              // kernel definition
#include "ke_env.h"          // ke_env
#include "ke_event.h"        // kernel event
#include "ke_timer.h"        // definitions for timer
#include "ke_mem.h"          // kernel memory manager
#endif //KE_SUPPORT

#if PLF_DEBUG
#include "dbg.h"        // For dbg_warning function
#endif //PLF_DEBUG


#include "em_map_ble_user.h"
#include "em_map_ble.h"

#include "lld_sleep.h"
#include "rwble.h"
#include "hw_rf.h"
#include "ad_rf.h"

#if (BLE_APP_PRESENT && BLE_PROX_REPORTER)
#include "app_proxr.h"
#endif

#if BLE_BATT_SERVER
//#include "app_batt.h"
#endif

#include "ke_event.h"

#include "pll_vcocal_lut.h"

#include "gapm_task.h"   

#if (USE_BLE_SLEEP == 0)
#pragma message "BLE sleep is disabled!"
#endif

#if (dg_configUSE_HW_TRNG == 0)
#warning "HW RNG is disabled!"
#endif
#include "hw_trng.h"

#include "hw_cpm.h"
#include "sys_clock_mgr.h"
#include "sys_power_mgr.h"


/**
 * @addtogroup DRIVERS
 * @{
 */
#if (dg_configUSE_LP_CLK == LP_CLK_RCX)
extern uint16_t rcx_clock_hz;
extern uint16_t rcx_tick_rate_hz;
#endif
extern uint32_t rcx_clock_period;
extern uint32_t ble_slot_duration_in_rcx;

#ifdef BLE_PROD_TEST
void lld_evt_deffered_elt_handler_custom(void);
#endif

/*
 * DEFINES
 ****************************************************************************************
 */

#if BLE_APP_PRESENT
# if !defined(CFG_PRINTF)
#  undef PROGRAM_ENABLE_UART
# else
#  define PROGRAM_ENABLE_UART
# endif
#endif // BLE_APP_PRESENT


#ifdef PROGRAM_ENABLE_UART
#include "uart.h"      // UART initialization
#endif

#if RWBLE_SW_VERSION_MAJOR >= 8
#define DUMMY_SIZE                      _EM_BLE_END
#else
#define DUMMY_SIZE                      EM_BLE_END
#endif

/*
 * STRUCTURE DEFINITIONS
 ****************************************************************************************
 */

/// Description of unloaded RAM area content
struct unloaded_area_tag
{
        uint32_t error;
};


/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

const struct gap_cfg_user_struct gap_cfg_user_var_struct = {
        .GAP_TMR_LECB_DISCONN_TIMEOUT_VAR = 0x0BB8,
        .GAP_APPEARANCE_VAR = 0x0000,
        .GAP_PPCP_CONN_INTV_MAX_VAR = 0x0064,
        .GAP_PPCP_CONN_INTV_MIN_VAR = 0x00c8,
        .GAP_PPCP_SLAVE_LATENCY_VAR = 0x0000,
        .GAP_PPCP_STO_MULT_VAR = 0x07d0,
        .GAP_TMR_LIM_ADV_TIMEOUT_VAR = 0x4650,
        .GAP_TMR_GEN_DISC_SCAN_VAR = 0x0300,
        .GAP_TMR_LIM_DISC_SCAN_VAR = 0x0300,
        .GAP_TMR_PRIV_ADDR_INT_VAR = 0x3A98,
        .GAP_TMR_CONN_PAUSE_CT_VAR = 0x0064,
        .GAP_TMR_CONN_PAUSE_PH_VAR = 0x01F4,
        .GAP_TMR_CONN_PARAM_TIMEOUT_VAR = 0x0BB8,
        .GAP_TMR_LECB_CONN_TIMEOUT_VAR = 0x0BB8,
        .GAP_TMR_SCAN_FAST_PERIOD_VAR = 0x0C00,
        .GAP_TMR_ADV_FAST_PERIOD_VAR = 0x0BB8,
        .GAP_LIM_DISC_SCAN_INT_VAR = 0x0012,
        .GAP_SCAN_FAST_INTV_VAR = 0x0030,
        .GAP_SCAN_FAST_WIND_VAR = 0x0030,
        .GAP_SCAN_SLOW_INTV1_VAR = 0x00CD,
        .GAP_SCAN_SLOW_INTV2_VAR = 0x019A,
        .GAP_SCAN_SLOW_WIND1_VAR = 0x0012,
        .GAP_SCAN_SLOW_WIND2_VAR = 0x0024,
        .GAP_ADV_FAST_INTV1_VAR = 0x0030,
        .GAP_ADV_FAST_INTV2_VAR = 0x0064,
        .GAP_ADV_SLOW_INTV_VAR = 0x00B0,
        .GAP_INIT_CONN_MIN_INTV_VAR = 0x0018,
        .GAP_INIT_CONN_MAX_INTV_VAR = 0x0028,
        .GAP_INQ_SCAN_INTV_VAR = 0x0012,
        .GAP_INQ_SCAN_WIND_VAR = 0x0012,
        .GAP_CONN_SUPERV_TIMEOUT_VAR = 0x07D0,
        .GAP_CONN_MIN_CE_VAR = 0x0000,
        .GAP_CONN_MAX_CE_VAR = 0xFFFF,
        .GAP_CONN_LATENCY_VAR = 0x0000,
        //.GAP_LE_MASK_VAR = 0x1F,
        .GAP_MAX_LE_MTU_VAR = 512,
        .GAP_DEV_NAME_VAR = "RIVIERAWAVES-BLE",
        //.GAP_DEV_NAME_VAR ="DIALOG SEMICONDUCTOR",
};

/// Variable storing the reason of platform reset
static uint32_t reset_reason __RETAINED = RESET_NO_ERROR;

#ifdef RAM_BUILD
/// Reserve space for the BLE ROM variables
unsigned char ble_rom_vars[0x7FE0000 - BLE_VAR_ADDR] __attribute__((section("ble_variables"))) __attribute__((unused));
#else
/// Reserve space for Exchange Memory, this section is linked first in the section "exchange_mem_case"
volatile uint8 dummy[DUMMY_SIZE] __attribute__((section("exchange_mem_case1")));
#endif

extern bool rf_in_sleep;                        // set to '1' when the BLE is sleeping, else 0

void ble_init(uint32_t base);
bool rwip_check_wakeup_boundary(void);
bool rwip_check_wakeup_boundary_rcx(void);

bool rwip_check_wakeup_boundary_any(void);
uint32_t rwip_slot_2_lpcycles_any(uint32_t slot_cnt);

void ble_regs_push(void);
void ble_regs_pop(void);
extern void patch_rom_functions(void);

#if (dg_configIMAGE_SETUP == DEVELOPMENT_MODE)
uint32_t retained_slp_duration __RETAINED;
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
// Hooks based on previous version can be added in the rom_func_addr_table_var[] definition.
////////////////////////////////////////////////////////////////////////////////////////////////////

/*
 * LOCAL FUNCTION DECLARATIONS
 ****************************************************************************************
 */
__RETAINED_CODE static uint32_t rwip_slot_2_lpcycles_xtal(uint32_t slot_cnt)
{
        uint32_t lpcycles;

        // Sanity check: The number of slots should not be too high to avoid overflow
        ASSERT_WARNING(slot_cnt < 1000000);

        if (dg_configUSE_LP_CLK == LP_CLK_32000) {
                // Compute the low power clock cycles - case of a 32kHz clock
                lpcycles = slot_cnt * 20;
        }
        else if (dg_configUSE_LP_CLK == LP_CLK_32768) {
                // Compute the low power clock cycles - case of a 32.768kHz clock
                lpcycles = (slot_cnt << 11)/100;
        }

        // Corner case, Sleep duration is exactly on slot boundary so slot interrupt will not
        // be generated on expected slot (but next one).

        // So reduce little bit sleep duration in order to allow fine time compensation
        // Note compensation will be in range of [1 , 2[ lp cycles

        lpcycles--;

        return(lpcycles);
}

__RETAINED_CODE static uint32_t rwip_slot_2_lpcycles_rcx(uint32_t slot_cnt)
{
        uint64_t lpcycles;

        lpcycles = (uint64_t)ble_slot_duration_in_rcx * (uint64_t)slot_cnt;
        lpcycles = (uint32_t)(lpcycles / 1000000);

        return (uint32_t)lpcycles;
}

__RETAINED_CODE uint32_t lld_sleep_lpcycles_2_us_rcx(uint32_t lpcycles)
{
        uint64_t res;
        uint32_t usec;

        res = (uint64_t)lpcycles * (uint64_t)rcx_clock_period;
        res = res >> 20;

        usec = (uint32_t)res;

        return usec;
}

__RETAINED_CODE void lld_sleep_compensate_func_rcx(void)
{
    uint32_t dur_us;
    // Get the number of low power sleep period
    uint32_t slp_period = ble_deepslstat_get();

    uint16_t fintetime_correction;

    // Sanity check: The duration of a sleep is limited
    ASSERT_ERR(slp_period < LLD_EVT_MAX_SLEEP_DURATION);

    // Convert sleep duration into us
    dur_us = lld_sleep_lpcycles_2_us_rcx(slp_period);

    // The correction values are then deduced from the sleep duration in us
    ble_basetimecntcorr_set(dur_us / LLD_EVT_SLOT_DURATION);

    // if the sleep duration is a multiple of slot then fine timer correction is set to 0 else is set to the difference
    fintetime_correction = (((dur_us % LLD_EVT_SLOT_DURATION) == 0) ? 0 : (LLD_EVT_SLOT_DURATION - (dur_us % LLD_EVT_SLOT_DURATION)));
    ble_finecntcorr_set(fintetime_correction);

    // Start the correction
    ble_deep_sleep_corr_en_setf(1);
}


/**
 ****************************************************************************************
 * @brief Initialization of ble core
 *
 ****************************************************************************************
 */
void init_pwr_and_clk_ble(void)
{
        uint32_t reg_local;
        /*
         * Power up BLE core & reset BLE Timers
         */
        GLOBAL_INT_DISABLE();

	hw_rf_request_on(true);
        
	reg_local = CRG_TOP->PMU_CTRL_REG;
        REG_CLR_FIELD(CRG_TOP, PMU_CTRL_REG, BLE_SLEEP, reg_local);

        if ((dg_configUSE_BOD == 1) && ((dg_configBLACK_ORCA_IC_REV == BLACK_ORCA_IC_REV_A)
                || ((dg_configUSE_AUTO_CHIP_DETECTION == 1) && (CHIP_IS_AD || CHIP_IS_AE)))) {
                hw_cpm_deactivate_bod_protection();
        }

        CRG_TOP->PMU_CTRL_REG = reg_local;

        if ((dg_configUSE_BOD == 1) && ((dg_configBLACK_ORCA_IC_REV == BLACK_ORCA_IC_REV_A)
                || ((dg_configUSE_AUTO_CHIP_DETECTION == 1) && (CHIP_IS_AD || CHIP_IS_AE)))) {
                hw_cpm_delay_usec(30);
                hw_cpm_activate_bod_protection();
        }

        reg_local = CRG_TOP->CLK_RADIO_REG;
        REG_SET_FIELD(CRG_TOP, CLK_RADIO_REG, BLE_ENABLE, reg_local, 1);
        REG_CLR_FIELD(CRG_TOP, CLK_RADIO_REG, BLE_DIV, reg_local);
        CRG_TOP->CLK_RADIO_REG = reg_local;

        GLOBAL_INT_RESTORE();

        while (!REG_GETF(CRG_TOP, SYS_STAT_REG, BLE_IS_UP)); // Wait for the BLE to wake up

        if (BLE->BLE_DEEPSLCNTL_REG & REG_MSK(BLE, BLE_DEEPSLCNTL_REG, DEEP_SLEEP_STAT)) {
                REG_SET_BIT(BLE, BLE_DEEPSLCNTL_REG, SOFT_WAKEUP_REQ);
                __NOP();
                __NOP();
                __NOP();
                while (BLE->BLE_DEEPSLCNTL_REG & REG_MSK(BLE, BLE_DEEPSLCNTL_REG, DEEP_SLEEP_STAT)) {
                        __NOP();
                }
        }

        // Reset the timing generator
        reg_local = BLE->BLE_RWBLECNTL_REG;
        REG_SET_FIELD(BLE, BLE_RWBLECNTL_REG, MASTER_SOFT_RST, reg_local, 1);
        REG_SET_FIELD(BLE, BLE_RWBLECNTL_REG, MASTER_TGSOFT_RST, reg_local, 1);
        BLE->BLE_RWBLECNTL_REG = reg_local;
        while (ble_master_tgsoft_rst_getf()) {}

        GLOBAL_INT_DISABLE();

        REG_SET_BIT(CRG_TOP, CLK_RADIO_REG, BLE_LP_RESET);      // Apply HW reset to BLE_Timers

        while (!REG_GETF(CRG_TOP, SYS_STAT_REG, RAD_IS_UP)); // Wait for the radio to wake up

        REG_CLR_BIT(CRG_TOP, CLK_RADIO_REG, BLE_LP_RESET);

        GLOBAL_INT_RESTORE();

        /*
         * Just make sure that BLE core is stopped (if already running)
         */
        while (!REG_GETF(CRG_TOP, SYS_STAT_REG, BLE_IS_UP)) {
                __NOP();
        }
        REG_CLR_BIT(BLE, BLE_RWBLECNTL_REG, RWBLE_EN);

        /*
         * Since BLE is stopped (and powered), set CLK_SEL
         */
        reg_local = BLE->BLE_CNTL2_REG;
        REG_SET_FIELD(BLE, BLE_CNTL2_REG, BLE_CLK_SEL, reg_local, 16);

        REG_SET_FIELD(BLE, BLE_CNTL2_REG, BLE_RSSI_SEL, reg_local, 1);
        BLE->BLE_CNTL2_REG = reg_local;

        /*
         * Set spi interface to software
         */
        // no BB_ONLY mode in 680

#if (dg_configUSE_CRYPTO == 1)
        GLOBAL_INT_DISABLE();
        //enable AES module
        REG_SETF (CRG_TOP, CLK_AMBA_REG, AES_CLK_ENABLE, 1);
        //enable ECC module
        REG_SETF (CRG_TOP, CLK_AMBA_REG, ECC_CLK_ENABLE, 1);
        //enable TRNG module
        REG_SETF (CRG_TOP, CLK_AMBA_REG, TRNG_CLK_ENABLE, 1);
        GLOBAL_INT_RESTORE();
#endif
}

void SetSystemVars_func(void)
{
}

/**
 ****************************************************************************************
 * @brief Map port pins
 *
 * The Uart and SPI port pins and GPIO ports (for debugging) are mapped
 ****************************************************************************************
 */
void set_pad_functions(void)
{
}


/**
 ****************************************************************************************
 * @brief Enable pad's and peripheral clocks assuming that peripherals' power domain is down.
 *
 * The Uart and SPi clocks are set.
 ****************************************************************************************
 */
void periph_init(void)
{
}



/*
 * EXPORTED FUNCTION DEFINITIONS
 ****************************************************************************************
 */
void conditionally_run_radio_cals(void)
{
}


/*
 * MAIN FUNCTION
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief BLE main function.
 *
 * This function is called right after the booting process has completed.
 ****************************************************************************************
 */

void ble_stack_init(void)
{
#ifndef RAM_BUILD
        platform_initialization();
        _ble_base = (unsigned int) dummy;
#endif

        /*
         * Apply ROM patches
         */
        patch_rom_functions();

        init_pwr_and_clk_ble();

        REG_CLR_BIT(BLE, BLE_CNTL2_REG, SW_RPL_SPI);

#ifndef RAM_BUILD
        /* Don't remove next line otherwise dummy[0] could be optimized away. The dummy array
         * is intended to reserve the needed Exchange Memory space in the retention memory.
         */
        dummy[0] = dummy[0];
#endif

        /*
         ************************************************************************************
         * BLE initialization
         ************************************************************************************
         */

#ifdef UNCALIBRATED_AT_FAB
        GPIO->RF_LNA_CTRL1_REG = 0x24E;
        GPIO->RF_LNA_CTRL2_REG = 0x26;
        GPIO->RF_LNA_CTRL3_REG = 0x7;
        GPIO->RF_REF_OSC_REG = 0x29AC;
        GPIO->RF_RSSI_COMP_CTRL_REG = 0x7777;
        GPIO->RF_VCO_CTRL_REG = 0x1;
#endif

        ble_init(EM_BASE_ADDR);

#ifdef RADIO_RIPPLE
        /* Set spi to HW (Ble)
         * Necessary: So from this point the BLE HW can generate spi burst iso SW
         * SPI BURSTS are necessary for the radio TX and RX burst, done by hardware
         * beause of the accurate desired timing
         */
        REG_SET_BIT(BLE, BLE_CNTL2_REG, SW_RPL_SPI);
#endif

        //Enable BLE core
        REG_SET_BIT(BLE, BLE_RWBLECNTL_REG, RWBLE_EN);

#if RW_BLE_SUPPORT && HCIC_ITF

        // If FW initializes due to FW reset, send the message to Host
//        if (error != RESET_NO_ERROR) {
//                rwble_send_message(error);
//        }
#endif


        if (BLE_USE_TIMING_DEBUG == 1) {
#define BLE_BLE_CNTL2_REG_DIAG5_Pos (5)
                REG_SET_BIT(BLE, BLE_CNTL2_REG, DIAG5);
#undef BLE_BLE_CNTL2_REG_DIAG5_Pos
        }

        /*
         ************************************************************************************
         * Sleep mode initializations
         ************************************************************************************
         */
        if (USE_BLE_SLEEP == 1) {
                rwip_env.sleep_enable = true;
        }
        rwip_env.ext_wakeup_enable = true;

        /*
         ************************************************************************************
         * PLL-LUT and MGC_KMODALPHA
         ************************************************************************************
         */

#ifdef BLE_PROD_TEST
        ke_event_callback_set(KE_EVENT_BLE_EVT_DEFER, &lld_evt_deffered_elt_handler_custom);
#endif


}

/// @} DRIVERS
#ifdef RAM_BUILD
void platform_reset_func(uint32_t error)
{
        reset_reason = error;

        ASSERT_ERROR(0);
}
#endif


/*********************************************************************************
 *** WAKEUP_LP_INT ISR
 ***/
#if (dg_configIMAGE_SETUP == DEVELOPMENT_MODE)
__RETAINED uint32_t ble_slp_misses_cnt;
__RETAINED uint32_t ble_slp_misses_max;
__RETAINED uint32_t ble_wakeups_cnt;
#endif

__RETAINED_CODE void ble_lp_isr(void)
{
        /*
         * Since XTAL 16MHz is activated, power-up the Radio Subsystem (including BLE)
         *
         * Note that BLE core clock is masked in order to handle the case where RADIO_PD does not
         * get into power down state. The BLE clock should be active only as long as system is
         * running at XTAL 16MHz. Also the BLE clock should be enabled before powering up the RADIO
         * Power Domain !
         */
        GLOBAL_INT_DISABLE();
        REG_SET_BIT(CRG_TOP, CLK_RADIO_REG, BLE_ENABLE);  // BLE clock enable
        REG_CLR_BIT(CRG_TOP, PMU_CTRL_REG, BLE_SLEEP);
        GLOBAL_INT_RESTORE();
        while (!REG_GETF(CRG_TOP, SYS_STAT_REG, BLE_IS_UP)) {}

        GLOBAL_INT_DISABLE();
        hw_rf_request_on(true);
        GLOBAL_INT_RESTORE();

        /*
         * BLE is up. The register status can be restored.
         */
        ble_regs_pop();

        /*
         * Check if BLE_SLP_IRQ has already asserted. In this case, we are delayed.
         */
#if (dg_configIMAGE_SETUP == DEVELOPMENT_MODE)
        ble_wakeups_cnt++;

        if (REG_GETF(BLE, BLE_INTSTAT_REG, SLPINTSTAT)) {
                ble_slp_misses_cnt++;

                ASSERT_WARNING(ble_slp_misses_cnt < (BLE_MAX_MISSES_ALLOWED + 1));
        }

        if (ble_wakeups_cnt == BLE_WAKEUP_MONITOR_PERIOD) {
                if (ble_slp_misses_cnt > ble_slp_misses_max) {
                        ble_slp_misses_max = ble_slp_misses_cnt;
                }
                ble_wakeups_cnt = 0;
                ble_slp_misses_cnt = 0;
        }
#endif

        DBG_SET_LOW(BLE_USE_TIMING_DEBUG, CPMDBG_BLE_IRQ);

        /*
         * Wait for BLE_SLP_IRQ to be asserted.
         */
        while (!REG_GETF(BLE, BLE_INTSTAT_REG, SLPINTSTAT)) {}
}


/**
 * @brief       Wake the BLE core via an external request.
 *
 * @details     If the BLE core is sleeping (permanently or not and external wake-up is enabled)
 *              then this function wakes it up.
 *
 * @attention   Return to normal mode if the BLE core was in permanent sleep.
 *
 * @return      bool
 *
 * @retval      The status of the requested operation.
 *              <ul>
 *                  <li> false, if the BLE core is not sleeping
 *                  <li> true, if the BLE core was woken-up successfully
 *              </ul>
 *
 */
bool ble_force_wakeup(void)
{
        bool retval = false;

        GLOBAL_INT_DISABLE();

        // If the BLE is sleeping, wake it up!
        if (REG_GETF(CRG_TOP, CLK_RADIO_REG, BLE_ENABLE) == 0) {                   // BLE clock is off
                if (REG_GETF(GPREG, GP_CONTROL_REG, BLE_WAKEUP_REQ) == 0) {        // No previous wake-up req
                        REG_SET_BIT(GPREG, GP_CONTROL_REG, BLE_WAKEUP_REQ);
                        pm_resource_sleeps_until(PM_BLE_ID, 4);                    // 3-4 LP cycles are needed
                        retval = true;
                }
        }

        GLOBAL_INT_RESTORE();

        return retval;
}


#define CUSTOM_INIT(pos, func, cast)            func = (cast)rom_func_addr_table_var[pos];

#if RWBLE_SW_VERSION_MAJOR >= 8
#include "em_map_ble.h"
#include "rwble_config.h"
extern unsigned int REG_BLE_EM_TX_BUFFER_SIZE;
extern unsigned int REG_BLE_EM_RX_BUFFER_SIZE;

extern uint8_t ble_duplicate_filter_max;
extern bool ble_duplicate_filter_found;
extern uint8_t llm_resolving_list_max;
#endif

void ble_platform_initialization(void)
{
        _ble_base = BLE_VAR_ADDR;

#if RWBLE_SW_VERSION_MAJOR >= 8
        REG_BLE_EM_RX_BUFFER_SIZE = dg_configBLE_DATA_LENGTH_RX_MAX + 11;
        REG_BLE_EM_TX_BUFFER_SIZE = dg_configBLE_DATA_LENGTH_TX_MAX + 11;
#endif

        use_h4tl = 0;   // 0 = GTL auto
        uint32_t seed;
        gap_cfg_user = (struct gap_cfg_user_struct*)&gap_cfg_user_var_struct;
        rom_func_addr_table = (uint32_t *)&rom_func_addr_table_var[0];
        rom_cfg_table = (uint32_t *)&rom_cfg_table_var[0];
#if RWBLE_SW_VERSION_MAJOR >= 8
        ble_duplicate_filter_max = dg_configBLE_DUPLICATE_FILTER_MAX;
        ble_duplicate_filter_found = true;      //What will be reported if max is exceeded, true means the extra devices are considered to be in the list and will not be reported
        llm_resolving_list_max = LLM_RESOLVING_LIST_MAX;        //this is the maximum spec value but it could require large amounts of heap 255*50=12750
#endif

        CUSTOM_INIT(custom_pti_set_func_pos, custom_pti_set, unsigned char (*)(void));

        // Initialize random process
        // Randomize seed
        if (dg_configUSE_HW_TRNG == 1) {
                hw_trng_enable(NULL);
                while (hw_trng_get_fifo_level() < 1) {
                        ;
                }
                hw_trng_get_numbers(&seed, 1);
                hw_trng_disable();
        }

        srand(seed);

}

/**
 * @brief       Check if the BLE core can enter sleep and, if so, enter sleep.
 *
 * @return      int
 *
 * @retval      The status of the requested operation.
 *              <ul>
 *                  <li> 0, if the BLE core cannot sleep
 *                  <li> 1, if the BLE core was put to sleep
 *                  <li> other, if the BLE core has to stay active but the caller may block
 *              </ul>
 *
 */

uint32_t logged_sleep_duration __RETAINED;

static int ble_rwip_sleep(bool forever, uint32_t *sleep_duration)
{
        uint32_t sleep_duration_in_lp_cycles;
        uint32_t dummy = MAX_SLEEP_DURATION_PERIODIC_WAKEUP_DEF;
        uint32_t wup_latency;
        int result = 0;

        DBG_SWDIAG(SLEEP, ALGO, 0);

        if (forever) {
                *sleep_duration = -1;
        } else {
                *sleep_duration = MAX_SLEEP_DURATION_EXTERNAL_WAKEUP_DEF;
        }

        do
        {
                /************************************************************************
                 **************            CHECK KERNEL EVENTS             **************
                 ************************************************************************/
                // Check if some kernel processing is ongoing
                if (!ke_sleep_check()) {
                        break;
                }

                result = -1;

                DBG_SWDIAG(SLEEP, ALGO, 1);
#if (DEEP_SLEEP)
                /************************************************************************
                 **************             CHECK ENABLE FLAG              **************
                 ************************************************************************/
                // Check sleep enable flag
                if (!rwip_env.sleep_enable) {
                        break;
                }

                /************************************************************************
                 **************              CHECK RW FLAGS                **************
                 ************************************************************************/
                // First check if no pending procedure prevent from going to sleep
                if (rwip_env.prevent_sleep != 0) {
                        break;
                }

                DBG_SWDIAG(SLEEP, ALGO, 2);

                /************************************************************************
                 **************           CHECK EXT WAKEUP FLAG            **************
                 ************************************************************************/
                /* If external wakeup enable, sleep duration can be set to maximum, otherwise
                 * system should be woken-up periodically to poll incoming packets from HCI */
                if ((BLE_APP_PRESENT == 0) && !rwip_env.ext_wakeup_enable) {
                        *sleep_duration = rom_cfg_table_var[max_sleep_duration_periodic_wakeup_pos];
                }

                /************************************************************************
                 **************            CHECK KERNEL TIMERS             **************
                 ************************************************************************/
                // If there's any timer pending, compute the time to wake-up to serve it
                if (ke_env.queue_timer.first != NULL) {
                        *sleep_duration = (BLE_GROSSTARGET_MASK >> 1); /* KE_TIMER_DELAY_MAX */
                }

                /************************************************************************
                 **************       DUMMY CHECKS FOR TIMERS AND EA       **************
                 *  to make sure that sleep will be probably allowed and continue with  *
                 *   waiting for the right time in the current slot (power consuming)   *
                 ************************************************************************/
                if (!ke_timer_sleep_check(&dummy, rwip_env.wakeup_delay)) {
                        break;
                }

                if (!ea_sleep_check(&dummy, rwip_env.wakeup_delay)) {
                        break;
                }

                /************************************************************************
                 **************           SET WUP_LATENCY (RCX)            **************
                 * The BLE_WUP_LATENCY is written to a local variable here. This is     *
                 * needed when the RCX is used, because the calculation of the latency  *
                 * comes from a function and is not a fixed number of LP cycles.        *
                 ************************************************************************/
                wup_latency = BLE_WUP_LATENCY;

                /************************************************************************
                 * Wait until there's enough time for the SLP ISR to restore clocks when
                 * the chip wakes up. Lower AMBA clocks while waiting, if possible, to
                 * reduce the power consumption.
                 *
                 * The "window" inside the BLE slot is calculated such that there is enough
                 * time for the SLP handler to program the clock compensation before the
                 * slot, that the system went to sleep, ends. More specifically,
                 *   window >= <clock restoration time> + <sleep preparation time> +
                 *            <ble core sleep entry time> + <SLP processing>
                 * where (assuming 16MHz is used):
                 *   <clock restoration time>: 0usec
                 *   <sleep period calc time>: 23 - 30usec (RCX) --- ~21usec (XTAL)
                 *   <sleep preparation time>: ~60 - 70usec (RCX) --- ~40 - 45usec (XTAL)
                 *   <ble core sleep entry time>: 2 to 4 LP cycles (61 - 122usec for 32768)
                 *   <SLP processing>: ~85usec (RCX) --- ~55 - 60usec (XTAL)
                 *
                 * So, when the 32768 is used, the "window" must be larger than:
                 *   0 + 61 +  61 + 55 = 177 (minimum)
                 *   0 + 66 + 122 + 60 = 248 (maximum).
                 * Thus, a window of [624, 300] is ok!
                 *
                 * When the RCX is used, the calculation becomes (assuming an RCX period
                 * of 95usec):
                 *   0 + 80 + 190 = 270 (minimum) (or, 80 + 2 * RCX_period)
                 *   0 + 100 + 380 = 480 (maximum) (or, 100 + 4 * RCX_period)
                 * The goal is to have completed the sleep period calculation within the current
                 * slot and put the BLE core to sleep at the next one.
                 *
                 * Thus, the window [250, 100] guarantees that there will be enough time
                 * for the SLP ISR by ensuring that the actual sleep entry will happen at
                 * the first half of the next slot. It is not yet clear that, although that
                 * the sleep duration calculations are executed in the current slot while
                 * the actual sleep entry happens at the next one, the calculated sleep
                 * duration does not have to be corrected (reduced by 1) before applied to the
                 * hardware!
                 *
                 * Note that 250 covers the case up to a ~15.5KHz RCX clock, which is too high.
                 * If the RCX clock is slower than 9.0Khz then the above calculation may fail. An
                 * assertion will hit in this case.
                 ************************************************************************/
                DBG_SET_HIGH(BLE_USE_TIMING_DEBUG, CPMDBG_BLE_SLEEP_ENTRY);
                if ((dg_configUSE_LP_CLK == LP_CLK_32000) || (dg_configUSE_LP_CLK == LP_CLK_32768)) {
                        if (!rwip_check_wakeup_boundary()) {
                                while (!rwip_check_wakeup_boundary()) {}
                        }
                } else if (dg_configUSE_LP_CLK == LP_CLK_RCX) {
                        ASSERT_WARNING(rcx_clock_hz >= 9000);

                        if (!rwip_check_wakeup_boundary_rcx()) {
                                while (!rwip_check_wakeup_boundary_rcx()) {}
                        }
                } else { /* LP_CLK_ANY */
                        if (!rwip_check_wakeup_boundary_any()) {
                                while (!rwip_check_wakeup_boundary_any()) {}
                        }
                }
                DBG_SET_LOW(BLE_USE_TIMING_DEBUG, CPMDBG_BLE_SLEEP_ENTRY);
                DBG_SET_HIGH(BLE_USE_TIMING_DEBUG, CPMDBG_BLE_SLEEP_ENTRY);

                // >>> Start of sleep entry delay measurement <<< //

                // Compute the duration up to the next software timer expires
                if (!ke_timer_sleep_check(sleep_duration, rwip_env.wakeup_delay)) {
                        break;
                }

                DBG_SWDIAG(SLEEP, ALGO, 3);

                /************************************************************************
                 **************                 CHECK EA                   **************
                 ************************************************************************/
                if (!ea_sleep_check(sleep_duration, rwip_env.wakeup_delay)) {
                        break;
                }

                DBG_SWDIAG(SLEEP, ALGO, 4);

#if (TL_ITF)
                /************************************************************************
                 **************                 CHECK TL                   **************
                 ************************************************************************/
                // Try to switch off TL
                if (!h4tl_stop()) {
                        break;
                }
#endif //TL_ITF

#if (GTL_ITF)
                /************************************************************************
                 **************                 CHECK TL                   **************
                 ************************************************************************/
                // Try to switch off Transport Layer
                if (!gtl_enter_sleep()) {
                        break;
                }
#endif //GTL_ITF

                DBG_SWDIAG(SLEEP, ALGO, 5);

                DBG_SET_LOW(BLE_USE_TIMING_DEBUG, CPMDBG_BLE_SLEEP_ENTRY);
                DBG_SET_HIGH(BLE_USE_TIMING_DEBUG, CPMDBG_BLE_SLEEP_ENTRY);

                // Sleep can be enabled

                if ((dg_configBLACK_ORCA_IC_REV == BLACK_ORCA_IC_REV_A)
                        || ((dg_configUSE_AUTO_CHIP_DETECTION == 1) && (CHIP_IS_AD || CHIP_IS_AE))) {
                        if (dg_configUSE_LP_CLK == LP_CLK_RCX) {
                                if (*sleep_duration != 0) {
                                        *sleep_duration = *sleep_duration - 1;
                                }
                        }
                }

                logged_sleep_duration = *sleep_duration;

                /************************************************************************
                 **************          PROGRAM CORE DEEP SLEEP           **************
                 ************************************************************************/
                // Prepare BLE_ENBPRESET_REG for next sleep cycle
                BLE->BLE_ENBPRESET_REG =  (wup_latency << 21)           /*BITFLD_TWEXT*/
                                        | (wup_latency << 10)           /*BITFLD_TWIRQ_SET*/
                                        | (1 << 0);                     /*BITFLD_TWIRQ_RESET*/

                // Put the BLE core into sleep
                if (*sleep_duration == (uint32_t) -1) {
                        // Sleep indefinitely (~36hours with 32KHz LP clock, ~113hours with 10.5Khz RCX)
                        sleep_duration_in_lp_cycles = -1;
                } else {
                        if ((dg_configUSE_LP_CLK == LP_CLK_32768) || (dg_configUSE_LP_CLK == LP_CLK_32000)) {
                                sleep_duration_in_lp_cycles = rwip_slot_2_lpcycles_xtal(*sleep_duration);
                        } else if (dg_configUSE_LP_CLK == LP_CLK_RCX) {
                                sleep_duration_in_lp_cycles = rwip_slot_2_lpcycles_rcx(*sleep_duration);
                        } else { /* LP_CLK_ANY */
                                sleep_duration_in_lp_cycles = rwip_slot_2_lpcycles_any(*sleep_duration);
                        }
                }
                lld_sleep_enter(sleep_duration_in_lp_cycles, rwip_env.ext_wakeup_enable);

                DBG_SWDIAG(SLEEP, SLEEP, 1);

                DBG_SET_LOW(BLE_USE_TIMING_DEBUG, CPMDBG_BLE_SLEEP_ENTRY);
                DBG_SET_HIGH(BLE_USE_TIMING_DEBUG, CPMDBG_BLE_SLEEP_ENTRY);

                /************************************************************************
                 **************               SWITCH OFF RF                **************
                 ************************************************************************/
                rwip_rf.sleep();
                // >>> End of sleep entry delay measurement <<< //

                DBG_SET_LOW(BLE_USE_TIMING_DEBUG, CPMDBG_BLE_SLEEP_ENTRY);
                DBG_SET_HIGH(BLE_USE_TIMING_DEBUG, CPMDBG_BLE_SLEEP_ENTRY);

                result = 1;

#if (dg_configIMAGE_SETUP == DEVELOPMENT_MODE)
                retained_slp_duration = sleep_duration_in_lp_cycles;
#endif
                *sleep_duration = sleep_duration_in_lp_cycles - wup_latency;

                while(!ble_deep_sleep_stat_getf());                             // 2 - 4 LP cycles
                DBG_SET_LOW(BLE_USE_TIMING_DEBUG, CPMDBG_BLE_SLEEP_ENTRY);

                if (*sleep_duration != 0) {
                        pm_resource_sleeps_until(PM_BLE_ID, *sleep_duration);
                }

                while ( !REG_GETF(BLE, BLE_CNTL2_REG, RADIO_PWRDN_ALLOW) ) {}   // 1 LP cycle
#endif // DEEP_SLEEP
        } while(0);

        return result;
}

/**
 * @brief       Check if the BLE stack has pending actions.
 *
 * @return      bool
 *
 * @retval      The status of the requested operation.
 *              <ul>
 *                  <li> false, if the BLE stack has pending actions
 *                  <li> true, if the BLE stack has finished
 *              </ul>
 *
 */
bool ble_block(void)
{
        bool result = false;

        /************************************************************************
         **************            CHECK KERNEL EVENTS             **************
         ************************************************************************/
        // Check if some kernel processing is ongoing
        if (ke_sleep_check()) {
                result = true;
        }

        return result;
}

/**
 * @brief Puts BLE to sleep
 *
 * @param[in] forever True to put the BLE to permanent sleep, else false.
 * @param[out] wakeup_time The sleep duration in LP cycles.
 *
 * @return      int
 *
 * @retval      The status of the requested operation.
 *              <ul>
 *                  <li> 0, if the BLE core cannot sleep
 *                  <li> 1, if the BLE core was put to sleep
 *                  <li> other, if the BLE core has to stay active but the caller may block
 *              </ul>
 *
 */
int ble_sleep(bool forever, uint32_t *sleep_duration_in_lp_cycles)
{
        int ret = 0;

        if (!rf_in_sleep) {
                ret = ble_rwip_sleep(forever, sleep_duration_in_lp_cycles);
                if (ret == 1) {
                        ble_regs_push();        // Push the BLE ret.vars to the retention memory

                        GLOBAL_INT_DISABLE();
                        REG_SET_BIT(CRG_TOP, PMU_CTRL_REG, BLE_SLEEP);
                        GLOBAL_INT_RESTORE();
                        while (!REG_GETF(CRG_TOP, SYS_STAT_REG, BLE_IS_DOWN));

                        GLOBAL_INT_DISABLE();
                        REG_CLR_BIT(CRG_TOP, CLK_RADIO_REG, BLE_ENABLE);
                        GLOBAL_INT_RESTORE();

                        // The BLE interrupts have been cleared. Clear them in the NVIC as well.
                        NVIC_ClearPendingIRQ(BLE_GEN_IRQn);

                        ret = true;
                } else {
                        // BLE stays active
                }
        } else {
                ASSERT_WARNING(!rf_in_sleep);   // The BLE is already sleeping...
        }

        return ret;
}

#if RWBLE_SW_VERSION_MAJOR >= 8
#if (dg_configUSE_CRYPTO == 1)
// The micro code of the EC controller
const uint32_t ba414e_ucode[] = {
    0x0400A, 0x20041, 0x2004D, 0x20059, 0x2002F, 0x20093, 0x20082, 0x20070,
    0x203F4, 0x20431, 0x2031A, 0x203DA, 0x20474, 0x204A3, 0x204E4, 0x20071,
    0x20082, 0x200A9, 0x200DF, 0x200EE, 0x200AE, 0x20135, 0x20087, 0x20136,
    0x2013A, 0x2013A, 0x20135, 0x20598, 0x20139, 0x20139, 0x20503, 0x20532,
    0x20372, 0x20387, 0x20393, 0x2057F, 0x20138, 0x20138, 0x20138, 0x20138,
    0x20139, 0x202ED, 0x20136, 0x2013B, 0x201A9, 0x201EE, 0x2024A, 0x1C480,
    0x14001, 0x11004, 0x11085, 0x11106, 0x00001, 0x1C090, 0x1C511, 0x00040,
    0x00001, 0x0C001, 0x00001, 0x1C292, 0x10012, 0x10092, 0x20050, 0x1C800,
    0x00001, 0x00002, 0x23039, 0x20031, 0x1E246, 0x04000, 0x1E24D, 0x04001,
    0x1CC00, 0x1F302, 0x026C6, 0x00001, 0x14001, 0x00002, 0x23039, 0x20031,
    0x1E246, 0x04000, 0x1E24D, 0x04001, 0x1CC00, 0x1F382, 0x026C6, 0x00001,
    0x14001, 0x00002, 0x23039, 0x20031, 0x10391, 0x1C580, 0x2A035, 0x1C29A,
    0x1EAC6, 0x0C380, 0x1EB47, 0x0C381, 0x1CC00, 0x1C800, 0x1EACB, 0x023C6,
    0x1EB4C, 0x023C6, 0x1FA82, 0x00D4B, 0x1EB1B, 0x00D4B, 0x00001, 0x14001,
    0x0C001, 0x00002, 0x23039, 0x20031, 0x10391, 0x1C580, 0x2A035, 0x1E113,
    0x0C380, 0x1E114, 0x0C381, 0x1E115, 0x00994, 0x1C292, 0x1F102, 0x00A92,
    0x00001, 0x14001, 0x00002, 0x1C29A, 0x1021A, 0x39059, 0x29071, 0x00002,
    0x20031, 0x10000, 0x1E246, 0x04001, 0x1E24D, 0x04000, 0x1CC00, 0x1F602,
    0x026C6, 0x00001, 0x14001, 0x00002, 0x23039, 0x1C580, 0x1E24D, 0x04005,
    0x1E246, 0x04004, 0x1CC00, 0x1F406, 0x026C6, 0x00001, 0x1CC00, 0x280AD,
    0x200A2, 0x14001, 0x10112, 0x2003B, 0x11006, 0x10092, 0x11106, 0x20044,
    0x00001, 0x00002, 0x1C292, 0x10212, 0x20093, 0x1415F, 0x00002, 0x1C292,
    0x10212, 0x0D005, 0x1C580, 0x1E24D, 0x00000, 0x1E246, 0x04004, 0x1CC00,
    0x1F406, 0x026C6, 0x1CC00, 0x280AD, 0x10015, 0x200FC, 0x0D000, 0x11005,
    0x10080, 0x10116, 0x20044, 0x1CC00, 0x240AD, 0x0C016, 0x11005, 0x11086,
    0x11106, 0x20050, 0x10000, 0x11086, 0x11106, 0x10391, 0x29075, 0x3905D,
    0x11004, 0x11086, 0x11106, 0x20044, 0x11206, 0x20095, 0x1C800, 0x0C000,
    0x200A2, 0x1CC00, 0x250DD, 0x00001, 0x14001, 0x1D286, 0x00001, 0x00002,
    0x10015, 0x200FC, 0x0C015, 0x1C292, 0x1C800, 0x0C092, 0x1FA86, 0x0C284,
    0x1EB1B, 0x0C284, 0x1C800, 0x0C000, 0x0C090, 0x14001, 0x00002, 0x10015,
    0x200FC, 0x0C015, 0x1C292, 0x1C800, 0x0C092, 0x1F106, 0x0C284, 0x1C800,
    0x0C000, 0x0C090, 0x00001, 0x14001, 0x1D280, 0x1C800, 0x1E246, 0x04000,
    0x1E24D, 0x04000, 0x1CC00, 0x1E393, 0x026C6, 0x1C800, 0x0C013, 0x1E246,
    0x00013, 0x1CC00, 0x1F380, 0x026C6, 0x1C800, 0x00001, 0x0C000, 0x0C090,
    0x10100, 0x14097, 0x14001, 0x00002, 0x1C580, 0x2A035, 0x10391, 0x1C292,
    0x10016, 0x1F100, 0x08392, 0x11008, 0x1F100, 0x0C384, 0x11080, 0x0810E,
    0x1F109, 0x0C001, 0x11089, 0x0A100, 0x1E880, 0x08392, 0x2B2EE, 0x1D185,
    0x08001, 0x08110, 0x1C900, 0x1C900, 0x1C900, 0x1C900, 0x1C980, 0x0A100,
    0x0A000, 0x1FA06, 0x00900, 0x00001, 0x14001, 0x14001, 0x14001, 0x14001,
    0x14001, 0x14001, 0x14001, 0x1C580, 0x2A035, 0x1C29A, 0x1E24A, 0x0001A,
    0x1CC00, 0x1E3CB, 0x0254A, 0x10014, 0x11084, 0x1FA80, 0x04881, 0x1FB08,
    0x04889, 0x10043, 0x10145, 0x1FA80, 0x04881, 0x1FB08, 0x0089A, 0x1FA82,
    0x0089A, 0x1FB0A, 0x008CB, 0x10014, 0x00100, 0x1D185, 0x1C800, 0x08001,
    0x08120, 0x3718A, 0x2718D, 0x20165, 0x1CB80, 0x1C800, 0x0A100, 0x0A000,
    0x10145, 0x11086, 0x1F401, 0x0C502, 0x14001, 0x1E34A, 0x0C481, 0x1E3CB,
    0x0C481, 0x1EACC, 0x0254A, 0x1EB4D, 0x025CB, 0x1E34E, 0x0C502, 0x1E3C7,
    0x0C502, 0x1EACE, 0x025CE, 0x1EB47, 0x023CA, 0x1E3CA, 0x026CC, 0x1FA81,
    0x026CC, 0x1EB4B, 0x06508, 0x1E34D, 0x023CE, 0x1E3CE, 0x023CE, 0x1FA82,
    0x026CD, 0x1EB47, 0x0274E, 0x1E34C, 0x025CC, 0x1FA8A, 0x08047, 0x1FB09,
    0x0264A, 0x00001, 0x100C3, 0x10145, 0x00001, 0x100C5, 0x10143, 0x00001,
    0x1EA94, 0x04884, 0x1EB15, 0x0488C, 0x1EAC7, 0x00A14, 0x1EB48, 0x00A95,
    0x1EACC, 0x0088A, 0x1EB4D, 0x02447, 0x1EACE, 0x00893, 0x1EB4C, 0x026CC,
    0x1E3C7, 0x023C8, 0x1E348, 0x0264E, 0x1CC00, 0x1E3C7, 0x02447, 0x11084,
    0x20313, 0x1C580, 0x2A035, 0x1C293, 0x23190, 0x1EA94, 0x00888, 0x1EB12,
    0x0088A, 0x1EACC, 0x00A14, 0x1EB4B, 0x00893, 0x1EACD, 0x02612, 0x1E3CC,
    0x025CC, 0x1E34D, 0x025CD, 0x1EACE, 0x026CD, 0x1CC00, 0x1EACE, 0x026CE,
    0x1CC00, 0x1EA95, 0x0274E, 0x1EA94, 0x02695, 0x1EA94, 0x02614, 0x1EB15,
    0x0274C, 0x1E24E, 0x00013, 0x1CC00, 0x1EA94, 0x02714, 0x1CC00, 0x00008,
    0x10214, 0x10284, 0x10314, 0x10391, 0x20117, 0x1C800, 0x00008, 0x1EA94,
    0x00894, 0x1EA95, 0x00A15, 0x1EACE, 0x00A95, 0x1CC00, 0x1EACE, 0x026CE,
    0x1CC00, 0x1E34E, 0x0264E, 0x1CC00, 0x1C800, 0x351EA, 0x1EA86, 0x00595,
    0x1CC00, 0x14001, 0x1EA86, 0x00995, 0x1CC00, 0x14001, 0x00002, 0x1C580,
    0x2A035, 0x1C293, 0x1EAA2, 0x00893, 0x11004, 0x100C3, 0x202B9, 0x10043,
    0x11090, 0x1014B, 0x11192, 0x2029A, 0x10043, 0x11090, 0x10116, 0x11192,
    0x202D5, 0x1004B, 0x11090, 0x1011A, 0x11192, 0x202D5, 0x11004, 0x100C3,
    0x202B9, 0x10043, 0x11090, 0x1014B, 0x11192, 0x2029A, 0x10043, 0x11090,
    0x10116, 0x11192, 0x1F200, 0x04002, 0x1F208, 0x0400A, 0x1F201, 0x04003,
    0x1F209, 0x0400B, 0x1004B, 0x11090, 0x10143, 0x11192, 0x2027C, 0x1004B,
    0x11090, 0x1011E, 0x11192, 0x202D5, 0x1004B, 0x11090, 0x1F380, 0x01122,
    0x1F208, 0x00022, 0x1F201, 0x00022, 0x1F389, 0x01122, 0x1004B, 0x11090,
    0x10112, 0x11192, 0x202D5, 0x1004B, 0x11090, 0x00100, 0x1D185, 0x1C800,
    0x08001, 0x08120, 0x1014B, 0x11192, 0x2029A, 0x2029A, 0x10900, 0x11192,
    0x2027C, 0x1CB80, 0x1C800, 0x0A100, 0x0A000, 0x1004B, 0x11086, 0x202E2,
    0x00001, 0x14001, 0x1C580, 0x2A035, 0x1C293, 0x1EAA2, 0x00893, 0x10202,
    0x10284, 0x1030A, 0x201F4, 0x10206, 0x10285, 0x1030C, 0x201F4, 0x10208,
    0x1028C, 0x1030E, 0x2026A, 0x1000A, 0x1008E, 0x10114, 0x20050, 0x1CC00,
    0x1C800, 0x3510E, 0x11008, 0x11089, 0x10114, 0x20050, 0x1CC00, 0x1C800,
    0x3510E, 0x14001, 0x11005, 0x100CB, 0x202B9, 0x1004B, 0x100CB, 0x202C4,
    0x11004, 0x100C3, 0x202B9, 0x10043, 0x11090, 0x1014B, 0x11192, 0x2027C,
    0x10043, 0x11086, 0x202E2, 0x00001, 0x1E347, 0x0C008, 0x1E3C8, 0x0C008,
    0x1EAC7, 0x08147, 0x1EB48, 0x08548, 0x1EAC9, 0x0C589, 0x1EB4A, 0x0C181,
    0x1F309, 0x02447, 0x1E3C7, 0x02447, 0x1E3C8, 0x024CA, 0x1E349, 0x024CA,
    0x1FA80, 0x02447, 0x1FB08, 0x06489, 0x1FA81, 0x024C8, 0x1FB09, 0x06389,
    0x1CC00, 0x00001, 0x1E347, 0x0C008, 0x1E3C8, 0x0C008, 0x1EACA, 0x023C7,
    0x1EB49, 0x02447, 0x1EAC7, 0x0C481, 0x1EB48, 0x0C081, 0x1E347, 0x023C7,
    0x1E348, 0x02448, 0x1CC00, 0x1E3C8, 0x024C8, 0x1E3CA, 0x023CA, 0x1FA82,
    0x02447, 0x1FB03, 0x024C8, 0x1FA8A, 0x02549, 0x1FB0B, 0x02547, 0x1CC00,
    0x00001, 0x1FA81, 0x04880, 0x1FB09, 0x04888, 0x11111, 0x1F202, 0x00022,
    0x1FA8A, 0x0C481, 0x1CC00, 0x00001, 0x1E347, 0x0C008, 0x1F389, 0x0C008,
    0x1E3C8, 0x023C7, 0x1F301, 0x02447, 0x11010, 0x11091, 0x1F301, 0x0C000,
    0x1CC00, 0x1FA89, 0x04088, 0x1CC00, 0x00001, 0x1FA8B, 0x04089, 0x1CC00,
    0x00008, 0x1F302, 0x0C008, 0x1F38A, 0x0C008, 0x1F303, 0x0C081, 0x1CC00,
    0x00008, 0x00001, 0x11110, 0x1F20A, 0x00022, 0x1E414, 0x0C10A, 0x1FA81,
    0x04A00, 0x1FB09, 0x04A08, 0x1CC00, 0x00001, 0x14001, 0x00001, 0x00001,
    0x1FA80, 0x04881, 0x1C800, 0x1FB08, 0x04889, 0x00001, 0x202F0, 0x1FA90,
    0x0089A, 0x1C800, 0x1EB46, 0x0089A, 0x00001, 0x1C580, 0x2A035, 0x1C29A,
    0x10084, 0x22307, 0x00010, 0x10084, 0x10012, 0x202F0, 0x00001, 0x1EAC6,
    0x04881, 0x1EB47, 0x00880, 0x1C800, 0x1E3CB, 0x02347, 0x1EAC6, 0x00D4B,
    0x1FB01, 0x00D4B, 0x00001, 0x00002, 0x1CC00, 0x1C800, 0x35319, 0x00001,
    0x14001, 0x14043, 0x00002, 0x235DA, 0x202FD, 0x2B2EF, 0x11084, 0x10043,
    0x202F6, 0x11084, 0x10014, 0x202F0, 0x10043, 0x100C3, 0x363FA, 0x2647A,
    0x10096, 0x363C7, 0x26468, 0x10016, 0x202F0, 0x10043, 0x100C3, 0x10114,
    0x3643D, 0x264AE, 0x10098, 0x363C7, 0x26468, 0x10018, 0x202F0, 0x00100,
    0x2B2EF, 0x1D185, 0x1C800, 0x08001, 0x08120, 0x2134B, 0x3135C, 0x1CB80,
    0x1C800, 0x0A100, 0x0A000, 0x3136B, 0x10043, 0x11086, 0x363C7, 0x26468,
    0x00001, 0x2036C, 0x14001, 0x10043, 0x100C3, 0x363FA, 0x2647A, 0x10043,
    0x100C3, 0x363FA, 0x2647A, 0x10043, 0x2735A, 0x100C3, 0x10900, 0x3643D,
    0x264AE, 0x00001, 0x1004C, 0x00001, 0x3735E, 0x00001, 0x10043, 0x10880,
    0x1F200, 0x04001, 0x1F208, 0x04009, 0x1FA90, 0x0089A, 0x1EB46, 0x0089A,
    0x2C2EF, 0x00020, 0x00001, 0x14047, 0x1C800, 0x00008, 0x200A2, 0x1130E,
    0x200A2, 0x00001, 0x00002, 0x00004, 0x11004, 0x10080, 0x1011B, 0x2337D,
    0x20381, 0x11008, 0x20381, 0x00001, 0x14001, 0x00002, 0x10081, 0x20381,
    0x14001, 0x20050, 0x1CC00, 0x1C800, 0x34386, 0x00001, 0x1400B, 0x00002,
    0x00004, 0x10001, 0x10080, 0x1011B, 0x20050, 0x1CC00, 0x1C800, 0x25392,
    0x00001, 0x14001, 0x1400F, 0x00002, 0x00004, 0x10004, 0x10080, 0x1011B,
    0x20050, 0x1CC00, 0x1C800, 0x343C5, 0x11008, 0x20050, 0x1CC00, 0x1C800,
    0x343C5, 0x263C2, 0x202FD, 0x10112, 0x1E246, 0x04002, 0x1E247, 0x0400A,
    0x1E34B, 0x02346, 0x1E34C, 0x023C7, 0x1EACB, 0x025CB, 0x1EB4D, 0x025CB,
    0x1E34C, 0x023CC, 0x1EACB, 0x0234B, 0x1EB4C, 0x0264C, 0x1C34E, 0x1E34D,
    0x0264C, 0x1E34B, 0x0264B, 0x1C34E, 0x1E34C, 0x026CB, 0x1CC00, 0x1C800,
    0x253C5, 0x203C2, 0x00002, 0x00001, 0x14001, 0x10004, 0x1401B, 0x1EACB,
    0x0C810, 0x1EB46, 0x0C810, 0x1CC00, 0x1C800, 0x2536B, 0x1EACB, 0x0C810,
    0x1EB47, 0x08846, 0x1CC00, 0x1C800, 0x2536B, 0x1F401, 0x06300, 0x1F409,
    0x06388, 0x00001, 0x00002, 0x202FD, 0x11084, 0x10043, 0x202F0, 0x10112,
    0x10043, 0x1E24C, 0x0400A, 0x1EACB, 0x0C000, 0x1EB46, 0x0C100, 0x1EACB,
    0x0804B, 0x1EB47, 0x0C408, 0x1E34B, 0x0234B, 0x1C34D, 0x1E34B, 0x025CC,
    0x1C34D, 0x1E3C6, 0x025C7, 0x20313, 0x00002, 0x202FD, 0x11084, 0x10043,
    0x202F6, 0x100C3, 0x1C800, 0x1E34D, 0x0C489, 0x1E346, 0x0C081, 0x1EACD,
    0x084CD, 0x1EB4E, 0x0C891, 0x1EAC6, 0x026C6, 0x1EB47, 0x026CD, 0x1EACB,
    0x0C081, 0x1EB4D, 0x0274E, 0x1E347, 0x023C7, 0x1E34C, 0x025CB, 0x1EACD,
    0x0094D, 0x1EB4E, 0x0094D, 0x1E34C, 0x0264B, 0x1E34E, 0x02346, 0x1E34B,
    0x026CC, 0x1E34D, 0x0C489, 0x1EACC, 0x025CB, 0x1FB08, 0x025CB, 0x1F380,
    0x0274C, 0x1C800, 0x1E3CE, 0x08046, 0x1C800, 0x1FA90, 0x088CD, 0x1EB4C,
    0x0274B, 0x1C800, 0x1F388, 0x023CC, 0x00001, 0x11086, 0x203C7, 0x2036C,
    0x14001, 0x00002, 0x2358E, 0x202FD, 0x11084, 0x10043, 0x202F6, 0x11085,
    0x10014, 0x202F0, 0x10043, 0x100C3, 0x10114, 0x1EACD, 0x0C891, 0x1EB47,
    0x0C88A, 0x1EAC6, 0x0814D, 0x1EB47, 0x026C7, 0x1E3CB, 0x080C6, 0x1E3CC,
    0x084C7, 0x1EAC6, 0x025CB, 0x1EB4E, 0x0264C, 0x1EAC7, 0x025C6, 0x1EB4D,
    0x080C6, 0x1F380, 0x023CE, 0x1E34E, 0x026CD, 0x1EAC6, 0x084C7, 0x1FB08,
    0x084C7, 0x1F380, 0x06700, 0x1C800, 0x1E3C7, 0x0804D, 0x1FA90, 0x088CB,
    0x1EB4D, 0x02647, 0x1C800, 0x1F388, 0x0234D, 0x00001, 0x11086, 0x2042E,
    0x1EACC, 0x0C810, 0x1EB46, 0x0C810, 0x1CC00, 0x1C800, 0x2536B, 0x1F401,
    0x0C800, 0x1F409, 0x06308, 0x00001, 0x00002, 0x202FD, 0x11084, 0x10043,
    0x202F6, 0x100C3, 0x1C800, 0x1EAC6, 0x0C891, 0x1EB47, 0x0C081, 0x1EACB,
    0x02346, 0x1FB10, 0x023C6, 0x1CC00, 0x1C800, 0x2536B, 0x1EAC6, 0x009CB,
    0x1EB47, 0x023C7, 0x1EACB, 0x0C489, 0x1EB4C, 0x04910, 0x1F300, 0x023C6,
    0x1E34B, 0x0234B, 0x1EAC6, 0x08846, 0x1EB4D, 0x08846, 0x1E347, 0x025CC,
    0x1C800, 0x1EACB, 0x08047, 0x1EB4D, 0x08047, 0x1F308, 0x025C6, 0x00001,
    0x11086, 0x20468, 0x14001, 0x00002, 0x202FD, 0x11084, 0x10043, 0x202F6,
    0x11085, 0x10014, 0x202F0, 0x10043, 0x100C3, 0x10114, 0x1EAC6, 0x0C891,
    0x1EB47, 0x0C882, 0x1EACB, 0x08546, 0x1EB46, 0x00946, 0x1E347, 0x080C7,
    0x1E34B, 0x084CB, 0x1EACC, 0x088C7, 0x1EB47, 0x023C7, 0x1E346, 0x02646,
    0x1C800, 0x1EAC6, 0x02347, 0x1FB10, 0x0264C, 0x1CC00, 0x1C800, 0x2536B,
    0x1EACC, 0x0264B, 0x1EB4B, 0x025CB, 0x1E346, 0x02646, 0x1EAC7, 0x0C110,
    0x1EB4D, 0x0C510, 0x1F300, 0x0234B, 0x1C800, 0x1E347, 0x08047, 0x1E34B,
    0x0804D, 0x1EAC6, 0x023CC, 0x1EB47, 0x0884B, 0x1C800, 0x1F308, 0x023C6,
    0x00001, 0x11086, 0x20468, 0x14001, 0x00002, 0x202FD, 0x11084, 0x10043,
    0x202F0, 0x10112, 0x10043, 0x1E24C, 0x0400A, 0x1EACB, 0x0C000, 0x1EB46,
    0x0C100, 0x1EACB, 0x0804B, 0x1EB46, 0x08046, 0x1EAC7, 0x0C408, 0x1EB4D,
    0x0C400, 0x1E34B, 0x0234B, 0x1E347, 0x026C7, 0x1E34B, 0x025CC, 0x1C34E,
    0x1E3C6, 0x025C7, 0x20313, 0x00002, 0x202FD, 0x10202, 0x10287, 0x1030E,
    0x2031E, 0x1C800, 0x00004, 0x0C001, 0x1C580, 0x1C092, 0x1C513, 0x0C092,
    0x1000E, 0x1009A, 0x1010A, 0x10393, 0x2005F, 0x1CC00, 0x2510E, 0x1021A,
    0x10287, 0x1030D, 0x20095, 0x1CC00, 0x10006, 0x1008A, 0x1010B, 0x10393,
    0x2005F, 0x1CC00, 0x00008, 0x1000C, 0x1008B, 0x1010B, 0x20044, 0x1CC00,
    0x00008, 0x1000D, 0x1008B, 0x1010B, 0x2005F, 0x1CC00, 0x2510E, 0x0C000,
    0x0C090, 0x14001, 0x00002, 0x235AA, 0x00004, 0x1C29A, 0x0C001, 0x1C580,
    0x1C092, 0x1C513, 0x0C092, 0x1021A, 0x1028B, 0x1030D, 0x20095, 0x1CC00,
    0x1000C, 0x1008D, 0x1011C, 0x10393, 0x2005F, 0x1CC00, 0x1000A, 0x1008D,
    0x1011B, 0x2005F, 0x1CC00, 0x00004, 0x0C000, 0x0C090, 0x202FD, 0x10208,
    0x1029B, 0x1031E, 0x2031E, 0x1C800, 0x10202, 0x1029C, 0x1031C, 0x2031E,
    0x1C800, 0x1009C, 0x10043, 0x202F6, 0x1009E, 0x10014, 0x202F0, 0x10043,
    0x100C3, 0x10114, 0x3643D, 0x264AE, 0x1008E, 0x363C7, 0x26468, 0x1C800,
    0x00004, 0x0C001, 0x1C580, 0x1C092, 0x1C513, 0x0C092, 0x1000E, 0x1009A,
    0x1011C, 0x10393, 0x2005F, 0x1CC00, 0x00008, 0x1001C, 0x1008A, 0x1011B,
    0x20050, 0x1CC00, 0x1C800, 0x3510E, 0x0C000, 0x0C090, 0x14001, 0x00002,
    0x00004, 0x20395, 0x20389, 0x10002, 0x10080, 0x1011B, 0x20378, 0x00004,
    0x10082, 0x10043, 0x202F0, 0x363DF, 0x264E9, 0x14001, 0x00002, 0x20433,
    0x11086, 0x10014, 0x202F0, 0x10043, 0x100C3, 0x2043D, 0x1008E, 0x2042E,
    0x00002, 0x0C001, 0x11005, 0x1008C, 0x1011B, 0x10391, 0x20075, 0x1C29A,
    0x11004, 0x1009A, 0x11104, 0x20075, 0x11004, 0x1009B, 0x11106, 0x33050,
    0x23044, 0x14001, 0x00002, 0x202FD, 0x10008, 0x10080, 0x1011B, 0x20378,
    0x10088, 0x10043, 0x202F0, 0x203DF, 0x1020E, 0x1028A, 0x1031C, 0x2031E,
    0x1C800, 0x10208, 0x1028C, 0x1031E, 0x2031E, 0x1C800, 0x1009E, 0x10043,
    0x202F6, 0x1009C, 0x10014, 0x202F0, 0x10043, 0x100C3, 0x10114, 0x2043D,
    0x10094, 0x203C7, 0x10314, 0x2036C, 0x10014, 0x10086, 0x1011B, 0x20050,
    0x1CC00, 0x1C800, 0x3510E, 0x11008, 0x11089, 0x20050, 0x1CC00, 0x1C800,
    0x3510E, 0x14001, 0x00002, 0x202FD, 0x2031E, 0x00008, 0x1001A, 0x1009A,
    0x1011B, 0x20050, 0x1001B, 0x1108E, 0x1110E, 0x20050, 0x1C800, 0x00008,
    0x1020E, 0x11286, 0x20434, 0x11086, 0x203C7, 0x11206, 0x1028D, 0x2031E,
    0x11086, 0x2042E
};

#define ECC_CODE_MEM 0x40030000
#define ECC_BASS_REG (127)
#define ECC_BASS_MEM (0x07FC0000 + ECC_BASS_REG * 1024)

typedef struct {
    volatile int ecc_busy; // Interrupt flag
    volatile unsigned int ecc_irq_counter;
    volatile unsigned int aes_irq_counter;
    volatile unsigned int aes_finish;
    volatile unsigned int aes_waitin;
} crypto_controller_t;

extern crypto_controller_t crypto_controller;
#endif

void crypto_init_func(void)
{
#if (dg_configUSE_CRYPTO == 1)
    // Base of the ECC code memory
    uint32_t *p_ecc_code_mem = (uint32_t *) ECC_CODE_MEM;
    
    GPREG->ECC_BASE_ADDR_REG = ECC_BASS_REG;
    
    // Initialize the ECC ucode
    for (uint32_t i = 0; i < sizeof(ba414e_ucode) / sizeof(uint32_t); i++)
        p_ecc_code_mem[i] = ba414e_ucode[i];
    
    crypto_controller.ecc_irq_counter = 0;
    
    NVIC_DisableIRQ(CRYPTO_IRQn);
    NVIC_ClearPendingIRQ(CRYPTO_IRQn);     
    NVIC_EnableIRQ(CRYPTO_IRQn);
#endif
}
#endif
