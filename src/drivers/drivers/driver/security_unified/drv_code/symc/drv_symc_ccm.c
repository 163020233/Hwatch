/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: drv symc ccm simple implement.
 *
 * Create: 2024-07-30
*/

#include "drv_symc.h"
#include "hal_symc.h"

#include "crypto_drv_common.h"

#if defined(CONFIG_DRV_SYMC_DEBUG_ENABLE)
#define crypto_drv_symc_ccm_log         crypto_print
#define crypto_drv_symc_ccm_dump        crypto_dump_data
#else
#define crypto_drv_symc_ccm_log(...)
#define crypto_drv_symc_ccm_dump(...)
#endif

#define SYMC_COMPAT_ERRNO(err_code)     DRV_COMPAT_ERRNO(ERROR_MODULE_SYMC, err_code)
#define symc_null_ptr_chk(ptr)   \
    crypto_chk_return((ptr) == TD_NULL, SYMC_COMPAT_ERRNO(ERROR_PARAM_IS_NULL), #ptr" is NULL\n")

static int inner_format_ccm_iv(const unsigned char *iv, unsigned int iv_length,
    unsigned char *ccm_iv, unsigned int ccm_iv_length)
{
    int ret;

    ccm_iv[0] = CRYPTO_AES_CCM_NQ_LEN - iv_length;
    ret = memcpy_s(&ccm_iv[1], ccm_iv_length - 1, iv, iv_length);
    crypto_chk_return(ret != EOK, ERROR_MEMCPY_S, "memcpy_s failed\n");

    return TD_SUCCESS;
}

static void inner_format_ccm_N(unsigned int total_aad_len, unsigned int tag_len, unsigned int data_len,
    const unsigned char *iv, unsigned int iv_len, unsigned char N[16])
{
    unsigned char idx = 0;
    unsigned char q;
    unsigned char N_len = CRYPTO_AES_BLOCK_SIZE_IN_BYTES;
    unsigned int i;

    /* CCM ADD N. */
    (td_void)memset_s(N, N_len, 0, N_len);

    /* First byte of N. */
    N[idx] = (total_aad_len > 0 ? 1 : 0) << CRYPTO_BIT_6;
    N[idx] |= ((tag_len - 2) / 2) << CRYPTO_BIT_3;  /* 2: ccm require */
    N[idx] |= (CRYPTO_AES_BLOCK_SIZE_IN_BYTES - 1 - iv_len);
    idx++;

    /* copy ccm_iv[1..] to N[1..] */
    for (i = 0; i < iv_len - 1; i++, idx++) {
        N[idx] = iv[1 + i];
    }

    q = N_len - idx;
    if (q >= CRYPTO_SYMC_CCM_Q_LEN_4B) {
        idx = CRYPTO_AES_BLOCK_SIZE_IN_BYTES - CRYPTO_SYMC_CCM_Q_LEN_4B;
        N[idx++] = (unsigned char)(data_len >> 24);     /* 24: bits 31-24 */
        N[idx++] = (unsigned char)(data_len >> 16);     /* 16: bits 23-16 */
        N[idx++] = (unsigned char)(data_len >> 8);      /* 8: bits 15-8 */
        N[idx++] = (unsigned char)(data_len);
    } else if (q == CRYPTO_SYMC_CCM_Q_LEN_3B) {
        N[idx++] = (unsigned char)(data_len >> 16);     /* 16: bits 23-16 */
        N[idx++] = (unsigned char)(data_len >> 8);      /* 8: bits 15-8 */
        N[idx++] = (unsigned char)(data_len);
    } else if (q == CRYPTO_SYMC_CCM_Q_LEN_2B) {
        N[idx++] = (unsigned char)(data_len >> 8);      /* 8: bits 15-8 */
        N[idx++] = (unsigned char)(data_len);
    }
}

static void inner_iv_plus_one(uint8_t *iv, uint32_t iv_len)
{
    uint32_t value = CRYPTO_BYTE_MAX + 1;
    int32_t i = (int32_t)iv_len - 1;

    while (value > CRYPTO_BYTE_MAX && i >= 0) {
        value = iv[i] + 1;
        iv[i] = value % (CRYPTO_BYTE_MAX + 1);
        iv--;
    }
}

static void inner_format_ccm_header(unsigned int aad_len, unsigned char *ccm_header, unsigned int *ccm_header_len)
{
    unsigned int idx = 0;
    ccm_header[idx++] = (unsigned char)(aad_len >> 8);  /* 8: bits 15-8 */
    ccm_header[idx++] = (unsigned char)(aad_len);
    *ccm_header_len = idx;
}

static int inner_ccm_crypt(crypto_symc_alg symc_alg, const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle,
    const uint8_t *iv, uint32_t iv_len,
    const uint8_t *aad, uint32_t aad_len,
    const uint8_t *input, uint8_t *output, uint32_t data_len,
    uint8_t *tag, uint32_t tag_len, bool is_encrypt)
{
    int ret;
    void *ccm_ctx = NULL;

    ret = drv_cipher_symc_ccm_setup(&ccm_ctx, symc_alg, key, key_len, keyslot_handle,
        iv, iv_len, is_encrypt);
    crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "drv_cipher_symc_ccm_setup failed\n");

    ret = drv_cipher_symc_ccm_set_lengths(ccm_ctx, aad_len, data_len, tag_len);
    crypto_chk_goto(ret != CRYPTO_SUCCESS, exit_teardown, "drv_cipher_symc_ccm_set_lengths failed\n");

    ret = drv_cipher_symc_ccm_update_ad(ccm_ctx, aad, aad_len);
    crypto_chk_goto(ret != CRYPTO_SUCCESS, exit_teardown, "drv_cipher_symc_ccm_update_ad failed\n");

    ret = drv_cipher_symc_ccm_update(ccm_ctx, input, output, data_len);
    crypto_chk_goto(ret != CRYPTO_SUCCESS, exit_teardown, "drv_cipher_symc_ccm_update failed\n");

    ret = drv_cipher_symc_ccm_finish(ccm_ctx, tag, tag_len);
    crypto_chk_goto(ret != CRYPTO_SUCCESS, exit_teardown, "drv_cipher_symc_ccm_finish failed\n");

exit_teardown:
    (void)drv_cipher_symc_ccm_teardown(ccm_ctx);
    return ret;
}

