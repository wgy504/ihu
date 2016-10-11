/**
 ****************************************************************************************
 *
 * @file hw_trng.c
 *
 * @brief Implementation of the True Random Number Generator Low Level Driver.
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

#if dg_configUSE_HW_TRNG

#include <hw_trng.h>

#if (dg_configSYSTEMVIEW)
#  include "SEGGER_SYSVIEW_FreeRTOS.h"
#else
#  define SEGGER_SYSTEMVIEW_ISR_ENTER()
#  define SEGGER_SYSTEMVIEW_ISR_EXIT()
#endif

#define HW_TRNG_FIFO_DEPTH      (32)
#define HW_TRNG_RAM             (0x40040000)

static hw_trng_cb trng_cb;

static void hw_disable_trng(void)
{
        REG_CLR_BIT(TRNG, TRNG_CTRL_REG, TRNG_ENABLE);
}

static void hw_disable_trng_clk(void)
{
        GLOBAL_INT_DISABLE();
        REG_CLR_BIT(CRG_TOP, CLK_AMBA_REG, TRNG_CLK_ENABLE);
        GLOBAL_INT_RESTORE();
}

static void hw_trng_clear_pending(void)
{
        // read TRNG_FIFOLVL_REG to clear level-sensitive source.
        TRNG->TRNG_FIFOLVL_REG;
        NVIC_ClearPendingIRQ(TRNG_IRQn);
}

void hw_trng_enable(hw_trng_cb callback)
{
        hw_trng_disable();

        if (callback != NULL) {
                trng_cb = callback;
                hw_trng_clear_pending();
                NVIC_EnableIRQ(TRNG_IRQn);
        }

        GLOBAL_INT_DISABLE();
        REG_SET_BIT(CRG_TOP, CLK_AMBA_REG, TRNG_CLK_ENABLE);
        GLOBAL_INT_RESTORE();
        REG_SET_BIT(TRNG, TRNG_CTRL_REG, TRNG_ENABLE);
}

void hw_trng_get_numbers(uint32_t* buffer, uint8_t size)
{
        if (size > HW_TRNG_FIFO_DEPTH)
                size = HW_TRNG_FIFO_DEPTH;

        for (int i = 0; i < size ; i++) {
                buffer[i] = *((volatile uint32_t *)HW_TRNG_RAM);
        }
}

uint8_t hw_trng_get_fifo_level(void)
{
        return (TRNG->TRNG_FIFOLVL_REG) & (REG_MSK(TRNG, TRNG_FIFOLVL_REG, TRNG_FIFOLVL) |
                                           REG_MSK(TRNG, TRNG_FIFOLVL_REG, TRNG_FIFOFULL));
}

void hw_trng_disable(void)
{
        hw_disable_trng();
        NVIC_DisableIRQ(TRNG_IRQn);
        hw_trng_clear_pending();
        hw_disable_trng_clk();
}

void TRNG_Handler(void)
{
        SEGGER_SYSTEMVIEW_ISR_ENTER();

        if (trng_cb != NULL)
                trng_cb();

        hw_trng_clear_pending();

        SEGGER_SYSTEMVIEW_ISR_EXIT();
}

#endif /* dg_configUSE_HW_TRNG */
