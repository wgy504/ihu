/**
 * \addtogroup BSP
 * \{
 * \addtogroup DEVICES
 * \{
 * \addtogroup Watchdog_Timer
 * \{
 * \brief Watchdog Timer
 */

/**
 *****************************************************************************************
 *
 * @file hw_watchdog.h
 *
 * @brief Definition of API for the Watchdog timer Low Level Driver.
 *
 * Copyright (C) 2015. Dialog Semiconductor Ltd, unpublished work. This computer
 * program includes Confidential, Proprietary Information and is a Trade Secret of
 * Dialog Semiconductor Ltd. All use, disclosure, and/or reproduction is prohibited
 * unless authorized in writing. All Rights Reserved.
 *
 * <black.orca.support@diasemi.com> and contributors.
 *
 *****************************************************************************************
 */

#ifndef HW_WATCHDOG_H_
#define HW_WATCHDOG_H_


#include <stdbool.h>
#include <stdint.h>
#include <sdk_defs.h>

#define NMI_MAGIC_NUMBER                0xDEADBEEF

/**
 * \brief Holds the stack contents when an NMI occurs.
 *
 * \details The stack contents are copied at this variable when an NMI occurs. The first position is
 *        marked with a special "flag" (0xDEADBEEF) to indicate that the data that follow are valid.
 */
extern uint32_t nmi_event_data[9];

/**
 * \brief Types of generated states if reload value is 0
 *
 * Generate NMI (non-maskable interrupt) or RST (reset of the system)
 *
 */
typedef enum {
        HW_WDG_RESET_NMI = 0,     /**< Generate NMI if the watchdog reaches 0 and WDOG reset if the counter become less or equal to -16 */
        HW_WDG_RESET_RST = 1      /**< Generate WDOG reset it the counter becomes less or equal than 0 */
} HW_WDG_RESET;

/**
 * \brief Watchdog timer interrupt callback
 *
 * \param [in] hardfault_args pointer to call stack
 *
 */
typedef void (*hw_watchdog_interrupt_cb)(unsigned long *exception_args);

/**
 * \brief Freeze the watchdog
 *
 * \return true if operation is allowed, else false
 *
 */
__RETAINED_CODE bool hw_watchdog_freeze(void);

/**
 * \brief Unfreeze the watchdog
 *
 */
__RETAINED_CODE void hw_watchdog_unfreeze(void);

/**
 * \brief Set positive reload value of the watchdog timer
 *
 * \param [in] value reload value from 0 (0x00) to 255 (0xFF)
 *
 */
static inline void hw_watchdog_set_pos_val(uint8_t value) __attribute__((always_inline));

static inline void hw_watchdog_set_pos_val(uint8_t value)
{
        WDOG->WATCHDOG_REG = (uint16_t)value;
}

/**
 * \brief Set negative reload value of the watchdog timer
 *
 * \param [in] value reload value from -16 (0x1FF) to 0 (0x00)
 *
 */
static inline void hw_watchdog_set_neg_val(uint8_t value)
{
        WDOG->WATCHDOG_REG = WDOG_WATCHDOG_REG_WDOG_VAL_NEG_Msk | (uint16_t)value;
}

/**
 * \brief Get reload value of the watchdog timer
 *
 */
static inline uint16_t hw_watchdog_get_val(void)
{
        return REG_GETF(WDOG, WATCHDOG_REG, WDOG_VAL);
}

/**
 * \brief Generate a reset signal of the system if reload value reaches 0
 *
 */
static inline void hw_watchdog_gen_RST(void) __attribute__((always_inline));

static inline void hw_watchdog_gen_RST(void)
{
        REG_SET_BIT(WDOG, WATCHDOG_CTRL_REG, NMI_RST);
}

/**
 * \brief Register an interrupt handler
 *
 * \param [in] handler function pointer to handler to call when an interrupt occurs
 *
 */
void hw_watchdog_register_int(hw_watchdog_interrupt_cb handler);

/**
 * \brief Unregister an interrupt handler
 *
 */
__RETAINED_CODE void hw_watchdog_unregister_int(void);

/**
 * \brief Handle NMI interrupt.
 *
 * \param [in] hardfault_args pointer to call stack
 *
 */
__RETAINED_CODE void hw_watchdog_handle_int(unsigned long *hardfault_args);

/**
 * \brief Check whether the timer has expired
 *
 * \return true, if the timer has expired, false otherwise
 *
 */
bool hw_watchdog_is_timer_expired(void);

/**
 * \brief Check what is generated when watchdog reaches 0 value
 *
 * If it is NMI (interrupt) or RST (system/wdog reset).
 *
 * \return HW_WDG_RESET_NMI if NMI interrupt is generated, otherwise HW_WDG_RESET_RST
 *
 */
HW_WDG_RESET hw_watchdog_is_irq_or_rst_gen(void);

#endif /* HW_WATCHDOG_H_ */


/**
 * \}
 * \}
 * \}
 */