int32_t drv_cipher_symc_ccm_encrypt(crypto_symc_alg symc_alg,
    const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle,
    const uint8_t *iv, uint32_t iv_len,
    const uint8_t *aad, uint32_t aad_len,
    const uint8_t *plain_text, uint8_t *cipher_text, uint32_t data_len,
    uint8_t *tag, uint32_t tag_len)
{
    int ret;

    ret = inner_ccm_crypt(symc_alg, key, key_len, keyslot_handle, iv, iv_len, aad, aad_len, plain_text, cipher_text,
        data_len, tag, tag_len, true);
    crypto_chk_goto(ret != CRYPTO_SUCCESS, error_clean, "crypt failed\n");

    return CRYPTO_SUCCESS;
error_clean:
    (void)memset_s(cipher_text, data_len, 0, data_len);
    return ret;
}

int32_t drv_cipher_symc_ccm_decrypt_verify(crypto_symc_alg symc_alg,
    const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle,
    const uint8_t *iv, uint32_t iv_len,
    const uint8_t *aad, uint32_t aad_len,
    const uint8_t *cipher_text, uint8_t *plain_text, uint32_t data_len,
    const uint8_t *tag, uint32_t tag_len)
{
    int ret;
    unsigned int chk_sum = 0;
    unsigned int i;
    unsigned char out_tag[CRYPTO_AES_BLOCK_SIZE_IN_BYTES];

    symc_null_ptr_chk(tag);

    ret = inner_ccm_crypt(symc_alg, key, key_len, keyslot_handle, iv, iv_len, aad, aad_len, cipher_text, plain_text,
        data_len, out_tag, tag_len, false);
    crypto_chk_goto(ret != CRYPTO_SUCCESS, error_clean, "crypt failed\n");

    for (i = 0; i < tag_len; i++) {
        chk_sum += tag[i] ^ out_tag[i];
    }
    if (chk_sum != 0) {
        crypto_log_err("tag verify failed\n");
        ret = SYMC_COMPAT_ERRNO(ERROR_SYMC_AEAD_TAG_INVALID);
        goto error_clean;
    }

    return CRYPTO_SUCCESS;
error_clean:
    (void)memset_s(plain_text, data_len, 0, data_len);
    return ret;
}

