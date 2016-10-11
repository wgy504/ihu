/**
 * \addtogroup BSP
 * \{
 * \addtogroup DEVICES
 * \{
 * \addtogroup SPI
 * \{
 */

/*
 *****************************************************************************************
 *
 * @file hw_spi.c
 *
 * @brief Implementation of the SPI Low Level Driver.
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

#if dg_configUSE_HW_SPI


#include <stdint.h>
#include <hw_spi.h>

#if (dg_configSYSTEMVIEW)
#  include "SEGGER_SYSVIEW_FreeRTOS.h"
#else
#  define SEGGER_SYSTEMVIEW_ISR_ENTER()
#  define SEGGER_SYSTEMVIEW_ISR_EXIT()
#endif

typedef enum {
        HW_SPI_TRANSFER_READ      = 1,
        HW_SPI_TRANSFER_WRITE     = 2,
        HW_SPI_TRANSFER_READWRITE = 3,
} HW_SPI_TRANSFER;

typedef struct
{
        SPI_Pad            cs_pad;
        hw_spi_tx_callback rx_cb;
        hw_spi_tx_callback tx_cb;
        void               *cb_data;

        const uint8_t      *tx_buffer;
        uint16_t           tx_len;
        uint16_t           tx_ix;

        uint8_t            *rx_buffer;
        uint16_t           rx_len;
        uint16_t           rx_ix;

        HW_SPI_TRANSFER    transfer_mode;
#ifdef HW_SPI_DMA_SUPPORT
        uint8_t            use_dma;
        DMA_setup          tx_dma;
        DMA_setup          rx_dma;
#endif
} SPI_Data;

/* SPI data are not retained. The user must ensure that they are updated after exiting sleep. */
static SPI_Data spi_data[2];

#define SPI_INT(id)  ((id) == HW_SPI1 ? (SPI_IRQn) : (SPI2_IRQn))
#define SPIIX(id)    ((id) == HW_SPI1 ? 0 : 1)
#define SPIDATA(id)  (&spi_data[SPIIX(id)])

//==================== Configuration functions =================================

uint8_t hw_spi_is_interrupt_enabled(HW_SPI_ID id)
{
        // Get MINT bit from SPI control register
        return HW_SPI_REG_GETF(id, SPI_CTRL_REG, SPI_MINT);
}

uint8_t hw_spi_get_interrupt_status(HW_SPI_ID id)
{
        // Get SPI interrupt bit from SPI control register
        return HW_SPI_REG_GETF(id, SPI_CTRL_REG, SPI_INT_BIT);
}

void hw_spi_clear_interrupt(HW_SPI_ID id)
{
        // Write 1 to SPI Clear Interrupt Register
        SBA(id)->SPI_CLEAR_INT_REG = 1;
}

void hw_spi_enable(HW_SPI_ID id, uint8_t on)
{
        // Set SPI ON bit in SPI control register
        HW_SPI_REG_SETF(id, SPI_CTRL_REG, SPI_ON, on);
}

uint8_t hw_spi_is_enabled(HW_SPI_ID id)
{
        // Get SPI ON bit from SPI control register
        return HW_SPI_REG_GETF(id, SPI_CTRL_REG, SPI_ON);
}

void hw_spi_set_clock_phase(HW_SPI_ID id, HW_SPI_PHA phase)
{
        uint32_t on = hw_spi_is_enabled(id);
        hw_spi_enable(id, 0);                                   // close SPI block
        HW_SPI_REG_SETF(id, SPI_CTRL_REG, SPI_PHA, phase);      // set SPI PHA bit
        hw_spi_enable(id, on);                                  // open SPI block
}

uint8_t hw_spi_get_clock_phase(HW_SPI_ID id)
{
        // Get SPI clock phase bit from SPI control register
        return HW_SPI_REG_GETF(id, SPI_CTRL_REG, SPI_PHA);
}

void hw_spi_set_clock_polarity(HW_SPI_ID id, HW_SPI_POL pol)
{
        uint32_t on = hw_spi_is_enabled(id);
        hw_spi_enable(id, 0);                                   // close SPI block
        HW_SPI_REG_SETF(id, SPI_CTRL_REG, SPI_POL, pol);        // set SPI POL bit
        hw_spi_enable(id, on);                                  // open SPI block
}

