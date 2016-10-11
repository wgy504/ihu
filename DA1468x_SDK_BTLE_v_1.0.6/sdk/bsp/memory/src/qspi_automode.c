/**
 ****************************************************************************************
 *
 * @file qspi_automode.c
 *
 * @brief Access QSPI flash when running in auto mode
 *
 * Copyright (C) 2015. Dialog Semiconductor Ltd, unpublished work. This computer
 * program includes Confidential, Proprietary Information and is a Trade Secret of
 * Dialog Semiconductor Ltd.  All use, disclosure, and/or reproduction is prohibited
 * unless authorized in writing. All Rights Reserved.
 *
 * <black.orca.support@diasemi.com> and contributors.
 *
 ****************************************************************************************
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "sdk_defs.h"

#if (dg_configDISABLE_BACKGROUND_FLASH_OPS == 0)
#include "osal.h"
#include "sys_power_mgr.h"
#endif

#include "hw_qspi.h"
#include "qspi_automode.h"

/*
 * QSPI controller allows to execute code directly from QSPI flash.
 * When code is executing from flash there is no possibility to reprogram it.
 * To be able to modify flash memory while it is used for code execution it must me assured that
 * during the time needed for erase/write no code is running from flash.
 */

/*
 * Debug options
 */
#if __DBG_QSPI_ENABLED
#define __DBG_QSPI_VOLATILE__           volatile
#warning "Automode: Debugging is on!"
#else
#define __DBG_QSPI_VOLATILE__
#endif


/*
 * Defines (generic)
 */
