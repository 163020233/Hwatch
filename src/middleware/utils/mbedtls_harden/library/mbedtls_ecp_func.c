/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: some functions for mbedtls ecp.
 *
*/
#include "library/common.h"

#include "mbedtls/platform.h"
#include "mbedtls/platform_util.h"
#include "mbedtls_ecp_func.h"
/* lookup table for mbedtls_ecp_group_id
    to uapi_drv_cipher_pke_ecc_curve_type_t */

const grp_type_info g_grp_type_info_table[] = {
#if defined(MBEDTLS_ECP_DP_SECP192R1_ENABLED)
    {MBEDTLS_ECP_DP_SECP192R1, UAPI_DRV_CIPHER_PKE_ECC_TYPE_FIPS_P192R},
#endif
#if defined(MBEDTLS_ECP_DP_SECP224R1_ENABLED)
    {MBEDTLS_ECP_DP_SECP224R1, UAPI_DRV_CIPHER_PKE_ECC_TYPE_FIPS_P224R},
#endif
#if defined(MBEDTLS_ECP_DP_SECP256R1_ENABLED)
    {MBEDTLS_ECP_DP_SECP256R1, UAPI_DRV_CIPHER_PKE_ECC_TYPE_FIPS_P256R},
#endif
#if defined(MBEDTLS_ECP_DP_SECP384R1_ENABLED)
    {MBEDTLS_ECP_DP_SECP384R1, UAPI_DRV_CIPHER_PKE_ECC_TYPE_FIPS_P384R},
#endif
#if defined(MBEDTLS_ECP_DP_SECP521R1_ENABLED)
    {MBEDTLS_ECP_DP_SECP521R1, UAPI_DRV_CIPHER_PKE_ECC_TYPE_FIPS_P521R},
#endif
#if defined(MBEDTLS_ECP_DP_BP256R1_ENABLED)
    {MBEDTLS_ECP_DP_BP256R1, UAPI_DRV_CIPHER_PKE_ECC_TYPE_RFC5639_P256},
#endif
#if defined(MBEDTLS_ECP_DP_BP384R1_ENABLED)
    {MBEDTLS_ECP_DP_BP384R1, UAPI_DRV_CIPHER_PKE_ECC_TYPE_RFC5639_P384},
#endif
#if defined(MBEDTLS_ECP_DP_BP512R1_ENABLED)
    {MBEDTLS_ECP_DP_BP512R1, UAPI_DRV_CIPHER_PKE_ECC_TYPE_RFC5639_P512},
#endif
#if defined(MBEDTLS_ECP_DP_CURVE25519_ENABLED)
    {MBEDTLS_ECP_DP_CURVE25519, UAPI_DRV_CIPHER_PKE_ECC_TYPE_RFC7748},
#endif
#if defined(MBEDTLS_ECP_DP_CURVE448_ENABLED)
    {MBEDTLS_ECP_DP_CURVE448, UAPI_DRV_CIPHER_PKE_ECC_TYPE_RFC7748_448},
#endif
    {MBEDTLS_ECP_DP_NONE, UAPI_DRV_CIPHER_PKE_ECC_TYPE_INVALID},
};
void mbedtls_alt_get_grp_type(mbedtls_ecp_group_id grp_id, uapi_drv_cipher_pke_ecc_curve_type_t *curve_type)
{
    unsigned int i = 0;
    *curve_type = UAPI_DRV_CIPHER_PKE_ECC_TYPE_INVALID;
    for (i = 0; i < sizeof(g_grp_type_info_table) / sizeof(g_grp_type_info_table[0]); i++) {
        if (g_grp_type_info_table[i].grp_id == grp_id) {
            *curve_type = g_grp_type_info_table[i].curve_type;
            return;
        }
    }
    return;
}

#define ECC_POINTS_SIZE    (sizeof(uapi_drv_cipher_pke_ecc_point_t))
uapi_drv_cipher_pke_ecc_point_t *ecp_point_malloc(const mbedtls_ecp_point *P,
    size_t ksize)
{
    int ret;
    uapi_drv_cipher_pke_ecc_point_t *ecc_point = mbedtls_calloc(1, ECC_POINTS_SIZE);
    if (ecc_point == NULL) {
        return NULL;
    }

    ecc_point->x = mbedtls_calloc(1, ksize);
    if (ecc_point->x == NULL) {
        goto cleanup;
    }

    ecc_point->y = mbedtls_calloc(1, ksize);
    if (ecc_point->y == NULL) {
        goto cleanup;
    }

    ecc_point->length = ksize;
    if (P != NULL) {
        MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary(&(P->X), ecc_point->x, ksize));
        MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary(&(P->Y), ecc_point->y, ksize));
    }
    return ecc_point;
cleanup:
    if (ecc_point != NULL) {
        if (ecc_point->x != NULL) {
            mbedtls_platform_zeroize(ecc_point->x, ksize);
            mbedtls_free(ecc_point->x);
            ecc_point->x = NULL;
        }
        if (ecc_point->y != NULL) {
            mbedtls_platform_zeroize(ecc_point->y, ksize);
            mbedtls_free(ecc_point->y);
            ecc_point->y = NULL;
        }
        mbedtls_platform_zeroize(ecc_point, ECC_POINTS_SIZE);
        mbedtls_free(ecc_point);
        ecc_point = NULL;
    }
    return NULL;
}

