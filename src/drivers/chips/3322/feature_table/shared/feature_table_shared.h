/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description:   shared configuration params for Feature Table
 *
 * Create:  2026-02-04
 */

#ifndef FEATURE_TABLE_SHARED_H
#define FEATURE_TABLE_SHARED_H

#include "stdint.h"

typedef struct {
    uint8_t board_id;
} feature_table_t;

typedef enum {
    FT_BOARD_ID              = 0x0,
    FT_PARAM_MAX,
} feature_table_param;

typedef enum {
    BOARD_ID_INIT           = 0x0,
    EVB_3322                = 0x1,
    EVB_DITING              = 0x2,
    BOARD_ID_MAX,
} board_id_t;

#endif
