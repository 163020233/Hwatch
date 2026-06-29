/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 *
 * Description: Provides perf utils for mbedtls.
 *
 */
#ifndef MBEDTLS_PERF_UTILS_H
#define MBEDTLS_PERF_UTILS_H

#include <stdint.h>
#include <stddef.h>

#include "mbedtls_perf.h"
#include "mbedtls/ecp.h"
#include "bignum_alt.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    MBEDTLS_PERF_TIMER_ID_0,
    MBEDTLS_PERF_TIMER_ID_1,
    MBEDTLS_PERF_TIMER_ID_2,
    MBEDTLS_PERF_TIMER_ID_3,
    MBEDTLS_PERF_TIMER_ID_4,
    MBEDTLS_PERF_TIMER_ID_5,
    MBEDTLS_PERF_TIMER_ID_CNT,
} mbedtls_perf_timer_id_e;

void mbedtls_perf_start(void);

void mbedtls_perf_start_with_id(mbedtls_perf_timer_id_e timer_id);
/* aes perf utils. */
#define AES_DATA_LEN_SMALL      128
#define AES_DATA_LEN_MIDDLE     1024

typedef enum {
    MBEDTLS_PERF_AES_TYPE_INIT,
    MBEDTLS_PERF_AES_TYPE_FREE,
    MBEDTLS_PERF_AES_TYPE_SETKEY_ENC,
    MBEDTLS_PERF_AES_TYPE_SETKEY_DEC,
    MBEDTLS_PERF_AES_TYPE_CRYPT_SMALL_CHUNK,     /* data_len is [0, 128B] */
    MBEDTLS_PERF_AES_TYPE_CRYPT_MIDDLE_CHUNK,    /* data_len is (128B, 1KB] */
    MBEDTLS_PERF_AES_TYPE_CRYPT_LARGE_CHUNK,     /* data_len is (1KB, ...] */
    MBEDTLS_PERF_AES_TYPE_CNT,
} mbedtls_perf_aes_type_e;

/* 统计单接口调用的耗时，与数据量无关. */
void mbedtls_perf_aes_end1(mbedtls_perf_aes_type_e aes_type);

/* 统计单接口调用的耗时，与数据量有关. */
void mbedtls_perf_aes_end2(mbedtls_perf_aes_type_e aes_type, size_t data_len);

/* bignum perf utils. */
typedef enum {
    MBEDTLS_PERF_BIGNUM_TYPE_EXP_MOD_192,
    MBEDTLS_PERF_BIGNUM_TYPE_EXP_MOD_256,
    MBEDTLS_PERF_BIGNUM_TYPE_EXP_MOD_320,
    MBEDTLS_PERF_BIGNUM_TYPE_EXP_MOD_384,
    MBEDTLS_PERF_BIGNUM_TYPE_EXP_MOD_448,
    MBEDTLS_PERF_BIGNUM_TYPE_EXP_MOD_512,
    MBEDTLS_PERF_BIGNUM_TYPE_EXP_MOD_1024,
    MBEDTLS_PERF_BIGNUM_TYPE_EXP_MOD_2048,
    MBEDTLS_PERF_BIGNUM_TYPE_EXP_MOD_3072,
    MBEDTLS_PERF_BIGNUM_TYPE_EXP_MOD_4096,

    MBEDTLS_PERF_BIGNUM_TYPE_CNT,
} mbedtls_perf_bignum_type_e;

void mbedtls_perf_bignum_end(mbedtls_alt_ecp_len_e ecp_len);

/* ccm perf utils. */
typedef enum {
    MBEDTLS_PERF_CCM_TYPE_INIT,
    MBEDTLS_PERF_CCM_TYPE_FREE,
    MBEDTLS_PERF_CCM_TYPE_SETKEY,
    MBEDTLS_PERF_CCM_TYPE_STARTS,
    MBEDTLS_PERF_CCM_TYPE_SET_LENGTHS,
    MBEDTLS_PERF_CCM_TYPE_UPDATE_AD,
    MBEDTLS_PERF_CCM_TYPE_UPDATE,
    MBEDTLS_PERF_CCM_TYPE_FINISH,
    MBEDTLS_PERF_CCM_TYPE_CNT,
} mbedtls_perf_ccm_type_e;

/* 统计单接口调用的耗时，与数据量无关. */
void mbedtls_perf_ccm_end1(mbedtls_perf_ccm_type_e ccm_type);

