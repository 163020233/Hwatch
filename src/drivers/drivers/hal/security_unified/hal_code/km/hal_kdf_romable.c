/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: hal keyslot romable. \n
 *
 */
#include "hal_keyslot_romable.h"
#include "hal_rkp_reg.h"

#include "crypto_drv_common.h"

#define FILE_ID FILE_ID_HAL_CODE_KM_HAL_KDF_ROMABLE

#define KDF_SW_GEN                          3

void hal_kdf_pbkdf2_set_val_romable(const uint32_t *data, uint32_t data_len)
{
    uint32_t i;
    for (i = 0; i < data_len / CRYPTO_WORD_WIDTH; i++) {
        km_reg_write(RKP_PBKDF2_VAL(i), data[i]);
    }
}

void hal_kdf_pbkdf2_clear_key_romable(void)
{
    uint32_t i;
    for (i = 0; i < KDF_PBKDF2_KEY_MAX_SIZE_IN_WORD; i++) {
        km_reg_write(RKP_PBKDF2_KEY(i), 0);
    }
}

void hal_kdf_pbkdf2_set_key_romable(const uint32_t *key, uint32_t key_len)
{
    uint32_t i;
    for (i = 0; i < key_len / CRYPTO_WORD_WIDTH; i++) {
        km_reg_write(RKP_PBKDF2_KEY(i), key[i]);
    }
}

#define KDF_KEY_BLOCK_SIZE_512_IN_WORD      16
#define KDF_KEY_BLOCK_SIZE_1024_IN_WORD     32
int32_t hal_kdf_pbkdf2_compute_romable(uint32_t alg, uint16_t iter_count,
    const uint32_t u_state_in[KDF_PBKDF2_DATA_MAX_SIZE_IN_WORD],
    uint32_t u_state_out[KDF_PBKDF2_DATA_MAX_SIZE_IN_WORD], uint32_t result[KDF_PBKDF2_VAL_MAX_SIZE_IN_WORD])
{
    int32_t ret;
    uint32_t i;
    uint32_t block_size_in_word = KDF_KEY_BLOCK_SIZE_512_IN_WORD;
    rkp_cmd_cfg cfgval = {0};

    if (iter_count == 0) {
        return CRYPTO_SUCCESS;
    }

    if (alg == PBKDF2_ALG_SEL_SHA384 || alg == PBKDF2_ALG_SEL_SHA512) {
        block_size_in_word = KDF_KEY_BLOCK_SIZE_1024_IN_WORD;
    }

    for (i = 0; i < block_size_in_word; i++) {
        km_reg_write(RKP_PBKDF2_DATA(i), u_state_in[i]);
    }

    cfgval.u32 = km_reg_read(RKP_CMD_CFG);
    cfgval.bits.rkp_pbkdf_calc_time = iter_count;
    cfgval.bits.pbkdf2_alg_sel_cfg = alg;
    cfgval.bits.pbkdf2_key_sel_cfg = KDF_SW_GEN;
    cfgval.bits.sw_calc_req = 0x1;         /* start calculation */
    km_reg_write(RKP_CMD_CFG, cfgval.u32);

    ret = hal_rkp_kdf_wait_done_romable();
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "hal_rkp_kdf_wait_done_romable failed\n");

    /* get u_state_out if need, for update. */
    if (u_state_out != NULL) {
        for (i = 0; i < block_size_in_word; i++) {
            u_state_out[i] = km_reg_read(RKP_PBKDF2_DATA(i));
        }
    }
    /* get result if need. */
    if (result != NULL) {
        for (i = 0; i < KDF_PBKDF2_VAL_MAX_SIZE_IN_WORD; i++) {
#if defined(CONFIG_PBKDF2_V1)
            result[i] ^= km_reg_read(RKP_PBKDF2_VAL(i));
#elif defined(CONFIG_PBKDF2_V2)
            result[i] = km_reg_read(RKP_PBKDF2_VAL(i));
#endif
        }
    }
    return CRYPTO_SUCCESS;
}
