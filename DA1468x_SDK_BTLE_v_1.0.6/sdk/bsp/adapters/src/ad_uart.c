/**
 * \addtogroup BSP
 * \{
 * \addtogroup ADAPTERS
 * \{
 * \addtogroup UART
 * \{
 */

/**
 *****************************************************************************************
 *
 * @file ad_uart.c
 *
 * @brief UART adapter implementation
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
#if dg_configUART_ADAPTER

#define UART_BUS(bus, name, _baud_rate, data_bits, _parity, _stop, _auto_flow_control, \
        _use_fifo, _dma_channel_tx, _dma_channel_rx, _tx_fifo_tr_lvl, _rx_fifo_tr_lvl) \
        __RETAINED uart_bus_dynamic_data dynamic_##bus; \
        const uart_device_config dev_##name = { \
                .bus_id = HW_##bus, \
                .bus_res_id = RES_ID_##bus, \
                .hw_init.baud_rate = _baud_rate, \
                .hw_init.data = data_bits, \
                .hw_init.parity = _parity, \
                .hw_init.stop = _stop, \
                .hw_init.auto_flow_control = _auto_flow_control, \
                .hw_init.use_fifo = _use_fifo, \
                .hw_init.use_dma = ((_dma_channel_tx != HW_DMA_CHANNEL_INVALID) && \
                        (_dma_channel_rx != HW_DMA_CHANNEL_INVALID)), \
                .hw_init.tx_fifo_tr_lvl = _tx_fifo_tr_lvl, \
                .hw_init.rx_fifo_tr_lvl = _rx_fifo_tr_lvl, \
                .hw_init.rx_dma_channel = _dma_channel_rx, \
                .hw_init.tx_dma_channel = _dma_channel_tx, \
                .bus_data = &dynamic_##bus \
        }; \
        const void *const name = &dev_##name;

#include <platform_devices.h>
#include <hw_wkup.h>
#include <hw_cpm.h>
#include <ad_uart.h>
#include <resmgmt.h>
#include <interrupts.h>
#include <sys_power_mgr.h>

#define DEVICE_RESOURCE_MASK(config) ad_uart_resource_mask((uart_device_config *) config)

/*
 * Array to hold current device for each UART.
 */
__RETAINED static const uart_device_config *current_config[2];
#define UARTIX(id) ((id) == HW_UART1 ? 0 : 1)

/*
 * Apply device specific configuration of UART controller
 */
static void ad_uart_bus_apply_config(uart_device_config *device)
{
        const HW_UART_ID id = ad_uart_get_hw_uart_id(device);

        current_config[UARTIX(id)] = device;

        hw_uart_init_ex(id, &device->hw_init);
        if (device->hw_init.auto_flow_control && id == HW_UART2) {
                /* Search GPIO settings for CTS */
                if (1 != hw_gpio_get_pins_with_function(HW_GPIO_FUNC_UART2_CTSN,
                                                                &device->bus_data->cts_pin, 1)) {
                        /* One and only one pin can be configured for CTS */
                        OS_ASSERT(0);
                }
        }
}

static inline uint32_t dma_resource_mask(int num)
{
        static const uint32_t res_mask[] = {
                RES_MASK(RES_ID_DMA_MUX01),
                RES_MASK(RES_ID_DMA_MUX23),
                RES_MASK(RES_ID_DMA_MUX45),
                RES_MASK(RES_ID_DMA_MUX67),
        };
        return res_mask[num >> 1];
}

static inline uint32_t ad_uart_resource_mask(uart_device_config *config)
{
        if (!config->hw_init.use_dma) {
                return (1 << config->bus_res_id);
        }
        return (1 << config->bus_res_id) | dma_resource_mask(config->hw_init.rx_dma_channel);
}

