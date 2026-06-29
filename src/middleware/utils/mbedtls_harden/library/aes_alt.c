/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: mbedtls alt aes implemention.
 *
*/

#include "library/common.h"

#if defined(MBEDTLS_AES_C) && defined(MBEDTLS_AES_ALT)

#include <string.h>

#include "mbedtls/aes.h"
#include "mbedtls/platform.h"
#include "mbedtls/platform_util.h"
#include "mbedtls/error.h"
#include "security_symc_pri.h"
#include "securec.h"

#include "mbedtls_perf_utils.h"
#ifndef BITS_OF_BYTE
#define BITS_OF_BYTE 8
#endif
#ifndef BLOCK_SIZE_IN_BYTE
#define BLOCK_SIZE_IN_BYTE  16
#endif

#define MBEDTLS_128_KEY_LEN  16
void mbedtls_aes_init(mbedtls_aes_context *ctx)
{
    MBEDTLS_PERF_START();
    if (ctx == NULL) {
        return;
    }
    (void)memset_s(ctx, sizeof(mbedtls_aes_context), 0, sizeof(mbedtls_aes_context));
    uapi_drv_cipher_symc_setup(&ctx->symc_handle, UAPI_DRV_SYMC_AES_CBC_ALG, NULL, MBEDTLS_128_KEY_LEN, 0, true);
    MBEDTLS_PERF_AES_END1(MBEDTLS_PERF_AES_TYPE_INIT);
}

void mbedtls_aes_free(mbedtls_aes_context *ctx)
{
    MBEDTLS_PERF_START();
    if (ctx == NULL) {
        return;
    }
    (void)uapi_drv_cipher_symc_teardown(ctx->symc_handle);
    mbedtls_platform_zeroize(ctx, sizeof(mbedtls_aes_context));
    MBEDTLS_PERF_AES_END1(MBEDTLS_PERF_AES_TYPE_FREE);
}

static int mbedtls_inner_aes_setkey(mbedtls_aes_context* ctx, const unsigned char *key,
                                    unsigned int keybits, bool is_encrypt)
{
    errcode_t ret;
    uint32_t key_len;
    if (keybits != 128 && keybits != 192 && keybits != 256) { // 128:128bits, 192:192bits, 256:256bits
        return MBEDTLS_ERR_AES_INVALID_KEY_LENGTH;
    }
    if (ctx == NULL || key == NULL) {
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }
    key_len = keybits / BITS_OF_BYTE;
    
    ret = uapi_drv_cipher_symc_set_key(ctx->symc_handle, key, key_len, UAPI_DRV_INVALID_KEY_SLOT);
    if (ret != ERRCODE_SUCC) {
        mbedtls_printf("mbedtls_aes_setkey_enc failed, ret=0x%x\n", ret);
        return MBEDTLS_ERR_ERROR_GENERIC_ERROR;
    }

    ret = uapi_drv_cipher_symc_set_crypt_mode(ctx->symc_handle, is_encrypt);
    if (ret != ERRCODE_SUCC) {
        mbedtls_printf("mbedtls_aes_setkey_enc failed, ret=0x%x\n", ret);
        return MBEDTLS_ERR_ERROR_GENERIC_ERROR;
    }

    return 0;
}

int mbedtls_aes_setkey_enc(mbedtls_aes_context* ctx, const unsigned char *key,
                           unsigned int keybits)
{
    int ret;
    MBEDTLS_PERF_START();
    ret = mbedtls_inner_aes_setkey(ctx, key, keybits, true);
    MBEDTLS_PERF_AES_END1(MBEDTLS_PERF_AES_TYPE_SETKEY_ENC);
    return ret;
}

#if !defined(MBEDTLS_BLOCK_CIPHER_NO_DECRYPT)

int mbedtls_aes_setkey_dec(mbedtls_aes_context *ctx, const unsigned char *key,
                           unsigned int keybits)
{
    int ret;
    MBEDTLS_PERF_START();
    ret = mbedtls_inner_aes_setkey(ctx, key, keybits, false);
    MBEDTLS_PERF_AES_END1(MBEDTLS_PERF_AES_TYPE_SETKEY_DEC);
    return ret;
}

#endif /* !MBEDTLS_BLOCK_CIPHER_NO_DECRYPT */

