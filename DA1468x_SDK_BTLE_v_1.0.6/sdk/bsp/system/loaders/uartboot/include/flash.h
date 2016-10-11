/**
 * \addtogroup BSP
 * \{
 * \addtogroup SYSTEM
 * \{
 * \addtogroup Boot-loader
 * \{
 */

/**
 ****************************************************************************************
 *
 * @file flash.h
 *
 * @brief Data flash abstraction layer implementation
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

#ifndef FLASH_H_
#define FLASH_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * \brief Container for handlers for data flash
 */
struct flash_ops {
        /**
         * \brief Initialize flash
         *
         * Initialize flash. Checks if correct flash is responding.
         *
         * \return true if flash is ready to be used
         */
        bool (*init) (void);

        /**
         * \brief Length of block which can be erased at once
         *
         * Block length should be power of 2 and it's assumed that blocks addressing starts from 0
         * and increases in given size, i.e. with block length of 4096 subsequent block addresses
         * are 0, 4096, 8192 and so on.
         *
         * \return block length
         */
        size_t (*erase_size) (void);

        /**
         * \brief Erase block at given address
         *
         * Handler should not assume \p addr is at the beginning of block and should simply erase
         * block which contains specified address.
         *
         * \param [in] addr block address
         *
         */
        void (*erase) (uint32_t addr);

        /**
         * \brief Program data to data flash
         *
         * Less than \p length data can be written in case programming block boundary is reached.
         *
         * \param [in] addr address in data flash
         * \param [in] buf data to be written
         * \param [in] length length of data
         *
         * \return number of bytes written
         *
         */
        size_t (*program) (uint32_t addr, const uint8_t *buf, size_t length);

        /**
         * \brief Read data from flash
         *
         * \param [in] addr address in data flash
         * \param [out] buf buffer for data
         * \param [in] length length of buffer
         *
         * \return number of bytes read
         *
         */
        size_t (*read) (uint32_t addr, uint8_t *buf, size_t length);

        /**
         * \brief Erase chip
         *
         * Erase whole flash memory.
         *
         */
        void (*chip_erase) (void);
};

extern struct flash_ops flash_ops_winbond_qspi;

/**
 * \brief Initialize flash
 *
 * \return true on success, false otherwise
 */
bool flash_init(struct flash_ops *ops);

/**
 * \brief Write RAM region to flash memory
 *
 * All blocks in data flash which include given region will be erased before writing.

 * \param [in] ptr pointer to source memory
 * \param [in] len length of memory region
 * \param [in] addr address in data flash
 * \param [in] ops handlers for data flash
 *
 * \return true on success, false otherwise
 *
 */
bool flash_copy_from_ram(const uint8_t *ptr, size_t len, uint32_t addr, struct flash_ops *ops);

/**
 * \brief Read data from flash memory to RAM
 *
 * \param [in] addr address in data flash
 * \param [in] ptr pointer to destination memory
 * \param [in] len length of memory region
 * \param [in] ops handlers for data flash
 *
 * \return true on success, false otherwise
 *
 */
bool flash_copy_to_ram(uint32_t addr, uint8_t *ptr, size_t len, struct flash_ops *ops);

/**
 * \brief Erase flash region
 *
 * All blocks which include given region will be erased, but \p addr does not need to be on erase
 * block boundary.
 *
 * \param [in] addr region address in data flash
 * \param [in] len length of memory region
 * \param [in] ops handlers for data flash
 *
 * \return true on success, false otherwise
 *
 */
bool flash_erase_region(uint32_t addr, size_t len, struct flash_ops *ops);

/**
 * \brief Chip erase
 *
 * \param [in] ops handlers for data flash
 *
 * \return true on success, false otherwise
 */
bool flash_chip_erase(struct flash_ops *ops);

#endif /* FLASH_H_ */

/**
 * \}
 * \}
 * \}
 */

