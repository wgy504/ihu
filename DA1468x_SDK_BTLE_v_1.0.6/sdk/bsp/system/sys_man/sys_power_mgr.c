/**
 ****************************************************************************************
 *
 * @file sys_power_mgr.c
 *
 * @brief Power Manager
 *
 * Copyright (C) 2015. Dialog Semiconductor Ltd, unpublished work. This computer
 * program includes Confidential, Proprietary Information and is a Trade Secret of
 * Dialog Semiconductor Ltd. All use, disclosure, and/or reproduction is prohibited
 * unless authorized in writing. All Rights Reserved.
 *
 * <black.orca.support@diasemi.com> and contributors.
 *
 ****************************************************************************************
 */

/**
 \addtogroup BSP
 \{
 \addtogroup SYSTEM
 \{
 \addtogroup Clock-Power_Manager
 \{
 */
#include <stdint.h>
#include <stdlib.h>
#include "sdk_defs.h"
#include "osal.h"
#include "hw_cpm.h"
#include "sys_tcs.h"
#include "sys_power_mgr.h"
#include "sys_clock_mgr.h"
#include "sys_charger.h"
#include "sys_rtc.h"
#include "sys_tcs.h"
#include "hw_watchdog.h"
#include "hw_timer1.h"
#include "hw_otpc.h"
#include "hw_wkup.h"
#include "hw_qspi.h"
#include "hw_gpio.h"
#include "hw_gpadc.h"
#include "hw_usb_charger.h"
#include "sys_watchdog.h"
#include "qspi_automode.h"

#if defined(CONFIG_USE_FTDF) || defined(CONFIG_USE_BLE)
#include "hw_rf.h"
#include "ad_rf.h"
#endif

#include "hw_dma.h"

#if (dg_configSYSTEMVIEW)
#  include "SEGGER_SYSVIEW_FreeRTOS.h"
#else
#  define SEGGER_SYSTEMVIEW_CPM_ISR_ENTER()
#  define SEGGER_SYSTEMVIEW_CPM_ISR_EXIT()
#endif

#if (CPM_DEBUG == 1)
#warning "Clock and Power Manager: Debugging mode is on!"
#endif

#if ((CPM_USE_FUNCTIONAL_DEBUG == 1) || (CPM_USE_TIMING_DEBUG == 1))
#warning "Clock and Power Manager: GPIO Debugging is on!"
#endif

#if (FLASH_DEBUG == 1)
#warning "Flash: Flash Debugging is on!"
#endif

#define OTP_QSPI_FLASH_UCODE_WAKEUP_ADDR        0x7F8F810
#define OTP_QSPI_FLASH_UCODE_WAKEUP_LEN         0x7F8F814

#define GUARD_TIME                              64      // in slots

#if (dg_configUSE_LP_CLK == LP_CLK_RCX)
#define MAX_TIMER_IDLE_COUNT                    (LP_CNT_MAX_VALUE + 1 - GUARD_TIME * 32)
#else
#define MAX_TIMER_IDLE_COUNT                    (LP_CNT_MAX_VALUE + 1 - GUARD_TIME * TICK_PERIOD)
#endif

#define MAX_IDLE_TIME_ALLOWED                   (MAX_TIMER_IDLE_COUNT * (1 + dg_configTim1Prescaler))

#if (dg_configUSE_LP_CLK == LP_CLK_RCX)
#define MAX_IDLE_TICKS_ALLOWED                  ((MAX_TIMER_IDLE_COUNT / 32) + 5)
#define MAX_TICKS_UNTIL_WRAP                    ((LP_CNT_MAX_VALUE + 1) / 32 - 1)
#else
#define MAX_IDLE_TICKS_ALLOWED                  ((MAX_TIMER_IDLE_COUNT / TICK_PERIOD) + 5)
#define MAX_TICKS_UNTIL_WRAP                    ((LP_CNT_MAX_VALUE + 1) / TICK_PERIOD - 1)
#endif

#define TICK_GUARD_PRESC_LIM                    2

extern uint32_t __mirrored_image_size;

/*
 * Imported functions
 */
#ifdef CONFIG_USE_BLE
void patch_rom_functions(void);
#endif

/*
 * Global and / or retained variables
 */

PRIVILEGED_DATA uint32_t lp_last_trigger;               // counts in prescaled LP cycles
PRIVILEGED_DATA static periph_init_cb periph_init;
PRIVILEGED_DATA static OS_MUTEX xSemaphorePM;
PRIVILEGED_DATA static system_state_t pm_system_sleeping;
PRIVILEGED_DATA uint64_t pm_mac_wakeup_time[PM_MAX_ID - PM_BASE_ID]; // 0: ID #0
                                                                     // 1: ID #1, etc.
                                                        // A value of zero is handled as "ignore".
PRIVILEGED_DATA static sleep_mode_t pm_current_sleep_mode;
PRIVILEGED_DATA static sleep_mode_t pm_user_sleep_mode;
PRIVILEGED_DATA static int8_t pm_sleep_block_cnt;
PRIVILEGED_DATA static bool pm_wakeup_mode_is_XTAL16;   // false: RC16, true: XTAL16M/PLL
PRIVILEGED_DATA uint16_t pm_wakeup_xtal16m_time;
PRIVILEGED_DATA adapter_call_backs_t *pm_adapters_cb[dg_configPM_MAX_ADAPTERS_CNT];

#if (dg_configDISABLE_BACKGROUND_FLASH_OPS == 0)
PRIVILEGED_DATA static qspi_ops *qspi_pending_ops;
#endif

#if (CPM_DEBUG == 1)
PRIVILEGED_DATA uint32_t low_power_periods_ret;
PRIVILEGED_DATA uint32_t sleep_period_ret;
PRIVILEGED_DATA uint32_t trigger_setting_ret;
PRIVILEGED_DATA uint32_t prev_trigger_setting_ret;
PRIVILEGED_DATA uint32_t trigger_hit_at_ret;
PRIVILEGED_DATA uint32_t lp_time1_ret;  // Power-up: start
PRIVILEGED_DATA uint32_t lp_time2_ret;  // Power-up: after the activation of the Power Domains
PRIVILEGED_DATA uint32_t lp_time3_ret;  // Power-up: after the clock setting - finish
#endif

uint32_t ulTimeSpentSleepingInTicks;                    // Counts in ticks

/*
 * Local variables
 */

static uint32_t sleep_blocked_until = 0;
static bool sleep_is_blocked = false;

/*
 * Forward declarations
 */

void xPortTickAdvance(void);

/*
 * \brief When resuming the OS, check if we were sleeping and calculate the time we slept.
 *
 * \return void
 *
 * \warning Must be called with interrupts disabled!
 *
 */
__RETAINED_CODE static void pm_sleep_exit(void);


/*
 * Function definitions
 */

/**
 * \brief Power down flash.
 *
 */
__RETAINED_CODE static void power_down_flash(void)
{
        hw_qspi_set_automode(false);                    // Set Manual Mode
        hw_qspi_set_bus_mode(HW_QSPI_BUS_MODE_SINGLE);  // Set SPI Mode - CS low

        hw_qspi_cs_enable();
        hw_qspi_write8(0xFF);                           // Exit from Fast Read mode
        hw_qspi_cs_disable();

        hw_qspi_cs_enable();                            // Power-down
        hw_qspi_write8(0xB9);
        hw_qspi_cs_disable();

        hw_qspi_set_bus_mode(HW_QSPI_BUS_MODE_QUAD);    // Set QUAD Mode - CS low
        hw_qspi_set_automode(true);                     // Set Auto Mode
}

/*
 * \brief Set-up the system for the chosen sleep mode.
 *
 * \return void
 *
 */
static void prepare_sleep_mode(void)
{
        if ((pm_current_sleep_mode == pm_mode_extended_sleep)
                        || (pm_current_sleep_mode == pm_mode_deep_sleep)) {
                hw_cpm_setup_retmem();                          // Set retmem mode for image copy

                // OTP
                if (((dg_configIMAGE_SETUP == DEVELOPMENT_MODE) && (dg_configEMULATE_OTP_COPY == 1))
                                || ((dg_configCODE_LOCATION == NON_VOLATILE_IS_OTP)
                                                && (dg_configEXEC_MODE == MODE_IS_MIRRORED))) {
                        /*
                         * Prepare OTP copy.
                         */
                        hw_otpc_init();                 // Activate OTPC

                        hw_otpc_prepare((uint32_t)&__mirrored_image_size);      // Set OTP copy size

                        hw_otpc_close();                // Deactivate OTPC
                }

                // QSPI
                if (((dg_configIMAGE_SETUP == DEVELOPMENT_MODE) && (dg_configEMULATE_QSPI_COPY == 1))
                                || ((dg_configCODE_LOCATION == NON_VOLATILE_IS_FLASH)
                                                && (dg_configEXEC_MODE == MODE_IS_MIRRORED))) {
                        ASSERT_WARNING(0);      // QSPI mirorred is not implemented!
                }
        } else {
                hw_cpm_setup_retmem_no_img_copy();              // Set retmem mode for no image copy

                // OTP
                if (((dg_configIMAGE_SETUP == DEVELOPMENT_MODE) && (dg_configEMULATE_OTP_COPY == 1))
                                || ((dg_configCODE_LOCATION == NON_VOLATILE_IS_OTP)
                                                && (dg_configEXEC_MODE == MODE_IS_MIRRORED))) {
                        /*
                         * Cancel OTP copy.
                         */
                        hw_otpc_init();                 // Activate OTPC

                        hw_otpc_cancel_prepare();       // Cancel copy

                        hw_otpc_close();                // Deactivate OTPC
                }
        }
}

static void prepare_qfis_code(void)
{
#if (dg_configFLASH_POWER_OFF == 1)
        dg_ucodeFLASH_POWER_OFF;
#elif (dg_configFLASH_POWER_DOWN == 1)
        dg_ucodeFLASH_POWER_DOWN;
#else
        dg_ucodeFLASH_ALWAYS_ACTIVE;
#endif
        volatile uint32_t *p;
        int i;

        p = (volatile uint32_t *)&(QSPIC->QSPIC_UCODE_START);

        for (i = 0; i < sizeof(ucode_wakeup)/sizeof(uint32_t); i++) {
                *p = ucode_wakeup[i];
                p++;
                /* Max 16 ucodes are allowed on wakeup */
                ASSERT_WARNING(i < 16);
        }

        /* zero out trailing words */
        for ( ; i < 16; i++, p++ ) {
                *p = 0;
        }

        /* Must have reached the end of ucodes area here */
        ASSERT_WARNING(p == &(QSPIC->QSPIC_UCODE_START) + 16);
}

