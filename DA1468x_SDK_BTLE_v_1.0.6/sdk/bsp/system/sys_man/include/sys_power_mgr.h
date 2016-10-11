/**
 * \addtogroup BSP
 * \{
 * \addtogroup SYSTEM
 * \{
 * \addtogroup POWER_MANAGER
 * 
 * \brief Power Manager
 *
 * \{
 */

/**
 ****************************************************************************************
 *
 * @file sys_power_mgr.h
 *
 * @brief Power manager header file.
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


#ifndef SYS_POWER_MGR_H_
#define SYS_POWER_MGR_H_

#include <stdint.h>
#include <stdbool.h>
#include "sdk_defs.h"
#include "osal.h"

typedef void (*periph_init_cb)(void);
typedef int32_t pm_id_t;

typedef enum {
        pm_mode_active,
        pm_mode_idle,
        pm_mode_extended_sleep_no_mirror,
        pm_mode_extended_sleep,
        pm_mode_deep_sleep_no_mirror,
        pm_mode_deep_sleep,
        pm_mode_hibernation,
} sleep_mode_t;

typedef struct _adptCB {
        bool (*ad_prepare_for_sleep)(void);
        void (*ad_sleep_canceled)(void);
        void (*ad_wake_up_ind)(bool);
        void (*ad_xtal16m_ready_ind)(void);
        uint8_t ad_sleep_preparation_time;
} adapter_call_backs_t;

typedef enum system_state_type {
        sys_active = 0,
        sys_idle,
        sys_powered_down,
} system_state_t;

#define PM_BASE_ID      (1000)
#define PM_MAX_ID       (1001)

#ifdef CONFIG_USE_BLE
#define PM_BLE_ID       (1000)
#endif



/*
 * Variables declarations
 */

extern uint32_t lp_last_trigger;
extern adapter_call_backs_t *pm_adapters_cb[dg_configPM_MAX_ADAPTERS_CNT];
extern uint16_t pm_wakeup_xtal16m_time;

/*
 * Function declarations
 */

/**
 * \brief Initialize the system after power-up.
 *
 * \param[in] peripherals_initialization Call-back to an application function that handles
 *            the initialization of the GPIOs and the peripherals.
 *
 * \return void
 *
 * \warning This function will change when the initialization of the GPIOs and the peripherals is
 *          moved to the Adapters (or wherever it is decided).
 *
 */
void pm_system_init(periph_init_cb peripherals_initialization);

/**
 * \brief Wait for the debugger to detach if sleep is used.
 *
 * \param[in] mode The sleep mode of the application. It must be different than pm_mode_active if
 *            the application intends to use sleep.
 *
 */
void pm_wait_debugger_detach(sleep_mode_t mode);

/**
 * \brief Sets the wake-up mode of the system (whether the OS will be resumed with RC16 or XTAL16).
 *
 * \param[in] wait_for_xtal16m This is the new wake-up mode.
 *
 * \return void
 *
 * \warning The function will block if another task is accessing the Power Manager.
 *
 */
void pm_set_wakeup_mode(bool wait_for_xtal16m);

/**
 * \brief Returns the wake-up mode of the system (whether the OS will be resumed with RC16 or XTAL16).
 *
 * \return The wake-up mode.
 *
 * \warning The function will block if another task is accessing the Power Manager.
 *
 */
bool pm_get_wakeup_mode(void);

/**
 * \brief Sets the sleep mode of the system.
 *
 * \param[in] mode This is the new sleep mode.
 *
 * \return The previous mode set.
 *
 * \warning The function will block if another task is accessing the Power Manager.
 *
 */
sleep_mode_t pm_set_sleep_mode(sleep_mode_t mode);

/**
 * \brief Returns the sleep mode of the system.
 *
 * \return The sleep mode used.
 *
 * \warning The function will block if another task is accessing the Power Manager.
 *
 */
sleep_mode_t pm_get_sleep_mode(void);

/**
 * \brief Sets the system in active mode.
 *
 * \return void
 *
 * \warning The function will block if another task is accessing the Power Manager.
 *
 */
void pm_stay_alive(void);

/**
 * \brief Allows the system to go to idle mode.
 *
 * \warning The function will block if another task is accessing the Power Manager.
 *
 */
void pm_stay_idle(void);

/**
 * \brief Restores the sleep mode of the system, which has been blocked via a call to
 *        pm_stay_alive().
 *
 * \return void
 *
 * \warning The function will block if another task is accessing the Power Manager.
 *
 */
void pm_resume_sleep(void);

/**
 * \brief Registers an Adapter to the Power Manager.
 *
 * \param[in] cb The pointer to the set of the call-back functions of this Adapter.
 *
 * \return pm_id_t The registered Adapter's ID in the Power Manager state table.
 *
 * \warning The function will block if another task is accessing the Power Manager.
 *
 */
pm_id_t pm_register_adapter(const adapter_call_backs_t *cb);

/**
 * \brief Unregisters an Adapter with a specific ID from the Power Manager.
 *
 * \param[in] id The ID of the Adapter in the Power Manager.
 *
 * \return void
 *
 * \warning The function will block if another task is accessing the Power Manager.
 *
 */