int mbedtls_aes_crypt_ecb(mbedtls_aes_context *ctx,
                          int mode,
                          const unsigned char input[16],
                          unsigned char output[16])
{
    uint32_t alg = UAPI_DRV_SYMC_AES_ECB_ALG;
    errcode_t ret;
    MBEDTLS_PERF_START();

    if (ctx == NULL) {
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }

    if (mode != MBEDTLS_AES_ENCRYPT && mode != MBEDTLS_AES_DECRYPT) {
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }

    if (mode == MBEDTLS_AES_ENCRYPT) {
        ret = uapi_drv_cipher_symc_set_crypt_mode(ctx->symc_handle, true);
    } else {
        ret = uapi_drv_cipher_symc_set_crypt_mode(ctx->symc_handle, false);
    }
    if (ret != ERRCODE_SUCC) {
        mbedtls_printf("mbedtls_aes_crypt_ecb failed, ret=0x%x\n", ret);
        return MBEDTLS_ERR_ERROR_GENERIC_ERROR;
    }

    ret = uapi_drv_cipher_symc_set_alg(ctx->symc_handle, alg);
    if (ret != ERRCODE_SUCC) {
        mbedtls_printf("mbedtls_aes_crypt_ecb failed, ret=0x%x\n", ret);
        return MBEDTLS_ERR_ERROR_GENERIC_ERROR;
    }

    ret = uapi_drv_cipher_symc_ecb_update(ctx->symc_handle, input, output, 16); // 16:input size = 16
    if (ret != ERRCODE_SUCC) {
        mbedtls_printf("mbedtls_aes_crypt_ecb failed, ret=0x%x\n", ret);
        return MBEDTLS_ERR_ERROR_GENERIC_ERROR;
    }

    MBEDTLS_PERF_AES_END2(16);  // 16:input size = 16
    return 0;
}

int mbedtls_internal_aes_encrypt(mbedtls_aes_context *ctx, const unsigned char input[16], unsigned char output[16])
{
    return mbedtls_aes_crypt_ecb(ctx, MBEDTLS_AES_ENCRYPT, input, output);
}
 
int mbedtls_internal_aes_decrypt(mbedtls_aes_context *ctx, const unsigned char input[16], unsigned char output[16])
{
    return mbedtls_aes_crypt_ecb(ctx, MBEDTLS_AES_DECRYPT, input, output);
}

#if defined(MBEDTLS_CIPHER_MODE_CBC)

int mbedtls_aes_crypt_cbc(mbedtls_aes_context *ctx,
                          int mode,
                          size_t length,
                          unsigned char iv[16],
                          const unsigned char *input,
                          unsigned char *output)
{
    uint32_t alg = UAPI_DRV_SYMC_AES_CBC_ALG;
    errcode_t ret;
    MBEDTLS_PERF_START();

    if (ctx == NULL || input == NULL || output == NULL) {
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }

    if (mode != MBEDTLS_AES_ENCRYPT && mode != MBEDTLS_AES_DECRYPT) {
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }

    if (mode == MBEDTLS_AES_ENCRYPT) {
        ret = uapi_drv_cipher_symc_set_crypt_mode(ctx->symc_handle, true);
    } else {
        ret = uapi_drv_cipher_symc_set_crypt_mode(ctx->symc_handle, false);
    }
    if (ret != ERRCODE_SUCC) {
        mbedtls_printf("mbedtls_aes_crypt_cbc failed, ret=0x%x\n", ret);
        return MBEDTLS_ERR_ERROR_GENERIC_ERROR;
    }

    ret = uapi_drv_cipher_symc_set_alg(ctx->symc_handle, alg);
    if (ret != ERRCODE_SUCC) {
        mbedtls_printf("mbedtls_aes_crypt_cbc failed, ret=0x%x\n", ret);
        return MBEDTLS_ERR_ERROR_GENERIC_ERROR;
    }

    ret = uapi_drv_cipher_symc_cbc_update(ctx->symc_handle, input, output, length, iv);
    if (ret != ERRCODE_SUCC) {
        mbedtls_printf("mbedtls_aes_crypt_cbc failed, ret=0x%x\n", ret);
        return MBEDTLS_ERR_ERROR_GENERIC_ERROR;
    }

    MBEDTLS_PERF_AES_END2(length);
    return 0;
}

