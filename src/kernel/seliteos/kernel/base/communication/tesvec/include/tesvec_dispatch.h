/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: tesvec dispatch header for SELiteOS.
 */

#ifndef TESVEC_DISPATCH_H
#define TESVEC_DISPATCH_H

#include <errcode.h>

typedef int32_t (*tesvec_dispatch_func)(uint32_t cmd, void *arg);

typedef struct {
    uint32_t cmd;
    tesvec_dispatch_func func;
} tesvec_ioctl_cmd_t;

const tesvec_ioctl_cmd_t *get_tesvec_dispatch_func_list(void);

#endif
