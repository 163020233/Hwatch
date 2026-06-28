/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: mbedtls alt gcm header.
 *
*/
#ifndef GCM_ALT_H
#define GCM_ALT_H

#include "mbedtls/private_access.h"

#include <stddef.h>
#include <stdint.h>
#include "mbedtls/build_info.h"
#include "mbedtls/platform_util.h"
#include "security_symc.h"

#ifdef __cplusplus
extern "C" {
#endif
typedef struct mbedtls_gcm_context {
    uint8_t MBEDTLS_PRIVATE(key)[32];
    uint32_t MBEDTLS_PRIVATE(key_len);
    uint32_t MBEDTLS_PRIVATE(keyslot_handle);
    cipher_handle_t MBEDTLS_PRIVATE(gcm_ctx);
} mbedtls_gcm_context;

#ifdef __cplusplus
}
#endif
#endif