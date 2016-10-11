/**************************************************************************//**
 * @file     system_ARMCM0.c
 * @brief    CMSIS Cortex-M0 Device System Source File
 *           for CM0 Device Series
 * @version  V1.05
 * @date     26. July 2011
 *
 * @note
 * Copyright (C) 2010-2011 ARM Limited. All rights reserved.
 *
 * @par
 * ARM Limited (ARM) is supplying this software for use with Cortex-M 
 * processor based microcontrollers.  This file can be freely distributed 
 * within development tools that are supporting such ARM based processors. 
 *
 * @par
 * THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * ARM SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 ******************************************************************************/
 

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "sdk_defs.h"
#include "interrupts.h"
#include "hw_cpm.h"
#include "hw_otpc.h"
#include "hw_qspi.h"
#include "hw_watchdog.h"
#include "sys_tcs.h"
#include "qspi_automode.h"


#define DEBUG_TCS_HANDLING_OTP          0


#if (DEBUG_TCS_HANDLING_OTP == 1)
const uint64_t sample_otp_tcs_data[] = {
                0,                      /* Mirrored/Cached At startup */
                0,                      /* Non-Volatile Memory */
                0,                      /* Product Ready */
                0,                      /* Redundancy */
                0,                      /* Reserved */
                0,                      /* Shuffle RAMs */
                0,                      /* JTAG */
                0,                      /* Sleep Clock */
                0,                      /* Position/Timestamp */
                0,                      /* Tester */
                0,                      /* Mirror Image Length */
                0xAA,                   /* TCS Valid */
                0,                      /* Chip Unique ID */
                0,                      /* Cache architecture */
                0,                      /* Serial Configuration Mapping */
                0,                      /* Image CRC */
                0,                      /* Reserved */
                0,                      /* QSPI Functions */
                0,                      /* UART STX timing */
                0,                      /* Reserved */
                0,                      /* Reserved */
                0,                      /* Reserved */
                0,                      /* Reserved */
                0xAFFFFFD750000028,     /* 1: BANDGAP_REG address */
                0xFFFFEFEC00001013,     /*    BANDGAP_REG value */
                0xAFFFFFDD50000022,     /* 2: CLK_16M_REG address */
                0xFFFFEB4D000014B2,     /*    CLK_16M_REG value */
                0xAFFFFFDF50000020,     /* 3: CLK_32K_REG address */
                0xFFFFF8FF00000700,     /*    CLK_32K_REG value */
                0xAFFFDF5D500020A2,     /* 4: _RF_a reg address */
                0xFFFFFFCA00000035,     /*    _RF_a reg value */
#ifdef CONFIG_USE_BLE
                0xBFFFFDFF40000200,     /* : BLE_CNTL2_REG address */
                0xFFFFEFEC00001013,     /*   BLE_CNTL2_REG value */
#endif
                0xAFFFDF7950002086,     /* 5: _RF_b reg address */
                0xFFFF37770000C888,     /*    _RF_b reg value */
                0xAFFFDF51500020AE,     /* 6: _RF_c reg address */
                0xFFFFFA12000005ED,     /*    _RF_c reg value */
#ifdef CONFIG_USE_BLE
                0xBFFFFDFB40000204,     /* : BLE_EM_BASE_REG address */
                0xFFFFEFEC00001013,     /*   BLE_EM_BASE_REG value */
#endif
                0xAFFFD2F550002D0A,     /* 7: _RF_d reg address */
                0xFFFFFF2F000000D0,     /*    _RF_d reg value */
                0xAFFFDF4D500020B2,     /* 8: _RF_e reg address */
                0xFFFFFFCA00000035,     /*    _RF_e reg value */
                0xAFFFE4F550001B0A,     /* 9: CHARGER_CTRL2_REG address */
                0xFFFFF2E400000D1B,     /*    CHARGER_CTRL2_REG value */
                0xAFFFBFFF50004000,     /* 10: APU: SRC1_CTRL_REG address */
                0xFFFFEFEC00001013,     /*    APU: SRC1_CTRL_REG value */
                0xAFFFEFF35000100C,     /* 11: UART_LCR_REG address */
                0xFFFFEFEC00001013,     /*    UART_LCR_REG value */
                0,                      /* 12: empty */
                0,                      /*  */
                0,                      /* 13: empty */
                0,                      /*  */
                0,                      /* 14: empty */
                0,                      /*  */
                0,                      /* 15: empty */
                0,                      /*  */
                0,                      /* 16: empty */
                0,                      /*  */
                0,                      /* 17: empty */
                0,                      /*  */
                0,                      /* 18: empty */
                0,                      /*  */
                0,                      /* 19: empty */
                0,                      /*  */
#ifndef CONFIG_USE_BLE
                0,                      /* 20: empty */
                0,                      /*  */
                0,                      /* 21: empty */
                0,                      /*  */
#endif
                0,                      /* 22: empty */
                0,                      /*  */
                0,                      /* 23: empty */
                0,                      /*  */
                0,                      /* 24: empty */
                0                       /* End TCS section */
};
#endif