void ad_uart_bus_acquire(uart_device dev)
{
        const uart_device_config *config = (uart_device_config *) dev;
        uart_bus_dynamic_data *data =  config->bus_data;
        OS_TASK current_task = OS_GET_CURRENT_TASK();

        if (current_task == data->owner) {
                data->bus_acquire_count++;
                return;
        }

        resource_acquire(DEVICE_RESOURCE_MASK(dev), OS_EVENT_FOREVER);
        data->owner = current_task;
        data->bus_acquire_count++;
}

void ad_uart_bus_release(uart_device dev)
{
        const uart_device_config *config =  (uart_device_config *) dev;

        /* A device release can only happen from the same task that owns it, or from an ISR */
        OS_ASSERT(in_interrupt() || (OS_GET_CURRENT_TASK() == config->bus_data->owner));

        if (--config->bus_data->bus_acquire_count == 0) {
                config->bus_data->owner = NULL;
                resource_release(DEVICE_RESOURCE_MASK(dev));
        }
}

uart_device ad_uart_open(const uart_device_id dev_id)
{
        uart_device_config *config = (uart_device_config *) dev_id;

        resource_acquire(DEVICE_RESOURCE_MASK(config), OS_EVENT_FOREVER);

        if (config->bus_data->open_count++ == 0) {
                ad_uart_bus_apply_config(config);
                OS_EVENT_CREATE(config->bus_data->event);
        }

        resource_release(DEVICE_RESOURCE_MASK(config));

        return (uart_device) config;
}

void ad_uart_close(uart_device dev)
{
        uart_device_config *config = (uart_device_config *) dev;

        --config->bus_data->open_count;
}

HW_UART_ID ad_uart_get_hw_uart_id(uart_device dev)
{
        uart_device_config *device = (uart_device_config *) dev;

        return device->bus_id;
}

struct uart_cb_data {
        uart_device_config *device;
        uint16_t transferred;
};

static void ad_uart_wait_event(void *p, uint16_t transferred)
{
        struct uart_cb_data *cd = (struct uart_cb_data *) p;
        cd->transferred = transferred;
        OS_EVENT_SIGNAL_FROM_ISR(cd->device->bus_data->event);
}

void ad_uart_write(uart_device dev, const char *wbuf, size_t wlen)
{
        uart_device_config *device = (uart_device_config *) dev;
        const HW_UART_ID id = device->bus_id;
        struct uart_cb_data cd = { .device = device, .transferred = 0 };

        ad_uart_bus_acquire(dev);

        hw_uart_send(id, (const uint8_t *) wbuf, wlen, ad_uart_wait_event, &cd);
        OS_EVENT_WAIT(device->bus_data->event, OS_EVENT_FOREVER);

        ad_uart_bus_release(dev);
}

int ad_uart_read(uart_device dev, char *rbuf, size_t rlen, OS_TICK_TIME timeout)
{
        uart_device_config *device = (uart_device_config *) dev;
        const HW_UART_ID id = device->bus_id;
        struct uart_cb_data cd = { .device = device, .transferred = 0 };

        ad_uart_bus_acquire(dev);

        hw_uart_receive(id, (uint8_t *) rbuf, rlen, ad_uart_wait_event, &cd);
        if (OS_EVENT_SIGNALED != OS_EVENT_WAIT(device->bus_data->event, timeout)) {
                /* Force callback */
                hw_uart_abort_receive(device->bus_id);
                /* Clear event */
                OS_EVENT_WAIT(device->bus_data->event, OS_EVENT_NO_WAIT);
        }

        ad_uart_bus_release(dev);

        return cd.transferred;
}

static void ad_uart_read_callback(void *p, uint16_t transferred)
{
        uart_device_config *device = (uart_device_config *) p;
        uart_bus_dynamic_data *data = device->bus_data;
        ad_uart_user_cb cb = data->read_cb;
        void *user_data = data->read_cb_data;
        data->read_cb = NULL;

        /* A not NULL callback must be registered before starting a transaction */
        OS_ASSERT(cb != NULL);

        ad_uart_bus_release((uart_device) device);
        cb(user_data, transferred);
}

