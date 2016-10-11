/**
 ****************************************************************************************
 *
 * @file ble_irb_helper.c
 *
 * @brief BLE IRB handlers
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

#include <string.h>
#include "ble_mgr.h"
#include "ble_mgr_irb.h"
#include "ble_irb_helper.h"
#include "ble_common.h"

void *alloc_ble_msg(uint16_t op_code, uint16_t size)
{
        irb_ble_hdr_t *msg;

        /* Allocate at least the size needed for the base message */
        OS_ASSERT(size >= sizeof(irb_ble_hdr_t));

        msg = OS_MALLOC(size);
        memset(msg, 0, size);
        msg->op_code  = op_code;
        msg->msg_len = size - sizeof(irb_ble_hdr_t);

        return msg;
}

static void *alloc_evt(uint16_t evt_code, uint16_t size)
{
        ble_evt_hdr_t *evt;

        /* Allocate at least the size needed for the base message */
        OS_ASSERT(size >= sizeof(*evt));

        evt = OS_MALLOC(size);
        memset(evt, 0, size);
        evt->evt_code = evt_code;
        evt->length = size - sizeof(*evt);

        return evt;
}

void *irb_ble_init_msg(OS_IRB *irb, uint16_t op_code, uint16_t size)
{
        /* Allocate at least the size needed for the base message */
        OS_ASSERT(size >= sizeof(irb_ble_hdr_t));

        /* prepare IRB */
        irb->status = IRB_PENDING;
        irb->class_id = IRB_BLE;
        irb->ptr_buf = alloc_ble_msg(op_code, size);

        return irb->ptr_buf;
}

void *irb_ble_init_evt(OS_IRB *irb, uint16_t evt_code, uint16_t size)
{
        /* Allocate at least the size needed for the base message */
        OS_ASSERT(size >= sizeof(ble_evt_hdr_t));

        /* prepare IRB */
        irb->status = IRB_PENDING;
        irb->class_id = IRB_BLE;
        irb->ptr_buf = alloc_evt(evt_code, size);

        return irb->ptr_buf;
}

void *irb_ble_replace_msg(OS_IRB *irb, uint16_t op_code, uint16_t size)
{
        /* Only works with class IRB_BLE IRBs */
        OS_ASSERT(irb->class_id == IRB_BLE);
        /* Allocate at least the size needed for the base message */
        OS_ASSERT(size >= sizeof(irb_ble_hdr_t));

        if (irb->ptr_buf) {
                irb_ble_free_msg(irb);
        }

        irb->ptr_buf = alloc_ble_msg(op_code, size);

        return irb->ptr_buf;
}

void irb_ble_mark_completed(OS_IRB *irb, bool free_msg)
{
        /* Only works with class IRB_BLE IRBs that are pending */
        OS_ASSERT(irb->class_id == IRB_BLE);
        OS_ASSERT(irb->status == IRB_PENDING);

        if (free_msg) {
                irb_ble_free_msg(irb);
        }

        irb->status = IRB_COMPLETED;
}

void irb_ble_mark_error(OS_IRB *irb, bool free_msg)
{
        /* Only works with class IRB_BLE IRBs that are pending */
        OS_ASSERT(irb->class_id == IRB_BLE);
        OS_ASSERT(irb->status == IRB_PENDING);

        if (free_msg) {
                irb_ble_free_msg(irb);
        }

        irb->status = IRB_ERROR;
}

void irb_ble_free_msg(OS_IRB *irb)
{
        /* Only works with class IRB_BLE IRBs */
        OS_ASSERT(irb->class_id == IRB_BLE);

        if (irb->ptr_buf) {
                OS_FREE(irb->ptr_buf);
                irb->ptr_buf = NULL;
        }
}


bool irb_execute(void *cmd, void **rsp)
{
        OS_IRB irb;
        uint16_t op_code __attribute__((unused));

        // save opcode for check later
        op_code = ((irb_ble_hdr_t *)cmd)->op_code;

        // prepare IRB
        irb.status = IRB_PENDING;
        irb.class_id = IRB_BLE;
        irb.ptr_buf = cmd;

        // acquire the BLE manager interface
        ble_mgr_acquire();

        // send and wait for response
        ble_mgr_command_queue_send(&irb, OS_QUEUE_FOREVER);
        ble_mgr_response_queue_get(&irb, OS_QUEUE_FOREVER);

        // release the BLE manager interface
        ble_mgr_release();

        if (irb.status != IRB_COMPLETED) {
                return false;
        }

        *rsp = irb.ptr_buf;

        /* The response op code must be the same as the original
         * command op code
         */
        OS_ASSERT(((irb_ble_hdr_t *)*rsp)->op_code == op_code);

        return true;
}