void pm_system_init(periph_init_cb peripherals_initialization)
{
        uint32_t rtc;

        if (dg_configUSE_WDOG == 0) {
                hw_watchdog_freeze();                   // Stop watchdog
        }

        if (dg_configPOWER_CONFIG == POWER_CONFIGURATION_1) {
                // Give power to 3V3 (GPIOs) during sleep
                hw_cpm_3v3_clamp_on();

                // Disable VBAT_ret since 3V3 is provided by the Clamp
                hw_cpm_ldo_vbat_ret_off();
        }

        if (dg_configCONFIG_VIA_OTP_HEADER) {
        }

        /* Time to copy the image must be less than the minimum sleep time */
        ASSERT_WARNING(dg_configIMAGE_COPY_TIME < dg_configMIN_SLEEP_TIME);

        periph_init = peripherals_initialization;

        if ((sys_tcs_xtal16m_settling_time == 0) || (dg_configUSE_LP_CLK == LP_CLK_RCX)) {
                pm_wakeup_xtal16m_time = dg_configWAKEUP_XTAL16_TIME;
        } else {
                pm_wakeup_xtal16m_time = sys_tcs_xtal16m_settling_time + dg_configWAKEUP_RC16_TIME;
        }

        hw_cpm_setup_retmem();                          // Set retmem mode

        if ((dg_configIMAGE_SETUP == DEVELOPMENT_MODE) && (dg_configEMULATE_OTP_COPY == 1)) {
                GLOBAL_INT_DISABLE();
                REG_SET_BIT(CRG_TOP, SYS_CTRL_REG, DEV_PHASE);  // Emulate OTP copy
                GLOBAL_INT_RESTORE();
        }

        if ((dg_configCODE_LOCATION == NON_VOLATILE_IS_FLASH) || (dg_configPOWER_FLASH == 1)) {
                if (dg_configCODE_LOCATION == NON_VOLATILE_IS_FLASH) {
                        hw_otpc_disable();              // Make sure OTPC is in STBY and off
                }

                /* Prepare QFIS code */
                prepare_qfis_code();
        } else if ((dg_configCODE_LOCATION != NON_VOLATILE_IS_OTP)
                        || ((dg_configCODE_LOCATION == NON_VOLATILE_IS_OTP)
                                        && (dg_configEXEC_MODE == MODE_IS_MIRRORED))) {
                hw_otpc_disable();                      // Make sure OTPC is off
        }

        if ((dg_configCODE_LOCATION != NON_VOLATILE_IS_NONE) && (dg_configEXEC_MODE == MODE_IS_CACHED)) {
                hw_cpm_set_cache_retained();            // Set cache in retained mode
        }

        hw_cpm_power_up_per_pd();                       // Exit peripheral power down

        if (dg_configUSE_ADC) {
                hw_gpadc_init(NULL);
                hw_gpadc_set_ldo_delay(0x0B);           // RC16 is the ADC clock (DIVN is 1:1).
                hw_gpadc_enable();
                hw_gpadc_set_ldo_constant_current(true);
                hw_gpadc_set_ldo_dynamic_current(true);
                hw_gpadc_set_input_mode(HW_GPADC_INPUT_MODE_SINGLE_ENDED);
                hw_gpadc_set_input(HW_GPADC_INPUT_SE_VDD);

                rtc = rtc_get();                        // 22 usec delay is required!
        }

        if (dg_configUSE_SW_CURSOR == 1) {
                hw_cpm_setup_sw_cursor();
        }

        if (dg_configCODE_LOCATION != NON_VOLATILE_IS_FLASH) {
                if (dg_configUSE_ProDK == 1) {          // Power Down Flash when not used
                        GPIO->P00_MODE_REG = 0x26;
                        GPIO->P01_MODE_REG = 0x26;
                        GPIO->P02_MODE_REG = 0x26;
                        GPIO->P03_MODE_REG = 0x26;
                        GPIO->P04_MODE_REG = 0x26;
                        GPIO->P05_MODE_REG = 0x26;

                        hw_qspi_init(NULL);
                        hw_qspi_set_div(HW_QSPI_DIV_1);
                        power_down_flash();
                }

                hw_qspi_disable_clock();                // Stop QSPI clock if not QSPI Flash
        }

        hw_cpm_dcdc_config();                           // Configure the DCDC

        if (dg_configUSE_USB == 1) {
                hw_charger_configure_usb_pins();        // Configure USB pads.
                usb_charger_init();                     // Start charger
        }

#if defined(CONFIG_USE_FTDF) || defined(CONFIG_USE_BLE)
        hw_rf_poweron();

        /* Make sure that the XTAL16 is enabled and settled before starting the initial calibration
         * and that the system clock is either XTAL16 or PLL. */
        while(!cm_poll_xtal16m_ready());
        ASSERT_WARNING(
                hw_cpm_get_sysclk() == SYS_CLK_IS_XTAL16M ||
                hw_cpm_get_sysclk() == SYS_CLK_IS_PLL);

        ad_rf_system_init();
        hw_rf_poweroff();

        ad_rf_init();
#endif

        if (periph_init != NULL) {
                periph_init();                          // Power on peripherals and GPIOs
        }

        sys_tcs_apply(tcs_system);

        DBG_CONFIGURE_LOW(EXCEPTION_DEBUG, EXCEPTIONDBG);
        DBG_CONFIGURE_LOW(CPM_USE_FUNCTIONAL_DEBUG, CPMDBG_TICK);
        DBG_CONFIGURE_LOW(CPM_USE_FUNCTIONAL_DEBUG, CPMDBG_EXT_WKUP);
        DBG_CONFIGURE_HIGH(CPM_USE_FUNCTIONAL_DEBUG, CPMDBG_POWERUP);
        DBG_CONFIGURE_HIGH(CPM_USE_TIMING_DEBUG, CPMDBG_SLEEP_EXIT);
        DBG_CONFIGURE_LOW(CPM_USE_TIMING_DEBUG, CPMDBG_LOWER_CLOCKS);
        DBG_CONFIGURE_LOW(FLASH_DEBUG, FLASHDBG_PAGE_PROG);
        DBG_CONFIGURE_LOW(FLASH_DEBUG, FLASHDBG_PAGE_PROG_WL);
        DBG_CONFIGURE_LOW(FLASH_DEBUG, FLASHDBG_PAGE_PROG_WL_IRQ);
        DBG_CONFIGURE_LOW(FLASH_DEBUG, FLASHDBG_SUSPEND);
        DBG_CONFIGURE_LOW(FLASH_DEBUG, FLASHDBG_SECTOR_ERASE);
        DBG_CONFIGURE_LOW(FLASH_DEBUG, FLASHDBG_TASK_NOTIFY);
        DBG_CONFIGURE_LOW(FLASH_DEBUG, FLASHDBG_SUSPEND_ACTION);
        DBG_CONFIGURE_LOW(FLASH_DEBUG, FLASHDBG_RESUME);

        hw_cpm_deactivate_pad_latches();                // Enable pads

        xSemaphorePM = xSemaphoreCreateMutex();         // Create Mutex
        ASSERT_WARNING(xSemaphorePM != NULL);

        if (dg_configUSE_ADC) {
                while ((rtc + 2) > rtc_get()) {
                        // 2 LP clocks are max ~60usec (XTAL32K) or 200usec (RCX)!
                }
                hw_gpadc_calibrate();
        }

        if (dg_configUSE_WDOG == 1) {
                hw_watchdog_unfreeze();                 // Start watchdog
        }
}

void pm_wait_debugger_detach(sleep_mode_t mode)
{
        volatile bool loop = true;
        hw_watchdog_freeze();                           // Stop watchdog

        // Make sure that the debugger is detached else sleep won't be possible - Skipped for FPGA!
        if (configUSE_TICKLESS_IDLE != 0) {
                if (mode != pm_mode_active) {
                        while (loop && REG_GETF(CRG_TOP, SYS_STAT_REG, DBG_IS_ACTIVE)) {
                        }
                }
        }

        if (dg_configUSE_WDOG == 1) {
                hw_watchdog_unfreeze();                 // Start watchdog
        }
}

void pm_set_wakeup_mode(bool wait_for_xtal16m)
{
        ASSERT_WARNING(xSemaphorePM != NULL);

        OS_EVENT_WAIT(xSemaphorePM, OS_EVENT_FOREVER);    // Block forever

        pm_wakeup_mode_is_XTAL16 = wait_for_xtal16m;

        OS_EVENT_SIGNAL(xSemaphorePM);
}

bool pm_get_wakeup_mode(void)
{
        bool mode;

        ASSERT_WARNING(xSemaphorePM != NULL);

        OS_EVENT_WAIT(xSemaphorePM, OS_EVENT_FOREVER);    // Block forever

        mode = pm_wakeup_mode_is_XTAL16;

        OS_EVENT_SIGNAL(xSemaphorePM);

        return mode;
}

sleep_mode_t pm_set_sleep_mode(sleep_mode_t mode)
{
        sleep_mode_t prev_mode;

        GLOBAL_INT_DISABLE();

        if (pm_sleep_block_cnt == 0) {
                // Change mode immediately
                prev_mode = pm_current_sleep_mode;
                pm_current_sleep_mode = mode;
        } else {
                // defer until pm_resume_sleep() is called
                prev_mode = pm_user_sleep_mode;
                pm_user_sleep_mode = mode;
        }

        prepare_sleep_mode();

        GLOBAL_INT_RESTORE();

        return prev_mode;
}

sleep_mode_t pm_get_sleep_mode(void)
{
        sleep_mode_t mode;

        GLOBAL_INT_DISABLE();

        mode = pm_current_sleep_mode;

        GLOBAL_INT_RESTORE();

        return mode;
}

void pm_stay_alive(void)
{
        GLOBAL_INT_DISABLE();

        if (pm_sleep_block_cnt == 0) {
                pm_user_sleep_mode = pm_current_sleep_mode;
        }
        pm_current_sleep_mode = pm_mode_active;
        pm_sleep_block_cnt++;

        /* Max reference counter value is 126 */
        ASSERT_WARNING(pm_sleep_block_cnt < 127);

        GLOBAL_INT_RESTORE();
}

void pm_stay_idle(void)
{
        GLOBAL_INT_DISABLE();

        if (pm_sleep_block_cnt == 0) {
                pm_user_sleep_mode = pm_current_sleep_mode;
        }
        pm_current_sleep_mode = pm_mode_idle;
        pm_sleep_block_cnt++;

        /* Max reference counter value is 126 */
        ASSERT_WARNING(pm_sleep_block_cnt < 127);

        GLOBAL_INT_RESTORE();
}

void pm_resume_sleep(void)
{
        GLOBAL_INT_DISABLE();

        /* pm_resume_sleep() must be called after pm_stay_idle/_alive(), and
         * for the same amount of times */
        ASSERT_ERROR(pm_sleep_block_cnt > 0);
        pm_sleep_block_cnt--;
        if (pm_sleep_block_cnt == 0) {
                pm_current_sleep_mode = pm_user_sleep_mode;
                prepare_sleep_mode();
        }

        GLOBAL_INT_RESTORE();
}

