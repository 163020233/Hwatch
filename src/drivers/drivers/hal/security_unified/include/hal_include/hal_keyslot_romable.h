/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: hal keyslot romable header. \n
 *
 * History: \n
 * 2025-03-08, Create file. \n
 */
#ifndef HAL_KEYSLOT_ROMABLE_H
#define HAL_KEYSLOT_ROMABLE_H

#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

int32_t hal_keyslot_mcipher_lock_romable(uint32_t keyslot_num);

void hal_keyslot_mcipher_unlock_romable(uint32_t keyslot_num);

int32_t hal_keyslot_hmac_lock_romable(uint32_t keyslot_num);

void hal_keyslot_hmac_unlock_romable(uint32_t keyslot_num);

#define KLAD_ENGINE_AES     0x1
#define KLAD_ENGINE_SM4     0x2
#define KLAD_ENGINE_SHA1    0x3
#define KLAD_ENGINE_SHA1    0x3

#define AES_KL_KEY_CFG_VAL          0x30201
#define SM4_KL_KEY_CFG_VAL          0x30501
#define SHA1_KL_KEY_CFG_VAL         0xA01
#define SHA256_KL_KEY_CFG_VAL       0xA11
#define SM3_KL_KEY_CFG_VAL          0xA21
#define FLASH_AES_KL_KEY_CFG_VAL    0x30207

int32_t hal_keyslot_mcipher_send_clear_key_romable(uint32_t keyslot_chn, const uint8_t *key, uint32_t key_len,
    uint32_t kl_key_cfg_val);

int32_t hal_keyslot_flash_send_clear_key_romable(const uint8_t *key, uint32_t key_len);

int32_t hal_keyslot_hmac_send_clear_key_romable(uint32_t keyslot_chn,
    const uint8_t *key, uint32_t key_len,
    uint32_t kl_key_cfg_val, uint32_t hmac_cal_cnt);

enum {
    HAL_MASTER_KEY_SEL_MRK0     = 0x0,
    HAL_MASTER_KEY_SEL_MRK1     = 0x1,

    /* HKM. */
    HAL_MASTER_KEY_SEL_SOC_ROOTKEY      =   0x0,
    HAL_MASTER_KEY_SEL_RESERVED         =   0x1,
    HAL_MASTER_KEY_SEL_OEM_ROOTKEY0     =   0x2,
    HAL_MASTER_KEY_SEL_TP_ROOTKEY       =   0x3,
};

enum {
    HAL_RKP_KEY_SEL_ODRK1       =   8,
    HAL_RKP_KEY_SEL_ABRK_TEE    =   19,
    HAL_RKP_KEY_SEL_ABRK_REE    =   20,
    HAL_RKP_KEY_SEL_RDRK_TEE    =   21,
    HAL_RKP_KEY_SEL_RDRK_REE    =   22,

    /* HKM. */
    HAL_RKP_KEY_SEL_SOC_RDRK_TEE    =   4,
    HAL_RKP_KEY_SEL_SOC_RDRK_REE    =   5,
    HAL_RKP_KEY_SEL_SOC_ERK_TEE     =   6,
    HAL_RKP_KEY_SEL_SOC_ERK_REE     =   7,
    HAL_RKP_KEY_SEL_OEM_ERK1_REE    =   16,
    HAL_RKP_KEY_SEL_OEM_ERK2_REE    =   17,
    HAL_RKP_KEY_SEL_ODRK0_TEE       =   18,
};

enum {
    HAL_RKP_ALG_SEL_SHA256      = 0x0,
    HAL_RKP_ALG_SEL_SHA1        = 0x1,
    HAL_RKP_ALG_SEL_SHA384      = 0x3,
    HAL_RKP_ALG_SEL_SHA512      = 0x4,
    HAL_RKP_ALG_SEL_SM3         = 0x5,
};

#define hal_compat_rkp_cmd_cfg(master_key_sel, key_sel, alg_sel)    \
    (((master_key_sel) << 12) | ((key_sel) << 4) | ((alg_sel) << 1))

enum {
    HAL_KL_RK_CHOOSE_ODRK1      = 0x7,
    HAL_KL_RK_CHOOSE_ABRK_TEE   = 0x14,
    HAL_KL_RK_CHOOSE_ABRK_REE   = 0x15,
    HAL_KL_RK_CHOOSE_RDRK_TEE   = 0x16,
    HAL_KL_RK_CHOOSE_RDRK_REE   = 0x17,

    /* HKM. */
    HAL_KL_RK_CHOOSE_OEM_RK0        = 0,
    HAL_KL_RK_CHOOSE_OEM_RK1        = 1,
    HAL_KL_RK_CHOOSE_OEM_RK2        = 2,
    HAL_KL_RK_CHOOSE_OEM_RK3        = 3,
    HAL_KL_RK_CHOOSE_SOC_RDRK_TEE   = 4,
    HAL_KL_RK_CHOOSE_SOC_RDRK_REE   = 5,
    HAL_KL_RK_CHOOSE_SOC_ERK_TEE    = 6,
    HAL_KL_RK_CHOOSE_SOC_ERK_REE    = 7,
    HAL_KL_RK_CHOOSE_OEM_ERK1_REE   = 16,
    HAL_KL_RK_CHOOSE_OEM_ERK2_REE   = 17,
    HAL_KL_RK_CHOOSE_ODRK_TEE       = 18,
};

#define hal_compat_kl_com_ctrl(rk_choose)   \
    ((rk_choose) << 8)

