/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: drv pbkdf2 soft compute. \n
 *
 * History: \n
 * 2025-08-06, Create file. \n
 */

#include "drv_hash.h"
#include "drv_keyslot.h"
#include "crypto_hash_common.h"

#define FILE_ID FILE_ID_DRV_CODE_HASH_DRV_PBKDF2_SOFT

int32_t inner_pbkdf2_param_chk(const crypto_kdf_pbkdf2_ex_param *param, const uint8_t *out, uint32_t out_len)
{
    crypto_hash_type type;

    drv_err_chk_return(param == TD_NULL, ERROR_PARAM_IS_NULL, "param is NULL\n");
    drv_err_chk_return(out == TD_NULL, ERROR_PARAM_IS_NULL, "out is NULL\n");

    if (param->plen != 0) {
        drv_err_chk_return(param->password == TD_NULL, ERROR_PARAM_IS_NULL, "password is NULL\n");
    }
    if (param->slen != 0) {
        drv_err_chk_return(param->salt == TD_NULL, ERROR_PARAM_IS_NULL, "salt is NULL\n");
    }
    drv_err_chk_return(param->count == 0, ERROR_INVALID_PARAM, "count is invalid\n");

    drv_err_chk_return(out_len == 0 || out_len >= CRYPTO_PBKDF2_OUT_MAX_LENGTH,
        ERROR_INVALID_PARAM, "out_len is invalid\n");

    type = param->hash_type;
    drv_err_chk_return(type != CRYPTO_HASH_TYPE_HMAC_SHA1 && type != CRYPTO_HASH_TYPE_HMAC_SHA224 &&
        type != CRYPTO_HASH_TYPE_HMAC_SHA256 && type != CRYPTO_HASH_TYPE_HMAC_SHA384 &&
        type != CRYPTO_HASH_TYPE_HMAC_SHA512 && type != CRYPTO_HASH_TYPE_HMAC_SM3,
        ERROR_INVALID_PARAM, "hash_type is invalid!\n");

    return CRYPTO_SUCCESS;
}

static inline void inner_pbkdf2_word_xor(const uint32_t *a, uint32_t *b, uint32_t len)
{
    uint32_t i;
    for (i = 0; i < len / CRYPTO_WORD_WIDTH; i++) {
        b[i] ^= a[i];
    }
}

#define MAX_PBKDF2_ITERATION_TIMES      65535
static int32_t inner_pbkdf2_update(void *pbkdf2_ctx, uint32_t count, bool is_hardware_support)
{
    int32_t ret;
    uint32_t left = count;
    uint32_t real_count;

    (void)ret;
    (void)left;
    (void)real_count;

    if (is_hardware_support) {
#if defined(CONFIG_PBKDF2_V2)
        /* Case 1. Hardware Version2 */
        while (left > 0) {
            real_count = crypto_min(left, MAX_PBKDF2_ITERATION_TIMES);

            ret = drv_cipher_pbkdf2_update_hardware(pbkdf2_ctx, real_count);
            drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "drv_cipher_pbkdf2_update_soft failed\n");

            left -= real_count;
        }
        return CRYPTO_SUCCESS;
#elif defined(CONFIG_PBKDF2_V1)
        /* Case 2. Hardware Version1 */
        if (count > MAX_PBKDF2_ITERATION_TIMES) {
            ret = drv_cipher_pbkdf2_update_soft(pbkdf2_ctx, count - MAX_PBKDF2_ITERATION_TIMES);
            drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "drv_cipher_pbkdf2_update_soft failed\n");

            left = MAX_PBKDF2_ITERATION_TIMES;
        }

        ret = drv_cipher_pbkdf2_update_hardware(pbkdf2_ctx, left);
        drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "drv_cipher_pbkdf2_update_soft failed\n");
        return CRYPTO_SUCCESS;
#endif
    }

    /* Case 3. Hardware unsupport. */
    return drv_cipher_pbkdf2_update_soft(pbkdf2_ctx, count);
}