pm_id_t pm_register_adapter(const adapter_call_backs_t *cb)
{
        pm_id_t ret = -1;
        int i = 0;

        ASSERT_WARNING(xSemaphorePM != NULL);
        ASSERT_WARNING(cb != NULL);

        OS_EVENT_WAIT(xSemaphorePM, OS_EVENT_FOREVER);    // Block forever

        while ((i < dg_configPM_MAX_ADAPTERS_CNT) && (pm_adapters_cb[i] != NULL)) {
                i++;
        }

        if (i < dg_configPM_MAX_ADAPTERS_CNT) {
                pm_adapters_cb[i] = (adapter_call_backs_t *)cb;
                ret = i;
        }

        ASSERT_WARNING(ret != -1);                      // Increase dg_configPM_MAX_ADAPTERS_CNT

        OS_EVENT_SIGNAL(xSemaphorePM);

        return ret;
}

void pm_unregister_adapter(pm_id_t id)
{
        ASSERT_WARNING(xSemaphorePM != NULL);

        OS_EVENT_WAIT(xSemaphorePM, OS_EVENT_FOREVER);    // Block forever

        ASSERT_WARNING((id >= 0) && (id < dg_configPM_MAX_ADAPTERS_CNT));     // Is id valid?
        ASSERT_WARNING(pm_adapters_cb[id] != NULL);     // Is it registered?

        pm_adapters_cb[id] = NULL;

        OS_EVENT_SIGNAL(xSemaphorePM);
}

void pm_resource_sleeps_until(pm_id_t id, uint32_t time_in_LP_cycles)
{
        uint32_t rtc_time;
        uint64_t lp_event_time;

#if   defined(CONFIG_USE_BLE)
        ASSERT_WARNING(id == PM_BLE_ID);
#endif

        /*
         * Update Real Time Clock value
         */
        rtc_time = rtc_get();

        /*
         * Calculate wake-up time taking into account possible overflows
         */
        lp_event_time = (uint64_t)rtc_time + time_in_LP_cycles - pm_wakeup_xtal16m_time;
        if (lp_event_time == 0) {
                lp_event_time = -1;     // The value 0 is reserved (undefined timeout value).
        }
        pm_mac_wakeup_time[id - PM_BASE_ID] = lp_event_time;
}

void pm_resource_is_awake(pm_id_t id)
{
#if   defined(CONFIG_USE_BLE)
        ASSERT_WARNING(id == PM_BLE_ID);
#endif

        pm_mac_wakeup_time[id - PM_BASE_ID] = 0; // Set to undefined (or infinite or ignored).
}

void pm_defer_sleep_for(pm_id_t id, uint32_t time_in_LP_cycles)
{
        uint32_t rtc_time;
        uint32_t lp_block_time;

#ifdef CONFIG_USE_BLE
        ASSERT_WARNING(id != PM_BLE_ID);        // Is id valid?
#endif


        ASSERT_WARNING((id >= 0) && (id < dg_configPM_MAX_ADAPTERS_CNT));
        ASSERT_WARNING(pm_adapters_cb[id] != NULL);                     // Is it registered?
        ASSERT_WARNING(time_in_LP_cycles <= dg_configPM_MAX_ADAPTER_DEFER_TIME);

        /*
         * Update Real Time Clock value
         */
        rtc_time = rtc_get_fromISR();

        lp_block_time = rtc_time + time_in_LP_cycles;

        if (sleep_is_blocked == false) {
                sleep_blocked_until = lp_block_time;
                sleep_is_blocked = true;
        } else {
                // Update only if the new block time is after the previous one
                if (sleep_blocked_until < lp_block_time) {
                        sleep_blocked_until = lp_block_time;
                }
        }
}


uint64_t pm_get_mac_wakeup_time(pm_id_t id)
{
#if   defined(CONFIG_USE_BLE)
        ASSERT_WARNING(id == PM_BLE_ID);
#endif

        return pm_mac_wakeup_time[id - PM_BASE_ID] + pm_wakeup_xtal16m_time;
}

/**
 * \brief Initialize the system after wake-up.
 *
 * \return void
 *
 * \warning Called in ISR context after the settling of the XTAL.
 *
 */
static void pm_init_wake_up(void)
{
        int i;
        uint32_t rtc;
        uint32_t iser;
        adapter_call_backs_t *p_Ad;

        if ((CPM_DEBUG == 0) && hw_cpm_check_per_pd_status()) {
                ASSERT_WARNING(0);      // Should not be here! Peripheral PD should be down!
        }

        hw_cpm_power_up_per_pd();                       // Exit peripheral power down

        if (dg_configUSE_ADC) {
                hw_gpadc_init(NULL);
                hw_gpadc_set_ldo_delay(0x0B);           // RC16 is the ADC clock (DIVN is 1:1).
                hw_gpadc_enable();
                hw_gpadc_set_ldo_constant_current(true);
                hw_gpadc_set_ldo_dynamic_current(true);
                hw_gpadc_set_input_mode(HW_GPADC_INPUT_MODE_SINGLE_ENDED);
                hw_gpadc_set_input(HW_GPADC_INPUT_SE_VDD);

                rtc = rtc_get_fromISR();                // 22 usec delay is required!
        }

        if (dg_configUSE_SW_CURSOR == 1) {
                hw_cpm_setup_sw_cursor();
        }

        if (dg_configLP_CLK_SOURCE == LP_CLK_IS_DIGITAL) {
                hw_cpm_configure_ext32k_pins();
        } else if ((dg_configUSE_LP_CLK == LP_CLK_32000)
                        || (dg_configUSE_LP_CLK == LP_CLK_32768)) {
                hw_cpm_configure_xtal32k_pins();
        }

        GLOBAL_INT_DISABLE();

        if (pm_wakeup_mode_is_XTAL16) {
                /* Mask interrupts again, since the Adapters or the periph_init() may call
                 * NVIC_EnableIRQ(). This is to ensure that no code other than the CPM code will be
                 * executed until the XTAL16M has settled, as the wake-up mode defines.
                 */
                iser = NVIC->ISER[0];                   // Log interrupt enable status
        } else {
                iser = 0;
        }
        
        // Inform Adapters
        for (i = 0; i < dg_configPM_MAX_ADAPTERS_CNT; i++) {
                p_Ad = pm_adapters_cb[i];
                if ((p_Ad != NULL) && (p_Ad->ad_wake_up_ind != NULL)) {
                        p_Ad->ad_wake_up_ind(false);
                }
        }

        if (dg_configUSE_USB == 1) {
                hw_charger_configure_usb_pins();        // Configure USB pads.
        }

        if (periph_init != NULL) {
                periph_init();                          // Power on peripherals and GPIOs
        }

        if (pm_wakeup_mode_is_XTAL16) {
                /* Disable all interrupts except for the ones that were active when this function
                 * was entered. No adapter should activate an interrupt that is not normally active
                 * in the ad_wake_up_ind() call-back. All interrupts will be properly restored to
                 * the state they were before the system entered into the sleep mode.
                 */
                NVIC->ICER[0] = ~iser;
                NVIC->ISER[0] = iser;
        }

        GLOBAL_INT_RESTORE();

        sys_tcs_apply(tcs_system);

        DBG_CONFIGURE_HIGH(CPM_USE_FUNCTIONAL_DEBUG, CPMDBG_TICK);
        DBG_CONFIGURE_LOW(CPM_USE_FUNCTIONAL_DEBUG, CPMDBG_EXT_WKUP);
        DBG_CONFIGURE_LOW(CPM_USE_FUNCTIONAL_DEBUG, CPMDBG_POWERUP);
        DBG_CONFIGURE_LOW(EXCEPTION_DEBUG, EXCEPTIONDBG);

        // A user definable macro that allows application code to be added.
        configPOST_SLEEP_PROCESSING();

        hw_cpm_deactivate_pad_latches();                // enable pads

        if (dg_configUSE_ADC) {
                while ((rtc + 2) > rtc_get_fromISR()) {
                        // 2 LP clocks are max ~60usec (XTAL32K) or 200usec (RCX)!
                }
                hw_gpadc_calibrate();
        }
}

#if (CPM_DEBUG == 1)
__RETAINED uint32_t rt_elapsed_time;
__RETAINED uint32_t rt_elapsed_ticks;

struct cpm_trigger_mon_st {
        uint16_t type;  //0: lp_last_trigger, 1: trigger
        uint16_t value;
};

#define MAX_TRG_MON_SZ          64      // Must be a power of 2

__RETAINED struct cpm_trigger_mon_st rt_trigger_mon[MAX_TRG_MON_SZ];
__RETAINED uint32_t rt_trigger_mon_wr;
#endif

__RETAINED_CODE static uint32_t pm_advance_time_compute(uint32_t prescaled_time, uint32_t *trigger)
{
        uint32_t elapsed_time;
        uint32_t elapsed_ticks;
        uint32_t tick_offset;
        uint32_t test_val;

        hw_timer1_int_disable();                        // Disable interrupt

        elapsed_time = (prescaled_time - lp_last_trigger) & LP_CNT_PRESCALED_MASK;

        /* When in Idle mode, the elapsed time period cannot be equal to LP_CNT_PRESCALED_MASK!
         * If this happens then it is a "fake" Timer1 interrupt caused by the 1 LP cycle propagation
         * delay of the internal interrupt of Timer1. This interrupt must be ignored!!! */
        if (pm_system_sleeping != sys_powered_down) {
                if (elapsed_time >= (LP_CNT_PRESCALED_MASK - 1)) {
                        *trigger = (lp_last_trigger + TICK_PERIOD) & LP_CNT_PRESCALED_MASK;
                        return 0;
                }
        }

        elapsed_ticks = elapsed_time / TICK_PERIOD;
        tick_offset = elapsed_time - (elapsed_ticks * TICK_PERIOD);       // Offset in current tick (in prescaled LP cycles).

#if (CPM_DEBUG == 1)
        rt_elapsed_time = elapsed_time;
        rt_elapsed_ticks = elapsed_ticks;
#endif

        /* We know that we can be within 1 tick period "earlier" than the system time since we may
         * have to intentionally advance the system time by 1 tick during the trigger programming.
         */
        if (elapsed_ticks == MAX_TICKS_UNTIL_WRAP) {
                return 0;
        }

        /* No more that MAX_IDLE_TICKS_ALLOWED ticks should have passed */
        ASSERT_WARNING(elapsed_ticks < MAX_IDLE_TICKS_ALLOWED);

        // Compute next trigger.
        *trigger = lp_last_trigger + (elapsed_ticks + 1) * TICK_PERIOD;
        if (TICK_GUARD_PRESC_LIM >=
                        (TICK_PERIOD - tick_offset)) {  // Too close in time
                *trigger += TICK_PERIOD;                // Set trigger at the next tick
                elapsed_ticks++;                        // Report 1 more tick spent sleeping
        }
        *trigger &= LP_CNT_PRESCALED_MASK;              // Make sure it's within limits...

        lp_last_trigger = (*trigger - TICK_PERIOD) & LP_CNT_PRESCALED_MASK;

#if (CPM_DEBUG == 1)
        rt_trigger_mon[rt_trigger_mon_wr].type = 0;
        rt_trigger_mon[rt_trigger_mon_wr].value = lp_last_trigger;
        rt_trigger_mon_wr++;
        rt_trigger_mon_wr %= MAX_TRG_MON_SZ;
#endif

        if ((dg_configUSE_LP_CLK == LP_CLK_32000) || (dg_configUSE_LP_CLK == LP_CLK_32768)) {
                test_val = lp_last_trigger + 1;
                /* test_val must be an exact multiple of TICK_PERIOD */
                ASSERT_WARNING((test_val % TICK_PERIOD) == 0);
        }
        return elapsed_ticks;
}