/* 统计单接口调用的耗时，与数据量有关. */
void mbedtls_perf_ccm_end2(mbedtls_perf_ccm_type_e ccm_type, size_t data_len);

/* cmac perf utils. */
#define CMAC_DATA_LEN_SMALL      128
#define CMAC_DATA_LEN_MIDDLE     1024

typedef enum {
    MBEDTLS_PERF_CMAC_TYPE_STARTS,
    MBEDTLS_PERF_CMAC_TYPE_FINISH,
    MBEDTLS_PERF_CMAC_TYPE_RESET,

    MBEDTLS_PERF_CMAC_TYPE_UPDATE_SMALL_CHUNK,      /* [0, 128B] */
    MBEDTLS_PERF_CMAC_TYPE_UPDATE_MIDDLE_CHUNK,     /* (128B, 1KB] */
    MBEDTLS_PERF_CMAC_TYPE_UPDATE_LARGE_CHUNK,      /* (1KB, ...) */

    MBEDTLS_PERF_CMAC_TYPE_CNT,
} mbedtls_perf_cmac_type_e;

/* 统计单接口调用的耗时，与数据量无关. */
void mbedtls_perf_cmac_end1(mbedtls_perf_cmac_type_e cmac_type);

/* 统计单接口调用的耗时，与数据量有关. */
void mbedtls_perf_cmac_end2(mbedtls_perf_cmac_type_e cmac_type, size_t data_len);
/* ecdsa perf utils. */
typedef enum {
    MBEDTLS_PERF_ECDSA_OP_SIGN,
    MBEDTLS_PERF_ECDSA_OP_VERIFY,
    MBEDTLS_PERF_ECDSA_OP_GENKEY,
    MBEDTLS_PERF_ECDSA_OP_ECDH,
    MBEDTLS_PERF_ECDSA_OP_CNT,
} mbedtls_perf_ecdsa_op_e;

void mbedtls_perf_ecdsa_end(mbedtls_ecp_group_id grp_id, mbedtls_perf_ecdsa_op_e ecdsa_op);

/* ecp perf utils. */
typedef enum {
    /* TIMER_ID_0 */
    MBEDTLS_PERF_ECP_TYPE_COPY,
    MBEDTLS_PERF_ECP_TYPE_POINT_INIT,
    MBEDTLS_PERF_ECP_TYPE_POINT_FREE,
    MBEDTLS_PERF_ECP_TYPE_POINT_CMP,
    MBEDTLS_PERF_ECP_TYPE_IS_ZERO,
    MBEDTLS_PERF_ECP_TYPE_SET_ZERO,
    MBEDTLS_PERF_ECP_TYPE_POINT_READ_STRING,
    MBEDTLS_PERF_ECP_TYPE_CURVE_INFO_FROM_NAME,
    MBEDTLS_PERF_ECP_TYPE_CURVE_INFO_FROM_GRP_ID,
    MBEDTLS_PERF_ECP_TYPE_CURVE_INFO_FROM_TLS_ID,
    /* TIMER_ID_1 */
    MBEDTLS_PERF_ECP_TYPE_GROUP_INIT,
    MBEDTLS_PERF_ECP_TYPE_GROUP_FREE,
    /* TIMER_ID_2 */
    MBEDTLS_PERF_ECP_TYPE_KEYPAIR_INIT,
    MBEDTLS_PERF_ECP_TYPE_KEYPAIR_FREE,
    MBEDTLS_PERF_ECP_TYPE_GROUP_COPY,

    MBEDTLS_PERF_ECP_TYPE1_CNT,
} mbedtls_perf_ecp_type1_e;