static int32_t inner_pbkdf2_process(const crypto_kdf_pbkdf2_ex_param *param, uint8_t *out, const uint32_t out_len,
    void *pbkdf2_ctx)
{
    int32_t ret;
    uint32_t i = 0;
    uint32_t hmac_len = crypto_hash_get_result_size(param->hash_type) / CRYPTO_BITS_IN_BYTE;
    uint32_t left = out_len;
    uint32_t real_out_len;
    bool is_hardware_support = false;   /* default is false. */

#if defined(CONFIG_PBKDF2_V1) || defined(CONFIG_PBKDF2_V2)
/* hardware support sha1. */
#if defined(CONFIG_HASH_SUPPORT_SHA1)
    if (param->hash_type == CRYPTO_HASH_TYPE_HMAC_SHA1) {
        is_hardware_support = true;
    }
#endif
/* hardware support sha256. */
#if defined(CONFIG_HASH_SUPPORT_SHA256)
    if (param->hash_type == CRYPTO_HASH_TYPE_HMAC_SHA256) {
        is_hardware_support = true;
    }
#endif
/* hardware support sha512. */
#if defined(CONFIG_HASH_SUPPORT_SHA512)
    if (param->hash_type == CRYPTO_HASH_TYPE_HMAC_SHA384 ||
        param->hash_type == CRYPTO_HASH_TYPE_HMAC_SHA512) {
        is_hardware_support = true;
    }
#endif
/* hardware support sm3. */
#if defined(CONFIG_HASH_SUPPORT_SM3)
    if (param->hash_type == CRYPTO_HASH_TYPE_HMAC_SM3) {
        is_hardware_support = true;
    }
#endif
#endif

    while (left > 0) {
        real_out_len = crypto_min(left, hmac_len);

        ret = drv_cipher_pbkdf2_set_salt(pbkdf2_ctx, param->salt, param->slen, i + 1);
        drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "drv_cipher_pbkdf2_set_salt failed\n");

        ret = inner_pbkdf2_update(pbkdf2_ctx, param->count, is_hardware_support);
        drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "inner_pbkdf2_update failed\n");

        ret = drv_cipher_pbkdf2_finish(pbkdf2_ctx, out + i * hmac_len, real_out_len);
        drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "drv_cipher_pbkdf2_finish failed\n");

        left -= real_out_len;
        i++;
    }

    return CRYPTO_SUCCESS;
}

int32_t drv_cipher_pbkdf2(const crypto_kdf_pbkdf2_param *param, uint8_t *out, const uint32_t out_len)
{
    crypto_kdf_pbkdf2_ex_param param_ex = { 0 };

    drv_err_chk_return(param == TD_NULL, ERROR_PARAM_IS_NULL, "param is NULL\n");

    param_ex.hash_type = param->hash_type;
    param_ex.password = param->password;
    param_ex.plen = param->plen;
    param_ex.salt = param->salt;
    param_ex.slen = param->slen;
    param_ex.count = param->count;
    return drv_cipher_pbkdf2_ex(&param_ex, out, out_len);
}

int32_t drv_cipher_pbkdf2_ex(const crypto_kdf_pbkdf2_ex_param *param, uint8_t *out, const uint32_t out_len)
{
    int32_t ret;
    void *pbkdf2_ctx = NULL;
    crypto_drv_func_enter();

    ret = inner_pbkdf2_param_chk(param, out, out_len);
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "inner_pbkdf2_param_chk failed\n");

    ret = drv_cipher_pbkdf2_setup(&pbkdf2_ctx, param->hash_type,
        param->password, param->plen, CRYPTO_INVALID_KEYSLOT_HANDLE);
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "drv_cipher_pbkdf2_setup failed\n");

    ret = inner_pbkdf2_process(param, out, out_len, pbkdf2_ctx);
    drv_err_chk_goto(ret != CRYPTO_SUCCESS, exit_pbkdf2_teardown, "inner_pbkdf2_process failed\n");