uint8_t hw_spi_get_clock_polarity(HW_SPI_ID id)
{
        // Get SPI clock polarity bit from SPI control register
        return HW_SPI_REG_GETF(id, SPI_CTRL_REG, SPI_POL);
}

void hw_spi_set_clock_freq(HW_SPI_ID id, HW_SPI_FREQ freq)
{
        uint32_t on = hw_spi_is_enabled(id);
        hw_spi_enable(id, 0);                                   // close SPI block
        HW_SPI_REG_SETF(id, SPI_CTRL_REG, SPI_CLK, freq);       // set frequency divider bit
        hw_spi_enable(id, on);                                  // open SPI block
}

uint8_t hw_spi_get_clock_freq(HW_SPI_ID id)
{
        // Get SPI clock frequency field from SPI control register
        return HW_SPI_REG_GETF(id, SPI_CTRL_REG, SPI_CLK);
}

void hw_spi_set_do_level(HW_SPI_ID id, uint8_t spi_do)
{
        uint32_t on = hw_spi_is_enabled(id);
        hw_spi_enable(id, 0);                                   // close SPI block
        HW_SPI_REG_SETF(id, SPI_CTRL_REG, SPI_DO, spi_do);      // set SPI DO output level
        hw_spi_enable(id, on);                                  // open SPI block
}

uint8_t hw_spi_get_do_level(HW_SPI_ID id)
{
        // Get SPI DO output level from SPI control register
        return HW_SPI_REG_GETF(id, SPI_CTRL_REG, SPI_DO);
}

void hw_spi_set_force_do(HW_SPI_ID id, uint8_t spi_force_do)
{
        uint32_t on = hw_spi_is_enabled(id);
        hw_spi_enable(id, 0);                                           // close SPI block
        HW_SPI_REG_SETF(id, SPI_CTRL_REG, SPI_FORCE_DO, spi_force_do);  // set SPI force DO bit
        hw_spi_enable(id, on);                                          // open SPI block
}

uint8_t hw_spi_get_force_do(HW_SPI_ID id)
{
        // Get value of SPI force DO bit from SPI control register
        return HW_SPI_REG_GETF(id, SPI_CTRL_REG, SPI_FORCE_DO);
}

void hw_spi_set_mode(HW_SPI_ID id, HW_SPI_MODE smn)
{
        uint32_t on = hw_spi_is_enabled(id);
        hw_spi_enable(id, 0);                                   // close SPI block
        HW_SPI_REG_SETF(id, SPI_CTRL_REG, SPI_SMN, smn);        // set SMN bit
        hw_spi_enable(id, on);                                  // open SPI block
}

uint8_t hw_spi_is_slave(HW_SPI_ID id)
{
        // Get value of SPI master/slave mode from SPI control register
        return HW_SPI_REG_GETF(id, SPI_CTRL_REG, SPI_SMN);
}

void hw_spi_set_word_size(HW_SPI_ID id, HW_SPI_WORD word)
{
        uint32_t on = hw_spi_is_enabled(id);
        hw_spi_enable(id, 0);                                   // close SPI block
        HW_SPI_REG_SETF(id, SPI_CTRL_REG, SPI_WORD, word);      // set SPI WORD bit
        hw_spi_enable(id, on);                                  // open SPI block
}

void hw_spi_set_9th_bit(HW_SPI_ID id, uint8_t val)
{
        // Get value of the SPI_EN_CTRL bit from the SPI control register
        HW_SPI_REG_SETF(id, SPI_CTRL_REG1, SPI_9BIT_VAL, val);
}

uint8_t hw_spi_get_9th_bit(HW_SPI_ID id)
{
        // Get value of the SPI_EN_CTRL bit from the SPI control register
        return HW_SPI_REG_GETF(id, SPI_CTRL_REG1, SPI_9BIT_VAL);
}

HW_SPI_WORD hw_spi_get_word_size(HW_SPI_ID id)
{
        // Get value of SPI master/slave mode from SPI control register
        return HW_SPI_REG_GETF(id, SPI_CTRL_REG, SPI_WORD);
}

