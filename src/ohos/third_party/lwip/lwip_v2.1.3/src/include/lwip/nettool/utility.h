/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026. All rights reserved.
 * Description: Character and Data Conversion API definition
 * Create:2026
 */

#ifndef LWIP_UTILITY_H
#define LWIP_UTILITY_H

#include "arch/cc.h"
#include "lwip/opt.h"

#ifdef __cplusplus
extern "C" {
#endif

s32_t lwip_isdigitstr(const s8_t *str);
#ifdef __cplusplus
}
#endif

#endif /* LWIP_UTILITY_H */