static void ad_uart_write_callback(void *p, uint16_t transferred)
{
        uart_device_config *device = (uart_device_config *) p;
        uart_bus_dynamic_data *data = device->bus_data;
        ad_uart_user_cb cb = data->write_cb;
        void *user_data = data->write_cb_data;
        data->write_cb = NULL;

        /* A not NULL callback must be registered before starting a transaction */
        OS_ASSERT(cb != NULL);

        ad_uart_bus_release((uart_device) device);
        cb(user_data, transferred);
}

void ad_uart_write_async(uart_device dev, const char *wbuf, size_t wlen, ad_uart_user_cb cb,
                                                                                void *user_data)
{
        uart_device_config *device = (uart_device_config *) dev;
        uart_bus_dynamic_data *data = device->bus_data;

        ad_uart_bus_acquire(dev);
        data->write_cb = cb;
        data->write_cb_data = user_data;

        hw_uart_send(device->bus_id, (const uint8_t *) wbuf, wlen, ad_uart_write_callback, dev);
}

void ad_uart_read_async(uart_device dev, char *rbuf, size_t rlen, ad_uart_user_cb cb,
                                                                                void *user_data)
{
        uart_device_config *device = (uart_device_config *) dev;
        uart_bus_dynamic_data *data = device->bus_data;

        ad_uart_bus_acquire(dev);
        data->read_cb = cb;
        data->read_cb_data = user_data;

        hw_uart_receive(device->bus_id, (uint8_t *) rbuf, rlen, ad_uart_read_callback, dev);
}

void ad_uart_abort_read_async(uart_device dev)
{
        uart_device_config *device = (uart_device_config *) dev;

        /* Only the device owner can abort the read transaction */
        OS_ASSERT(device->bus_data->owner = OS_GET_CURRENT_TASK());

        /* Force callback */
        hw_uart_abort_receive(device->bus_id);
}

/* Return time in us for one byte transmission at 8N1 (10 bits per byte) */
static uint32_t byte_time(HW_UART_BAUDRATE baud)
{
        switch(baud) {
        case HW_UART_BAUDRATE_1000000: return 10;
        case HW_UART_BAUDRATE_230400: return 44;
        case HW_UART_BAUDRATE_115200: return 87;
        case HW_UART_BAUDRATE_57600: return 174;
        case HW_UART_BAUDRATE_38400: return 261;
        case HW_UART_BAUDRATE_28800: return 348;
        case HW_UART_BAUDRATE_19200: return 521;
        case HW_UART_BAUDRATE_14400: return 695;
        case HW_UART_BAUDRATE_9600: return 1042;
        case HW_UART_BAUDRATE_4800: return 2084;
        default:
                /* Illegal baudrate requested */
                OS_ASSERT(0);
                return 0;
        }
}

static bool ad_uart_flow_off(void)
{
        int i;
        const uart_device_config *serial2 = current_config[1];

        if (serial2 && serial2->bus_data->cts_pin != 0 && hw_uart_afce_getf(HW_UART2)) {
                /* Check if CTS is asserted, if so don't sleep */
                if (hw_uart_cts_getf(HW_UART2)) {
                        return false;
                }

                /* Stop flow, it should tell host to stop sending data */
                hw_uart_rts_setf(HW_UART2, 0);

                /*
                 * Wait for 1 character duration to ensure host has not started a transmission
                 * at the same time
                 * 0.25us at 16MHz for one loop
                 */
                i = byte_time(serial2->hw_init.baud_rate) * cm_cpu_clk_get_fromISR() / 4;
                asm volatile (
                                "    cmp %0, #0\n"
                                "    ble L_END\n"
                                "L_LOOP:\n"
                                "    sub %0, %0, #1\n"
                                "    bne L_LOOP\n"
                                "L_END:"
                                : "=r" (i) : "r" (i));

                /* Check if data has been received during wait time */
                if (hw_uart_receive_fifo_not_empty(HW_UART2)) {
                        hw_uart_afce_setf(HW_UART2, 1);
                        hw_uart_rts_setf(HW_UART2, 1);
                        return false;
                }
        }

        return true;
}