#if (dg_configCODE_LOCATION == NON_VOLATILE_IS_FLASH) || (dg_configPOWER_FLASH == 1)
/*
 * We need the partition table information for configure_cache(),
 * to adjust the cacheable area accordingly.
 */
#define PARTITION_TABLE_ENTRY(start, size, id) \
                                        const uint32_t id##_start = 0x8000000 + start; \
                                        const uint32_t id##_end = 0x8000000 + start + size;
#define PARTITION2(start, size, id, flags) \
                                        const uint32_t id##_start = 0x8000000 + start; \
                                        const uint32_t id##_end = 0x8000000 + start + size;

#include "flash_partitions.h"
#else
/*
 * This is just so that compilation doesn't break for RAM builds.
 * The code that uses it is optimized out in RAM builds.
 */
extern uint32_t NVMS_PARAM_PART_end;
#endif

#if (dg_configCONFIG_HEADER_IN_FLASH == 1) || (dg_configUSE_AUTO_CHIP_DETECTION == 1)

/*
 * TCS Offsets
 */
#if (DEBUG_TCS_HANDLING_OTP == 1)
#define OTP_HEADER_BASE_ADDR_IN_OTP     (uint8_t *)sample_otp_tcs_data
#else
#define OTP_HEADER_BASE_ADDR_IN_OTP     (0x7F8E9C0)
#endif

#define TCS_SECTION_OFFSET              (184) /* 0x7F8EA78 - 0x7F8E9C0 */
#define TCS_SECTION_LENGTH              (24)  /* 24 entries, 16 bytes each for OTP, 8 bytes each for Flash*/

#define BANDGAP_OFFSET                  (12)
#define FLASH_TCS_OFFSET                (0x1000)

#else

#if (DEBUG_TCS_HANDLING_OTP == 1)
#define OTP_HEADER_BASE_ADDR_IN_OTP     (uint8_t *)sample_otp_tcs_data
#else
#define OTP_HEADER_BASE_ADDR_IN_OTP     (0x7F8E9C0)
#endif

#define TCS_SECTION_OFFSET              (184)
#define TCS_SECTION_LENGTH              (24)

#define BANDGAP_OFFSET                  (0)
#define FLASH_TCS_OFFSET                (0)

#endif

#ifndef __SYSTEM_CLOCK
# define __SYSTEM_CLOCK                 (16000000UL) /* 16 MHz */
#endif

#if (dg_configUSE_LP_CLK == LP_CLK_32000)
# define LP_CLK_FREQ                    (32000)

#elif (dg_configUSE_LP_CLK == LP_CLK_32768)
# define LP_CLK_FREQ                    (32768)

#elif (dg_configUSE_LP_CLK == LP_CLK_RCX)
# define LP_CLK_FREQ                    (0)

#elif (dg_configUSE_LP_CLK == LP_CLK_ANY)
// Must be defined in the custom_config_<>.h file.

#else
# error "dg_configUSE_LP_CLK is not defined or has an unknown value!"
#endif

