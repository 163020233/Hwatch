/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: mbedtls alt ecp implemention.
 *
*/
#include "library/common.h"

#if defined(MBEDTLS_ECP_C) && defined(MBEDTLS_ECP_ALT)
#include "mbedtls/platform.h"
#include "mbedtls/platform_util.h"
#include "mbedtls/error.h"
#include "bn_mul.h"
#include "security_pke_pri.h"
#include "security_pke.h"
#include "mbedtls_ecp_func.h"

#include "mbedtls_perf_utils.h"

/*-------------------------------LOOKUP TABLE----------------------------------*/
static const mbedtls_ecp_curve_info ecp_supported_curves[] = {
#if defined(MBEDTLS_ECP_DP_SECP521R1_ENABLED)
    { MBEDTLS_ECP_DP_SECP521R1,    25,     521,    "secp521r1"         },
#endif
#if defined(MBEDTLS_ECP_DP_BP512R1_ENABLED)
    { MBEDTLS_ECP_DP_BP512R1,      28,     512,    "brainpoolP512r1"   },
#endif
#if defined(MBEDTLS_ECP_DP_SECP384R1_ENABLED)
    { MBEDTLS_ECP_DP_SECP384R1,    24,     384,    "secp384r1"         },
#endif
#if defined(MBEDTLS_ECP_DP_BP384R1_ENABLED)
    { MBEDTLS_ECP_DP_BP384R1,      27,     384,    "brainpoolP384r1"   },
#endif
#if defined(MBEDTLS_ECP_DP_SECP256R1_ENABLED)
    { MBEDTLS_ECP_DP_SECP256R1,    23,     256,    "secp256r1"         },
#endif
#if defined(MBEDTLS_ECP_DP_SECP256K1_ENABLED)
    { MBEDTLS_ECP_DP_SECP256K1,    22,     256,    "secp256k1"         },
#endif
#if defined(MBEDTLS_ECP_DP_BP256R1_ENABLED)
    { MBEDTLS_ECP_DP_BP256R1,      26,     256,    "brainpoolP256r1"   },
#endif
#if defined(MBEDTLS_ECP_DP_SECP224R1_ENABLED)
    { MBEDTLS_ECP_DP_SECP224R1,    21,     224,    "secp224r1"         },
#endif
#if defined(MBEDTLS_ECP_DP_SECP224K1_ENABLED)
    { MBEDTLS_ECP_DP_SECP224K1,    20,     224,    "secp224k1"         },
#endif
#if defined(MBEDTLS_ECP_DP_SECP192R1_ENABLED)
    { MBEDTLS_ECP_DP_SECP192R1,    19,     192,    "secp192r1"         },
#endif
#if defined(MBEDTLS_ECP_DP_SECP192K1_ENABLED)
    { MBEDTLS_ECP_DP_SECP192K1,    18,     192,    "secp192k1"         },
#endif
#if defined(MBEDTLS_ECP_DP_CURVE25519_ENABLED)
    { MBEDTLS_ECP_DP_CURVE25519,   29,     256,    "x25519"            },
#endif
#if defined(MBEDTLS_ECP_DP_CURVE448_ENABLED)
    { MBEDTLS_ECP_DP_CURVE448,     30,     448,    "x448"              },
#endif
    { MBEDTLS_ECP_DP_NONE,          0,     0,      NULL                },
};

static mbedtls_ecp_group_id ecp_supported_grp_id[] = {
#if defined(MBEDTLS_ECP_DP_SECP521R1_ENABLED)
    MBEDTLS_ECP_DP_SECP521R1,
#endif
#if defined(MBEDTLS_ECP_DP_BP512R1_ENABLED)
    MBEDTLS_ECP_DP_BP512R1,
#endif
#if defined(MBEDTLS_ECP_DP_SECP384R1_ENABLED)
    MBEDTLS_ECP_DP_SECP384R1,
#endif
#if defined(MBEDTLS_ECP_DP_BP384R1_ENABLED)
    MBEDTLS_ECP_DP_BP384R1,
#endif
#if defined(MBEDTLS_ECP_DP_SECP256R1_ENABLED)
    MBEDTLS_ECP_DP_SECP256R1,
#endif
#if defined(MBEDTLS_ECP_DP_SECP256K1_ENABLED)
    MBEDTLS_ECP_DP_SECP256K1,
#endif
#if defined(MBEDTLS_ECP_DP_BP256R1_ENABLED)
    MBEDTLS_ECP_DP_BP256R1,
#endif
#if defined(MBEDTLS_ECP_DP_SECP224R1_ENABLED)
    MBEDTLS_ECP_DP_SECP224R1,
#endif
#if defined(MBEDTLS_ECP_DP_SECP224K1_ENABLED)
    MBEDTLS_ECP_DP_SECP224K1,
#endif
#if defined(MBEDTLS_ECP_DP_SECP192R1_ENABLED)
    MBEDTLS_ECP_DP_SECP192R1,
#endif
#if defined(MBEDTLS_ECP_DP_SECP192K1_ENABLED)
    MBEDTLS_ECP_DP_SECP192K1,
#endif
#if defined(MBEDTLS_ECP_DP_CURVE25519_ENABLED)
    MBEDTLS_ECP_DP_CURVE25519,
#endif
#if defined(MBEDTLS_ECP_DP_CURVE448_ENABLED)
    MBEDTLS_ECP_DP_CURVE448,
#endif
    MBEDTLS_ECP_DP_NONE,
};
#define ECP_CURVES_NUMBER                  (sizeof(ecp_supported_curves) / sizeof(ecp_supported_curves[0]))
#define BITS_TO_BYTES(x)                   (((x) + 7) >> 3)
#define ECP_CURVE25519_KEY_SIZE            32
#define ECP_CURVE448_KEY_SIZE              56

#define ECP_LOW_ORDER_0_BIT                0
#define ECP_LOW_ORDER_1_BIT                1
#define ECP_LOW_ORDER_2_BIT                2
#define ECP_BITS_OF_BITE                   8
#define ECP_CURVE25519_HIGH_ORDER_0_BIT    (ECP_CURVE25519_KEY_SIZE * ECP_BITS_OF_BITE - 1)
#define ECP_CURVE25519_HIGH_ORDER_1_BIT    (ECP_CURVE25519_KEY_SIZE * ECP_BITS_OF_BITE - 2)
#define ECP_CURVE448_HIGH_ORDER_0_BIT      (ECP_CURVE448_KEY_SIZE * ECP_BITS_OF_BITE - 1)

#define ECP_CURVE25519_NBIT_SIZE           254
#define ECP_CURVE448_NBIT_SIZE             447

#define MBEDTLS_ECP_TRUE                   1
#define MBEDTLS_ECP_FALSE                  0

#define ECP_CHK_RETURN(ret) \
    do { \
        if ((ret) != 0) { \
            return (ret); \
        } \
    } while (0)
/*-------------------------------LOOKUP TABLE----------------------------------*/

/*-------------------------------- COMPARE / COPY ---------------------------------*/
int mbedtls_ecp_copy(mbedtls_ecp_point *P, const mbedtls_ecp_point *Q)
{
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    MBEDTLS_PERF_START_WITH_ID(MBEDTLS_PERF_TIMER_ID_0);
    if (P == NULL || Q == NULL) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    ret = mbedtls_mpi_copy(&P->X, &Q->X);
    if (ret != 0) {
        return ret;
    }

    ret = mbedtls_mpi_copy(&P->Y, &Q->Y);
    if (ret != 0) {
        return ret;
    }

    ret = mbedtls_mpi_copy(&P->Z, &Q->Z);
    if (ret != 0) {
        return ret;
    }

    MBEDTLS_PERF_ECP_END1_WITH_ID(MBEDTLS_PERF_TIMER_ID_0, MBEDTLS_PERF_ECP_TYPE_COPY);
    return 0;
}

int mbedtls_ecp_group_copy(mbedtls_ecp_group *dst, const mbedtls_ecp_group *src)
{
    int ret;
    MBEDTLS_PERF_START_WITH_ID(MBEDTLS_PERF_TIMER_ID_2);
    if (dst == NULL || src == NULL) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    if (dst == src) {
        return 0;
    }

    if (src->id == MBEDTLS_ECP_DP_NONE) {
        return MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE;
    }
    mbedtls_ecp_group_init(dst);

    dst->id = src->id;
    dst->nbits = src->nbits;
    dst->pbits = src->pbits;

    if ((ret = mbedtls_mpi_copy(&dst->P, &src->P)) != 0 ||
        (ret = mbedtls_mpi_copy(&dst->A, &src->A)) != 0 ||
        (ret = mbedtls_mpi_copy(&dst->B, &src->B)) != 0 ||
        (ret = mbedtls_mpi_copy(&dst->N, &src->N)) != 0 ||
        (ret = mbedtls_ecp_copy(&dst->G, &src->G)) != 0) {
        goto cleanup;
    } else {
        MBEDTLS_PERF_ECP_END1_WITH_ID(MBEDTLS_PERF_TIMER_ID_2, MBEDTLS_PERF_ECP_TYPE_GROUP_COPY);
        return 0;
    }
cleanup:
    mbedtls_ecp_group_free(dst);
    return ret;
}

int mbedtls_ecp_point_cmp(const mbedtls_ecp_point *P,
                          const mbedtls_ecp_point *Q)
{
    MBEDTLS_PERF_START_WITH_ID(MBEDTLS_PERF_TIMER_ID_0);
    if (P == NULL || Q == NULL) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    if (P == Q) {
        return 0;
    }

    if (mbedtls_mpi_cmp_mpi(&P->X, &Q->X) == 0 &&
        mbedtls_mpi_cmp_mpi(&P->Y, &Q->Y) == 0 &&
        mbedtls_mpi_cmp_mpi(&P->Z, &Q->Z) == 0) {
        MBEDTLS_PERF_ECP_END1_WITH_ID(MBEDTLS_PERF_TIMER_ID_0, MBEDTLS_PERF_ECP_TYPE_POINT_CMP);
        return 0;
    }

    return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
}

