/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: hmac-sm3 simple API function implementation.
 *
 * Create: 2024-07-04
*/

#include "crypto_drv_common.h"
#include "sl_common.h"
#include "cipher.h"
#include "km.h"
#include "security_hmac.h"

#ifdef CONFIG_HASH_SUPPORT_HMAC_SM3
errcode_t uapi_drv_cipher_hmac_sm3_start(hmac_handle_t *ctx, const uint8_t *key, uint32_t key_len,
    uint32_t keyslot_handle)
{
    return crypto_hmac_start(ctx, UAPI_DRV_CIPHER_HASH_TYPE_HMAC_SM3, key, key_len, keyslot_handle);
}

errcode_t uapi_drv_cipher_hmac_sm3_update(hmac_handle_t ctx, const uint8_t *buf, uint32_t len)
{
    return crypto_hmac_update(ctx, buf, len);
}
errcode_t uapi_drv_cipher_hmac_sm3_finish(hmac_handle_t ctx, uint8_t out[32])
{
    return crypto_hmac_finish(ctx, out, HASH_SM3_RESULT_SIZE);
}

errcode_t uapi_drv_cipher_hmac_sm3(const uint8_t *buf, uint32_t len,
    const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle,
    uint8_t out[32])
{
    return crypto_hmac(buf, len, key, key_len, keyslot_handle, out,
        HASH_SM3_RESULT_SIZE, UAPI_DRV_CIPHER_HASH_TYPE_HMAC_SM3);
}
#else
errcode_t uapi_drv_cipher_hmac_sm3_start(hmac_handle_t *ctx, const uint8_t *key, uint32_t key_len,
    uint32_t keyslot_handle)
{
    crypto_unused(ctx);
    crypto_unused(key);
    crypto_unused(key_len);
    crypto_unused(keyslot_handle);
    return ERRCODE_SECURITY_UNSUPPORT;
}

errcode_t uapi_drv_cipher_hmac_sm3_update(hmac_handle_t ctx, const uint8_t *buf, uint32_t len)
{
    crypto_unused(ctx);
    crypto_unused(buf);
    crypto_unused(len);
    return ERRCODE_SECURITY_UNSUPPORT;
}
errcode_t uapi_drv_cipher_hmac_sm3_finish(hmac_handle_t ctx, uint8_t out[32])
{
    crypto_unused(ctx);
    crypto_unused(out);
    return ERRCODE_SECURITY_UNSUPPORT;
}

errcode_t uapi_drv_cipher_hmac_sm3(const uint8_t *buf, uint32_t len,
    const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle,
    uint8_t out[32])
{
    crypto_unused(buf);
    crypto_unused(len);
    crypto_unused(key);
    crypto_unused(key_len);
    crypto_unused(keyslot_handle);
    crypto_unused(out);
    return ERRCODE_SECURITY_UNSUPPORT;
}
#endif /* CONFIG_HASH_SUPPORT_HMAC_SM3 */