typedef struct {
    crypto_symc_alg symc_alg;
    uint8_t key[CRYPTO_256_KEY_LEN];
    uint8_t ccm_iv[CRYPTO_AES_BLOCK_SIZE_IN_BYTES];
    uint8_t ctr0[CRYPTO_AES_BLOCK_SIZE_IN_BYTES];
    uint8_t stream_block[CRYPTO_AES_BLOCK_SIZE_IN_BYTES];
    uint32_t keyslot_handle;
    uint32_t key_len;
    uint32_t iv_len;
    uint32_t aad_len;
    uint32_t tag_len;
    uint32_t total_aad_len;
    uint32_t ccm_header_len;
    void *cmac_ctx;
    void *ctr_ctx;
    uint8_t nc_off;
    bool is_encrypt;
} drv_symc_ccm_ctx;

CRYPTO_STATIC int32_t inner_drv_symc_ccm_ctx_chk(const drv_symc_ccm_ctx *ctx)
{
    /* symc_alg chk. */
    crypto_chk_return(ctx->symc_alg != CRYPTO_SYMC_ALG_AES && ctx->symc_alg != CRYPTO_SYMC_ALG_SM4,
        ERROR_INVALID_PARAM, "symc_alg is invalid\n");
    /* iv_len chk. */
    crypto_chk_return(ctx->iv_len > 13 || ctx->iv_len < 7,      // 7: min iv_len for ccm, 13: max iv_len for ccm.
        SYMC_COMPAT_ERRNO(ERROR_INVALID_PARAM), "iv_len must be 7~13 for CCM\n");

    return CRYPTO_SUCCESS;
}