uint32_t hw_spi_get_memory_word_size(HW_SPI_ID id)
{
        switch (hw_spi_get_word_size(id)) {
        case HW_SPI_WORD_16BIT:
                return 2;
        case HW_SPI_WORD_32BIT:
                return 4;
        case HW_SPI_WORD_8BIT:
        case HW_SPI_WORD_9BIT:
        default:
                return 1;
        }
}

void hw_spi_reset(HW_SPI_ID id)
{
        // Set reset bit in SPI control register
        HW_SPI_REG_SETF(id, SPI_CTRL_REG, SPI_RST, 1);
}

uint8_t hw_spi_is_tx_fifo_full(HW_SPI_ID id)
{
        // Get value of the SPI TX FIFO full bit from the SPI control register
        return HW_SPI_REG_GETF(id, SPI_CTRL_REG, SPI_TXH);
}

uint8_t hw_spi_get_di(HW_SPI_ID id)
{
        // Get value of the SPI DI pin
        return HW_SPI_REG_GETF(id, SPI_CTRL_REG, SPI_DI);
}

void hw_spi_set_cs_ctrl(HW_SPI_ID id, uint8_t spi_en_ctrl)
{
        uint32_t on = hw_spi_is_enabled(id);
        hw_spi_enable(id, 0);                                           // close SPI block
        HW_SPI_REG_SETF(id, SPI_CTRL_REG, SPI_EN_CTRL, spi_en_ctrl);    // set SPI WORD bit
        hw_spi_enable(id, on);                                          // open SPI block
}

uint8_t hw_spi_get_cs_ctrl(HW_SPI_ID id)
{
        // Get value of the SPI_EN_CTRL bit from the SPI control register
        return HW_SPI_REG_GETF(id, SPI_CTRL_REG, SPI_EN_CTRL);
}

void hw_spi_set_cs_pad(HW_SPI_ID id, const SPI_Pad *cs_pad)
{
        SPI_Data *spid = SPIDATA(id);

        spid->cs_pad.port = cs_pad->port;
        spid->cs_pad.pin = cs_pad->pin;
}

void hw_spi_init(HW_SPI_ID id, const spi_config *cfg)
{
        SPI_Data *spid = SPIDATA(id);

        GLOBAL_INT_DISABLE();
        uint32_t clk_per_reg_local = CRG_PER->CLK_PER_REG;
        REG_SET_FIELD(CRG_PER, CLK_PER_REG, SPI_CLK_SEL, clk_per_reg_local, 0); // select SPI clock
        REG_SET_FIELD(CRG_PER, CLK_PER_REG, SPI_ENABLE, clk_per_reg_local, 1);  // enable SPI clock
        CRG_PER->CLK_PER_REG = clk_per_reg_local;
        GLOBAL_INT_RESTORE();

        // close SPI block, if opened
        hw_spi_enable(id, 0);

        uint16_t ctrl_reg = 
                ((cfg->word_mode << SPI_SPI_CTRL_REG_SPI_WORD_Pos) & SPI_SPI_CTRL_REG_SPI_WORD_Msk) |
                ((cfg->smn_role << SPI_SPI_CTRL_REG_SPI_SMN_Pos) & SPI_SPI_CTRL_REG_SPI_SMN_Msk) |
                ((cfg->polarity_mode << SPI_SPI_CTRL_REG_SPI_POL_Pos) & SPI_SPI_CTRL_REG_SPI_POL_Msk) |
                ((cfg->phase_mode << SPI_SPI_CTRL_REG_SPI_PHA_Pos) & SPI_SPI_CTRL_REG_SPI_PHA_Msk) |
                ((cfg->mint_mode << SPI_SPI_CTRL_REG_SPI_MINT_Pos) & SPI_SPI_CTRL_REG_SPI_MINT_Msk) |
                ((cfg->xtal_freq << SPI_SPI_CTRL_REG_SPI_CLK_Pos) & SPI_SPI_CTRL_REG_SPI_CLK_Msk);
        SBA(id)->SPI_CTRL_REG = ctrl_reg;

        // set FIFO mode
        HW_SPI_REG_SETF(id, SPI_CTRL_REG1, SPI_FIFO_MODE, cfg->fifo_mode);
        // enable SPI block (if needed)
        hw_spi_enable(id, cfg->disabled ? 0 : 1);
        // set SPI CS pad
        spid->cs_pad.port = cfg->cs_pad.port;
        spid->cs_pad.pin = cfg->cs_pad.pin;
#ifdef HW_SPI_DMA_SUPPORT
        spid->use_dma = cfg->use_dma;
        if (spid->use_dma) {
                hw_spi_set_dma_channels(id, cfg->rx_dma_channel, HW_DMA_PRIO_2);
        }
#endif
}

