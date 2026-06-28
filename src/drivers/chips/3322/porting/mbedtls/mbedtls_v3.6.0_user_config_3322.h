/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: mbedtls alt platform hardware configuration.
 *
*/
#ifndef MBEDTLS_USER_CONFIG_3322_H
#define MBEDTLS_USER_CONFIG_3322_H

#include "mbedtls_porting.h"

#define MBEDTLS_NO_PLATFORM_ENTROPY     /* Disable platform default entropy. */
#define MBEDTLS_ENTROPY_HARDWARE_ALT    /* use self define hardware entropy. */

#undef MBEDTLS_TIMING_C

/* for connect, it's not Unix or Windows, this MACRO need to be defined. */
#define MBEDTLS_TEST_SW_INET_PTON

#if defined(MBEDTLS_HARDEN_OPEN)

#define MBEDTLS_SHA256_ALT
#define MBEDTLS_SHA512_ALT
#define MBEDTLS_CMAC_ALT
#define MBEDTLS_CCM_ALT
#define MBEDTLS_GCM_ALT
#define MBEDTLS_AES_ALT
#define MBEDTLS_PBKDF2_HMAC_ALT
#define MBEDTLS_ECP_ALT
#define MBEDTLS_BIGNUM_ALT

#define MBEDTLS_ECDSA_SIGN_ALT
#define MBEDTLS_ECDSA_VERIFY_ALT
#define MBEDTLS_ECDSA_GENKEY_ALT
#define MBEDTLS_ECDH_COMPUTE_SHARED_ALT

#endif
#endif /* MBEDTLS_USER_CONFIG_3322_H */