int mbedtls_ecp_is_zero(mbedtls_ecp_point *pt)
{
    MBEDTLS_PERF_START_WITH_ID(MBEDTLS_PERF_TIMER_ID_0);
    if (pt == NULL) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }
    if (mbedtls_mpi_cmp_int(&pt->Z, 0) == 0) {
        MBEDTLS_PERF_ECP_END1_WITH_ID(MBEDTLS_PERF_TIMER_ID_0, MBEDTLS_PERF_ECP_TYPE_IS_ZERO);
        return MBEDTLS_ECP_TRUE;
    } else {
        MBEDTLS_PERF_ECP_END1_WITH_ID(MBEDTLS_PERF_TIMER_ID_0, MBEDTLS_PERF_ECP_TYPE_IS_ZERO);
        return MBEDTLS_ECP_FALSE;
    }
}

/*-------------------------------- COMPARE / COPY ---------------------------------*/

/*--------------------------------- CALCULATE -------------------------------------*/

/*
 *  uapi_drv_cipher_pke_ecc_point_t -> mbedtls_ecp_point
 */
static int inner_read_point(const uapi_drv_cipher_pke_ecc_point_t *point_src, mbedtls_ecp_point *point_dst)
{
    int ret = 0;

    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&point_dst->X, point_src->x, point_src->length));
    if (point_src->y != NULL) {
        MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&point_dst->Y, point_src->y, point_src->length));
    }
    MBEDTLS_MPI_CHK(mbedtls_mpi_lset(&point_dst->Z, 1));
cleanup:
    return ret;
}

/*
 *  multiplication R = m * P
 */
int mbedtls_ecp_mul(mbedtls_ecp_group *grp, mbedtls_ecp_point *R,
                    const mbedtls_mpi *m, const mbedtls_ecp_point *P,
                    int (*f_rng)(void *, unsigned char *, size_t), void *p_rng)
{
    return mbedtls_ecp_mul_restartable(grp, R, m, P, f_rng, p_rng, NULL);
}

/*  R = m * P + n * Q */
int mbedtls_ecp_muladd(mbedtls_ecp_group *grp, mbedtls_ecp_point *R,
                       const mbedtls_mpi *m, const mbedtls_ecp_point *P,
                       const mbedtls_mpi *n, const mbedtls_ecp_point *Q)
{
    return mbedtls_ecp_muladd_restartable(grp, R, m, P, n, Q, NULL);
}

static void inner_little_big_endian_trans(uint8_t *buffer, size_t size)
{
    uint8_t *start = buffer;
    uint8_t *end = start + size - 1;
    uint8_t temp;
    while (start < end) {
        temp = *start;
        *start = *end;
        *end = temp;
        start++;
        end--;
    }
}

int mbedtls_ecp_mul_restartable(mbedtls_ecp_group *grp, mbedtls_ecp_point *R,
                                const mbedtls_mpi *m, const mbedtls_ecp_point *P,
                                mbedtls_f_rng_t *f_rng, void *p_rng,
                                mbedtls_ecp_restart_ctx *rs_ctx)
{
    uapi_drv_cipher_pke_ecc_curve_type_t curve_type;
    uapi_drv_cipher_pke_data_t *m_data = NULL;
    uapi_drv_cipher_pke_ecc_point_t *p_point = NULL, *r_point = NULL;

    errcode_t ret;
    size_t ksize = 0;
    MBEDTLS_PERF_START_WITH_ID(MBEDTLS_PERF_TIMER_ID_2);

    (void)f_rng;
    (void)p_rng;
    /* restart is not supported */
    (void)rs_ctx;

    if (grp == NULL || R == NULL || m == NULL || P == NULL) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }
    /* step1 :check public key and private key valid */
    MBEDTLS_MPI_CHK(mbedtls_ecp_check_privkey(grp, m));
    MBEDTLS_MPI_CHK(mbedtls_ecp_check_pubkey(grp, P));

    mbedtls_alt_get_grp_type(grp->id, &curve_type);
    if (curve_type == UAPI_DRV_CIPHER_PKE_ECC_TYPE_INVALID) {
        return MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE;
    }

    /* step2 :get buffer and store data */
    ksize = mbedtls_ecp_get_klen(curve_type);

    p_point = ecp_point_malloc(P, ksize);
    if (p_point == NULL) {
        ret = MBEDTLS_ERR_ECP_ALLOC_FAILED;
        goto cleanup;
    }

    r_point = ecp_point_malloc(NULL, ksize);
    if (r_point == NULL) {
        ret = MBEDTLS_ERR_ECP_ALLOC_FAILED;
        goto cleanup;
    }

    m_data = ecp_data_malloc(m, ksize);
    if (m_data == NULL) {
        ret = MBEDTLS_ERR_ECP_ALLOC_FAILED;
        goto cleanup;
    }

    /* The data for curve 25519 and curve 448 requires endianness conversion. */
    if (curve_type == UAPI_DRV_CIPHER_PKE_ECC_TYPE_RFC7748 ||
        curve_type == UAPI_DRV_CIPHER_PKE_ECC_TYPE_RFC7748_448) {
        inner_little_big_endian_trans(p_point->x, ksize);
        inner_little_big_endian_trans(m_data->data, ksize);
    }

    /* step3. get R = m * P */
    ret = uapi_drv_cipher_pke_mul_dot(curve_type, m_data, p_point, r_point);
    if (ret != 0) {
        mbedtls_printf("mbedtls_ecp_mul failed, ret=0x%x\n", ret);
        ret = MBEDTLS_ERR_ERROR_GENERIC_ERROR;
        goto cleanup;
    }

    if (curve_type == UAPI_DRV_CIPHER_PKE_ECC_TYPE_RFC7748 ||
        curve_type == UAPI_DRV_CIPHER_PKE_ECC_TYPE_RFC7748_448) {
        inner_little_big_endian_trans(r_point->x, ksize);
    }

    ret = inner_read_point(r_point, R);
    if (ret != 0) {
        mbedtls_printf("mbedtls_ecp mul read point failed, ret=0x%x\n", ret);
        ret = MBEDTLS_ERR_ERROR_GENERIC_ERROR;
        goto cleanup;
    }
    ret = 0;
cleanup:
    ecp_point_free(p_point, ksize);
    ecp_point_free(r_point, ksize);
    p_point = r_point = NULL;
    ecp_data_free(m_data, ksize);
    m_data = NULL;
    MBEDTLS_PERF_ECP_END2_WITH_ID(MBEDTLS_PERF_TIMER_ID_2, grp->id, MBEDTLS_PERF_ECP_TYPE_MUL);
    return ret;
}

int mbedtls_ecp_muladd_restartable(
    mbedtls_ecp_group *grp, mbedtls_ecp_point *R,
    const mbedtls_mpi *m, const mbedtls_ecp_point *P,
    const mbedtls_mpi *n, const mbedtls_ecp_point *Q,
    mbedtls_ecp_restart_ctx *rs_ctx)
{
    uapi_drv_cipher_pke_ecc_curve_type_t curve_type;
    uapi_drv_cipher_pke_data_t *m_data = NULL, *n_data = NULL;
    uapi_drv_cipher_pke_ecc_point_t *p_point = NULL, *q_point = NULL, *r_point = NULL;
    errcode_t ret;
    size_t ksize = 0;
    MBEDTLS_PERF_START_WITH_ID(MBEDTLS_PERF_TIMER_ID_2);

    if (grp == NULL || R == NULL || m == NULL ||
        P == NULL || n == NULL || Q == NULL) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    MBEDTLS_MPI_CHK(mbedtls_ecp_check_privkey(grp, m));
    MBEDTLS_MPI_CHK(mbedtls_ecp_check_privkey(grp, n));
    MBEDTLS_MPI_CHK(mbedtls_ecp_check_pubkey(grp, Q));
    MBEDTLS_MPI_CHK(mbedtls_ecp_check_pubkey(grp, P));

    (void)rs_ctx;
    if (mbedtls_ecp_get_type(grp) == MBEDTLS_ECP_TYPE_MONTGOMERY) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    mbedtls_alt_get_grp_type(grp->id, &curve_type);
    ksize = mbedtls_ecp_get_klen(curve_type);

    p_point = ecp_point_malloc(P, ksize);
    if (p_point == NULL) {
        ret = MBEDTLS_ERR_ECP_ALLOC_FAILED;
        goto cleanup;
    }

    q_point = ecp_point_malloc(Q, ksize);
    if (q_point == NULL) {
        ret = MBEDTLS_ERR_ECP_ALLOC_FAILED;
        goto cleanup;
    }

    r_point = ecp_point_malloc(NULL, ksize);
    if (r_point == NULL) {
        ret = MBEDTLS_ERR_ECP_ALLOC_FAILED;
        goto cleanup;
    }

    m_data = ecp_data_malloc(m, ksize);
    if (m_data == NULL) {
        ret = MBEDTLS_ERR_ECP_ALLOC_FAILED;
        goto cleanup;
    }

    n_data = ecp_data_malloc(n, ksize);
    if (n_data == NULL) {
        ret = MBEDTLS_ERR_ECP_ALLOC_FAILED;
        goto cleanup;
    }

    /* R = m P + n Q */
    ret = uapi_drv_cipher_pke_mul_dot_add(curve_type, m_data, n_data, p_point, q_point, r_point);
    if (ret != ERRCODE_SUCC) {
        mbedtls_printf("mbedtls_mul_add failed, ret=0x%x, curve_type is 0x%x\n", ret, curve_type);
        ret = MBEDTLS_ERR_ERROR_GENERIC_ERROR;
        goto cleanup;
    }

    MBEDTLS_MPI_CHK(inner_read_point(r_point, R));
    ret = 0;
cleanup:
    ecp_point_free(p_point, ksize);
    ecp_point_free(q_point, ksize);
    ecp_point_free(r_point, ksize);
    p_point = q_point = r_point = NULL;
    ecp_data_free(m_data, ksize);
    ecp_data_free(n_data, ksize);
    m_data = n_data = NULL;
    MBEDTLS_PERF_ECP_END2_WITH_ID(MBEDTLS_PERF_TIMER_ID_2, grp->id, MBEDTLS_PERF_ECP_TYPE_MULADD);
    return ret;
}

/*--------------------------------- CALCULATE -------------------------------------*/

