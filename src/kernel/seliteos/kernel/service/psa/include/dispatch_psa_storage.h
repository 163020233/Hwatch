/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: PSA storage function header
 */

#ifndef __DISPATCH_PSA_STORAGE_H__
#define __DISPATCH_PSA_STORAGE_H__

#include "errcode.h"
#include "psa_service_ioctl.h"
#include "psa_common.h"

#ifdef PSA_STORAGE_SUPPORT
int32_t dispatch_psa_storage_read(uint32_t cmd, void *arg);

int32_t dispatch_psa_storage_write(uint32_t cmd, void *arg);

int32_t dispatch_psa_storage_remove(uint32_t cmd, void *arg);
#else

static inline int32_t dispatch_psa_storage_read(uint32_t cmd, void *arg)
{
    (void)cmd;
    (void)arg;

    return ERRCODE_NOT_SUPPORT;
}

static inline int32_t dispatch_psa_storage_write(uint32_t cmd, void *arg)
{
    (void)cmd;
    (void)arg;

    return ERRCODE_NOT_SUPPORT;
}

static inline int32_t dispatch_psa_storage_remove(uint32_t cmd, void *arg)
{
    (void)cmd;
    (void)arg;

    return ERRCODE_NOT_SUPPORT;
}

#endif

#endif /* __DISPATCH_PSA_STORAGE_H__ */