__RETAINED_UNINIT __attribute__((unused)) uint32_t black_orca_chip_version;
uint32_t SystemCoreClock __RETAINED = __SYSTEM_CLOCK;   /*!< System Clock Frequency (Core Clock)*/
uint32_t SystemLPClock __RETAINED = LP_CLK_FREQ;        /*!< System Low Power Clock Frequency (LP Clock)*/

/**
 * @brief  Write data to a specific location in the QSPI Flash.
 *
 */
static void write_flash(uint32_t addr, const uint8_t *buf, size_t size)
{
        uint16_t written = 0;

        do {
                written += qspi_automode_write_flash_page(addr + written, &buf[written], size - written);
        } while (written != size);
}

/**
 * Apply trim values from OTP.
 *
 * @brief Writes the trim values located in the OTP to the corresponding system registers.
 *
 * @param[out] tcs_array The valid <address, value> pairs are placed in this buffer.
 * @param[out] valid_entries The number of valid pairs.
 *
 * @return True if at least one trim value has been applied, else false.
 *
 */
static bool apply_trim_values_from_otp(uint32_t *tcs_array, uint32_t *valid_entries)
{
        uint32_t address;
        uint32_t inverted_address;
        uint32_t value;
        uint32_t inverted_value;
        uint32_t *p;
        int i;
        int index = 0;
        int vdd = 0;
        int retries = 0;
        bool forward_reading = true;
        bool res = false;

        p = (uint32_t *)(OTP_HEADER_BASE_ADDR_IN_OTP + TCS_SECTION_OFFSET);

        for (i = 0; i < TCS_SECTION_LENGTH; i++) {
                do {
                        address = *p;
                        p++;
                        inverted_address = *p;
                        p++;
                        value = *p;
                        p++;
                        inverted_value = *p;
                        p++;

                        if ((address == 0) && (value == 0)) {
                                break;
                        }

                        // Check validity
                        if ((address != ~inverted_address) || (value != ~inverted_value)) {
                                // Change LDO core voltage level and retry
                                vdd++;
                                vdd &= 0x3;
                                REG_SETF(CRG_TOP, LDO_CTRL1_REG, LDO_CORE_SETVDD, vdd);

                                // Wait for the voltage to settle...
                                SysTick->CTRL = 0;
                                SysTick->LOAD = 500;    // 500 * (62.5 * 4) = 125usec
                                SysTick->VAL = 0;
                                SysTick->CTRL = 0x5;    // Start using system clock
                                while ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) == 0) {}

                                // Adjust the read pointer
                                p -= 4;
                        }

                        retries++;
                        if (retries == 32) {
                                // Unrecoverable problem! Assert in development mode
                                ASSERT_WARNING(0);

                                // Unrecoverable problem! Issue a HW reset.
                                hw_cpm_reset_system();
                        }
                } while ((address != ~inverted_address) || (value != ~inverted_value));

                retries = 0;

                // Read the complete TCS area but skip empty entries.
                if ((address == 0) && (value == 0)) {
                        if (((dg_configBLACK_ORCA_IC_REV == BLACK_ORCA_IC_REV_A) &&
                                        (dg_configBLACK_ORCA_IC_STEP >= BLACK_ORCA_IC_STEP_D))
                                || ((dg_configUSE_AUTO_CHIP_DETECTION == 1)
                                        && (CHIP_IS_AD || CHIP_IS_AE))) {
                                if (!forward_reading) {
                                        break;
                                }

                                forward_reading = false;
                                p = (uint32_t *)(OTP_HEADER_BASE_ADDR_IN_OTP + TCS_SECTION_OFFSET);
                                p += (TCS_SECTION_LENGTH - 1) * 4;
                        }
                        else {
                                (void)forward_reading;
                        }

                        continue;
                }

                if (!forward_reading) {
                        p -= 8;
                }

                sys_tcs_store_pair(address, value);

                tcs_array[(index * 2) + 0] = address;
                tcs_array[(index * 2) + 1] = value;

                *valid_entries = index + 1;
                index++;

                res = true;
        }

        return res;
}

