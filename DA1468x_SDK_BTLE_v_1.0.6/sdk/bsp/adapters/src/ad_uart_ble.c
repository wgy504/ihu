/**
 ****************************************************************************************
 *
 * @file ad_uart_ble.c
 *
 * @brief UART adapter for BLE
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

#if dg_configUART_BLE_ADAPTER

#include <string.h>

#include "osal.h"

#include "sdk_defs.h"

#include "ble_config.h"
#include "ad_ble.h"
#include "ble_irb_helper.h"
#include "ble_mgr.h"
#include "ble_mgr_gtl.h"
#include "ble_mgr_irb_common.h"
#include "ad_uart_ble.h"
#include "hw_uart.h"
#include "hw_gpio.h"

/* Task priorities */
#define mainAD_UART_BLE_TASK_PRIORITY    ( OS_TASK_PRIORITY_NORMAL )

/* Event group bits */
#define mainBIT_IN_QUEUE      (1 << 0)
#define mainBIT_RX_DONE       (1 << 1)
#define mainBIT_TX_DONE       (1 << 2)

/* Size of the sync pattern (RW!) */
#define BLE_GTL_SYNC_PATTERN_SIZE             3
#define BLE_HCI_SYNC_PATTERN_SIZE             4

/* Application-specific HCI opcode mask. Used for demultiplexing HCI
 * messages. Its the Vendor Specific (VS) OGF (0x3F) plus the MSB of OCF
 * in order to distinguish for ble-related VS commands
 */
#define APP_SPECIFIC_HCI_MASK 0xFE00

#ifdef BLE_PROD_TEST
// We assume REVD
#define CFG_GPIO_BOOTUART_TX_PORT       HW_GPIO_PORT_1
#define CFG_GPIO_BOOTUART_TX_PIN        HW_GPIO_PIN_3
#define CFG_GPIO_BOOTUART_RX_PORT       HW_GPIO_PORT_2
#define CFG_GPIO_BOOTUART_RX_PIN        HW_GPIO_PIN_3

/* provided by linker script */
extern char __patchable_params;

#endif


#if APP_SPECIFIC_HCI_ENABLE == 1
extern OS_BASE_TYPE APP_SPECIFIC_HCI_SEND(const void *item);
#endif

/* BLE External Interface state machine functions */
static void ble_eif_init(void);
static void ble_eif_read_start(void);
static void ble_eif_read_header(uint8_t len);
static void ble_eif_read_payload(uint16_t len, uint8_t* p_buf);
static void ble_eif_rx_done(void);

/* GTL/HCI helper functions */
static uint16_t ble_eif_type2hdrlen(uint8_t msg_type);
static void *ble_eif_msg2param(uint8_t msg_type, void const *msg);

/* ROM functions */
extern uint8_t hci_cmd_get_max_param_size(uint16_t opcode);

/* BLE environment context */
static struct ble_eif_env_tag ble_eif_env;

/* GTL synchronization pattern */
static const uint8_t ble_gtl_sync_pattern[BLE_GTL_SYNC_PATTERN_SIZE + BLE_HCI_SYNC_PATTERN_SIZE] =
        { 0x52, 0x57, 0x21, 0x01, 0x03, 0x0C, 0x00 };   // "RW!" for GTL and H_COMMAND_RESET for HCI

/* BLE manager interface handles */
static const ble_mgr_interface_t *ble_mgr_if;

/* Stores the handle of the task that will be notified when the transmission is complete. */
static OS_TASK ad_uart_ble_task_handle = NULL;


/**
 * \brief Configure UART2 GPIO pins
 */