int32_t drv_cipher_symc_ccm_setup(void **ctx, crypto_symc_alg symc_alg,
    const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle,
    const uint8_t *iv, uint32_t iv_len, bool is_encrypt)
{
    int ret;
    drv_symc_ccm_ctx *ccm_ctx = NULL;

    symc_null_ptr_chk(ctx);
    if (keyslot_handle == CRYPTO_INVALID_KEYSLOT_HANDLE) {
        symc_null_ptr_chk(key);
    }
    symc_null_ptr_chk(iv);
    /* symc_alg check. */
    crypto_chk_return(symc_alg != CRYPTO_SYMC_ALG_AES && symc_alg != CRYPTO_SYMC_ALG_SM4,
        SYMC_COMPAT_ERRNO(ERROR_INVALID_PARAM), "symc_alg is invalid\n");
    /* key_len check. */
    crypto_chk_return(key_len != CRYPTO_128_KEY_LEN && key_len != CRYPTO_192_KEY_LEN && key_len != CRYPTO_256_KEY_LEN,
        SYMC_COMPAT_ERRNO(ERROR_INVALID_PARAM), "key_len is invalid\n");
    /* iv_len check, ccm iv len should be 7~13. */
    crypto_chk_return(iv_len > 13 || iv_len < 7,      // 7: min iv_len for ccm, 13: max iv_len for ccm.
        SYMC_COMPAT_ERRNO(ERROR_INVALID_PARAM), "iv_len must be 7~13 for CCM\n");

    ccm_ctx = crypto_malloc_coherent(sizeof(drv_symc_ccm_ctx));
    crypto_chk_return(ccm_ctx == NULL, ERROR_MALLOC, "malloc failed\n");
    (void)memset_s(ccm_ctx, sizeof(drv_symc_ccm_ctx), 0, sizeof(drv_symc_ccm_ctx));

    if (keyslot_handle == CRYPTO_INVALID_KEYSLOT_HANDLE) {
        ret = memcpy_s(ccm_ctx->key, sizeof(ccm_ctx->key), key, key_len);
        crypto_chk_goto_with_ret(ret, ret != EOK, error_free, ERROR_MEMCPY_S, "memcpy_s failed\n");
    }

    ccm_ctx->keyslot_handle = keyslot_handle;
    ccm_ctx->is_encrypt = is_encrypt;
    ccm_ctx->key_len = key_len;
    ccm_ctx->symc_alg = symc_alg;
    ccm_ctx->iv_len = iv_len;

    ret = inner_format_ccm_iv(iv, iv_len, ccm_ctx->ccm_iv, sizeof(ccm_ctx->ccm_iv));
    crypto_chk_goto(ret != CRYPTO_SUCCESS, error_free, "inner_format_ccm_iv failed, ret is 0x%x\n", ret);

    ret = memcpy_s(ccm_ctx->ctr0, sizeof(ccm_ctx->ctr0), ccm_ctx->ccm_iv, ccm_ctx->iv_len + 1);
    crypto_chk_goto_with_ret(ret, ret != EOK, error_free, ERROR_MEMCPY_S, "memcpy_s failed\n");
    crypto_drv_symc_ccm_dump("ctr0", ccm_ctx->ctr0, CRYPTO_AES_BLOCK_SIZE_IN_BYTES);

    /* cmac start. */
    ret = drv_symc_mac_start(&(ccm_ctx->cmac_ctx), symc_alg, CRYPTO_SYMC_WORK_MODE_CBC_MAC,
        ccm_ctx->key, ccm_ctx->key_len, ccm_ctx->keyslot_handle);
    crypto_chk_goto(ret != CRYPTO_SUCCESS, error_free, "drv_symc_mac_start failed\n");

    /* ctr setup. */
    ret = drv_cipher_symc_setup(&(ccm_ctx->ctr_ctx),
        crypto_compat_symc_alg(symc_alg, CRYPTO_SYMC_WORK_MODE_CTR, CRYPTO_SYMC_BIT_WIDTH_128BIT),
        ccm_ctx->key, ccm_ctx->key_len, ccm_ctx->keyslot_handle, is_encrypt);
    crypto_chk_goto(ret != CRYPTO_SUCCESS, error_ghash_destroy, "drv_cipher_symc_setup failed\n");

    *ctx = ccm_ctx;
    return CRYPTO_SUCCESS;

error_ghash_destroy:
    (void)drv_symc_mac_destroy(ccm_ctx->cmac_ctx);
error_free:
    (void)memset_s(ccm_ctx, sizeof(drv_symc_ccm_ctx), 0, sizeof(drv_symc_ccm_ctx));
    crypto_free_coherent(ccm_ctx);
    return ret;
}

