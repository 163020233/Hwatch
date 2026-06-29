/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides common utils for security_unified service layer. \n
 *
 * History: \n
 * 2023-03-14, Create file. \n
 */

#include "crypto_errno.h"
#include "crypto_drv_common.h"

#include "security_hmac.h"
#include "security_hash.h"
#if defined(CONFIG_SECURITY_UNIFIED_SUPPORT_HASH)
#include "drv_hash.h"
#endif
#include "cipher.h"
#include "km.h"
#include "sl_common.h"

#define ERRCODE_SECURITY_UNIFIED      0x80001500
#define hash_null_ptr_chk(ptr)   \
    crypto_chk_return((ptr) == TD_NULL, ERRCODE_SECURITY_PARAM_IS_NULL, #ptr" is NULL\r\n")

errcode_t crypto_sl_common_get_errcode(int32_t from_errcode)
{
    errcode_t ret = (uint32_t)from_errcode & 0xFF;
    if ((uint32_t)from_errcode == CRYPTO_SUCCESS) {
        return ERRCODE_SUCC;
    } else if ((uint32_t)from_errcode == CRYPTO_FAILURE) {
        return ERRCODE_FAIL;
    }
    return ERRCODE_SECURITY_UNIFIED + ret;
}

errcode_t crypto_sl_enhance_get_errcode(int32_t from_errcode)
{
    errcode_t ret = (uint32_t)from_errcode & 0xFF;
    if ((uint32_t)from_errcode == CRYPTO_ENHANCE_SUCCESS) {
        return ERRCODE_SUCC;
    } else if ((uint32_t)from_errcode == CRYPTO_ENHANCE_FAILURE) {
        return ERRCODE_FAIL;
    }
    return ERRCODE_SECURITY_UNIFIED + ret;
}

#if defined(CONFIG_SECURITY_UNIFIED_SUPPORT_HASH) || defined(CONFIG_SECURITY_UNIFIED_SUPPORT_SYMC)

#if defined(CONFIG_HASH_SUPPORT_HMAC_SHA1) || defined(CONFIG_HASH_SUPPORT_HMAC_SHA224) || \
    defined(CONFIG_HASH_SUPPORT_HMAC_SHA256) || defined(CONFIG_HASH_SUPPORT_HMAC_SHA2384) || \
    defined(CONFIG_HASH_SUPPORT_HMAC_SHA512) || defined(CONFIG_HASH_SUPPORT_HMAC_SM3)
errcode_t crypto_hmac_start(hmac_handle_t *ctx, uint32_t hash_type,
    const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle)
{
    int32_t ret;
    crypto_service_preprocess(HASH_MODULE_ID);
    ret = drv_cipher_hmac_compute_setup((void **)ctx);
    crypto_chk_goto(ret != ERRCODE_SUCC, exit_post_process, "hmac setup failed\r\n");

    ret = drv_cipher_hmac_compute_start((void *)(*ctx), (crypto_hash_type)hash_type,
        key, key_len, keyslot_handle);
    crypto_chk_goto(ret != ERRCODE_SUCC, error_teardown, "hmac start failed!\r\n");

    return crypto_sl_common_get_errcode(ret);

error_teardown:
    drv_cipher_hmac_compute_teardown((void *)(*ctx));
    *ctx = 0;
exit_post_process:
    crypto_service_postprocess(HASH_MODULE_ID);
    return crypto_sl_common_get_errcode(ret);
}

errcode_t crypto_hmac_update(hmac_handle_t ctx, const uint8_t *buf, uint32_t len)
{
    int32_t ret = drv_cipher_hmac_compute_update((void *)ctx, buf, len);
    return crypto_sl_common_get_errcode(ret);
}

errcode_t crypto_hmac_finish(hmac_handle_t ctx, uint8_t *out, uint32_t out_len)
{
    int32_t ret = drv_cipher_hmac_compute_finish((void *)ctx, out, out_len);
    return crypto_sl_common_get_errcode(ret);
}

errcode_t crypto_hmac_destroy(hmac_handle_t ctx)
{
    int32_t ret = drv_cipher_hmac_compute_teardown((void *)ctx);
    crypto_service_postprocess(HASH_MODULE_ID);
    return crypto_sl_common_get_errcode(ret);
}

errcode_t crypto_hmac(const uint8_t *buf, uint32_t len,
    const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle,
    uint8_t *out, uint32_t out_len, uint32_t hash_type)
{
    crypto_service_preprocess(HASH_MODULE_ID);
    int32_t ret = drv_cipher_hmac_compute((crypto_hash_type)hash_type, key, key_len, keyslot_handle,
        buf, len, out, out_len);
    crypto_service_postprocess(HASH_MODULE_ID);
    return crypto_sl_common_get_errcode(ret);
}
errcode_t uapi_drv_cipher_hmac_destroy(hmac_handle_t ctx)
{
    return crypto_hmac_destroy(ctx);
}
#else /* HMAC_SHA1/HMAC_SHA224/HMAC_SHA256/HMAC_SHA384/HMAC_SHA512/HMAC_SM3 */
errcode_t uapi_drv_cipher_hmac_destroy(hmac_handle_t ctx)
{
    crypto_unused(ctx);
    return ERRCODE_SECURITY_UNSUPPORT;
}
#endif
errcode_t uapi_drv_cipher_sha_destroy(hash_handle_t ctx)
{
    int32_t ret = drv_cipher_hash_compute_teardown((void *)ctx);
    crypto_service_postprocess(HASH_MODULE_ID);
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_hash_compute_clone(hash_handle_t dst_handle, hash_handle_t src_handle)
{
    int32_t ret = drv_cipher_hash_compute_clone((void *)dst_handle, (const void *)src_handle);
    return crypto_sl_common_get_errcode(ret);
}

#else
errcode_t uapi_drv_cipher_sha_destroy(hash_handle_t ctx)
{
    crypto_unused(ctx);
    return ERRCODE_SECURITY_UNSUPPORT;
}

errcode_t uapi_drv_cipher_hash_compute_clone(hash_handle_t dst_handle, hash_handle_t src_handle)
{
    crypto_unused(dst_handle);
    crypto_unused(src_handle);
    return ERRCODE_SECURITY_UNSUPPORT;
}
#endif /* CONFIG_SECURITY_UNIFIED_SUPPORT_HASH || CONFIG_SECURITY_UNIFIED_SUPPORT_SYMC */