static void set_uart2_pin_functions(void)
{
#ifdef BLE_PROD_TEST
    uint32_t *pparams = (void*)&__patchable_params;
    HW_GPIO_PORT tx_port, rx_port;
    HW_GPIO_PIN tx_pin, rx_pin;

    if (pparams[0] != 0xffffffff)
            tx_port = (HW_GPIO_PORT)pparams[0];
    else
            tx_port = CFG_GPIO_BOOTUART_TX_PORT;
    if (pparams[1] != 0xffffffff)
            tx_pin = (HW_GPIO_PIN)pparams[1];
    else
            tx_pin = CFG_GPIO_BOOTUART_TX_PIN;
    if (pparams[2] != 0xffffffff)
            rx_port = (HW_GPIO_PORT)pparams[2];
    else
            rx_port = CFG_GPIO_BOOTUART_RX_PORT;
    if (pparams[3] != 0xffffffff)
            rx_pin = (HW_GPIO_PIN)pparams[3];
    else
            rx_pin = CFG_GPIO_BOOTUART_RX_PIN;

    hw_gpio_set_pin_function(tx_port, tx_pin, HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_UART2_TX);
    hw_gpio_set_pin_function(rx_port, rx_pin, HW_GPIO_MODE_INPUT, HW_GPIO_FUNC_UART2_RX);

#else
    // We assume REVD
    hw_gpio_set_pin_function(HW_GPIO_PORT_1, HW_GPIO_PIN_3, HW_GPIO_MODE_OUTPUT,
            HW_GPIO_FUNC_UART2_TX);
    hw_gpio_set_pin_function(HW_GPIO_PORT_2, HW_GPIO_PIN_3, HW_GPIO_MODE_INPUT,
            HW_GPIO_FUNC_UART2_RX);
    hw_gpio_set_pin_function(HW_GPIO_PORT_1, HW_GPIO_PIN_5, HW_GPIO_MODE_OUTPUT,
            HW_GPIO_FUNC_UART2_RTSN);
    hw_gpio_set_pin_function(HW_GPIO_PORT_1, HW_GPIO_PIN_6, HW_GPIO_MODE_INPUT,
            HW_GPIO_FUNC_UART2_CTSN);

#endif
}

/**
 * \brief Initializes the UART2 module.
 * 
 * \param br UART baud rate.
 */
static void uart2_init(HW_UART_BAUDRATE br)
{
        uart_config uart_init = {
#ifdef BLE_PROD_TEST
                		.auto_flow_control = 0,
#else
                        .auto_flow_control = 1,
#endif
                        .baud_rate         = br,
                        .data              = HW_UART_DATABITS_8,
                        .stop              = HW_UART_STOPBITS_1,
                        .parity            = HW_UART_PARITY_NONE,
                        .use_dma           = 0,
                        .use_fifo          = 1,
                        .rx_dma_channel    = HW_DMA_CHANNEL_2,
                        .tx_dma_channel    = HW_DMA_CHANNEL_3,

        };

        // Initialize UART2 module
        hw_uart_init(HW_UART2, &uart_init);

        // Set UART2 GPIO pin functions
        set_uart2_pin_functions();
}

/**
 * \brief Actions after BLE external interface TX
 */
void ad_uart_ble_rx_done(const uint8_t *data, uint16_t written)
{
        /* Notify the task that the reception is complete. */
        OS_TASK_NOTIFY_FROM_ISR(ad_uart_ble_task_handle, mainBIT_RX_DONE, OS_NOTIFY_SET_BITS);
}

/**
 * \brief Actions after BLE external interface TX
 */
void ad_uart_ble_tx_done(const uint8_t *data, uint16_t written)
{
        /* Notify the task that the transmission is complete. */
        OS_TASK_NOTIFY_FROM_ISR(ad_uart_ble_task_handle, mainBIT_TX_DONE, OS_NOTIFY_SET_BITS);
}

/**
 * \brief Transmit a message over UART2
 * 
 * \param [in]  pxMsg   Pointer to the message buffer
 */
void ad_uart_ble_transmit_msg(irb_ble_stack_msg_t *pxMsg)
{
        uint8_t *bufPtr = NULL;
        uint16_t usLength = sizeof(uint8_t) + pxMsg->msg_size;

        /* Point TX buffer pointer to the beginning of the HCI message */
        bufPtr = (uint8_t *) &pxMsg->msg;

        /* Decrement the pointer to be able to emulate a packed message to be sent to UART */
        bufPtr -= sizeof(uint8_t);

        /* Fill-in the message type */
        *bufPtr = pxMsg->msg_type;

        /* Start the transmission - an interrupt is generated when the transmission is complete */
        hw_uart_send(HW_UART2, bufPtr, usLength, (hw_uart_tx_callback) &ad_uart_ble_tx_done, NULL);
}

