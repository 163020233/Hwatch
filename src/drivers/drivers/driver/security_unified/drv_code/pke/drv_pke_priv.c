/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: drv pke inner implement.
 *
 * Create: 2025-01-07
*/
#include "drv_pke_priv.h"
#include "crypto_pke_common.h"
#include "crypto_hash_struct.h"
#include "drv_trng.h"

/* result size */
#define HASH_SIZE_SHA_1                            20
#define HASH_SIZE_SHA_224                          28
#define HASH_SIZE_SHA_256                          32
#define HASH_SIZE_SHA_384                          48
#define HASH_SIZE_SHA_512                          64
#define HASH_SIZE_SHA_MAX                          64

uint32_t drv_get_hash_len(const drv_pke_hash_type hash_type)
{
    switch (hash_type) {
        case DRV_PKE_HASH_TYPE_SHA1:
            return HASH_SIZE_SHA_1;
        case DRV_PKE_HASH_TYPE_SHA224:
            return HASH_SIZE_SHA_224;
        case DRV_PKE_HASH_TYPE_SHA256:
            return HASH_SIZE_SHA_256;
        case DRV_PKE_HASH_TYPE_SHA384:
            return HASH_SIZE_SHA_384;
        case DRV_PKE_HASH_TYPE_SHA512:
            return HASH_SIZE_SHA_512;
        default:
            crypto_log_err("unsupport hash type\n");
            return HASH_SIZE_SHA_256;
    }
}

static const td_s8 g_bits[] = {0, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4};
#define MAX_LOW_2BITS              3
#define MAX_LOW_3BITS              7
#define MAX_LOW_4BITS              0xF
#define MAX_LOW_8BITS              0xFF

#define SHIFT_4BITS                4
#define SHIFT_8BITS                8
#define SHIFT_16BITS               16
#define SHIFT_24BITS               24

#define BOUND_VALUE_1              1
unsigned int rsa_get_bit_num(const unsigned char *big_num, unsigned int num_len)
{
    unsigned int i = 0;
    td_s8 num = 0;

    for (i = 0; i < num_len; i++) {
        if (big_num[i] == 0x00) {
            continue;
        }
        num = g_bits[(big_num[i] & (MAX_LOW_8BITS - MAX_LOW_4BITS)) >> SHIFT_4BITS];
        if (num > 0) {
            return (num_len - i - BOUND_VALUE_1) * CRYPTO_BITS_IN_BYTE + num + CRYPTO_WORD_WIDTH;
        }

        num = g_bits[big_num[i] & MAX_LOW_4BITS];
        if (num > 0) {
            return (num_len - i - BOUND_VALUE_1) * CRYPTO_BITS_IN_BYTE + num;
        }
    }
    return 0;
}

#if defined(CONFIG_PKE_RSA_SHA1_SUPPORT)
static const unsigned char g_asn1_sha1[] = {
    0x30, 0x21, 0x30, 0x09, 0x06, 0x05, 0x2b, 0x0e,
    0x03, 0x02, 0x1a, 0x05, 0x00, 0x04, 0x14,
};

static const unsigned char g_empty_l_sha1[] = {
    0xda, 0x39, 0xa3, 0xee, 0x5e, 0x6b, 0x4b, 0x0d,
    0x32, 0x55, 0xbf, 0xef, 0x95, 0x60, 0x18, 0x90,
    0xaf, 0xd8, 0x07, 0x09,
};
#endif

#if defined(CONFIG_PKE_RSA_SHA224_SUPPORT)
static const unsigned char g_asn1_sha224[] = {
    0x30, 0x2d, 0x30, 0x0d, 0x06, 0x09, 0x60, 0x86,
    0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x04, 0x05,
    0x00, 0x04, 0x1c
};