/*--------------------------------- CHECK KEY -------------------------------------*/
int mbedtls_ecp_check_pub_priv(const mbedtls_ecp_keypair *pub, const mbedtls_ecp_keypair *prv,
    int (*f_rng)(void *, unsigned char *, size_t), void *p_rng)
{
    errcode_t ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    mbedtls_ecp_point Q;
    mbedtls_ecp_group grp;

    MBEDTLS_PERF_START_WITH_ID(MBEDTLS_PERF_TIMER_ID_3);
    if (pub == NULL || prv == NULL) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    if (pub->grp.id == MBEDTLS_ECP_DP_NONE) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    if (pub->grp.id != prv->grp.id) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    /* step 1. compare pub->Q , prv->Q */
    if ((ret = mbedtls_ecp_point_cmp(&pub->Q, &prv->Q)) != 0) {
        return ret;
    }

    mbedtls_ecp_point_init(&Q);
    mbedtls_ecp_group_init(&grp);

    mbedtls_ecp_group_copy(&grp, &prv->grp);

    /* step2: check private key valid , calculate Q = prv->d * G */
    MBEDTLS_MPI_CHK(mbedtls_ecp_mul(&grp, &Q, &prv->d, &prv->grp.G, f_rng, p_rng));

    /* step3: compare Q , prv->Q */
    if ((ret = mbedtls_ecp_point_cmp(&Q, &prv->Q)) != 0) {
        ret = MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
        goto cleanup;
    }

cleanup:
    mbedtls_ecp_point_free(&Q);
    mbedtls_ecp_group_free(&grp);
    MBEDTLS_PERF_ECP_END2_WITH_ID(MBEDTLS_PERF_TIMER_ID_3, grp.id, MBEDTLS_PERF_ECP_TYPE_CHECK_PUB_PRIV);
    return ret;
}

#if defined(MBEDTLS_ECP_MONTGOMERY_ENABLED)
#if defined(MBEDTLS_ECP_DP_CURVE25519_ENABLED)
#define ECP_CURVE25519_BAD_POINT_SIZE   32
/* X25519 bad point #1, little-endian 32 bytes */
static const unsigned char x25519_bad_point_1_le[ECP_CURVE25519_BAD_POINT_SIZE] = {
    0xe0, 0xeb, 0x7a, 0x7c, 0x3b, 0x41, 0xb8, 0xae,
    0x16, 0x56, 0xe3, 0xfa, 0xf1, 0x9f, 0xc4, 0x6a,
    0xda, 0x09, 0x8d, 0xeb, 0x9c, 0x32, 0xb1, 0xfd,
    0x86, 0x62, 0x05, 0x16, 0x5f, 0x49, 0xb8, 0x00
};
/* X25519 bad point #2, little-endian 32 bytes */
static const unsigned char x25519_bad_point_2_le[ECP_CURVE25519_BAD_POINT_SIZE] = {
    0x5f, 0x9c, 0x95, 0xbc, 0xa3, 0x50, 0x8c, 0x24,
    0xb1, 0xd0, 0xb1, 0x55, 0x9c, 0x83, 0xef, 0x5b,
    0x04, 0x44, 0x5c, 0xc4, 0x58, 0x1c, 0x8e, 0x86,
    0xd8, 0x22, 0x4e, 0xdd, 0xd0, 0x9f, 0x11, 0x57
};
#endif /* MBEDTLS_ECP_DP_CURVE25519_ENABLED */

/*
 * check bad point 0, 1, -1 (mod p)
 * curve25519 bad point reference: https://cr.yp.to/ecdh.html#validate, https://eprint.iacr.org/2017/806.pdf
 */
static int inner_check_montgomery_bad_points(const mbedtls_mpi *X, const mbedtls_mpi *P,
                                             const mbedtls_ecp_group_id grp_id)
{
    int ret;
    mbedtls_mpi x_tmp, bad_point_1, bad_point_2;

    mbedtls_mpi_init(&x_tmp);
    mbedtls_mpi_init(&bad_point_1);
    mbedtls_mpi_init(&bad_point_2);
    MBEDTLS_MPI_CHK(mbedtls_mpi_copy(&x_tmp, X));
    MBEDTLS_MPI_CHK(mbedtls_mpi_mod_mpi(&x_tmp, &x_tmp, P));

    /* Step 1. check 0 and 1 */
    if (mbedtls_mpi_cmp_int(&x_tmp, 1) <= 0) {
        ret = MBEDTLS_ERR_ECP_INVALID_KEY;
        goto cleanup;
    }

#if defined(MBEDTLS_ECP_DP_CURVE25519_ENABLED)
    if (grp_id == MBEDTLS_ECP_DP_CURVE25519) {
        /* Step 2. check curve25519 bad point */
        MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary_le(&bad_point_1,
                                                   x25519_bad_point_1_le,
                                                   ECP_CURVE25519_BAD_POINT_SIZE));
        MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary_le(&bad_point_2,
                                                   x25519_bad_point_2_le,
                                                   ECP_CURVE25519_BAD_POINT_SIZE));
        if (mbedtls_mpi_cmp_mpi(&x_tmp, &bad_point_1) == 0) {
            ret = MBEDTLS_ERR_ECP_INVALID_KEY;
            goto cleanup;
        }

        if (mbedtls_mpi_cmp_mpi(&x_tmp, &bad_point_2) == 0) {
            ret = MBEDTLS_ERR_ECP_INVALID_KEY;
            goto cleanup;
        }
    }
#else
    (void) grp_id;
#endif

    /* Step 3. check P - 1 */
    MBEDTLS_MPI_CHK(mbedtls_mpi_add_int(&x_tmp, &x_tmp, 1));
    if (mbedtls_mpi_cmp_mpi(&x_tmp, P) == 0) {
        ret = MBEDTLS_ERR_ECP_INVALID_KEY;
        goto cleanup;
    }
    ret = 0;

cleanup:
    mbedtls_mpi_free(&x_tmp);
    mbedtls_mpi_free(&bad_point_1);
    mbedtls_mpi_free(&bad_point_2);
    return ret;
}

/*
 * Check validity of a public key
 */
static int inner_check_montgomery_pubkey(const mbedtls_ecp_group *grp, const mbedtls_ecp_point *pt)
{
    if (mbedtls_mpi_size(&pt->X) > BITS_TO_BYTES(grp->nbits)) {
        return MBEDTLS_ERR_ECP_INVALID_KEY;
    }
 
    if (mbedtls_mpi_cmp_int(&pt->X, 0) < 0) {
        return MBEDTLS_ERR_ECP_INVALID_KEY;
    }

    return inner_check_montgomery_bad_points(&pt->X, &grp->P, grp->id);
}

/*
* reference :RFC7748 https://rfc-editor.org/rfc/rfc7748.html
*/
static int inner_check_montgomery_privkey(const mbedtls_ecp_group *grp, const mbedtls_mpi *d)
{
      /* Check low-order bits and key length according to RFC7748 section 5 paragraph 5 */
    if (mbedtls_mpi_get_bit(d, ECP_LOW_ORDER_0_BIT) != 0 ||
        mbedtls_mpi_get_bit(d, ECP_LOW_ORDER_1_BIT) != 0 ||
        mbedtls_mpi_bitlen(d) != grp->nbits + 1) {
        return MBEDTLS_ERR_ECP_INVALID_KEY;
    }
    /* Additional check for Curve25519: verify low-order bit 2 is zero  */
    if (grp->nbits == ECP_CURVE25519_NBIT_SIZE &&
        mbedtls_mpi_get_bit(d, ECP_LOW_ORDER_2_BIT) != 0) {
        return MBEDTLS_ERR_ECP_INVALID_KEY;
    }
    return 0;
}
#endif /* MBEDTLS_ECP_MONTGOMERY_ENABLED */

#if defined(MBEDTLS_ECP_SHORT_WEIERSTRASS_ENABLED)
static int inner_check_short_weierstrass_pubkey(const mbedtls_ecp_group *grp, const mbedtls_ecp_point *pt)
{
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    bool is_on_curve;
    uapi_drv_cipher_pke_ecc_curve_type_t curve_type;
    size_t ksize = 0;
    uapi_drv_cipher_pke_ecc_point_t *ecc_point = NULL;

    /* step 1. check  0 <= X,Y < P */
    if (mbedtls_mpi_cmp_int(&pt->X, 0) < 0 ||
        mbedtls_mpi_cmp_int(&pt->Y, 0) < 0 ||
        mbedtls_mpi_cmp_mpi(&pt->X, &grp->P) >= 0 ||
        mbedtls_mpi_cmp_mpi(&pt->Y, &grp->P) >= 0) {
        return MBEDTLS_ERR_ECP_INVALID_KEY;
    }

    mbedtls_alt_get_grp_type(grp->id, &curve_type);
    if (curve_type == UAPI_DRV_CIPHER_PKE_ECC_TYPE_INVALID) {
        ret = MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE;
        goto cleanup;
    }

    ksize = mbedtls_ecp_get_klen(curve_type);
    ecc_point = ecp_point_malloc(pt, ksize);
    if (ecc_point == NULL) {
        return MBEDTLS_ERR_ECP_ALLOC_FAILED;
    }
    /* step 2. check  dot on curve */
    ret = uapi_drv_cipher_pke_check_dot_on_curve(curve_type, ecc_point, &is_on_curve);
    if (ret != ERRCODE_SUCC || is_on_curve == 0) {
        ret = MBEDTLS_ERR_ECP_INVALID_KEY;
        goto cleanup;
    }
    ret = 0;
cleanup:
    ecp_point_free(ecc_point, ksize);
    ecc_point = NULL;
    return ret;
}

static int inner_check_short_weierstrass_privkey(const mbedtls_ecp_group *grp, const mbedtls_mpi *d)
{
    /*  1 <= d < N */
    if (mbedtls_mpi_cmp_int(d, 1) < 0 ||
        mbedtls_mpi_cmp_mpi(d, &grp->N) >= 0) {
        return MBEDTLS_ERR_ECP_INVALID_KEY;
    } else {
        return 0;
    }
}
#endif /* MBEDTLS_ECP_SHORT_WEIERSTRASS_ENABLED */