//=========================== CS handling function =============================

void hw_spi_set_cs_low(HW_SPI_ID id)
{
        SPI_Data *spid = SPIDATA(id);

        hw_gpio_set_inactive(spid->cs_pad.port, spid->cs_pad.pin);    // pull CS low
}

void hw_spi_set_cs_high(HW_SPI_ID id)
{
        SPI_Data *spid = SPIDATA(id);

        hw_gpio_set_active(spid->cs_pad.port, spid->cs_pad.pin);    // push CS high
}

//=========================== FIFO control functions ===========================

void hw_spi_set_fifo_mode(HW_SPI_ID id, HW_SPI_FIFO mode)
{
        uint32_t on = hw_spi_is_enabled(id);
        hw_spi_enable(id, 0);                                           // close SPI block
        HW_SPI_REG_SETF(id, SPI_CTRL_REG1, SPI_FIFO_MODE, mode);        // set SPI FIFO bit
        hw_spi_enable(id, on);                                          // open SPI block
}

HW_SPI_FIFO hw_spi_get_fifo_mode(HW_SPI_ID id)
{
        // Get the SPI FIFO mode from the secondary SPI control register
        return HW_SPI_REG_GETF(id, SPI_CTRL_REG1, SPI_FIFO_MODE);
}

HW_SPI_FIFO hw_spi_change_fifo_mode(HW_SPI_ID id, HW_SPI_FIFO mode)
{
        HW_SPI_FIFO old_mode = hw_spi_get_fifo_mode(id);

        if (old_mode != mode) {
                if (old_mode != HW_SPI_FIFO_RX_ONLY || !hw_spi_is_slave(id)) {
                        hw_spi_wait_while_busy(id);
                }
                hw_spi_set_fifo_mode(id, mode);
        }

        return old_mode;
}

//=========================== DMA control functions ============================

#ifdef HW_SPI_DMA_SUPPORT

static void hw_spi_rx_dma_callback(void *user_data, uint16_t len)
{
        SPI_Data *spid = user_data;
        hw_spi_tx_callback cb = spid->rx_cb;

        spid->rx_cb = NULL;
        spid->rx_ix = len;
        spid->tx_ix = len;
        if (cb) {
                cb(spid->cb_data, len);
        }
}

static void hw_spi_tx_dma_callback(void *user_data, uint16_t len)
{
        SPI_Data *spid = user_data;
        hw_spi_tx_callback cb = spid->tx_cb;

        spid->tx_cb = NULL;
        spid->tx_ix = len;
        spid->tx_ix = len;
        if (cb) {
                cb(spid->cb_data, len);
        }
}

