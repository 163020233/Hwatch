/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: mbedtls alt platform hardware configuration.
 *
*/
#ifndef MBEDTLS_USER_CONFIG_3322_H
#define MBEDTLS_USER_CONFIG_3322_H

#define MBEDTLS_NO_PLATFORM_ENTROPY     /* Disable platform default entropy. */
#define MBEDTLS_ENTROPY_HARDWARE_ALT    /* use self define hardware entropy. */

#undef MBEDTLS_TIMING_C

/* for connect, it's not Unix or Windows, this MACRO need to be defined. */
#define MBEDTLS_TEST_SW_INET_PTON

#if defined(XTS_SUPPORT)
#define MBEDTLS_ECP_RESTARTABLE
#endif

#if defined(MBEDTLS_HARDEN_OPEN)
#undef MBEDTLS_DES_C
#define MBEDTLS_HKDF_ALT
#define MBEDTLS_BIGNUM_ALT
#define MBEDTLS_SHA256_ALT
#define MBEDTLS_SHA512_ALT
#define MBEDTLS_CMAC_ALT
#define MBEDTLS_CCM_ALT
#define MBEDTLS_GCM_ALT
#define MBEDTLS_AES_ALT
#define MBEDTLS_PBKDF2_HMAC_ALT

#undef MBEDTLS_ECP_DP_SECP192K1_ENABLED
#undef MBEDTLS_ECP_DP_SECP224K1_ENABLED
#undef MBEDTLS_ECP_DP_SECP256K1_ENABLED

#if !defined(MBEDTLS_ECP_RESTARTABLE)
#define MBEDTLS_ECDSA_SIGN_ALT
#define MBEDTLS_ECDSA_VERIFY_ALT
#define MBEDTLS_ECDSA_GENKEY_ALT
#define MBEDTLS_ECDH_COMPUTE_SHARED_ALT
#define MBEDTLS_ECP_ALT
#endif
#include "mbedtls_build_config_check.h"
#endif

#if defined(MBEDTLS_CCM_ALT) || defined(MBEDTLS_GCM_ALT)
#undef MBEDTLS_CAMELLIA_C
#undef MBEDTLS_ARIA_C
#endif

#endif /* MBEDTLS_USER_CONFIG_3322_H */