int mbedtls_ecp_check_pubkey(const mbedtls_ecp_group *grp, const mbedtls_ecp_point *pt)
{
    int ret;
    MBEDTLS_PERF_START_WITH_ID(MBEDTLS_PERF_TIMER_ID_0);
    if (grp == NULL || pt == NULL) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    if (mbedtls_mpi_cmp_int(&pt->Z, 1) != 0) {
        return MBEDTLS_ERR_ECP_INVALID_KEY;
    }

#if defined(MBEDTLS_ECP_MONTGOMERY_ENABLED)
    if (mbedtls_ecp_get_type(grp) == MBEDTLS_ECP_TYPE_MONTGOMERY) {
        ret = inner_check_montgomery_pubkey(grp, pt);
        goto exit;
    }
#endif

#if defined(MBEDTLS_ECP_SHORT_WEIERSTRASS_ENABLED)
    if (mbedtls_ecp_get_type(grp) == MBEDTLS_ECP_TYPE_SHORT_WEIERSTRASS) {
        ret = inner_check_short_weierstrass_pubkey(grp, pt);
        goto exit;
    }
#endif

    ret = MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
exit:
    MBEDTLS_PERF_ECP_END2_WITH_ID(MBEDTLS_PERF_TIMER_ID_0, grp->id, MBEDTLS_PERF_ECP_TYPE_CHECK_PUBKEY);
    return ret;
}

/*
 * Check that an mbedtls_mpi is valid as a private key
 */
int mbedtls_ecp_check_privkey(const mbedtls_ecp_group *grp,
                              const mbedtls_mpi *d)
{
    int ret;
    MBEDTLS_PERF_START_WITH_ID(MBEDTLS_PERF_TIMER_ID_0);
    if (grp == NULL || d == NULL) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

#if defined(MBEDTLS_ECP_MONTGOMERY_ENABLED)
    if (mbedtls_ecp_get_type(grp) == MBEDTLS_ECP_TYPE_MONTGOMERY) {
        ret = inner_check_montgomery_privkey(grp, d);
        goto exit;
    }
#endif /* MBEDTLS_ECP_MONTGOMERY_ENABLED */

#if defined(MBEDTLS_ECP_SHORT_WEIERSTRASS_ENABLED)
    if (mbedtls_ecp_get_type(grp) == MBEDTLS_ECP_TYPE_SHORT_WEIERSTRASS) {
        ret = inner_check_short_weierstrass_privkey(grp, d);
        goto exit;
    }
#endif /* MBEDTLS_ECP_SHORT_WEIERSTRASS_ENABLED */

    ret = MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
exit:
    MBEDTLS_PERF_ECP_END2_WITH_ID(MBEDTLS_PERF_TIMER_ID_0, grp->id, MBEDTLS_PERF_ECP_TYPE_CHECK_PRIVKEY);
    return ret;
}

/*--------------------------------- CHECK KEY -------------------------------------*/

/*--------------------------------- GEN KEY --------------------------------------*/
int mbedtls_ecp_gen_key(mbedtls_ecp_group_id grp_id, mbedtls_ecp_keypair *key,
                        mbedtls_f_rng_t *f_rng,
                        void *p_rng)
{
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;

    MBEDTLS_PERF_START_WITH_ID(MBEDTLS_PERF_TIMER_ID_3);
    if (key == NULL) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    if ((ret = mbedtls_ecp_group_load(&key->grp, grp_id)) != 0) {
        return ret;
    }

    ret = mbedtls_ecp_gen_keypair(&key->grp, &key->d, &key->Q, f_rng, p_rng);
    MBEDTLS_PERF_ECP_END2_WITH_ID(MBEDTLS_PERF_TIMER_ID_3, key->grp.id, MBEDTLS_PERF_ECP_TYPE_GEN_KEY);
    return ret;
}

#if defined(MBEDTLS_ECP_MONTGOMERY_ENABLED)
/*
* reference :RFC7748 https://rfc-editor.org/rfc/rfc7748.html
*/
static int inner_gen_montgomery_privkey(size_t high_bit,
                                        mbedtls_mpi *d,
                                        int (*f_rng)(void *, unsigned char *, size_t),
                                        void *p_rng)
{
    int ret = MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    size_t n = high_bit / ECP_BITS_OF_BITE + 1;

    MBEDTLS_MPI_CHK(mbedtls_mpi_fill_random(d, n, f_rng, p_rng));
    MBEDTLS_MPI_CHK(mbedtls_mpi_shift_r(d, ECP_BITS_OF_BITE * n - high_bit - 1));
    MBEDTLS_MPI_CHK(mbedtls_mpi_set_bit(d, high_bit, 1));
    MBEDTLS_MPI_CHK(mbedtls_mpi_set_bit(d, ECP_LOW_ORDER_0_BIT, 0));
    MBEDTLS_MPI_CHK(mbedtls_mpi_set_bit(d, ECP_LOW_ORDER_1_BIT, 0));
    if (high_bit == ECP_CURVE25519_NBIT_SIZE) {
        MBEDTLS_MPI_CHK(mbedtls_mpi_set_bit(d, ECP_LOW_ORDER_2_BIT, 0));
    }

cleanup:
    return ret;
}
#endif /* MBEDTLS_ECP_MONTGOMERY_ENABLED */

#if defined(MBEDTLS_ECP_SHORT_WEIERSTRASS_ENABLED)
static int inner_gen_short_weierstrass_privkey(const mbedtls_mpi *N,
                                               mbedtls_mpi *d,
                                               int (*f_rng)(void *, unsigned char *, size_t),
                                               void *p_rng)
{
    int ret;
    ret = mbedtls_mpi_random(d, 1, N, f_rng, p_rng);
    if (ret == MBEDTLS_ERR_MPI_NOT_ACCEPTABLE) {
        return MBEDTLS_ERR_ECP_RANDOM_FAILED;
    }
    return ret;
}
#endif

int mbedtls_ecp_gen_privkey(const mbedtls_ecp_group *grp,
                            mbedtls_mpi *d,
                            int (*f_rng)(void *, unsigned char *, size_t),
                            void *p_rng)
{
    int ret;
    MBEDTLS_PERF_START_WITH_ID(MBEDTLS_PERF_TIMER_ID_0);
    if (f_rng == NULL) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }
#if defined(MBEDTLS_ECP_MONTGOMERY_ENABLED)
    if (mbedtls_ecp_get_type(grp) == MBEDTLS_ECP_TYPE_MONTGOMERY) {
        ret = inner_gen_montgomery_privkey(grp->nbits, d, f_rng, p_rng);
        goto exit;
    }
#endif

#if defined(MBEDTLS_ECP_SHORT_WEIERSTRASS_ENABLED)
    if (mbedtls_ecp_get_type(grp) == MBEDTLS_ECP_TYPE_SHORT_WEIERSTRASS) {
        ret = inner_gen_short_weierstrass_privkey(&grp->N, d, f_rng, p_rng);
        goto exit;
    }
#endif

    ret = MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
exit:
    MBEDTLS_PERF_ECP_END2_WITH_ID(MBEDTLS_PERF_TIMER_ID_0, grp->id, MBEDTLS_PERF_ECP_TYPE_GEN_PRIVKEY);
    return ret;
}

int mbedtls_ecp_gen_keypair(mbedtls_ecp_group *grp, mbedtls_mpi *d, mbedtls_ecp_point *Q,
                            int (*f_rng)(void *, unsigned char *, size_t), void *p_rng)
{
    uapi_drv_cipher_pke_ecc_point_t* q_point = NULL;
    uapi_drv_cipher_pke_data_t* d_data = NULL;
    uapi_drv_cipher_pke_ecc_curve_type_t curve_type;

    size_t ksize = 0;
    errcode_t ret;

    MBEDTLS_PERF_START_WITH_ID(MBEDTLS_PERF_TIMER_ID_0);
    (void)f_rng;
    (void)p_rng;

    if (grp == NULL || d == NULL || Q == NULL) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    mbedtls_alt_get_grp_type(grp->id, &curve_type);
    if (curve_type == UAPI_DRV_CIPHER_PKE_ECC_TYPE_INVALID) {
        ret = MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE;
        goto cleanup;
    }

    ksize = mbedtls_ecp_get_klen(curve_type);

    q_point = ecp_point_malloc(NULL, ksize);
    if (q_point == NULL) {
        ret = MBEDTLS_ERR_ECP_ALLOC_FAILED;
        goto cleanup;
    }

    d_data = ecp_data_malloc(NULL, ksize);
    if (d_data == NULL) {
        ret = MBEDTLS_ERR_ECP_ALLOC_FAILED;
        goto cleanup;
    }

    ret = uapi_drv_cipher_pke_ecc_gen_key(curve_type, NULL, d_data, q_point);
    if (ret != ERRCODE_SUCC) {
        ret = MBEDTLS_ERR_ECP_INVALID_KEY;
        goto cleanup;
    }

    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&Q->X, q_point->x, ksize));
    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&Q->Y, q_point->y, ksize));
    MBEDTLS_MPI_CHK(mbedtls_mpi_lset(&Q->Z, 1));
    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(d, d_data->data, ksize));

    ret = 0;
cleanup:
    ecp_point_free(q_point, ksize);
    ecp_data_free(d_data, ksize);
    q_point = NULL;
    d_data = NULL;
    MBEDTLS_PERF_ECP_END2_WITH_ID(MBEDTLS_PERF_TIMER_ID_0, grp->id, MBEDTLS_PERF_ECP_TYPE_GEN_KEYPAIR);
    return ret;
}

int mbedtls_ecp_gen_keypair_base(mbedtls_ecp_group *grp, const mbedtls_ecp_point *G,
                                 mbedtls_mpi *d, mbedtls_ecp_point *Q,
                                 int (*f_rng)(void *, unsigned char *, size_t), void *p_rng)
{
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    MBEDTLS_PERF_START_WITH_ID(MBEDTLS_PERF_TIMER_ID_3);
    if (grp == NULL || G == NULL || d == NULL || Q == NULL) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }
    /* step 1. generate a d */
    MBEDTLS_MPI_CHK(mbedtls_ecp_gen_privkey(grp, d, f_rng, p_rng));
    /* step 2. Calculate a Q */
    MBEDTLS_MPI_CHK(mbedtls_ecp_mul(grp, Q, d, G, f_rng, p_rng));

cleanup:
    MBEDTLS_PERF_ECP_END2_WITH_ID(MBEDTLS_PERF_TIMER_ID_3, grp->id, MBEDTLS_PERF_ECP_TYPE_GEN_KEYPAIR_BASE);
    return ret;
}

int mbedtls_ecp_keypair_calc_public(mbedtls_ecp_keypair *key,
                                    int (*f_rng)(void *, unsigned char *, size_t),
                                    void *p_rng)
{
    int ret;
    MBEDTLS_PERF_START_WITH_ID(MBEDTLS_PERF_TIMER_ID_3);
    if (key == NULL) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }
    ret = mbedtls_ecp_mul(&key->grp, &key->Q, &key->d, &key->grp.G, f_rng, p_rng);
    MBEDTLS_PERF_ECP_END2_WITH_ID(MBEDTLS_PERF_TIMER_ID_3, key->grp.id, MBEDTLS_PERF_ECP_TYPE_KEYPAIR_CALC_PUBLIC);
    return ret;
}
/*--------------------------------- GEN KEY --------------------------------------*/

