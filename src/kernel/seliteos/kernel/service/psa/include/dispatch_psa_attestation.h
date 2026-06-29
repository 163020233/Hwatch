/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: attestation api header. \n
 *
 * History: \n
 * 2025-03-14, Create file. \n
 */

#ifndef DISPATCH_PSA_ATTESTATION_H
#define DISPATCH_PSA_ATTESTATION_H

#include "errcode.h"
#include "psa_service_ioctl.h"

#ifdef PSA_ATTESTATION_SUPPORT
int32_t dispatch_psa_attest_gen_key(uint32_t cmd, void *arg);
int32_t dispatch_psa_attest_get_token(uint32_t cmd, void *arg);
#else
static inline int32_t dispatch_psa_attest_gen_key(uint32_t cmd, void *arg)
{
    (void)cmd;
    (void)arg;

    return ERRCODE_NOT_SUPPORT;
}

static inline int32_t dispatch_psa_attest_get_token(uint32_t cmd, void *arg)
{
    (void)cmd;
    (void)arg;

    return ERRCODE_NOT_SUPPORT;
}
#endif /* PSA_ATTESTATION_SUPPORT */

#endif /* DISPATCH_PSA_ATTESTATION_H */