/**
 * Apply trim values from Flash.
 *
 * @brief  Writes the trim values located in the Flash to the corresponding system registers.
 *
 * @return True if at least one trim value has been applied, else false.
 *
 */
static bool apply_trim_values_from_flash(void)
{
        uint32_t address;
        uint32_t value;
        uint32_t *p;
        int i;
        bool res = false;

        p = (uint32_t *)(NVMS_PARAM_PART_end - FLASH_TCS_OFFSET);

        for (i = 0; i < (TCS_SECTION_LENGTH + 1); i++) {
                address = *p;
                p++;
                value = *p;
                p++;

                if ((address == 0xFFFFFFFF) && (value == 0xFFFFFFFF)) {
                        if (i == 0) {
                                continue;       // Skip CLK_FREQ_TRIM_REG entry
                        }
                        else {
                                break;          // Not programmed.
                        }
                }

                if ((address == 0) && (value == 0)) {
                        /* The first entry is the XTAL16M trim and is placed there by the customer.
                         * As it may not be always there, don't abort based on its value. */
                        if (i != 0) {
                                break;
                        }
                }
                else {
                        res = sys_tcs_store_pair(address, value);
                }
        }

        return res;
}

/**
 * Check whether the code has been compiled for a chip version that is compatible with the chip
 * it runs on.
 */
static bool is_compatible_chip_version(void)
{
        const uint32_t ver = black_orca_get_chip_version();

        /* Oldest supported version is AD. */
        if ((ver < BLACK_ORCA_IC_VERSION(A, D)) && (dg_configUSE_AUTO_CHIP_DETECTION == 0)) {
                return false;
        }

        if ((ver == BLACK_ORCA_TARGET_IC) || (dg_configUSE_AUTO_CHIP_DETECTION == 1)) {
                return true;
        }

        return false;
}


#ifdef OS_BAREMETAL

static volatile bool nortos_xtal16m_settled = false;

void XTAL16RDY_Handler(void)
{
        nortos_xtal16m_settled = true;
}

/* carry out clock initialization sequence */
static void nortos_clk_setup(void)
{
        // Setup DIVN
        if (dg_configEXT_CRYSTAL_FREQ == EXT_CRYSTAL_IS_16M) {
                hw_cpm_set_divn(false);                 // External crystal is 16MHz
        }
        else {
                hw_cpm_set_divn(true);                  // External crystal is 32MHz
        }
        hw_cpm_enable_rc32k();
        hw_cpm_lp_set_rc32k();

        NVIC_ClearPendingIRQ(XTAL16RDY_IRQn);
        nortos_xtal16m_settled = false;
        NVIC_EnableIRQ(XTAL16RDY_IRQn);                 // Activate XTAL16 Ready IRQ

        hw_cpm_set_xtal16m_settling_time(dg_configXTAL16_SETTLE_TIME_RC32K);
        hw_cpm_enable_xtal16m();                        // Enable XTAL16M

#ifdef TEST_XTAL16M_KICKING
        SysTick->CTRL = 0;                              // Disable SysTick
        SysTick->LOAD = 19999;                          // ~20msec
        SysTick->VAL = 0;
        SysTick->CTRL = SysTick_CTRL_ENABLE_Msk;        // Use ~1MHz clock, Enable
        while (SysTick->VAL == 0);                      // Wait until SysTick reloaded
#endif

        while(!hw_cpm_is_xtal16m_started()) {           // Block until XTAL16M starts
#ifdef TEST_XTAL16M_KICKING
                if ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) != 0) {
                        hw_watchdog_freeze();           // Stop watchdog
                        while(1);                       // Stop here, to facilitate debugging
                }
#endif
        }

#ifdef TEST_XTAL16M_KICKING
        SysTick->CTRL = 0;                              // Disable SysTick
#endif

        /* Wait for XTAL16M to settle */
        while(!nortos_xtal16m_settled)
                ;

        hw_cpm_set_sysclk(SYS_CLK_IS_XTAL16M);
}

#endif  /* OS_BAREMETAL */