void pm_unregister_adapter(pm_id_t id);

/**
 * \brief Informs the PM when a MAC has planned to wake-up.
 *
 * \param[in] id The ID of the MAC.
 * \param[in] time_in_LP_cycles The offset from the current system time in (non-prescaled) Low Power
 *            clock cycles, when the caller has planned to be active. In other words, it is the time
 *            that the MAC will be sleeping, counting from this moment.
 *
 * \return void
 *
 * \warning Cannot be called from Interrupt Context! Must be called with ALL interrupts disabled!
 *
 */
void pm_resource_sleeps_until(pm_id_t id, uint32_t time_in_LP_cycles);

/**
 * \brief Informs the PM when a MAC has waken-up.
 *
 * \param[in] id The ID of the MAC.
 *
 * \return void
 *
 * \warning The function never blocks! Is is called from Interrupt Context!
 *
 */
void pm_resource_is_awake(pm_id_t id);

/**
 * \brief Called by an Adapter to ask the PM not to go to sleep for some short period.
 *
 * \param[in] id The ID of the Adapter.
 * \param[in] time_in_LP_cycles The offset from the current system time, in (non-prescaled) Low
 *            Power clock cycles, until when the caller requests the system to stay active.
 *
 * \return void
 *
 * \warning Called from Interrupt Context! Must be called with ALL interrupts disabled!
 *
 */
void pm_defer_sleep_for(pm_id_t id, uint32_t time_in_LP_cycles);

/**
 * \brief Called to retrieve the next wakeup time for a MAC
 *
 * \param[in] id The ID of the MAC

 * \return The time when MAC with id needs to wakeup
 *
 * \warning If the returned value equals pm_wakeup_xtal16m_time, then this
 *          MAC will either not sleep, or sleep forever
 *
 */
uint64_t pm_get_mac_wakeup_time(pm_id_t id);

/*
 * \brief Put the system to idle or sleep or blocks in a WFI() waiting for the next tick, if neither
 *        idle nor sleep is possible.
 *
 * \detail Puts the system to idle or sleep, if possible. If an exit-from-idle or a wake-up is
 *         needed, it programs the Timer1 to generate an interrupt after the specified idle or sleep
 *         period. Else, the system stays forever in idle or sleep mode.
 *         If neither idle nor sleep is possible, it blocks in a WFI() call waiting for the next
 *         (already programmed) OS tick to hit.
 *
 * \param[in] xLowPowerPeriods The number of (prescaled) low power clock periods the OS will be
 *            idle. If it is 0 then the OS indicates that it can block forever waiting for an
 *            external event. If the system goes to sleep, then it can wake up only from an external
 *            event in this case.
 *
 * \return void
 *
 * \warning Must be called with interrupts disabled!
 *
 */
__RETAINED_CODE void pm_sleep_enter(uint32_t low_power_periods);

/*
 * \brief When continuing from the WFI(), check if we were sleeping and, if so, power-up the system.
 *
 * \return void
 *
 * \warning Must be called with interrupts disabled!
 *
 */
void pm_system_wake_up(void);

/**
 * \brief Advances time from the previous tick that hit.
 *
 * \details Calculate how many ticks have passed since the last tick.
 *
 * \return uint32_t The number of ticks passed.
 *
 */
__RETAINED_CODE uint32_t pm_advance_time(uint32_t prescaled_time);

#if (dg_configDISABLE_BACKGROUND_FLASH_OPS == 0)

typedef struct _pm_qspi_ops {
        OS_TASK handle;
        uint32_t addr;
        const uint8_t *buf;
        uint16_t *size;
        uint16_t written;
        bool op_type;           /* false: erase, true: program */
        bool suspended;
        struct _pm_qspi_ops *next;
} qspi_ops;

/**
 * \brief Register a program or erase QSPI operation to be executed by the CPM in background
 *
 * \param [in] handle the handle of the task that registers the operation
 * \param [in] addr starting offset
 * \param [in] buf the buffer containing the data to be written
 * \param [inout] len pointer to the length of the data in the buffer
 *
 * \returns true if the operation was registered successfully, else false
 */
bool pm_register_qspi_operation(OS_TASK handle, uint32_t addr, const uint8_t *buf, uint16_t *len);

/**
 * \brief Execute the "active WFI" when Flash operations should be processed in the background
 *
 */
__RETAINED_CODE void pm_execute_active_wfi(void);

/**
 * \brief Notify tasks waiting for Flash operations that they have been completed
 *
 */
__RETAINED_CODE void pm_process_completed_qspi_operations(void);
#endif

#define pm_conv_ticks_2_prescaled_lpcycles( x )   ( (x) * TICK_PERIOD )
#define pm_conv_ms_2_prescaled_lpcycles( x )      ( (x) * configSYSTICK_CLOCK_HZ / (1 + dg_configTim1Prescaler) / 1000 )

#endif /* SYS_POWER_MGR_H_ */

/**
 \}
 \}
 \}
 */
