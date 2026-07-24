/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: drv keyslot implement.
 *
 * Create: 2025-06-11
*/
#include "drv_keyslot.h"
#include "hal_keyslot.h"
#include "hal_keyslot_romable.h"

#include "crypto_drv_common.h"

#define FILE_ID FILE_ID_DRV_CODE_KM_DRV_KEYSLOT

#define KEYSLOT_TYPE_MCIPHER_MASK   0x0
#define KEYSLOT_TYPE_HMAC_MASK      0x1
#define KEYSLOT_TYPE_FLASH_MASK     0x2

#define KEYSLOT_MAGIC_BYTE              0xA
#define KEYSLOT_MAGIC_BYTE_SHIFT_BIT    28
#define KEYSLOT_MAGIC_MASK              0xF
#define KEYSLOT_TYPE_SHIFT_BIT          24
#define KEYSLOT_TYPE_MASK               0xF
#define KEYSLOT_NUM_MASK                0xFF

/* keyslot_handle = |---- Magic Byte(4 bit) 0xA ----|---- keyslot_type(4 bit) ----
    |---- reserved(16 bit) ----|---- keyslot_num(8 bit) ----| */
static uint32_t inner_compat_keyslot_handle(uint32_t keyslot_type_mask, uint32_t keyslot_num)
{
    return (((KEYSLOT_MAGIC_BYTE & KEYSLOT_TYPE_MASK) << KEYSLOT_MAGIC_BYTE_SHIFT_BIT) |   \
        ((keyslot_type_mask & KEYSLOT_TYPE_MASK) << KEYSLOT_TYPE_SHIFT_BIT) | keyslot_num);
}

static uint32_t inner_get_keyslot_magic_byte(uint32_t keyslot_handle)
{
    return (keyslot_handle >> KEYSLOT_MAGIC_BYTE_SHIFT_BIT) & KEYSLOT_MAGIC_MASK;
}

static uint32_t inner_get_keyslot_type(uint32_t keyslot_handle)
{
    return (keyslot_handle >> KEYSLOT_TYPE_SHIFT_BIT) & KEYSLOT_TYPE_MASK;
}

static uint32_t inner_get_keyslot_num(uint32_t keyslot_handle)
{
    return (keyslot_handle & KEYSLOT_NUM_MASK);
}

int32_t drv_keyslot_setup(uint32_t *keyslot_handle, crypto_keyslot_type keyslot_type)
{
    int32_t ret;
    uint32_t keyslot_num;

    drv_err_chk_return(keyslot_handle == NULL, ERROR_PARAM_IS_NULL);
    drv_err_chk_return(keyslot_type != CRYPTO_KEYSLOT_TYPE_MCIPHER && keyslot_type != CRYPTO_KEYSLOT_TYPE_HMAC &&
        keyslot_type != CRYPTO_KEYSLOT_TYPE_FLASH, ERROR_INVALID_PARAM);
    ret = hal_keyslot_try_alloc_chn_romable(&keyslot_num, (uint32_t)keyslot_type);
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret);

    *keyslot_handle = inner_compat_keyslot_handle((uint32_t)keyslot_type, keyslot_num);
    return CRYPTO_SUCCESS;
}

int32_t drv_keyslot_teardown(uint32_t keyslot_handle)
{
    uint32_t keyslot_type = inner_get_keyslot_type(keyslot_handle);
    uint32_t keyslot_num = inner_get_keyslot_num(keyslot_handle);
    drv_err_chk_return(inner_get_keyslot_magic_byte(keyslot_handle) != KEYSLOT_MAGIC_BYTE, ERROR_INVALID_HANDLE);

    return hal_keyslot_free_chn_romable(keyslot_num, keyslot_type);
}

#if defined(CONFIG_HASH_SUPPORT_SHA384) || defined(CONFIG_HASH_SUPPORT_SHA512)
#define HMAC_KEY_CAL_CNT            8
#else
#define HMAC_KEY_CAL_CNT            4
#endif