static __RETAINED_CODE void configure_cache(void)
{
#if (dg_configCODE_LOCATION == NON_VOLATILE_IS_FLASH) || (dg_configPOWER_FLASH == 1)
        bool flush = false;

        GLOBAL_INT_DISABLE();

        if (dg_configCACHEABLE_QSPI_AREA_LEN != -1) {
                /* set cacheable area */
                REG_SETF(CACHE, CACHE_CTRL2_REG, CACHE_LEN, dg_configCACHEABLE_QSPI_AREA_LEN >> 16);
                flush = true;
        }

        if (dg_configCACHE_ASSOCIATIVITY != CACHE_ASSOC_AS_IS) {
                if (CACHE->CACHE_ASSOCCFG_REG != dg_configCACHE_ASSOCIATIVITY) {
                        /* override the set associativity setting */
                        CACHE->CACHE_ASSOCCFG_REG = dg_configCACHE_ASSOCIATIVITY;
                        flush = true;
                }
        }

        if (dg_configCACHE_LINESZ != CACHE_LINESZ_AS_IS) {
                if (CACHE->CACHE_LNSIZECFG_REG != dg_configCACHE_LINESZ) {
                        /* override the cache line setting */
                        CACHE->CACHE_LNSIZECFG_REG = dg_configCACHE_LINESZ;
                        flush = true;
                }
        }

        if (flush) {
                /* flush cache */
                REG_SET_BIT(CACHE, CACHE_CTRL1_REG, CACHE_FLUSH);
        }

        GLOBAL_INT_RESTORE();
#endif
}


/**
 * Initialize the system
 *
 * @brief  Setup the microcontroller system.
 *         Initialize the System.
 */
