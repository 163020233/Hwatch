/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 *
 * Description: Provides cipher driver pke utils. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-11-28, Create file. \n
 */
#include "crypto_pke_common.h"
#include "hal_pke_alg.h"
#include "drv_hash.h"
#include "drv_trng.h"

#define FILE_ID FILE_ID_COMMON_CRYPTO_PKE_COMMON

#define BYTE_VALUE  256
#define MAX_ECC_LENGTH 72
#define PKE_COMPAT_ERRNO(err_code)      HAL_COMPAT_ERRNO(ERROR_MODULE_PKE, err_code)

#define MAX_HASH_SIZE       64      /* SHA512. */
/* According to "RFC8032 6. Ed25519 Python Illustration" secret_expand function.  */
int32_t crypto_pke_common_secret_expand(const uint8_t *secret, uint8_t *a,
    uint8_t *prefix, uint32_t len)
{
    int32_t ret;
    uint32_t i;
    uint8_t h[MAX_HASH_SIZE];
    drv_pke_data hash_data = {
        .data = h,
        .length = DRV_PKE_LEN_512,
    };
    drv_pke_data secret_data = {
        .data = (uint8_t *)secret,
        .length = len
    };

    ret = crypto_pke_common_calc_hash(&secret_data, 1, DRV_PKE_HASH_TYPE_SHA512, &hash_data);
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "crypto_pke_common_calc_hash failed, ret is 0x%x\n", ret);

    for (i = 0; i < len; i++) {
        a[i] = h[len - 1 - i];
    }

    /* [255], [254], [2], [1], [0] must be 0,1,0,0,0 */
    a[len - 1] &= 0xF8;
    a[0] &= 0x7F;
    a[0] |= 0x40;

    ret = memcpy_s(prefix, len, h + len, len);
    drv_err_chk_return(ret != EOK, CRYPTO_FAILURE, "memcpy_s failed\n");

    return CRYPTO_SUCCESS;
}

void crypto_pke_common_little_big_endian_trans(uint8_t *output_endian, const uint8_t *input_endian,
    uint32_t byte_len)
{
    uint32_t i;
    for (i = 0; i < byte_len; i++) {
        output_endian[i] = input_endian[byte_len - 1 - i];
    }
}

bool inner_drv_is_zero(const uint8_t *val, uint32_t length)
{
    uint32_t i;
    for (i = 0; i < length; i++) {
        if (val[i] != 0) {
            return false;
        }
    }
    return true;
}

static void inner_arry_right_shift_value(uint8_t *k, uint32_t k_len, uint32_t shift_bit)
{
    uint32_t i = 0;
    uint8_t left_mask = ((1 << shift_bit) - 1) & 0xFF;
    uint8_t right_mask = ~((1 << shift_bit) - 1) & 0xFF;
    uint32_t left_shift_bit = CRYPTO_BITS_IN_BYTE - shift_bit;

    for (i = k_len - 1; i > 0; i--) {
        k[i] = (((k[i] & right_mask) >> shift_bit) & (0xFF >> shift_bit)) |
            (((k[i - 1] & left_mask) << left_shift_bit) & (0xFF << left_shift_bit));
    }
    k[0] = ((k[0] & right_mask) >> shift_bit) & (0xFF >> shift_bit);

    return;
}

static void inner_bn_add(const uint8_t *a, const uint8_t *b, uint8_t *c, uint32_t len,
    uint32_t *carry)
{
    int32_t i;
    *carry = 0;
    uint32_t addition = 0;
    for (i = (int)(len - 1); i >= 0; i--) {
        addition = a[i] + b[i] + *carry;
        c[i] = addition % CRYPTO_BYTE_VALUE;
        *carry = addition / CRYPTO_BYTE_VALUE;
    }
}

static void inner_bn_xor(const uint8_t *a, const uint8_t *b, uint8_t *c, uint32_t len)
{
    uint32_t i;
    for (i = 0; i < len; i++) {
        c[i] = a[i] ^ b[i];
    }
}

static void inner_bn_and(const uint8_t *a, const uint8_t *b, uint8_t *c, uint32_t len)
{
    uint32_t i;
    for (i = 0; i < len; i++) {
        c[i] = a[i] & b[i];
    }
}

