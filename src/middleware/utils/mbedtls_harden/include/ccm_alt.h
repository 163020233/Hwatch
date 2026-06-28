/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: mbedtls alt ccm header.
 *
*/
#ifndef CCM_ALT_H
#define CCM_ALT_H

#include <stddef.h>
#include <stdint.h>
#include "mbedtls/build_info.h"
#include "mbedtls/platform_util.h"
#include "security_symc.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct mbedtls_ccm_context {
    uint8_t key[32];
    uint32_t key_len;
    uint32_t keyslot_handle;
    cipher_handle_t ccm_ctx;
} mbedtls_ccm_context;
#ifdef __cplusplus
}
#endif
#endif