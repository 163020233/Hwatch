/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 *
 * Description: Provides perf for mbedtls.
 *
 */
#ifndef MBEDTLS_PERF_H
#define MBEDTLS_PERF_H

#ifdef __cplusplus
extern "C" {
#endif

void mbedtls_perf_init(void);

void mbedtls_perf_aes_dump(void);

void mbedtls_perf_bignum_dump(void);

void mbedtls_perf_ccm_dump(void);

void mbedtls_perf_cmac_dump(void);

void mbedtls_perf_ecdsa_dump(void);

void mbedtls_perf_ecp_dump(void);

void mbedtls_perf_gcm_dump(void);

void mbedtls_perf_sha_dump(void);

#ifdef __cplusplus
}
#endif

#endif /* MBEDTLS_PERF_H */