typedef enum {
    /* TIMER_ID_0 */
    MBEDTLS_PERF_ECP_TYPE_WRITE_KEY_EXT,
    MBEDTLS_PERF_ECP_TYPE_CHECK_PUBKEY,
    MBEDTLS_PERF_ECP_TYPE_CHECK_PRIVKEY,
    MBEDTLS_PERF_ECP_TYPE_GEN_KEYPAIR,
    MBEDTLS_PERF_ECP_TYPE_GEN_PRIVKEY,
    MBEDTLS_PERF_ECP_TYPE_WRITE_KEY,
    MBEDTLS_PERF_ECP_TYPE_POINT_WRITE_BINARY,
    MBEDTLS_PERF_ECP_TYPE_POINT_READ_BINARY,

    /* TIMER_ID_2 */
    MBEDTLS_PERF_ECP_TYPE_GROUP_LOAD,
    MBEDTLS_PERF_ECP_TYPE_MUL,
    MBEDTLS_PERF_ECP_TYPE_MULADD,
    MBEDTLS_PERF_ECP_TYPE_TLS_READ_POINT,
    MBEDTLS_PERF_ECP_TYPE_TLS_WRITE_POINT,

    /* TIMER_ID_3 */
    MBEDTLS_PERF_ECP_TYPE_CHECK_PUB_PRIV,
    MBEDTLS_PERF_ECP_TYPE_GEN_KEY,
    MBEDTLS_PERF_ECP_TYPE_GEN_KEYPAIR_BASE,
    MBEDTLS_PERF_ECP_TYPE_KEYPAIR_CALC_PUBLIC,
    MBEDTLS_PERF_ECP_TYPE_READ_KEY,
    MBEDTLS_PERF_ECP_TYPE_SET_PUBLIC_KEY,
    MBEDTLS_PERF_ECP_TYPE_EXPORT,
    MBEDTLS_PERF_ECP_TYPE_TLS_READ_GROUP,

    MBEDTLS_PERF_ECP_TYPE2_CNT,
} mbedtls_perf_ecp_type2_e;

/* 统计单接口调用的耗时，与曲线类型无关. */
void mbedtls_perf_ecp_end1_with_id(mbedtls_perf_timer_id_e timer_id,
    mbedtls_perf_ecp_type1_e ecp_type);

/* 统计单接口调用的耗时，与曲线类型有关. */
void mbedtls_perf_ecp_end2_with_id(mbedtls_perf_timer_id_e timer_id,
    mbedtls_ecp_group_id grp_id,
    mbedtls_perf_ecp_type2_e ecp_type);

/* gcm perf utils. */
typedef enum {
    MBEDTLS_PERF_GCM_TYPE_INIT,
    MBEDTLS_PERF_GCM_TYPE_FREE,
    MBEDTLS_PERF_GCM_TYPE_SETKEY,
    MBEDTLS_PERF_GCM_TYPE_STARTS,
    MBEDTLS_PERF_GCM_TYPE_UPDATE_AD,
    MBEDTLS_PERF_GCM_TYPE_UPDATE,
    MBEDTLS_PERF_GCM_TYPE_FINISH,
    MBEDTLS_PERF_GCM_TYPE_CNT,
} mbedtls_perf_gcm_type_e;

/* 统计单接口调用的耗时，与数据量无关. */
void mbedtls_perf_gcm_end1(mbedtls_perf_gcm_type_e gcm_type);

/* 统计单接口调用的耗时，与数据量有关. */
void mbedtls_perf_gcm_end2(mbedtls_perf_gcm_type_e gcm_type, size_t data_len);

/* sha perf utils. */
#define SHA_DATA_LEN_SMALL      128
#define SHA_DATA_LEN_MIDDLE     1024

typedef enum {
    MBEDTLS_PERF_SHA_TYPE_INIT,
    MBEDTLS_PERF_SHA_TYPE_FREE,
    MBEDTLS_PERF_SHA_TYPE_CLONE,
    MBEDTLS_PERF_SHA_TYPE_START,
    MBEDTLS_PERF_SHA_TYPE_FINISH,
    MBEDTLS_PERF_SHA_TYPE_UPDATE_SMALL_CHUNK,
    MBEDTLS_PERF_SHA_TYPE_UPDATE_MIDDLE_CHUNK,
    MBEDTLS_PERF_SHA_TYPE_UPDATE_LARGE_CHUNK,

    MBEDTLS_PERF_SHA_TYPE_CNT,
} mbedtls_perf_sha_type_e;

/* 统计单接口调用的耗时，与数据量无关. */
void mbedtls_perf_sha_end1(mbedtls_perf_sha_type_e sha_type);

/* 统计单接口调用的耗时，与数据量有关. */
void mbedtls_perf_sha_end2(mbedtls_perf_sha_type_e sha_type, size_t data_len);

#if defined(MBEDTLS_PERF_UTILS_OPEN)
#define MBEDTLS_PERF_START                  mbedtls_perf_start
#define MBEDTLS_PERF_START_WITH_ID          mbedtls_perf_start_with_id