__RETAINED_CODE static void pm_advance_time_apply(uint32_t trigger)
{
        uint32_t dummy __attribute__((unused));
        uint32_t timer1_current_value;
        uint32_t diff;

        timer1_current_value = (hw_timer1_get_value() + 1) & LP_CNT_PRESCALED_MASK;
        diff = (timer1_current_value - trigger) & LP_CNT_PRESCALED_MASK;

        while (diff <= LP_CNT_PRESCALED_MASK/2) {
                trigger += TICK_PERIOD;                 // Set trigger at the next tick
                trigger &= LP_CNT_PRESCALED_MASK;       // Make sure it's within limits...

                timer1_current_value = (hw_timer1_get_value() + 1) & LP_CNT_PRESCALED_MASK;
                diff = (timer1_current_value - trigger) & LP_CNT_PRESCALED_MASK;
        }

        HW_TIMER1_SET_TRIGGER(trigger, dummy);          // Set reload value
        hw_timer1_int_enable();                         // Enable interrupt

#if (CPM_DEBUG == 1)
        rt_trigger_mon[rt_trigger_mon_wr].type = 1;
        rt_trigger_mon[rt_trigger_mon_wr].value = trigger;
        rt_trigger_mon_wr++;
        rt_trigger_mon_wr %= MAX_TRG_MON_SZ;
#endif
}

__RETAINED_CODE uint32_t pm_advance_time(uint32_t prescaled_time)
{
        uint32_t elapsed_ticks;
        uint32_t trigger;

        elapsed_ticks = pm_advance_time_compute(prescaled_time, &trigger);
        pm_advance_time_apply(trigger);

        return elapsed_ticks;
}

/**
 * \brief Calculate how many ticks have passed since the time the system entered sleep or idle mode.
 *
 * \return void
 *
 */
__RETAINED_CODE static void pm_calc_slept_time(void)
{
        uint32_t rtc_time __attribute__((unused));
        uint32_t lp_prescaled_time;
        uint32_t trigger;
        uint32_t dummy;

        /*
         * Update Real Time Clock value and calculate the time spent sleeping.
         * lp_prescaled_time - lp_last_trigger : sleep time in prescaled lp cycles
         * lp_previous_time : lp timer offset at sleep entry
         */
        rtc_time = rtc_get_fromCPM(&lp_prescaled_time, &dummy);
#if (CPM_DEBUG == 1)
        trigger_hit_at_ret = lp_prescaled_time;
#endif

        // Calculate time spent sleeping in ticks and the offset in this tick period. 
        ulTimeSpentSleepingInTicks = pm_advance_time_compute(lp_prescaled_time, &trigger);

        // Advance time
        if (ulTimeSpentSleepingInTicks > 0) {
                xTaskIncrementTick();
                vTaskStepTick( ulTimeSpentSleepingInTicks - 1 );
        }

        pm_advance_time_apply(trigger);
        pm_system_sleeping = sys_active;
}

/**
 * \brief Initialize system after a wake-up.
 *
 * \return void
 *
 */
static void pm_wakeup(uint32_t trimmed)
{
        uint32_t rtc_time __attribute__((unused));
        uint32_t dummy __attribute__((unused));

        if (dg_configUSE_WDOG == 0) {
                hw_watchdog_freeze();                   // Stop watchdog
        }

#if (CPM_DEBUG == 1)
        rtc_time = rtc_get_fromCPM(&dummy, &lp_time1_ret);
#endif

        /*
         * Init System Power Domain blocks: GPIO, WD Timer, Sys Timer, etc.
         * Power up and init Peripheral Power Domain blocks, and release the pad latches.
         */
        pm_init_wake_up();

#if (CPM_DEBUG == 1)
        rtc_time = rtc_get_fromCPM(&dummy, &lp_time2_ret);
#endif

        /*
         * Wait for XTAL16, if pm_wakeup_mode_is_XTAL16 is true
         */
        if (pm_wakeup_mode_is_XTAL16) {
                if (trimmed) {
                        /*
                         * Did not enter sleep due to a pending interrupt or the debugger being
                         * attached! Simulate wake-up...
                         */
                        cm_sys_xtal16m_running();
                } else {
                        cm_check_xtal_startup();
                        cm_halt_until_xtal16m_ready();
                }
        } else {
                /* Restore AHB, APB. If sys_clk is not RC16, it will be restored when the XTAL16
                 * settles.
                 */
                cm_sys_clk_sleep(false);

                if (trimmed) {
                        /*
                         * Did not enter sleep due to a pending interrupt or the debugger being
                         * attached! Simulate wake-up...
                         */
                        cm_sys_xtal16m_running();
                }
        }

#if (CPM_DEBUG == 1)
        rtc_time = rtc_get_fromCPM(&dummy, &lp_time3_ret);
#endif

        if (dg_configUSE_WDOG == 1) {
                hw_watchdog_unfreeze();                 // Start watchdog
        }
}

/**
 * \brief Interrupt handler of WKUPCT
 *
 * \return void
 *
 */
void WKUP_GPIO_Handler(void)
{
        SEGGER_SYSTEMVIEW_CPM_ISR_ENTER();

        uint32_t ulPreviousMask;

        ulPreviousMask = portSET_INTERRUPT_MASK_FROM_ISR();

        hw_wkup_handler();

        DBG_SET_HIGH(CPM_USE_FUNCTIONAL_DEBUG, CPMDBG_EXT_WKUP);

        portCLEAR_INTERRUPT_MASK_FROM_ISR(ulPreviousMask);

        SEGGER_SYSTEMVIEW_CPM_ISR_EXIT();
}

/**
 * \brief Interrupt handler of Timer1
 *
 * \return void
 *
 */
__RETAINED_CODE void SWTIM1_Handler(void)
{
        SEGGER_SYSTEMVIEW_CPM_ISR_ENTER();

        uint32_t ulPreviousMask;

        ulPreviousMask = portSET_INTERRUPT_MASK_FROM_ISR();

        if (pm_system_sleeping == sys_active) {
                uint32_t rtc_time __attribute__((unused));
                uint32_t lp_prescaled_time, unused;
                uint32_t elapsed_ticks;
                uint32_t test_val;

                DBG_SET_HIGH(CPM_USE_FUNCTIONAL_DEBUG, CPMDBG_TICK);

                /*
                 * Update Real Time Clock value and calculate the time spent sleeping
                 */
                rtc_time = rtc_get_fromCPM(&lp_prescaled_time, &unused);

                if ((dg_configUSE_LP_CLK == LP_CLK_32000) || (dg_configUSE_LP_CLK == LP_CLK_32768)) {
                        test_val = lp_last_trigger + 1;
                        ASSERT_WARNING((test_val % TICK_PERIOD) == 0);
                }

                elapsed_ticks = pm_advance_time(lp_prescaled_time);

                while (elapsed_ticks) {
                        xPortTickAdvance();
                        elapsed_ticks--;
                }

                DBG_SET_LOW(CPM_USE_FUNCTIONAL_DEBUG, CPMDBG_TICK);
        }

        portCLEAR_INTERRUPT_MASK_FROM_ISR(ulPreviousMask);

        SEGGER_SYSTEMVIEW_CPM_ISR_EXIT();
}

__RETAINED_CODE void pm_system_wake_up(void)
{
        /*
         * Check if it is a wake-up.
         */
        if (pm_system_sleeping == sys_powered_down) {
                uint32_t iser;
                uint32_t trimmed = false;
#if (dg_configIMAGE_SETUP == DEVELOPMENT_MODE)
                uint32_t primask;
#endif

                /* Disable all IRQs except for Timer1 and XTAL16RDY. Other ISRs may call OS routines
                 * that require the system timing to have been restored. Thus, their execution must
                 * be deferred until the call to the vTaskStepTick().
                 */
                iser = NVIC->ISER[0];                   // Log interrupt enable status

                // Disable all interrupts except for the ones of Timer1 and XTAL16M
                NVIC->ICER[0] = iser & ~((uint32_t)(1 << SWTIM1_IRQn) | (uint32_t)(1 << XTAL16RDY_IRQn));

                // Clear RCX calibration flag
                if (dg_configUSE_LP_CLK == LP_CLK_RCX) {
                        cm_rcx_calibration_is_on = false;
                }

                trimmed = !hw_cpm_check_sleep_flag();

#if (dg_configIMAGE_SETUP == DEVELOPMENT_MODE)
                /* If the code stops at this point then the interrupts were enabled while they
                 * shouldn't be so.
                 */
                primask = __get_PRIMASK();
                ASSERT_WARNING(primask == 1);
#endif

                /*
                 * Clear the "XTAL16 Ready" flag in the Event Group of the Clock Manager. It will be
                 * set again to 1 when the XTAL16M has settled.
                 * Note: this translates to a message in a queue that unblocks the Timer task in order to
                 * service it. This will be done when the OS scheduler is resumed. Even if the
                 * XTAL16RDY_IRQn hits while still in this function (pm_wakeup_mode_is_XTAL16 is true), this
                 * will result to a second message being added to the same queue. When the OS scheduler is
                 * resumed, the first task that will be executed is the Timer task. This will first process
                 * the first message of the queue (clear Event Group bits) and then the second (set Event
                 * Group bits), which is the desired operation.
                 */
                if (!trimmed) {
                        cm_sys_clk_wakeup();
                }

                /* Exit the critical section. */
                taskENABLE_INTERRUPTS();

                pm_wakeup(trimmed);

                DBG_SET_LOW(CPM_USE_FUNCTIONAL_DEBUG, CPMDBG_TICK);

                /* Determine how long the microcontroller was actually in a low power
                 * state. This time will be less than xExpectedIdleTime if the
                 * microcontroller was brought out of low power mode by an interrupt
                 * other than the sleep timer interrupt.
                 *
                 * >>> This information is provided by the Power Manager. <<<
                 *
                 * Note that the scheduler is suspended before
                 * portSUPPRESS_TICKS_AND_SLEEP() is called, and resumed when
                 * portSUPPRESS_TICKS_AND_SLEEP() returns.  Therefore no other tasks will
                 * execute until this function completes.
                 */

                /* Correct the kernel's tick count to account for the time the
                 * microcontroller spent in its low power state.
                 */
                pm_sleep_exit();

                DBG_SET_LOW(CPM_USE_TIMING_DEBUG, CPMDBG_SLEEP_EXIT);

                /* Restore all interrupts */
                NVIC->ISER[0] = iser;
        } else {
                /* Correct the kernel's tick count to account for the time the
                 * microcontroller spent in its low power state.
                 */
                pm_sleep_exit();

                DBG_SET_LOW(CPM_USE_TIMING_DEBUG, CPMDBG_SLEEP_EXIT);

                pm_system_sleeping = sys_active;

                taskENABLE_INTERRUPTS();
        }
}

