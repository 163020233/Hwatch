/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024. All rights reserved.
 * Description: mbedtls harden adapt internal header file.
 * Create: 2023-05-10
*/

#ifndef AES_ALT_H
#define AES_ALT_H
#include "mbedtls/private_access.h"

#include "mbedtls/build_info.h"
#include "mbedtls/platform_util.h"

#include <stddef.h>
#include <stdint.h>
#include "mbedtls_crypto_type.h"

typedef struct mbedtls_aes_context
{
    td_handle kslot_handle;
    unsigned char key[32];
    unsigned int key_len;
}
mbedtls_aes_context;

#if defined(MBEDTLS_CIPHER_MODE_XTS)

typedef struct mbedtls_aes_xts_context
{
    mbedtls_aes_context MBEDTLS_PRIVATE(crypt); /*!< The AES context to use for AES block
                                        encryption or decryption. */
    mbedtls_aes_context MBEDTLS_PRIVATE(tweak); /*!< The AES context used for tweak
                                        computation. */
} mbedtls_aes_xts_context;
#endif /* MBEDTLS_CIPHER_MODE_XTS */


#endif /* aes_alt.h */