static const unsigned char g_empty_l_sha224[] = {
    0xd1, 0x4a, 0x02, 0x8c, 0x2a, 0x3a, 0x2b, 0xc9,
    0x47, 0x61, 0x02, 0xbb, 0x28, 0x82, 0x34, 0xc4,
    0x15, 0xa2, 0xb0, 0x1f, 0x82, 0x8e, 0xa6, 0x2a,
    0xc5, 0xb3, 0xe4, 0x2f
};
#endif

#if defined(CONFIG_PKE_RSA_SHA256_SUPPORT)
static const unsigned char g_asn1_sha256[] = {
    0x30, 0x31, 0x30, 0x0d, 0x06, 0x09, 0x60, 0x86,
    0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x01, 0x05,
    0x00, 0x04, 0x20,
};

static const unsigned char g_empty_l_sha256[] = {
    0xe3, 0xb0, 0xc4, 0x42, 0x98, 0xfc, 0x1c, 0x14,
    0x9a, 0xfb, 0xf4, 0xc8, 0x99, 0x6f, 0xb9, 0x24,
    0x27, 0xae, 0x41, 0xe4, 0x64, 0x9b, 0x93, 0x4c,
    0xa4, 0x95, 0x99, 0x1b, 0x78, 0x52, 0xb8, 0x55,
};
#endif

#if defined(CONFIG_PKE_RSA_SHA384_SUPPORT)
static const unsigned char g_asn1_sha384[] = {
    0x30, 0x41, 0x30, 0x0d, 0x06, 0x09, 0x60, 0x86,
    0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x02, 0x05,
    0x00, 0x04, 0x30,
};

static const unsigned char g_empty_l_sha384[] = {
    0x38, 0xb0, 0x60, 0xa7, 0x51, 0xac, 0x96, 0x38,
    0x4c, 0xd9, 0x32, 0x7e, 0xb1, 0xb1, 0xe3, 0x6a,
    0x21, 0xfd, 0xb7, 0x11, 0x14, 0xbe, 0x07, 0x43,
    0x4c, 0x0c, 0xc7, 0xbf, 0x63, 0xf6, 0xe1, 0xda,
    0x27, 0x4e, 0xde, 0xbf, 0xe7, 0x6f, 0x65, 0xfb,
    0xd5, 0x1a, 0xd2, 0xf1, 0x48, 0x98, 0xb9, 0x5b,
};
#endif

#if defined(CONFIG_PKE_RSA_SHA512_SUPPORT)
static const unsigned char g_asn1_sha512[] = {
    0x30, 0x51, 0x30, 0x0d, 0x06, 0x09, 0x60, 0x86,
    0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x03, 0x05,
    0x00, 0x04, 0x40,
};

static const unsigned char g_empty_l_sha512[] = {
    0xcf, 0x83, 0xe1, 0x35, 0x7e, 0xef, 0xb8, 0xbd,
    0xf1, 0x54, 0x28, 0x50, 0xd6, 0x6d, 0x80, 0x07,
    0xd6, 0x20, 0xe4, 0x05, 0x0b, 0x57, 0x15, 0xdc,
    0x83, 0xf4, 0xa9, 0x21, 0xd3, 0x6c, 0xe9, 0xce,
    0x47, 0xd0, 0xd1, 0x3c, 0x5d, 0x85, 0xf2, 0xb0,
    0xff, 0x83, 0x18, 0xd2, 0x87, 0x7e, 0xec, 0x2f,
    0x63, 0xb9, 0x31, 0xbd, 0x47, 0x41, 0x7a, 0x81,
    0xa5, 0x38, 0x32, 0x7a, 0xf9, 0x27, 0xda, 0x3e,
};
#endif

#if defined(CONFIG_PKE_RSA_LABEL_SUPPORT)
static unsigned char g_l_hash[HASH_SIZE_SHA_512] = {0x0};
#endif

typedef struct {
    drv_pke_hash_type hash_type;
    unsigned int hash_len;
    const unsigned char *asn1_data;
    unsigned int asn1_len;
    const unsigned char *lhash_data;
} pke_hash_item;