void pm_sleep_exit(void)
{
        if (pm_system_sleeping != sys_active) {
                /*
                 * Calculate how long we've been idle or sleeping.
                 */
                pm_calc_slept_time();

                /*
                 * If the RCX is used and we've been idle for too long then force a
                 * re-calibration of the RCX to make sure that we keep track of the clock's changes.
                 */
                if (dg_configUSE_LP_CLK == LP_CLK_RCX) {
                        if (pm_system_sleeping == sys_idle) {
                                GLOBAL_INT_DISABLE();
                                if ((ulTimeSpentSleepingInTicks > 250) /*&& cm_lp_clk_is_avail()*/ && !cm_rcx_calibration_is_on) {
                                        cm_calibrate_rcx_start();
                                }
                                GLOBAL_INT_RESTORE();
                        }
                }
        }
}

__RETAINED_CODE static void apply_wfi(bool allow_entering_sleep, uint32_t sleep_period)
{
#if (dg_configIMAGE_SETUP == DEVELOPMENT_MODE)
        uint32_t primask;
#endif
        bool low_vbat;

        // If an interrupt is already pending, do not sleep.
        if (NVIC->ISER[0] & NVIC->ISPR[0]) {
                DBG_SET_LOW(CPM_USE_TIMING_DEBUG, CPMDBG_SLEEP_ENTER);
                DBG_SET_HIGH(CPM_USE_TIMING_DEBUG, CPMDBG_SLEEP_EXIT);
                return;
        }

        if (allow_entering_sleep) {
                DBG_SET_LOW(CPM_USE_FUNCTIONAL_DEBUG, CPMDBG_EXT_WKUP);

                /*
                 * Hibernation check
                 */
                if (pm_current_sleep_mode == pm_mode_hibernation) {
                        sleep_period = 0;
                }

                /*
                 * Voltage underflow check.
                 */
                if ((dg_configBATTERY_LOW_LEVEL != 0) && (!hw_charger_check_vbus())) {
                        low_vbat = usb_charger_is_battery_low();
                } else {
                        low_vbat = false;
                }

                if (low_vbat) {
                        sleep_period = 0;
                        pm_current_sleep_mode = pm_mode_hibernation;    // Force hibernation
                }

                /*
                 * Hooks
                 */
                if (sleep_period == 0) {
                        // A user definable macro that allows application code to be added.
                        configPRE_STOP_PROCESSING();
                } else {
                        // A user definable macro that allows application code to be added.
                        configPRE_SLEEP_PROCESSING( sleep_period );
                }

                /*
                 * Mark that a wake-up interrupt will be treated as such and not as a
                 * typical interrupt.
                 */
                pm_system_sleeping = sys_powered_down;

                DBG_SET_LOW(CPM_USE_FUNCTIONAL_DEBUG, CPMDBG_POWERUP);

                /*
                 * Make sure that an XTAL16RDY_IRQn will hit the next time we wake up.
                 */
                if ((sys_tcs_xtal16m_settling_time == 0) || (dg_configUSE_LP_CLK == LP_CLK_RCX)) {
                        hw_cpm_set_xtal16m_settling_time(dg_configXTAL16_SETTLE_TIME);
                } else {
                        hw_cpm_set_xtal16m_settling_time(sys_tcs_xtal16m_settling_time);
                }

                if ((pm_current_sleep_mode >= pm_mode_deep_sleep_no_mirror)
                                && (sleep_period == 0)) {
                        // Deep Sleep mode! Wake-up only from external button!

                        if (((dg_configLOW_VBAT_HANDLING == 0) && low_vbat)
                                        || (dg_configFORCE_DEEP_SLEEP == 1)) {
                                hw_cpm_enable_clockless();      // Stop RC32 on sleep entry
                        }

                        hw_timer1_disable();            // Stop OS timer

                        if (dg_configPOWER_CONFIG == POWER_CONFIGURATION_1) {
                                // Switch to RC32K and turn-off the LP clock used so far
                                hw_cpm_lp_set_rc32k();

                                if (dg_configLP_CLK_SOURCE == LP_CLK_IS_DIGITAL) {
                                        ;               // Do nothing
                                } else if ((dg_configUSE_LP_CLK == LP_CLK_32000)
                                                || (dg_configUSE_LP_CLK == LP_CLK_32768)) {
                                        hw_cpm_disable_xtal32k();
                                } else if (dg_configUSE_LP_CLK == LP_CLK_RCX) {
                                        hw_cpm_disable_rcx();
                                }

                                // Disable Sampling / Recharging (needed to stop LP clock).
                                hw_cpm_reset_recharge_period();
                        }

                        if (pm_current_sleep_mode == pm_mode_hibernation) {
                                hw_cpm_no_retmem();
                                hw_cpm_enable_reset_on_wup();

                                if (dg_configPOWER_CONFIG != POWER_CONFIGURATION_1) {
                                        // Give power to 3V3 (GPIOs) during sleep.
                                        hw_cpm_3v3_clamp_on();

                                        // Disable VBAT_ret since 3V3 is provided by the Clamp.
                                        hw_cpm_ldo_vbat_ret_off();
                                }

                                hw_cpm_lp_set_rc32k();

                                if (dg_configLP_CLK_SOURCE == LP_CLK_IS_DIGITAL) {
                                        ;               // Do nothing
                                } else if ((dg_configUSE_LP_CLK == LP_CLK_32000)
                                                || (dg_configUSE_LP_CLK == LP_CLK_32768)) {
                                        hw_cpm_disable_xtal32k();
                                } else if (dg_configUSE_LP_CLK == LP_CLK_RCX) {
                                        hw_cpm_disable_rcx();
                                }

                                // Disable Sampling / Recharging (needed to stop LP clock).
                                hw_cpm_reset_recharge_period();

                                if (dg_configLOW_VBAT_HANDLING == 1) {
                                        /*
                                         * Disable all IRQs except for VBUS and WKUP (if used by the
                                         * application).
                                         */

                                        uint32_t iser;

                                        iser = NVIC->ISER[0];
                                        NVIC->ICER[0] = iser & ~((uint32_t)(1 << VBUS_IRQn) | (uint32_t)(1 << WKUP_GPIO_IRQn));

                                        /*
                                         * Clear all interrupts except for VBUS and WKUP (if used
                                         * by the application) so that the system is not reset by
                                         * an already pending interrupt.
                                         */
                                        NVIC->ICPR[0] = iser & ~((uint32_t)(1 << VBUS_IRQn) | (uint32_t)(1 << WKUP_GPIO_IRQn));
                                } else {
                                        hw_cpm_enable_clockless();      // Stop RC32 on sleep entry
                                }
                        } else if (dg_configFORCE_DEEP_SLEEP == 1) {
                                if (dg_configPOWER_CONFIG != POWER_CONFIGURATION_1) {
                                        // Switch to RC32K and turn-off the LP clock used so far
                                        hw_cpm_lp_set_rc32k();

                                        if (dg_configLP_CLK_SOURCE == LP_CLK_IS_DIGITAL) {
                                                ;               // Do nothing
                                        } else if ((dg_configUSE_LP_CLK == LP_CLK_32000)
                                                        || (dg_configUSE_LP_CLK == LP_CLK_32768)) {
                                                hw_cpm_disable_xtal32k();
                                        } else if (dg_configUSE_LP_CLK == LP_CLK_RCX) {
                                                hw_cpm_disable_rcx();
                                        }

                                        // Disable Sampling / Recharging (needed to stop LP clock).
                                        hw_cpm_reset_recharge_period();

                                        // Give power to 3V3 (GPIOs) during sleep
                                        hw_cpm_3v3_clamp_on();

                                        // Disable VBAT_ret since 3V3 is provided by the Clamp
                                        hw_cpm_ldo_vbat_ret_off();

                                        // BOD is not available in clockless mode!
                                }

                                if (dg_configLP_CLK_SOURCE == LP_CLK_IS_DIGITAL) {
                                        /* The external LP clock will be available at wake-up.
                                         * Do nothing.
                                         */
                                        ;
                                } else if ((dg_configUSE_LP_CLK == LP_CLK_32000)
                                                || (dg_configUSE_LP_CLK == LP_CLK_32768)) {
                                        /*
                                         * Set the proper settling time since RC32K will be used
                                         * until the XTAL32K is settled, which happens much later
                                         * than the XTAL16M settling.
                                         */
                                        hw_cpm_set_xtal16m_settling_time(dg_configXTAL16_SETTLE_TIME_RC32K);
                                }
                        }

                        // Note: BOD is disabled in all clockless modes!
                }

                /*
                 * Turn-off System Power Domain upon sleep entry
                 */
                SCB->SCR |= (1 << 2);

                /*
                 * Switch to 16MHz system clock. AHB/APB dividers are set to 1.
                 */
                cm_sys_clk_sleep(true);
                hw_cpm_pll_sys_off();

                /*
                 * Put the Flash in Power-Down.
                 */
                if ((dg_configCODE_LOCATION == NON_VOLATILE_IS_FLASH) && (dg_configFLASH_POWER_DOWN == 1)) {
                        power_down_flash();
                }

                /*
                 * Turn-off Peripheral Power Domain
                 */
                hw_cpm_activate_pad_latches();
                hw_cpm_power_down_periph_pd();

                hw_cpm_wait_per_power_down();
                hw_cpm_wait_rad_power_down();

                /*
                 * Stop the clock to the RF unit.
                 */
                hw_cpm_rfcu_clk_off();

                /*
                 * Enable OSC16M amplitude regulation
                 */
                hw_cpm_enable_osc16m_amp_reg();

                /*
                 * Make sure that the RC16 will be stopped by the HW FSM.
                 */
                hw_cpm_disable_rc16();

                /*
                 * Set a flag to detect if sleep indeed took place.
                 */
                hw_cpm_set_sleep_flag();

                /*
                 * Prepare the DCDC for sleep.
                 */
                if (dg_configUSE_DCDC == 1) {
                        if ((pm_current_sleep_mode == pm_mode_hibernation)
                                || ((dg_configFORCE_DEEP_SLEEP == 1)
                                        && (pm_current_sleep_mode >= pm_mode_deep_sleep_no_mirror)
                                        && (sleep_period == 0)) ) {
                                hw_cpm_dcdc_off();

                                /* Turn off 1V8 and 1V8P since the system will
                                 * resume either by recharging the battery (where a
                                 * reset/reboot will be issued on the occurrence of
                                 * VBUS_IRQn) or by replacing the battery.
                                 */
                                hw_cpm_ldo_io_ret_off();
                        } else if (dg_configPOWER_CONFIG != POWER_CONFIGURATION_3) {
                                hw_cpm_dcdc_off();
                        } else {
                                if (!hw_cpm_dcdc_is_active()) {
                                        hw_cpm_dcdc_on();
                                }
                                hw_cpm_dcdc_sleep();
                        }
                } else {
                        if ((pm_current_sleep_mode == pm_mode_hibernation)
                                || ((dg_configFORCE_DEEP_SLEEP == 1)
                                        && (pm_current_sleep_mode >= pm_mode_deep_sleep_no_mirror)
                                        && (sleep_period == 0)) ) {
                                /* Turn off 1V8 and 1V8P since the system will
                                 * resume either by recharging the battery (where a
                                 * reset/reboot will be issued on the occurrence of
                                 * VBUS_IRQn) or by replacing the battery.
                                 */
                                hw_cpm_ldo_io_ret_off();
                        }
                }

                /*
                 * Conditionally place the system into permanent sleep to allow for sleep current
                 * measurements.
                 */
                if (dg_configTESTMODE_MEASURE_SLEEP_CURRENT == 1) {
                        uint32_t iser;

                        iser = NVIC->ISER[0];                   // Log interrupt enable status
                        NVIC->ICER[0] = iser;                   // Disable all interrupts
                        NVIC->ICPR[0] = iser;                   // Clear all interrupts

                        uint32_t pmu_ctrl_reg = CRG_TOP->PMU_CTRL_REG;
                        REG_SET_FIELD(CRG_TOP, PMU_CTRL_REG, RETAIN_RAM, pmu_ctrl_reg, dg_configTESTMODE_RETAIN_RAM);
                        REG_SET_FIELD(CRG_TOP, PMU_CTRL_REG, RETAIN_CACHE, pmu_ctrl_reg, dg_configTESTMODE_RETAIN_CACHE);
                        REG_SET_FIELD(CRG_TOP, PMU_CTRL_REG, RETAIN_ECCRAM, pmu_ctrl_reg, dg_config_TESTMODE_RETAIN_ECCRAM);
                        CRG_TOP->PMU_CTRL_REG = pmu_ctrl_reg;

                        __WFI();
                }
        } else {
                DBG_SET_HIGH(CPM_USE_TIMING_DEBUG, CPMDBG_LOWER_CLOCKS);

                if (!hw_dma_channel_active()) {
                        // Lower the clocks to reduce power consumption.
                        cm_lower_all_clocks();
                }

                /*
                 * Hook to add any application specific code before entering Idle state.
                 */
                configPRE_IDLE_ENTRY( sleep_period );
        }

        DBG_CONFIGURE_LOW(CPM_USE_TIMING_DEBUG, CPMDBG_SLEEP_ENTER);

#if (dg_configIMAGE_SETUP == DEVELOPMENT_MODE)
        /* If the code stops at this point then the interrupts were enabled while they
         * shouldn't be so.
         */
        primask = __get_PRIMASK();
        ASSERT_WARNING(primask == 1);
#endif

        /*
         * Disable watchdog when the debugger is attached or when deep sleep is prohibited
         * and the only monitored task is the IDLE task. In both cases System PD would
         * remain active causing the watchdog to hit.
         */
        if (dg_configUSE_WDOG == 1) {
                if (hw_cpm_is_debugger_attached() || (!allow_entering_sleep && sys_watchdog_monitor_mask_empty())) {
                        hw_watchdog_freeze();
                }
        }

        /*
         * Sleep
         */
        __WFI();

        /* Make sure that the code will be executed at the fastest possible speed. */
        if (!allow_entering_sleep) {
                hw_cpm_set_hclk_div(ahb_div1);
        }

        /*
         *  Restart watchdog. If WFI() was a sleep WFI() then this is done automatically by the hardware.
         *  If it was an idle WFI() then we have to do it manually. Here we do it in all cases for simplicity.
         */
        if (dg_configUSE_WDOG == 1) {
                hw_watchdog_unfreeze();
        }

        // Make sure that Timer1 is not programmed to hit within the next few usec.
        hw_timer1_invalidate_trigger();

        if (!allow_entering_sleep) {
                // Restore clocks that may have been lowered before entering IDLE.
                cm_restore_all_clocks();

                DBG_SET_LOW(CPM_USE_TIMING_DEBUG, CPMDBG_LOWER_CLOCKS);
        }

        DBG_SET_HIGH(CPM_USE_TIMING_DEBUG, CPMDBG_SLEEP_EXIT);

        if (pm_system_sleeping == sys_powered_down) {
                DBG_CONFIGURE_HIGH(CPM_USE_TIMING_DEBUG, CPMDBG_SLEEP_EXIT);
                DBG_CONFIGURE_LOW(CPM_USE_TIMING_DEBUG, CPMDBG_LOWER_CLOCKS);
                DBG_CONFIGURE_LOW(CPM_USE_TIMING_DEBUG, CPMDBG_SLEEP_ENTER);
                DBG_CONFIGURE_LOW(CPM_USE_TIMING_DEBUG, CPMDBG_XTAL16M_SETTLED);
                DBG_CONFIGURE_LOW(BLE_USE_TIMING_DEBUG, CPMDBG_BLE_IRQ);
                DBG_CONFIGURE_LOW(BLE_USE_TIMING_DEBUG, CPMDBG_BLE_SLEEP_ENTRY);
                DBG_CONFIGURE_LOW(USB_CHARGER_TIMING_DEBUG, CHRGDBG_CRITICAL_SECTION);
                DBG_CONFIGURE_LOW(USB_CHARGER_TIMING_DEBUG, CHRGDBG_FSM_TASK);
                DBG_CONFIGURE_LOW(USB_CHARGER_TIMING_DEBUG, CPMDBG_CONTROL_TASK);
                DBG_CONFIGURE(BLE_RX_EN_DEBUG, BLEBDG_RXEN, BLE_RX_EN_FUNC);
                DBG_CONFIGURE_LOW(FLASH_DEBUG, FLASHDBG_PAGE_PROG);
                DBG_CONFIGURE_LOW(FLASH_DEBUG, FLASHDBG_PAGE_PROG_WL);
                DBG_CONFIGURE_LOW(FLASH_DEBUG, FLASHDBG_PAGE_PROG_WL_IRQ);
                DBG_CONFIGURE_LOW(FLASH_DEBUG, FLASHDBG_SUSPEND);
                DBG_CONFIGURE_LOW(FLASH_DEBUG, FLASHDBG_SECTOR_ERASE);
                DBG_CONFIGURE_LOW(FLASH_DEBUG, FLASHDBG_TASK_NOTIFY);
                DBG_CONFIGURE_LOW(FLASH_DEBUG, FLASHDBG_SUSPEND_ACTION);
                DBG_CONFIGURE_LOW(FLASH_DEBUG, FLASHDBG_RESUME);

                if (BLE_USE_TIMING_DEBUG == 1) {
                        CPMDBG_BLE_LP_IRQ_MODE_REG = 0x31C;
                }

                if (CPM_USE_TIMING_DEBUG == 1) {
                        if (dg_configLP_CLK_SOURCE == LP_CLK_IS_DIGITAL) {
                                hw_cpm_configure_ext32k_pins();
                        } else if ((dg_configUSE_LP_CLK == LP_CLK_32000)
                                        || (dg_configUSE_LP_CLK == LP_CLK_32768)) {
                                hw_cpm_configure_xtal32k_pins();
                        }
                        hw_cpm_power_up_per_pd();
                        hw_cpm_deactivate_pad_latches();
                }
        }

        /*
         * Start the DCDC.
         */
        if (pm_system_sleeping == sys_powered_down) {
                /*
                 * Take the the Flash out of Power-Down in case the system did not go to sleep due
                 * to a pending interrupt (so the uCode wasn't executed).
                 */
                if (((dg_configCODE_LOCATION == NON_VOLATILE_IS_FLASH) || (dg_configPOWER_FLASH == 1))
                                && (dg_configFLASH_POWER_DOWN == 1)) {
                        dg_configFLASH_POWER_DOWN_C_CODE;
                }

                if (((dg_configPOWER_CONFIG == POWER_CONFIGURATION_1)
                        || (dg_configFORCE_DEEP_SLEEP == 1))
                                && (pm_current_sleep_mode >= pm_mode_deep_sleep_no_mirror)
                                && (sleep_period == 0)) {
                        // Return to normal operation mode
                        hw_cpm_3v3_clamp_off();
                        hw_cpm_ldo_vbat_ret_on();
                        hw_cpm_start_ldos();
                        hw_cpm_disable_clockless();

                        if (dg_configLP_CLK_SOURCE == LP_CLK_IS_DIGITAL) {
                                // Restore Sampling / Recharging timer value.
                                hw_cpm_set_recharge_period((uint16_t)dg_configSET_RECHARGE_PERIOD);

                                // Set it as the LP clock
                                hw_cpm_lp_set_ext32k();
                        } else if ((dg_configUSE_LP_CLK == LP_CLK_32000)
                                        || (dg_configUSE_LP_CLK == LP_CLK_32768)) {
                                // Enable XTAL32K
                                hw_cpm_enable_xtal32k();

                                // Restore Sampling / Recharging timer value.
                                hw_cpm_set_recharge_period((uint16_t)dg_configSET_RECHARGE_PERIOD);

                                // Block sleep until XTAL32K is settled
                                cm_lp_clk_timer_start();
                                cm_lp_clk_wakeup();
                        } else if (dg_configUSE_LP_CLK == LP_CLK_RCX) {
                                // Enable RCX
                                hw_cpm_enable_rcx();

                                // Restore Sampling / Recharging timer value.
                                hw_cpm_set_recharge_period(cm_rcx_us_2_lpcycles_low_acc(dg_configSET_RECHARGE_PERIOD));

                                // Set it as the LP clock
                                hw_cpm_lp_set_rcx();
                        }
                }

                if (dg_configUSE_DCDC == 1) {
                        hw_cpm_dcdc_on();
                }

                if (dg_configUSE_WDOG == 1) {
                        hw_watchdog_set_pos_val(dg_configWDOG_RESET_VALUE);
                } else {
                        hw_watchdog_freeze();                   // Stop watchdog
                }

                if ((pm_current_sleep_mode >= pm_mode_deep_sleep_no_mirror)
                                && (sleep_period == 0)) {
                        hw_timer1_enable();                     // Start OS timer
                }

#ifdef CONFIG_USE_BLE
                /*
                 * Apply ROM pathes
                 */
                patch_rom_functions();
#endif

                /*
                 * Wake-up event occurred! Clear SCB->SCR and continue.
                 * Note: Any interrupt may have triggered a WFI() exit! If one was pending before
                 * calling the WFI() then the system domain was not put in power down at all!
                 */
                SCB->SCR &= ~(1 << 2);
        }
}

