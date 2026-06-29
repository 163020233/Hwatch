/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: mbedtls alt ecdsa implement.
 *
*/

#include "library/common.h"

#if defined(MBEDTLS_ECDSA_C)
#include "mbedtls/ecdsa.h"
#include <string.h>
#include "mbedtls/platform.h"
#include "mbedtls/platform_util.h"
#include "mbedtls/error.h"

#include "mbedtls_ecp_func.h"

#include "mbedtls_perf_utils.h"

#if defined(MBEDTLS_ECDSA_SIGN_ALT)
/*
 * Compute ECDSA signature of a hashed message
 */
int mbedtls_ecdsa_sign(mbedtls_ecp_group *grp, mbedtls_mpi *r, mbedtls_mpi *s,
                       const mbedtls_mpi *d, const unsigned char *buf, size_t blen,
                       int (*f_rng)(void *, unsigned char *, size_t), void *p_rng)
{
    int ret;
    unsigned int klen = 0;
    uapi_drv_cipher_pke_ecc_curve_type_t curve_type;
    uapi_drv_cipher_pke_data_t *priv_key = NULL, *r_data = NULL, *s_data = NULL;
    uapi_drv_cipher_pke_data_t hash_data = { 0 };
    uapi_drv_cipher_pke_ecc_sig_t sig = { 0 };

    MBEDTLS_PERF_START();
    (void)f_rng;
    (void)p_rng;
    if (grp == NULL || r == NULL || s == NULL || d == NULL) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    if (buf == NULL && blen != 0) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    mbedtls_alt_get_grp_type(grp->id, &curve_type);
    if (curve_type == UAPI_DRV_CIPHER_PKE_ECC_TYPE_INVALID) {
        ret = MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE;
        goto cleanup;
    }
    klen = mbedtls_ecp_get_klen(curve_type);

    priv_key = ecp_data_malloc_with_value(curve_type, d, klen);
    if (priv_key == NULL) {
        ret = MBEDTLS_ERR_ECP_ALLOC_FAILED;
        goto cleanup;
    }

    r_data = ecp_data_malloc(NULL, klen);
    if (r_data == NULL) {
        ret = MBEDTLS_ERR_ECP_ALLOC_FAILED;
        goto cleanup;
    }

    s_data = ecp_data_malloc(NULL, klen);
    if (s_data == NULL) {
        ret = MBEDTLS_ERR_ECP_ALLOC_FAILED;
        goto cleanup;
    }

    hash_data.data = (unsigned char *)buf;
    hash_data.length = blen;

    sig.r = r_data->data;
    sig.s = s_data->data;
    sig.length = r_data->length;

    ret = uapi_drv_cipher_pke_ecdsa_sign(curve_type, priv_key, &hash_data, &sig);
    if (ret != ERRCODE_SUCC) {
        mbedtls_printf("mbedtls ecdsa sign failed ,ret=0x%x\n", ret);
        ret = MBEDTLS_ERR_ERROR_GENERIC_ERROR;
        goto cleanup;
    }

    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(r, r_data->data, klen));
    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(s, s_data->data, klen));
cleanup:
    ecp_data_free(priv_key, klen);
    ecp_data_free(r_data, klen);
    ecp_data_free(s_data, klen);
    MBEDTLS_PERF_ECDSA_END(grp->id, MBEDTLS_PERF_ECDSA_OP_SIGN);
    return ret;
}
#endif /* !MBEDTLS_ECDSA_SIGN_ALT */

#if defined(MBEDTLS_ECDSA_VERIFY_ALT)
/*
 * Verify ECDSA signature of hashed message
 */