#endif /* MBEDTLS_CIPHER_MODE_CBC */

#if defined(MBEDTLS_CIPHER_MODE_XTS)

void mbedtls_aes_xts_init(mbedtls_aes_xts_context *ctx)
{
    mbedtls_aes_init(&ctx->crypt);
    mbedtls_aes_init(&ctx->tweak);
}

void mbedtls_aes_xts_free(mbedtls_aes_xts_context *ctx)
{
    mbedtls_aes_free(&ctx->crypt);
    mbedtls_aes_free(&ctx->tweak);
}

static inline void inner_gf128mul_x_ble(unsigned char r[16], const unsigned char x[16])
{
    uint64_t a, b, ra, rb;

    a = MBEDTLS_GET_UINT64_LE(x, 0);
    b = MBEDTLS_GET_UINT64_LE(x, 8);    // 8: byte len

    ra = (a << 1)  ^ 0x0087 >> (8 - ((b >> 63) << 3));  // 0x0087, 8, 63, 3: algo require
    rb = (a >> 63) | (b << 1);  // 63: algo require

    MBEDTLS_PUT_UINT64_LE(ra, r, 0);
    MBEDTLS_PUT_UINT64_LE(rb, r, 8);    // 8: byte len
}

static int inner_xts_process_tail(mbedtls_aes_xts_context *ctx, int mode,
    unsigned char *t, const unsigned char *input, unsigned char *output, size_t tail_len)
{
    int ret;
    unsigned int i;
    unsigned char tmp[BLOCK_SIZE_IN_BYTE];
    unsigned char *prev_output = output - BLOCK_SIZE_IN_BYTE;

    if (tail_len == 0) {
        return 0;
    }

    for (i = 0; i < tail_len; i++) {
        output[i] = prev_output[i];
    }
    mbedtls_xor(tmp, input, t, tail_len);
    mbedtls_xor(tmp + i, prev_output + i, t + i, BLOCK_SIZE_IN_BYTE - i);
    ret = mbedtls_aes_crypt_ecb(&ctx->crypt, mode, tmp, tmp);
    if (ret != 0) {
        return ret;
    }
    mbedtls_xor(prev_output, tmp, t, BLOCK_SIZE_IN_BYTE);
    return 0;
}

int mbedtls_aes_xts_setkey_enc(mbedtls_aes_xts_context *ctx,
    const unsigned char *key, unsigned int keybits)
{
    int ret;
    unsigned int key_len;
    if (keybits == 256) {   // 256: 256bits
        key_len = 16;   // 16: aes-128
    } else if (keybits == 512) {    // 512: 512bits
        key_len = 32;   // 32: aes-256
    } else {
        return MBEDTLS_ERR_AES_INVALID_KEY_LENGTH;
    }

    ret = mbedtls_aes_setkey_enc(&ctx->crypt, key, keybits / 2);    // 2: half
    if (ret != 0) {
        return ret;
    }

    return mbedtls_aes_setkey_enc(&ctx->tweak, key + key_len, keybits / 2); // 2: half
}

int mbedtls_aes_xts_setkey_dec(mbedtls_aes_xts_context *ctx,
    const unsigned char *key, unsigned int keybits)
{
    return mbedtls_aes_xts_setkey_enc(ctx, key, keybits);
}