exit_pbkdf2_teardown:
    drv_cipher_pbkdf2_teardown(pbkdf2_ctx);
    crypto_drv_func_exit();
    return ret;
}

int32_t drv_cipher_pbkdf2_setup(void **ctx, crypto_hash_type hash_type,
    const uint8_t *password, uint32_t plen, uint32_t keyslot_handle)
{
    int32_t ret;
    drv_pbkdf2_ctx_t *pbkdf2_ctx = NULL;
    crypto_klad_engine klad_engine = CRYPTO_KLAD_ENGINE_INVALID;

    if (crypto_hash_remove_hmac_flag(hash_type) == CRYPTO_HASH_TYPE_SHA1) {
#if defined(CONFIG_HASH_SUPPORT_SHA1)
        klad_engine = CRYPTO_KLAD_ENGINE_SHA1_HMAC;
#endif
    } else if (crypto_hash_remove_hmac_flag(hash_type) == CRYPTO_HASH_TYPE_SM3) {
#if defined(CONFIG_HASH_SUPPORT_SM3)
        klad_engine = CRYPTO_KLAD_ENGINE_SM3_HMAC;
#endif
    } else {
#if defined(CONFIG_HASH_SUPPORT_SHA256)
        klad_engine = CRYPTO_KLAD_ENGINE_SHA2_HMAC;
#endif
    }

    pbkdf2_ctx = crypto_malloc_coherent(sizeof(drv_pbkdf2_ctx_t));
    drv_err_chk_return(pbkdf2_ctx == NULL, ERROR_MALLOC, "malloc failed\n");
    (void)memset_s(pbkdf2_ctx, sizeof(drv_pbkdf2_ctx_t), 0, sizeof(drv_pbkdf2_ctx_t));

    pbkdf2_ctx->block_size = crypto_hash_get_block_size(hash_type) / CRYPTO_BITS_IN_BYTE;
    pbkdf2_ctx->result_size = crypto_hash_get_result_size(hash_type) / CRYPTO_BITS_IN_BYTE;
    /*
     * if plen <= block_size:
     *    local_password = password
     * else:
     *    local_password = hash(password)
     */
    if (plen <= pbkdf2_ctx->block_size) {
        ret = memcpy_s(pbkdf2_ctx->password, sizeof(pbkdf2_ctx->password), password, plen);
        drv_err_chk_goto_with_ret(ret, ret != EOK, error_free, ERROR_MEMCPY_S, "memcpy_s failed\n");
    } else {
        ret = drv_cipher_hash_compute(crypto_hash_remove_hmac_flag(hash_type), password, plen,
            (uint8_t *)pbkdf2_ctx->password, pbkdf2_ctx->result_size);
        drv_err_chk_goto(ret != CRYPTO_SUCCESS, error_free, "hash_compute failed\n");
    }

    if ((keyslot_handle == CRYPTO_INVALID_KEYSLOT_HANDLE) && (klad_engine != CRYPTO_KLAD_ENGINE_INVALID)) {
        ret = drv_keyslot_setup(&pbkdf2_ctx->keyslot_handle, CRYPTO_KEYSLOT_TYPE_HMAC);
        drv_err_chk_goto(ret != CRYPTO_SUCCESS, error_free, "drv_keyslot_setup failed\n");
        pbkdf2_ctx->is_local_keyslot = true;

        ret = drv_keyslot_set_clear_key(pbkdf2_ctx->keyslot_handle, klad_engine,
            (uint8_t *)pbkdf2_ctx->password, pbkdf2_ctx->block_size);
        drv_err_chk_goto(ret != CRYPTO_SUCCESS, error_keyslot_teardown, "drv_keyslot_set_clear_key failed\n");
    } else {
        pbkdf2_ctx->keyslot_handle = keyslot_handle;
    }

    pbkdf2_ctx->hash_type = hash_type;
    *ctx = pbkdf2_ctx;
    return CRYPTO_SUCCESS;
error_keyslot_teardown:
    (void)drv_keyslot_teardown(pbkdf2_ctx->keyslot_handle);
error_free:
    (void)memset_s(pbkdf2_ctx, sizeof(drv_pbkdf2_ctx_t), 0, sizeof(drv_pbkdf2_ctx_t));
    crypto_free_coherent(pbkdf2_ctx);
    return ret;
}

