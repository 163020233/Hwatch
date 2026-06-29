/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: mbedtls alt ecp header.
 *
*/
#ifndef ECP_ALT_H
#define ECP_ALT_H

#include "mbedtls/private_access.h"

#include <stddef.h>
#include <stdint.h>
#include "mbedtls/build_info.h"
#include "mbedtls/platform_util.h"
#include "mbedtls/ecp.h"
#include "security_pke.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct mbedtls_ecp_group {
    mbedtls_ecp_group_id id;
    mbedtls_mpi P;
    mbedtls_mpi A;
    mbedtls_mpi B;
    mbedtls_ecp_point G;
    mbedtls_mpi N;
    size_t pbits;
    size_t nbits;

    unsigned int MBEDTLS_PRIVATE(h);
} mbedtls_ecp_group;

#if MBEDTLS_VERSION_NUMBER == 0x03060000
typedef int mbedtls_f_rng_t(void *p_rng,
                            unsigned char *output, size_t output_size);
#endif

#ifdef __cplusplus
}
#endif
#endif