int mbedtls_aes_crypt_xts(mbedtls_aes_xts_context *ctx, int mode,
    size_t length, const unsigned char data_unit[16],
    const unsigned char *input, unsigned char *output)
{
    int ret;
    size_t block_num = length / BLOCK_SIZE_IN_BYTE;
    size_t tail_len = length % BLOCK_SIZE_IN_BYTE;
    unsigned char curr_tweak[BLOCK_SIZE_IN_BYTE];
    unsigned char last_tweak[BLOCK_SIZE_IN_BYTE];
    unsigned char tmp[BLOCK_SIZE_IN_BYTE];
    unsigned int i;
    unsigned char *t = mode == MBEDTLS_AES_DECRYPT ? last_tweak : curr_tweak;

    if (mode != MBEDTLS_AES_ENCRYPT && mode != MBEDTLS_AES_DECRYPT) {
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }
    if (block_num == 0) {
        return MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH;
    }
    if (length > (1 << 20) * BLOCK_SIZE_IN_BYTE) {  // 20: max block size
        return MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH;
    }

    /* Compute the tweak. */
    ret = mbedtls_aes_crypt_ecb(&ctx->tweak, MBEDTLS_AES_ENCRYPT, data_unit, curr_tweak);
    if (ret != 0) {
        return ret;
    }

    for (i = 0; i < block_num; i++) {
        if ((i == block_num - 1) && (mode == MBEDTLS_AES_DECRYPT) && (tail_len != 0)) {
            (void)memcpy_s(last_tweak, sizeof(last_tweak), curr_tweak, sizeof(curr_tweak));
            inner_gf128mul_x_ble(curr_tweak, curr_tweak);
        }
        mbedtls_xor(tmp, input + i * BLOCK_SIZE_IN_BYTE, curr_tweak, BLOCK_SIZE_IN_BYTE);
        ret = mbedtls_aes_crypt_ecb(&ctx->crypt, mode, tmp, tmp);
        if (ret != 0) {
            return ret;
        }
        mbedtls_xor(output + i * BLOCK_SIZE_IN_BYTE, tmp, curr_tweak, BLOCK_SIZE_IN_BYTE);
        /* Update the tweak for the next block. */
        inner_gf128mul_x_ble(curr_tweak, curr_tweak);
    }

    return inner_xts_process_tail(ctx, mode, t, input + block_num * BLOCK_SIZE_IN_BYTE,
        output + block_num * BLOCK_SIZE_IN_BYTE, tail_len);
}

#endif /* MBEDTLS_CIPHER_MODE_XTS */

#if defined(MBEDTLS_CIPHER_MODE_CFB)

int mbedtls_aes_crypt_cfb128(mbedtls_aes_context *ctx,
                             int mode,
                             size_t length,
                             size_t *iv_off,
                             unsigned char iv[16],
                             const unsigned char *input,
                             unsigned char *output)
{
    uint32_t alg = UAPI_DRV_SYMC_AES_CFB128_ALG;
    errcode_t ret;
    MBEDTLS_PERF_START();

    if (ctx == NULL || iv_off == NULL || input == NULL || output == NULL) {
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }

    if (mode != MBEDTLS_AES_ENCRYPT && mode != MBEDTLS_AES_DECRYPT) {
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }

    if (mode == MBEDTLS_AES_ENCRYPT) {
        ret = uapi_drv_cipher_symc_set_crypt_mode(ctx->symc_handle, true);
    } else {
        ret = uapi_drv_cipher_symc_set_crypt_mode(ctx->symc_handle, false);
    }
    if (ret != ERRCODE_SUCC) {
        mbedtls_printf("mbedtls_aes_crypt_cfb128 failed, ret=0x%x\n", ret);
        return MBEDTLS_ERR_ERROR_GENERIC_ERROR;
    }

    ret = uapi_drv_cipher_symc_set_alg(ctx->symc_handle, alg);
    if (ret != ERRCODE_SUCC) {
        mbedtls_printf("mbedtls_aes_crypt_cfb128 failed, ret=0x%x\n", ret);
        return MBEDTLS_ERR_ERROR_GENERIC_ERROR;
    }

    ret = uapi_drv_cipher_symc_cfb128_update(ctx->symc_handle, input, output, length, iv, (uint8_t *)iv_off);
    if (ret != ERRCODE_SUCC) {
        mbedtls_printf("mbedtls_aes_crypt_cfb128 failed, ret=0x%x\n", ret);
        return MBEDTLS_ERR_ERROR_GENERIC_ERROR;
    }

    MBEDTLS_PERF_AES_END2(length);
    return 0;
}

