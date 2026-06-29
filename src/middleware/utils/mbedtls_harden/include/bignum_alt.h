/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: mbedtls alt bignum header.
 *
*/
#ifndef BIGNUM_ALT_H
#define BIGNUM_ALT_H

#include "mbedtls/private_access.h"
#include "mbedtls/bignum.h"
#include "mbedtls/build_info.h"
#include "mbedtls/platform_util.h"

#ifdef __cplusplus
extern "C" {
#endif
#define MBEDTLS_ERR_BIGNUM_HARDWARE_UNSUPPORT   0x1000

typedef enum {
    MBEDTLS_ALT_ECP_LEN_192 = 24,
    MBEDTLS_ALT_ECP_LEN_224 = 32,
    MBEDTLS_ALT_ECP_LEN_256 = 32,
    MBEDTLS_ALT_ECP_LEN_384 = 48,
    MBEDTLS_ALT_ECP_LEN_448 = 56,
    MBEDTLS_ALT_ECP_LEN_512 = 64,
    MBEDTLS_ALT_ECP_LEN_521 = 68,
    MBEDTLS_ALT_ECP_LEN_576 = 72,
    MBEDTLS_ALT_ECP_LEN_1024 = 128,
    MBEDTLS_ALT_ECP_LEN_1536 = 192,
    MBEDTLS_ALT_ECP_LEN_2048 = 256,
    MBEDTLS_ALT_ECP_LEN_3072 = 384,
    MBEDTLS_ALT_ECP_LEN_4096 = 512,
    MBEDTLS_ALT_ECP_LEN_INVALID = 0xFFFFFFFF,
} mbedtls_alt_ecp_len_e;

int mbedtls_mpi_exp_mod_alt(mbedtls_mpi *X, const mbedtls_mpi *A, const mbedtls_mpi *E, const mbedtls_mpi *N);
#ifdef __cplusplus
}
#endif
#endif