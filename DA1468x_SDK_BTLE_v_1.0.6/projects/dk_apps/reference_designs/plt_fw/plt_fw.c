/**
 ****************************************************************************************
 *
 * @file plt_fw.c
 *
 * @brief PLT Firmware core code.
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
#include <string.h>
#include <stdio.h>

#include "osal.h"
#include "hw_gpio.h"

#include "ble_mgr.h"
#include "ble_mgr_irb_common.h"
#include "co_version.h"

#include "packers.h"
#include "plt_fw.h"
#include "Xtal_TRIM.h"

#define MAX_TRIM 2047
#define MIN_TRIM 0

typedef void (*plt_cmd_handler)(OS_IRB *irb);

void xtal_trim(OS_IRB *irb);
void fw_version_get(OS_IRB *irb);
void hci_custom_action(OS_IRB *irb);

plt_cmd_handler plt_cmd_handlers[] = {
		NULL,
		NULL,
		xtal_trim,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		fw_version_get,
		NULL,
		hci_custom_action /* 0xFE0A */

};

uint8_t *irb_init_evt(OS_IRB *irb, uint8_t event_code, uint16_t param_length, uint8_t num_hci, uint16_t opcode)
{

	irb_ble_stack_msg_t* msgBuf = NULL;
	uint8_t *b;
	// Allocate the space needed for the message
	msgBuf = OS_MALLOC(sizeof( irb_ble_stack_msg_t ) + param_length - 1);

	msgBuf->op_code = IRB_BLE_STACK_MSG;                // fill IRB message OP code
	msgBuf->msg_type = HCI_EVT_MSG;                  // fill stack message type
	msgBuf->msg_size = param_length + 2;    // evt header length = 2

	msgBuf->msg.hci.evt.event_code = event_code;
	msgBuf->msg.hci.evt.param_length = param_length;

	b = (uint8_t *)&msgBuf->msg.hci.evt.param;
	/* Num_HCI_Command_Packets */
	w8le(b, num_hci);
	padv(b, uint8_t);
	w16le(b, opcode);
	padv(b, uint16_t);


	/* prepare IRB */
	if (irb->ptr_buf != NULL) {
		OS_FREE_FUNC(irb->ptr_buf);
	}
	irb->status = IRB_PENDING;
	irb->class_id = IRB_BLE;
	irb->ptr_buf = msgBuf;

	return b;
}

void plt_parse_irb(OS_IRB *irb)
{
	irb_ble_stack_msg_t *msg = (irb_ble_stack_msg_t *)irb->ptr_buf;
	uint8_t *p = (uint8_t *)&msg->msg;

	/* Get only lower 8-bits of OCF (others are unused) */
	uint8_t cmd = r8le(p);


	if (cmd < xtal_trim_cmd || cmd > (sizeof(plt_cmd_handlers) / sizeof(plt_cmd_handler)) - 1
			|| plt_cmd_handlers[cmd] == NULL) {
		/* Error. Discard command */
		OS_FREE_FUNC(irb->ptr_buf);
		return;
	}

	plt_cmd_handlers[cmd](irb);

}

static inline void enable_output_xtal(void)
{
	GPIO->GPIO_CLK_SEL = 0x3; /* Select XTAL16 clock */
	hw_gpio_set_pin_function(HW_GPIO_PORT_0, HW_GPIO_PIN_5, HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_CLOCK);
}

static inline void disable_output_xtal(void)
{
	hw_gpio_set_pin_function(HW_GPIO_PORT_0, HW_GPIO_PIN_5, HW_GPIO_MODE_INPUT, HW_GPIO_FUNC_GPIO);
}

static inline uint16_t auto_xtal_trim(uint16_t gpio_input)
{

        int r;
        HW_GPIO_PORT port;
        HW_GPIO_PIN pin;
        HW_GPIO_MODE mode;
        HW_GPIO_FUNC function;

        int gpio = gpio_input & 0xFF;
        port = gpio / 10;
        pin = gpio % 10;

        /* Store pulse input gpio previous mode and function */
        hw_gpio_get_pin_function(port, pin, &mode, &function);

        r = auto_trim(gpio);

        /* Restore pulse input gpio previous mode and functions.
         * This is needed because they use the UART RX pin for
         * pulse input. It must be restored to resume UART operation
         */
        hw_gpio_set_pin_function(port, pin, mode, function);


        if (r < 0)
                return -r;
        else
                return 0;
}