int mbedtls_aes_crypt_cfb8(mbedtls_aes_context *ctx,
                           int mode,
                           size_t length,
                           unsigned char iv[16],
                           const unsigned char *input,
                           unsigned char *output)
{
    uint32_t alg = UAPI_DRV_SYMC_AES_CFB8_ALG;
    errcode_t ret;
    uint8_t iv_off = 0;
    MBEDTLS_PERF_START();

    if (ctx == NULL || input == NULL || output == NULL) {
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }

    if (mode != MBEDTLS_AES_ENCRYPT && mode != MBEDTLS_AES_DECRYPT) {
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }

    if (mode == MBEDTLS_AES_ENCRYPT) {
        ret = uapi_drv_cipher_symc_set_crypt_mode(ctx->symc_handle, true);
    } else {
        ret = uapi_drv_cipher_symc_set_crypt_mode(ctx->symc_handle, false);
    }
    if (ret != ERRCODE_SUCC) {
        mbedtls_printf("mbedtls_aes_crypt_cfb8 failed, ret=0x%x\n", ret);
        return MBEDTLS_ERR_ERROR_GENERIC_ERROR;
    }

    ret = uapi_drv_cipher_symc_set_alg(ctx->symc_handle, alg);
    if (ret != ERRCODE_SUCC) {
        mbedtls_printf("mbedtls_aes_crypt_cfb8 failed, ret=0x%x\n", ret);
        return MBEDTLS_ERR_ERROR_GENERIC_ERROR;
    }

    ret = uapi_drv_cipher_symc_cfb8_update(ctx->symc_handle, input, output, length, iv, &iv_off);
    if (ret != ERRCODE_SUCC) {
        mbedtls_printf("mbedtls_aes_crypt_cfb8 failed, ret=0x%x\n", ret);
        return MBEDTLS_ERR_ERROR_GENERIC_ERROR;
    }

    MBEDTLS_PERF_AES_END2(length);
    return 0;
}

#endif /* MBEDTLS_CIPHER_MODE_CFB */

#if defined(MBEDTLS_CIPHER_MODE_OFB)
int mbedtls_aes_crypt_ofb(mbedtls_aes_context *ctx,
                          size_t length,
                          size_t *iv_off,
                          unsigned char iv[16],
                          const unsigned char *input,
                          unsigned char *output)
{
    errcode_t ret;
    uint32_t alg = UAPI_DRV_SYMC_AES_OFB_ALG;
    MBEDTLS_PERF_START();

    if (ctx == NULL || input == NULL || output == NULL || iv_off == NULL) {
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }

    ret = uapi_drv_cipher_symc_set_alg(ctx->symc_handle, alg);
    if (ret != ERRCODE_SUCC) {
        mbedtls_printf("mbedtls_aes_crypt_ofb failed, ret=0x%x\n", ret);
        return MBEDTLS_ERR_ERROR_GENERIC_ERROR;
    }

    ret = uapi_drv_cipher_symc_ofb_update(ctx->symc_handle, input, output, length, iv, (uint8_t *)iv_off);
    if (ret != ERRCODE_SUCC) {
        mbedtls_printf("mbedtls_aes_crypt_ofb failed, ret=0x%x\n", ret);
        return MBEDTLS_ERR_ERROR_GENERIC_ERROR;
    }

    MBEDTLS_PERF_AES_END2(length);
    return 0;
}


#endif /* MBEDTLS_CIPHER_MODE_OFB */

#if defined(MBEDTLS_CIPHER_MODE_CTR)

int mbedtls_aes_crypt_ctr(mbedtls_aes_context *ctx,
                          size_t length,
                          size_t *nc_off,
                          unsigned char nonce_counter[16],
                          unsigned char stream_block[16],
                          const unsigned char *input,
                          unsigned char *output)
{
    errcode_t ret;
    uint32_t alg = UAPI_DRV_SYMC_AES_CTR_ALG;
    MBEDTLS_PERF_START();

    if (ctx == NULL || input == NULL || output == NULL || nc_off == NULL) {
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }

    ret = uapi_drv_cipher_symc_set_alg(ctx->symc_handle, alg);
    if (ret != ERRCODE_SUCC) {
        mbedtls_printf("mbedtls_aes_crypt_ctr failed, ret=0x%x\n", ret);
        return MBEDTLS_ERR_ERROR_GENERIC_ERROR;
    }

    ret = uapi_drv_cipher_symc_ctr_update(ctx->symc_handle, input, output,
                                          length, nonce_counter, (uint8_t *)nc_off, stream_block);
    if (ret != ERRCODE_SUCC) {
        mbedtls_printf("mbedtls_aes_crypt_ctr failed, ret=0x%x\n", ret);
        return MBEDTLS_ERR_ERROR_GENERIC_ERROR;
    }
    MBEDTLS_PERF_AES_END2(length);
    return 0;
}

#endif /* MBEDTLS_CIPHER_MODE_CTR */

#endif