/**
 * \brief Main UART TX queue event handling task
 */
static void ad_uart_ble_task(void *pvParameters)
{
        OS_IRB xIRB;
        irb_ble_stack_msg_t *msg_rx = NULL;
        uint32_t ulNotifiedValue;
        OS_BASE_TYPE xResult;
        bool uart_tx_ongoing = false;

        /* Register for task notifications from BLE manager. */
        ble_register_app();

        /* Check if there are messages waiting in the BLE manager's event queue. */
        if (uxQueueMessagesWaiting(ble_mgr_if->evt_q)) {
                OS_TASK_NOTIFY(ad_uart_ble_task_handle, mainBIT_IN_QUEUE, OS_NOTIFY_SET_BITS);
        }

        /* Enable UART interrupts to CPU. */
        NVIC_ClearPendingIRQ(UART2_IRQn);
        NVIC_EnableIRQ(UART2_IRQn);

        for (;;) {
                /*
                 * Wait on any of the event group bits, then clear them all.
                 */
                xResult = OS_TASK_NOTIFY_WAIT(0x0, OS_TASK_NOTIFY_ALL_BITS, &ulNotifiedValue,
                                                                            OS_TASK_NOTIFY_FOREVER);
                /* Guaranteed to succeed since we're waiting forever for the notification */
                OS_ASSERT(xResult == OS_OK);

                /* Check if notification is coming from BLE manager's event queue. */
                if (ulNotifiedValue & mainBIT_IN_QUEUE) {
                        if (uxQueueMessagesWaiting(ble_mgr_if->evt_q) && !uart_tx_ongoing) {
                                /* Get message from the queue. */
                                OS_QUEUE_GET(ble_mgr_if->evt_q, &xIRB, 0);

                                if (xIRB.status == IRB_COMPLETED) {
                                        /* Successfully completed IRB sent previously, do nothing. */
                                }
                                else if (xIRB.status == IRB_ERROR) {
                                        /* Error in previously sent IRB, send back to queue. */
                                        xIRB.status = IRB_PENDING;
                                        ble_mgr_command_queue_send(&xIRB, OS_QUEUE_FOREVER);
                                }
                                else {
                                        /* New pending IRB - attempt to send */
                                        msg_rx = (irb_ble_stack_msg_t *) xIRB.ptr_buf;

                                        /* Set TX ongoing flag. */
                                        uart_tx_ongoing = true;

                                        /* Send the message to the external interface. */
                                        ad_uart_ble_transmit_msg(msg_rx);
                                }
                        }
                }

                /* Check RX done event. */
                if (ulNotifiedValue & mainBIT_RX_DONE) {
                        ble_eif_rx_done();
                }

                /* Check TX done event. */
                if (ulNotifiedValue & mainBIT_TX_DONE) {
                        /* Reset TX ongoing flag. */
                        uart_tx_ongoing = false;

                        /* Mark IRB as completed. */
                        irb_ble_mark_completed(&xIRB, true);

                        /* Send back to BLE manager. */
                        ble_mgr_command_queue_send(&xIRB, OS_QUEUE_FOREVER);
                }

                /* Check if there are more messages waiting in the BLE manager's event queue. */
                if (uxQueueMessagesWaiting(ble_mgr_if->evt_q)) {
                        OS_TASK_NOTIFY(ad_uart_ble_task_handle, mainBIT_IN_QUEUE, OS_NOTIFY_SET_BITS);
                }

        }
}