/* According to A184616. */
int32_t crypto_pke_common_bin2naf_fast(const uint8_t *x, uint8_t *positive_bits, uint8_t *negative_bits,
    const uint32_t len, uint32_t *carry)
{
    int32_t ret = CRYPTO_FAILURE;
    uint8_t *buf = NULL;
    uint8_t *xh = NULL;
    uint8_t *x3 = NULL;
    uint8_t *c = NULL;

    buf = crypto_malloc_coherent(MAX_NAF_LEN * 3);  // 3: for xh/x3/c
    drv_err_chk_return(buf == NULL, ERROR_MALLOC, "malloc failed\n");

    xh = buf;
    x3 = xh + MAX_NAF_LEN;
    c = x3 + MAX_NAF_LEN;

    ret = memcpy_s(xh, MAX_NAF_LEN, x, len);
    drv_err_chk_goto_with_ret(ret, ret != EOK, exit_free, ERROR_MEMCPY_S, "memcpy_s failed\n");

    /* Step 1. xh = x >> 1. */
    inner_arry_right_shift_value(xh, len, 1);

    /* Step 2. x3 = x + xh. */
    inner_bn_add(x, xh, x3, len, carry);

    /* Step 3. c = xh ^ x3. */
    inner_bn_xor(xh, x3, c, len);

    /* Step 4. np = x3 & c. */
    inner_bn_and(x3, c, positive_bits, len);

    /* Step 5. nm = xh & c. */
    inner_bn_and(xh, c, negative_bits, len);

    ret = CRYPTO_SUCCESS;
exit_free:
    crypto_free_coherent(buf);
    return ret;
}
/*
 * c = a + b.
 * required: len(a) = len(b) = len, len(c) = len + 1.
 */
void crypto_pke_common_normal_add(const uint8_t *a, const uint8_t *b, uint8_t *c,
    uint32_t len)
{
    uint32_t addition = 0;
    inner_bn_add(a, b, &c[1], len, &addition);
    c[0] = (uint8_t)addition;
}

/* a >= b - 1, a.length = c.length, a.length >= b.length */
int32_t crypto_pke_common_normal_sub(const drv_pke_data *a, const drv_pke_data *b, const drv_pke_data *c)
{
    int32_t ret;
    int32_t i = 0;
    int32_t j = 0;
    uint8_t carry = 0;

    ret = memcpy_s(c->data, c->length, a->data, a->length);
    drv_err_chk_return(ret != EOK, ERROR_MEMCPY_S, "memcpy_s failed\n");

    for (i = (int)a->length - 1, j = (int)b->length - 1; i >= 1 && j >= 0; i--, j--) {
        if (a->data[i] < b->data[j] || a->data[i] - b->data[j] < carry) {
            /* need to call one */
            c->data[i] = c->data[i] + ((uint8_t)BYTE_VALUE - b->data[j] - carry);
            carry = 1;  /* note next byte */
        } else {
            c->data[i] -= (b->data[j] + carry);
            carry = 0;  /* note next byte */
        }
    }

    /* process c->data[i] */
    if (j == 0) {               /* a->length == b->length */
        c->data[i] -= (b->data[j] + carry);
    } else {                    /* a->length > b->length */
        c->data[i] -= carry;
    }

    return CRYPTO_SUCCESS;
}

void crypto_pke_common_bn_add_one(uint8_t *data, uint32_t len)
{
    uint32_t idx = len - 1;
    uint8_t carry = 1;
    uint32_t value;
    while (carry == 1 && idx >= 1) {
        value = data[idx] + carry;
        if (value > 0xFF) {
            carry = 1;
        } else {
            carry = 0;
        }
        data[idx] = value % (0x100);
        idx--;
    }
    data[idx] += carry;
}

void crypto_pke_common_bn_sub_one(uint8_t *data, uint32_t len)
{
    int32_t idx = (int)(len - 1);
    uint8_t carry = 1;
    while (carry == 1 && idx >= 0) {
        if (data[idx] == 0) {
            carry = 1;
            data[idx] = 0xFF;
        } else {
            carry = 0;
            data[idx]--;
        }
        if (idx == 0) {
            return;
        }
        idx--;
    }
}