void pm_sleep_enter(uint32_t low_power_periods)
{
        uint32_t rtc_time;
        uint32_t lp_prescaled_time;
        uint32_t lp_current_time;
        uint32_t trigger;
        uint32_t sleep_period;
        bool allow_stopping_tick = false;
        bool allow_entering_sleep = false;
        bool abort_sleep = false;
        uint32_t dummy;
        bool mac_status = false;        // false: off, true: on

#if (CPM_DEBUG == 1)
        low_power_periods_ret = low_power_periods;
#endif

#if defined CONFIG_USE_BLE || defined CONFIG_USE_FTDF
        uint32_t tmp = CRG_TOP->SYS_STAT_REG;
#endif

#ifdef CONFIG_USE_BLE
        if ((tmp & CRG_TOP_SYS_STAT_REG_BLE_IS_UP_Msk) == CRG_TOP_SYS_STAT_REG_BLE_IS_UP_Msk) {
                mac_status = true;
        }
#endif


        if (dg_configUSE_LP_CLK == LP_CLK_RCX) {
                if (cm_rcx_calibration_is_on && mac_status) {
                        abort_sleep = true;
                }
        }

#if (dg_configDISABLE_BACKGROUND_FLASH_OPS == 0)
        if (qspi_pending_ops != NULL) {
                abort_sleep = true;
        }
#endif

        /*
         * Update Real Time Clock value
         */
        rtc_time = rtc_get_fromCPM(&lp_prescaled_time, &lp_current_time);

        /*
         * Check if sleep is allowed and if it is possible (i.e. if the MAC(s) are turned off).
         * Make sure that even if a MAC has reported it's sleeping, it is powered down. If not, 
         * block sleeping (i.e. we run in "active" mode for testing).
         */
        if (!abort_sleep && (pm_current_sleep_mode != pm_mode_active)) {
                uint32_t lp_tick_offset;
                uint32_t os_sleep_time = 0;
                uint32_t rtc_offset;
                uint32_t sleep_time_reduction = 0;
                bool is_infinite = true;
                uint32_t wakeup_time = (uint32_t)pm_wakeup_xtal16m_time;

                /* We plan to stop Tick */
                allow_stopping_tick = true;

                if ((pm_current_sleep_mode != pm_mode_idle) && !mac_status) {
                        /* We plan to enter sleep */
                        allow_entering_sleep = true;
                }

                /*
                 * Compute when the earliest wake-up time is.
                 * 
                 * lp_prescaled_time - lp_last_trigger : offset in this tick period (in prescaled LP 
                 *         cycles, for the calculation of the sleep_period of the OS tasks, this 
                 *         offset must be subtracted).
                 * rtc_time : absolute time (in LP cycles)
                 * 
                 * sleep_period holds the result (in prescaled LP cycles).
                 */

                // 1. Check OS first!
                if (low_power_periods) {
                        // 1a. Offset in this tick period.
                        lp_tick_offset = lp_current_time
                                        - (1 + dg_configTim1Prescaler) * lp_last_trigger;
                        // 1b. Set within valid limits.
                        lp_tick_offset &= LP_CNT_NATIVE_MASK;
                        if (lp_tick_offset > (LP_CNT_NATIVE_MASK / 2)) {
                                lp_tick_offset = 0;
                        }
                        // 1c. Calculate time of wake-up as an offset of the current RTC value.
                        if (lp_tick_offset > (1 + dg_configTim1Prescaler) * low_power_periods) {
                                // We are already late! The tick interrupt may already be pending...
                                allow_entering_sleep = false;
                                allow_stopping_tick = false;
                        } else {
                                os_sleep_time = (1 + dg_configTim1Prescaler) * low_power_periods
                                                - lp_tick_offset;
                        }
                        // 1d. Subtract power-up time.
                        if (allow_entering_sleep) {
                                if (pm_wakeup_mode_is_XTAL16) {
                                        if (os_sleep_time > wakeup_time) {
                                                sleep_time_reduction = wakeup_time;
                                                os_sleep_time -= sleep_time_reduction;
                                        } else {
                                                allow_entering_sleep = false;
                                        }
                                } else {
                                        if (os_sleep_time > dg_configWAKEUP_RC16_TIME) {
                                                sleep_time_reduction = dg_configWAKEUP_RC16_TIME;
                                                os_sleep_time -= sleep_time_reduction;
                                        } else {
                                                allow_entering_sleep = false;
                                        }
                                }
                        }
                        // 1e. Initially, wake-up time is set for the OS case. 
                        sleep_period = os_sleep_time;
                        is_infinite = false;
                } else {
                        // 1f. Sleep period is infinite for the OS.
                        sleep_period = rtc_time - 1;
                }

                if (allow_entering_sleep) {                     // Power-down is possible.
#ifdef CONFIG_USE_BLE
                        // 2. Check BLE.
                        if (pm_mac_wakeup_time[0] != 0) {
                                uint32_t os_restore_time = dg_configOS_TICK_RESTORE_TIME;

                                if (pm_mac_wakeup_time[0] < rtc_time) {
                                        allow_entering_sleep = false;
                                } else {
                                        rtc_offset = (uint32_t) (pm_mac_wakeup_time[0] - rtc_time);
                                        if (rtc_offset > os_restore_time) {
                                                if (is_infinite || ((rtc_offset - os_restore_time) < sleep_period)) {
                                                        /* As it is not easy to estimate when
                                                         * exactly the OS tick should be resumed
                                                         * after waking up to serve the MAC, the
                                                         * OS resumption is set at the earliest
                                                         * point in time possible.
                                                         */
                                                        sleep_time_reduction = os_restore_time;
                                                        sleep_period = rtc_offset - os_restore_time;
                                                        is_infinite = false;
                                                }
                                        } else {
                                                allow_entering_sleep = false;
                                        }
                                }
                        }
#endif


                        do {
                                int i;
                                adapter_call_backs_t *p_Ad;

                                // 4. Check if sleep is blocked for some short period
                                if (sleep_is_blocked) {
                                        rtc_offset = sleep_blocked_until - rtc_time;
                                        if (rtc_offset < dg_configPM_MAX_ADAPTER_DEFER_TIME) {
                                                // Still valid ==> Block is ON.
                                                allow_entering_sleep = false;
                                                break;
                                        } else {
                                                // Time has passed! Reset flag!
                                                sleep_is_blocked = false;
                                        }
                                }

                                // 5. Calculate the overhead added from the Adapters.
                                if (!is_infinite) {
                                        for (i = 0; i < dg_configPM_MAX_ADAPTERS_CNT; i++) {
                                                p_Ad = pm_adapters_cb[i];
                                                if (p_Ad != NULL) {
                                                        if (sleep_period > p_Ad->ad_sleep_preparation_time) {
                                                                sleep_time_reduction += p_Ad->ad_sleep_preparation_time;
                                                                sleep_period -= p_Ad->ad_sleep_preparation_time;
                                                        } else {
                                                                sleep_period = 0;
                                                                allow_entering_sleep = false;
                                                                break;
                                                        }
                                                }
                                        }
                                }
                        } while (0);
                }

                // 6. Calculate sleep_period and check if power-down is possible.
                if (is_infinite) {
                        sleep_period = 0;
                } else {
                        // 6a. Adjust for image data copy time, if power-down is possible.
                        if ((dg_configEXEC_MODE == MODE_IS_MIRRORED)
                                        && allow_entering_sleep
                                        && ((pm_current_sleep_mode == pm_mode_extended_sleep)
                                         || (pm_current_sleep_mode == pm_mode_deep_sleep))) {
                                uint32_t copy_time = dg_configIMAGE_COPY_TIME;

                                if (sleep_period > copy_time) {
                                        sleep_time_reduction += copy_time;
                                        sleep_period -= copy_time;
                                }
                                // else it is smaller than dg_configMIN_SLEEP_TIME anyway...
                        }

                        // 6b. Abort power down if sleep_period is too short!
                        if (sleep_period < dg_configMIN_SLEEP_TIME) {
                                allow_entering_sleep = false;
                        }

                        // 6c. Restore sleep time if power-down is not possible.
                        if (!allow_entering_sleep) {
                                /* Since the system will not be powered-down, restore the sleep time
                                 * by inverting any reductions that have been made to account for
                                 * delays relevant to power-down / wake-up.
                                 */
                                sleep_period += sleep_time_reduction;

                                /* If the CPU clock is too slow, wake-up earlier in order to be able
                                 * to resume the OS in time.
                                 */
                                if (cm_cpu_clk_get_fromISR() < cpuclk_16M) {
                                        sleep_period -= dg_configWAKEUP_RC16_TIME;
                                }
                        }

                        // 6d. Check if sleep period is too small!
                        if (sleep_period <= ((1 + dg_configTim1Prescaler) * TICK_PERIOD)) {
                                allow_stopping_tick = false;
                        }
                }

#if (CPM_DEBUG == 1)
                sleep_period_ret = sleep_period;
                prev_trigger_setting_ret = lp_last_trigger;
#endif
        }

        if (allow_stopping_tick) {
                /*
                 * Mark that a wake-up interrupt will be treated as such and not as a typical interrupt
                 */
                pm_system_sleeping = sys_idle;

                if ((pm_current_sleep_mode >= pm_mode_deep_sleep_no_mirror)
                                && (sleep_period == 0)) {
                        /* Interrupt is already disabled! 
                         * No LP clock will be available during sleep and no wake up has been 
                         * requested! The system will wake up only from an external event!
                         * No trigger will be programmed.
                         */
                } else {
                        uint32_t lp_latest_time;
                        uint32_t computational_delay = 0; // Up to 1 for 16MHz, up to 10 for 1MHz CPU clock!

                        /* If no "deep sleep" mode and sleep_period is infinite then wake up 
                         * periodically to implement RTC.
                         * If "deep sleep" and wake-up has been requested after a sleep_period then
                         * schedule the wake up. 
                         * In both cases, the lp clock will stay alive to allow the timer to wake 
                         * the system up in time. The lp_last_trigger already holds the last trigger 
                         * that hit since the prvStopTickInterrupt() has been called!
                         */

                        if (sleep_period == 0) {
                                /* 
                                 * No wake-up requested. Still, we need to wake-up just before the 
                                 * timer1 wraps! Allow 64 ticks time before this happens, for 
                                 * safety.
                                 */
                                trigger = lp_prescaled_time - 64 * TICK_PERIOD;
                        } else {
                                // Check possible overflow and allow for 64 ticks "guard time".
                                if (sleep_period > MAX_IDLE_TIME_ALLOWED) {
                                        sleep_period = MAX_IDLE_TIME_ALLOWED;
                                } else {
                                        /*
                                         * Configure Timer1 to raise interrupt at the requested time.
                                         */
                                        rtc_time = rtc_get_fromCPM(&dummy, &lp_latest_time);

                                        computational_delay = (lp_latest_time - lp_current_time) & LP_CNT_NATIVE_MASK;

                                        /* Make sure computational_delay is less than 10 prescaled cycles (else is too big!). */
                                        ASSERT_WARNING(computational_delay < (10 * (1 + dg_configTim1Prescaler)));

                                        sleep_period -= computational_delay;
                                }

                                // Calculate sleep period in prescaled LP cycles!
                                sleep_period /= (1 + dg_configTim1Prescaler);

                                /* Sleep 1 prescaled cycle less, when prescaling is used, to
                                 * account for errors due to prescaling. */
                                sleep_period -= (dg_configTim1Prescaler ? 1 : 0);

                                // Set wake-up trigger (minus any computational delays).
                                trigger = (lp_prescaled_time + sleep_period) & LP_CNT_PRESCALED_MASK;
                        }

#if (CPM_DEBUG == 1)
                        trigger_setting_ret = trigger;
#endif
                        HW_TIMER1_SET_TRIGGER(trigger, dummy);          // Set reload value
                        NVIC_ClearPendingIRQ(SWTIM1_IRQn);              // Clear any pending IRQ from this source
                        hw_timer1_int_enable();                         // Enable interrupt

#if (CPM_DEBUG == 1)
                        rt_trigger_mon[rt_trigger_mon_wr].type = 1;
                        rt_trigger_mon[rt_trigger_mon_wr].value = trigger;
                        rt_trigger_mon_wr++;
                        rt_trigger_mon_wr %= MAX_TRG_MON_SZ;
#endif
                }

                /**********************************************************************************/

                if (allow_entering_sleep) {
                        int i;
                        adapter_call_backs_t *p_Ad;

                        /*
                         * Inquiry Adapters about the forthcoming sleep entry
                         */

                        // 1. Inquiry Adapters
                        for (i = dg_configPM_MAX_ADAPTERS_CNT - 1; i >= 0; i--) {
                                p_Ad = pm_adapters_cb[i];
                                if ((p_Ad != NULL) && (p_Ad->ad_prepare_for_sleep != NULL)) {
                                        if (!p_Ad->ad_prepare_for_sleep()) {
                                                break;
                                        }
                                }
                        }

                        // 2. If an Adapter rejected sleep, resume any Adapters that have already accepted it.
                        if (i >= 0) {
                                allow_entering_sleep = false;   // Sleep has been canceled.

                                i++;
                                while (i < dg_configPM_MAX_ADAPTERS_CNT) {
                                        p_Ad = pm_adapters_cb[i];
                                        if ((p_Ad != NULL) && (p_Ad->ad_sleep_canceled != NULL)) {
                                                p_Ad->ad_sleep_canceled();
                                        }
                                        i++;
                                }
                        }
                }

                apply_wfi(allow_entering_sleep, sleep_period);

                if (pm_system_sleeping == sys_powered_down) {
                        if (dg_configIMAGE_SETUP == DEVELOPMENT_MODE) {
                                rtc_time = rtc_get_fromISR();
                        }
                }

                if (!allow_entering_sleep) {
                        /*
                         * Hook to add any application specific code after exiting Idle state.
                         * Note: the System and the Peripheral Power Domains are active.
                         */
                        configPOST_IDLE_ENTRY( sleep_period );
                }
        } else {
                /* Make sure that it's not vTaskStepTick() that advances time but the Timer1 ISR, in
                 * this case!
                 */
               ulTimeSpentSleepingInTicks = 0;

#if (dg_configDISABLE_BACKGROUND_FLASH_OPS == 1)
               /* Wait for an interrupt
                *
                * Any interrupt will cause an exit from WFI(). This is not a problem since even
                * if an interrupt other that the tick interrupt occurs before the next tick comes,
                * the only thing that should be done is to resume the scheduler. Since no tick has
                * occurred, the OS time will be the same.
                */
               __WFI();
#else
               pm_execute_active_wfi();
#endif
        }
}

