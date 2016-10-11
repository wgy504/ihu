/*
 * plt_fw.h
 *
 *  Created on: Sep 11, 2015
 *      Author: akostop
 */

#ifndef PLT_FW_H_
#define PLT_FW_H_

#include "ble_common.h"
#include "ble_mgr_irb.h"

#define PLT_VERSION_STR "1.1"

#define HCI_CMD_PARAM_OFFSET 3

#define XTAL_TRIM_EVT_PARAM_SIZE 5
#define XTAL_TRIM_EVT_OPCODE 0xFE02
#define XTAL_TRIM_EVT_EVCODE 0xE

#define FW_VERSION_GET_EVT_PARAM_SIZE 0x45
#define FW_VERSION_GET_EVT_OPCODE 0xFE08
#define FW_VERSION_GET_EVT_EVCODE 0xE

#define HCI_CUSTOM_ACTION_EVT_PARAM_SIZE 0x4
#define HCI_CUSTOM_ACTION_EVT_OPCODE 0xFE0A
#define HCI_CUSTOM_ACTION_EVT_EVCODE 0xE


enum plt_cmd_e {
	xtal_trim_cmd = 1,
};

typedef struct {
        irb_ble_hdr_t    hdr;
        uint8_t			 operation;
        uint16_t         trim_value;
} plt_cmd_xtrim_t;

typedef struct {
	ble_evt_hdr_t      hdr;                         ///< Event header
    uint8_t			   num_hci_cmd_packets;
    uint16_t 		   opcode;
} plt_evt_hdr_t;

typedef struct {
        plt_evt_hdr_t      hdr;                         ///< Event header
        uint16_t           trim_value;
} plt_evt_xtrim_t;


void plt_parse_irb(OS_IRB *irb);

#endif /* PLT_FW_H_ */
