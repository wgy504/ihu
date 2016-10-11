/**
 ****************************************************************************************
 *
 * @file flash.c
 *
 * @brief Data flash abstraction layer API
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

/**
 * \addtogroup BSP
 * \{
 * \addtogroup SYSTEM
 * \{
 * \addtogroup Boot-loader
 * \{
 */
#include <stdint.h>
#include <string.h>
#include "hw_qspi.h"
#include "flash.h"


#define WINBOND_ID				0xEF
#define MACRONIX_ID 			0xC2
#define GIGA_ID 				0xC8

#define INST_WREN      			0x06    // FLASH Write Enable
#define INST_WRSR    			0x01    // FLASH Write Status Register
#define INST_RDSR				0x05    // FLASH Read Status Register

#define WND_STATUS_QE         (0x02) 	// Winbond Quad Enable
#define MCX_STATUS_QE         (0x40) 	// Macronix Quad Enable
#define GIG_STATUS_QE	      (0x02)   	// GIGA Quad Enable

//NV end

static const qspi_config qspi_cfg = {
        HW_QSPI_ADDR_SIZE_24, HW_QSPI_POL_HIGH, HW_QSPI_SAMPLING_EDGE_NEGATIVE
};

static uint8_t read_buf[4096]; // Buffer 4 KB

static size_t winbond_qspi_erase_size(void);
static void winbond_qspi_erase(uint32_t addr);
static size_t winbond_qspi_program(uint32_t addr, const uint8_t *buf, size_t len);
static size_t winbond_qspi_read(uint32_t addr, uint8_t *buf, size_t len);
static bool winbond_qspi_init(void);
static void winbond_qspi_chip_erase(void);

/* ops for Winbond W25Q32BV serial flash */
struct flash_ops flash_ops_winbond_qspi = {
        .init       = winbond_qspi_init,
        .erase_size = winbond_qspi_erase_size,
        .erase      = winbond_qspi_erase,
        .program    = winbond_qspi_program,
        .read       = winbond_qspi_read,
        .chip_erase = winbond_qspi_chip_erase,
};

static uint8_t qspi_release_power_down(void)
{
        uint8_t id;

        hw_qspi_cs_enable();
        hw_qspi_write32(0x000000AB);
        id = hw_qspi_read8();
        hw_qspi_cs_disable();

        return id;
}

static void winbond_qspi_exit_continuous_read(void)
{
        hw_qspi_cs_enable();
        hw_qspi_write16(0xFFFF);
        hw_qspi_cs_disable();
}

static uint16_t qspi_read_id(void)
{
        uint16_t id;

        hw_qspi_cs_enable();
        hw_qspi_write32(0x00000090);
        id = hw_qspi_read16();
        hw_qspi_cs_disable();

        return id;
}

static void winbond_qspi_write_enable(void)
{
        hw_qspi_cs_enable();
        hw_qspi_write8(0x06); // Write Enable
        hw_qspi_cs_disable();
}

static void winbond_qspi_wait_while_busy()
{
        hw_qspi_cs_enable();
        hw_qspi_write8(0x05); // Read Status Register-1
        while (hw_qspi_read8() & 0x01); // BUSY bit
        hw_qspi_cs_disable();
}

static bool winbond_qspi_init(void)
{
        uint16_t id;

        hw_qspi_set_automode(false);

        hw_qspi_init(&qspi_cfg);

        hw_qspi_set_div(HW_QSPI_DIV_1);

        hw_qspi_set_bus_mode(HW_QSPI_BUS_MODE_SINGLE);

        hw_qspi_cs_disable();

        winbond_qspi_exit_continuous_read();

        /*Set io2/io3 high for the case flash operates in single SPI mode*/
        hw_qspi_set_io2_output(true);
        hw_qspi_set_io2(1);
        hw_qspi_set_io3_output(true);
        hw_qspi_set_io3(1);
 
        //Release from power down
        qspi_release_power_down();
        id = qspi_read_id();

 
        switch ((id & 0x00ff))
        {
        case  WINBOND_ID:
        	// Write Enable
        	winbond_qspi_write_enable();
        	// CS Low
        	hw_qspi_cs_enable();
        	hw_qspi_write8(0x31); //write status register-2 only
        	hw_qspi_write8(WND_STATUS_QE);
        	// CS High
        	hw_qspi_cs_disable();
        	// Wait end of write
        	winbond_qspi_wait_while_busy();

                // Write Enable
                winbond_qspi_write_enable();
                // CS Low
                hw_qspi_cs_enable();
                hw_qspi_write8(0x1); //Some Winbond devices require to write status register-1 & register-2
                hw_qspi_write8(0);
                hw_qspi_write8(WND_STATUS_QE);
                // CS High
                hw_qspi_cs_disable();
                // Wait end of write
                winbond_qspi_wait_while_busy();

        	break;

        case MACRONIX_ID:
        	// Write Enable
        	winbond_qspi_write_enable();

        	// CS Low
        	hw_qspi_cs_enable();

        	// Write Status Registers
        	hw_qspi_write8(INST_WRSR);
        	hw_qspi_write8(MCX_STATUS_QE);

        	// CS High
        	hw_qspi_cs_disable();

        	// Wait end of write
        	winbond_qspi_wait_while_busy();

        	break;

        case GIGA_ID:
        	// Write Enable
        	winbond_qspi_write_enable();

        	// CS Low
        	hw_qspi_cs_enable();

        	// Write Status Registers
        	hw_qspi_write8(INST_WRSR);
        	hw_qspi_write8(0x0);
                hw_qspi_write8(GIG_STATUS_QE);

        	// CS High
        	hw_qspi_cs_disable();

        	// Wait end of write
        	winbond_qspi_wait_while_busy();

        	break;
        }

        /*
         * If id == 0 flash did not power powered up (should not happen)
         * If id == 0xFFFF flash is not probably connected at since it does not responded
         */
        return id != 0 && id != 0xFFFF;
}