static const pke_hash_item g_pke_hash_list[] = {
#if defined(CONFIG_PKE_RSA_SHA1_SUPPORT)
    {
        .hash_type = DRV_PKE_HASH_TYPE_SHA1,
        .hash_len = HASH_SIZE_SHA_1,
        .asn1_data = g_asn1_sha1,
        .asn1_len = sizeof(g_asn1_sha1),
        .lhash_data = g_empty_l_sha1,
    },
#endif
#if defined(CONFIG_PKE_RSA_SHA224_SUPPORT)
    {
        .hash_type = DRV_PKE_HASH_TYPE_SHA224,
        .hash_len = HASH_SIZE_SHA_224,
        .asn1_data = g_asn1_sha224,
        .asn1_len = sizeof(g_asn1_sha224),
        .lhash_data = g_empty_l_sha224,
    },
#endif
#if defined(CONFIG_PKE_RSA_SHA256_SUPPORT)
    {
        .hash_type = DRV_PKE_HASH_TYPE_SHA256,
        .hash_len = HASH_SIZE_SHA_256,
        .asn1_data = g_asn1_sha256,
        .asn1_len = sizeof(g_asn1_sha256),
        .lhash_data = g_empty_l_sha256,
    },
#endif
#if defined(CONFIG_PKE_RSA_SHA384_SUPPORT)
    {
        .hash_type = DRV_PKE_HASH_TYPE_SHA384,
        .hash_len = HASH_SIZE_SHA_384,
        .asn1_data = g_asn1_sha384,
        .asn1_len = sizeof(g_asn1_sha384),
        .lhash_data = g_empty_l_sha384,
    },
#endif
#if defined(CONFIG_PKE_RSA_SHA512_SUPPORT)
    {
        .hash_type = DRV_PKE_HASH_TYPE_SHA512,
        .hash_len = HASH_SIZE_SHA_512,
        .asn1_data = g_asn1_sha512,
        .asn1_len = sizeof(g_asn1_sha512),
        .lhash_data = g_empty_l_sha512,
    },
#endif
};

int pkcs1_get_hash(const drv_pke_hash_type hash_type, const drv_pke_data *label,
    rsa_pkcs1_hash_info *hash_info)
{
    unsigned int i;
    int ret = CRYPTO_FAILURE;
    drv_pke_data h_hash = {0};
    const pke_hash_item *item = NULL;

    crypto_unused(ret);
    crypto_unused(h_hash);
    crypto_unused(label);
    crypto_unused(hash_info);

    for (i = 0; i < crypto_array_size(g_pke_hash_list); i++) {
        if (g_pke_hash_list[i].hash_type == hash_type) {
            item = &g_pke_hash_list[i];
            hash_info->hash_type = item->hash_type;
            hash_info->hash_len = item->hash_len;
            hash_info->asn1_data = (unsigned char *)item->asn1_data;
            hash_info->asn1_len = item->asn1_len;
            hash_info->lhash_data = (unsigned char *)item->lhash_data;
            break;
        }
    }
    if (item == NULL) {
        crypto_log_err("unsupport hash type\n");
        return PKE_COMPAT_ERRNO(ERROR_UNSUPPORT);
    }

#if defined(CONFIG_PKE_RSA_LABEL_SUPPORT)
    if (label != TD_NULL && label->data != TD_NULL && label->length != 0) {
        hash_info->lhash_data = g_l_hash;
        h_hash.data = hash_info->lhash_data;
        h_hash.length = hash_info->hash_len;

        ret = crypto_pke_common_calc_hash(label, 1, hash_info->hash_type, &h_hash);
        crypto_chk_return(ret != TD_SUCCESS, ret, "crypto_pke_common_calc_hash failed, ret is 0x%x\n", ret);
    }
#endif

    return TD_SUCCESS;
}