void crypto_pke_common_get_mont_param(const uint8_t p[PKE_MONT_LEN_IN_BYTE],
    uint8_t mont_param[PKE_MONT_LEN_IN_BYTE])
{
    int32_t i = 0;
    uint8_t tmp[PKE_MONT_LEN_IN_BYTE + 1] = {0};
    uint8_t *s = &tmp[1];
    uint8_t s_len = PKE_MONT_LEN_IN_BYTE;
    uint8_t *x = mont_param;
    uint8_t x_len = PKE_MONT_LEN_IN_BYTE;
    uint8_t xi;

    /* Step 1. Compute s = (p - 1) >> 1. */
    (void)memcpy_s(s, PKE_MONT_LEN_IN_BYTE, p, PKE_MONT_LEN_IN_BYTE);
    crypto_pke_common_bn_sub_one(s, s_len);
    inner_arry_right_shift_value(s, s_len, 1);

    /* Step 2. Initialize x = 1. */
    (void)memset_s(x, x_len, 0, x_len);
    x[x_len - 1] = 1;

    for (i = 1; i < PKE_MONT_LEN_IN_BYTE * CRYPTO_BITS_IN_BYTE; i++) {
        xi = s[s_len - 1] & 1;
        if (xi == 1) {
            /* Compute s = s + p. */
            crypto_pke_common_normal_add(s, p, tmp, s_len);
            /* Compute x = x + 2^i. */
            x[x_len - i / CRYPTO_BITS_IN_BYTE - 1] |= (1 << (i % CRYPTO_BITS_IN_BYTE));
        }
        /* s = s >> 1. */
        inner_arry_right_shift_value(s, s_len, 1);
    }
    /* x = 2^bit_len - x, means x = ~x + 1. */
    for (i = 0; i < PKE_MONT_LEN_IN_BYTE; i++) {
        x[i] = ~x[i];
    }
    crypto_pke_common_bn_add_one(x, x_len);
}

int32_t crypto_pke_rsa_hash_len_check(uint32_t klen, drv_pke_hash_type hash_type)
{
#if defined(CONFIG_PKE_RSA_SECURITY_STRENGTH_CHECK)
    /* check whether the security strength is the same. */
    drv_err_chk_return((klen == DRV_PKE_LEN_2048 && hash_type < DRV_PKE_HASH_TYPE_SHA224) ||
        (klen == DRV_PKE_LEN_3072 && hash_type < DRV_PKE_HASH_TYPE_SHA256) ||
        (klen == DRV_PKE_LEN_4096 && hash_type < DRV_PKE_HASH_TYPE_SHA384),
        ERROR_PKE_SECURITY_STRENGTH_NOT_MATCH, "the security strength of the hash doesn't match the rsa!\n");
#endif
    crypto_unused(klen);
    crypto_unused(hash_type);

    return CRYPTO_SUCCESS;
}

static crypto_hash_type drv_pke_get_hash_type(drv_pke_hash_type hash_type)
{
    switch (hash_type) {
        case DRV_PKE_HASH_TYPE_SHA1:
            return CRYPTO_HASH_TYPE_SHA1;
        case DRV_PKE_HASH_TYPE_SHA224:
            return CRYPTO_HASH_TYPE_SHA224;
        case DRV_PKE_HASH_TYPE_SHA256:
            return CRYPTO_HASH_TYPE_SHA256;
        case DRV_PKE_HASH_TYPE_SHA384:
            return CRYPTO_HASH_TYPE_SHA384;
        case DRV_PKE_HASH_TYPE_SHA512:
            return CRYPTO_HASH_TYPE_SHA512;
        case DRV_PKE_HASH_TYPE_SM3:
            return CRYPTO_HASH_TYPE_SM3;
        default:
            drv_set_err(ERROR_UNEXPECTED, "logged error");
            return CRYPTO_HASH_TYPE_INVALID;
    }
}

#define INVALID_HANDLE  0xFFFF0000
int32_t crypto_pke_common_calc_hash(const drv_pke_data *arr, uint32_t arr_len,
    const drv_pke_hash_type hash_type, drv_pke_data *hash)
{
    int32_t ret = CRYPTO_FAILURE;
    void *hash_ctx = NULL;
    uint32_t i;
    crypto_hash_type hash_alg;

    hash_alg = drv_pke_get_hash_type(hash_type);
    if (hash_alg == CRYPTO_HASH_TYPE_INVALID) {
        drv_set_err(PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), "direct error return");
        return PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM);
    }

    ret = drv_cipher_hash_compute_setup(&hash_ctx);
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "hash setup failed, ret is 0x%x\n", ret);

    ret = drv_cipher_hash_compute_start(hash_ctx, hash_alg);
    drv_err_chk_goto(ret != CRYPTO_SUCCESS, exit_teardown, "hash start failed, ret is 0x%x\n", ret);

    for (i = 0;i < arr_len; i++) {
        ret = drv_cipher_hash_compute_update(hash_ctx, arr[i].data, arr[i].length);
        drv_err_chk_goto(ret != CRYPTO_SUCCESS, exit_teardown, "hash update failed, ret is 0x%x\n", ret);
    }

    ret = drv_cipher_hash_compute_finish(hash_ctx, hash->data, hash->length);
    drv_err_chk_goto(ret != CRYPTO_SUCCESS, exit_teardown, "hash finish failed, ret is 0x%x\n", ret);

