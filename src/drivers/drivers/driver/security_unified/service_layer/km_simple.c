/**RSA
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: km service header file.
 *
 * Create: 2025-06-11
 */

#include "security_km.h"
#include "drv_keyslot.h"
#include "crypto_drv_common.h"
#include "sl_common.h"

#define FILE_ID FILE_ID_SERVICE_LAYER_KM_SIMPLE

errcode_t uapi_drv_keyslot_setup(uint32_t *keyslot_handle, uapi_drv_keyslot_type_t keyslot_type)
{
    int32_t ret;
    crypto_service_preprocess(KM_MODULE_ID);
    ret = drv_keyslot_setup(keyslot_handle, (crypto_keyslot_type)keyslot_type);
    if (ret != TD_SUCCESS) {
        crypto_service_postprocess(KM_MODULE_ID);
    }
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_keyslot_teardown(uint32_t keyslot_handle)
{
    int32_t ret = drv_keyslot_teardown(keyslot_handle);
    crypto_service_postprocess(KM_MODULE_ID);
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_keyslot_set_clear_key(uint32_t keyslot_handle, uapi_drv_klad_engine_t klad_engine,
    const uint8_t *key, uint32_t key_len)
{
    int32_t ret;
    crypto_service_preprocess(SPACC_KM_MODULE_ID);
    ret = drv_keyslot_set_clear_key(keyslot_handle, (crypto_klad_engine)klad_engine, key, key_len);
    crypto_service_postprocess(SPACC_KM_MODULE_ID);
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_keyslot_set_hard_key(uint32_t keyslot_handle,
    uapi_drv_keyslot_hard_key_type_t hard_key_type, uapi_drv_klad_engine_t klad_engine,
    const uint8_t *salt, uint32_t salt_len, uint32_t key_len)
{
    int32_t ret;
    crypto_service_preprocess(SPACC_KM_MODULE_ID);
    ret = drv_keyslot_set_hard_key(keyslot_handle,
        (crypto_keyslot_hard_key_type)hard_key_type, (crypto_klad_engine)klad_engine, salt, salt_len, key_len);
    crypto_service_postprocess(SPACC_KM_MODULE_ID);
    return crypto_sl_common_get_errcode(ret);
}