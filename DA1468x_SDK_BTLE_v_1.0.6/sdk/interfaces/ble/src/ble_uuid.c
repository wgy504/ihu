/**
 ****************************************************************************************
 *
 * @file ble_uuid.c
 *
 * @brief BLE UUID definitions
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
#include "ble_uuid.h"

static uint8_t base_uuid[] = { 0xFB, 0x34, 0x9B, 0x5F, 0x80, 0x00, 0x00, 0x80,
                                                0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

static void compact_uuid(att_uuid_t *uuid)
{
        if (uuid->type == ATT_UUID_16) {
                // nothing to compact
                return;
        }

        if (memcmp(uuid->uuid128, base_uuid, 12) || memcmp(&uuid->uuid128[14], &base_uuid[14], 2)) {
                // not a BT UUID
                return;
        }

        uuid->type = ATT_UUID_16;
        uuid->uuid16 = uuid->uuid128[13] << 8 | uuid->uuid128[12];
}

void ble_uuid_create16(uint16_t uuid16, att_uuid_t *uuid)
{
        uuid->type = ATT_UUID_16;
        uuid->uuid16 = uuid16;
}

void ble_uuid_from_buf(const uint8_t *buf, att_uuid_t *uuid)
{
        uuid->type = ATT_UUID_128;
        memcpy(uuid->uuid128, buf, ATT_UUID_LENGTH);

        compact_uuid(uuid);
}

bool ble_uuid_from_string(const char *str, att_uuid_t *uuid)
{
        int idx = 0;
        int num_dash = 0;
        uint8_t b = 0;

        // initial check that UUID format seem to be correct
        if (strlen(str) != 36 || str[8] != '-' || str[13] != '-' || str[18] != '-' || str[23] != '-') {
                return false;
        }

        while (*str) {
                char c = *str++;

                if (c == '-') {
                        num_dash++;
                        continue;
                }

                b <<= 4;

                if (c >= '0' && c <= '9') {
                        b |= c - '0';
                } else if (c >= 'a' && c <= 'f') {
                        b |= c - 'a' + 0x0A;
                } else if (c >= 'A' && c <= 'F') {
                        b |= c - 'A' + 0x0A;
                } else {
                        return false;
                }

                idx++;

                // add full byte if idx is even
                if ((idx & 0x1) == 0) {
                        uuid->uuid128[ATT_UUID_LENGTH - idx / 2] = b;
                }
        }

        if ((idx != ATT_UUID_LENGTH * 2) || (num_dash != 4)) {
                return false;
        }

        uuid->type = ATT_UUID_128;

        // try to make it UUID16 if possible
        compact_uuid(uuid);

        return true;
}

bool ble_uuid_equal(const att_uuid_t *uuid1, const att_uuid_t *uuid2)
{
        if (uuid1->type != uuid2->type) {
                return false;
        }

        if (uuid1->type == ATT_UUID_16) {
                return uuid1->uuid16 == uuid2->uuid16;
        }

        return !memcmp(uuid1->uuid128, uuid2->uuid128, ATT_UUID_LENGTH);
}