#define KDF_KEY_BLOCK_SIZE_512      64
#define KDF_KEY_BLOCK_SIZE_1024     128

int32_t drv_cipher_pbkdf2_set_salt(void *ctx, const uint8_t *salt, uint32_t slen, uint32_t count)
{
    int32_t ret;
    uint8_t *data_pad = NULL;
    drv_pbkdf2_ctx_t *pbkdf2_ctx = (drv_pbkdf2_ctx_t *)ctx;
    bool is_salt_overflow = false;
    void *hmac_ctx = NULL;
    uint32_t count_be32 = crypto_cpu_to_be32(count);

    /* set salt mean start pbkdf2 compute. */
    (void)memset_s(pbkdf2_ctx->result, sizeof(pbkdf2_ctx->result), 0, sizeof(pbkdf2_ctx->result));
    (void)memset_s(pbkdf2_ctx->data, sizeof(pbkdf2_ctx->data), 0, sizeof(pbkdf2_ctx->data));

    /* Formatting of the PBKDF2 salt Padding
     * The PBKDF2 salt Padding formatted as shown below:
     *                        big endian
     *         salt length || 32 bit(i) || 1 bit(1) || 2 word tail length
     *
     * sm3:
     * sha1:
     * sha256:   salt_len  ||   32 bit  ||   8 bit  ||  64 bit
     *
     * sha384:
     * sha512:   salt_len  ||   32 bit  ||   8 bit  ||  128 bit
     */
    if (pbkdf2_ctx->block_size == KDF_KEY_BLOCK_SIZE_512) {
        if (slen > pbkdf2_ctx->block_size - 13) {    // 13: refer above.
            is_salt_overflow = true;
        }
    } else {
        if (slen > pbkdf2_ctx->block_size - 21) {    // 21: refer above.
            is_salt_overflow = true;
        }
    }

    /*
     * Case 1. len(salt || padding) <= block_size.
     * data = salt || INT_32_BE(i).
    */
    if (!is_salt_overflow) {
        data_pad = (uint8_t *)pbkdf2_ctx->data;
        ret = memcpy_s(data_pad, sizeof(pbkdf2_ctx->data), salt, slen);
        drv_err_chk_return(ret != EOK, ERROR_MEMCPY_S, "memcpy_s failed\n");

        data_pad[slen + 3] = count & 0xff;          /* 3: need fill 32bit i, big endian padding */
        data_pad[slen + 2] = (count >> 8) & 0xff;   /* 2/8: need fill 32bit i, big endian padding */
        data_pad[slen + 1] = (count >> 16) & 0xff;  /* 16: need fill 32bit i, big endian padding */
        data_pad[slen] = (count >> 24) & 0xff;      /* 24: need fill 32bit i, big endian padding */

        pbkdf2_ctx->data_len = slen + sizeof(uint32_t);

        return CRYPTO_SUCCESS;
    }

    /*
     * Case 2. len(salt || padding) > block_size.
     * data = hmac(salt || INT_32_BE(i)).
     * is_salt_overflow = true, means iteration_count reduce by 1.
    */
    ret = drv_cipher_hmac_compute_setup(&hmac_ctx);
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "hmac_compute_setup failed\n");

    ret = drv_cipher_hmac_compute_start(hmac_ctx, pbkdf2_ctx->hash_type,
        (uint8_t *)pbkdf2_ctx->password, pbkdf2_ctx->block_size, CRYPTO_INVALID_KEYSLOT_HANDLE);
    drv_err_chk_goto(ret != CRYPTO_SUCCESS, exit_teardown, "hmac_compute_start failed\n");

    ret = drv_cipher_hmac_compute_update(hmac_ctx, salt, slen);
    drv_err_chk_goto(ret != CRYPTO_SUCCESS, exit_teardown, "hmac_compute_update failed\n");

    ret = drv_cipher_hmac_compute_update(hmac_ctx, (uint8_t *)(&count_be32), sizeof(count_be32));
    drv_err_chk_goto(ret != CRYPTO_SUCCESS, exit_teardown, "hmac_compute_update failed\n");

    ret = drv_cipher_hmac_compute_finish(hmac_ctx, (uint8_t *)pbkdf2_ctx->result, pbkdf2_ctx->result_size);
    drv_err_chk_goto(ret != CRYPTO_SUCCESS, exit_teardown, "hmac_compute_finish failed\n");

    ret = memcpy_s(pbkdf2_ctx->data, sizeof(pbkdf2_ctx->data), pbkdf2_ctx->result, pbkdf2_ctx->result_size);
    drv_err_chk_goto_with_ret(ret, ret != EOK, exit_teardown, ERROR_MEMCPY_S, "memcpy_s failed\n");

    pbkdf2_ctx->data_len = pbkdf2_ctx->result_size;
    pbkdf2_ctx->is_salt_overflow = true;
    ret = CRYPTO_SUCCESS;