void ad_uart_ble_init(void)
{
        /* Disable UART2 interrupt to CPU. */
        NVIC_DisableIRQ(UART2_IRQn);

        /* Get BLE manager interface. */
        ble_mgr_if = ble_mgr_get_interface();

        /* Initialize the UART2 interface. */
        uart2_init(HW_UART_BAUDRATE_115200);

        /* Create task. */
        OS_TASK_CREATE("bleU",                         // Text name assigned to the task
                       ad_uart_ble_task,               // Function implementing the task
                       NULL,                           // No parameter passed
                       512,                            // Size of the stack to allocate to task
                       mainAD_UART_BLE_TASK_PRIORITY,  // Priority of the task
                       ad_uart_ble_task_handle);       // Task handle
        OS_ASSERT(ad_uart_ble_task_handle);

        /* Initialize the external interface state machine. */
        ble_eif_init();
}

void ad_uart_ble_notify(void)
{
        if(ad_uart_ble_task_handle) {
                OS_TASK_NOTIFY(ad_uart_ble_task_handle, mainBIT_IN_QUEUE, OS_NOTIFY_SET_BITS);
        }
}

/**
 * \brief Local function: places BLE EIF in RX_START state and sets the external interface
 *        environment
 */
void ble_eif_read_start(void)
{
        //Initialize external interface in reception mode state
        ble_eif_env.rx_state = S_EIF_RX_START;

        //Set the external interface environment to message type 1 byte reception
        hw_uart_receive(HW_UART2, &ble_eif_env.curr_msg_type, 1,
                        (hw_uart_tx_callback) &ad_uart_ble_rx_done, NULL);
}

/**
 * \brief Local function: places BLE EIF in RX header state and sets the external interface
 *        environment
 *
 * \param [in]  len   Length of header to be received in the currently set buffer.
 */
void ble_eif_read_header(uint8_t len)
{
        //change Rx state - wait for header next
        ble_eif_env.rx_state = S_EIF_RX_HEADER;

        //set external interface environment to header reception of len bytes
        hw_uart_receive(HW_UART2, &ble_eif_env.curr_hdr_buff[0], len,
                        (hw_uart_tx_callback) &ad_uart_ble_rx_done, NULL);
}

/**
 * \brief Local function: places BLE EIF in RX payload state and sets the external interface
 *        environment
 *
 * \param [in]  len      Length of payload to be received in the currently set buffer.
 * \param [in]  p_buf    Pointer to the payload buffer
 */
void ble_eif_read_payload(uint16_t len, uint8_t* p_buf)
{
        // Change rx state to payload reception
        ble_eif_env.rx_state = S_EIF_RX_PAYLOAD;

        // Start external interface reception of len bytes
        hw_uart_receive(HW_UART2, p_buf, len, (hw_uart_tx_callback) &ad_uart_ble_rx_done, NULL);
}

/**
 * \brief Local function: places BLE EIF in RX_START_OUT_OF_SYNC state
 */
static void ble_eif_read_next_out_of_sync(void)
{
        //Set external interface reception state to S_EIF_RX_OUT_OF_SYNC
        ble_eif_env.rx_state = S_EIF_RX_OUT_OF_SYNC;

        //Set the external interface environment to 1 byte reception
        hw_uart_receive(HW_UART2, &ble_eif_env.out_of_sync.byte, 1,
                        (hw_uart_tx_callback) &ad_uart_ble_rx_done, NULL);
}

/**
 * \brief Check received byte in out of sync state
 *
 * This function is the algorithm to check that received byte stream in out of sync state
 * corresponds to GTL_reset command.
 *
 * Level of reception is incremented when bytes of GTL_reset_cmd are detected.
 *
 * \return   True if sync is completely received / False otherwise
 */
static bool ble_eif_out_of_sync_check(void)
{
        bool sync_found = false;

        // Check received byte according to current byte position
        if (ble_eif_env.out_of_sync.byte == ble_gtl_sync_pattern[ble_eif_env.out_of_sync.index]) {
                // Increment index
                ble_eif_env.out_of_sync.index++;
        } else if ( (ble_eif_env.out_of_sync.byte == BLE_HCI_CMD_MSG) &&
                (ble_eif_env.out_of_sync.index == 0) ) {
                ble_eif_env.out_of_sync.index = BLE_GTL_SYNC_PATTERN_SIZE + 1;
        } else if ( ble_eif_env.out_of_sync.index < BLE_GTL_SYNC_PATTERN_SIZE ){
                // Re-initialize index
                ble_eif_env.out_of_sync.index = 0;
        }

        if ( (ble_eif_env.out_of_sync.index == BLE_GTL_SYNC_PATTERN_SIZE) ||
             (ble_eif_env.out_of_sync.index ==
                        BLE_GTL_SYNC_PATTERN_SIZE + BLE_HCI_SYNC_PATTERN_SIZE ) ) {
                // Sync has been found
                sync_found = true;
        }

        return sync_found;
}