int32_t drv_cipher_symc_ccm_set_lengths(void *ctx, uint32_t total_aad_len,
    uint32_t data_len, uint32_t tag_len)
{
    int ret;
    unsigned char diff;
    drv_symc_ccm_ctx *ccm_ctx = (drv_symc_ccm_ctx *)ctx;
    unsigned char ccm_N[CRYPTO_AES_BLOCK_SIZE_IN_BYTES] = { 0 };
    unsigned char ccm_header[CRYPTO_AES_BLOCK_SIZE_IN_BYTES] = { 0 };

    symc_null_ptr_chk(ccm_ctx);

    ret = inner_drv_symc_ccm_ctx_chk(ccm_ctx);
    crypto_chk_return(ret != CRYPTO_SUCCESS, ret);

    /* tag_len check, ccm tag len should be 4,6,8,10,12,14,16. */
    crypto_chk_return(
        tag_len % 2 != 0 || tag_len < 4 || tag_len > 16,  // 4: min tag_len, 16: max tag_len, 2: judge if even.
        SYMC_COMPAT_ERRNO(ERROR_INVALID_PARAM), "tag_len must be 4,6,8,10,12,14,16 for CCM\n");
    diff = CRYPTO_IV_LEN_IN_BYTES - ccm_ctx->iv_len - 1;
    if (diff == 2) {    // 2: 2 * 8 bits to represent data_len.
        crypto_chk_return(data_len > 0xFFFF, SYMC_COMPAT_ERRNO(ERROR_INVALID_PARAM), // 0xFFFF: 2^16 - 1
            "data_len is too long\n");
    }
    if (diff == 3) {    // 3: 3 * 8 bits to represent data_len.
        crypto_chk_return(data_len > 0xFFFFFF, SYMC_COMPAT_ERRNO(ERROR_INVALID_PARAM), // 0xFFFFFF: 2^24 - 1
            "data_len is too long\n");
    }

    /* ccm cmac N. */
    inner_format_ccm_N(total_aad_len, tag_len, data_len, ccm_ctx->ccm_iv, ccm_ctx->iv_len + 1, ccm_N);
    inner_iv_plus_one(ccm_ctx->ccm_iv, sizeof(ccm_ctx->ccm_iv));
    inner_format_ccm_header(total_aad_len, ccm_header, &(ccm_ctx->ccm_header_len));
    crypto_drv_symc_ccm_dump("ccm_iv", ccm_ctx->ccm_iv, CRYPTO_AES_BLOCK_SIZE_IN_BYTES);

    /* CMAC(N || ccm_header) */
    crypto_drv_symc_ccm_dump("ccm_N", ccm_N, CRYPTO_AES_BLOCK_SIZE_IN_BYTES);
    ret = drv_symc_mac_update(ccm_ctx->cmac_ctx, ccm_N, CRYPTO_AES_BLOCK_SIZE_IN_BYTES);
    crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "drv_symc_mac_update failed\n");

    if (total_aad_len != 0) {
        crypto_drv_symc_ccm_dump("ccm_header", ccm_header, ccm_ctx->ccm_header_len);
        ret = drv_symc_mac_update(ccm_ctx->cmac_ctx, ccm_header, ccm_ctx->ccm_header_len);
        crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "drv_symc_mac_update failed\n");
    }

    ccm_ctx->total_aad_len = total_aad_len;
    ccm_ctx->tag_len = tag_len;
    return CRYPTO_SUCCESS;
}

int32_t drv_cipher_symc_ccm_update_ad(void *ctx, const uint8_t *aad, uint32_t aad_len)
{
    int ret;
    drv_symc_ccm_ctx *ccm_ctx = (drv_symc_ccm_ctx *)ctx;
    unsigned char aad_padding[CRYPTO_AES_BLOCK_SIZE_IN_BYTES] = { 0 };
    unsigned int aad_padding_len;

    if (aad_len == 0) {
        return CRYPTO_SUCCESS;
    }

    symc_null_ptr_chk(ccm_ctx);
    if (aad_len != 0) {
        symc_null_ptr_chk(aad);
    }

    /* CMAC(aad || aad_padding) */
    crypto_drv_symc_ccm_dump("aad", aad, aad_len);
    ret = drv_symc_mac_update(ccm_ctx->cmac_ctx, aad, aad_len);
    crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "drv_symc_mac_update failed\n");

    ccm_ctx->aad_len += aad_len;
    if (ccm_ctx->aad_len == ccm_ctx->total_aad_len) {
        aad_padding_len = CRYPTO_AES_BLOCK_SIZE_IN_BYTES -
            (ccm_ctx->total_aad_len + ccm_ctx->ccm_header_len) % CRYPTO_AES_BLOCK_SIZE_IN_BYTES;
        if ((aad_padding_len % CRYPTO_AES_BLOCK_SIZE_IN_BYTES) != 0) {
            crypto_drv_symc_ccm_dump("aad_padding", aad_padding, aad_padding_len);
            ret = drv_symc_mac_update(ccm_ctx->cmac_ctx, aad_padding, aad_padding_len);
            crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "drv_symc_mac_update failed\n");
        }
    }
    return CRYPTO_SUCCESS;
}

