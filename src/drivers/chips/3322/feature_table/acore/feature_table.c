/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description:   A-core configuration interface for Feature Table
 *
 * Create:  2026-02-04
 */

#include "feature_table.h"
#include "debug_print.h"
#include "chip_io.h"

#define FEATURE_TABLE_START 0x82000200
#define BYTE_MASK           0xFFU

void feature_table_set(feature_table_param param, uint32_t value)
{
    switch (param) {
        case FT_BOARD_ID:
            writeb(FEATURE_TABLE_START, (value & BYTE_MASK));
            break;
        default:
            PRINT("[Error] feature table not exists this param = %#x \r\n", param);
            break;
    }
}