exit_teardown:
    (void)drv_cipher_hmac_compute_teardown(hmac_ctx);
    return ret;
}

int32_t drv_cipher_pbkdf2_update_soft(void *ctx, uint32_t iteration_count)
{
    int32_t ret;
    drv_pbkdf2_ctx_t *pbkdf2_ctx = (drv_pbkdf2_ctx_t *)ctx;
    uint32_t i;
    uint32_t count;

    count = (pbkdf2_ctx->is_salt_overflow) ? (iteration_count - 1) : iteration_count;
    for (i = 0; i < count; i++) {
        ret = drv_cipher_hmac_compute(pbkdf2_ctx->hash_type,
            (uint8_t *)pbkdf2_ctx->password, pbkdf2_ctx->block_size, pbkdf2_ctx->keyslot_handle,
            (uint8_t *)pbkdf2_ctx->data, pbkdf2_ctx->data_len, (uint8_t *)pbkdf2_ctx->data, pbkdf2_ctx->result_size);
        drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "drv_cipher_hmac_compute failed\n");
        inner_pbkdf2_word_xor(pbkdf2_ctx->data, pbkdf2_ctx->result, pbkdf2_ctx->result_size);
        pbkdf2_ctx->data_len = pbkdf2_ctx->result_size;
    }

    return CRYPTO_SUCCESS;
}

int32_t drv_cipher_pbkdf2_finish(void *ctx, uint8_t *out_key, uint32_t out_key_len)
{
    drv_pbkdf2_ctx_t *pbkdf2_ctx = (drv_pbkdf2_ctx_t *)ctx;
    int32_t ret;

    ret = memcpy_s(out_key, out_key_len, pbkdf2_ctx->result, out_key_len);
    drv_err_chk_return(ret != EOK, ERROR_MEMCPY_S, "memcpy_s failed\n");

    return CRYPTO_SUCCESS;
}

int32_t drv_cipher_pbkdf2_teardown(void *ctx)
{
    drv_pbkdf2_ctx_t *pbkdf2_ctx = (drv_pbkdf2_ctx_t *)ctx;

    if (pbkdf2_ctx->keyslot_handle != CRYPTO_INVALID_KEYSLOT_HANDLE && pbkdf2_ctx->is_local_keyslot == true) {
        (void)drv_keyslot_teardown(pbkdf2_ctx->keyslot_handle);
    }

    (void)memset_s(pbkdf2_ctx, sizeof(drv_pbkdf2_ctx_t), 0, sizeof(drv_pbkdf2_ctx_t));
    crypto_free_coherent(pbkdf2_ctx);
    return CRYPTO_SUCCESS;
}