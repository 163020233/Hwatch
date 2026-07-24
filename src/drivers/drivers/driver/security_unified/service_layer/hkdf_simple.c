/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: Provides cipher driver source \n
 *
 */
#include "cipher.h"
#include "drv_hash.h"
#include "sl_common.h"
#include "crypto_drv_common.h"
#include "security_hash_pri.h"

#define FILE_ID FILE_ID_SERVICE_LAYER_HKDF_SIMPLE

errcode_t uapi_drv_cipher_pbkdf2(const uapi_drv_cipher_kdf_pbkdf2_param_t *param, uint8_t *out, const uint32_t out_len)
{
#ifdef CONFIG_SECURITY_UNIFIED_SUPPORT_PBKDF2
    int32_t ret;
    crypto_service_preprocess(HASH_MODULE_ID);
    ret = drv_cipher_pbkdf2((const crypto_kdf_pbkdf2_param *)param, out, out_len);
    crypto_service_postprocess(HASH_MODULE_ID);
    return crypto_sl_common_get_errcode(ret);
#else
    crypto_unused(param);
    crypto_unused(out);
    crypto_unused(out_len);
    return ERRCODE_SECURITY_UNSUPPORT;
#endif
}

errcode_t uapi_drv_cipher_pbkdf2_ex(const uapi_drv_cipher_kdf_pbkdf2_ex_param_t *param,
    uint8_t *out, const uint32_t out_len)
{
#ifdef CONFIG_SECURITY_UNIFIED_SUPPORT_PBKDF2_EX
    int32_t ret;
    crypto_service_preprocess(HASH_MODULE_ID);
    ret = drv_cipher_pbkdf2_ex((const crypto_kdf_pbkdf2_ex_param *)param, out, out_len);
    crypto_service_postprocess(HASH_MODULE_ID);
    return crypto_sl_common_get_errcode(ret);
#else
    crypto_unused(param);
    crypto_unused(out);
    crypto_unused(out_len);
    return ERRCODE_SECURITY_UNSUPPORT;
#endif
}

errcode_t uapi_drv_cipher_hkdf_extract(uapi_drv_cipher_hkdf_extract_t *extract_param,
    uint8_t *prk, uint32_t *prk_length)
{
#ifdef CONFIG_HASH_SUPPORT_HKDF
    crypto_service_preprocess(HASH_MODULE_ID);
    int32_t ret = drv_cipher_hkdf_extract((crypto_hkdf_extract_t *)extract_param, prk, prk_length);
    crypto_service_postprocess(HASH_MODULE_ID);
    return crypto_sl_common_get_errcode(ret);
#else
    crypto_unused(extract_param);
    crypto_unused(prk);
    crypto_unused(prk_length);
    return ERRCODE_SECURITY_UNSUPPORT;
#endif
}

errcode_t uapi_drv_cipher_hkdf_expand(const uapi_drv_cipher_hkdf_expand_t *expand_param,
    uint8_t *okm, uint32_t okm_length)
{
#ifdef CONFIG_HASH_SUPPORT_HKDF
    crypto_service_preprocess(HASH_MODULE_ID);
    int32_t ret = drv_cipher_hkdf_expand((crypto_hkdf_expand_t *)expand_param, okm, okm_length);
    crypto_service_postprocess(HASH_MODULE_ID);
    return crypto_sl_common_get_errcode(ret);
#else
    crypto_unused(expand_param);
    crypto_unused(okm);
    crypto_unused(okm_length);
    return ERRCODE_SECURITY_UNSUPPORT;
#endif
}

errcode_t uapi_drv_cipher_hkdf(uapi_drv_cipher_hkdf_t *hkdf_param, uint8_t *okm, uint32_t okm_length)
{
#ifdef CONFIG_HASH_SUPPORT_HKDF
    crypto_service_preprocess(HASH_MODULE_ID);
    int32_t ret = drv_cipher_hkdf((crypto_hkdf_t *)hkdf_param, okm, okm_length);
    crypto_service_postprocess(HASH_MODULE_ID);
    return crypto_sl_common_get_errcode(ret);
#else
    crypto_unused(hkdf_param);
    crypto_unused(okm);
    crypto_unused(okm_length);
    return ERRCODE_SECURITY_UNSUPPORT;
#endif
}