void hw_spi_set_dma_channels(HW_SPI_ID id, int8_t channel, HW_DMA_PRIO pri)
{
        SPI_Data *spid = SPIDATA(id);
        uint16_t wordsize = hw_spi_get_memory_word_size(id);

        /* Make sure the channel is valid or -1 (no DMA) */
        ASSERT_ERROR(channel < 0 ||
                channel == HW_DMA_CHANNEL_0 ||
                channel == HW_DMA_CHANNEL_2 ||
                channel == HW_DMA_CHANNEL_4 ||
                channel == HW_DMA_CHANNEL_6);

        if (channel < 0 || wordsize > 2) {
                spid->use_dma = 0;
                spid->rx_dma.channel_number = 0;
                spid->tx_dma.channel_number = 0;
        } else {
                spid->use_dma = 1;

                spid->rx_dma.channel_number = channel;
                spid->rx_dma.bus_width = wordsize == 1 ? HW_DMA_BW_BYTE : HW_DMA_BW_HALFWORD;
                spid->rx_dma.irq_enable = HW_DMA_IRQ_STATE_ENABLED;
                spid->rx_dma.dma_req_mux = SPIIX(id) == 0 ? HW_DMA_TRIG_SPI_RXTX :
                                                                        HW_DMA_TRIG_SPI2_RXTX;
                spid->rx_dma.irq_nr_of_trans = 0;
                spid->rx_dma.a_inc = HW_DMA_AINC_FALSE;
                spid->rx_dma.b_inc = HW_DMA_BINC_TRUE;
                spid->rx_dma.circular = HW_DMA_MODE_NORMAL;
                spid->rx_dma.dma_prio = pri;
                spid->rx_dma.dma_idle = HW_DMA_IDLE_INTERRUPTING_MODE; /* Not used by the HW in this case */
                spid->rx_dma.dma_init = HW_DMA_INIT_AX_BX_AY_BY;
                spid->rx_dma.dreq_mode = HW_DMA_DREQ_TRIGGERED;
                spid->rx_dma.src_address = (uint32_t)&SBA(id)->SPI_RX_TX_REG0;
                spid->rx_dma.dest_address = 0;  // Change during transmission
                spid->rx_dma.length = 0;       // Change during transmission
                spid->rx_dma.callback = hw_spi_rx_dma_callback;
                spid->rx_dma.user_data = spid;

                spid->tx_dma.channel_number = channel + 1;
                spid->tx_dma.bus_width = wordsize == 1 ? HW_DMA_BW_BYTE : HW_DMA_BW_HALFWORD;
                spid->tx_dma.irq_enable = HW_DMA_IRQ_STATE_ENABLED;
                spid->tx_dma.dma_req_mux = SPIIX(id) == 0 ? HW_DMA_TRIG_SPI_RXTX :
                                                                        HW_DMA_TRIG_SPI2_RXTX;
                spid->tx_dma.irq_nr_of_trans = 0;
                spid->tx_dma.a_inc = HW_DMA_AINC_TRUE;
                spid->tx_dma.b_inc = HW_DMA_BINC_FALSE;
                spid->tx_dma.circular = HW_DMA_MODE_NORMAL;
                spid->tx_dma.dma_prio = pri;
                spid->tx_dma.dma_idle = HW_DMA_IDLE_INTERRUPTING_MODE; /* Not used by the HW in this case */
                spid->tx_dma.dma_init = HW_DMA_INIT_AX_BX_AY_BY;
                spid->tx_dma.dreq_mode = HW_DMA_DREQ_TRIGGERED;
                spid->tx_dma.src_address = 0; // Change during transmission
                spid->tx_dma.dest_address = (uint32_t)&SBA(id)->SPI_RX_TX_REG0;
                spid->tx_dma.length = 0;     // Change during transmission
                spid->tx_dma.callback = hw_spi_tx_dma_callback;
                spid->tx_dma.user_data = spid;
        }
}
#endif

//===================== Read/Write functions ===================================

uint16_t hw_spi_writeread(HW_SPI_ID id, uint16_t val)
{
        uint16_t v;
        hw_spi_fifo_write16(id, val);
        while (!hw_spi_get_interrupt_status(id)) {
        }
        v = hw_spi_fifo_read16(id);
        hw_spi_clear_interrupt(id);
        return v;
}

uint32_t hw_spi_writeread32(HW_SPI_ID id, uint32_t val)
{
        uint32_t v;
        hw_spi_fifo_write32(id, val);
        while (!hw_spi_get_interrupt_status(id)) {
        }
        v = hw_spi_fifo_read32(id);
        hw_spi_clear_interrupt(id);
        return v;
}

static void hw_spi_read_word(HW_SPI_ID id, uint8_t *buf, uint32_t wordsize)
{
        switch (wordsize) {
        case 1:
                *buf = hw_spi_fifo_read8(id);
                break;
        case 2:
                *(uint16_t *) buf = hw_spi_fifo_read16(id);
                break;
        case 4:
                *(uint32_t *) buf = hw_spi_fifo_read32(id);
                break;
        }
}