/*----------------------------------- READ / WRITE ------------------------------*/
int mbedtls_ecp_group_load(mbedtls_ecp_group *grp, mbedtls_ecp_group_id id)
{
    errcode_t ret;
    uapi_drv_cipher_pke_ecc_curve_type_t curve_type;
    const uapi_drv_cipher_pke_ecc_param* ecc_param = NULL;
    uapi_drv_cipher_pke_ecc_point_t G;
    uint32_t ksize;

    MBEDTLS_PERF_START_WITH_ID(MBEDTLS_PERF_TIMER_ID_2);
    if (grp == NULL) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    mbedtls_ecp_group_free(grp);
    mbedtls_ecp_group_init(grp);
    grp->id = id;
    mbedtls_alt_get_grp_type(grp->id, &curve_type);
    if (curve_type == UAPI_DRV_CIPHER_PKE_ECC_TYPE_INVALID) {
        return MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE;
    }

    ret = uapi_drv_cipher_pke_get_ecc_param(curve_type, &ecc_param);
    if (ret != ERRCODE_SUCC) {
        mbedtls_printf("group load get ecc param failed, ret=0x%x\n", ret);
        return MBEDTLS_ERR_ERROR_GENERIC_ERROR;
    }

    ksize = ecc_param->ksize;

    ret = mbedtls_mpi_read_binary(&grp->P, ecc_param->p, ksize);
    if (ret != 0) {
        mbedtls_printf("group load get P failed, ret=0x%x\n", ret);
        return MBEDTLS_ERR_ERROR_GENERIC_ERROR;
    }

    if (ecc_param->a != NULL) {
        ret = mbedtls_mpi_read_binary(&grp->A, ecc_param->a, ksize);
        if (ret != 0) {
            mbedtls_printf("group load get A failed, ret=0x%x\n", ret);
            return MBEDTLS_ERR_ERROR_GENERIC_ERROR;
        }
    }

    if (ecc_param->b != NULL) {
        ret = mbedtls_mpi_read_binary(&grp->B, ecc_param->b, ksize);
        if (ret != 0) {
            mbedtls_printf("group load get B failed, ret=0x%x\n", ret);
            return MBEDTLS_ERR_ERROR_GENERIC_ERROR;
        }
    }

    ret = mbedtls_mpi_read_binary(&grp->N, ecc_param->n, ksize);
    if (ret != 0) {
        mbedtls_printf("group load get N failed, ret=0x%x\n", ret);
        return MBEDTLS_ERR_ERROR_GENERIC_ERROR;
    }

    grp->pbits = mbedtls_mpi_bitlen(&grp->P);

    switch (grp->id) {
        case MBEDTLS_ECP_DP_CURVE448:
            MBEDTLS_MPI_CHK(mbedtls_mpi_lset(&grp->G.X, 5)); // 5: curve448's public key x
            MBEDTLS_MPI_CHK(mbedtls_mpi_lset(&grp->G.Z, 1));
            mbedtls_mpi_free(&grp->G.Y);
            grp->nbits = ECP_CURVE448_NBIT_SIZE;
            (void)G;
            break;

        case MBEDTLS_ECP_DP_CURVE25519:
            MBEDTLS_MPI_CHK(mbedtls_mpi_lset(&grp->G.X, 9)); // 9: curve25519's public key x
            MBEDTLS_MPI_CHK(mbedtls_mpi_lset(&grp->G.Z, 1));
            mbedtls_mpi_free(&grp->G.Y);
            grp->nbits = ECP_CURVE25519_NBIT_SIZE;
            (void)G;
            break;

        default:
            G.x = (uint8_t *)ecc_param->gx;
            G.y = (uint8_t *)ecc_param->gy;
            G.length = ksize;

            ret = inner_read_point(&G, &grp->G);
            if (ret != 0) {
                mbedtls_printf("group load get G failed, ret=0x%x\n", ret);
                return MBEDTLS_ERR_ERROR_GENERIC_ERROR;
            }
            grp->nbits = mbedtls_mpi_bitlen(&grp->N);
            break;
    }

    ret = 0;
cleanup:
    MBEDTLS_PERF_ECP_END2_WITH_ID(MBEDTLS_PERF_TIMER_ID_2, grp->id, MBEDTLS_PERF_ECP_TYPE_GROUP_LOAD);
    return ret;
}

#if defined(MBEDTLS_ECP_MONTGOMERY_ENABLED)
static int inner_read_montgomery_key(mbedtls_ecp_group_id grp_id, mbedtls_mpi *d,
                                     const unsigned char *buf, size_t buflen)
{
    int ret;

    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary_le(d, buf, buflen));
    /* Set the two least significant bits to 0 (Montgomery curve private key masking requirement) */
    MBEDTLS_MPI_CHK(mbedtls_mpi_set_bit(d, ECP_LOW_ORDER_0_BIT, 0));
    MBEDTLS_MPI_CHK(mbedtls_mpi_set_bit(d, ECP_LOW_ORDER_1_BIT, 0));
    if (grp_id == MBEDTLS_ECP_DP_CURVE25519) {
        if (buflen != ECP_CURVE25519_KEY_SIZE) {
            return MBEDTLS_ERR_ECP_INVALID_KEY;
        }
        /* Set 2nd bit to 0 */
        MBEDTLS_MPI_CHK(mbedtls_mpi_set_bit(d, ECP_LOW_ORDER_2_BIT, 0));
        /* Set MSB to 0 */
        MBEDTLS_MPI_CHK(mbedtls_mpi_set_bit(d, ECP_CURVE25519_HIGH_ORDER_0_BIT, 0));
        /* Set 2nd MSB to 1 */
        MBEDTLS_MPI_CHK(mbedtls_mpi_set_bit(d, ECP_CURVE25519_HIGH_ORDER_1_BIT, 1));
    } else if (grp_id == MBEDTLS_ECP_DP_CURVE448) {
        if (buflen != ECP_CURVE448_KEY_SIZE) {
            return MBEDTLS_ERR_ECP_INVALID_KEY;
        }
        /* Curve448 specific bit masking - set most significant bit to 1 */
        MBEDTLS_MPI_CHK(mbedtls_mpi_set_bit(d, ECP_CURVE448_HIGH_ORDER_0_BIT, 1));
    }
cleanup:
    return ret;
}
#endif

#if defined(MBEDTLS_ECP_SHORT_WEIERSTRASS_ENABLED)
static int inner_read_short_weierstrass_key(mbedtls_mpi *d, const unsigned char *buf, size_t buflen)
{
    return mbedtls_mpi_read_binary(d, buf, buflen);
}
#endif
int mbedtls_ecp_read_key(mbedtls_ecp_group_id grp_id, mbedtls_ecp_keypair *key,
                         const unsigned char *buf, size_t buflen)
{
    int ret = MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE;
    MBEDTLS_PERF_START_WITH_ID(MBEDTLS_PERF_TIMER_ID_3);
    if ((ret = mbedtls_ecp_group_load(&key->grp, grp_id)) != 0) {
        return ret;
    }

#if defined(MBEDTLS_ECP_MONTGOMERY_ENABLED)
    if (mbedtls_ecp_get_type(&key->grp) == MBEDTLS_ECP_TYPE_MONTGOMERY) {
        /*
         * Mask the key as mandated by RFC7748 for Curve25519 and Curve448.
         */
        MBEDTLS_MPI_CHK(inner_read_montgomery_key(grp_id, &key->d, buf, buflen));
    }
#endif
#if defined(MBEDTLS_ECP_SHORT_WEIERSTRASS_ENABLED)
    if (mbedtls_ecp_get_type(&key->grp) == MBEDTLS_ECP_TYPE_SHORT_WEIERSTRASS) {
        MBEDTLS_MPI_CHK(inner_read_short_weierstrass_key(&key->d, buf, buflen));
    }
#endif

    if (ret == 0) {
        MBEDTLS_MPI_CHK(mbedtls_ecp_check_privkey(&key->grp, &key->d));
    }

cleanup:
    if (ret != 0) {
        mbedtls_mpi_free(&key->d);
    }
    MBEDTLS_PERF_ECP_END2_WITH_ID(MBEDTLS_PERF_TIMER_ID_3, grp_id, MBEDTLS_PERF_ECP_TYPE_READ_KEY);
    return ret;
}

int mbedtls_ecp_set_public_key(mbedtls_ecp_group_id grp_id,
                               mbedtls_ecp_keypair *key,
                               const mbedtls_ecp_point *Q)
{
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    MBEDTLS_PERF_START_WITH_ID(MBEDTLS_PERF_TIMER_ID_3);
    if (key == NULL || Q == NULL) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    if (key->grp.id == MBEDTLS_ECP_DP_NONE) {
        /* Group not set yet */
        ret = mbedtls_ecp_group_load(&key->grp, grp_id);
        if (ret != 0) {
            return ret;
        }
    } else if (key->grp.id != grp_id) {
        /* Group mismatch */
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    ret = mbedtls_ecp_copy(&key->Q, Q);
    MBEDTLS_PERF_ECP_END2_WITH_ID(MBEDTLS_PERF_TIMER_ID_3, grp_id, MBEDTLS_PERF_ECP_TYPE_SET_PUBLIC_KEY);
    return ret;
}

#if !defined(MBEDTLS_DEPRECATED_REMOVED)
int mbedtls_ecp_write_key(mbedtls_ecp_keypair *key,
                          unsigned char *buf, size_t buflen)
{
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    MBEDTLS_PERF_START_WITH_ID(MBEDTLS_PERF_TIMER_ID_0);
    if (key == NULL) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    if (buf == NULL && buflen != 0) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }
#if defined(MBEDTLS_ECP_MONTGOMERY_ENABLED)
    if (mbedtls_ecp_get_type(&key->grp) == MBEDTLS_ECP_TYPE_MONTGOMERY) {
        if (key->grp.id == MBEDTLS_ECP_DP_CURVE25519) {
            if (buflen < ECP_CURVE25519_KEY_SIZE) {
                return MBEDTLS_ERR_ECP_BUFFER_TOO_SMALL;
            }
        } else if (key->grp.id == MBEDTLS_ECP_DP_CURVE448) {
            if (buflen < ECP_CURVE448_KEY_SIZE) {
                return MBEDTLS_ERR_ECP_BUFFER_TOO_SMALL;
            }
        }
        MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary_le(&key->d, buf, buflen));
    }
