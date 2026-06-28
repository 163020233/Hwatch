/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
*/
#ifndef MBEDTLS_ECP_FUNC_H
#define MBEDTLS_ECP_FUNC_H
#include "mbedtls/private_access.h"

#include "mbedtls/platform_util.h"
#include "mbedtls/ecp.h"
#include "security_pke.h"

#ifdef __cplusplus
extern "C" {
#endif
void mbedtls_alt_get_grp_type(mbedtls_ecp_group_id grp_id, uapi_drv_cipher_pke_ecc_curve_type_t *curve_type);
uapi_drv_cipher_pke_ecc_point_t *ecp_point_malloc(const mbedtls_ecp_point *P, size_t ksize);
uapi_drv_cipher_pke_ecc_point_t *ecp_point_malloc_with_value(uapi_drv_cipher_pke_ecc_curve_type_t curve_type, const mbedtls_ecp_point *P, size_t ksize);
void ecp_point_free(uapi_drv_cipher_pke_ecc_point_t *ecc_points, size_t ksize);

uapi_drv_cipher_pke_data_t *ecp_data_malloc(const mbedtls_mpi *m, size_t ksize);
uapi_drv_cipher_pke_data_t *ecp_data_malloc_with_value(uapi_drv_cipher_pke_ecc_curve_type_t curve_type, const mbedtls_mpi *m, size_t ksize);
void ecp_data_free(uapi_drv_cipher_pke_data_t *ecc_data, size_t ksize);

typedef struct {
    mbedtls_ecp_group_id grp_id;
    uapi_drv_cipher_pke_ecc_curve_type_t curve_type;
} grp_type_info;

#define MBEDTLS_ECP_KLEN_MASK        0x000003FF
#define mbedtls_ecp_get_attr(value, mask)         (((uint32_t)(value) & (uint32_t)(mask)))
#define mbedtls_ecp_get_klen(curve_type)              \
    mbedtls_ecp_get_attr(curve_type, MBEDTLS_ECP_KLEN_MASK)

#ifdef __cplusplus
}
#endif
#endif