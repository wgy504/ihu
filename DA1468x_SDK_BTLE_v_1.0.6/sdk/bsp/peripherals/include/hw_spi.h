/**
 * \addtogroup BSP
 * \{
 * \addtogroup DEVICES
 * \{
 * \addtogroup SPI
 * \{
 * \brief SPI Controller
 */

/**
 *****************************************************************************************
 *
 * @file hw_spi.h
 *
 * @brief Definition of API for the SPI Low Level Driver.
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

#ifndef HW_SPI_H_
#define HW_SPI_H_

#if dg_configUSE_HW_SPI

#include <stdint.h>
#include <sdk_defs.h>
#include "hw_dma.h"
#include "hw_gpio.h"

#define HW_SPI_DMA_SUPPORT

/* SPI Base Address */
#define SBA(id)                       ((SPI_Type *)id)

typedef void (*hw_spi_tx_callback)(void *user_data, uint16_t transferred);

/*
 * ENUMERATION DEFINITIONS
 *****************************************************************************************
 */

/**
 * \brief SPI controller number
 *
 */
#define HW_SPI1         ((void *)SPI_BASE)
#define HW_SPI2         ((void *)SPI2_BASE)
typedef void * HW_SPI_ID;

/**
 * \brief Word size
 *
 */
typedef enum {
        HW_SPI_WORD_8BIT,       /**< Only the first SPI RX/TX register is used */
        HW_SPI_WORD_16BIT,      /**< Only the first SPI RX/TX register is used */
        HW_SPI_WORD_32BIT,      /**< Both first and secondary SPI RX/TX registers are used */
        HW_SPI_WORD_9BIT,       /**< Only valid in master mode */
} HW_SPI_WORD;

/**
 * \brief Master/slave mode
 *
 */
typedef enum {
        HW_SPI_MODE_MASTER,
        HW_SPI_MODE_SLAVE,
} HW_SPI_MODE;

/**
 * \brief Clock polarity
 *
 */
typedef enum {
        HW_SPI_POL_LOW,
        HW_SPI_POL_HIGH,
} HW_SPI_POL;

/**
 * \brief Clock phase
 *
 */
typedef enum {
        HW_SPI_PHA_MODE_0,
        HW_SPI_PHA_MODE_1,
} HW_SPI_PHA;

/**
 * \brief Disable/enable interrupts to the CPU
 *
 */
typedef enum {
        HW_SPI_MINT_DISABLE,
        HW_SPI_MINT_ENABLE,
} HW_SPI_MINT;

/**
 * \brief Clock frequency
 *
 */
typedef enum {
        HW_SPI_FREQ_DIV_8,      /**< (XTAL or PLL/2)/(PER_DIV20 *8) */
        HW_SPI_FREQ_DIV_4,      /**< (XTAL or PLL/2)/(PER_DIV20 *4) */
        HW_SPI_FREQ_DIV_2,      /**< (XTAL or PLL/2)/(PER_DIV20 *2) */
        HW_SPI_FREQ_DIV_14,     /**< (XTAL or PLL/2)/(PER_DIV20 *14) */
} HW_SPI_FREQ;

/**
 * \brief FIFO mode
 *
 */
typedef enum {
        HW_SPI_FIFO_RX_TX,      /**< Bidirectional mode */
        HW_SPI_FIFO_RX_ONLY,    /**< Read only mode */
        HW_SPI_FIFO_TX_ONLY,    /**< Write only mode */
        HW_SPI_FIFO_NONE,       /**< Backwards compatible mode */
} HW_SPI_FIFO;

/**
 * \brief SPI chip-select pin definition
 *
 */
typedef struct
{
        HW_GPIO_PORT port;
        HW_GPIO_PIN pin;
} SPI_Pad;

/**
 * \brief SPI configuration
 *
 */