void SystemInit(void)
{
        if (((dg_configBLACK_ORCA_IC_REV == BLACK_ORCA_IC_REV_A)
                && (dg_configBLACK_ORCA_IC_STEP == BLACK_ORCA_IC_STEP_E))
                || ((dg_configUSE_AUTO_CHIP_DETECTION == 1) && CHIP_IS_AE)) {
                /*
                 * Use fast clocks from now on.
                 */
                hw_cpm_set_hclk_div(0);
                hw_cpm_set_pclk_div(0);
        }
        bool trimming_ok = false;

        /*
         * Make sure we are running on a chip version that the code has been built for.
         */
        ASSERT_WARNING(is_compatible_chip_version());

        /*
         * Detect chip version (optionally).
         */
        if (dg_configUSE_AUTO_CHIP_DETECTION == 1) {
                black_orca_chip_version = black_orca_get_chip_version();

                if (!CHIP_IS_AD && !CHIP_IS_AE) {
                        // Oldest supported version is AD.
                        ASSERT_WARNING(0);
                }

                if ((CHIP_IS_AD || CHIP_IS_AE) && (dg_configLOW_VBAT_HANDLING == 0)) {
                        // dg_configLOW_VBAT_HANDLING cannot be 0 when AD or AE is used
                        ASSERT_WARNING(0);
                }
        }

        /*
         * Initialize UNINIT variables.
         */
        sys_tcs_init();

        /*
         * Activate BOD protection (only for AD; for AE is already enabled by the bootloader).
         */
        if ((dg_configUSE_BOD == 1)
                && (((dg_configBLACK_ORCA_IC_REV == BLACK_ORCA_IC_REV_A)
                                && (dg_configBLACK_ORCA_IC_STEP == BLACK_ORCA_IC_STEP_D))
                        || ((dg_configUSE_AUTO_CHIP_DETECTION == 1) && CHIP_IS_AD))) {
                hw_cpm_activate_bod_protection_at_init();       // Use BOD
        }

        /*
         * Apply default priorities to interrupts.
         */
        set_interrupt_priorities(__dialog_interrupt_priorities);

        /*
         * Enable debugger.
         */
        if (dg_configENABLE_DEBUGGER) {
                ENABLE_DEBUGGER;
        }

#if (dg_configIMAGE_SETUP == PRODUCTION_MODE) && (dg_configENABLE_DEBUGGER > 0)
#       warning "Enabling debugger (JTAG) access in PRODUCTION build!"
#endif

        /*
         * If we are executing from RAM, make sure that code written in Flash won't have left the
         * system in "unknown" state.
         */
        if (dg_configCODE_LOCATION == NON_VOLATILE_IS_NONE) {
                GLOBAL_INT_DISABLE();
                CRG_TOP->PMU_CTRL_REG |= 0xE;
                CRG_TOP->PMU_CTRL_REG &= ~1;
                GLOBAL_INT_RESTORE();
        }

        /*
         * Switch to RC16.
         */
        hw_cpm_set_sysclk(SYS_CLK_IS_RC16);             // Use RC16
        hw_cpm_start_ldos();                            // Make sure all LDOs are as expected
        hw_cpm_disable_xtal16m();                       // Disable XTAL16M
        hw_cpm_reset_radio_vdd();                       // Set Radio voltage to 1.40V

        /*
         * Initialize Flash (needed for RAM applications).
         */
        if ((dg_configCODE_LOCATION == NON_VOLATILE_IS_FLASH) || (dg_configPOWER_FLASH == 1)) {
                hw_qspi_set_div(HW_QSPI_DIV_1);
        }

        /*
         * Trim values handling.
         */
        if ((dg_configCONFIG_HEADER_IN_FLASH == 1)
                || ((dg_configUSE_AUTO_CHIP_DETECTION == 1) && CHIP_IS_AD)) {
                /*
                 * Check if the trim values exist in FLASH.
                 */
                if (dg_configCODE_LOCATION == NON_VOLATILE_IS_FLASH) {
                        uint16_t bandgap_v;

                        /*
                         * Make sure the memory map is OK.
                         */
                        ASSERT_WARNING((NVMS_PARAM_PART_end >= MEMORY_QSPIF_BASE)
                                && (NVMS_PARAM_PART_end <= (MEMORY_QSPIF_BASE + 0x3F00000)));

                        bandgap_v = *(volatile uint16_t *)0x800000C;

                        if (bandgap_v != 0xFFFF) {
                                /*
                                 * Check if TCS is in FLASH and is valid and apply trim values.
                                 */
                                trimming_ok = apply_trim_values_from_flash();
                        }
                }
        }

        if ((((dg_configBLACK_ORCA_IC_REV == BLACK_ORCA_IC_REV_A)
                && (dg_configBLACK_ORCA_IC_STEP == BLACK_ORCA_IC_STEP_D))
                || ((dg_configUSE_AUTO_CHIP_DETECTION == 1) && CHIP_IS_AD)) && trimming_ok) {
                /*
                 * Retain trimming_ok so SystemInitPost() can find it later
                 */
                GPIO->GPIO_CLK_SEL = 1;
                /*
                 * Use fast clocks from now on.
                 */
                hw_cpm_set_hclk_div(0);
                hw_cpm_set_pclk_div(0);
        }
}



/**
 * Process the TCS
 *
 * @brief  Process the TCS section.
 */