static void hw_spi_write_word(HW_SPI_ID id, const uint8_t *buf, uint32_t wordsize)
{
        switch (wordsize) {
        case 1:
                hw_spi_fifo_write8(id, *buf);
                break;
        case 2:
                hw_spi_fifo_write16(id, *(uint16_t *) buf);
                break;
        case 4:
                hw_spi_fifo_write32(id, *(uint32_t *) buf);
                break;
        }
}

static void hw_spi_transfer_write(HW_SPI_ID id)
{
        SPI_Data *spid = SPIDATA(id);
        uint32_t wordsize = hw_spi_get_memory_word_size(id);
        const uint8_t *txbuf = &spid->tx_buffer[spid->tx_ix];

        // Write output FIFO
        while (spid->tx_ix < spid->tx_len) {
                if (hw_spi_is_tx_fifo_full(id)) {
                        hw_spi_clear_interrupt(id);
                        break;
                }

                hw_spi_write_word(id, txbuf, wordsize);
                spid->tx_ix += wordsize;
                txbuf += wordsize;
        }
}

/* Non-cached, non-retained global. */
static volatile uint32_t hw_spi_transfer_read_dummy;

static void hw_spi_transfer_read(HW_SPI_ID id)
{
        SPI_Data *spid = SPIDATA(id);
        uint32_t wordsize = hw_spi_get_memory_word_size(id);
        uint8_t *rxbuf = &spid->rx_buffer[spid->rx_ix];
        hw_spi_transfer_read_dummy = 0xFFFFFFFF;

        // Read input FIFO
        while (spid->rx_ix < spid->rx_len) {
                if (!hw_spi_get_interrupt_status(id))
                        break;

                hw_spi_read_word(id, rxbuf, wordsize);
                spid->rx_ix += wordsize;
                rxbuf += wordsize;
                hw_spi_clear_interrupt(id);
        }

        // This is for slave only where clock is provided by master
        // and not writes are needed
        if (hw_spi_get_fifo_mode(id) == HW_SPI_FIFO_RX_ONLY) {
                return;
        }

        // Write output FIFO with 0xFF (rx_len is intentional here)
        while (spid->tx_ix < spid->rx_len) {
                if (hw_spi_is_tx_fifo_full(id)) {
                        break;
                }

                hw_spi_write_word(id, (uint8_t *) &hw_spi_transfer_read_dummy, wordsize);
                spid->tx_ix += wordsize;
        }
}

static void hw_spi_transfer(HW_SPI_ID id)
{
        SPI_Data *spid = SPIDATA(id);
        uint32_t wordsize = hw_spi_get_memory_word_size(id);
        uint8_t *rxbuf = &spid->rx_buffer[spid->rx_ix];
        const uint8_t *txbuf = &spid->tx_buffer[spid->tx_ix];

        // Read input FIFO
        while (spid->rx_ix < spid->rx_len) {
                if (!hw_spi_get_interrupt_status(id)) {
                        break;
                }

                hw_spi_read_word(id, rxbuf, wordsize);
                spid->rx_ix += wordsize;
                rxbuf += wordsize;
                hw_spi_clear_interrupt(id);
        }

        // This is for slave only where clock is provided by master
        // and no writes are needed
        if (hw_spi_get_fifo_mode(id) == HW_SPI_FIFO_RX_ONLY) {
                return;
        }

        // Write output FIFO
        while (spid->tx_ix < spid->tx_len) {
                if (hw_spi_is_tx_fifo_full(id))
                        break;

                hw_spi_write_word(id, txbuf, wordsize);
                spid->tx_ix += wordsize;
                txbuf += wordsize;
        }
}