typedef struct
{
        SPI_Pad         cs_pad;
        HW_SPI_WORD     word_mode;
        HW_SPI_MODE     smn_role;
        HW_SPI_POL      polarity_mode;
        HW_SPI_PHA      phase_mode;
        HW_SPI_MINT     mint_mode;
        HW_SPI_FREQ     xtal_freq;
        HW_SPI_FIFO     fifo_mode;
        uint8_t         disabled; // Should SPI be disabled on init
#ifdef HW_SPI_DMA_SUPPORT
        uint8_t         use_dma;
        HW_DMA_CHANNEL  rx_dma_channel:8;
        HW_DMA_CHANNEL  tx_dma_channel:8;
#endif
} spi_config;

//===================== Read/Write functions ===================================

/**
 * \brief Write a value to an SPI register field
 *
 * \param [in] id identifies SPI1 or SPI2
 * \param [in] reg the SPI register
 * \param [in] field the SPI register field
 * \param [in] val value to be written
 *
 * \sa HW_SPI_REG_GETF
 *
 */
#define HW_SPI_REG_SETF(id, reg, field, val) \
        SBA(id)->reg = ((SBA(id)->reg & ~(SPI_##reg##_##field##_Msk)) | \
        ((SPI_##reg##_##field##_Msk) & ((val) << (SPI_##reg##_##field##_Pos))))

/**
 * \brief Get the value of an SPI register field
 *
 * \param [in] id identifies SPI1 or SPI2
 * \param [in] reg the SPI register
 * \param [in] field the SPI register field
 *
 * \sa HW_SPI_REG_SETF
 *
 */
#define HW_SPI_REG_GETF(id, reg, field) \
        ((SBA(id)->reg & (SPI_##reg##_##field##_Msk)) >> (SPI_##reg##_##field##_Pos))

/**
 * \brief Read SPI RX/TX register
 *
 * \param [in] id identifies SPI1 or SPI2
 *
 * \return read data
 *
 */
static inline uint16_t hw_spi_fifo_read16(HW_SPI_ID id)
{
        // Read data from the SPI RX register or RX FIFO
        return SBA(id)->SPI_RX_TX_REG0;
}

/**
 * \brief Write the SPI RX/TX register
 *
 * If FIFO is full undefined data can be sent.
 * This function should be called only when FIFO is not full.
 * Call hw_spi_is_tx_fifo_full() before using this.
 *
 * \param [in] id identifies SPI1 or SPI2
 * \param [in] data data to be written
 *
 * \sa hw_spi_is_tx_fifo_full
 *
 */
static inline void hw_spi_fifo_write16(HW_SPI_ID id, uint16_t data)
{
        SBA(id)->SPI_RX_TX_REG0 = data;
}

/**
 * \brief Read byte from the SPI RX/TX register
 *
 * \param [in] id identifies SPI1 or SPI2
 *
 * \return read byte
 *
 */
static inline uint8_t hw_spi_fifo_read8(HW_SPI_ID id)
{
        // Read byte from the SPI RX register or RX FIFO
        return (uint8_t) SBA(id)->SPI_RX_TX_REG0;
}

/**
 * \brief Write byte to the SPI RX/TX register
 *
 * If FIFO is full undefined data can be sent.
 * This function should be called only when FIFO is not full.
 * Call hw_spi_is_tx_fifo_full() before using this.
 * Use this function in 8 and 9 bit mode.
 *
 * \param [in] id identifies SPI1 or SPI2
 * \param [in] data byte to be written
 *
 * \sa hw_spi_is_tx_fifo_full
 *
 */
static inline void hw_spi_fifo_write8(HW_SPI_ID id, uint8_t data)
{
        // Write byte to the SPI TX register or TX FIFO
        SBA(id)->SPI_RX_TX_REG0 = data;
}

/**
 * \brief Read 32 bits from the SPI
 *
 * Should be used when 32 bit transfer is set.
 *
 * \param [in] id identifies SPI1 or SPI2
 *
 * \return read word
 *
 */
static inline uint32_t hw_spi_fifo_read32(HW_SPI_ID id)
{
        // Make sure SPI_RX_TX_REG1 is read first
        uint32_t res = SBA(id)->SPI_RX_TX_REG1 << 16;
        // Read byte from the SPI RX register or RX FIFO
        return SBA(id)->SPI_RX_TX_REG0 | res;
}

/**
 * \brief Write32 bits to the SPI
 *
 * Should be used when 32 bit transfer is set.
 * If FIFO is full undefined data can be sent.
 * This function should be called only when FIFO is not full.
 *
 * \param [in] id identifies SPI1 or SPI2
 * \param [in] val value to send
 *
 * \sa hw_spi_is_tx_fifo_full
 *
 */
static inline void hw_spi_fifo_write32(HW_SPI_ID id, uint32_t val)
{
        // Make sure SPI_RX_TX_REG1 is written first
        SBA(id)->SPI_RX_TX_REG1 = val >> 16;
        SBA(id)->SPI_RX_TX_REG0 = (uint16_t) val;
}

/**
 * \brief Writes 8, 9 or 16 bits to the SPI
 *
 * Function sends word to SPI and reads back data on same clock.
 * Word size must be setup before.
 * For 9 bits transfer only 8 bits are used/returned.
 * 9th bit is sent according to value setup before.
 * On read 9th bit should be ignored.
 * Data is sent in big-endian mode, MSB goes first.
 *
 * \param [in] id identifies SPI1 or SPI2
 * \param [in] val value to send
 *
 * \return value read from MISO line
 *
 */
uint16_t hw_spi_writeread(HW_SPI_ID id, uint16_t val);

/**
 * \brief Writes 32 bits to the SPI
 *
 * Function sends word to SPI and reads back data on same
 * clock. Word size should be setup before to 32 bits.
 * While it will work correctly for shorter words it is
 * more efficient to use hw_spi_writeread instead.
 * Data is sent in big-endian mode, MSB goes first.
 *
 * \param [in] id identifies SPI1 or SPI2
 * \param [in] val value to send
 *
 * \return value read from MISO line
 *
 * \sa hw_spi_writeread
 *
 */
uint32_t hw_spi_writeread32(HW_SPI_ID id, uint32_t val);

/**
 * \brief Write and reads array of bytes through SPI
 *
 * Initiates SPI transmission, data is sent and received at the same time.
 * If no callback is provided this function wait for transfer to finish.
 * If callback is provided function sets up transfer and ends immediately.
 * In this case data pointed by in_buf and out_buf should not be touched till callback is
 * called.
 *
 * \note If 16 or 32 bits mode is used data is taken from memory in little
 * endian, but transmitted in big endian so stream of bytes send over line
 * is in different order then in memory.
 *
 * \note This function sets FIFO mode to HW_SPI_FIFO_RX_TX.
 * If no callback is provided FIFO mode will be restored to value set before this function was
 * called.
 * If callback is provided, FIFO mode is not restored, it this case user code must change mode
 * manually if needed.
 *
 * \param [in] id identifies SPI1 or SPI2
 * \param [in] out_buf data to send
 * \param [out] in_buf buffer for incoming data
 * \param [in] len data length in bytes (regardless of SPI word size)
 * \param [in] cb callback to call after transfer is finished
 * \param [in] user_data parameter for callback
 *
 */
void hw_spi_writeread_buf(HW_SPI_ID id, const uint8_t *out_buf, uint8_t *in_buf, uint16_t len,
                                                            hw_spi_tx_callback cb, void *user_data);

/**
 * \brief Write array of bytes to SPI
 *
 * Initiates SPI transmission, no data is received (Write only mode)
 * If no callback is provided this function wait for transfer to finish.
 * If callback is provided function sets up transfer and ends immediately.
 * In this case data pointed by buf should not be touched till callback is
 * called.
 *
 * \note If 16 or 32 bits mode is used data is written to memory in little
 * endian, but transmitted in big endian so stream of bytes send over line
 * is in different order then in memory.
 *
 * \note This function sets FIFO mode to HW_SPI_FIFO_TX_ONLY, if no callback
 * is provided FIFO mode will be restored to value set before this function was called.
 * If callback is provided, FIFO mode is not restored, it this case user code
 * must change mode manually if needed.
 *
 * \param [in] id identifies SPI1 or SPI2
 * \param [in] out_buf data to send
 * \param [in] len data length in bytes (regardless of SPI word size)
 * \param [in] cb callback to call after transfer is finished
 * \param [in] user_data parameter for callback
 *
 */
void hw_spi_write_buf(HW_SPI_ID id, const uint8_t *out_buf, uint16_t len,
                                                            hw_spi_tx_callback cb, void *user_data);

/**
 * \brief Reads array of bytes through SPI
 *
 * Initiates SPI read transfer.
 * If no callback is provided this function wait for transfer to finish.
 * If callback is provided function sets up transfer and ends immediately.
 * In this case data pointed by in_buf should not be touched till callback is
 * called.
 *
 * \note If 16 or 32 bits mode is used data is taken from memory in little
 * endian, but transmitted in big endian so stream of bytes send over line
 * is in different order then in memory.
 *
 * \note This function sets FIFO mode to HW_SPI_FIFO_RX_ONLY (slave) or HW_SPI_FIFO_RX_TX (master).
 * If no callback is provided FIFO mode will be restored to value set before this function was
 * called.
 * If callback is provided, FIFO mode is not restored, it this case user code must change mode
 * manually if needed.
 *
 * \param [in] id identifies SPI1 or SPI2
 * \param [out] in_buf buffer for incoming data
 * \param [in] len data length in bytes (regardless of SPI word size)
 * \param [in] cb callback to call after transfer is finished
 * \param [in] user_data parameter for callback
 *
 */
void hw_spi_read_buf(HW_SPI_ID id, uint8_t *in_buf, uint16_t len,
                                                            hw_spi_tx_callback cb, void *user_data);

//============== Interrupt handling ============================================


/**
 * \brief Enables the SPI maskable interrupt (MINT) to the CPU
 *
 * \param [in] id identifies SPI1 or SPI2
 *
 */
static inline void hw_spi_enable_interrupt(HW_SPI_ID id)
{
        // Set MINT bit in SPI control register
        HW_SPI_REG_SETF(id, SPI_CTRL_REG, SPI_MINT, 1);
}

/**
 * \brief Disables the SPI maskable interrupt (MINT) to the CPU
 *
 * \param [in] id identifies SPI1 or SPI2
 *
 */
static inline void hw_spi_disable_interrupt(HW_SPI_ID id)
{
        // Set MINT bit in SPI control register
        HW_SPI_REG_SETF(id, SPI_CTRL_REG, SPI_MINT, 0);
}

/**
 * \brief Get the status of the SPI maskable interrupt (MINT) to the CPU
 *
 * \param [in] id identifies SPI1 or SPI2
 *
 * \return SPI maskable interrupt (MINT) status
 *
 */
HW_SPI_MINT hw_spi_is_interrupt_enabled(HW_SPI_ID id);

/**
 * \brief Get the SPI interrupt bit value
 *
 * \param [in] id identifies SPI1 or SPI2
 *
 * \return the value of the SPI interrupt bit:
 *         0 = RX FIFO or register is empty,
 *         1 = SPI interrupt: Data has been transmitted and received
 *
 */
uint8_t hw_spi_get_interrupt_status(HW_SPI_ID id);

/**
 * \brief Clear the SPI interrupt bit
 *
 * \param [in] id identifies SPI1 or SPI2
 *
 */
void hw_spi_clear_interrupt(HW_SPI_ID id);

//==================== Configuration functions =================================

/**
 * \brief Switch the SPI module on and off
 *
 * \param [in] id identifies SPI1 or SPI2
 * \param [in] on the SPI module switch:
 *                0 = SPI module switched off (power saving). Everything is reset
 *                    except control registers. When this bit is cleared, the SPI
 *                    will remain active in master mode until the shift register
 *                    and holding register are both empty,
 *                1 = SPI module switched on: Should only be set after all control bits
 *                    have the desired values. So two writes are needed!
 *
 */
void hw_spi_enable(HW_SPI_ID id, uint8_t on);

/**
 * \brief Get the on/off status of the SPI module
 *
 * \param [in] id identifies SPI1 or SPI2
 *
 * \return the status of the SPI ON bit:
 *         0 = SPI module switched off,
 *         1 = SPI module switched on
 *
 */
uint8_t hw_spi_is_enabled(HW_SPI_ID id);

/**
 * \brief Set the SPI clock phase
 *
 * \param [in] id identifies SPI1 or SPI2
 * \param [in] phase SPI clock phase
 *
 */
void hw_spi_set_clock_phase(HW_SPI_ID id, HW_SPI_PHA phase);

/**
 * \brief Get the SPI clock phase
 *
 * \param [in] id identifies SPI1 or SPI2
 *
 * \return SPI clock phase
 *
 */
HW_SPI_PHA hw_spi_get_clock_phase(HW_SPI_ID id);

/**
 * \brief Set the SPI clock polarity
 *
 * \param [in] id identifies SPI1 or SPI2
 * \param [in] pol SPI clock polarity
 *
 */
void hw_spi_set_clock_polarity(HW_SPI_ID id, HW_SPI_POL pol);

/**
 * \brief Get the SPI clock polarity
 *
 * \param [in] id identifies SPI1 or SPI2
 *
 * \return SPI clock polarity
 *
 */
HW_SPI_POL hw_spi_get_clock_polarity(HW_SPI_ID id);

/**
 * \brief Set the SPI clock frequency
 *
 * \note SPI core clock 41.472 MHz\@PLL = 165888 MHz gives Max allowed
 *       frequency of SPI clock = 20.736 MHz
 *
 * \param [in] id identifies SPI1 or SPI2
 * \param [in] freq selected SPI clock frequency in master mode
 *
 */
void hw_spi_set_clock_freq(HW_SPI_ID id, HW_SPI_FREQ freq);

/**
 * \brief Get the SPI clock frequency
 *
 * \note SPI core clock 41.472 MHz\@PLL = 165888 MHz gives Max allowed
 *       frequency of SPI clock = 20.736 MHz
 *
 * \param [in] id identifies SPI1 or SPI2
 *
 * \return SPI clock frequency in master mode
 *
 */
HW_SPI_FREQ hw_spi_get_clock_freq(HW_SPI_ID id);

/**
 * \brief Pin SPI DO output level when SPI is idle or when SPI_FORCE_DO = 1
 *
 * \param [in] id identifies SPI1 or SPI2
 * \param [in] spi_do the desired SPI DO output level (0 or 1)
 *
 */
void hw_spi_set_do_level(HW_SPI_ID id, uint8_t spi_do);

/**
 * \brief Get SPI DO output level selected for SPI force DO operation
 *
 * \param [in] id identifies SPI1 or SPI2
 *
 * \return the selected SPI DO output level
 *
 */
uint8_t hw_spi_get_do_level(HW_SPI_ID id);

/**
 * \brief Force SPI DO output level to selected value
 *
 * \param [in] id identifies SPI1 or SPI2
 * \param [in] spi_force_do 0 = Normal operation,
 *                          1 = Force SPI DO output level to the value of SPI_DO bit
 *                              of the control register
 *
 */
void hw_spi_set_force_do(HW_SPI_ID id, uint8_t spi_force_do);

/**
 * \brief Get the SPI force DO bit value
 *
 * \param [in] id identifies SPI1 or SPI2
 *
 * \return the selected SPI force DO bit value
 *
 */
uint8_t hw_spi_get_force_do(HW_SPI_ID id);

/**
 * \brief Set SPI master/slave mode
 *
 * \param [in] id identifies SPI1 or SPI2
 * \param [in] smn SPI mode - master/slave
 *
 */
void hw_spi_set_mode(HW_SPI_ID id, HW_SPI_MODE smn);

/**
 * \brief Get the SPI master/slave mode
 *
 * \param [in] id identifies SPI1 or SPI2
 *
 * \return SPI master/slave mode
 *
 */
HW_SPI_MODE hw_spi_is_slave(HW_SPI_ID id);

/**
 * \brief Set SPI word mode
 *
 * \param [in] id identifies SPI1 or SPI2
 * \param [in] word word length
 *
 */
void hw_spi_set_word_size(HW_SPI_ID id, HW_SPI_WORD word);

/**
 * \brief Get the SPI word mode
 *
 * \param [in] id identifies SPI1 or SPI2
 *
 * \return SPI word mode
 *
 */
HW_SPI_WORD hw_spi_get_word_size(HW_SPI_ID id);

/**
 * \brief Get the SPI word size
 *
 * Returns number of bytes that will be read/written to/from memory.
 *
 * \param [in] id identifies SPI1 or SPI2
 *
 * \return SPI word mode
 *
 */
uint32_t hw_spi_get_memory_word_size(HW_SPI_ID id);

/**
 * \brief Reset SPI module
 *
 * \param [in] id identifies SPI1 or SPI2
 *
 */
void hw_spi_reset(HW_SPI_ID id);

/**
 * \brief Get the value of the SPI TX FIFO full bit
 *
 * \param [in] id identifies SPI1 or SPI2
 *
 * \return SPI TX FIFO full bit:
 *         0 = TX FIFO is not full, data can be written,
 *         1 = TX FIFO is full, data cannot be written
 *
 */
uint8_t hw_spi_is_tx_fifo_full(HW_SPI_ID id);

/**
 * \brief Get the actual value of the SPI DI pin
 *        (delayed with two internal clock cycles).
 *
 * \param [in] id identifies SPI1 or SPI2
 *
 * \return SPI DI pin value
 *
 */
uint8_t hw_spi_get_di(HW_SPI_ID id);

/**
 * \brief Control SPI EN (SPI1 only, master mode)
 *
 * Gate SPI clock with SPI EN (slave mode).
 *
 * \param [in] id identifies SPI1 or SPI2
 * \param [in] spi_en_ctrl 0 = In master mode, SPI EN pin is disabled (SPI1 only)
 *                             in slave mode, SPI clock pin is not gated,
 *                         1 = In master mode, SPI EN pin is enabled (SPI1 only)
 *                             in slave mode, SPI clock pin is active if SPI EN = 0
 *                             and inactive if SPI EN = 1
 *
 */
void hw_spi_set_cs_ctrl(HW_SPI_ID id, uint8_t spi_en_ctrl);

/**
 * \brief Get the value of the SPI EN control bit
 *
 * \param [in] id identifies SPI1 or SPI2
 *
 * \return the SPI_EN_CTRL bit value:
 *         0 = In master mode, SPI EN pin is disabled (SPI1 only)
 *             in slave mode, SPI clock pin is not gated,
 *         1 = In master mode, SPI EN pin is enabled (SPI1 only)
 *             in slave mode, SPI clock pin is active if SPI EN = 0
 *             and inactive if SPI EN = 1
 *
 */
uint8_t hw_spi_get_cs_ctrl(HW_SPI_ID id);

/**
 * \brief Set SPI Chip Select (CS) Pad
 *
 * \param [in] id identifies SPI1 or SPI2
 * \param [in] cs_pad struct SPI_Pad with CS port and pin selection
 *
 */
void hw_spi_set_cs_pad(HW_SPI_ID id, const SPI_Pad *cs_pad);

/**
 * \brief Set value of 9 bit for 9 bits word size
 *
 * \param [in] id identifies SPI1 or SPI2
 * \param [in] val value that will be sent as MSB in 9 bits mode:
 *                 0 = zero will be sent
 *                 1 = one will be sent
 *
 */
void hw_spi_set_9th_bit(HW_SPI_ID id, uint8_t val);

/**
 * \brief Get value currently set as 9 bit for 9 bits word size
 *
 * \param [in] id identifies SPI1 or SPI2
 *
 * \return 0 - if 9ht bit is set to 0,
 *         1 - if 9th bit is set to 1
 *
 */
uint8_t hw_spi_get_9th_bit(HW_SPI_ID id);

/**
 * \brief Initialize the SPI module
 *
 * \note The SPI clock source is set to DIVN (16MHz, regardless of PLL or XTAL16M being used).
 *
 * \param [in] id identifies SPI1 or SPI2
 * \param [in] cfg pointer to SPI configuration struct
 *
 */
void hw_spi_init(HW_SPI_ID id, const spi_config *cfg);

//=========================== CS handling function =============================

/**
 * \brief Set SPI CS low
 *
 * \param [in] id identifies SPI1 or SPI2
 *
 */
void hw_spi_set_cs_low(HW_SPI_ID id);

/**
 * \brief Set SPI CS high
 *
 * \param [in] id identifies SPI1 or SPI2
 *
 */
void hw_spi_set_cs_high(HW_SPI_ID id);

//=========================== FIFO control functions ===========================

/**
 * \brief Set SPI FIFO mode
 *
 * \param [in] id identifies SPI1 or SPI2
 * \param [in] mode SPI FIFO mode
 *
 */
void hw_spi_set_fifo_mode(HW_SPI_ID id, HW_SPI_FIFO mode);

/**
 * \brief Get SPI FIFO mode
 *
 * \param [in] id identifies SPI1 or SPI2
 *
 * \return currently selected SPI FIFO mode
 *
 */
HW_SPI_FIFO hw_spi_get_fifo_mode(HW_SPI_ID id);

/**
 * \brief Change SPI FIFO mode
 *
 * Unlike hw_spi_set_fifo_mode() it checks current FIFO mode and if mode is going to change
 * waits till all data were sent before changing mode.
 * If mode is same, registers are not touched and no waiting is performed.
 *
 * \param [in] id identifies SPI1 or SPI2
 * \param [in] mode SPI FIFO mode
 *
 * \return previously selected SPI FIFO mode
 *
 *  \sa hw_spi_set_fifo_mode
 *
 */
HW_SPI_FIFO hw_spi_change_fifo_mode(HW_SPI_ID id, HW_SPI_FIFO mode);

//=========================== DMA control functions ============================

/**
 * \brief Set up both dma channels for SPI
 *
 * \param [in] id identifies SPI1 or SPI2
 * \param [in] channel rx channel number, tx channel will be rx + 1
 * \param [in] pri DMA priority for both channels
 *
 */
void hw_spi_set_dma_channels(HW_SPI_ID id, int8_t channel, HW_DMA_PRIO pri);

/**
 * \brief Set SPI priority
 *
 * \param [in] id identifies SPI1 or SPI2
 * \param [in] priority 0 = The SPI has low priority, the DMA request signals are reset
 *                          after the corresponding acknowledge,
 *                      1 = The SPI has high priority, DMA request signals remain active
 *                          until the FIFOS are filled/emptied, so the DMA holds the AHB bus
 *
 */
static inline void hw_spi_set_dma_priority(HW_SPI_ID id, uint8_t priority)
{
        HW_SPI_REG_SETF(id, SPI_CTRL_REG1, SPI_PRIORITY, priority);
}

/**
 * \brief Get the currently set SPI priority
 *
 * \param [in] id identifies SPI1 or SPI2
 *
 * \return the currently set priority:
 *         0 = The SPI has low priority, the DMA request signals are reset
 *             after the corresponding acknowledge,
 *         1 = The SPI has high priority, DMA request signals remain active
 *             until the FIFOS are filled/emptied, so the DMA holds the AHB bus
 *
 */
static inline uint8_t hw_spi_get_dma_priority(HW_SPI_ID id)
{
        return HW_SPI_REG_GETF(id, SPI_CTRL_REG1, SPI_PRIORITY);
}

//=========================== Other functions ============================

/**
 * \brief Get SPI busy status
 *
 * \param [in] id identifies SPI1 or SPI2
 *
 * \return status of SPI BUSY bit:
 *         0 = The SPI is not busy with a transfer. This means that either
 *             no TX-data is available or that the transfers have been suspended
 *             due to a full RX-FIFO. The SPI interrupt bit can be used to
 *             distinguish between these situations,
 *         1 = The SPI is busy with a transfer
 *
 */
static inline uint8_t hw_spi_is_busy(HW_SPI_ID id)
{
        // Get the value of the SPI BUSY bit from the secondary SPI control register
        return HW_SPI_REG_GETF(id, SPI_CTRL_REG1, SPI_BUSY);
}

/**
 * \brief Wait till SPI is not busy
 *
 * \param [in] id identifies SPI1 or SPI2
 *
 *
 */
static inline void hw_spi_wait_while_busy(HW_SPI_ID id)
{
        while (hw_spi_is_busy(id)) {
        }
}

#endif /* dg_configUSE_HW_SPI */

#endif /* HW_SPI_H_ */

/**
 * \}
 * \}
 * \}
 */