#define MBEDTLS_PERF_AES_END1           mbedtls_perf_aes_end1
#define MBEDTLS_PERF_AES_END2(data_len) do {    \
    if ((data_len) <= AES_DATA_LEN_SMALL) {     \
        mbedtls_perf_aes_end2(MBEDTLS_PERF_AES_TYPE_CRYPT_SMALL_CHUNK, (data_len)); \
    } else if ((data_len) <= AES_DATA_LEN_MIDDLE) { \
        mbedtls_perf_aes_end2(MBEDTLS_PERF_AES_TYPE_CRYPT_MIDDLE_CHUNK, (data_len));    \
    } else {    \
        mbedtls_perf_aes_end2(MBEDTLS_PERF_AES_TYPE_CRYPT_LARGE_CHUNK, (data_len)); \
    }   \
} while (0)

#define MBEDTLS_PERF_BIGNUM_END(size)       \
    mbedtls_perf_bignum_end((size))

#define MBEDTLS_PERF_CCM_END1           mbedtls_perf_ccm_end1
#define MBEDTLS_PERF_CCM_END2           mbedtls_perf_ccm_end2

#define MBEDTLS_PERF_CMAC_END1               mbedtls_perf_cmac_end1
#define MBEDTLS_PERF_CMAC_END2(data_len) do {    \
    if ((data_len) <= CMAC_DATA_LEN_SMALL) {     \
        mbedtls_perf_cmac_end2(MBEDTLS_PERF_CMAC_TYPE_UPDATE_SMALL_CHUNK, (data_len)); \
    } else if ((data_len) <= CMAC_DATA_LEN_MIDDLE) { \
        mbedtls_perf_cmac_end2(MBEDTLS_PERF_CMAC_TYPE_UPDATE_MIDDLE_CHUNK, (data_len));    \
    } else {    \
        mbedtls_perf_cmac_end2(MBEDTLS_PERF_CMAC_TYPE_UPDATE_LARGE_CHUNK, (data_len)); \
    }   \
} while (0)

#define MBEDTLS_PERF_ECDSA_END              mbedtls_perf_ecdsa_end

#define MBEDTLS_PERF_ECP_END1_WITH_ID       mbedtls_perf_ecp_end1_with_id
#define MBEDTLS_PERF_ECP_END2_WITH_ID       mbedtls_perf_ecp_end2_with_id

#define MBEDTLS_PERF_GCM_END1               mbedtls_perf_gcm_end1
#define MBEDTLS_PERF_GCM_END2               mbedtls_perf_gcm_end2

#define MBEDTLS_PERF_SHA_END1               mbedtls_perf_sha_end1
#define MBEDTLS_PERF_SHA_END2(data_len) do {    \
    if ((data_len) <= SHA_DATA_LEN_SMALL) {     \
        mbedtls_perf_sha_end2(MBEDTLS_PERF_SHA_TYPE_UPDATE_SMALL_CHUNK, (data_len)); \
    } else if ((data_len) <= SHA_DATA_LEN_MIDDLE) { \
        mbedtls_perf_sha_end2(MBEDTLS_PERF_SHA_TYPE_UPDATE_MIDDLE_CHUNK, (data_len));    \
    } else {    \
        mbedtls_perf_sha_end2(MBEDTLS_PERF_SHA_TYPE_UPDATE_LARGE_CHUNK, (data_len)); \
    }   \
} while (0)
#else
#define MBEDTLS_PERF_START(...)
#define MBEDTLS_PERF_START_WITH_ID(...)

#define MBEDTLS_PERF_AES_END1(...)
#define MBEDTLS_PERF_AES_END2(...)

#define MBEDTLS_PERF_BIGNUM_END(...)

#define MBEDTLS_PERF_CCM_END1(...)
#define MBEDTLS_PERF_CCM_END2(...)

#define MBEDTLS_PERF_CMAC_END1(...)
#define MBEDTLS_PERF_CMAC_END2(...)

#define MBEDTLS_PERF_ECDSA_END(...)

#define MBEDTLS_PERF_GCM_END1(...)
#define MBEDTLS_PERF_GCM_END2(...)

#define MBEDTLS_PERF_SHA_END1(...)
#define MBEDTLS_PERF_SHA_END2(...)

#define MBEDTLS_PERF_ECP_END1_WITH_ID(...)
#define MBEDTLS_PERF_ECP_END2_WITH_ID(...)

#endif

#ifdef __cplusplus
}
#endif

#endif /* MBEDTLS_PERF_UTILS_H */