void hw_spi_writeread_buf(HW_SPI_ID id, const uint8_t *out_buf, uint8_t *in_buf, uint16_t len,
                                                             hw_spi_tx_callback cb, void *user_data)
{
        SPI_Data *spid = SPIDATA(id);
        uint16_t wordsize = hw_spi_get_memory_word_size(id);
        HW_SPI_FIFO old_mode;

        spid->rx_cb = cb;
        spid->cb_data = user_data;

        spid->tx_buffer = out_buf;
        spid->tx_len = len;
        spid->tx_ix = 0;
        spid->rx_buffer = in_buf;
        spid->rx_len = len;
        spid->rx_ix = 0;
        spid->transfer_mode = HW_SPI_TRANSFER_READWRITE;

        old_mode = hw_spi_change_fifo_mode(id, HW_SPI_FIFO_RX_TX);

        if (cb == NULL) {
                while (spid->rx_ix < len) {
                        hw_spi_transfer(id);
                }
                hw_spi_change_fifo_mode(id, old_mode);
#ifdef HW_SPI_DMA_SUPPORT
        } else if (spid->use_dma) {
                spid->rx_dma.dest_address = (uint32_t)in_buf;
                spid->rx_dma.length = len / wordsize;
                spid->tx_dma.src_address = (uint32_t)out_buf;
                spid->tx_dma.length = len / wordsize;
                spid->tx_dma.dma_init = HW_DMA_INIT_AX_BX_AY_BY;
                spid->tx_dma.a_inc = HW_DMA_AINC_TRUE;
                // DMA requested
                hw_dma_channel_initialization(&spid->rx_dma);
                hw_dma_channel_initialization(&spid->tx_dma);
                GLOBAL_INT_DISABLE();
                hw_dma_channel_enable(spid->rx_dma.channel_number, HW_DMA_STATE_ENABLED);
                hw_dma_channel_enable(spid->tx_dma.channel_number, HW_DMA_STATE_ENABLED);
                GLOBAL_INT_RESTORE();
#endif
        } else {
                // Interrupt driven
                NVIC_DisableIRQ(SPI_INT(id));
                // start writing bytes
                hw_spi_transfer(id);
                hw_spi_enable_interrupt(id);
                NVIC_EnableIRQ(SPI_INT(id));
        }
}

void hw_spi_write_buf(HW_SPI_ID id, const uint8_t *out_buf, uint16_t len,
                                                             hw_spi_tx_callback cb, void *user_data)
{
        SPI_Data *spid = SPIDATA(id);
        uint16_t wordsize = hw_spi_get_memory_word_size(id);
        HW_SPI_FIFO old_mode;

        spid->tx_cb = cb;
        spid->cb_data = user_data;

        spid->tx_buffer = out_buf;
        spid->tx_len = len;
        spid->tx_ix = 0;
        spid->rx_len = 0;
        spid->rx_ix = 0;
        spid->transfer_mode = HW_SPI_TRANSFER_WRITE;

        old_mode = hw_spi_change_fifo_mode(id, HW_SPI_FIFO_TX_ONLY);

        if (cb == NULL) {
                while (spid->tx_ix < len) {
                        hw_spi_transfer_write(id);
                }
                hw_spi_change_fifo_mode(id, old_mode);
#ifdef HW_SPI_DMA_SUPPORT
        } else if (spid->use_dma && wordsize < 4 && len > 1) {
                spid->tx_dma.src_address = (uint32_t)out_buf;
                spid->tx_dma.length = len / wordsize;
                spid->tx_dma.dma_init = HW_DMA_INIT_AX_BX_AY_BY;
                spid->tx_dma.a_inc = HW_DMA_AINC_TRUE;
                // DMA requested
                hw_dma_channel_initialization(&spid->tx_dma);
                hw_dma_channel_enable(spid->tx_dma.channel_number, HW_DMA_STATE_ENABLED);
#endif
        } else {
                // Interrupt driven
                NVIC_DisableIRQ(SPI_INT(id));
                hw_spi_enable_interrupt(id);
                // Start writing
                hw_spi_transfer_write(id);
                NVIC_EnableIRQ(SPI_INT(id));
        }
}

/* Non-cached, non-retained global. */
static volatile uint32_t hw_spi_read_buf_dummy;