int mbedtls_ecdsa_verify(mbedtls_ecp_group *grp,
                         const unsigned char *buf, size_t blen,
                         const mbedtls_ecp_point *Q,
                         const mbedtls_mpi *r,
                         const mbedtls_mpi *s)
{
    int ret;
    unsigned int klen = 0;
    uapi_drv_cipher_pke_ecc_curve_type_t curve_type;
    uapi_drv_cipher_pke_ecc_point_t *pub_key = NULL;
    uapi_drv_cipher_pke_data_t *r_data = NULL, *s_data = NULL;
    uapi_drv_cipher_pke_data_t hash_data = { 0 };
    uapi_drv_cipher_pke_ecc_sig_t sig = { 0 };

    MBEDTLS_PERF_START();
    if (grp == NULL || Q == NULL || r == NULL || s == NULL) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    if (buf == NULL && blen != 0) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    mbedtls_alt_get_grp_type(grp->id, &curve_type);
    if (curve_type == UAPI_DRV_CIPHER_PKE_ECC_TYPE_INVALID) {
        ret = MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE;
        goto cleanup;
    }
    klen = mbedtls_ecp_get_klen(curve_type);

    pub_key = ecp_point_malloc_with_value(curve_type, Q, klen);
    if (pub_key == NULL) {
        ret = MBEDTLS_ERR_ECP_ALLOC_FAILED;
        goto cleanup;
    }

    r_data = ecp_data_malloc(r, klen);
    if (r_data == NULL) {
        ret = MBEDTLS_ERR_ECP_ALLOC_FAILED;
        goto cleanup;
    }

    s_data = ecp_data_malloc(s, klen);
    if (s_data == NULL) {
        ret = MBEDTLS_ERR_ECP_ALLOC_FAILED;
        goto cleanup;
    }

    hash_data.data = (unsigned char *)buf;
    hash_data.length = blen;

    sig.r = r_data->data;
    sig.s = s_data->data;
    sig.length = r_data->length;

    ret = uapi_drv_cipher_pke_ecdsa_verify(curve_type, pub_key, &hash_data, &sig);
    if (ret != ERRCODE_SUCC) {
        mbedtls_printf("mbedtls ecdsa verify failed ,ret=0x%x\n", ret);
        ret = MBEDTLS_ERR_ERROR_GENERIC_ERROR;
        goto cleanup;
    }
cleanup:
    ecp_point_free(pub_key, klen);
    ecp_data_free(s_data, klen);
    ecp_data_free(r_data, klen);
    MBEDTLS_PERF_ECDSA_END(grp->id, MBEDTLS_PERF_ECDSA_OP_VERIFY);
    return ret;
}
#endif /* MBEDTLS_ECDSA_VERIFY_ALT */
#if defined(MBEDTLS_ECDSA_GENKEY_ALT)
/*
 * Generate key pair
 */
int mbedtls_ecdsa_genkey(mbedtls_ecdsa_context *ctx, mbedtls_ecp_group_id gid,
                         int (*f_rng)(void *, unsigned char *, size_t), void *p_rng)
{
    int ret;
    unsigned int klen = 0;
    uapi_drv_cipher_pke_ecc_curve_type_t curve_type;
    uapi_drv_cipher_pke_ecc_point_t *output_pub_key = NULL;
    uapi_drv_cipher_pke_data_t *output_priv_key = NULL;

    MBEDTLS_PERF_START();
    (void)f_rng;
    (void)p_rng;

    if (ctx == NULL) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    mbedtls_alt_get_grp_type(gid, &curve_type);
    if (curve_type == UAPI_DRV_CIPHER_PKE_ECC_TYPE_INVALID) {
        ret = MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE;
        goto cleanup;
    }
    klen = mbedtls_ecp_get_klen(curve_type);

    ret = mbedtls_ecp_group_load(&ctx->grp, gid);
    if (ret != 0) {
        return ret;
    }

    output_priv_key = ecp_data_malloc(NULL, klen);
    if (output_priv_key == NULL) {
        ret = MBEDTLS_ERR_ECP_ALLOC_FAILED;
        goto cleanup;
    }

    output_pub_key = ecp_point_malloc(NULL, klen);
    if (output_pub_key == NULL) {
        ret = MBEDTLS_ERR_ECP_ALLOC_FAILED;
        goto cleanup;
    }

    ret = uapi_drv_cipher_pke_ecc_gen_key(curve_type, NULL, output_priv_key, output_pub_key);
    if (ret != 0) {
        mbedtls_printf("mbedtls gen key failed, ret=0x%x\n", ret);
        ret = MBEDTLS_ERR_ERROR_GENERIC_ERROR;
        goto cleanup;
    }

    ret = mbedtls_mpi_read_binary(&ctx->d, output_priv_key->data, klen);
    if (ret != 0) {
        goto cleanup;
    }
    ret = mbedtls_mpi_read_binary(&ctx->Q.X, output_pub_key->x, klen);
    if (ret != 0) {
        goto cleanup;
    }
    ret = mbedtls_mpi_read_binary(&ctx->Q.Y, output_pub_key->y, klen);
    if (ret != 0) {
        goto cleanup;
    }
    ret = mbedtls_mpi_lset(&ctx->Q.Z, 1);
    if (ret != 0) {
        goto cleanup;
    }
    ret = 0;
cleanup:
    ecp_data_free(output_priv_key, klen);
    ecp_point_free(output_pub_key, klen);
    MBEDTLS_PERF_ECDSA_END(gid, MBEDTLS_PERF_ECDSA_OP_GENKEY);
    return ret;
}
#endif /* MBEDTLS_ECDSA_GENKEY_ALT */

#endif /* MBEDTLS_ECDSA_C */