static void ad_uart_set_cts_as_wakeup(bool enable)
{
        const uart_device_config *serial2 = current_config[1];
        uint8_t cts_pin;

        if (serial2 && serial2->bus_data->cts_pin != 0) {
                cts_pin = serial2->bus_data->cts_pin;

                /* Enable or disable wake up source */
                hw_wkup_configure_pin(cts_pin >> 4, cts_pin & 7, enable, HW_WKUP_PIN_STATE_LOW);

                /* Set pin function to GPIO or CTS */
                hw_gpio_set_pin_function(cts_pin >> 4, cts_pin & 0x7, HW_GPIO_MODE_INPUT,
                                        enable ? HW_GPIO_FUNC_GPIO : HW_GPIO_FUNC_UART2_CTSN);
        }
}

static bool ad_uart_prepare_for_sleep(void)
{
        /* Do not sleep when there is transmission in progress */
        if (hw_uart_tx_in_progress(HW_UART1) || hw_uart_tx_in_progress(HW_UART2)) {
                return false;
        }

        if (!ad_uart_flow_off()) {
                /* Incoming data, prevent sleep */
                return false;
        }

        /* Reconfigure CTS as GPIO with wake up function */
        ad_uart_set_cts_as_wakeup(true);

        return !(hw_uart_is_busy(HW_UART1) || hw_uart_is_busy(HW_UART2));
}

static void ad_uart_flow_on(void)
{
        const uart_device_config *serial2 = current_config[1];

        if (serial2 && serial2->bus_data->cts_pin != 0) {
                /* Restore hardware flow control */
                hw_uart_afce_setf(HW_UART2, 1);
                hw_uart_rts_setf(HW_UART2, 1);
        }
}

static void ad_uart_sleep_canceled(void)
{
        /* UART was not powered down yet, disconnect from reconfigure CTS and turn on flow */
        ad_uart_set_cts_as_wakeup(false);
        ad_uart_flow_on();
}

static void ad_uart_wake_up_ind(bool arg)
{
}

static void ad_uart_xtal16m_ready_ind(void)
{
        if (current_config[0] != NULL) {
                hw_uart_reinit_ex(HW_UART1, &current_config[0]->hw_init);
        }
        if (current_config[1] != NULL) {
                hw_uart_reinit_ex(HW_UART2, &current_config[1]->hw_init);
                ad_uart_set_cts_as_wakeup(false);
                ad_uart_flow_on();
        }
}

const adapter_call_backs_t ad_uart_pm_call_backs = {
        .ad_prepare_for_sleep = ad_uart_prepare_for_sleep,
        .ad_sleep_canceled = ad_uart_sleep_canceled,
        .ad_wake_up_ind = ad_uart_wake_up_ind,
        .ad_xtal16m_ready_ind = ad_uart_xtal16m_ready_ind,
        .ad_sleep_preparation_time = 0
};

void ad_uart_init(void)
{
        pm_register_adapter(&ad_uart_pm_call_backs);
}

#if dg_configUART_SOFTWARE_FIFO
void ad_uart_set_soft_fifo(uart_device dev, uint8_t *buf, uint8_t size)
{
        uart_device_config *device = (uart_device_config *) dev;
        const HW_UART_ID id = device->bus_id;

        ad_uart_bus_acquire(dev);

        hw_uart_set_soft_fifo(id, buf, size);

        ad_uart_bus_release(dev);
}
#endif

#endif /* dg_configUART_ADAPTER */
/**
 * \}
 * \}
 * \}
 */