static size_t winbond_qspi_erase_size(void)
{
        return 0x1000; // 4KB
}

static void winbond_qspi_erase(uint32_t addr)
{
        addr &= 0x00FFF000;

        winbond_qspi_write_enable();
        hw_qspi_cs_enable();
        hw_qspi_write8(0x20); // Sector Erase (4KB)
        hw_qspi_write8((uint8_t) (addr >> 16));
        hw_qspi_write8((uint8_t) (addr >>  8));
        hw_qspi_write8((uint8_t) (addr >>  0));
        hw_qspi_cs_disable();
        winbond_qspi_wait_while_busy();
}

static size_t winbond_qspi_program(uint32_t addr, const uint8_t *buf, size_t len)
{
        size_t written = 0;

        if (!len) {
                return 0;
        }

        winbond_qspi_write_enable();
        hw_qspi_cs_enable();
        hw_qspi_write8(0x02); // Page Program
        hw_qspi_write8((uint8_t) (addr >> 16));
        hw_qspi_write8((uint8_t) (addr >>  8));
        hw_qspi_write8((uint8_t) (addr >>  0));
        do {
                hw_qspi_write8(*(buf + written));
                written++;
                addr++;
        } while ((addr & 0xFF) && (written < len));
        hw_qspi_cs_disable();
        winbond_qspi_wait_while_busy();

        return written;
}

static size_t winbond_qspi_read(uint32_t addr, uint8_t *buf, size_t len)
{
        size_t read = 0;

        if (!len) {
                return 0;
        }

        hw_qspi_cs_enable();
        hw_qspi_write8(0x0B); // Fast Read
        hw_qspi_write8((uint8_t) (addr >> 16));
        hw_qspi_write8((uint8_t) (addr >>  8));
        hw_qspi_write8((uint8_t) (addr >>  0));
        hw_qspi_dummy8();
        do {
                buf[read] = hw_qspi_read8();
                read++;
        } while (read < len);
        hw_qspi_cs_disable();

        return read;
}

static void winbond_qspi_chip_erase(void)
{
        winbond_qspi_write_enable();
        hw_qspi_cs_enable();
        hw_qspi_write8(0x60); // Chip erase
        hw_qspi_cs_disable();
        winbond_qspi_wait_while_busy();
}

bool flash_init(struct flash_ops *ops)
{
        return ops->init();
}

bool flash_copy_from_ram(const uint8_t *ptr, size_t len, uint32_t addr, struct flash_ops *ops)
{
        const uint8_t *p = ptr;
        const uint8_t *b;
        size_t erase_mask = ops->erase_size() - 1;
        size_t sector_start;
        uint16_t i;

        while (len) {
                size_t chunk;
                size_t offset;
                size_t write_size;
                size_t write_addr;

                sector_start = addr & ~erase_mask;
                offset = addr - sector_start;

                chunk = ops->erase_size() - offset;
                if (chunk > len) {
                        chunk = len;
                }

                /* Read whole sector first */
                ops->read(sector_start, read_buf, ops->erase_size());

                /* Check if bytes can be written without erase */
                for (i = 0; i < chunk; i++) {
                        if ((read_buf[i + offset] & p[i]) != p[i]) {
                                /* Erase is needed - cannot write this byte */
                                ops->erase(sector_start);
                                break;
                        }
                }

                /*
                 * If erase was needed, update sector data with caller data and write
                 * whole sector instead of just what caller requested.
                 */
                if (i < chunk) {
                        memcpy(read_buf + offset, p, chunk);
                        write_addr = sector_start;
                        write_size = ops->erase_size();
                        b = read_buf; /* Use sector buffer */
                } else {
                        write_addr = addr;
                        write_size = chunk;
                        b = p; /* Use caller's buffer */
                }

                /* Write up to one sector of data */
                while (write_size > 0) {
                        /* write one page at a time */
                        size_t written = ops->program(write_addr, b, write_size);
                        write_addr += written;
                        write_size -= written;
                        b += written;
                }

                addr += chunk;
                p += chunk;
                len -= chunk;
        }

        return true;
}

bool flash_copy_to_ram(uint32_t addr, uint8_t *ptr, size_t len, struct flash_ops *ops)
{
        size_t read;

        read = ops->read(addr, ptr, len);

        return (read == len);
}

bool flash_erase_region(uint32_t addr, size_t len, struct flash_ops *ops)
{
        uint32_t end_addr = addr + len;
        size_t erase_mask = ~(ops->erase_size() - 1);

        addr &= erase_mask;

        while (addr < end_addr) {
                ops->erase(addr);
                addr += ops->erase_size();
        }

        return true;
}

bool flash_chip_erase(struct flash_ops *ops)
{
        ops->chip_erase();

        return true;
}

/*
 * \}
 * \}
 * \}
 */