exit_teardown:
    (void)drv_cipher_hash_compute_teardown(hash_ctx);
    return ret;
}

int32_t crypto_pke_common_data_cmp(const drv_pke_data *a, const drv_pke_data *b)
{
    uint32_t diff;
    if (a->length == b->length) {
        return memcmp(a->data, b->data, b->length);
    }
    /* Case 1. a.length > b.length. */
    if (a->length > b->length) {
        diff = a->length - b->length;
        if (inner_drv_is_zero(a->data, diff)) {
            return memcmp(&a->data[diff], b->data, b->length);
        } else {
            return 1;
        }
    }

    /* Case 2. a.length < b.length. */
    return 0 - crypto_pke_common_data_cmp(b, a);
}

#define MAX_SOFT_ADD_MOD_TIMES 1024
int32_t crypto_pke_common_add_mod(const drv_pke_data *a, const drv_pke_data *b, drv_pke_data *c,
    const drv_pke_data *n)
{
    int32_t ret;
    uint8_t and_data[MAX_ECC_LENGTH + 1] = { 0 };
    uint8_t sub_data[MAX_ECC_LENGTH + 1] = { 0 };
    drv_pke_data and;
    drv_pke_data sub;
    uint32_t len = a->length;
    uint32_t count = 0;

    drv_err_chk_return(a->length > MAX_ECC_LENGTH, ERROR_INVALID_PARAM, "add param length invalid!");
    drv_err_chk_return(a->length != b->length || a->length != c->length, ERROR_INVALID_PARAM, "add mod length invalid!");

    and.data = and_data;
    and.length = len + 1;
    sub.data = sub_data;
    sub.length = len + 1;

    /* normal_add required: len(a) = len(b) = len, len(c) = len + 1. */
    (void)crypto_pke_common_normal_add(a->data, b->data, and.data, len);

    /* a + b < n */
    if (crypto_pke_common_data_cmp(&and, n) < 0) {
        ret = memcpy_s(c->data, c->length, and.data + 1, and.length - 1);
        return ret;
    }

    /* a + b >= n */
    while (crypto_pke_common_data_cmp(&and, n) >= 0) {
        drv_err_chk_return(count > MAX_SOFT_ADD_MOD_TIMES, ERROR_UNEXPECTED, "add mod n is too small!");

        /* normal_sub required: len(a) = len(c), len(a) >= len(b), a > b. */
        ret = crypto_pke_common_normal_sub(&and, n, &sub);
        drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "crypto_pke_common_normal_sub failed!");

        ret = memcpy_s(and.data, and.length, sub.data, sub.length);
        drv_err_chk_return(ret != EOK, ERROR_MEMCPY_S, "memcpy_s failed!");

        count++;
    }

    ret = memcpy_s(c->data, c->length, sub.data + 1, sub.length - 1);
    drv_err_chk_return(ret != EOK, ERROR_MEMCPY_S, "memcpy_s failed!");
    return ret;
}

bool crypto_pke_common_get_bit_from_low_to_high(const uint8_t *in, uint32_t size, uint32_t pos)
{
    uint32_t byteIndex = (size * 8 - pos) / 8;
    uint32_t bitIndex = pos % 8;

    return (in[byteIndex] >> bitIndex) & 1;
}

bool crypto_pke_common_get_bit_from_high_to_low(const uint8_t *bn, uint32_t index)
{
    return bn[index / CRYPTO_BITS_IN_BYTE] & (0x1 << (CRYPTO_BITS_IN_BYTE - index % CRYPTO_BITS_IN_BYTE - 1));
}

uint32_t crypto_pke_common_get_high_0_bit_length(const drv_pke_data *a)
{
    uint32_t i;
    int32_t j;
    uint32_t cnt = 0;
    bool flag = false;

    for (i = 0; i < a->length; i++) {
        for (j = 7; j >= 0; j--) {  /* 7: highest bit position in one byte */
            if (((a->data[i] >> j) & 1) != 0) {
                flag = true;
                break;
            } else {
                cnt++;
            }
        }
        if (flag) {
            break;
        }
    }
    return cnt;
}