int inner_get_random(unsigned char *random, const unsigned int size)
{
    return drv_cipher_trng_get_multi_random(size, random);
}

#define PKE_RANDOM_MAX_TRY_TIMES    100
int inner_get_random_with_nonzero_octets(unsigned char *random, unsigned int size)
{
    int ret;
    unsigned int i;
    unsigned int try_times = 0;

    ret = inner_get_random(random, size);
    crypto_chk_return(ret != TD_SUCCESS, ret, "inner_get_random failed\n");

    for (i = 0; i < size; i++) {
        try_times = 0;
        while (random[i] == 0 && try_times < PKE_RANDOM_MAX_TRY_TIMES) {
            ret = inner_get_random(&random[i], 1);
            crypto_chk_return(ret != TD_SUCCESS, ret, "inner_get_random failed\n");
            try_times++;
        }
        if (try_times >= PKE_RANDOM_MAX_TRY_TIMES) {
            return CRYPTO_FAILURE;
        }
    }
    return TD_SUCCESS;
}

int inner_get_limit_random(unsigned char *rand, const unsigned char *limit, const unsigned int size)
{
    int ret;
    unsigned int start_idx = 0;
    while (start_idx < size && limit[start_idx] == 0) {
        start_idx++;
    }
    if (start_idx == size) {
        crypto_log_err("the limit is zero!\n");
        return CRYPTO_FAILURE;
    }
    (void)memset_s(rand, size, 0, size);
    ret = inner_get_random(&rand[start_idx], size - start_idx);
    crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "inner_get_random failed\n");

    /* Check is required, cause memset for rand may set limit[0] to zero. */
    if (limit[start_idx] == 0) {
        crypto_log_err("limit[0] is zero!\n");
        return CRYPTO_FAILURE;
    }
    rand[start_idx] = rand[start_idx] % limit[start_idx];

    return CRYPTO_SUCCESS;
}

td_bool inner_drv_is_in_range(const uint8_t *value, const uint8_t *range, uint32_t len)
{
    if (memcmp(value, range, len) < 0 && inner_drv_is_zero(value, len) == false) {
        return TD_TRUE;
    }
    return TD_FALSE;
}

int inner_sm2_kdf(const drv_pke_ecc_point *param, unsigned char *out, const unsigned int klen)
{
    int ret = CRYPTO_FAILURE;
    unsigned int block = 0;
    unsigned int i = 0;
    unsigned int ct = 0;
    drv_pke_data arr[3];
    unsigned char h[CRYPTO_SM2_LEN_IN_BYTES] = {0};
    drv_pke_data hash = {CRYPTO_SM2_LEN_IN_BYTES, h};

    crypto_param_check(param == TD_NULL);
    crypto_param_check(param->x == TD_NULL);
    crypto_param_check(param->y == TD_NULL);
    crypto_param_check(out == TD_NULL);

    arr[0].data = param->x;
    arr[0].length = CRYPTO_SM2_LEN_IN_BYTES;
    arr[1].data = param->y;
    arr[1].length = CRYPTO_SM2_LEN_IN_BYTES;
    arr[2].data = (unsigned char *)&ct; // 2 is index of arr
    arr[2].length = (uint32_t)sizeof(ct); // 2 is index of arr
    if (klen == 0) {
        return TD_SUCCESS;
    }

    block = (klen + CRYPTO_SM2_LEN_IN_BYTES - 1) / CRYPTO_SM2_LEN_IN_BYTES;
    for (i = 0; i < block; i++) {
        ct = crypto_cpu_to_be32(i + 1);
        /* *** H = SM3(X || Y || CT) *** */
        ret = crypto_pke_common_calc_hash(arr, crypto_array_size(arr), DRV_PKE_HASH_TYPE_SM3, &hash);
        crypto_chk_return(ret != TD_SUCCESS, ret, "crypto_pke_common_calc_hash failed, ret is 0x%x\n", ret);

        if (i == (block - 1)) {
            ret = memcpy_s(out + i * CRYPTO_SM2_LEN_IN_BYTES, CRYPTO_SM2_LEN_IN_BYTES, h,
                klen - i * CRYPTO_SM2_LEN_IN_BYTES);
            crypto_chk_return(ret != EOK, ret, "memcpy_s failed, ret is 0x%x\n", ret);
        } else {
            (void)memcpy_s(out + i * CRYPTO_SM2_LEN_IN_BYTES, CRYPTO_SM2_LEN_IN_BYTES, h, CRYPTO_SM2_LEN_IN_BYTES);
        }
    }
    if (inner_drv_is_zero(out, klen)) {
        return CRYPTO_FAILURE;
    }

    return TD_SUCCESS;
}

