/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: drvbox call source for SELiteOS.
 */

#include "security_hash.h"
#include "cipher.h"
#include "security_symc.h"
#include "crypto_osal_lib.h"
#include "unified_cipher_pke.h"
#include "trng.h"
#include "chips_porting.h"
#include "pmp_mgr.h"
#include "los_base.h"
#include "drv_pke.h"
#include "los_drivermgr.h"

#define SECURITY_UNIFIED_DRV_ID 2
#define LITEOS_DRVBOX_CALL_DEF(name, n)             (uintptr_t)(name),

const uintptr_t g_security_unified[] = {
    #include "drvbox_calls_def.h"
};

void crypto_switch_drvbox(void)
{
    if (!LOS_IN_KERNEL) {
        (void)LOS_DrvmgrSwitch2Drvbox(SECURITY_UNIFIED_DRV_ID);
    }
}

void crypto_switch_usrbox(void)
{
    if (!LOS_IN_KERNEL) {
        (void)LOS_DrvmgrSwitch2Usrbox();
    }
}

errcode_t drvbox_uapi_drv_cipher_symc_crypt(uint32_t alg, const uint8_t *src, uint8_t *dst, uint32_t data_len,
    uint8_t iv[16], const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle, bool is_encrypt)
{
    errcode_t ret = ERRCODE_FAIL;
    int res = LOS_FAILURE;

    res = check_data_range_valid((uintptr_t)src, data_len);
    los_chk_return(res != LOS_SUCCESS, ERRCODE_SECURITY_MEMORY_ACCESS, "src is invalid\n");
    res = check_data_range_valid((uintptr_t)dst, data_len);
    los_chk_return(res != LOS_SUCCESS, ERRCODE_SECURITY_MEMORY_ACCESS, "dst is invalid\n");

    crypto_switch_drvbox();
    ret = uapi_drv_cipher_symc_crypt(alg, src, dst, data_len, iv, key, key_len, keyslot_handle, is_encrypt);
    crypto_switch_usrbox();

    return ret;
}

errcode_t drvbox_uapi_drv_cipher_symc_gcm_encrypt(uapi_drv_cipher_symc_alg_t alg,
    const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle,
    const uint8_t *iv, uint32_t iv_len,
    const uint8_t *aad, uint32_t aad_len,
    const uint8_t *plain_text, uint8_t *cipher_text, uint32_t data_len,
    uint8_t *tag, uint32_t tag_len)
{
    errcode_t ret = ERRCODE_FAIL;
    int res = LOS_FAILURE;

    res = check_data_range_valid((uintptr_t)plain_text, data_len);
    los_chk_return(res != LOS_SUCCESS, ERRCODE_SECURITY_MEMORY_ACCESS, "plain_text is invalid\n");
    res = check_data_range_valid((uintptr_t)cipher_text, data_len);
    los_chk_return(res != LOS_SUCCESS, ERRCODE_SECURITY_MEMORY_ACCESS, "cipher_text is invalid\n");

    crypto_switch_drvbox();
    ret = uapi_drv_cipher_symc_gcm_encrypt(alg, key, key_len, keyslot_handle, iv, iv_len, aad, aad_len,
        plain_text, cipher_text, data_len, tag, tag_len);
    crypto_switch_usrbox();

    return ret;
}

errcode_t drvbox_uapi_drv_cipher_symc_gcm_decrypt_verify(uapi_drv_cipher_symc_alg_t alg,
    const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle,
    const uint8_t *iv, uint32_t iv_len,
    const uint8_t *aad, uint32_t aad_len,
    const uint8_t *cipher_text, uint8_t *plain_text, uint32_t data_len,
    const uint8_t *tag, uint32_t tag_len)
{
    errcode_t ret = ERRCODE_FAIL;
    int res = LOS_FAILURE;

    res = check_data_range_valid((uintptr_t)cipher_text, data_len);
    los_chk_return(res != LOS_SUCCESS, ERRCODE_SECURITY_MEMORY_ACCESS, "cipher_text is invalid\n");
    res = check_data_range_valid((uintptr_t)plain_text, data_len);
    los_chk_return(res != LOS_SUCCESS, ERRCODE_SECURITY_MEMORY_ACCESS, "plain_text is invalid\n");

    crypto_switch_drvbox();
    ret = uapi_drv_cipher_symc_gcm_decrypt_verify(alg, key, key_len, keyslot_handle, iv, iv_len, aad, aad_len,
        cipher_text, plain_text, data_len, tag, tag_len);
    crypto_switch_usrbox();

    return ret;
}
