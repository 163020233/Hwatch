/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: Provides cipher simple driver source \n
 *
 * History: \n
 * 2024-11-14, Create file. \n
 */

#include "sl_common.h"
#include "crypto_drv_common.h"

#include "security_mac.h"
#include "drv_symc.h"

#ifdef CONFIG_SYMC_SUPPORT_CBC_MAC
errcode_t uapi_drv_cipher_cbc_mac(uapi_drv_cipher_symc_alg_t alg,
    const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle,
    const uint8_t *data, uint32_t data_len, uint8_t out_mac[16])
{
    crypto_service_preprocess(SYMC_MODULE_ID);
    int32_t ret = drv_symc_mac_compute((crypto_symc_alg)alg, CRYPTO_SYMC_WORK_MODE_CBC_MAC,
        key, key_len, keyslot_handle, data, data_len, out_mac);
    crypto_service_postprocess(SYMC_MODULE_ID);

    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_cbc_mac_start(mac_handle_t *ctx,
    uapi_drv_cipher_symc_alg_t alg, const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle)
{
    crypto_service_preprocess(SYMC_MODULE_ID);
    int32_t ret = drv_symc_mac_start((void **)ctx, (crypto_symc_alg)alg, CRYPTO_SYMC_WORK_MODE_CBC_MAC,
        key, key_len, keyslot_handle);
    if (ret != CRYPTO_SUCCESS) {
        crypto_service_postprocess(SYMC_MODULE_ID);
    }
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_cbc_mac_update(mac_handle_t ctx, const uint8_t *data, uint32_t data_len)
{
    int32_t ret = drv_symc_mac_update((void *)ctx, data, data_len);
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_cbc_mac_finish(mac_handle_t ctx, uint8_t out_mac[16])
{
    int32_t ret = drv_symc_mac_finish((void *)ctx, out_mac);
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_cbc_mac_destroy(mac_handle_t ctx)
{
    int32_t ret = drv_symc_mac_destroy((void *)ctx);
    crypto_service_postprocess(SYMC_MODULE_ID);

    return crypto_sl_common_get_errcode(ret);
}
#else
errcode_t uapi_drv_cipher_cbc_mac(uapi_drv_cipher_symc_alg_t alg,
    const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle,
    const uint8_t *data, uint32_t data_len, uint8_t out_mac[16])
{
    crypto_unused(alg);
    crypto_unused(key);
    crypto_unused(key_len);
    crypto_unused(keyslot_handle);
    crypto_unused(data);
    crypto_unused(data_len);
    crypto_unused(out_mac);

    return ERRCODE_SECURITY_UNSUPPORT;
}

errcode_t uapi_drv_cipher_cbc_mac_start(mac_handle_t *ctx,
    uapi_drv_cipher_symc_alg_t alg, const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle)
{
    crypto_unused(ctx);
    crypto_unused(alg);
    crypto_unused(key);
    crypto_unused(key_len);
    crypto_unused(keyslot_handle);

    return ERRCODE_SECURITY_UNSUPPORT;
}

errcode_t uapi_drv_cipher_cbc_mac_update(mac_handle_t ctx, const uint8_t *data, uint32_t data_len)
{
    crypto_unused(ctx);
    crypto_unused(data);
    crypto_unused(data_len);

    return ERRCODE_SECURITY_UNSUPPORT;
}

errcode_t uapi_drv_cipher_cbc_mac_finish(mac_handle_t ctx, uint8_t out_mac[16])
{
    crypto_unused(ctx);
    crypto_unused(out_mac);

    return ERRCODE_SECURITY_UNSUPPORT;
}

errcode_t uapi_drv_cipher_cbc_mac_destroy(mac_handle_t ctx)
{
    crypto_unused(ctx);

    return ERRCODE_SECURITY_UNSUPPORT;
}
#endif

#ifdef CONFIG_SYMC_SUPPORT_CMAC
errcode_t uapi_drv_cipher_cmac(uapi_drv_cipher_symc_alg_t alg,
    const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle,
    const uint8_t *data, uint32_t data_len, uint8_t out_mac[16])
{
    crypto_service_preprocess(SYMC_MODULE_ID);
    int32_t ret = drv_symc_mac_compute((crypto_symc_alg)alg, CRYPTO_SYMC_WORK_MODE_CMAC,
        key, key_len, keyslot_handle, data, data_len, out_mac);
    crypto_service_postprocess(SYMC_MODULE_ID);

    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_cmac_ctx_init(mac_handle_t ctx, uint32_t ctx_size,
    uapi_drv_cipher_symc_alg_t alg, const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle)
{
    crypto_service_preprocess(SYMC_MODULE_ID);
    int32_t ret = drv_symc_mac_ctx_init((void *)ctx, ctx_size, (crypto_symc_alg)alg,
        CRYPTO_SYMC_WORK_MODE_CMAC, key, key_len, keyslot_handle);
    if (ret != CRYPTO_SUCCESS) {
        crypto_service_postprocess(SYMC_MODULE_ID);
    }

    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_cmac_ctx_deinit(mac_handle_t ctx, uint32_t ctx_size)
{
    int32_t ret;
    crypto_service_postprocess(SYMC_MODULE_ID);
    crypto_chk_goto_with_ret(ret, ctx == 0, exit, ERROR_PARAM_IS_NULL, "ctx is null\n");
    (void)memset_s((void *)ctx, ctx_size, 0, ctx_size);
    ret = 0;
exit:
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_cmac_start(mac_handle_t *ctx,
    uapi_drv_cipher_symc_alg_t alg, const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle)
{
    crypto_service_preprocess(SYMC_MODULE_ID);
    int32_t ret = drv_symc_mac_start((void **)ctx, (crypto_symc_alg)alg, CRYPTO_SYMC_WORK_MODE_CMAC,
        key, key_len, keyslot_handle);
    if (ret != CRYPTO_SUCCESS) {
        crypto_service_postprocess(SYMC_MODULE_ID);
    }

    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_cmac_update(mac_handle_t ctx, const uint8_t *data, uint32_t data_len)
{
    int32_t ret = drv_symc_mac_update((void *)ctx, data, data_len);
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_cmac_finish(mac_handle_t ctx, uint8_t out_mac[16])
{
    int32_t ret = drv_symc_mac_finish((void *)ctx, out_mac);
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_cmac_destroy(mac_handle_t ctx)
{
    int32_t ret = drv_symc_mac_destroy((void *)ctx);
    crypto_service_postprocess(SYMC_MODULE_ID);

    return crypto_sl_common_get_errcode(ret);
}
#else
errcode_t uapi_drv_cipher_cmac(uapi_drv_cipher_symc_alg_t alg,
    const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle,
    const uint8_t *data, uint32_t data_len, uint8_t out_mac[16])
{
    crypto_unused(alg);
    crypto_unused(key);
    crypto_unused(key_len);
    crypto_unused(keyslot_handle);
    crypto_unused(data);
    crypto_unused(data_len);
    crypto_unused(out_mac);

    return ERRCODE_SECURITY_UNSUPPORT;
}

errcode_t uapi_drv_cipher_cmac_ctx_init(mac_handle_t ctx, uint32_t ctx_size,
    uapi_drv_cipher_symc_alg_t alg, const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle)
{
    crypto_unused(ctx);
    crypto_unused(alg);
    crypto_unused(key);
    crypto_unused(ctx_size);
    crypto_unused(key_len);
    crypto_unused(keyslot_handle);

    return ERRCODE_SECURITY_UNSUPPORT;
}

errcode_t uapi_drv_cipher_cmac_start(mac_handle_t *ctx,
    uapi_drv_cipher_symc_alg_t alg, const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle)
{
    crypto_unused(ctx);
    crypto_unused(alg);
    crypto_unused(key);
    crypto_unused(key_len);
    crypto_unused(keyslot_handle);

    return ERRCODE_SECURITY_UNSUPPORT;
}

errcode_t uapi_drv_cipher_cmac_update(mac_handle_t ctx, const uint8_t *data, uint32_t data_len)
{
    crypto_unused(ctx);
    crypto_unused(data);
    crypto_unused(data_len);

    return ERRCODE_SECURITY_UNSUPPORT;
}

errcode_t uapi_drv_cipher_cmac_finish(mac_handle_t ctx, uint8_t out_mac[16])
{
    crypto_unused(ctx);
    crypto_unused(out_mac);

    return ERRCODE_SECURITY_UNSUPPORT;
}

errcode_t uapi_drv_cipher_cmac_destroy(mac_handle_t ctx)
{
    crypto_unused(ctx);

    return ERRCODE_SECURITY_UNSUPPORT;
}
#endif