int32_t drv_cipher_symc_ccm_update(void *ctx, const uint8_t *src, uint8_t *dst, uint32_t data_len)
{
    int ret;
    drv_symc_ccm_ctx *ccm_ctx = (drv_symc_ccm_ctx *)ctx;

    symc_null_ptr_chk(ccm_ctx);
    if (data_len != 0) {
        symc_null_ptr_chk(src);
        symc_null_ptr_chk(dst);
    }

    /* CMAC(plain_text) */
    if (ccm_ctx->is_encrypt == true) {  // for encrypt.
        crypto_drv_symc_ccm_dump("plain_text", src, data_len);
        ret = drv_symc_mac_update(ccm_ctx->cmac_ctx, src, data_len);
        crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "drv_symc_mac_update failed\n");
    }
    ret = drv_cipher_symc_ctr_update(ccm_ctx->ctr_ctx, src, dst, data_len, ccm_ctx->ccm_iv, &ccm_ctx->nc_off,
        ccm_ctx->stream_block);
    crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "drv_cipher_symc_ctr_update failed\n");
    if (ccm_ctx->is_encrypt == false) {  // for decrypt.
        crypto_drv_symc_ccm_dump("plain_text", dst, data_len);
        ret = drv_symc_mac_update(ccm_ctx->cmac_ctx, dst, data_len);
        crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "drv_symc_mac_update failed\n");
    }
    return CRYPTO_SUCCESS;
}

int32_t drv_cipher_symc_ccm_finish(void *ctx, uint8_t *tag, uint32_t tag_len)
{
    int ret;
    drv_symc_ccm_ctx *ccm_ctx = (drv_symc_ccm_ctx *)ctx;
    unsigned int i;
    unsigned char ccm_cmac[CRYPTO_AES_BLOCK_SIZE_IN_BYTES] = { 0 };

    symc_null_ptr_chk(ccm_ctx);
    symc_null_ptr_chk(tag);
    crypto_chk_return(tag_len != ccm_ctx->tag_len, SYMC_COMPAT_ERRNO(ERROR_INVALID_PARAM),
        "tag_len is invalid\n");
    /* tag_len check, ccm tag len should be 4,6,8,10,12,14,16. */
    crypto_chk_return(
        tag_len % 2 != 0 || tag_len < 4 || tag_len > 16,  // 4: min tag_len, 16: max tag_len, 2: judge if even.
        SYMC_COMPAT_ERRNO(ERROR_INVALID_PARAM), "tag_len must be 4,6,8,10,12,14,16 for CCM\n");

    ret = drv_symc_mac_finish(ccm_ctx->cmac_ctx, ccm_cmac);
    crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "drv_symc_mac_finish failed\n");
    crypto_drv_symc_ccm_dump("ccm_cmac", ccm_cmac, CRYPTO_AES_BLOCK_SIZE_IN_BYTES);

    crypto_drv_symc_ccm_dump("ctr0", ccm_ctx->ctr0, CRYPTO_AES_BLOCK_SIZE_IN_BYTES);
    ret = hal_cipher_symc_crypt(ccm_ctx->symc_alg, CRYPTO_SYMC_WORK_MODE_ECB, CRYPTO_SYMC_BIT_WIDTH_128BIT,
        crypto_get_phys_addr(ccm_ctx->ctr0), crypto_get_phys_addr(ccm_ctx->ctr0),
        CRYPTO_AES_BLOCK_SIZE_IN_BYTES, NULL, ccm_ctx->key, ccm_ctx->key_len, ccm_ctx->keyslot_handle, 0);
    crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "hal_cipher_symc_crypt failed\n");
    crypto_drv_symc_ccm_dump("Encrypt(ctr0)", ccm_ctx->ctr0, CRYPTO_AES_BLOCK_SIZE_IN_BYTES);

    for (i = 0; i < tag_len; i++) {
        tag[i] = ccm_ctx->ctr0[i] ^ ccm_cmac[i];
    }

    return CRYPTO_SUCCESS;
}

int32_t drv_cipher_symc_ccm_teardown(void *ctx)
{
    drv_symc_ccm_ctx *ccm_ctx = (drv_symc_ccm_ctx *)ctx;

    symc_null_ptr_chk(ccm_ctx);
    (void)drv_symc_mac_destroy(ccm_ctx->cmac_ctx);
    (void)drv_cipher_symc_teardown(ccm_ctx->ctr_ctx);

    (void)memset_s(ccm_ctx, sizeof(drv_symc_ccm_ctx), 0, sizeof(drv_symc_ccm_ctx));
    crypto_free_coherent(ccm_ctx);
    return CRYPTO_SUCCESS;
}
