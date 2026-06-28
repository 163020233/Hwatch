/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: os platform config files
 */

#ifndef _HISOC_CLIC_H
#define _HISOC_CLIC_H

#include "asm/platform.h"
#include "los_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define CLIC_CFG                            0
#define CLIC_INFO                           4
#define CLIC_INTIP(x)                       (0x1000 + 4 * (x))
#define CLIC_INTIE(x)                       (0x1001 + 4 * (x))
#define CLIC_INTATTR(x)                     (0x1002 + 4 * (x))
#define CLIC_INTCTL(x)                      (0x1003 + 4 * (x))
#define CLIC_INFO_CLICINTCTLBITS_PRIO_LIMIT 8
#define REG_CLIC_INFO_CLICINTCTLBITS 3

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
