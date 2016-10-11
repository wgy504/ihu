/**
 * \addtogroup BSP
 * \{
 * \addtogroup ADAPTERS
 * \{
 * \addtogroup UART_ADAPTER
 *
 * \brief UART adapter
 *
 * \{
 */

/**
 *****************************************************************************************
 *
 * @file ad_uart.h
 *
 * @brief UART adapter API
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

#ifndef AD_UART_H_
#define AD_UART_H_

#if dg_configUART_ADAPTER

#include <osal.h>
#include <resmgmt.h>
#include <hw_uart.h>
#include <hw_dma.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Device pointer, handle to use with ad_uart_read(), ad_uart_write() etc.
 *
 */
typedef void *uart_device;

/**
 * \brief Device id, those are created by UART_BUS macro in platform_devices.h
 *
 */
typedef const void *uart_device_id;

#ifndef UART_BUS

/**
 * \brief Entry for slave device
 *
 * \param [in] bus_id valid values: UART1, UART2
 * \param [in] name of uart may be COM1, or something like this
 * \param [in] _baud_rate required uart baud rate from enum HW_UART_BAUDRATE
 * \param [in] data_bits value from enum HW_UART_DATABITS
 * \param [in] _parity value from enum HW_UART_PARITY
 * \param [in] _stop value from enum HW_UART_STOPBITS
 * \param [in] _auto_flow_control if 1 auto flow control should be used
 * \param [in] _use_fifo if 1 fifo should be used
 * \param [in] dma_channel_tx DMA number for tx channel, rx msut have the previous number, pass HW_DMA_CHANNEL_INVALID for no DMA
 * \param [in] dma_channel_rx DMA number for rx channel, tx must have the next number, pass HW_DMA_CHANNEL_INVALID for no DMA
 * \param [in] tx_fifo_tr_lvl the tx fifo trigger level, valid numbers 0..3
 * \param [in] rx_fifo_tr_lvl the rx fifo trigger level, valid numbers 0..3
 *
 */
#define UART_BUS(bus_id, name, _baud_rate, data_bits, _parity, _stop, _auto_flow_control,\
        _use_fifo, dma_channel_tx, dma_channel_rx, tx_fifo_tr_lvl, rx_fifo_tr_lvl) \
        extern const void *const name;

#endif

/**
 * \brief Initialize adapter
 *
 */
void ad_uart_init(void);

/**
 * \brief Asynchronous callback function
 *
 */
typedef void (*ad_uart_user_cb)(void *user_data, uint16_t transferred);

/**
 * \brief Open UART device
 *
 * First call to this function will configure UART block. Later calls from other tasks will just
 * return handle to already initialized UART.
 *
 * \param [in] dev_id identifier as passed to to UART_BUS()
 *
 * \return device handle that can be used with other functions
 */
uart_device ad_uart_open(const uart_device_id dev_id);

/*
 * Close UART device
 *
 * \param [in] device handle to device opened with ad_uart_open()
 *
 * \sa ad_uart_open()
 *
 */
void ad_uart_close(uart_device device);

/**
 * \brief Write to UART
 *
 * This function performs write to UART.
 * This function is blocking. It can wait first for bus access, then it waits till transaction is
 * completed.
 *
 * \param [in] dev handle to UART device
 * \param [in] wbuf pointer to data to be sent
 * \param [in] wlen size of data to be sent
 *
 * \sa ad_uart_open()
 * \sa ad_uart_close()
 *
 */
void ad_uart_write(uart_device dev, const char *wbuf, size_t wlen);

/**
 * \brief Start asynchronous write to UART
 *
 * This function sets up write of \p wlen bytes to UART.
 * When write is done user callback \p cb will be called just after ad_uart_bus_release() is called.
 * When write operation is started, user must not release memory pointer \p wbuf.
 *
 * \param [in] dev handle to UART device
 * \param [in] wbuf pointer to data to be sent
 * \param [in] wlen size of data to be sent
 * \param [in] cb function to call after write finishes
 * \param [in] user_data pointer to pass to \p cb
 *
 * \sa ad_uart_open()
 * \sa ad_uart_close()
 *
 * \note The callback is called from within UART ISR and at that time resources
 *       are released
 *
 * \warning Do not call this function consecutively without guaranteeing that the previous
 *          async transaction has completed.
 *
 * \warning After the callback is called, it is not guaranteed that the scheduler will give
 *          control to the task waiting for this transaction to complete. This is important to
 *          consider if more than one tasks are using this API.
 */
void ad_uart_write_async(uart_device dev, const char *wbuf, size_t wlen, ad_uart_user_cb cb,
                                                                                void *user_data);

/**
 * \brief Read from UART
 *
 * This function reads rlen bytes from UART.
 * This function is blocking. It can wait first for bus access, then it waits till transaction is
 * completed.
 * If timeout is OS_EVENT_FOREVER, exactly \p rlen bytes must be received.
 * If timeout is specified, function can exit after timeout with less bytes than requested.
 *
 * \param [in] dev handle to UART device
 * \param [out] rbuf pointer to data to be read from UART
 * \param [in] rlen size of buffer pointed by rbuf
 * \param [in] timeout time in ticks to wait for data
 *
 * \return number of bytes read, 0 if no data arrived in specified time
 *
 * \sa ad_uart_open()
 * \sa ad_uart_close()
 *
 */
int ad_uart_read(uart_device dev, char *rbuf, size_t rlen, OS_TICK_TIME timeout);