void SystemInitPost(void)
{
        bool trimming_ok;
        int i;

        SystemCoreClock = __SYSTEM_CLOCK;
        SystemLPClock = LP_CLK_FREQ;

        /*
         * Initialize Flash
         */
        if ((dg_configCODE_LOCATION == NON_VOLATILE_IS_FLASH) || (dg_configPOWER_FLASH == 1)) {
                qspi_automode_init();                   // The bootloader may have left the Flash in wrong mode...
        }

        /*
         * Retrieve trimming_ok status
         */
        trimming_ok = (GPIO->GPIO_CLK_SEL != 0);

        /*
         * If the trim values are not in FLASH, check OTP.
         */
        if (!trimming_ok) {
                uint32_t tcs_pairs[TCS_SECTION_LENGTH * 2];
                uint32_t valid_tcs_pairs = 0;

                hw_otpc_init();         // Start clock.
                hw_otpc_disable();      // Make sure it is in standby mode.
                hw_otpc_init();         // Restart clock.
                hw_otpc_manual_read_on(false);

                /*
                 * Apply trim values from OTP and copy the header to FLASH.
                 */
                trimming_ok = apply_trim_values_from_otp(tcs_pairs, &valid_tcs_pairs);

                if (dg_configIMAGE_SETUP == PRODUCTION_MODE) {
                        ASSERT_ERROR(sys_tcs_is_calibrated_chip);       // Uncalibrated chip!!!
                }

                if (trimming_ok) {
                        /*
                         * Bandgap is set. Use fast clocks from now on.
                         */
                        hw_cpm_set_hclk_div(0);
                        hw_cpm_set_pclk_div(0);

                        if ((dg_configCODE_LOCATION == NON_VOLATILE_IS_FLASH)
                                && ((dg_configCONFIG_HEADER_IN_FLASH == 1)
                                || ((dg_configUSE_AUTO_CHIP_DETECTION == 1) && CHIP_IS_AD))) {
                                /*
                                 * If the BANDGAP value is not written in the Flash Header,
                                 * write it.
                                 */
                                for (i = 0; i < valid_tcs_pairs; i++) {
                                        if (tcs_pairs[i * 2] == (uint32_t)&(CRG_TOP->BANDGAP_REG)) {
                                                uint16_t val;

                                                val = *(uint16_t *)(MEMORY_QSPIF_BASE + BANDGAP_OFFSET);

                                                if (val != tcs_pairs[(i * 2) + 1]) {
                                                        // Write the value in the Flash.
                                                        write_flash(BANDGAP_OFFSET,
                                                                        (uint8_t *)&tcs_pairs[(i * 2) + 1],
                                                                        2);
                                                }

                                                break;
                                        }
                                }

                                /*
                                 * Write the TCS section to Flash.
                                 */
                                if (valid_tcs_pairs != 0) {
                                        uint32_t first_entry[2];
                                        uint32_t *p;
                                        uint32_t base;

                                        // Save the first entry.
                                        p = (uint32_t *)(NVMS_PARAM_PART_end - FLASH_TCS_OFFSET);
                                        first_entry[0] = *p;    // address
                                        p++;
                                        first_entry[1] = *p;    // value

                                        qspi_automode_erase_flash_sector(NVMS_PARAM_PART_end - FLASH_TCS_OFFSET);

                                        base = NVMS_PARAM_PART_end - FLASH_TCS_OFFSET;

                                        write_flash(base, (uint8_t *)first_entry, 8);

                                        base += 8;
                                        write_flash(base, (uint8_t *)tcs_pairs,
                                                        valid_tcs_pairs * 2 * 4);
                                }

                                /*
                                 * Reset system so that a "clean boot" follows.
                                 */
                                hw_cpm_reboot_system();
                        }
                }

                hw_otpc_manual_read_off();
                hw_otpc_disable();
        }

        if ((dg_configCODE_LOCATION == NON_VOLATILE_IS_FLASH) || (dg_configPOWER_FLASH == 1)) {
                hw_cpm_enable_qspi_init();              // Enable QSPI init after wakeup

                hw_qspi_set_read_pipe_clock_delay(6);   // Set read pipe clock delay to 6 (Last review date: Feb 15, 2016 - 12:25:47)
        }

        /*
         * All trim values have been loaded from TCS or defaults are used.
         */
        sys_tcs_sort_array();
        sys_tcs_apply(tcs_system);
        hw_cpm_set_preferred_values();

#ifdef OS_BAREMETAL
        /* perform clock initialization here, as there is no CPM to do it later for us */
        nortos_clk_setup();
#endif

        configure_cache();
}

uint32_t black_orca_phy_addr(uint32_t addr)
{
        static const uint32 remap[] = {
                MEMORY_ROM_BASE,
                MEMORY_OTP_BASE,
                MEMORY_QSPIF_BASE,
                MEMORY_SYSRAM_BASE,
                MEMORY_QSPIF_BASE,
                MEMORY_OTP_BASE,
                MEMORY_CACHERAM_BASE,
                0
        };

        if (addr >= MEMORY_REMAPPED_END)
                return addr;

        return addr + remap[REG_GETF(CRG_TOP, SYS_CTRL_REG, REMAP_ADR0)];
}