void xtal_trim(OS_IRB *irb)
{
	uint8_t *b;
	uint8_t operation;
	uint16_t trim_value;
	uint16_t outval;

	/* Parse incoming message */
	irb_ble_stack_msg_t *msg = (irb_ble_stack_msg_t *)irb->ptr_buf;
	b = (uint8_t *)&msg->msg + HCI_CMD_PARAM_OFFSET;

	operation = r8le(b);
	padv(b, uint8_t);

	trim_value = r16le(b);

	outval = 0;

	switch (operation) {
	case 0: /* Read trim value */
		outval = CRG_TOP->CLK_FREQ_TRIM_REG;
		break;
	case 1: /* Write trim value */
		CRG_TOP->CLK_FREQ_TRIM_REG = trim_value;
		break;
	case 2: /* Enable output xtal on P05 */
		enable_output_xtal();
		break;
	case 3: /* Increase trim value by delta */
		CRG_TOP->CLK_FREQ_TRIM_REG = CRG_TOP->CLK_FREQ_TRIM_REG + trim_value;
		break;
	case 4: /* Decrease trim value by delta */
		CRG_TOP->CLK_FREQ_TRIM_REG = CRG_TOP->CLK_FREQ_TRIM_REG - trim_value;
		break;
	case 5: /* Disable output xtal on P05 */
		disable_output_xtal();
		break;
	case 6: /* Auto calibration test */
                outval = auto_xtal_trim(trim_value);
		break;
	}

	/* Prepare response */
	b = irb_init_evt(irb, XTAL_TRIM_EVT_EVCODE, XTAL_TRIM_EVT_PARAM_SIZE, 1, XTAL_TRIM_EVT_OPCODE);

	/* Write output value */
	w16le(b, outval);

	// Send directly to BLE Manager's event queue
	ble_mgr_event_queue_send(irb, OS_QUEUE_FOREVER);

}

void fw_version_get(OS_IRB *irb)
{
	uint8_t *b;
	uint8_t av_len;
	uint8_t bv_len;
	uint8_t ble_ver_str[32];

	/* Prepare response */
	b = irb_init_evt(irb, FW_VERSION_GET_EVT_EVCODE, FW_VERSION_GET_EVT_PARAM_SIZE, 1, FW_VERSION_GET_EVT_OPCODE);

	memset(b, 0, FW_VERSION_GET_EVT_PARAM_SIZE - 3);

	bv_len = snprintf(ble_ver_str, 32, "%d.%d.%d.%d", RWBLE_SW_VERSION_MAJOR,
			RWBLE_SW_VERSION_MINOR, RWBLE_SW_VERSION_BUILD,
			RWBLE_SW_VERSION_SUB_BUILD ) + 1;

	av_len = strlen(PLT_VERSION_STR) + 1;

	/* write ble version length */
	w8le(b, bv_len);
	padv(b, uint8_t);

	/* write application version length */
	w8le(b, av_len);
	padv(b, uint8_t);

	/* ble version string */
	memcpy(b, ble_ver_str, bv_len);
	padvN(b, 32);

	memcpy(b, PLT_VERSION_STR, av_len);

	// Send directly to BLE Manager's event queue
	ble_mgr_event_queue_send(irb, OS_QUEUE_FOREVER);

}

void hci_custom_action(OS_IRB *irb)
{
        uint8_t *b;
        uint8_t av_len;
        uint8_t bv_len;
        uint8_t ble_ver_str[32];

        uint8_t data;

        /* Parse incoming message */
        irb_ble_stack_msg_t *msg = (irb_ble_stack_msg_t *)irb->ptr_buf;
        b = (uint8_t *)&msg->msg + HCI_CMD_PARAM_OFFSET;

        data = r8le(b);

        /* Prepare response */
        b = irb_init_evt(irb, HCI_CUSTOM_ACTION_EVT_EVCODE, HCI_CUSTOM_ACTION_EVT_PARAM_SIZE, 1, HCI_CUSTOM_ACTION_EVT_OPCODE);

        memset(b, 0, HCI_CUSTOM_ACTION_EVT_PARAM_SIZE - 3);

        /* echo read data */
        w8le(b, data);

        // Send directly to BLE Manager's event queue
        ble_mgr_event_queue_send(irb, OS_QUEUE_FOREVER);

}
