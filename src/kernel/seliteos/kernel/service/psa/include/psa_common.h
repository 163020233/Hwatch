/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: psa common header. \n
 *
 * History: \n
 * 2025-03-14, Create file. \n
 */

#ifndef DISPATCH_PSA_COMMON_H
#define DISPATCH_PSA_COMMON_H

#include "errcode.h"
#include "chips_porting.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef SECURITY_ENHANCEMENT
#define PSA_INTERNAL_SUCCESS  0x3CA5965A
#define PSA_INTERNAL_FAILURE  0xC35A69A5
#else
#define PSA_INTERNAL_SUCCESS  0UL
#define PSA_INTERNAL_FAILURE  0xFFFFFFFF
#endif

int psa_get_wrap_errcode(errcode_t from_errcode);

#ifdef __cplusplus
}
#endif

#endif