#if (dg_configDISABLE_BACKGROUND_FLASH_OPS == 0)
/**
 * \brief Monitor the progress of an ongoing program QSPI operation
 *
 * \param[out] bool true if the operation is still in progress else false
 *
 * \returns true if an interrupt is pending, else false
 */
__RETAINED_CODE static bool process_qspi_program_finish(bool *in_progress)
{
        bool pending_irq = false;

        DBG_SET_HIGH(FLASH_DEBUG, FLASHDBG_PAGE_PROG_WL);

        do {
                if (NVIC->ISER[0] & NVIC->ISPR[0]) {
                        pending_irq = true;

                        DBG_SET_HIGH(FLASH_DEBUG, FLASHDBG_PAGE_PROG_WL_IRQ);
                }

                *in_progress = qspi_check_program_erase_in_progress();
        } while (!pending_irq && *in_progress);

        DBG_SET_LOW(FLASH_DEBUG, FLASHDBG_PAGE_PROG_WL_IRQ);
        DBG_SET_LOW(FLASH_DEBUG, FLASHDBG_PAGE_PROG_WL);

        return pending_irq;
}

/**
 * \brief Issue (or continue execution of a) program QSPI operation
 *
 * \returns true if the operation was completed, else false (i.e. an interrupt is pending)
 */
