/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: hash kernel API function implementation.
 *
 * Create: 2023-05-26
 * Update: 2024-12-31.
 *     Optimize by using hash API2.0.
*/

#include "drv_hash.h"
#include "crypto_hash_common.h"

#define FILE_ID FILE_ID_DRV_CODE_HASH_DRV_HKDF

#define HASH_COMPAT_ERRNO(err_code)         (err_code)

int drv_cipher_hkdf_extract(crypto_hkdf_extract_t *extract_param, unsigned char *prk, unsigned int *prk_length)
{
    unsigned int input_block_length = 0;
    crypto_hash_type hash_type = 0;
    unsigned char local_salt[CRYPTO_HASH_BLOCK_SIZE_MAX] = { 0 };
    unsigned int local_salt_len = CRYPTO_HASH_BLOCK_SIZE_MAX;
    int ret = 0;
    unsigned int result_size;

    drv_err_chk_return(extract_param == NULL || extract_param->ikm == NULL, CRYPTO_FAILURE, "Param is NULL!\n");
    drv_err_chk_return(extract_param->salt_length != 0 && extract_param->salt == NULL,
        CRYPTO_FAILURE, "Param is NULL!\n");
    drv_err_chk_return(prk == NULL || prk_length == NULL, CRYPTO_FAILURE, "Param is NULL!\n");
    drv_err_chk_return(crypto_hash_common_type_check(extract_param->hmac_type) != CRYPTO_SUCCESS,
        HASH_COMPAT_ERRNO(ERROR_INVALID_PARAM), "invalid hmac_type");

    hash_type = crypto_hash_remove_hmac_flag(extract_param->hmac_type);
    input_block_length = crypto_hash_get_block_size(hash_type) / CRYPTO_BITS_IN_BYTE;
    result_size = crypto_hash_get_result_size(hash_type) / CRYPTO_BITS_IN_BYTE;
    /* default set to the HashLen, if extract_param->salt_length == 0, the salt is a string of HashLen zeros. */
    local_salt_len = result_size;

    if (extract_param->salt_length > input_block_length) {
        local_salt_len = result_size;
        ret = drv_cipher_hash_compute(hash_type, extract_param->salt, extract_param->salt_length,
            local_salt, local_salt_len);
        drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "drv_cipher_hash_compute failed\n");
    } else if (extract_param->salt_length != 0) {
        local_salt_len = extract_param->salt_length;
        ret = memcpy_s(local_salt, local_salt_len, extract_param->salt, extract_param->salt_length);
        drv_err_chk_return(ret != CRYPTO_SUCCESS, HASH_COMPAT_ERRNO(ERROR_MEMCPY_S), "memcpy_s failed\n");
    }

    *prk_length = result_size;
    ret = drv_cipher_hmac_compute(extract_param->hmac_type, local_salt, local_salt_len,
        CRYPTO_INVALID_KEYSLOT_HANDLE, extract_param->ikm, extract_param->ikm_length,
        prk, *prk_length);
    drv_err_chk_goto(ret != CRYPTO_SUCCESS, exit_clean, "drv_cipher_hmac_compute failed\n");

exit_clean:
    (void)memset_s(local_salt, sizeof(local_salt), 0, sizeof(local_salt));
    return ret;
}