int32_t drv_keyslot_set_clear_key(uint32_t keyslot_handle, crypto_klad_engine klad_engine,
    const uint8_t *key, uint32_t key_len)
{
    uint32_t keyslot_num = inner_get_keyslot_num(keyslot_handle);
    uint32_t keyslot_type = inner_get_keyslot_type(keyslot_handle);
    uint32_t kl_key_cfg_val;

    drv_err_chk_return(key == NULL, ERROR_PARAM_IS_NULL);
    drv_err_chk_return(inner_get_keyslot_magic_byte(keyslot_handle) != KEYSLOT_MAGIC_BYTE, ERROR_INVALID_HANDLE);

    if (keyslot_type == KEYSLOT_TYPE_MCIPHER_MASK ||
        keyslot_type == KEYSLOT_TYPE_FLASH_MASK) {
        drv_err_chk_return(key_len != CRYPTO_128_KEY_LEN && key_len != CRYPTO_192_KEY_LEN &&
            key_len != CRYPTO_256_KEY_LEN, ERROR_INVALID_PARAM);
    } else if (keyslot_type == KEYSLOT_TYPE_HMAC_MASK) {
        drv_err_chk_return(key_len > CRYPTO_HASH_KEY_MAX_LENGTH, ERROR_INVALID_PARAM);
    }

    if (keyslot_type == KEYSLOT_TYPE_MCIPHER_MASK) {
        if (klad_engine == CRYPTO_KLAD_ENGINE_AES) {
            kl_key_cfg_val = AES_KL_KEY_CFG_VAL;
        } else if (klad_engine == CRYPTO_KLAD_ENGINE_SM4) {
            kl_key_cfg_val = SM4_KL_KEY_CFG_VAL;
        } else {

            drv_set_err(ERROR_INVALID_PARAM, "direct error return");
            return ERROR_INVALID_PARAM;
        }
        return hal_keyslot_mcipher_send_clear_key_romable(keyslot_num, key, key_len, kl_key_cfg_val);
    } else if (keyslot_type == KEYSLOT_TYPE_HMAC_MASK) {
        if (klad_engine == CRYPTO_KLAD_ENGINE_SHA1_HMAC) {
            kl_key_cfg_val = SHA1_KL_KEY_CFG_VAL;
        } else if (klad_engine == CRYPTO_KLAD_ENGINE_SHA2_HMAC) {
            kl_key_cfg_val = SHA256_KL_KEY_CFG_VAL;
        } else if (klad_engine == CRYPTO_KLAD_ENGINE_SM3_HMAC) {
            kl_key_cfg_val = SM3_KL_KEY_CFG_VAL;
        } else {

            drv_set_err(ERROR_INVALID_PARAM, "direct error return");
            return ERROR_INVALID_PARAM;
        }
        return hal_keyslot_hmac_send_clear_key_romable(keyslot_num, key, key_len, kl_key_cfg_val, HMAC_KEY_CAL_CNT);
    } else if (keyslot_type == KEYSLOT_TYPE_FLASH_MASK) {
        return hal_keyslot_flash_send_clear_key_romable(key, key_len);
    }


    drv_set_err(ERROR_INVALID_PARAM, "direct error return");
    return ERROR_INVALID_PARAM;
}

int32_t drv_keyslot_set_hard_key(uint32_t keyslot_handle,
    crypto_keyslot_hard_key_type hard_key_type, crypto_klad_engine klad_engine,
    const uint8_t *salt, uint32_t salt_len, uint32_t key_len)
{
    int32_t ret;
    uint32_t kl_key_cfg_val = AES_KL_KEY_CFG_VAL;
    uint32_t rkp_cmd_cfg_val;
    uint32_t kl_com_ctrl_val;
    uint32_t keyslot_num = inner_get_keyslot_num(keyslot_handle);
    uint32_t keyslot_type = inner_get_keyslot_type(keyslot_handle);
    uint8_t salt_padding[28] = { 0 };   // 28: salt len

    drv_err_chk_return(salt == NULL, ERROR_PARAM_IS_NULL);
    drv_err_chk_return(salt_len > sizeof(salt_padding), ERROR_INVALID_PARAM);
    drv_err_chk_return(key_len != CRYPTO_128_KEY_LEN && key_len != CRYPTO_192_KEY_LEN &&
        key_len != CRYPTO_256_KEY_LEN, ERROR_INVALID_PARAM);
    drv_err_chk_return(inner_get_keyslot_magic_byte(keyslot_handle) != KEYSLOT_MAGIC_BYTE, ERROR_INVALID_HANDLE);

    ret = memcpy_s(salt_padding, sizeof(salt_padding), salt, salt_len);
    drv_err_chk_return(ret != EOK, ERROR_MEMCPY_S);

    ret = crypto_km_common_get_hard_key_type_item(hard_key_type, &rkp_cmd_cfg_val, &kl_com_ctrl_val);
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret);

    if (keyslot_type == KEYSLOT_TYPE_MCIPHER_MASK) {
        if (klad_engine == CRYPTO_KLAD_ENGINE_AES) {
            kl_key_cfg_val = AES_KL_KEY_CFG_VAL;
        } else if (klad_engine == CRYPTO_KLAD_ENGINE_SM4) {
            kl_key_cfg_val = SM4_KL_KEY_CFG_VAL;
        } else {

            drv_set_err(ERROR_INVALID_PARAM, "direct error return");
            return ERROR_INVALID_PARAM;
        }
    } else if (keyslot_type == KEYSLOT_TYPE_HMAC_MASK) {
        if (klad_engine == CRYPTO_KLAD_ENGINE_SHA2_HMAC) {
            kl_key_cfg_val = SHA256_KL_KEY_CFG_VAL;
        } else if (klad_engine == CRYPTO_KLAD_ENGINE_SM3_HMAC) {
            kl_key_cfg_val = SM3_KL_KEY_CFG_VAL;
        } else {

            drv_set_err(ERROR_INVALID_PARAM, "direct error return");
            return ERROR_INVALID_PARAM;
        }
    } else if (keyslot_type == KEYSLOT_TYPE_FLASH_MASK) {
        if ((hard_key_type != CRYPTO_KEYSLOT_HARD_KEY_TYPE_ODRK_TEE) &&
            (hard_key_type != CRYPTO_KEYSLOT_HARD_KEY_TYPE_ODRK_REE)) {

            drv_set_err(ERROR_INVALID_PARAM, "direct error return");
            return ERROR_INVALID_PARAM;
        }
        keyslot_num = 0;
        kl_key_cfg_val = FLASH_AES_KL_KEY_CFG_VAL;
    } else {

        drv_set_err(ERROR_INVALID_PARAM, "direct error return");
        return ERROR_INVALID_PARAM;
    }

    return hal_keyslot_send_effective_key_romable(keyslot_num, salt_padding, key_len, kl_key_cfg_val,
        rkp_cmd_cfg_val, kl_com_ctrl_val);
}
