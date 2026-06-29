/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: mbedtls ecdh alt implemention.
 *
*/
#include "library/common.h"

#if defined(MBEDTLS_ECDH_C)

#include "mbedtls/ecdh.h"
#include "mbedtls/platform.h"
#include "mbedtls/platform_util.h"
#include "mbedtls/error.h"
#include <string.h>

#include "mbedtls_ecp_func.h"

#include "mbedtls_perf_utils.h"

#if defined(MBEDTLS_ECDH_COMPUTE_SHARED_ALT)
int mbedtls_ecdh_compute_shared(mbedtls_ecp_group *grp, mbedtls_mpi *z,
    const mbedtls_ecp_point *Q, const mbedtls_mpi *d,
    int (*f_rng)(void *, unsigned char *, size_t), void *p_rng)
{
    int ret;
    uapi_drv_cipher_pke_ecc_curve_type_t curve_type;
    uapi_drv_cipher_pke_data_t *input_priv_key = NULL, *output_shared_key = NULL;
    uapi_drv_cipher_pke_ecc_point_t *input_pub_key = NULL;
    unsigned int klen = 0;

    MBEDTLS_PERF_START();
    (void)f_rng;
    (void)p_rng;

    if (grp == NULL || Q == NULL || d == NULL || z == NULL) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    mbedtls_alt_get_grp_type(grp->id, &curve_type);
    if (curve_type == UAPI_DRV_CIPHER_PKE_ECC_TYPE_INVALID) {
        ret = MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE;
        goto cleanup;
    }
    klen = mbedtls_ecp_get_klen(curve_type);

    input_pub_key = ecp_point_malloc_with_value(curve_type, Q, klen);
    if (input_pub_key == NULL) {
        ret = MBEDTLS_ERR_ECP_ALLOC_FAILED;
        goto cleanup;
    }

    input_priv_key = ecp_data_malloc_with_value(curve_type, d, klen);
    if (input_priv_key == NULL) {
        ret = MBEDTLS_ERR_ECP_ALLOC_FAILED;
        goto cleanup;
    }

    output_shared_key = ecp_data_malloc(NULL, klen);
    if (output_shared_key == NULL) {
        ret = MBEDTLS_ERR_ECP_ALLOC_FAILED;
        goto cleanup;
    }

    ret = uapi_drv_cipher_pke_ecc_gen_ecdh_key(curve_type, input_pub_key, input_priv_key, output_shared_key);
    if (ret != ERRCODE_SUCC) {
        mbedtls_printf("mbedtls ecdh failed, ret=0x%x\n", ret);
        goto cleanup;
    }

    if (curve_type == UAPI_DRV_CIPHER_PKE_ECC_TYPE_RFC7748 ||
        curve_type == UAPI_DRV_CIPHER_PKE_ECC_TYPE_RFC7748_448) {
        ret = mbedtls_mpi_read_binary_le(z, output_shared_key->data, output_shared_key->length);
    } else {
        ret = mbedtls_mpi_read_binary(z, output_shared_key->data, output_shared_key->length);
    }

cleanup:
    ecp_data_free(output_shared_key, klen);
    ecp_data_free(input_priv_key, klen);
    ecp_point_free(input_pub_key, klen);
    MBEDTLS_PERF_ECDSA_END(grp->id, MBEDTLS_PERF_ECDSA_OP_ECDH);
    return ret;
}
#endif /* MBEDTLS_ECDH_COMPUTE_SHARED_ALT */
#endif /* MBEDTLS_ECDH_C */