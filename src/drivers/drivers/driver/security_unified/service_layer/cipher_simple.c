/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: Provides cipher simple driver source \n
 *
 * History: \n
 * 2024-05-23, Create file. \n
 */

#include "sl_common.h"
#include "crypto_common_macro.h"
#include "crypto_drv_common.h"

#include "drv_symc.h"
#include "security_km.h"
#include "security_symc.h"

#if defined(CONFIG_SYMC_SUPPORT_ECB) || defined(CONFIG_SYMC_SUPPORT_CBC) || defined(CONFIG_SYMC_SUPPORT_CTR) || \
    defined(CONFIG_SYMC_SUPPORT_OFB) || defined(CONFIG_SYMC_SUPPORT_CFB8) || defined(CONFIG_SYMC_SUPPORT_CFB128)
errcode_t uapi_drv_cipher_symc_crypt(uint32_t alg, const uint8_t *src, uint8_t *dst, uint32_t data_len,
    uint8_t iv[16], const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle, bool is_encrypt)
{
    crypto_service_preprocess(SYMC_MODULE_ID);
    int32_t ret = drv_cipher_symc_crypt(alg, src, dst, data_len, iv, key, key_len, keyslot_handle, is_encrypt);
    crypto_service_postprocess(SYMC_MODULE_ID);

    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_symc_setup(cipher_handle_t *ctx, uint32_t alg,
    const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle, bool is_encrypt)
{
    crypto_service_preprocess(SYMC_MODULE_ID);
    int32_t ret = drv_cipher_symc_setup((void **)ctx, alg, key, key_len, keyslot_handle, is_encrypt);
    if (ret != CRYPTO_SUCCESS) {
        crypto_service_postprocess(SYMC_MODULE_ID);
    }

    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_symc_teardown(cipher_handle_t ctx)
{
    int32_t ret = drv_cipher_symc_teardown((void *)ctx);
    crypto_service_postprocess(SYMC_MODULE_ID);

    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_symc_set_key(cipher_handle_t ctx, const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle)
{
    int32_t ret = drv_cipher_symc_set_key((void *)ctx, key, key_len, keyslot_handle);
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_symc_set_alg(cipher_handle_t ctx, uint32_t alg)
{
    int32_t ret = drv_cipher_symc_set_alg((void *)ctx, alg);
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_symc_set_crypt_mode(cipher_handle_t ctx, bool is_encrypt)
{
    int32_t ret = drv_cipher_symc_set_crypt_mode((void *)ctx, is_encrypt);
    return crypto_sl_common_get_errcode(ret);
}

#else
errcode_t uapi_drv_cipher_symc_crypt(uint32_t alg, const uint8_t *src, uint8_t *dst, uint32_t data_len,
    uint8_t iv[16], const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle, bool is_encrypt)
{
    crypto_unused(alg);
    crypto_unused(src);
    crypto_unused(dst);
    crypto_unused(data_len);
    crypto_unused(iv);
    crypto_unused(key);
    crypto_unused(key_len);
    crypto_unused(keyslot_handle);
    crypto_unused(is_encrypt);
    return ERRCODE_SECURITY_UNSUPPORT;
}

errcode_t uapi_drv_cipher_symc_setup(cipher_handle_t *ctx, uint32_t alg,
    const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle, bool is_encrypt)
{
    crypto_unused(ctx);
    crypto_unused(alg);
    crypto_unused(key);
    crypto_unused(key_len);
    crypto_unused(keyslot_handle);
    crypto_unused(is_encrypt);
    return ERRCODE_SECURITY_UNSUPPORT;
}

errcode_t uapi_drv_cipher_symc_teardown(cipher_handle_t ctx)
{
    crypto_unused(ctx);
    return ERRCODE_SECURITY_UNSUPPORT;
}

errcode_t uapi_drv_cipher_symc_set_key(cipher_handle_t ctx, const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle)
{
    (void)keyslot_handle;
    (void)ctx;
    (void)key;
    (void)key_len;
    return ERRCODE_SECURITY_UNSUPPORT;
}

errcode_t uapi_drv_cipher_symc_set_alg(cipher_handle_t ctx, uint32_t alg)
{
    (void)ctx;
    (void)alg;
    return ERRCODE_SECURITY_UNSUPPORT;
}

errcode_t uapi_drv_cipher_symc_set_crypt_mode(cipher_handle_t ctx, bool is_encrypt)
{
    (void)ctx;
    (void)is_encrypt;
    return ERRCODE_SECURITY_UNSUPPORT;
}
#endif

#if defined(CONFIG_SYMC_SUPPORT_ECB)
errcode_t uapi_drv_cipher_symc_ecb_update(cipher_handle_t ctx, const uint8_t *src, uint8_t *dst, uint32_t data_len)
{
    int32_t ret = drv_cipher_symc_ecb_update((void *)ctx, src, dst, data_len);

    return crypto_sl_common_get_errcode(ret);
}
#else
errcode_t uapi_drv_cipher_symc_ecb_update(cipher_handle_t ctx, const uint8_t *src, uint8_t *dst, uint32_t data_len)
{
    crypto_unused(ctx);
    crypto_unused(src);
    crypto_unused(dst);
    crypto_unused(data_len);
    return ERRCODE_SECURITY_UNSUPPORT;
}
#endif

#if defined(CONFIG_SYMC_SUPPORT_CBC)
errcode_t uapi_drv_cipher_symc_cbc_update(cipher_handle_t ctx, const uint8_t *src, uint8_t *dst, uint32_t data_len,
    uint8_t iv[16])
{
    int32_t ret = drv_cipher_symc_cbc_update((void *)ctx, src, dst, data_len, iv);

    return crypto_sl_common_get_errcode(ret);
}
#else
errcode_t uapi_drv_cipher_symc_cbc_update(cipher_handle_t ctx, const uint8_t *src, uint8_t *dst, uint32_t data_len,
    uint8_t iv[16])
{
    crypto_unused(ctx);
    crypto_unused(src);
    crypto_unused(dst);
    crypto_unused(data_len);
    crypto_unused(iv);
    return ERRCODE_SECURITY_UNSUPPORT;
}
#endif

#if defined(CONFIG_SYMC_SUPPORT_CTR)
errcode_t uapi_drv_cipher_symc_ctr_update(cipher_handle_t ctx, const uint8_t *src, uint8_t *dst, uint32_t data_len,
    uint8_t nonce_counter[16], uint8_t *nc_off, uint8_t stream_block[16])
{
    int32_t ret = drv_cipher_symc_ctr_update((void *)ctx, src, dst, data_len, nonce_counter, nc_off, stream_block);

    return crypto_sl_common_get_errcode(ret);
}
#else
errcode_t uapi_drv_cipher_symc_ctr_update(cipher_handle_t ctx, const uint8_t *src, uint8_t *dst, uint32_t data_len,
    uint8_t nonce_counter[16], uint8_t *nc_off, uint8_t stream_block[16])
{
    crypto_unused(ctx);
    crypto_unused(src);
    crypto_unused(dst);
    crypto_unused(data_len);
    crypto_unused(nonce_counter);
    crypto_unused(nc_off);
    crypto_unused(stream_block);
    return ERRCODE_SECURITY_UNSUPPORT;
}
#endif

#if defined(CONFIG_SYMC_SUPPORT_OFB)
errcode_t uapi_drv_cipher_symc_ofb_update(cipher_handle_t ctx, const uint8_t *src, uint8_t *dst, uint32_t data_len,
    uint8_t iv[16], uint8_t *iv_off)
{
    int32_t ret = drv_cipher_symc_ofb_update((void *)ctx, src, dst, data_len, iv, iv_off);

    return crypto_sl_common_get_errcode(ret);
}
#else
errcode_t uapi_drv_cipher_symc_ofb_update(cipher_handle_t ctx, const uint8_t *src, uint8_t *dst, uint32_t data_len,
    uint8_t iv[16], uint8_t *iv_off)
{
    crypto_unused(ctx);
    crypto_unused(src);
    crypto_unused(dst);
    crypto_unused(data_len);
    crypto_unused(iv);
    crypto_unused(iv_off);
    return ERRCODE_SECURITY_UNSUPPORT;
}
#endif

#if defined(CONFIG_SYMC_SUPPORT_CFB8)
errcode_t uapi_drv_cipher_symc_cfb8_update(cipher_handle_t ctx, const uint8_t *src, uint8_t *dst, uint32_t data_len,
    uint8_t iv[16], uint8_t *iv_off)
{
    int32_t ret = drv_cipher_symc_cfb8_update((void *)ctx, src, dst, data_len, iv, iv_off);

    return crypto_sl_common_get_errcode(ret);
}
#else
errcode_t uapi_drv_cipher_symc_cfb8_update(cipher_handle_t ctx, const uint8_t *src, uint8_t *dst, uint32_t data_len,
    uint8_t iv[16], uint8_t *iv_off)
{
    crypto_unused(ctx);
    crypto_unused(src);
    crypto_unused(dst);
    crypto_unused(data_len);
    crypto_unused(iv);
    crypto_unused(iv_off);

    return ERRCODE_SECURITY_UNSUPPORT;
}
#endif

#if defined(CONFIG_SYMC_SUPPORT_CFB128)
errcode_t uapi_drv_cipher_symc_cfb128_update(cipher_handle_t ctx, const uint8_t *src, uint8_t *dst, uint32_t data_len,
    uint8_t iv[16], uint8_t *iv_off)
{
    int32_t ret = drv_cipher_symc_cfb128_update((void *)ctx, src, dst, data_len, iv, iv_off);

    return crypto_sl_common_get_errcode(ret);
}
#else
errcode_t uapi_drv_cipher_symc_cfb128_update(cipher_handle_t ctx, const uint8_t *src, uint8_t *dst, uint32_t data_len,
    uint8_t iv[16], uint8_t *iv_off)
{
    crypto_unused(ctx);
    crypto_unused(src);
    crypto_unused(dst);
    crypto_unused(data_len);
    crypto_unused(iv);
    crypto_unused(iv_off);
    return ERRCODE_SECURITY_UNSUPPORT;
}
#endif

#if defined(CONFIG_SYMC_SUPPORT_CCM)
errcode_t uapi_drv_cipher_symc_ccm_encrypt(uapi_drv_cipher_symc_alg_t alg,
    const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle,
    const uint8_t *iv, uint32_t iv_len,
    const uint8_t *aad, uint32_t aad_len,
    const uint8_t *plain_text, uint8_t *cipher_text, uint32_t data_len,
    uint8_t *tag, uint32_t tag_len)
{
    crypto_service_preprocess(SYMC_MODULE_ID);
    int32_t ret = drv_cipher_symc_ccm_encrypt((crypto_symc_alg)alg, key, key_len, keyslot_handle,
        iv, iv_len, aad, aad_len, plain_text, cipher_text, data_len, tag, tag_len);
    crypto_service_postprocess(SYMC_MODULE_ID);

    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_symc_ccm_decrypt_verify(uapi_drv_cipher_symc_alg_t alg,
    const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle,
    const uint8_t *iv, uint32_t iv_len,
    const uint8_t *aad, uint32_t aad_len,
    const uint8_t *cipher_text, uint8_t *plain_text, uint32_t data_len,
    const uint8_t *tag, uint32_t tag_len)
{
    crypto_service_preprocess(SYMC_MODULE_ID);
    int32_t ret = drv_cipher_symc_ccm_decrypt_verify((crypto_symc_alg)alg, key, key_len, keyslot_handle,
        iv, iv_len, aad, aad_len, cipher_text, plain_text, data_len, tag, tag_len);
    crypto_service_postprocess(SYMC_MODULE_ID);

    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_symc_ccm_setup(cipher_handle_t *ctx, uapi_drv_cipher_symc_alg_t alg,
    const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle,
    const uint8_t *iv, uint32_t iv_len, bool is_encrypt)
{
    crypto_service_preprocess(SYMC_MODULE_ID);
    int32_t ret = drv_cipher_symc_ccm_setup((void **)ctx, (crypto_symc_alg)alg, key, key_len, keyslot_handle,
        iv, iv_len, is_encrypt);
    if (ret != CRYPTO_SUCCESS) {
        crypto_service_postprocess(SYMC_MODULE_ID);
    }

    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_symc_ccm_set_lengths(cipher_handle_t ctx, uint32_t total_aad_len,
    uint32_t data_len, uint32_t tag_len)
{
    int32_t ret = drv_cipher_symc_ccm_set_lengths((void *)ctx, total_aad_len, data_len, tag_len);

    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_symc_ccm_update_ad(cipher_handle_t ctx, const uint8_t *aad, uint32_t aad_len)
{
    int32_t ret = drv_cipher_symc_ccm_update_ad((void *)ctx, aad, aad_len);

    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_symc_ccm_update(cipher_handle_t ctx, const uint8_t *src, uint8_t *dst, uint32_t data_len)
{
    int32_t ret = drv_cipher_symc_ccm_update((void *)ctx, src, dst, data_len);

    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_symc_ccm_finish(cipher_handle_t ctx, uint8_t *tag, uint32_t tag_len)
{
    int32_t ret = drv_cipher_symc_ccm_finish((void *)ctx, tag, tag_len);

    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_symc_ccm_teardown(cipher_handle_t ctx)
{
    int32_t ret = drv_cipher_symc_ccm_teardown((void *)ctx);
    crypto_service_postprocess(SYMC_MODULE_ID);

    return crypto_sl_common_get_errcode(ret);
}
#else
errcode_t uapi_drv_cipher_symc_ccm_encrypt(uapi_drv_cipher_symc_alg_t alg,
    const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle,
    const uint8_t *iv, uint32_t iv_len,
    const uint8_t *aad, uint32_t aad_len,
    const uint8_t *plain_text, uint8_t *cipher_text, uint32_t data_len,
    uint8_t *tag, uint32_t tag_len)
{
    crypto_unused(alg);
    crypto_unused(key);
    crypto_unused(key_len);
    crypto_unused(keyslot_handle);
    crypto_unused(iv);
    crypto_unused(iv_len);
    crypto_unused(aad);
    crypto_unused(aad_len);
    crypto_unused(plain_text);
    crypto_unused(cipher_text);
    crypto_unused(data_len);
    crypto_unused(tag);
    crypto_unused(tag_len);
    return ERRCODE_SECURITY_UNSUPPORT;
}

errcode_t uapi_drv_cipher_symc_ccm_decrypt_verify(uapi_drv_cipher_symc_alg_t alg,
    const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle,
    const uint8_t *iv, uint32_t iv_len,
    const uint8_t *aad, uint32_t aad_len,
    const uint8_t *cipher_text, uint8_t *plain_text, uint32_t data_len,
    const uint8_t *tag, uint32_t tag_len)
{
    crypto_unused(alg);
    crypto_unused(key);
    crypto_unused(key_len);
    crypto_unused(keyslot_handle);
    crypto_unused(iv);
    crypto_unused(iv_len);
    crypto_unused(aad);
    crypto_unused(aad_len);
    crypto_unused(cipher_text);
    crypto_unused(plain_text);
    crypto_unused(data_len);
    crypto_unused(tag);
    crypto_unused(tag_len);
    return ERRCODE_SECURITY_UNSUPPORT;
}

errcode_t uapi_drv_cipher_symc_ccm_setup(cipher_handle_t *ctx, uapi_drv_cipher_symc_alg_t alg,
    const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle,
    const uint8_t *iv, uint32_t iv_len, bool is_encrypt)
{
    crypto_unused(ctx);
    crypto_unused(alg);
    crypto_unused(key);
    crypto_unused(key_len);
    crypto_unused(keyslot_handle);
    crypto_unused(iv);
    crypto_unused(iv_len);
    crypto_unused(is_encrypt);

    return ERRCODE_SECURITY_UNSUPPORT;
}

errcode_t uapi_drv_cipher_symc_ccm_set_lengths(cipher_handle_t ctx, uint32_t total_aad_len,
    uint32_t data_len, uint32_t tag_len)
{
    crypto_unused(ctx);
    crypto_unused(total_aad_len);
    crypto_unused(data_len);
    crypto_unused(tag_len);

    return ERRCODE_SECURITY_UNSUPPORT;
}

errcode_t uapi_drv_cipher_symc_ccm_update_ad(cipher_handle_t ctx, const uint8_t *aad, uint32_t aad_len)
{
    crypto_unused(ctx);
    crypto_unused(aad);
    crypto_unused(aad_len);

    return ERRCODE_SECURITY_UNSUPPORT;
}

errcode_t uapi_drv_cipher_symc_ccm_update(cipher_handle_t ctx, const uint8_t *src, uint8_t *dst, uint32_t data_len)
{
    crypto_unused(ctx);
    crypto_unused(src);
    crypto_unused(dst);
    crypto_unused(data_len);

    return ERRCODE_SECURITY_UNSUPPORT;
}

errcode_t uapi_drv_cipher_symc_ccm_finish(cipher_handle_t ctx, uint8_t *tag, uint32_t tag_len)
{
    crypto_unused(ctx);
    crypto_unused(tag);
    crypto_unused(tag_len);

    return ERRCODE_SECURITY_UNSUPPORT;
}

errcode_t uapi_drv_cipher_symc_ccm_teardown(cipher_handle_t ctx)
{
    crypto_unused(ctx);

    return ERRCODE_SECURITY_UNSUPPORT;
}
#endif

#if defined(CONFIG_SYMC_SUPPORT_GCM)
errcode_t uapi_drv_cipher_symc_gcm_encrypt(uapi_drv_cipher_symc_alg_t alg,
    const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle,
    const uint8_t *iv, uint32_t iv_len,
    const uint8_t *aad, uint32_t aad_len,
    const uint8_t *plain_text, uint8_t *cipher_text, uint32_t data_len,
    uint8_t *tag, uint32_t tag_len)
{
    crypto_service_preprocess(SYMC_MODULE_ID);
    int32_t ret = drv_cipher_symc_gcm_encrypt((crypto_symc_alg)alg, key, key_len, keyslot_handle,
        iv, iv_len, aad, aad_len, plain_text, cipher_text, data_len, tag, tag_len);
    crypto_service_postprocess(SYMC_MODULE_ID);

    return crypto_sl_common_get_errcode(ret);
}
errcode_t uapi_drv_cipher_symc_gcm_decrypt_verify(uapi_drv_cipher_symc_alg_t alg,
    const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle,
    const uint8_t *iv, uint32_t iv_len,
    const uint8_t *aad, uint32_t aad_len,
    const uint8_t *cipher_text, uint8_t *plain_text, uint32_t data_len,
    const uint8_t *tag, uint32_t tag_len)
{
    crypto_service_preprocess(SYMC_MODULE_ID);
    int32_t ret = drv_cipher_symc_gcm_decrypt_verify((crypto_symc_alg)alg, key, key_len, keyslot_handle,
        iv, iv_len, aad, aad_len, cipher_text, plain_text, data_len, tag, tag_len);
    crypto_service_postprocess(SYMC_MODULE_ID);

    return crypto_sl_common_get_errcode(ret);
}
errcode_t uapi_drv_cipher_symc_gcm_setup(cipher_handle_t *ctx, uapi_drv_cipher_symc_alg_t alg,
    const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle,
    const uint8_t *iv, uint32_t iv_len, bool is_encrypt)
{
    crypto_service_preprocess(SYMC_MODULE_ID);
    int32_t ret = drv_cipher_symc_gcm_setup((void **)ctx, (crypto_symc_alg)alg, key, key_len, keyslot_handle,
        iv, iv_len, is_encrypt);
    if (ret != CRYPTO_SUCCESS) {
        crypto_service_postprocess(SYMC_MODULE_ID);
    }

    return crypto_sl_common_get_errcode(ret);
}
errcode_t uapi_drv_cipher_symc_gcm_update_ad(cipher_handle_t ctx, const uint8_t *aad, uint32_t aad_len)
{
    int32_t ret = drv_cipher_symc_gcm_update_ad((void *)ctx, aad, aad_len);

    return crypto_sl_common_get_errcode(ret);
}
errcode_t uapi_drv_cipher_symc_gcm_update(cipher_handle_t ctx, const uint8_t *src, uint8_t *dst, uint32_t data_len)
{
    int32_t ret = drv_cipher_symc_gcm_update((void *)ctx, src, dst, data_len);

    return crypto_sl_common_get_errcode(ret);
}
errcode_t uapi_drv_cipher_symc_gcm_finish(cipher_handle_t ctx, uint8_t *tag, uint32_t tag_len)
{
    int32_t ret = drv_cipher_symc_gcm_finish((void *)ctx, tag, tag_len);

    return crypto_sl_common_get_errcode(ret);
}
errcode_t uapi_drv_cipher_symc_gcm_teardown(cipher_handle_t ctx)
{
    int32_t ret = drv_cipher_symc_gcm_teardown((void *)ctx);
    crypto_service_postprocess(SYMC_MODULE_ID);

    return crypto_sl_common_get_errcode(ret);
}
#else
errcode_t uapi_drv_cipher_symc_gcm_encrypt(uapi_drv_cipher_symc_alg_t alg,
    const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle,
    const uint8_t *iv, uint32_t iv_len,
    const uint8_t *aad, uint32_t aad_len,
    const uint8_t *plain_text, uint8_t *cipher_text, uint32_t data_len,
    uint8_t *tag, uint32_t tag_len)
{
    crypto_unused(alg);
    crypto_unused(key);
    crypto_unused(key_len);
    crypto_unused(keyslot_handle);
    crypto_unused(iv);
    crypto_unused(iv_len);
    crypto_unused(aad);
    crypto_unused(aad_len);
    crypto_unused(plain_text);
    crypto_unused(cipher_text);
    crypto_unused(data_len);
    crypto_unused(tag);
    crypto_unused(tag_len);
    return ERRCODE_SECURITY_UNSUPPORT;
}
errcode_t uapi_drv_cipher_symc_gcm_decrypt_verify(uapi_drv_cipher_symc_alg_t alg,
    const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle,
    const uint8_t *iv, uint32_t iv_len,
    const uint8_t *aad, uint32_t aad_len,
    const uint8_t *cipher_text, uint8_t *plain_text, uint32_t data_len,
    const uint8_t *tag, uint32_t tag_len)
{
    crypto_unused(alg);
    crypto_unused(key);
    crypto_unused(key_len);
    crypto_unused(keyslot_handle);
    crypto_unused(iv);
    crypto_unused(iv_len);
    crypto_unused(aad);
    crypto_unused(aad_len);
    crypto_unused(cipher_text);
    crypto_unused(plain_text);
    crypto_unused(data_len);
    crypto_unused(tag);
    crypto_unused(tag_len);
    return ERRCODE_SECURITY_UNSUPPORT;
}
errcode_t uapi_drv_cipher_symc_gcm_setup(cipher_handle_t *ctx, uapi_drv_cipher_symc_alg_t alg,
    const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle,
    const uint8_t *iv, uint32_t iv_len, bool is_encrypt)
{
    crypto_unused(ctx);
    crypto_unused(alg);
    crypto_unused(key);
    crypto_unused(key_len);
    crypto_unused(keyslot_handle);
    crypto_unused(iv);
    crypto_unused(iv_len);
    crypto_unused(is_encrypt);

    return ERRCODE_SECURITY_UNSUPPORT;
}
errcode_t uapi_drv_cipher_symc_gcm_update_ad(cipher_handle_t ctx, const uint8_t *aad, uint32_t aad_len)
{
    crypto_unused(ctx);
    crypto_unused(aad);
    crypto_unused(aad_len);

    return ERRCODE_SECURITY_UNSUPPORT;
}
errcode_t uapi_drv_cipher_symc_gcm_update(cipher_handle_t ctx, const uint8_t *src, uint8_t *dst, uint32_t data_len)
{
    crypto_unused(ctx);
    crypto_unused(src);
    crypto_unused(dst);
    crypto_unused(data_len);

    return ERRCODE_SECURITY_UNSUPPORT;
}
errcode_t uapi_drv_cipher_symc_gcm_finish(cipher_handle_t ctx, uint8_t *tag, uint32_t tag_len)
{
    crypto_unused(ctx);
    crypto_unused(tag);
    crypto_unused(tag_len);

    return ERRCODE_SECURITY_UNSUPPORT;
}
errcode_t uapi_drv_cipher_symc_gcm_teardown(cipher_handle_t ctx)
{
    crypto_unused(ctx);

    return ERRCODE_SECURITY_UNSUPPORT;
}
#endif /* CONFIG_SECURITY_UNIFIED_SUPPORT_SYMC */

errcode_t uapi_drv_cipher_symc_dma_copy(uint64_t src_addr, uint64_t dst_addr, uint32_t len)
{
    int32_t ret;
    crypto_service_preprocess(SYMC_MODULE_ID);
    ret = drv_cipher_symc_dma_copy(src_addr, dst_addr, len);
    crypto_service_postprocess(SYMC_MODULE_ID);
    return crypto_sl_common_get_errcode(ret);
}