#define BITS32(base, reg, field, v) \
        ((uint32) (((uint32) (v) << (base ## _ ## reg ## _ ## field ## _Pos)) & \
                (base ## _ ## reg ## _ ## field ## _Msk)))

/* Macros to put functions that need to be copied to ram in one section (retained) */
#define QSPI_SECTION                    __attribute__((section ("text_retained")))
#define QSPI_SECTION_NOINLINE           __attribute__((section ("text_retained"), noinline))


/*
 * Flash specific defines
 */

/* SUS bit delay after SUSPEND command */
#define FLASH_SUS_DELAY                 (20) // in usec

/* Flash page size */
#define FLASH_MAX_WRITE_SIZE            dg_configFLASH_MAX_WRITE_SIZE

/* Erase/Write in progress */
#define W25Q_STATUS1_BUSY_BIT           0
#define W25Q_STATUS1_BUSY_MASK          (1 << W25Q_STATUS1_BUSY_BIT)

/* WE Latch bit */
#define W25Q_STATUS1_WEL_BIT            1
#define W25Q_STATUS1_WEL_MASK           (1 << W25Q_STATUS1_WEL_BIT)

/* Suspend */
#define W25Q_STATUS2_SUS_BIT            7
#define W25Q_STATUS2_SUS_MASK           (1 << W25Q_STATUS2_SUS_BIT)

/* QPI Enable */
#define W25Q_STATUS2_QE_BIT             1
#define W25Q_STATUS2_QE_MASK            (1 << W25Q_STATUS2_QE_BIT)

/*
 * Flash Commands
 *
 * Note: Default command issuing mode is single mode! If commands specific to other modes have to be
 * issued then the mode must be changed!
 */
#define W25Q_WRITE_STATUS_REGISTER1     0x01
#define W25Q_WRITE_STATUS_REGISTER2     0x31
#define W25Q_PAGE_PROGRAM               0x02
#define W25Q_WRITE_DISABLE              0x04
#define W25Q_READ_STATUS_REGISTER1      0x05
#define W25Q_WRITE_ENABLE               0x06
#define W25Q_WRITE_ENABLE_NON_VOL       0x50
#define W25Q_SECTOR_ERASE               0x20
#define W25Q_QUAD_PAGE_PROGRAM          0x32    // Requires single mode for the command entry!
#define W25Q_READ_STATUS_REGISTER2      0x35
#define W25Q_BLOCK_ERASE                0x52
#define W25Q_ERASE_PROGRAM_SUSPEND      0x75
#define W25Q_ERASE_PROGRAM_RESUME       0x7A
#define W25Q_BLOCK_ERASE_64K            0xD8
#define W25Q_CHIP_ERASE                 0xC7
#define W25Q_RELEASE_POWER_DOWN         0xAB
#define W25Q_FAST_READ_QPI              0x0B
#define W25Q_FAST_READ_QUAD             0xEB
#define W25Q_READ_DEVICE_ID_SINGLE      0x90    // Requires single mode for the command entry!
#define W25Q_READ_DEVICE_ID_DUAL        0x92    // Requires dual mode for the command entry!
#define W25Q_READ_DEVICE_ID_QUAD        0x94
#define W25Q_READ_UNIQUE_ID             0x4B    // Requires single mode for the command entry!
#define W25Q_READ_JEDEC_ID              0x9F    // Requires single mode for the command entry!
#define W25Q_READ_SFDP_REG              0x5A    // Requires single mode for the command entry!
#define W25Q_ERASE_SECURITY_REGS        0x44    // Requires single mode for the command entry!
#define W25Q_PROGR_SECURITY_REGS        0x42    // Requires single mode for the command entry!
#define W25Q_READ_SECURITY_REGS         0x48    // Requires single mode for the command entry!
#define W25Q_ENTER_QPI_MODE             0x38    // Requires single mode for the command entry!
#define W25Q_EXIT_QPI_MODE              0xFF    // Requires quad mode for the command entry!

/*
 * Use QUAD mode for page write.
 *
 * Note: If the flash does not support QUAD mode or it is not connected for QUAD mode set it to 0
 * (single mode).
 */
#ifndef QUAD_MODE
#define QUAD_MODE                       1
#endif

#ifndef ERASE_IN_AUTOMODE
#define ERASE_IN_AUTOMODE               1
#endif


/*
 * Forward declarations
 */
QSPI_SECTION static uint8_t flash_read_status_register_1(void);



/*
 * Function definitions
 */

/**
 * \brief Set bus mode to single or QUAD mode.
 *
 * \param[in] mode Can be single (HW_QSPI_BUS_MODE_SINGLE) or quad (HW_QSPI_BUS_MODE_QUAD) mode.
 *
 * \note DUAL mode page program so is not supported by this function.
 */
static inline void qspi_set_bus_mode(HW_QSPI_BUS_MODE mode) __attribute__((always_inline));

static inline void qspi_set_bus_mode(HW_QSPI_BUS_MODE mode)
{
        if (mode == HW_QSPI_BUS_MODE_SINGLE) {
                QSPIC->QSPIC_CTRLBUS_REG = REG_MSK(QSPIC, QSPIC_CTRLBUS_REG, QSPIC_SET_SINGLE);
                QSPIC->QSPIC_CTRLMODE_REG |=
                        BITS32(QSPIC, QSPIC_CTRLMODE_REG, QSPIC_IO2_OEN, 1) |
                        BITS32(QSPIC, QSPIC_CTRLMODE_REG, QSPIC_IO2_DAT, 1) |
                        BITS32(QSPIC, QSPIC_CTRLMODE_REG, QSPIC_IO3_OEN, 1) |
                        BITS32(QSPIC, QSPIC_CTRLMODE_REG, QSPIC_IO3_DAT, 1);
        } else {
#if QUAD_MODE
                QSPIC->QSPIC_CTRLBUS_REG = REG_MSK(QSPIC, QSPIC_CTRLBUS_REG, QSPIC_SET_QUAD);
                QSPIC->QSPIC_CTRLMODE_REG &=
                        ~(BITS32(QSPIC, QSPIC_CTRLMODE_REG, QSPIC_IO2_OEN, 1) |
                          BITS32(QSPIC, QSPIC_CTRLMODE_REG, QSPIC_IO3_OEN, 1));
#endif
        }
}

/**
 * \brief Set the mode of the QSPI controller (manual or auto)
 *
 * \param[in] automode True for auto and false for manual mode setting.
 */
static inline void qspi_set_automode(bool automode) __attribute__((always_inline));

static inline void qspi_set_automode(bool automode)
{
        REG_SETF(QSPIC, QSPIC_CTRLMODE_REG, QSPIC_AUTO_MD, automode);
}

/**
 * \brief Write to the Flash the contents of a buffer
 *
 * \param[in] wbuf Pointer to the beginning of the buffer
 * \param[in] wlen The number of bytes to be written
 *
 * \note The data are transferred as bytes (8 bits wide). No optimization is done in trying to use
 *       faster access methods (i.e. transfer words instead of bytes whenever it is possible).
 */
QSPI_SECTION static void qspi_write(const uint8_t *wbuf, size_t wlen)
{
        size_t i;

        hw_qspi_cs_enable();

        for (i = 0; i < wlen; ++i) {
                hw_qspi_write8(wbuf[i]);
        }

        hw_qspi_cs_disable();
}

/**
 * \brief Write an arbitrary number of bytes to the Flash and then read an arbitrary number of bytes
 *       from the Flash in one transaction
 *
 * \param[in] wbuf Pointer to the beginning of the buffer that contains the data to be written
 * \param[in] wlen The number of bytes to be written
 * \param[in] rbuf Pointer to the beginning of the buffer than the read data are stored
 * \param[in] rlen The number of bytes to be read
 *
 * \note The data are transferred as bytes (8 bits wide). No optimization is done in trying to use
 *       faster access methods (i.e. transfer words instead of bytes whenever it is possible).
 */
QSPI_SECTION static void qspi_transact(const uint8_t *wbuf, size_t wlen, uint8_t *rbuf, size_t rlen)
{
        size_t i;

        hw_qspi_cs_enable();

        for (i = 0; i < wlen; ++i) {
                hw_qspi_write8(wbuf[i]);
        }

        for (i = 0; i < rlen; ++i) {
                rbuf[i] = hw_qspi_read8();
        }

        hw_qspi_cs_disable();
}


/**
 * \brief Exit from QPI mode.
 */
static inline void flash_reset_continuous_mode(void) __attribute__((always_inline));

static inline void flash_reset_continuous_mode(void)
{
        hw_qspi_cs_enable();
        hw_qspi_write8(W25Q_EXIT_QPI_MODE);
        hw_qspi_cs_disable();

        while (flash_read_status_register_1() & W25Q_STATUS1_BUSY_MASK);
}

/**
 * \brief Release Flash from Power Down mode or Get Device ID when Flash is not in Power Down mode
 *
 * \return uint8_t The Device ID of the Flash, in case of a "Get Device ID" command.
 *
 * \note The function blocks until the Flash executes the command.
 */
QSPI_SECTION static uint8_t flash_release_power_down(void)
{
        uint8_t id;

        hw_qspi_cs_enable();
        hw_qspi_write32(0x000000AB);
        id = hw_qspi_read8();
        hw_qspi_cs_disable();

        while (flash_read_status_register_1() & W25Q_STATUS1_BUSY_MASK);

        return id;
}

/**
 * \brief Set WEL (Write Enable Latch) bit of the Status Register of the Flash
 * \details The WEL bit must be set prior to every Page Program, Quad Page Program, Sector Erase,
 *       Block Erase, Chip Erase, Write Status Register and Erase/Program Security Registers
 *       instruction. In the case of Write Status Register command, any status bits will be written
 *       as non-volatile bits.
 *
 * \note This function blocks until the Flash has processed the command and it will be repeated if,
 *       for any reason, the command was not successfully executed by the Flash.
 */
QSPI_SECTION static void flash_write_enable(void)
{
        __DBG_QSPI_VOLATILE__ uint8_t status;
        uint8_t cmd[] = { W25Q_WRITE_ENABLE };

        do {
                qspi_write(cmd, 1);

                /* Verify */
                do {
                        status = flash_read_status_register_1();
                } while (status & W25Q_STATUS1_BUSY_MASK);
        } while (!(status & W25Q_STATUS1_WEL_MASK));
}

/**
 * \brief Enable volatile writes to Status Register bits
 * \details When this command is issued, any writes to any of the Status Registers of the Flash are
 *        done as volatile writes. This command is valid only when the Write Status Register command
 *        follows.
 *
 * \note This function blocks until the Flash has processed the command.
 */
QSPI_SECTION __attribute__((unused)) static void flash_wre_volatile(void)
{
        __DBG_QSPI_VOLATILE__ uint8_t status;
        uint8_t cmd[] = { W25Q_WRITE_ENABLE_NON_VOL };

        qspi_write(cmd, 1);

        /* Verify */
        do {
                status = flash_read_status_register_1();
        } while (status & W25Q_STATUS1_BUSY_MASK);
}

/**
 * \brief Read the Status Register 1 of the Flash
 *
 * \return uint8_t The value of the Status Register 1 of the Flash.
 */
QSPI_SECTION static uint8_t flash_read_status_register_1(void)
{
        __DBG_QSPI_VOLATILE__ uint8_t status;
        uint8_t cmd[] = { W25Q_READ_STATUS_REGISTER1 };

        qspi_transact(cmd, 1, &status, 1);

        return status;
}

/**
 * \brief Write the Status Register 1 of the Flash
 *
 * \param[in] value The value to be written.
 *
 * \note This function blocks until the Flash has processed the command. No verification that the
 *        value has been actually written is done though. It is up to the caller to decide whether
 *        such verification is needed or not and execute it on its own.
 */
QSPI_SECTION __attribute__((unused)) static void flash_write_status_register_1(uint8_t value)
{
        __DBG_QSPI_VOLATILE__ uint8_t status;
        uint8_t cmd[2] = { W25Q_WRITE_STATUS_REGISTER1, value };

        qspi_write(cmd, 2);

        /* Wait for the Flash to process the command */
        do {
                status = flash_read_status_register_1();
        } while (status & W25Q_STATUS1_BUSY_MASK);
}

/**
 * \brief Read the Status Register 2 of the Flash
 *
 * \return uint8_t The value of the Status Register 2 of the Flash.
 */
QSPI_SECTION __attribute__((unused)) static uint8_t flash_read_status_register_2(void)
{
        __DBG_QSPI_VOLATILE__ uint8_t status;
        uint8_t cmd[] = { W25Q_READ_STATUS_REGISTER2 };

        qspi_transact(cmd, 1, &status, 1);

        return status;
}

/**
 * \brief Write the Status Register 2 of the Flash
 *
 * \param[in] value The value to be written.
 *
 * \note This function blocks until the Flash has processed the command. No verification that the
 *        value has been actually written is done though. It is up to the caller to decide whether
 *        such verification is needed or not and execute it on its own.
 */
QSPI_SECTION __attribute__((unused)) static void flash_write_status_register_2(uint8_t value)
{
        __DBG_QSPI_VOLATILE__ uint8_t status;
        uint8_t cmd[2] = { W25Q_WRITE_STATUS_REGISTER2, value };

        qspi_write(cmd, 2);

        /* Wait for the Flash to process the command */
        do {
                status = flash_read_status_register_1();
        } while (status & W25Q_STATUS1_BUSY_MASK);
}

/**
 * \brief Fast copy of a buffer to a FIFO
 * \details Implementation of a fast copy of the contents of a buffer to a FIFO in assembly. All
 *        addresses are word aligned.
 *
 * \param[in] start Pointer to the beginning of the buffer
 * \param[in] end Pointer to the end of the buffer
 * \param[in] Pointer to the FIFO
 *
 * \warning No validity checks are made! It is the responsibility of the caller to make sure that
 *        sane values are passed to this function.
 */
static inline
void fast_write_to_fifo32(uint32_t start, uint32_t end, uint32_t dest) __attribute__((always_inline));

static inline void fast_write_to_fifo32(uint32_t start, uint32_t end, uint32_t dest)
{
        asm volatile(   "copy:                                  \n"
                        "       ldmia %[start]!, {r3}           \n"
                        "       str r3, [%[dest]]               \n"
                        "       cmp %[start], %[end]            \n"
                        "       blt copy                        \n"
                        :
                        :                                                         /* output */
                        [start] "r" (start), [end] "r" (end), [dest] "r" (dest) : /* inputs (%0, %1, %2) */
                        "r3");                                                    /* registers that are destroyed */
}

/**
 * \brief Write data (up to 1 page) to Flash
 *
 * \param[in] addr The address of the Flash where the data will be written. It may be anywhere in a
 *        page.
 * \param[in] buf Pointer to the beginning of the buffer that contains the data to be written.
 * \param[in] size The number of bytes to be written.
 *
 * \return size_t The number of bytes written.
 *
 * \warning The boundary of the page where addr belongs to, will not be crossed! The caller should
 *        issue another flash_write_page() call in order to write the remaining data to the next
 *        page.
 */
QSPI_SECTION static size_t flash_write_page(uint32_t addr, const uint8_t *buf, uint16_t size)
{
        size_t i = 0;
        size_t odd = ((uint32_t) buf) & 3;
        size_t size_aligned32;
        size_t tmp;

        DBG_SET_HIGH(FLASH_DEBUG, FLASHDBG_PAGE_PROG);

        flash_write_enable();

        /* Reduce max write size, that can reduce interrupt latency time */
        if (size > FLASH_MAX_WRITE_SIZE) {
                size = FLASH_MAX_WRITE_SIZE;
        }

        /* Make sure write will not cross page boundary */
        tmp = 256 - (addr & 0xFF);
        if (size > tmp) {
                size = tmp;
        }

        hw_qspi_cs_enable();

        hw_qspi_write32(W25Q_QUAD_PAGE_PROGRAM | ((addr >> 8) & 0xFF00) | ((addr << 8) & 0xFF0000)
                | (addr << 24));

#if QUAD_MODE
        qspi_set_bus_mode(HW_QSPI_BUS_MODE_QUAD);
#endif

        if (odd) {
                odd = 4 - odd;
                for (i = 0; i < odd && i < size; ++i) {
                        hw_qspi_write8(buf[i]);
                }
        }

        size_aligned32 = ((size - i) & ~0x3);

        if (size_aligned32) {
                fast_write_to_fifo32((uint32_t)(buf + i), (uint32_t)(buf + i + size_aligned32),
                        (uint32_t)&(QSPIC->QSPIC_WRITEDATA_REG));
                i += size_aligned32;
        }

        for (; i < size; i++) {
                hw_qspi_write8(buf[i]);
        }

        hw_qspi_cs_disable();

        DBG_SET_LOW(FLASH_DEBUG, FLASHDBG_PAGE_PROG);

#if QUAD_MODE
        qspi_set_bus_mode(HW_QSPI_BUS_MODE_SINGLE);
#endif

        return i;
}

/**
 * \brief Erase a sector of the Flash
 *
 * \param[in] addr The address of the sector to be erased.
 *
 * \note This function blocks until the Flash has processed the command.
 */
QSPI_SECTION __attribute__((unused)) static void flash_erase_sector(uint32_t addr)
{
        uint8_t cmd[4] = { W25Q_SECTOR_ERASE, addr >> 16, addr >>  8, addr };
        __DBG_QSPI_VOLATILE__ uint8_t status;

        flash_write_enable();

        qspi_write(cmd, 4);

        /* Wait for the Flash to process the command */
        do {
                status = flash_read_status_register_1();
        } while (status & W25Q_STATUS1_BUSY_MASK);
}

/**
 * \brief Check if the Flash can accept commands
 *
 * \return bool True if the Flash is not busy else false.
 *
 */
QSPI_SECTION_NOINLINE static bool qspi_writable(void)
{
        bool writable;

        /*
         * From now on QSPI may not be available, turn off interrupts.
         */
        GLOBAL_INT_DISABLE();

        /*
         * Turn on command entry mode.
         */
        flash_activate_command_entry_mode();

        /*
         * Check if flash is ready.
         */
        writable = !(flash_read_status_register_1() & W25Q_STATUS1_BUSY_MASK);

        /*
         * Restore auto mode.
         */
        flash_deactivate_command_entry_mode();

        /*
         * Let other code to be executed including QSPI one.
         */
        GLOBAL_INT_RESTORE();

        return writable;
}

/**
 * \brief Get the status of an Erase when it is done automatically by the QSPI controller
 *
 * \return uint8_t The status of the Erase
 *        0: No Erase
 *        1: Pending erase request
 *        2: Erase procedure is running
 *        3: Suspended Erase procedure
 *        4: Finishing the Erase procedure
 */
QSPI_SECTION static uint8_t qspi_get_erase_status(void)
{
        QSPIC->QSPIC_CHCKERASE_REG = 0;

        return HW_QSPIC_REG_GETF(ERASECTRL, ERS_STATE);
}

/**
 * \brief Get the address size used by the QSPI controller
 *
 * \return The address size used (HW_QSPI_ADDR_SIZE_24 or HW_QSPI_ADDR_SIZE_32).
 */
static inline HW_QSPI_ADDR_SIZE qspi_get_address_size(void) __attribute__((always_inline));

static inline HW_QSPI_ADDR_SIZE qspi_get_address_size(void)
{
        return (HW_QSPI_ADDR_SIZE)HW_QSPIC_REG_GETF(CTRLMODE, USE_32BA);
}

__RETAINED_CODE void flash_activate_command_entry_mode(void)
{
        /*
         * Switch to single mode for command entry.
         */
        qspi_set_bus_mode(HW_QSPI_BUS_MODE_SINGLE);

        /*
         * Turn off auto mode to allow write.
         */
        qspi_set_automode(false);

        /*
         * Exit continuous mode (QPI mode), after this the flash will interpret commands again.
         */
        flash_reset_continuous_mode();
}

__RETAINED_CODE void flash_deactivate_command_entry_mode(void)
{
#if QUAD_MODE
        qspi_set_bus_mode(HW_QSPI_BUS_MODE_QUAD);
#endif
        qspi_set_automode(true);
}

#if (dg_configDISABLE_BACKGROUND_FLASH_OPS == 0)
__RETAINED_CODE void flash_erase_sector_manual_mode(uint32_t addr)
{
        uint8_t cmd[4] = { W25Q_SECTOR_ERASE, addr >> 16, addr >>  8, addr };

        /*
         * Turn on command entry mode.
         */
        flash_activate_command_entry_mode();

        /*
         * Issue the erase sector command.
         */
        flash_write_enable();
        qspi_write(cmd, 4);

        /*
         * Flash stays in manual mode.
         */
}

__RETAINED_CODE size_t flash_program_page_manual_mode(uint32_t addr, const uint8_t *buf, uint16_t size)
{
        size_t written = flash_write_page(addr, buf, size);

        /*
         * Flash stays in manual mode.
         */

        return written;
}

__RETAINED_CODE bool qspi_check_program_erase_in_progress(void)
{
        __DBG_QSPI_VOLATILE__ uint8_t status_1;

        status_1 = flash_read_status_register_1();
        return (status_1 & W25Q_STATUS1_BUSY_MASK);
}

__RETAINED_CODE bool qspi_check_and_suspend(void)
{
        uint8_t cmd[] = { W25Q_ERASE_PROGRAM_SUSPEND };
        __DBG_QSPI_VOLATILE__ uint8_t status_1;
        __DBG_QSPI_VOLATILE__ uint8_t status_2;
        bool am = hw_qspi_get_automode();
        bool ret = true;

        if (am) {
                /*
                 * Turn on command entry mode.
                 */
                flash_activate_command_entry_mode();
        }

        /*
         * Suspend action.
         */
        DBG_SET_HIGH(FLASH_DEBUG, FLASHDBG_SUSPEND_ACTION);

        /*
         * Check if an operation is ongoing.
         */
        status_1 = flash_read_status_register_1();
        while ((status_1 & W25Q_STATUS1_BUSY_MASK) != 0) {
                qspi_write(cmd, 1);

                /*
                 * Check if flash is ready.
                 */
                status_1 = flash_read_status_register_1();
        }

        hw_cpm_delay_usec(FLASH_SUS_DELAY);  // Wait for SUS bit to be updated

        DBG_SET_LOW(FLASH_DEBUG, FLASHDBG_SUSPEND_ACTION);

        status_2 = flash_read_status_register_2();
        if ((status_2 & W25Q_STATUS2_SUS_MASK) == 0) {
                ret = false;
        }

        /*
         * Restore auto mode.
         */
        flash_deactivate_command_entry_mode();

        return ret;
}

__RETAINED_CODE void qspi_resume(void)
{
        uint8_t cmd[] = { W25Q_ERASE_PROGRAM_RESUME };
        __DBG_QSPI_VOLATILE__ uint8_t status_2;

        do {
                /*
                 * Turn on command entry mode.
                 */
                flash_activate_command_entry_mode();

                /*
                 * Check if suspended.
                 */
                status_2 = flash_read_status_register_2();
                if ((status_2 & W25Q_STATUS2_SUS_MASK) == 0) {
                        break;
                }

                /*
                 * Wait for flash to become ready again.
                 */
                do {
                        /*
                         * Resume action.
                         */
                        qspi_write(cmd, 1);

                        /*
                         * Check if SUS bit is cleared.
                         */
                        status_2 = flash_read_status_register_2();
                } while (status_2 & W25Q_STATUS2_SUS_MASK);
        } while (0);

        /*
         * Flash stays in manual mode.
         */
}
#endif

/**
 * \brief Erase sector (via CPM background processing or using the QSPI controller)
 *
 * \details This function will execute a Flash sector erase operation. The operation will either be
 *        carried out immediately (dg_configDISABLE_BACKGROUND_FLASH_OPS is set to 1) or it will be
 *        deferred to be executed by the CPM when the system becomes idle (when
 *        dg_configDISABLE_BACKGROUND_FLASH_OPS is set to 0, default value). In the latter case, the
 *        caller will block until the CPM completes the registered erase operation.
 *
 * \param[in] addr The address of the sector to be erased.
 */
QSPI_SECTION static void qspi_erase_sector(uint32_t addr)
{
#if (dg_configDISABLE_BACKGROUND_FLASH_OPS == 0)
        OS_TASK handle;

        handle = OS_GET_CURRENT_TASK();

        /* Instruct CPM to erase this sector */
        if (pm_register_qspi_operation(handle, addr, NULL, 0)) {
                /* Block until erase is completed */
                OS_TASK_SUSPEND(handle);
        }
        else {
                /* The PM has not started yet... */

#endif
                /* Wait for previous erase to end */
                while (qspi_get_erase_status() != 0) {
                }

                if (qspi_get_address_size() == HW_QSPI_ADDR_SIZE_32) {
                        addr >>= 12;
                } else {
                        addr >>= 4;
                }

                /* Setup erase block page */
                HW_QSPIC_REG_SETF(ERASECTRL, ERS_ADDR, addr);

                /* Fire erase */
                HW_QSPIC_REG_SETF(ERASECTRL, ERASE_EN, 1);
#if (dg_configDISABLE_BACKGROUND_FLASH_OPS == 0)
        }
#endif
}

/**
 * \brief Write data to a page in the Flash (via CPM background processing or using the QSPI
 *        controller)
 *
 * \details This function will execute a Flash program page operation. The operation will either be
 *        carried out immediately (dg_configDISABLE_BACKGROUND_FLASH_OPS is set to 1) or it will be
 *        deferred to be executed by the CPM when the system becomes idle (when
 *        dg_configDISABLE_BACKGROUND_FLASH_OPS is set to 0, default value). In the latter case, the
 *        caller will block until the CPM completes the registered page program operation.
 *
 * \param[in] addr The address of the Flash where the data will be written. It may be anywhere in a
 *        page.
 * \param[in] buf Pointer to the beginning of the buffer that contains the data to be written.
 * \param[in] size The number of bytes to be written.
 *
 * \return size_t The number of bytes written.
 *
 * \warning The caller should ensure that buf does not point to QSPI mapped memory.
 */
QSPI_SECTION_NOINLINE static size_t write_page(uint32_t addr, const uint8_t *buf, uint16_t size)
{
        size_t written;

#if (dg_configDISABLE_BACKGROUND_FLASH_OPS == 0)
        OS_TASK handle;

        handle = OS_GET_CURRENT_TASK();

        /* Instruct CPM to program this sector */
        if (pm_register_qspi_operation(handle, addr, buf, &size)) {
                /* Block until program is completed */
                OS_TASK_SUSPEND(handle);
                written = size;
        }
        else {
#endif
                __DBG_QSPI_VOLATILE__ uint8_t status;

                /*
                 * From now on QSPI may not be available, turn of interrupts.
                 */
                GLOBAL_INT_DISABLE();

                /*
                 * Turn on command entry mode.
                 */
                flash_activate_command_entry_mode();

                /*
                 * Write data into the page of the Flash.
                 */
                written = flash_write_page(addr, buf, size);

                /* Wait for the Flash to process the command */
                do {
                        status = flash_read_status_register_1();
                } while (status & W25Q_STATUS1_BUSY_MASK);

                /*
                 * Restore auto mode.
                 */
                flash_deactivate_command_entry_mode();

                /*
                 * Let other code to be executed including QSPI one.
                 */
                GLOBAL_INT_RESTORE();
#if (dg_configDISABLE_BACKGROUND_FLASH_OPS == 0)
        }
#endif

        return written;
}

/**
 * \brief Erase a sector of the Flash
 *
 * \details The time and the way that the operation will be carried out depends on the following
 *        settings:
 *        ERASE_IN_AUTOMODE = 0: the command is issued immediately in manual mode
 *        ERASE_IN_AUTOMODE = 1:
 *              dg_configDISABLE_BACKGROUND_FLASH_OPS = 0: the operation is executed manually by the
 *                      CPM when the system becomes idle
 *              dg_configDISABLE_BACKGROUND_FLASH_OPS = 1: the operation is executed automatically
 *                      by the QSPI controller.
 *
 * \param[in] addr The address of the sector to be erased.
 */
QSPI_SECTION_NOINLINE static void erase_sector(uint32_t addr)
{
#if ERASE_IN_AUTOMODE
        /*
         * Erase sector in automode
         */
        qspi_erase_sector(addr);

        /*
         * Wait for erase to finish
         */
        while (qspi_get_erase_status()) {
        }
#else
        /*
         * From now on QSPI may not be available, turn of interrupts.
         */
        GLOBAL_INT_DISABLE();

        /*
         * Turn off auto mode to allow write.
         */
        qspi_set_automode(false);

        /*
         * Get the Flash out of Power Down mode.
         */
        flash_release_power_down();

        /*
         * Exit continuous mode, after this the flash will interpret commands again.
         */
        flash_reset_continuous_mode();

        /*
         * Execute erase command.
         */
        flash_erase_sector(addr);

        /*
         * Restore auto mode.
         */
        flash_deactivate_command_entry_mode();

        /*
         * Let other code to be executed including QSPI one.
         */
        GLOBAL_INT_RESTORE();
#endif
}

uint32_t qspi_automode_get_code_buffer_size(void)
{
        /* Return 1 in case some code will pass this value to memory allocation function */
        return 1;
}

void qspi_automode_set_code_buffer(void *ram)
{
        (void) ram; /* Unused */
}

bool qspi_automode_writable(void)
{
        return qspi_writable();
}

size_t qspi_automode_write_flash_page(uint32_t addr, const uint8_t *buf, size_t size)
{
        while (!qspi_automode_writable()) {
        }

        return write_page(addr, buf, size);
}

void qspi_automode_erase_flash_sector(uint32_t addr)
{
        while (!qspi_automode_writable()) {
        }

        erase_sector(addr);
}

size_t qspi_automode_read(uint32_t addr, uint8_t *buf, size_t len)
{
        memcpy(buf, (void *)(MEMORY_QSPIF_BASE + addr), len);

        return len;
}

void qspi_automode_flash_power_up(void)
{
        if (dg_configCODE_LOCATION != NON_VOLATILE_IS_FLASH) {

                /*
                 * Turn on command entry mode.
                 */
                flash_activate_command_entry_mode();

                /*
                 * Release Flash from Power Down mode.
                 */
                flash_release_power_down();

                /*
                 * Restore auto mode.
                 */
                flash_deactivate_command_entry_mode();
        }
}

static const qspi_config qspi_cfg = {
        HW_QSPI_ADDR_SIZE_24, HW_QSPI_POL_HIGH, HW_QSPI_SAMPLING_EDGE_NEGATIVE
};

__RETAINED_CODE bool qspi_automode_init(void)
{
        hw_qspi_enable_clock();
#if __DBG_QSPI_ENABLED
        REG_SETF(CRG_TOP, CLK_AMBA_REG, QSPI_DIV, 3);
#endif

        /*
         * Setup erase instruction that will be sent by QSPI controller to erase sector in automode.
         */
        hw_qspi_set_erase_instruction(W25Q_SECTOR_ERASE, HW_QSPI_BUS_MODE_SINGLE,
                HW_QSPI_BUS_MODE_SINGLE, 8, 5);
        /*
         * Setup instruction pair that will temporarily suspend erase operation to allow read.
         */
        hw_qspi_set_suspend_resume_instructions(W25Q_ERASE_PROGRAM_SUSPEND, HW_QSPI_BUS_MODE_SINGLE,
                                        W25Q_ERASE_PROGRAM_RESUME, HW_QSPI_BUS_MODE_SINGLE, 7);

        /*
         * QSPI controller must send write enable before erase, this sets it up.
         */
        hw_qspi_set_write_enable_instruction(W25Q_WRITE_ENABLE, HW_QSPI_BUS_MODE_SINGLE);

        /*
         * Setup instruction that will be used to periodically check erase operation status.
         * Check LSB which is 1 when erase is in progress.
         */
        hw_qspi_set_read_status_instruction(W25Q_READ_STATUS_REGISTER1, HW_QSPI_BUS_MODE_SINGLE,
                HW_QSPI_BUS_MODE_SINGLE, W25Q_STATUS1_BUSY_BIT, 1, 20, 0);

        /*
         * This sequence is necessary if flash is working in continuous read mode, when instruction
         * is not sent on every read access just address. Sending 0xFFFF will exit this mode.
         * This sequence is sent only when QSPI is working in automode and decides to send one of
         * instructions above.
         * If flash is working in DUAL bus mode sequence should be 0xFFFF and size should be
         * HW_QSPI_BREAK_SEQ_SIZE_2B.
         */
        hw_qspi_set_break_sequence(0xFFFF, HW_QSPI_BUS_MODE_SINGLE, HW_QSPI_BREAK_SEQ_SIZE_1B, 0);

        /*
         * If application starts from FLASH then bootloader must have set read instruction.
         */
        if (dg_configCODE_LOCATION != NON_VOLATILE_IS_FLASH) {
                hw_qspi_init(&qspi_cfg);
                hw_qspi_set_div(HW_QSPI_DIV_1);
        }
        flash_activate_command_entry_mode();
        hw_qspi_set_read_instruction(W25Q_FAST_READ_QUAD, 1, 2, HW_QSPI_BUS_MODE_SINGLE,
                                        HW_QSPI_BUS_MODE_QUAD, HW_QSPI_BUS_MODE_QUAD,
                                        HW_QSPI_BUS_MODE_QUAD);

        hw_qspi_set_extra_byte(0xA0, HW_QSPI_BUS_MODE_QUAD, 0);
        flash_deactivate_command_entry_mode();

        HW_QSPIC_REG_SETF(BURSTCMDB, CS_HIGH_MIN, 0);

        return true;
}