#endif
#if defined(MBEDTLS_ECP_SHORT_WEIERSTRASS_ENABLED)
    if (mbedtls_ecp_get_type(&key->grp) == MBEDTLS_ECP_TYPE_SHORT_WEIERSTRASS) {
        MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary(&key->d, buf, buflen));
    }
#endif
cleanup:
    MBEDTLS_PERF_ECP_END2_WITH_ID(MBEDTLS_PERF_TIMER_ID_0, key->grp.id, MBEDTLS_PERF_ECP_TYPE_WRITE_KEY);
    return ret;
}
#endif /* MBEDTLS_DEPRECATED_REMOVED */

mbedtls_ecp_group_id mbedtls_ecp_keypair_get_group_id(const mbedtls_ecp_keypair *key)
{
    if (key == NULL) {
        return MBEDTLS_ECP_DP_NONE;
    }
    return key->grp.id;
}

int mbedtls_ecp_write_public_key(const mbedtls_ecp_keypair *key,
                                 int format, size_t *olen,
                                 unsigned char *buf, size_t buflen)
{
    return mbedtls_ecp_point_write_binary(&key->grp, &key->Q,
                                          format, olen, buf, buflen);
}

int mbedtls_ecp_export(const mbedtls_ecp_keypair *key, mbedtls_ecp_group *grp,
                       mbedtls_mpi *d, mbedtls_ecp_point *Q)
{
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    MBEDTLS_PERF_START_WITH_ID(MBEDTLS_PERF_TIMER_ID_3);
    if (key == NULL) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    if (grp != NULL) {
        ret = mbedtls_ecp_group_copy(grp, &key->grp);
        if (ret != 0) {
            return ret;
        }
    }

    if (d != NULL) {
        ret = mbedtls_mpi_copy(d, &key->d);
        if (ret != 0) {
            return ret;
        }
    }

    if (Q != NULL) {
        ret = mbedtls_ecp_copy(Q, &key->Q);
        if (ret != 0) {
            return ret;
        }
    }

    if (grp != NULL) {
        MBEDTLS_PERF_ECP_END2_WITH_ID(MBEDTLS_PERF_TIMER_ID_3, grp->id, MBEDTLS_PERF_ECP_TYPE_EXPORT);
    }
    return 0;
}

#if defined(MBEDTLS_ECP_MONTGOMERY_ENABLED)
static int inner_read_montgomery_binary(const mbedtls_ecp_group *grp,
                                        mbedtls_ecp_point *pt,
                                        const unsigned char *buf, size_t ilen)
{
    size_t plen = mbedtls_mpi_size(&grp->P);
    int ret;
    if (plen != ilen) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }
    ret = mbedtls_mpi_read_binary_le(&pt->X, buf, plen);
    ECP_CHK_RETURN(ret);

    if (grp->id == MBEDTLS_ECP_DP_CURVE25519) {
        ret = mbedtls_mpi_set_bit(&pt->X, plen * ECP_BITS_OF_BITE - 1, 0);
        ECP_CHK_RETURN(ret);
    }
    ret = mbedtls_mpi_lset(&pt->Z, 1);
    ECP_CHK_RETURN(ret);
    return 0;
}
#endif

#if defined(MBEDTLS_ECP_SHORT_WEIERSTRASS_ENABLED)
static int inner_ecp_mod_sqrt_p3mod4(mbedtls_mpi *y,
                                     const mbedtls_mpi *a,
                                     const mbedtls_mpi *p)
{
    int ret = 0;
    mbedtls_mpi e, t;
    mbedtls_mpi_init(&e);
    mbedtls_mpi_init(&t);

    /* e = (p + 1) / 4 */
    MBEDTLS_MPI_CHK(mbedtls_mpi_add_int(&e, p, 1));
    MBEDTLS_MPI_CHK(mbedtls_mpi_shift_r(&e, 2)); // 2:  div 4
    /* y = a^e mod p */
    MBEDTLS_MPI_CHK(mbedtls_mpi_exp_mod(y, a, &e, p, NULL));
    /* y^2 % p == a */
    MBEDTLS_MPI_CHK(mbedtls_mpi_mul_mpi(&t, y, y));
    MBEDTLS_MPI_CHK(mbedtls_mpi_mod_mpi(&t, &t, p));
    MBEDTLS_MPI_CHK(mbedtls_mpi_cmp_mpi(&t, a));

cleanup:
    mbedtls_mpi_free(&e);
    mbedtls_mpi_free(&t);
    return ret;
}

static int inner_read_ecp_compressed_point(const mbedtls_ecp_group *grp,
                                           mbedtls_ecp_point *pt,
                                           unsigned char const *buf,
                                           size_t buflen,
                                           size_t plen)
{
    int ret = MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    mbedtls_mpi rhs, t;
    if (buflen != plen + 1) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    if (mbedtls_mpi_get_bit(&grp->P, ECP_LOW_ORDER_0_BIT) != 1 ||
        mbedtls_mpi_get_bit(&grp->P, ECP_LOW_ORDER_1_BIT) != 1) {
        return MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE;
    }
    mbedtls_mpi_init(&rhs);
    mbedtls_mpi_init(&t);
    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&pt->X, buf + 1, plen));
    /* rhs = x^3 + A*x + B mod P */
    MBEDTLS_MPI_CHK(mbedtls_mpi_mul_mpi(&rhs, &pt->X, &pt->X));  /* x^2 */
    MBEDTLS_MPI_CHK(mbedtls_mpi_mod_mpi(&rhs, &rhs, &grp->P));
    MBEDTLS_MPI_CHK(mbedtls_mpi_mul_mpi(&rhs, &rhs, &pt->X));    /* x^3 */
    MBEDTLS_MPI_CHK(mbedtls_mpi_mod_mpi(&rhs, &rhs, &grp->P));
    MBEDTLS_MPI_CHK(mbedtls_mpi_mul_mpi(&t, &grp->A, &pt->X));   /* A*x */
    MBEDTLS_MPI_CHK(mbedtls_mpi_mod_mpi(&t, &t, &grp->P));
    MBEDTLS_MPI_CHK(mbedtls_mpi_add_mpi(&rhs, &rhs, &t));
    MBEDTLS_MPI_CHK(mbedtls_mpi_add_mpi(&rhs, &rhs, &grp->B));
    MBEDTLS_MPI_CHK(mbedtls_mpi_mod_mpi(&rhs, &rhs, &grp->P));
    /* Y = sqrt(rhs) mod P */
    MBEDTLS_MPI_CHK(inner_ecp_mod_sqrt_p3mod4(&pt->Y, &rhs, &grp->P));

    if (mbedtls_mpi_get_bit(&pt->Y, ECP_LOW_ORDER_0_BIT) != (buf[0] & 1)) {
        MBEDTLS_MPI_CHK(mbedtls_mpi_sub_mpi(&pt->Y, &grp->P, &pt->Y));
    }
    MBEDTLS_MPI_CHK(mbedtls_mpi_lset(&pt->Z, 1));
cleanup:
    mbedtls_mpi_free(&rhs);
    mbedtls_mpi_free(&t);
    return ret;
}

static int inner_read_short_weierstrass_binary(const mbedtls_ecp_group *grp,
                                               mbedtls_ecp_point *pt,
                                               const unsigned char *buf, size_t ilen)
{
    size_t plen = mbedtls_mpi_size(&grp->P);
    int ret = MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    if (buf[0] == 0x00) {
        if (ilen == 1) {
            return mbedtls_ecp_set_zero(pt);
        } else {
            return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
        }
    } else if (buf[0] == 0x04) { // 0x04 : UNCOMPRESSED
        if (ilen != 1 + plen * 2) { // 2: 2 * plen + 1 == buffer size
            return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
        }
        MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&pt->X, buf + 1, plen));
        MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&pt->Y, buf + 1 + plen, plen));
        MBEDTLS_MPI_CHK(mbedtls_mpi_lset(&pt->Z, 1));
    } else if (buf[0] == 0x02 || buf[0] == 0x03) { // 0x02 : COMPRESSED, 0x03 : COMPRESSED
        return inner_read_ecp_compressed_point(grp, pt, buf, ilen, plen);
    }
cleanup:
    return ret;
}
#endif
int mbedtls_ecp_point_read_binary(const mbedtls_ecp_group *grp,
                                  mbedtls_ecp_point *pt,
                                  const unsigned char *buf, size_t ilen)
{
    int ret = MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE;
    MBEDTLS_PERF_START_WITH_ID(MBEDTLS_PERF_TIMER_ID_0);
    if (grp == NULL || pt == NULL || buf == NULL) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    if (ilen < 1) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

#if defined(MBEDTLS_ECP_MONTGOMERY_ENABLED)
    if (mbedtls_ecp_get_type(grp) == MBEDTLS_ECP_TYPE_MONTGOMERY) {
        ret = inner_read_montgomery_binary(grp, pt, buf, ilen);
        goto exit;
    }
#endif
#if defined(MBEDTLS_ECP_SHORT_WEIERSTRASS_ENABLED)
    if (mbedtls_ecp_get_type(grp) == MBEDTLS_ECP_TYPE_SHORT_WEIERSTRASS) {
        ret = inner_read_short_weierstrass_binary(grp, pt, buf, ilen);
        goto exit;
    }
#endif
exit:
    MBEDTLS_PERF_ECP_END2_WITH_ID(MBEDTLS_PERF_TIMER_ID_0, grp->id, MBEDTLS_PERF_ECP_TYPE_POINT_READ_BINARY);
    return ret;
}