/**
 * \brief Start asynchronous read from UART
 *
 * This function sets up read of \p rlen bytes from UART.
 * While this function does not wait for read operation to finish it will start from gaining
 * access to UART resource by calling \sa ad_uart_bus_acquire().
 * If function should not block at all user should call \sa ad_uart_bus_acquire() before.
 *
 * When read operation is started, user must not release memory pointer \p rbuf.
 * After data is received ad_uart_bus_release() will be called just before user callback is executed.
 *
 * To abort started read operation ad_uart_abort_read_async() should be called.
 *
 * \param [in] dev handle to UART device
 * \param [out] rbuf pointer to data to be read from UART
 * \param [in] rlen size of buffer pointed by rbuf
 * \param [in] cb function to call after write is finished or aborted
 * \param [in] user_data pointer to pass to \p cb
 *
 * \sa ad_uart_open()
 * \sa ad_uart_close()
 * \sa ad_uart_abort_read_async()
 *
 * \note The callback is called from within UART ISR and at that time resources
 *       are released
 *
 * \warning Do not call this function consecutively without guaranteeing that the previous
 *          async transaction has completed.
 *
 * \warning After the callback is called, it is not guaranteed that the scheduler will give
 *          control to the task waiting for this transaction to complete. This is important to
 *          consider if more than one tasks are using this API.
 */
void ad_uart_read_async(uart_device dev, char *rbuf, size_t rlen, ad_uart_user_cb cb,
                                                                                void *user_data);

/**
 * \brief Abort previously started asynchronous read
 *
 * This function aborts asynchronous read started with \sa ad_uart_read_async().
 * To avoid unpredictable results ad_uart_bus_acquire() must be called before ad_uart_read_async(),
 * because ad_uart_bus_release() can be called from interrupt fired after user code decided to
 * abort read operation. In that case ad_uart_bus_release() could already release uart for other
 * tasks and calling abort would interfere with next read.
 * If resource is blocked there is no risk of calling abort when read is finishing. User callback
 * will be called only once.
 *
 * \param [in] dev handle to UART device
 *
 * \sa ad_uart_read_async()
 * \sa ad_uart_bus_release()
 *
 */
void ad_uart_abort_read_async(uart_device dev);

/**
 * \brief Acquire access to UART bus
 *
 * This function waits for UART bus to be available, and locks it for current task's use only.
 * This function can be called several times, but number of ad_uart_bus_release() calls must match
 * number of calls to this function.
 *
 * This function should be used if normal ad_uart_read(), ad_uart_write() are not enough
 * and some other UART controller calls are required. In this case typical usage for this function
 * would look like this:
 *
 * ad_uart_bus_acquire(dev);
 * id = ad_uart_get_hw_uart_id(dev);
 * ...
 * hw_uart_set...(id, ...);
 * hw_uart_write_buffer(id, ...)
 * ...
 * ad_uart_bus_release(dev);
 *
 * \param [in] dev handle to UART device
 *
 */
void ad_uart_bus_acquire(uart_device dev);

/**
 * \brief Release access to UART bus
 *
 * This function decrements acquire counter for this device and when it reaches 0 UART bus is
 * released and can be used by other tasks.
 *
 * \param [in] dev handle to UART device
 *
 * \sa ad_uart_bus_acquire
 */
void ad_uart_bus_release(uart_device dev);

/**
 * \brief Get UART controller id
 *
 * This function returns id that can be used to get UART controller id. This id is argument
 * for lower level functions starting with hw_uart_ prefix.
 *
 * \param [in] dev handle to UART device
 *
 * \return id that can be used with hw_uart_... functions
 */
HW_UART_ID ad_uart_get_hw_uart_id(uart_device dev);

#if dg_configUART_SOFTWARE_FIFO
/**
 * \brief Set software fifo
 *
 * This function configures UART to use software FIFO. This allows to receive data when there is
 * no active read in progress.
 *
 * \param [in] dev handle to UART device
 * \param [in] buf buffer to use as software FIFO
 * \param [in] size software FIFO size
 *
 */
void ad_uart_set_soft_fifo(uart_device dev, uint8_t *buf, uint8_t size);
#endif

/**
 * \brief UART device data run time data
 *
 * Variables of this type are automatically generated by \sa UART_BUS() macro.
 * Structure keeps runtime data related to UART.
 *
 */
typedef struct {
        OS_EVENT event;        /**< Event used for synchronization in accessing I2C controller */
        OS_TASK owner;         /**< Task that acquired this device */
        int8_t bus_acquire_count;/**< This keeps track of number of calls to ad_uart_bus_acquire() */
        int8_t open_count;     /**< Open count */
        ad_uart_user_cb read_cb;  /**< User function to call after asynchronous read finishes */
        ad_uart_user_cb write_cb; /**< User function to call after asynchronous write finishes */
        void *read_cb_data;    /**< Data to pass to read_cb */
        void *write_cb_data;   /**< Data to pass to write_cb */
        uint8_t cts_pin;       /**< Port (high nibble) and pin (low nibble) for CTS */
} uart_bus_dynamic_data;

/**
 * \brief UART bus constant data
 *
 * Variable of this type keeps static configuration needed to configure UART bus.
 * Those variables are generated by UART_BUS().
 *
 */
typedef struct uart_device_config {
        HW_UART_ID bus_id;              /**< UART id as needed by hw_uart_... functions */
        RES_ID bus_res_id;              /**< UART resource ID RES_ID_UART1 or RES_ID_UART2 */
        uart_config_ex hw_init;            /**< UART hardware configuration to use */
        uart_bus_dynamic_data *bus_data; /**< pointer to dynamic bus data */
} uart_device_config;

#ifdef __cplusplus
extern }
#endif

#endif /* dg_configUART_ADAPTER */

#endif /* AD_UART_H_ */

/**
 * \}
 * \}
 * \}
 */
