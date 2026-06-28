/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024. All rights reserved.
 * Description: mbedtls harden adapt internal header file.
 * Create: 2023-05-10
*/

#ifndef GCM_ALT_H
#define GCM_ALT_H

/**
 * \brief          The GCM context alt structure.
 */
typedef struct mbedtls_gcm_context
{
    mbedtls_cipher_context_t MBEDTLS_PRIVATE(cipher_ctx);  /*!< The cipher context used. */
    uint64_t MBEDTLS_PRIVATE(len);                         /*!< The total length of the encrypted data. */
}
mbedtls_gcm_context;


#endif /* gcm_alt.h */