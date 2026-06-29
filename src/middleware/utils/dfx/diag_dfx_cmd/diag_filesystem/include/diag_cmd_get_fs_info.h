/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: dfx get filesystem info
 * This file should be changed only infrequently and with great care.
 */
#ifndef DIAG_CMD_GET_FS_INFO_H
#define DIAG_CMD_GET_FS_INFO_H

#include "errcode.h"
#include "diag.h"

#define MAX_FS_VOLUMES_LEN (16)

typedef struct {
    char name[MAX_FS_VOLUMES_LEN];
    uint32_t total_size;
    uint32_t used_size;
    uint32_t free_size;
} diag_fs_info_t;

errcode_t diag_fs_proc_init(void);

#endif