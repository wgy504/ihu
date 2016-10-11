/**
 *****************************************************************************************
 *
 * @file hw_trng.h
 *
 * @brief Definition of API for the True Random Number Generator Low Level Driver.
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

#if dg_configUSE_HW_TRNG

#include <sdk_defs.h>

/**
 * \brief TRNG callback.
 *
 * This function is called by the TRNG driver when the interrupt is fired.
 *
 * \note If the TRNG is not needed anymore the hw_trng_disable function should be called in the
 * callback function to save power.
 *
 */
typedef void (*hw_trng_cb)(void);

/**
 * \brief TRNG enable.
 *
 * This function enables the TRNG. If callback is not NULL it will be called when a TRNG interrupt
 * occurs. The interrupt is triggered when the TRNG FIFO is full.
 *
 * \param [in] callback The callback function that is called when a TRNG interrupt occurs.
 *
 * \note If the amount of random numbers needed is less than the contents of the FIFO it is faster
 * and more power efficient to use a wait loop in combination with the hw_trng_get_fifo_level
 * function. If the FIFO has the required level use the hw_trng_get_numbers function to get the
 * random numbers.
 *
 */
void hw_trng_enable(hw_trng_cb callback);

/**
 * \brief TRNG get numbers.
 *
 * This function reads the random number from the TRNG FIFO.
 *
 * \param [in] buffer The buffer to write the numbers to.
 * \param [in] size The size of the buffer (max 32).
 *
 * \note Do not forget to disable the TRNG after reading the amount of numbers needed to save
 * power.
 *
 */
void hw_trng_get_numbers(uint32_t* buffer, uint8_t size);

/**
 * \brief TRNG get FIFO level.
 *
 * This function returns the current level of the TRNG FIFO.
 *
 * \return The current level of the TRNG FIFO.
 *
 */
uint8_t hw_trng_get_fifo_level(void);

/**
 * \brief TRNG disable.
 *
 * This function disables the TRNG and its interrupt.
 *
 */
void hw_trng_disable(void);

#endif /* dg_configUSE_HW_TRNG */