/* len(a) = len(b) */
uint32_t crypto_pke_common_get_two_bn_high_0_bit_length(const drv_pke_data *a, const drv_pke_data *b)
{
    uint32_t i;
    int32_t j;
    uint32_t cnt = 0;
    bool flag = false;
    bool bit1;
    bool bit2;

    for (i = 0; i < a->length; i++) {
        for (j = 7; j >= 0; j--) {  /* 7: highest bit position in one byte */
            bit1 = (a->data[i] >> j) & 1;
            bit2 = (b->data[i] >> j) & 1;
            if ((bit1 != 0) && (bit2 != 0)) {
                flag = true;
                break;
            } else {
                cnt++;
            }
        }
        if (flag) {
            break;
        }
    }
    return cnt;
}

void crypto_pke_common_clear_highest_bit(drv_pke_data *a)
{
    uint32_t i;
    int32_t j;
    bool flag = false;

    for (i = 0; i < a->length; i++) {
        for (j = 7; j >= 0; j--) {    /* 7: highest bit position in one byte */
            if (((a->data[i] >> j) & 1) != 0) {
                a->data[i] &= ~(1U << j);
                flag = true;
                break;
            }
        }
        if (flag) {
            break;
        }
    }
}

void crypto_pke_common_set_bit_to_1_and_clear_higher_bit(uint8_t *byte, uint32_t pos)
{
    /* clear pos higher bit to 0 */
    const uint8_t mask_lower = (1 << pos) - 1;
    uint8_t lower_bits = *byte & mask_lower;
    /* set pos bit to 1 */
    *byte =  lower_bits | (1 << pos);
}

uint32_t crypto_pke_common_get_high_0_bit_length_sec(const drv_pke_data *a)
{
    uint32_t i;
    int32_t j;
    uint32_t cnt = 0;
    bool flag = false;

    for (i = 0; i < a->length; i++) {
        for (j = 7; j >= 0; j--) {  /* 7: highest bit position in one byte */
            if (((a->data[i] >> j) & 1) != 0) {
                flag = true;
            } else if (!flag) {
                cnt++;
            }
        }
    }
    return cnt;
}

void crypto_pke_common_clear_highest_bit_sec(drv_pke_data *a)
{
    unsigned int i;
    int j;
    bool flag = false;

    for (i = 0; i < a->length; i++) {
        for (j = 7; j >= 0; j--) {    /* 7: highest bit position in one byte */
            if ((!flag) && (((a->data[i] >> j) & 1) != 0)) {
                a->data[i] &= ~(1U << j);
                flag = true;
            }
        }
    }
}

#define MAXIMUM_DELAY_COUNT 200
void random_delay(void)
{
    uint32_t random_delay = 0;
    drv_cipher_trng_get_random(&random_delay);
    uint32_t count = random_delay & MAXIMUM_DELAY_COUNT;
    for (uint32_t i = 0; i < count; i++) {
        __asm__ __volatile__("nop");
    }
}

int memcmp_se(const void *cs, const void *ct, unsigned int count)
{
    volatile int ret = CRYPTO_ENHANCE_FAILURE;

    random_delay();
    ret = memcmp(cs, ct, count);
    if (ret != 0) {
        return CRYPTO_ENHANCE_FAILURE;
    }
    random_delay();
    ret = memcmp(cs, ct, count);
    if (ret != 0) {
        return CRYPTO_ENHANCE_FAILURE;
    }
    return CRYPTO_ENHANCE_SUCCESS;
}

int memset_se(void *dest, unsigned int dest_max, unsigned char c, unsigned int count)
{
    volatile int ret = CRYPTO_ENHANCE_FAILURE;

    random_delay();
    ret = memset_s(dest, dest_max, c, count);
    if (ret != EOK) {
        return CRYPTO_ENHANCE_FAILURE;
    }
    random_delay();
    ret = memset_s(dest, dest_max, c, count);
    if (ret != EOK) {
        return CRYPTO_ENHANCE_FAILURE;
    }
    return CRYPTO_ENHANCE_SUCCESS;
}

int memcpy_se(void *dest, unsigned int dest_max, const void *src, unsigned int count)
{
    volatile int ret = CRYPTO_ENHANCE_FAILURE;

    random_delay();
    ret = memcpy_s(dest, dest_max, src, count);
    if (ret != EOK) {
        return CRYPTO_ENHANCE_FAILURE;
    }
    random_delay();
    ret = memcmp_se(dest, src, count);
    if (ret != CRYPTO_ENHANCE_SUCCESS) {
        return CRYPTO_ENHANCE_FAILURE;
    }
    return CRYPTO_ENHANCE_SUCCESS;
}