void hw_spi_read_buf(HW_SPI_ID id, uint8_t *in_buf, uint16_t len,
                                                             hw_spi_tx_callback cb, void *user_data)
{
        hw_spi_read_buf_dummy = 0xFFFFFFFF;
        SPI_Data *spid = SPIDATA(id);
        uint16_t wordsize = hw_spi_get_memory_word_size(id);
        HW_SPI_FIFO old_mode;
        HW_SPI_FIFO new_mode;

        spid->rx_cb = cb;
        spid->cb_data = user_data;

        spid->tx_len = 0;
        spid->tx_ix = 0;
        spid->rx_buffer = in_buf;
        spid->rx_len = len;
        spid->rx_ix = 0;
        spid->transfer_mode = HW_SPI_TRANSFER_READ;

        new_mode = hw_spi_is_slave(id) ? HW_SPI_FIFO_RX_ONLY : HW_SPI_FIFO_RX_TX;
        old_mode = hw_spi_change_fifo_mode(id, new_mode);
        if (cb == NULL) {
                while (spid->rx_ix < len) {
                        hw_spi_transfer_read(id);
                }
                hw_spi_change_fifo_mode(id, old_mode);
#ifdef HW_SPI_DMA_SUPPORT
        } else if (spid->use_dma && wordsize < 4 && len > 1) {
                spid->rx_dma.dest_address = (uint32_t)in_buf;
                spid->rx_dma.length = len / wordsize;
                // DMA requested
                hw_dma_channel_initialization(&spid->rx_dma);
                if (!hw_spi_is_slave(id)) {
                        spid->tx_dma.src_address = (uint32_t) &hw_spi_read_buf_dummy;
                        spid->tx_dma.length = len / wordsize;
                        /*
                         * We don't use HW_DMA_INIT_AX_BX_BY because it will lock the bus until
                         * the DMA transaction is finished, which might cause bus starvation to
                         * other peripherals.
                         */
                        spid->tx_dma.dma_init = HW_DMA_INIT_AX_BX_AY_BY;
                        spid->tx_dma.a_inc = HW_DMA_AINC_FALSE;
                        hw_dma_channel_initialization(&spid->tx_dma);
                        GLOBAL_INT_DISABLE();
                        hw_dma_channel_enable(spid->rx_dma.channel_number, HW_DMA_STATE_ENABLED);
                        hw_dma_channel_enable(spid->tx_dma.channel_number, HW_DMA_STATE_ENABLED);
                        GLOBAL_INT_RESTORE();
                } else {
                        hw_dma_channel_enable(spid->rx_dma.channel_number, HW_DMA_STATE_ENABLED);
                }
#endif
        } else {
                // Interrupt driven
                NVIC_DisableIRQ(SPI_INT(id));
                // start writing dummy bytes
                hw_spi_transfer_read(id);
                hw_spi_enable_interrupt(id);
                NVIC_EnableIRQ(SPI_INT(id));
        }
}

//=========================== Interrupt handling ===============================

static void SPI_Interrupt_Handler(HW_SPI_ID id)
{
        SPI_Data *spid = SPIDATA(id);
        uint16_t *ix;
        uint16_t *len;
        hw_spi_tx_callback *pcb;

        switch (spid->transfer_mode) {
        case HW_SPI_TRANSFER_READ:
                hw_spi_transfer_read(id);
                ix = &spid->rx_ix;
                len = &spid->rx_len;
                pcb = &spid->rx_cb;
                break;
        case HW_SPI_TRANSFER_WRITE:
                hw_spi_transfer_write(id);
                ix = &spid->tx_ix;
                len = &spid->tx_len;
                pcb = &spid->tx_cb;
                break;
        default:
                hw_spi_transfer(id);
                ix = &spid->rx_ix;
                len = &spid->rx_len;
                pcb = &spid->rx_cb;
                break;
        }
        // Fire callback when done
        if (*len <= *ix) {
                hw_spi_tx_callback cb = *pcb;
                *pcb = NULL;
                hw_spi_disable_interrupt(id);
                if (cb) {
                        cb(spid->cb_data, *len);
                }
        }
}

/**
 * \brief SPI1 Interrupt Handler
 *
 */
void SPI_Handler(void)
{
        SEGGER_SYSTEMVIEW_ISR_ENTER();

        SPI_Interrupt_Handler(HW_SPI1);

        SEGGER_SYSTEMVIEW_ISR_EXIT();
}

/**
 * \brief SPI2 Interrupt Handler
 *
 */
void SPI2_Handler(void)
{
        SEGGER_SYSTEMVIEW_ISR_ENTER();

        SPI_Interrupt_Handler(HW_SPI2);

        SEGGER_SYSTEMVIEW_ISR_EXIT();
}

#endif /* dg_configUSE_HW_SPI */
/**
 * \}
 * \}
 * \}
 */

