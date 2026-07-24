/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: key derivation algorithm of pbkdf2 implementation.
 *
*/

#include "drv_hash.h"
#include "hal_keyslot_romable.h"
#include "crypto_hash_common.h"

#define FILE_ID FILE_ID_DRV_CODE_KM_DRV_PBKDF2

#define KM_COMPAT_ERRNO(err_code)         DRV_COMPAT_ERRNO(ERROR_MODULE_KM, err_code)

static int32_t inner_get_pbkdf2_alg(crypto_hash_type hash_type, uint32_t *pbkdf2_alg)
{
    switch (hash_type) {
        case CRYPTO_HASH_TYPE_HMAC_SHA1:
            *pbkdf2_alg = PBKDF2_ALG_SEL_SHA1;
            break;
        case CRYPTO_HASH_TYPE_HMAC_SHA256:
            *pbkdf2_alg = PBKDF2_ALG_SEL_SHA256;
            break;
        case CRYPTO_HASH_TYPE_HMAC_SHA384:
            *pbkdf2_alg = PBKDF2_ALG_SEL_SHA384;
            break;
        case CRYPTO_HASH_TYPE_HMAC_SHA512:
            *pbkdf2_alg = PBKDF2_ALG_SEL_SHA512;
            break;
        case CRYPTO_HASH_TYPE_HMAC_SM3:
            *pbkdf2_alg = PBKDF2_ALG_SEL_SM3;
            break;
        default:
            drv_set_err(ERROR_INVALID_PARAM, "direct error return");
            return ERROR_INVALID_PARAM;
    }
    return CRYPTO_SUCCESS;
}

#define KDF_KEY_BLOCK_SIZE_512     64
#define KDF_KEY_BLOCK_SIZE_1024    128
int32_t drv_cipher_pbkdf2_update_hardware(void *ctx, uint16_t iteration_count)
{
    int32_t ret;
    drv_pbkdf2_ctx_t *pbkdf2_ctx = (drv_pbkdf2_ctx_t *)ctx;
    uint16_t count;
    uint32_t data_len_in_bits;
    uint32_t pbkdf2_alg;
    uint8_t *data_pad = NULL;
    uint32_t padding_len = KDF_KEY_BLOCK_SIZE_512;
    const uint32_t *state_val = NULL;
    uint32_t state_val_size;

    (void)state_val;
    (void)state_val_size;

    ret = inner_get_pbkdf2_alg(pbkdf2_ctx->hash_type, &pbkdf2_alg);
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "inner_get_pbkdf2_alg failed\n");

    if (pbkdf2_alg == PBKDF2_ALG_SEL_SHA384 || pbkdf2_alg == PBKDF2_ALG_SEL_SHA512) {
        padding_len = KDF_KEY_BLOCK_SIZE_1024;
    }

    count = (pbkdf2_ctx->is_salt_overflow) ? (iteration_count - 1) : iteration_count;

    ret = hal_rkp_lock_romable();
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "hal_rkp_lock_romable failed\n");

    /* set password at first, clean in the end. */
    hal_kdf_pbkdf2_set_key_romable(pbkdf2_ctx->password, pbkdf2_ctx->block_size);

    /* set result. */
#if defined(CONFIG_PBKDF2_V1)
    state_val = (const uint32_t *)(uintptr_t)crypto_hash_common_get_state_iv(pbkdf2_ctx->hash_type, &state_val_size);
    drv_err_chk_goto(state_val == NULL, exit_clean, "crypto_hash_common_get_state_iv failed\n");
    hal_kdf_pbkdf2_set_val_romable(state_val, state_val_size);
#elif defined(CONFIG_PBKDF2_V2)
    hal_kdf_pbkdf2_set_val_romable(pbkdf2_ctx->result, pbkdf2_ctx->result_size);
#else
    ret = ERROR_UNSUPPORT;
    goto exit_clean;
#endif

    data_len_in_bits = (pbkdf2_ctx->data_len + padding_len) * CRYPTO_BITS_IN_BYTE;
    /* padding.
     * data_pad = data || 0x80 || 0000..00 || INT_32_BE(data_len_in_bits)
     */
    data_pad = (uint8_t *)pbkdf2_ctx->data;
    (void)memset_s(&data_pad[pbkdf2_ctx->data_len], sizeof(pbkdf2_ctx->data) - pbkdf2_ctx->data_len, 0,
        sizeof(pbkdf2_ctx->data) - pbkdf2_ctx->data_len);

    data_pad[pbkdf2_ctx->data_len] = 0x80;   // 0x80: 1-bit 1
    data_pad[pbkdf2_ctx->block_size - 1] = data_len_in_bits & 0xff;
    data_pad[pbkdf2_ctx->block_size - 2] = (data_len_in_bits >> 8) & 0xff;  /* 2: 2bit 8: right move 8 */
    data_pad[pbkdf2_ctx->block_size - 3] = (data_len_in_bits >> 16) & 0xff; /* 3: 3bit 16: right move 16 */
    data_pad[pbkdf2_ctx->block_size - 4] = (data_len_in_bits >> 24) & 0xff; /* 4: 4bit 24: right move 24 */

    ret = hal_kdf_pbkdf2_compute_romable(pbkdf2_alg, count, pbkdf2_ctx->data, pbkdf2_ctx->data, pbkdf2_ctx->result);
    drv_err_chk_goto(ret != CRYPTO_SUCCESS, exit_clean, "hal_kdf_pbkdf2_compute_romable failed\n");
    pbkdf2_ctx->data_len = pbkdf2_ctx->result_size;

    /* set data[result_size..] = 0. */
    (void)memset_s(&(data_pad[pbkdf2_ctx->result_size]), sizeof(pbkdf2_ctx->data) - pbkdf2_ctx->result_size,
        0, sizeof(pbkdf2_ctx->data) - pbkdf2_ctx->result_size);

exit_clean:
    /* clear key. */
    hal_kdf_pbkdf2_clear_key_romable();
    hal_rkp_unlock_romable();
    return ret;
}
