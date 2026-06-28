/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: mbedtls alt aes header.
 *
*/

#ifndef AES_ALT_H
#define AES_ALT_H
#include "mbedtls/private_access.h"

#include "mbedtls/build_info.h"
#include "mbedtls/platform_util.h"

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
typedef struct mbedtls_aes_context {
    uintptr_t symc_handle;
} mbedtls_aes_context;

typedef struct mbedtls_aes_xts_context {
    mbedtls_aes_context MBEDTLS_PRIVATE(crypt);
    mbedtls_aes_context MBEDTLS_PRIVATE(tweak);
} mbedtls_aes_xts_context;

#ifdef __cplusplus
}

#endif
#endif /* aes_alt.h */