int32_t hal_keyslot_send_effective_key_romable(uint32_t keyslot_chn, const uint8_t salt[28], uint32_t key_len,
    uint32_t kl_key_cfg_val, uint32_t rkp_cmd_cfg_val, uint32_t kl_com_ctrl_val);

int32_t hal_keyslot_wait_done_romable(void);

void hal_keyslot_debug_romable(void);

typedef int32_t (*keyslot_wait_done)(void);
typedef void (*keyslot_debug_func)(void);
typedef int32_t (*keyslot_try_alloc_chn)(uint32_t *chn_num, uint32_t keyslot_type);
typedef int32_t (*keyslot_free_chn)(uint32_t chn_num, uint32_t keyslot_type);
typedef void (*keyslot_mutex_lock)(void);
typedef void (*keyslot_mutex_unlock)(void);

typedef struct {
    keyslot_wait_done       wait_done_func;
    keyslot_debug_func      debug_func;
    keyslot_try_alloc_chn   try_alloc_chn;
    keyslot_free_chn        free_chn;
    keyslot_mutex_lock      mutex_lock;
    keyslot_mutex_unlock    mutex_unlock;
} hal_keyslot_func_table_t;

void hal_keyslot_register_romable(const hal_keyslot_func_table_t *func_table);

void hal_keyslot_debug_default(void);

void hal_keyslot_mutex_lock_romable(void);

void hal_keyslot_mutex_unlock_romable(void);

int32_t hal_klad_lock_romable(void);

void hal_klad_unlock_romable(void);

int32_t hal_rkp_lock_romable(void);

void hal_rkp_unlock_romable(void);

int32_t hal_rkp_kdf_wait_done_romable(void);

int32_t hal_klad_com_route_wait_done_romable(void);

enum {
    HAL_RKP_DEOB_ALG_SEL_AES    =   0,
    HAL_RKP_DEOB_ALG_SEL_SM4    =   1,
};

enum {
    HAL_RKP_DEOB_KEY_SEL_MRK1   =   0,
    HAL_RKP_DEOB_KEY_SEL_MRK0   =   1,
    HAL_RKP_DEOB_KEY_SEL_RUSK   =   2,
    HAL_RKP_DEOB_KEY_SEL_USK    =   3,

    /* HKM. */
    HAL_RKP_DEOB_KEY_SEL_SOC_ROOTKEY    =   0,
    HAL_RKP_DEOB_KEY_SEL_SOC_RUSK       =   1,
    HAL_RKP_DEOB_KEY_SEL_OEM_ROOTKEY0   =   2,
    HAL_RKP_DEOB_KEY_SEL_TP_ROOTKEY     =   3,
};

#define hal_compat_rkp_deob_cfg(alg_sel, key_sel)   \
    (((alg_sel) << 3) | ((key_sel) << 1))

int32_t hal_rkp_deob_update_romable(uint32_t rkp_deob_cfg_val);

int32_t hal_rkp_deob_wait_done_romable(void);

uint32_t hal_keyslot_get_klad_crc_romable(void);

#define KEYSLOT_TYPE_MCIPHER_VAL        0
#define KEYSLOT_TYPE_HMAC_VAL           1
#define KEYSLOT_TYPE_FLASH_VAL          2
int32_t hal_keyslot_try_alloc_chn_romable(uint32_t *chn_num, uint32_t keyslot_type);

int32_t hal_keyslot_free_chn_romable(uint32_t chn_num, uint32_t keyslot_type);

int32_t hal_keyslot_try_alloc_chn_default(uint32_t *chn_num, uint32_t keyslot_type);

int32_t hal_keyslot_free_chn_default(uint32_t chn_num, uint32_t keyslot_type);

#define PBKDF2_ALG_SEL_SHA1     1
#define PBKDF2_ALG_SEL_SHA256   0
#define PBKDF2_ALG_SEL_SHA384   3
#define PBKDF2_ALG_SEL_SHA512   4
#define PBKDF2_ALG_SEL_SM3      5

void hal_kdf_pbkdf2_set_val_romable(const uint32_t *data, uint32_t data_len);

void hal_kdf_pbkdf2_set_data_romable(const uint32_t *data, uint32_t data_len);

void hal_kdf_pbkdf2_set_key_romable(const uint32_t *key, uint32_t key_len);

void hal_kdf_pbkdf2_clear_key_romable(void);

#define KDF_PBKDF2_VAL_MAX_SIZE_IN_WORD     16
#define KDF_PBKDF2_VAL_MAX_SIZE             (KDF_PBKDF2_VAL_MAX_SIZE_IN_WORD * CRYPTO_WORD_WIDTH)
#define KDF_PBKDF2_KEY_MAX_SIZE_IN_WORD     32
#define KDF_PBKDF2_KEY_MAX_SIZE             (KDF_PBKDF2_KEY_MAX_SIZE_IN_WORD * CRYPTO_WORD_WIDTH)
#define KDF_PBKDF2_DATA_MAX_SIZE_IN_WORD    32
#define KDF_PBKDF2_DATA_MAX_SIZE            (KDF_PBKDF2_DATA_MAX_SIZE_IN_WORD * CRYPTO_WORD_WIDTH)

int32_t hal_kdf_pbkdf2_compute_romable(uint32_t alg, uint16_t iter_count,
    const uint32_t u_state_in[KDF_PBKDF2_DATA_MAX_SIZE_IN_WORD],
    uint32_t u_state_out[KDF_PBKDF2_DATA_MAX_SIZE_IN_WORD], uint32_t result[KDF_PBKDF2_VAL_MAX_SIZE_IN_WORD]);
#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