int drv_cipher_hkdf_expand(const crypto_hkdf_expand_t *expand_param, unsigned char *okm, unsigned int okm_length)
{
    unsigned int block_num = 0;
    unsigned char output_block[64] = { 0 };  /* 64 is the max hash output length */
    unsigned int output_block_length = 0;
    unsigned char *buffer = NULL;
    unsigned int buffer_length = 0;
    unsigned int processing_len = 0;
    unsigned int last_len = 0;
    unsigned int i;
    int ret = 0;

    drv_err_chk_return(expand_param == NULL || expand_param->prk == NULL, HASH_COMPAT_ERRNO(ERROR_INVALID_PARAM),
        "Param is NULL!\n");
    drv_err_chk_return(expand_param->info_length != 0 && expand_param->info == NULL,
        HASH_COMPAT_ERRNO(ERROR_INVALID_PARAM), "Param is NULL!\n");
    drv_err_chk_return(okm == NULL, HASH_COMPAT_ERRNO(ERROR_INVALID_PARAM), "Param is NULL!\n");
    drv_err_chk_return(expand_param->info_length > HKDF_EXPAND_INFO_MAX_LENGTH,
        HASH_COMPAT_ERRNO(ERROR_INVALID_PARAM), "Param is NULL!\n");
    drv_err_chk_return(okm_length == 0, HASH_COMPAT_ERRNO(ERROR_INVALID_PARAM), "okm_length is invalid\n");
    drv_err_chk_return(crypto_hash_common_type_check(expand_param->hmac_type) != CRYPTO_SUCCESS,
        HASH_COMPAT_ERRNO(ERROR_INVALID_PARAM), "invalid hmac_type");

    output_block_length = crypto_hash_get_result_size(expand_param->hmac_type) / CRYPTO_BITS_IN_BYTE;
    /* According to RFC 5869 Section2.3, the length at least hash_length. */
    if (expand_param->prk_length < output_block_length) {

        drv_set_err(HASH_COMPAT_ERRNO(ERROR_INVALID_PARAM), "direct error return");
        return HASH_COMPAT_ERRNO(ERROR_INVALID_PARAM);
    }
    /* According to RFC 5869 Section2.3, the length must <= 255 * hash_length. */
    if (okm_length > 255 * output_block_length) {   // 255: refer to comment above

        drv_set_err(HASH_COMPAT_ERRNO(ERROR_INVALID_PARAM), "direct error return");
        return HASH_COMPAT_ERRNO(ERROR_INVALID_PARAM);
    }

    buffer_length = output_block_length + expand_param->info_length + 1;
    buffer = crypto_malloc_coherent(buffer_length);
    if (buffer == NULL) {

        drv_set_err(CRYPTO_FAILURE, "direct error return");
        return CRYPTO_FAILURE;
    }

    block_num = okm_length / output_block_length + ((okm_length % output_block_length) == 0 ? 0 : 1);
    for (i = 0; i < block_num; i++) {
        if (i == 0) {
            output_block_length = 0;
        } else {
            ret = memcpy_s(buffer, buffer_length, output_block, output_block_length);
            drv_err_chk_goto(ret != EOK, okm_exit, "memcpy_s failed\n");
        }

        if (expand_param->info != NULL) {
            ret = memcpy_s(buffer + output_block_length, buffer_length - output_block_length,
                expand_param->info, expand_param->info_length);
            drv_err_chk_goto(ret != EOK, okm_exit, "memcpy_s failed\n");
        }
        buffer[output_block_length + expand_param->info_length] = i + 1;

        processing_len = output_block_length + expand_param->info_length + 1;
        output_block_length = crypto_hash_get_result_size(expand_param->hmac_type) / CRYPTO_BITS_IN_BYTE;

        ret = drv_cipher_hmac_compute(expand_param->hmac_type, expand_param->prk, expand_param->prk_length,
            CRYPTO_INVALID_KEYSLOT_HANDLE, buffer, processing_len,
            output_block, output_block_length);
        drv_err_chk_goto(ret != CRYPTO_SUCCESS, okm_exit, "drv_cipher_hmac_compute failed\n");

        if (i != block_num - 1) {
            ret = memcpy_s(okm + i * output_block_length, okm_length - i * output_block_length,
                output_block, output_block_length);
            drv_err_chk_goto_with_ret(ret, ret != EOK, okm_exit,
                HASH_COMPAT_ERRNO(ERROR_MEMCPY_S), "memcpy_s failed\n");
        }
    }

    last_len = (okm_length % output_block_length) == 0 ? output_block_length : (okm_length % output_block_length);
    ret = memcpy_s(okm + (i - 1) * output_block_length, okm_length - (i - 1) * output_block_length,
        output_block, last_len);
    drv_err_chk_goto_with_ret(ret, ret != EOK, okm_exit, HASH_COMPAT_ERRNO(ERROR_MEMCPY_S), "memcpy_s failed\n");

    goto expand_exit;

okm_exit:
    memset_s(okm, okm_length, 0x00, okm_length);
expand_exit:
    memset_s(output_block, sizeof(output_block), 0x00, sizeof(output_block));
    memset_s(buffer, buffer_length, 0x00, buffer_length);
    crypto_free_coherent(buffer);
    return ret;
}

int drv_cipher_hkdf(crypto_hkdf_t *hkdf_param, unsigned char *okm, unsigned int okm_length)
{
    unsigned char prk[64] = { 0 };  /* 64 is the max hash result length */
    unsigned int prk_length = 64;   /* 64 is the max hash result length */
    crypto_hkdf_extract_t hkdf_extract = { 0 };
    crypto_hkdf_expand_t hkdf_expand = { 0 };
    int ret = 0;

    drv_err_chk_return(hkdf_param == NULL, CRYPTO_FAILURE, "Param is NULL!\n");

    hkdf_extract.hmac_type = hkdf_param->hmac_type;
    hkdf_extract.ikm = hkdf_param->ikm;
    hkdf_extract.ikm_length = hkdf_param->ikm_length;
    hkdf_extract.salt = hkdf_param->salt;
    hkdf_extract.salt_length = hkdf_param->salt_length;
    ret = drv_cipher_hkdf_extract(&hkdf_extract, prk, &prk_length);
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "drv_cipher_hkdf_extract failed\n");

    hkdf_expand.hmac_type = hkdf_param->hmac_type;
    hkdf_expand.info = hkdf_param->info;
    hkdf_expand.info_length = hkdf_param->info_length;
    hkdf_expand.prk = prk;
    hkdf_expand.prk_length = prk_length;
    ret = drv_cipher_hkdf_expand(&hkdf_expand, okm, okm_length);

    (void)memset_s(prk, sizeof(prk), 0x00, sizeof(prk));

    return ret;
}