mbedtls_ecp_curve_type mbedtls_ecp_get_type(const mbedtls_ecp_group *grp)
{
    if (grp == NULL) {
        return MBEDTLS_ECP_TYPE_NONE;
    }

    switch (grp->id) {
        case MBEDTLS_ECP_DP_SECP192R1:
        case MBEDTLS_ECP_DP_SECP224R1:
        case MBEDTLS_ECP_DP_SECP256R1:
        case MBEDTLS_ECP_DP_SECP384R1:
        case MBEDTLS_ECP_DP_SECP521R1:
        case MBEDTLS_ECP_DP_BP256R1:
        case MBEDTLS_ECP_DP_BP384R1:
        case MBEDTLS_ECP_DP_BP512R1:
        case MBEDTLS_ECP_DP_SECP192K1:
        case MBEDTLS_ECP_DP_SECP224K1:
        case MBEDTLS_ECP_DP_SECP256K1:
            return MBEDTLS_ECP_TYPE_SHORT_WEIERSTRASS;
        case MBEDTLS_ECP_DP_CURVE25519:
        case MBEDTLS_ECP_DP_CURVE448:
            return MBEDTLS_ECP_TYPE_MONTGOMERY;
        case MBEDTLS_ECP_DP_NONE:
        default:
            return MBEDTLS_ECP_TYPE_NONE;
    }
}

int mbedtls_ecp_tls_read_point(const mbedtls_ecp_group *grp,
                               mbedtls_ecp_point *pt,
                               const unsigned char **buf, size_t buf_len)
{
    size_t point_len;
    const unsigned char *p;
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    MBEDTLS_PERF_START_WITH_ID(MBEDTLS_PERF_TIMER_ID_1);

    if (grp == NULL || pt == NULL || buf == NULL || *buf == NULL) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    /*
        BUF_LEN = ECPointLen(1 byte) || ECPoint(ECPointLen bytes)
    */
    if (buf_len < 2) { // 2 : one for length, at least one for data
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    p = *buf;
    point_len = p[0];
    if (point_len < 1 || point_len > buf_len - 1) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    ret = mbedtls_ecp_point_read_binary(grp, pt, p + 1, point_len);
    if (ret != 0) {
        return ret;
    }

    *buf = p + 1 + point_len;
    MBEDTLS_PERF_ECP_END2_WITH_ID(MBEDTLS_PERF_TIMER_ID_1, grp->id, MBEDTLS_PERF_ECP_TYPE_TLS_READ_POINT);
    return 0;
}

#define TLS_ECP_POINT_MAX_SIZE 255
#define TLS_ECP_POINT_MIN_SIZE 1
int mbedtls_ecp_tls_write_point(const mbedtls_ecp_group *grp, const mbedtls_ecp_point *pt,
                                int format, size_t *olen,
                                unsigned char *buf, size_t blen)
{
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    size_t written_len;
    MBEDTLS_PERF_START_WITH_ID(MBEDTLS_PERF_TIMER_ID_1);
    if (format != MBEDTLS_ECP_PF_COMPRESSED &&
        format != MBEDTLS_ECP_PF_UNCOMPRESSED) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    if (grp == NULL || pt == NULL || olen == NULL || buf == NULL) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    /*
        BUF_LEN = ECPointLen(1 byte) || ECPoint(ECPointLen bytes)
    */
    if (blen < 2) { // 2 : 1 for length, at least one for data
        return MBEDTLS_ERR_ECP_BUFFER_TOO_SMALL;
    }

    ret = mbedtls_ecp_point_write_binary(grp, pt, format,
                                         &written_len,
                                         buf + 1,
                                         blen - 1);
    ECP_CHK_RETURN(ret);

    /* TLS ECP POINT  1 < len < 256 */
    if (written_len < TLS_ECP_POINT_MIN_SIZE ||
        written_len > TLS_ECP_POINT_MAX_SIZE) {
        return MBEDTLS_ERR_ERROR_GENERIC_ERROR;
    }

    buf[0] = (unsigned char)written_len;
    *olen = 1 + written_len;
    MBEDTLS_PERF_ECP_END2_WITH_ID(MBEDTLS_PERF_TIMER_ID_1, grp->id, MBEDTLS_PERF_ECP_TYPE_TLS_WRITE_POINT);
    return 0;
}

int mbedtls_ecp_tls_read_group(mbedtls_ecp_group *grp,
                               const unsigned char **buf, size_t len)
{
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    mbedtls_ecp_group_id grp_id;
    MBEDTLS_PERF_START_WITH_ID(MBEDTLS_PERF_TIMER_ID_3);

    ret = mbedtls_ecp_tls_read_group_id(&grp_id, buf, len);
    if (ret != 0) {
        return ret;
    }

    ret = mbedtls_ecp_group_load(grp, grp_id);
    if (ret != 0) {
        return ret;
    }
    MBEDTLS_PERF_ECP_END2_WITH_ID(MBEDTLS_PERF_TIMER_ID_3, grp->id, MBEDTLS_PERF_ECP_TYPE_TLS_READ_GROUP);
    return 0;
}

int mbedtls_ecp_tls_read_group_id(mbedtls_ecp_group_id *grp,
                                  const unsigned char **buf, size_t len)
{
    uint16_t tls_id;
    const mbedtls_ecp_curve_info *curve_info;
    const unsigned char *p;

    /*
        BUF = CURVE_TYPE(1 byte) | CURVE_ID(2 byte)
    */
    if (len < 3) { // 3 :at least 3 bytes
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    p = *buf;
    if (*p != MBEDTLS_ECP_TLS_NAMED_CURVE) { // First byte is curve_type; only named_curve is handled
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    p = p + 1;
    tls_id = MBEDTLS_GET_UINT16_BE(p, 0);

    curve_info = mbedtls_ecp_curve_info_from_tls_id(tls_id);
    if (curve_info == NULL) {
        return MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE;
    }

    p = p + 2; // 2 :two bytes are the namedcurve value
    *buf = p;
    *grp = curve_info->grp_id;

    return 0;
}

int mbedtls_ecp_tls_write_group(const mbedtls_ecp_group *grp, size_t *olen,
                                unsigned char *buf, size_t blen)
{
    const mbedtls_ecp_curve_info *curve_info;
    if (grp == NULL || olen == NULL) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    curve_info = mbedtls_ecp_curve_info_from_grp_id(grp->id);
    if (curve_info == NULL) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    /*
        BUF = CURVE_TYPE(1 byte) | CURVE_ID(2 byte)
    */
    *olen = 3; // 3: 1 byte to store curve type ,2 bypes are namedcurve value
    if (blen < 3) { // 3: *olen
        return MBEDTLS_ERR_ECP_BUFFER_TOO_SMALL;
    }

    *buf = MBEDTLS_ECP_TLS_NAMED_CURVE; // curve type
    buf = buf + 1;
    MBEDTLS_PUT_UINT16_BE(curve_info->tls_id, buf, 0);

    return 0;
}

int mbedtls_ecp_write_key_ext(const mbedtls_ecp_keypair *key,
                              size_t *olen, unsigned char *buf, size_t buflen)
{
    size_t len;
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;

    MBEDTLS_PERF_START_WITH_ID(MBEDTLS_PERF_TIMER_ID_0);
    if (key == NULL || olen == NULL) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    if (buf == NULL && buflen != 0) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }
    len = BITS_TO_BYTES(key->grp.nbits);
    if (len > buflen) {
        /* For robustness, ensure *olen <= buflen even on error. */
        *olen = 0;
        return MBEDTLS_ERR_ECP_BUFFER_TOO_SMALL;
    }
    *olen = len;
    /* Private key not set (or zero, which is invalid). */
    if (mbedtls_mpi_cmp_int(&key->d, 0) == 0) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }
#if defined(MBEDTLS_ECP_MONTGOMERY_ENABLED)
    if (mbedtls_ecp_get_type(&key->grp) == MBEDTLS_ECP_TYPE_MONTGOMERY) {
        ret = mbedtls_mpi_write_binary_le(&key->d, buf, len);
        ECP_CHK_RETURN(ret);
    }
#endif

#if defined(MBEDTLS_ECP_SHORT_WEIERSTRASS_ENABLED)
    if (mbedtls_ecp_get_type(&key->grp) == MBEDTLS_ECP_TYPE_SHORT_WEIERSTRASS) {
        ret = mbedtls_mpi_write_binary(&key->d, buf, len);
        ECP_CHK_RETURN(ret);
    }
#endif

    MBEDTLS_PERF_ECP_END2_WITH_ID(MBEDTLS_PERF_TIMER_ID_0, key->grp.id, MBEDTLS_PERF_ECP_TYPE_WRITE_KEY_EXT);
    return ret;
}

const mbedtls_ecp_group_id *mbedtls_ecp_grp_id_list(void)
{
    return ecp_supported_grp_id;
}

const mbedtls_ecp_curve_info *mbedtls_ecp_curve_list(void)
{
    return ecp_supported_curves;
}

const mbedtls_ecp_curve_info *mbedtls_ecp_curve_info_from_tls_id(uint16_t tls_id)
{
    size_t i;
    MBEDTLS_PERF_START_WITH_ID(MBEDTLS_PERF_TIMER_ID_0);
    for (i = 0; i < ECP_CURVES_NUMBER - 1; i++) {
        if (ecp_supported_curves[i].tls_id == tls_id) {
            MBEDTLS_PERF_ECP_END1_WITH_ID(MBEDTLS_PERF_TIMER_ID_0, MBEDTLS_PERF_ECP_TYPE_CURVE_INFO_FROM_TLS_ID);
            return &ecp_supported_curves[i];
        }
    }
    return NULL;
}

const mbedtls_ecp_curve_info *mbedtls_ecp_curve_info_from_grp_id(mbedtls_ecp_group_id grp_id)
{
    size_t i;
    MBEDTLS_PERF_START_WITH_ID(MBEDTLS_PERF_TIMER_ID_0);
    for (i = 0; i < ECP_CURVES_NUMBER - 1; i++) {
        if (ecp_supported_curves[i].grp_id == grp_id) {
            MBEDTLS_PERF_ECP_END1_WITH_ID(MBEDTLS_PERF_TIMER_ID_0, MBEDTLS_PERF_ECP_TYPE_CURVE_INFO_FROM_GRP_ID);
            return &ecp_supported_curves[i];
        }
    }
    return NULL;
}

const mbedtls_ecp_curve_info *mbedtls_ecp_curve_info_from_name(const char *name)
{
    size_t i;
    MBEDTLS_PERF_START_WITH_ID(MBEDTLS_PERF_TIMER_ID_0);
    if (name == NULL) {
        return NULL;
    }

    for (i = 0; i < ECP_CURVES_NUMBER - 1; i++) {
        if (strcmp(ecp_supported_curves[i].name, name) == 0) {
            MBEDTLS_PERF_ECP_END1_WITH_ID(MBEDTLS_PERF_TIMER_ID_0, MBEDTLS_PERF_ECP_TYPE_CURVE_INFO_FROM_NAME);
            return &ecp_supported_curves[i];
        }
    }
    return NULL;
}

#define ECP_BINARY      2
#define ECP_HEXADECIMAL 16
int mbedtls_ecp_point_read_string(mbedtls_ecp_point *P, int radix,
                                  const char *x, const char *y)
{
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    MBEDTLS_PERF_START_WITH_ID(MBEDTLS_PERF_TIMER_ID_0);
    if (P == NULL || x == NULL || y == NULL) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    if (radix < ECP_BINARY || radix > ECP_HEXADECIMAL) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }
    MBEDTLS_MPI_CHK(mbedtls_mpi_read_string(&P->X, radix, x));
    MBEDTLS_MPI_CHK(mbedtls_mpi_read_string(&P->Y, radix, y));
    MBEDTLS_MPI_CHK(mbedtls_mpi_lset(&P->Z, 1));

cleanup:
    MBEDTLS_PERF_ECP_END1_WITH_ID(MBEDTLS_PERF_TIMER_ID_0, MBEDTLS_PERF_ECP_TYPE_POINT_READ_STRING);
    return ret;
}