/**
 * \brief Static function handling external interface out of synchronization detection
 *
 * At external interface reception, when packet indicator opcode of a command is not
 * recognized.
 *
 * \param [in]  rx_byte   The received byte.
 */
static void ble_eif_out_of_sync(uint8_t rx_byte)
{
        // Initialize index
        ble_eif_env.out_of_sync.index = 0;

        // Check the RX byte
        ble_eif_env.out_of_sync.byte = rx_byte;
        ble_eif_out_of_sync_check();

        // Start reception of new packet ID
        ble_eif_read_next_out_of_sync();
}

#if APP_SPECIFIC_HCI_ENABLE == 1
static inline bool ble_eif_is_app_specific(irb_ble_stack_msg_t *msg)
{
        if (msg->msg_type == HCI_CMD_MSG &&
                        (msg->msg.hci.cmd.op_code & APP_SPECIFIC_HCI_MASK) == APP_SPECIFIC_HCI_MASK)
                return true;
        return false;
}
#endif

/**
 * \brief Function called at each RX interrupt.
 *
 * According to BLE RX state, the received data is treated differently: message type,
 * header or payload. Once payload is obtained (if existing) the appropriate function is called,
 * thus generating an IRB to be sent to the BLE Manager.
 */
void ble_eif_rx_done(void)
{
        //check BLE state to see what was received
        switch (ble_eif_env.rx_state)
        {
        /* RECEIVE MESSAGE TYPE STATE*/
        case S_EIF_RX_START:
        {
                if((ble_eif_env.curr_msg_type > 0) && (ble_eif_env.curr_msg_type <= GTL_MSG)) {
                        ble_eif_read_header(ble_eif_type2hdrlen(ble_eif_env.curr_msg_type));
                }
                else {
                        // Incorrect packet indicator -> enter in out of sync
                        ble_eif_out_of_sync(ble_eif_env.curr_msg_type);
                }
        }
        break;
        /* RECEIVE MESSAGE TYPE STATE END*/

        /* RECEIVE HEADER STATE*/
        case S_EIF_RX_HEADER:
        {
                uint16_t param_length = 0;

                // Check received packet indicator
                switch (ble_eif_env.curr_msg_type)
                {
                case BLE_HCI_CMD_MSG:
                        param_length = ble_eif_env.curr_hdr_buff[HCI_CMD_HEADER_LENGTH-1];
                        break;
                case BLE_HCI_ACL_MSG:
                        param_length = ble_eif_env.curr_hdr_buff[HCI_ACL_HEADER_LENGTH-2] +
                                      (ble_eif_env.curr_hdr_buff[HCI_ACL_HEADER_LENGTH-1] << 8);
                        break;
                case BLE_HCI_SCO_MSG:
                        param_length = ble_eif_env.curr_hdr_buff[HCI_SCO_HEADER_LENGTH-1];
                        break;
                case BLE_HCI_EVT_MSG:
                        param_length = ble_eif_env.curr_hdr_buff[HCI_EVT_HEADER_LENGTH-1];
                        break;
                case BLE_GTL_MSG:
                        param_length = ble_eif_env.curr_hdr_buff[GTL_MSG_HEADER_LENGTH-2] +
                                      (ble_eif_env.curr_hdr_buff[GTL_MSG_HEADER_LENGTH-1] << 8);
                        break;
                }

                // Allocate message buffer
                if(ble_eif_env.curr_msg_type < GTL_MSG) {
                        ble_eif_env.p_msg_rx = ble_hci_alloc(ble_eif_env.curr_msg_type, param_length);
                }
                else {
                        ble_eif_env.p_msg_rx = OS_MALLOC(sizeof(irb_ble_stack_msg_t) + param_length
                                                         - sizeof(uint32_t));

                        // Fill-in OP code
                        ble_eif_env.p_msg_rx->op_code = IRB_BLE_STACK_MSG;
                        // Fill-in message type
                        ble_eif_env.p_msg_rx->msg_type = GTL_MSG;
                        // Fill-in message size
                        ble_eif_env.p_msg_rx->msg_size = GTL_MSG_HEADER_LENGTH + param_length;
                }

                // Copy message header
                memcpy(&ble_eif_env.p_msg_rx->msg, ble_eif_env.curr_hdr_buff,
                       ble_eif_type2hdrlen(ble_eif_env.curr_msg_type));

                // Check if parameters are expected
                if (param_length == 0)
                {
                        /* Create and send IRB */

                        OS_IRB new_irb;

                        // Initialize status
                        new_irb.status   = IRB_PENDING;
                        // Fill-in Class ID
                        new_irb.class_id = IRB_BLE;
                        // Copy pointer to allocated message
                        new_irb.ptr_buf  = ble_eif_env.p_msg_rx;

#if APP_SPECIFIC_HCI_ENABLE == 1
                        if (ble_eif_is_app_specific(ble_eif_env.p_msg_rx)) {
                                APP_SPECIFIC_HCI_SEND(&new_irb);
                        } else {
#endif
                                // Send IRB to BLE Manager
                                ble_mgr_command_queue_send(&new_irb, OS_QUEUE_FOREVER);
#if APP_SPECIFIC_HCI_ENABLE == 1
                        }
#endif

                        // Restart a new packet reception
                        ble_eif_read_start();
                }
                else
                {
                        if(ble_eif_env.curr_msg_type == HCI_CMD_MSG) {
                                hci_cmd_msg_t *hci_cmd = (hci_cmd_msg_t *) &ble_eif_env.p_msg_rx->msg;

                                // Check received parameter size
                                if(param_length > hci_cmd_get_max_param_size(hci_cmd->op_code)) {
                                        // Incorrect header -> enter in out of sync
                                        ble_eif_out_of_sync(hci_cmd->param_length);
                                }
                        }

                        // Start payload reception
                        ble_eif_read_payload(param_length,
                                             ble_eif_msg2param(ble_eif_env.curr_msg_type,
                                                               &ble_eif_env.p_msg_rx->msg));
                }
        }
        break;
        /* RECEIVE HEADER STATE END*/

        /* RECEIVE PAYLOAD STATE */
        case S_EIF_RX_PAYLOAD:
        {
                OS_IRB new_irb;

                // Initialize status
                new_irb.status   = IRB_PENDING;
                // Fill-in Class ID
                new_irb.class_id = IRB_BLE;
                // Copy pointer to allocated message
                new_irb.ptr_buf  = ble_eif_env.p_msg_rx;

#if APP_SPECIFIC_HCI_ENABLE == 1
                if (ble_eif_is_app_specific(ble_eif_env.p_msg_rx)) {
                        APP_SPECIFIC_HCI_SEND(&new_irb);
                } else {
#endif
                        // Send IRB to BLE Manager
                        ble_mgr_command_queue_send(&new_irb, OS_QUEUE_FOREVER);
#if APP_SPECIFIC_HCI_ENABLE == 1
                }
#endif
                // Restart a new packet reception
                ble_eif_read_start();
        }
        break;
        /* RECEIVE PAYLOAD STATE END*/

        /* RX OUT OF SYNC STATE */
        case S_EIF_RX_OUT_OF_SYNC:
        default:
        {
                // Check if sync pattern is fully received
                if (ble_eif_out_of_sync_check())
                {
                        if (ble_eif_env.out_of_sync.index ==
                                            BLE_GTL_SYNC_PATTERN_SIZE + BLE_HCI_SYNC_PATTERN_SIZE) {

                                // Allocate the message
                                ble_eif_env.p_msg_rx = OS_MALLOC(sizeof(irb_ble_stack_msg_t)
                                                                 - sizeof(uint8_t));

                                // Fill-in OP code
                                ble_eif_env.p_msg_rx->op_code = IRB_BLE_STACK_MSG;
                                // Fill-in message type
                                ble_eif_env.p_msg_rx->msg_type = HCI_CMD_MSG;
                                // Fill-in message size
                                ble_eif_env.p_msg_rx->msg_size = HCI_CMD_HEADER_LENGTH;

                                volatile hci_cmd_msg_t *hci_cmd =
                                                       (hci_cmd_msg_t *) &ble_eif_env.p_msg_rx->msg;

                                // Send H_RESET_CMD
                                hci_cmd->op_code = HCI_RESET_CMD_OP_CODE;
                                hci_cmd->param_length = 0x00;

                                OS_IRB new_irb;

                                // Initialize status
                                new_irb.status   = IRB_PENDING;
                                // Fill-in Class ID
                                new_irb.class_id = IRB_BLE;
                                // Copy pointer to allocated message
                                new_irb.ptr_buf  = ble_eif_env.p_msg_rx;

                                // Send IRB to BLE Manager
                                ble_mgr_command_queue_send(&new_irb, OS_QUEUE_FOREVER);

                                // Restart a new packet reception
                                ble_eif_read_start();
                        }
                        //change BLE RX state to message type reception
                        ble_eif_read_start();
                }
                else
                {
                        // Start a new byte reception
                        ble_eif_read_next_out_of_sync();
                }
        }
        break;
        /* RX OUT OF SYNC STATE END*/
        }
        /* STATE SWITCH END */
}