int inner_ecc_hash_len_check(unsigned int klen, unsigned int hash_len)
{
#if defined(CONFIG_PKE_ECC_SECURITY_STRENGTH_CHECK)
    /* increase security strength check. for nist-fips-224, it's the same security strength with SHA224 */
    drv_crypto_pke_check_param((hash_len < klen) &&
        !(hash_len == HASH_SIZE_SHA_224 && klen == DRV_PKE_LEN_224) &&
        !(hash_len == HASH_SIZE_SHA_512 && (klen == DRV_PKE_LEN_521 || klen == DRV_PKE_LEN_576)));
#endif
    crypto_unused(klen);
    crypto_unused(hash_len);
    return CRYPTO_SUCCESS;
}

int32_t inner_modular_chk(const drv_pke_data *p, bool is_odd_required)
{
    bool length_chk_valid = false;
    uint32_t i;
    uint32_t valid_length_list[] = {
        DRV_PKE_LEN_192, DRV_PKE_LEN_256, DRV_PKE_LEN_384, DRV_PKE_LEN_448, DRV_PKE_LEN_512,
        DRV_PKE_LEN_1024, DRV_PKE_LEN_1536, DRV_PKE_LEN_2048, DRV_PKE_LEN_3072, DRV_PKE_LEN_4096
    };
    /* 1. length check */
    for (i = 0; i < crypto_array_size(valid_length_list); i++) {
        if (p->length == valid_length_list[i]) {
            length_chk_valid = true;
            break;
        }
    }
    if (length_chk_valid == false) {
        crypto_log_err("modular length is invalid!\n");
        return ERROR_INVALID_RSA_MODULAR;
    }
    /* 2. even check. */
    if (is_odd_required) {
        if ((p->data[p->length - 1] & 0x1) == 0) {
            crypto_log_err("modular is even!\n");
            return ERROR_INVALID_RSA_MODULAR;
        }
    }
    /* 3. zero check. */
    if (inner_drv_is_zero(p->data, p->length)) {
        crypto_log_err("modular is zero!\n");
        return ERROR_INVALID_RSA_MODULAR;
    }
    /* 4. modular valid bit check. */
    if (crypto_pke_common_get_high_0_bit_length(p) > CRYPTO_PKE_MAX_OFFSET_VALID_BIT) {
        crypto_log_err("modular valid bit is too small!\n");
        return ERROR_INVALID_RSA_MODULAR;
    }

    return CRYPTO_SUCCESS;
}

int32_t inner_ecdsa_type_check(drv_pke_ecc_curve_type ecc_type)
{
    drv_pke_curve_id curve_id = crypto_pke_get_attr(ecc_type, CRYPTO_PKE_CURVE_ID_MASK, CRYPTO_PKE_CURVE_ID_SHIFT);
    if (curve_id <= FIPS_P521R_CURVE_ID) {
        return CRYPTO_SUCCESS;
    }
    if (curve_id == SM2_CURVE_ID) {
        return CRYPTO_SUCCESS;
    }
    return ERROR_INVALID_PARAM;
}