__RETAINED_CODE static bool process_qspi_program(void)
{
        uint16_t *p_size;
        qspi_ops *op;
        bool pending_irq;
        bool skip_wfi = false;

        op = qspi_pending_ops;
        p_size = op->size;

        do {
                bool in_progress;

                op->written += flash_program_page_manual_mode(op->addr + op->written,
                                        op->buf + op->written, *p_size - op->written);

                /* Check if the operation has finished or if an interrupt
                 * is pending.
                 */
                pending_irq = process_qspi_program_finish(&in_progress);
                if (!in_progress && (op->written == *p_size)) {
                        // Notify the waiting task without delay
                        skip_wfi = true;
                }
        } while (!pending_irq && (op->written < *p_size));

        return skip_wfi;
}

void pm_execute_active_wfi(void)
{
        bool skip_wfi = false;
        qspi_ops *op;

        op = qspi_pending_ops;

        // If an interrupt is already pending, do not sleep.
        if (NVIC->ISER[0] & NVIC->ISPR[0]) {
                DBG_SET_LOW(CPM_USE_TIMING_DEBUG, CPMDBG_SLEEP_ENTER);
                DBG_SET_HIGH(CPM_USE_TIMING_DEBUG, CPMDBG_SLEEP_EXIT);
                return;
        }

        // Proceed with executing any pending QSPI program / erase operations.
        if (op != NULL) {
                if (op->suspended) {
                        DBG_SET_HIGH(FLASH_DEBUG, FLASHDBG_RESUME);

                        qspi_resume();
                        hw_cpm_delay_usec(FLASH_SUS_DELAY); // Guard time until SUSPEND can be issued!
                        op->suspended = false;

                        DBG_SET_LOW(FLASH_DEBUG, FLASHDBG_RESUME);

                        if (op->op_type == true) {      // program
                                bool pending_irq, in_progress;

                                pending_irq = process_qspi_program_finish(&in_progress);
                                if (!in_progress) {
                                        if (op->written == *op->size) {
                                                // Notify the waiting task without delay
                                                skip_wfi = true;
                                        }

                                        if (!pending_irq && !skip_wfi) {
                                                // More data to write...
                                                skip_wfi = process_qspi_program();
                                        }
                                }
                        }
                } else {
                        if (op->op_type == false) {     // erase
                                DBG_SET_HIGH(FLASH_DEBUG, FLASHDBG_SECTOR_ERASE);

                                flash_erase_sector_manual_mode(op->addr);

                                DBG_SET_LOW(FLASH_DEBUG, FLASHDBG_SECTOR_ERASE);
                        } else {                        // program
                                flash_activate_command_entry_mode();
                                skip_wfi = process_qspi_program();
                        }
                }
        }

        /* Wait for an interrupt
         *
         * Any interrupt will cause an exit from WFI(). This is not a problem since even
         * if an interrupt other that the tick interrupt occurs before the next tick comes,
         * the only thing that should be done is to resume the scheduler. Since no tick has
         * occurred, the OS time will be the same.
         */
        DBG_SET_LOW(CPM_USE_TIMING_DEBUG, CPMDBG_SLEEP_ENTER);
        if (!skip_wfi) {
                __WFI();
        }
        DBG_SET_HIGH(CPM_USE_TIMING_DEBUG, CPMDBG_SLEEP_EXIT);

        // Suspend any ongoing QSPI program / erase operations.
        if (op != NULL) {
                DBG_SET_HIGH(FLASH_DEBUG, FLASHDBG_SUSPEND);

                if (qspi_check_and_suspend()) {
                        op->suspended = true;
                }

                DBG_SET_LOW(FLASH_DEBUG, FLASHDBG_SUSPEND);
        }
}

void pm_process_completed_qspi_operations(void)
{
        qspi_ops *op;

        op = qspi_pending_ops;

        if (op && (op->suspended == false)) {
                DBG_SET_HIGH(FLASH_DEBUG, FLASHDBG_TASK_NOTIFY);

                if ((op->op_type == false)
                        || ((op->op_type == true) && (op->written == *(op->size)))) {
                        // The QSPI operation has been completed
                        if (op->written != 0) {
                                *(op->size) = op->written;
                        }
                        qspi_pending_ops = op->next;

                        OS_TASK_RESUME_FROM_ISR(op->handle);
                        // Calling portYIELD() here is needless (and an error...)

                        free(op);
                }

                DBG_SET_LOW(FLASH_DEBUG, FLASHDBG_TASK_NOTIFY);
        }
}

bool pm_register_qspi_operation(OS_TASK handle, uint32_t addr, const uint8_t *buf, uint16_t *size)
{
        qspi_ops *op;

        if (xSemaphorePM == NULL) {
                return false;
        }

        op = (qspi_ops *)malloc(sizeof(qspi_ops));
        ASSERT_ERROR(op != NULL);

        OS_EVENT_WAIT(xSemaphorePM, OS_EVENT_FOREVER);    // Block forever

        op->handle = handle;
        op->addr = addr;
        op->buf = buf;
        op->size = size;
        op->written = 0;
        if (buf == NULL) {
                op->op_type = false;                    // erase
        } else {
                op->op_type = true;                     // program
        }
        op->suspended = false;
        op->next = NULL;

        if (qspi_pending_ops) {
                qspi_ops *p = qspi_pending_ops;

                while (p->next != NULL) {
                        p = p->next;
                }

                p->next = op;
        } else {
                qspi_pending_ops = op;
        }

        OS_EVENT_SIGNAL(xSemaphorePM);

        return true;
}
#endif

/**
 \}
 \}
 \}
 */