/**
 * \brief Initialize BLE external interface state machine.
 */
void ble_eif_init(void)
{
        // Initialize BLE environment context
        ble_eif_env.p_msg_rx = NULL;
        ble_eif_env.p_msg_tx = NULL;

        //start external interface reception
        ble_eif_read_start();
}

/**
 * \brief Returns the header length that corresponds to a GTL/HCI message type
 *
 * \param [in]  msg_type   Message type
 *
 * \return Header length in bytes
 */
uint16_t ble_eif_type2hdrlen(uint8_t msg_type)
{
        // Check received packet indicator
        switch (msg_type)
        {
        case BLE_HCI_CMD_MSG:
                return HCI_CMD_HEADER_LENGTH;
        case BLE_HCI_ACL_MSG:
                return HCI_ACL_HEADER_LENGTH;
        case BLE_HCI_SCO_MSG:
                return HCI_SCO_HEADER_LENGTH;
        case BLE_HCI_EVT_MSG:
                return HCI_EVT_HEADER_LENGTH;
        case BLE_GTL_MSG:
                return GTL_MSG_HEADER_LENGTH;

        default:
                return 0;
        }
}

/**
 * \brief Convert a GTL/HCI message pointer to message payload pointer
 *
 * \param [in]  msg   Pointer to the message buffer
 *
 * \return Pointer to the GTL/HCI message payload
 */
void *ble_eif_msg2param(uint8_t msg_type, void const *msg)
{
        // Check received packet indicator
        switch (msg_type)
        {
        case BLE_HCI_CMD_MSG:
                return (void *) (((uint8_t *) msg) + HCI_CMD_HEADER_LENGTH);
        case BLE_HCI_ACL_MSG:
                return (void *) (((uint8_t *) msg) + HCI_ACL_HEADER_LENGTH);
        case BLE_HCI_SCO_MSG:
                return (void *) (((uint8_t *) msg) + HCI_SCO_HEADER_LENGTH);
        case BLE_HCI_EVT_MSG:
                return (void *) (((uint8_t *) msg) + HCI_EVT_HEADER_LENGTH);
        case BLE_GTL_MSG:
                return (void *) (((uint8_t *) msg) + GTL_MSG_HEADER_LENGTH);
        default:
                return (void *) NULL;
        }
}

#endif  /* dg_configUART_BLE_ADAPTER */