#if defined(MBEDTLS_ECP_MONTGOMERY_ENABLED)
static int inner_montgomery_write_binary(size_t plen, const mbedtls_ecp_point *P,
                                         size_t *olen, unsigned char *buf, size_t buflen)
{
    int ret;
    *olen = plen;
    if (buflen < plen) {
        return MBEDTLS_ERR_ECP_BUFFER_TOO_SMALL;
    }
    ret = mbedtls_mpi_write_binary_le(&P->X, buf, plen);
    ECP_CHK_RETURN(ret);
    return 0;
}
#endif

#if defined(MBEDTLS_ECP_SHORT_WEIERSTRASS_ENABLED)
static int inner_short_weierstrass_write_binary(size_t plen, const mbedtls_ecp_point *P,
                                                int format, size_t *olen,
                                                unsigned char *buf, size_t buflen)
{
    int ret;

    if (mbedtls_mpi_cmp_int(&P->Z, 0) == 0) {
        *olen = 1;
        if (buflen < *olen) {
            return MBEDTLS_ERR_ECP_BUFFER_TOO_SMALL;
        }
        buf[0] = 0x00;
        return 0;
    }
    switch (format) {
        case MBEDTLS_ECP_PF_UNCOMPRESSED: {
            *olen = 2 * plen + 1; // 2: Calculate output buffer length: double input length for hex encoding plus null terminator
            if (buflen < *olen) {
                return MBEDTLS_ERR_ECP_BUFFER_TOO_SMALL;
            }
            buf[0] = 0x04;
            MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary(&P->X, buf + 1, plen));
            MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary(&P->Y, buf + 1 + plen, plen));
            break;
        }
        case MBEDTLS_ECP_PF_COMPRESSED: {
            *olen = plen + 1;
            if (buflen < *olen) {
                return MBEDTLS_ERR_ECP_BUFFER_TOO_SMALL;
            }
            buf[0] = 0x02 + mbedtls_mpi_get_bit(&P->Y, ECP_LOW_ORDER_0_BIT);
            MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary(&P->X, buf + 1, plen));
            break;
        }
    }
cleanup:
    return ret;
}
#endif

int mbedtls_ecp_point_write_binary(const mbedtls_ecp_group *grp,
                                   const mbedtls_ecp_point *P,
                                   int format, size_t *olen,
                                   unsigned char *buf, size_t buflen)
{
    int ret;
    size_t plen;

    MBEDTLS_PERF_START_WITH_ID(MBEDTLS_PERF_TIMER_ID_0);
    if (grp == NULL || P == NULL || olen == NULL || buf == NULL) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    if (format != MBEDTLS_ECP_PF_COMPRESSED &&
        format != MBEDTLS_ECP_PF_UNCOMPRESSED) {
        *olen = 0;
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    plen = mbedtls_mpi_size(&grp->P);

#if defined(MBEDTLS_ECP_MONTGOMERY_ENABLED)
    (void)format;
    if (mbedtls_ecp_get_type(grp) == MBEDTLS_ECP_TYPE_MONTGOMERY) {
        ret = inner_montgomery_write_binary(plen, P, olen, buf, buflen);
        goto exit;
    }
#endif
#if defined(MBEDTLS_ECP_SHORT_WEIERSTRASS_ENABLED)
    if (mbedtls_ecp_get_type(grp) == MBEDTLS_ECP_TYPE_SHORT_WEIERSTRASS) {
        ret = inner_short_weierstrass_write_binary(plen, P, format, olen, buf, buflen);
        goto exit;
    }
#endif
    ret = MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE;
exit:
    MBEDTLS_PERF_ECP_END2_WITH_ID(MBEDTLS_PERF_TIMER_ID_0, grp->id, MBEDTLS_PERF_ECP_TYPE_POINT_WRITE_BINARY);
    return ret;
}

int mbedtls_ecp_set_zero(mbedtls_ecp_point *pt)
{
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    MBEDTLS_PERF_START_WITH_ID(MBEDTLS_PERF_TIMER_ID_0);
    if (pt == NULL) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }
    MBEDTLS_MPI_CHK(mbedtls_mpi_lset(&pt->X, 1));
    MBEDTLS_MPI_CHK(mbedtls_mpi_lset(&pt->Y, 1));
    MBEDTLS_MPI_CHK(mbedtls_mpi_lset(&pt->Z, 0));

cleanup:
    MBEDTLS_PERF_ECP_END1_WITH_ID(MBEDTLS_PERF_TIMER_ID_0, MBEDTLS_PERF_ECP_TYPE_SET_ZERO);
    return ret;
}
/*----------------------------------- READ / WRITE ------------------------------*/

/*----------------------------------- INIT / FREE ------------------------------*/
void mbedtls_ecp_point_init(mbedtls_ecp_point *pt)
{
    MBEDTLS_PERF_START_WITH_ID(MBEDTLS_PERF_TIMER_ID_0);
    if (pt == NULL) {
        return;
    }
    mbedtls_mpi_init(&(pt->X));
    mbedtls_mpi_init(&(pt->Y));
    mbedtls_mpi_init(&(pt->Z));
    MBEDTLS_PERF_ECP_END1_WITH_ID(MBEDTLS_PERF_TIMER_ID_0, MBEDTLS_PERF_ECP_TYPE_POINT_INIT);
    return;
}

void mbedtls_ecp_point_free(mbedtls_ecp_point *pt)
{
    MBEDTLS_PERF_START_WITH_ID(MBEDTLS_PERF_TIMER_ID_0);
    if (pt == NULL) {
        return;
    }

    mbedtls_mpi_free(&(pt->X));
    mbedtls_mpi_free(&(pt->Y));
    mbedtls_mpi_free(&(pt->Z));
    MBEDTLS_PERF_ECP_END1_WITH_ID(MBEDTLS_PERF_TIMER_ID_0, MBEDTLS_PERF_ECP_TYPE_POINT_FREE);
    return;
}

void mbedtls_ecp_group_init(mbedtls_ecp_group *grp)
{
    MBEDTLS_PERF_START_WITH_ID(MBEDTLS_PERF_TIMER_ID_1);
    if (grp == NULL) {
        return;
    }
    grp->id = MBEDTLS_ECP_DP_NONE;
    mbedtls_mpi_init(&grp->P);
    mbedtls_mpi_init(&grp->A);
    mbedtls_mpi_init(&grp->B);
    mbedtls_ecp_point_init(&grp->G);
    mbedtls_mpi_init(&grp->N);
    grp->pbits = 0;
    grp->nbits = 0;
    grp->h = 0;
    MBEDTLS_PERF_ECP_END1_WITH_ID(MBEDTLS_PERF_TIMER_ID_1, MBEDTLS_PERF_ECP_TYPE_GROUP_INIT);
    return;
}

void mbedtls_ecp_group_free(mbedtls_ecp_group *grp)
{
    MBEDTLS_PERF_START_WITH_ID(MBEDTLS_PERF_TIMER_ID_1);
    if (grp == NULL) {
        return;
    }

    mbedtls_mpi_free(&grp->A);
    mbedtls_mpi_free(&grp->B);
    mbedtls_ecp_point_free(&grp->G);
    mbedtls_mpi_free(&grp->N);
    mbedtls_mpi_free(&grp->P);

    mbedtls_platform_zeroize(grp, sizeof(mbedtls_ecp_group));
    MBEDTLS_PERF_ECP_END1_WITH_ID(MBEDTLS_PERF_TIMER_ID_1, MBEDTLS_PERF_ECP_TYPE_GROUP_INIT);
    return;
}

void mbedtls_ecp_keypair_free(mbedtls_ecp_keypair *key)
{
    MBEDTLS_PERF_START_WITH_ID(MBEDTLS_PERF_TIMER_ID_2);
    if (key == NULL) {
        return;
    }

    mbedtls_ecp_group_free(&key->grp);
    mbedtls_mpi_free(&key->d);
    mbedtls_ecp_point_free(&key->Q);
    MBEDTLS_PERF_ECP_END1_WITH_ID(MBEDTLS_PERF_TIMER_ID_2, MBEDTLS_PERF_ECP_TYPE_KEYPAIR_FREE);
    return;
}

void mbedtls_ecp_keypair_init(mbedtls_ecp_keypair *key)
{
    MBEDTLS_PERF_START_WITH_ID(MBEDTLS_PERF_TIMER_ID_2);
    if (key == NULL) {
        return;
    }

    mbedtls_ecp_group_init(&key->grp);
    mbedtls_mpi_init(&key->d);
    mbedtls_ecp_point_init(&key->Q);
    MBEDTLS_PERF_ECP_END1_WITH_ID(MBEDTLS_PERF_TIMER_ID_2, MBEDTLS_PERF_ECP_TYPE_KEYPAIR_INIT);
    return;
}

/*----------------------------------- INIT / FREE ------------------------------*/

/*---------------------------------SELF_TEST-----------------------------------*/
#if defined(MBEDTLS_SELF_TEST)
int mbedtls_ecp_self_test(int verbose)
{
    (void)verbose;
    return 0;
}
#endif
/*---------------------------------SELF_TEST-----------------------------------*/

#endif // MBEDTLS_ECP_C MBEDTLS_ECP_ALT