uapi_drv_cipher_pke_ecc_point_t *ecp_point_malloc_with_value(uapi_drv_cipher_pke_ecc_curve_type_t curve_type,
    const mbedtls_ecp_point *P, size_t ksize)
{
    int ret;
    uapi_drv_cipher_pke_ecc_point_t *ecc_point = mbedtls_calloc(1, ECC_POINTS_SIZE);
    if (ecc_point == NULL) {
        return NULL;
    }

    ecc_point->x = mbedtls_calloc(1, ksize);
    if (ecc_point->x == NULL) {
        goto cleanup;
    }

    ecc_point->y = mbedtls_calloc(1, ksize);
    if (ecc_point->y == NULL) {
        goto cleanup;
    }

    ecc_point->length = ksize;
    if (curve_type == UAPI_DRV_CIPHER_PKE_ECC_TYPE_RFC7748 ||
        curve_type == UAPI_DRV_CIPHER_PKE_ECC_TYPE_RFC7748_448) {
        MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary_le(&(P->X), ecc_point->x, ksize));
        MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary_le(&(P->Y), ecc_point->y, ksize));
    } else {
        MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary(&(P->X), ecc_point->x, ksize));
        MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary(&(P->Y), ecc_point->y, ksize));
    }
    return ecc_point;
cleanup:
    if (ecc_point != NULL) {
        if (ecc_point->x != NULL) {
            mbedtls_platform_zeroize(ecc_point->x, ksize);
            mbedtls_free(ecc_point->x);
            ecc_point->x = NULL;
        }
        if (ecc_point->y != NULL) {
            mbedtls_platform_zeroize(ecc_point->y, ksize);
            mbedtls_free(ecc_point->y);
            ecc_point->y = NULL;
        }
        mbedtls_platform_zeroize(ecc_point, ECC_POINTS_SIZE);
        mbedtls_free(ecc_point);
        ecc_point = NULL;
    }
    return NULL;
}

void ecp_point_free(uapi_drv_cipher_pke_ecc_point_t *ecc_points, size_t ksize) // point free
{
    if (ecc_points == NULL) {
        return;
    }

    if (ecc_points->x != NULL) {
        mbedtls_platform_zeroize(ecc_points->x, ksize);
        mbedtls_free(ecc_points->x);
        ecc_points->x = NULL;
    }

    if (ecc_points->y != NULL) {
        mbedtls_platform_zeroize(ecc_points->y, ksize);
        mbedtls_free(ecc_points->y);
        ecc_points->y = NULL;
    }

    mbedtls_platform_zeroize(ecc_points, ECC_POINTS_SIZE);
    mbedtls_free(ecc_points);
}

#define ECC_DATA_SIZE       (sizeof(uapi_drv_cipher_pke_data_t))
uapi_drv_cipher_pke_data_t *ecp_data_malloc(const mbedtls_mpi *m, size_t ksize)
{
    int ret;
    uapi_drv_cipher_pke_data_t *ecc_data = mbedtls_calloc(1, ECC_DATA_SIZE);
    if (ecc_data == NULL) {
        return NULL;
    }

    ecc_data->data = mbedtls_calloc(1, ksize);
    if (ecc_data->data == NULL) {
        goto cleanup;
    }

    ecc_data->length = ksize;
    if (m != NULL) {
        MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary(m, ecc_data->data, ksize));
    }
    return ecc_data;
cleanup:
    if (ecc_data != NULL) {
        if (ecc_data->data != NULL) {
            mbedtls_platform_zeroize(ecc_data->data, ksize);
            mbedtls_free(ecc_data->data);
            ecc_data->data = NULL;
        }
        mbedtls_platform_zeroize(ecc_data, ECC_DATA_SIZE);
        mbedtls_free(ecc_data);
        ecc_data = NULL;
    }
    return NULL;
}

void ecp_data_free(uapi_drv_cipher_pke_data_t *ecc_data, size_t ksize)
{
    if (ecc_data == NULL) {
        return;
    }

    if (ecc_data->data != NULL) {
        mbedtls_platform_zeroize(ecc_data->data, ksize);
        mbedtls_free(ecc_data->data);
        ecc_data->data = NULL;
    }

    mbedtls_platform_zeroize(ecc_data, ECC_DATA_SIZE);
    mbedtls_free(ecc_data);
}

uapi_drv_cipher_pke_data_t *ecp_data_malloc_with_value(uapi_drv_cipher_pke_ecc_curve_type_t curve_type,
    const mbedtls_mpi *m, size_t ksize)
{
    int ret;
    uapi_drv_cipher_pke_data_t *ecc_data = mbedtls_calloc(1, ECC_DATA_SIZE);
    if (ecc_data == NULL) {
        return NULL;
    }

    ecc_data->data = mbedtls_calloc(1, ksize);
    if (ecc_data->data == NULL) {
        goto cleanup;
    }

    ecc_data->length = ksize;
    if (m != NULL) {
        if (curve_type == UAPI_DRV_CIPHER_PKE_ECC_TYPE_RFC7748 ||
            curve_type == UAPI_DRV_CIPHER_PKE_ECC_TYPE_RFC7748_448) {
            MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary_le(m, ecc_data->data, ksize));
        } else {
            MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary(m, ecc_data->data, ksize));
        }
    }
    return ecc_data;
cleanup:
    if (ecc_data != NULL) {
        if (ecc_data->data != NULL) {
            mbedtls_platform_zeroize(ecc_data->data, ksize);
            mbedtls_free(ecc_data->data);
            ecc_data->data = NULL;
        }
        mbedtls_platform_zeroize(ecc_data, ECC_DATA_SIZE);
        mbedtls_free(ecc_data);
        ecc_data = NULL;
    }
    return NULL;
}