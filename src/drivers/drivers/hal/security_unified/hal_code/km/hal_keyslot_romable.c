/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: hal keyslot romable. \n
 *
 * History: \n
 * 2025-03-08, Create file. \n
 */
#include "hal_keyslot_romable.h"
#include "hal_klad_reg.h"
#include "hal_rkp_reg.h"
#if !defined(CONFIG_HKM_SUPPORT)
#include "hal_keyslot_reg.h"
#endif

#include "crypto_drv_common.h"
#ifdef SECURITY_ENHANCEMENT
#include "hal_trng_romable.h"
#include "security_enhancement.h"
#endif

#define REE_KL_KEY_SEC_CFG_VAL  0x1E
#define TEE_KL_KEY_SEC_CFG_VAL  0x3F

#define HKL_KEY_LEN             16

/* Parse key_len in bytes to register value.
** 16 --> 0x01
** 24 --> 0x10
** 32 --> 0x11
 */
#define key_len_parse(key_len)      ((key_len) / 8 - 1)

#ifndef CRYPTO_KLAD_CLR_ROUTE_TIMEOUT
#define CRYPTO_KLAD_CLR_ROUTE_TIMEOUT      1000
#endif

ROM_SHARE_BSS hal_keyslot_func_table_t g_keyslot_table;

CRYPTO_STATIC bool hal_kl_key_cfg_is_hmac(uint32_t kl_key_cfg_val)
{
    if (kl_key_cfg_val == AES_KL_KEY_CFG_VAL || kl_key_cfg_val == SM4_KL_KEY_CFG_VAL ||
        kl_key_cfg_val == FLASH_AES_KL_KEY_CFG_VAL) {
        return false;
    }
    return true;
}

CRYPTO_STATIC void inner_klad_set_clr_key_data(const uint8_t *key, uint32_t key_len)
{
    uint32_t data_word;
    uint32_t i; // for count
    uint32_t j; // for random start index
    uint32_t random_val = 0;

#ifdef SECURITY_ENHANCEMENT
    (void)hal_trng_get_random_romable(&random_val);
    random_val %= (HKL_KEY_LEN / CRYPTO_WORD_WIDTH);
    tee_random_delay();
#endif

    /* set high 128 bits to KL_DATA_IN_0~3. */
    for (i = 0, j = random_val * CRYPTO_WORD_WIDTH; i < HKL_KEY_LEN; i += CRYPTO_WORD_WIDTH) {
        data_word = ((uint32_t)(key)[j] |
            ((uint32_t)(key)[j + 1] << 8) |     // 1,8: bit8~15
            ((uint32_t)(key)[j + 2] << 16) |    // 2,16: bit16~bit23
            ((uint32_t)(key)[j + 3] << 24));    // 3,24: bit24~bit31
        km_reg_write(KL_DATA_IN_0 + j, data_word);
        j = (j + CRYPTO_WORD_WIDTH) % HKL_KEY_LEN;
    }

    if (key_len == HKL_KEY_LEN) {
        data_word = 0;
        return;
    }

#if defined(CONFIG_HKM_SUPPORT)
    /* set low 128 bits to KL_DATA_IN_4~7. */
    for (i = 0, j = random_val * CRYPTO_WORD_WIDTH; i < HKL_KEY_LEN; i += CRYPTO_WORD_WIDTH) {
        if (HKL_KEY_LEN + j < key_len) {
            data_word = ((uint32_t)(key)[HKL_KEY_LEN + j] |
                ((uint32_t)(key)[HKL_KEY_LEN + j + 1] << 8) |     // 1,8: bit8~15
                ((uint32_t)(key)[HKL_KEY_LEN + j + 2] << 16) |    // 2,16: bit16~bit23
                ((uint32_t)(key)[HKL_KEY_LEN + j + 3] << 24));    // 3,24: bit24~bit31
            km_reg_write(KL_DATA_IN_4 + j, data_word);
        } else {
            km_reg_write(KL_DATA_IN_4 + j, 0);
        }
        j = (j + CRYPTO_WORD_WIDTH) % HKL_KEY_LEN;
    }
#endif /* CONFIG_HKM_SUPPORT */
    data_word = 0;
}

CRYPTO_STATIC void inner_klad_clear_clr_key_data(uint32_t key_len)
{
    uint32_t i;  // for count

    /* clear high 128 bits to KL_DATA_IN_0~3. */
    for (i = 0; i < HKL_KEY_LEN; i += CRYPTO_WORD_WIDTH) {
        km_reg_write(KL_DATA_IN_0 + i, 0);
    }

    if (key_len == HKL_KEY_LEN) {
        return;
    }

#if defined(CONFIG_HKM_SUPPORT)
    /* clear low 128 bits to KL_DATA_IN_4~7. */
    for (i = 0; i < HKL_KEY_LEN; i += CRYPTO_WORD_WIDTH) {
        km_reg_write(KL_DATA_IN_4 + i, 0);
    }
#endif /* CONFIG_HKM_SUPPORT */
}

CRYPTO_STATIC int32_t inner_keyslot_lock(uint32_t keyslot_num, bool is_hmac)
{
#if !defined(CONFIG_HKM_SUPPORT)
    kc_cpu_lock_cmd lock_cmd;
    uint32_t cpu_reg_addr = KC_TEECPU_LOCK_CMD;       /* default is tee cpu. */
    uint32_t lock_state_expected = KS_STAT_TEE_LOCK;
    kc_rd_slot_num slot;
    kc_rd_lock_status stat;

    if (crypto_get_cpu_type() == CRYPTO_CPU_TYPE_ACPU) {
        cpu_reg_addr = KC_REECPU_LOCK_CMD;
        lock_state_expected = KS_STAT_REE_LOCK;
    }

    /* Step 1. try to lock. */
    lock_cmd.u32 = km_reg_read(cpu_reg_addr);
    lock_cmd.bits.flush_hmac_kslot_ind = (uint8_t)is_hmac;
    lock_cmd.bits.key_slot_num = keyslot_num;
    lock_cmd.bits.lock_cmd = KS_CPU_LOCK;         /* start lock and unlock */
    km_reg_write(cpu_reg_addr, lock_cmd.u32);

    /* Step 2. Check if lock success. */
    slot.u32 = km_reg_read(KC_RD_SLOT_NUM);
    slot.bits.slot_cfg_type = (uint8_t)is_hmac;
    slot.bits.slot_num_cfg = keyslot_num;
    km_reg_write(KC_RD_SLOT_NUM, slot.u32);

    stat.u32 = km_reg_read(KC_RD_LOCK_STATUS);
    if (lock_state_expected != stat.bits.rd_lock_status) {
        return ERROR_CHN_BUSY;
    }
#endif
    crypto_unused(keyslot_num);
    crypto_unused(is_hmac);
    return CRYPTO_SUCCESS;
}

CRYPTO_STATIC void inner_keyslot_unlock(uint32_t keyslot_num, bool is_hmac)
{
#if !defined(CONFIG_HKM_SUPPORT)
    kc_cpu_lock_cmd lock_cmd;
    uint32_t cpu_reg_addr = KC_TEECPU_LOCK_CMD;       /* default is tee cpu. */

    if (crypto_get_cpu_type() == CRYPTO_CPU_TYPE_ACPU) {
        cpu_reg_addr = KC_REECPU_LOCK_CMD;
    }

    /* Step 1. Try to unlock keyslot. */
    lock_cmd.u32 = km_reg_read(cpu_reg_addr);
    lock_cmd.bits.flush_hmac_kslot_ind = (uint8_t)is_hmac;
    lock_cmd.bits.key_slot_num = keyslot_num;
    lock_cmd.bits.lock_cmd = KS_CPU_UNLOCK;         /* start lock and unlock */
    km_reg_write(cpu_reg_addr, lock_cmd.u32);
#endif
    crypto_unused(keyslot_num);
    crypto_unused(is_hmac);
}

int32_t hal_keyslot_mcipher_lock_romable(uint32_t keyslot_num)
{
    return inner_keyslot_lock(keyslot_num, false);
}

void hal_keyslot_mcipher_unlock_romable(uint32_t keyslot_num)
{
    inner_keyslot_unlock(keyslot_num, false);
}

int32_t hal_keyslot_hmac_lock_romable(uint32_t keyslot_num)
{
    return inner_keyslot_lock(keyslot_num, true);
}

void hal_keyslot_hmac_unlock_romable(uint32_t keyslot_num)
{
    inner_keyslot_unlock(keyslot_num, true);
}

CRYPTO_STATIC int32_t inner_send_clear_key(uint32_t keyslot_chn, const uint8_t *key, uint32_t key_len,
    uint32_t kl_key_cfg_val)
{
    volatile int32_t ret = CRYPTO_FAILURE;
    uint32_t key_sec_cfg_val = TEE_KL_KEY_SEC_CFG_VAL;
    kl_clr_ctrl clr_ctrl = { 0 };

    hal_keyslot_mutex_lock_romable();
    /* Step 1. Configure KL_KEY_CFG. */
    km_reg_write(KL_KEY_CFG, kl_key_cfg_val);

    /* Step 2. Configure KL_KEY_SEC_CFG. */
    if (crypto_get_cpu_type() == CRYPTO_CPU_TYPE_ACPU) {
        key_sec_cfg_val = REE_KL_KEY_SEC_CFG_VAL;
    }
    km_reg_write(KL_KEY_SEC_CFG, key_sec_cfg_val);

    /* Step 3. Configure KL_KEY_ADDR. */
    km_reg_write(KL_KEY_ADDR, keyslot_chn << 1);

    /* Step 4. Configure KL_DATA_IN. */
#if defined(CONFIG_HKM_SUPPORT)
    inner_klad_set_clr_key_data(key, key_len);
#else
    inner_klad_set_clr_key_data(key, HKL_KEY_LEN);
#endif

    /* Step 5. Start send key. */
    clr_ctrl.bits.kl_clr_key_size = key_len_parse(key_len);
    clr_ctrl.bits.kl_clr_start = 1;
    km_reg_write(KL_CLR_CTRL, clr_ctrl.u32);

    /* Step 6. Wait finish. */
    ret = hal_keyslot_wait_done_romable();
    crypto_chk_goto(ret != CRYPTO_SUCCESS, exit_clean);

#if !defined(CONFIG_HKM_SUPPORT)
    if (key_len != HKL_KEY_LEN) {
        km_reg_write(KL_KEY_ADDR, (keyslot_chn << 1) | 0x1);
        inner_klad_set_clr_key_data(key + HKL_KEY_LEN, key_len - HKL_KEY_LEN);

        km_reg_write(KL_CLR_CTRL, clr_ctrl.u32);

        ret = hal_keyslot_wait_done_romable();
        crypto_chk_goto(ret != CRYPTO_SUCCESS, exit_clean);
    }
#endif

    ret = CRYPTO_SUCCESS;
exit_clean:
    if (ret != CRYPTO_SUCCESS) {
        hal_keyslot_debug_romable();
    }
    /* Step 7. Clear KL_DATA_IN */
    inner_klad_clear_clr_key_data(key_len);
    hal_keyslot_mutex_unlock_romable();
    return ret;
}

int32_t hal_keyslot_mcipher_send_clear_key_romable(uint32_t keyslot_chn, const uint8_t *key, uint32_t key_len,
    uint32_t kl_key_cfg_val)
{
    return inner_send_clear_key(keyslot_chn, key, key_len, kl_key_cfg_val);
}

int32_t hal_keyslot_flash_send_clear_key_romable(const uint8_t *key, uint32_t key_len)
{
    return inner_send_clear_key(0, key, key_len, FLASH_AES_KL_KEY_CFG_VAL);
}

int32_t hal_keyslot_wait_done_romable(void)
{
    uint32_t i;
    kl_clr_ctrl clr_ctrl;
    kl_int_raw int_raw;
    if (g_keyslot_table.wait_done_func) {
        return g_keyslot_table.wait_done_func();
    }

    for (i = 0; i < CRYPTO_KLAD_CLR_ROUTE_TIMEOUT; i++) {
        clr_ctrl.u32 = km_reg_read(KL_CLR_CTRL);
        if (clr_ctrl.bits.kl_clr_start == 0) {
            int_raw.u32 = km_reg_read(KL_INT_RAW);
            int_raw.bits.clr_kl_int_raw = 0x1;
            km_reg_write(KL_INT_RAW, int_raw.u32);
            break;
        }
        crypto_udelay(1);
    }
    if (i >= CRYPTO_KLAD_CLR_ROUTE_TIMEOUT) {
        return ERROR_KLAD_TIMEOUT;
    }

    /* Step 7. Error check. */
    if (km_reg_read(KL_ERROR) != 0) {
        return ERROR_KM_KL_LOGIC;
    }
    if (km_reg_read(KC_ERROR) != 0) {
        return ERROR_KM_KC_LOGIC;
    }

    return CRYPTO_SUCCESS;
}

void hal_keyslot_debug_romable(void)
{
    if (g_keyslot_table.debug_func != NULL) {
        g_keyslot_table.debug_func();
    }
}

void hal_keyslot_register_romable(const hal_keyslot_func_table_t *func_table)
{
    (void)memcpy_s(&g_keyslot_table, sizeof(hal_keyslot_func_table_t), func_table, sizeof(hal_keyslot_func_table_t));
}

#define RKP_SALT_LEN    28
int32_t hal_keyslot_send_effective_key_romable(uint32_t keyslot_chn, const uint8_t salt[28], uint32_t key_len,
    uint32_t kl_key_cfg_val, uint32_t rkp_cmd_cfg_val, uint32_t kl_com_ctrl_val)
{
    uint32_t i;
    uint32_t salt_word;
    volatile int32_t ret = CRYPTO_FAILURE;
    uint32_t key_sec_cfg_val = TEE_KL_KEY_SEC_CFG_VAL;
    rkp_cmd_cfg cfgval;
    kl_com_ctrl com_ctrl;

    crypto_unused(key_len);

    hal_keyslot_mutex_lock_romable();
    /* Step 1. Configure KL_KEY_CFG. */
    km_reg_write(KL_KEY_CFG, kl_key_cfg_val);

    /* Step 2. Configure KL_KEY_SEC_CFG. */
    if (crypto_get_cpu_type() == CRYPTO_CPU_TYPE_ACPU) {
        key_sec_cfg_val = REE_KL_KEY_SEC_CFG_VAL;
    }
    km_reg_write(KL_KEY_SEC_CFG, key_sec_cfg_val);

    /* Step 3. Configure KL_KEY_ADDR. */
    if (hal_kl_key_cfg_is_hmac(kl_key_cfg_val)) {
        km_reg_write(KL_KEY_ADDR, keyslot_chn);
    } else {
        km_reg_write(KL_KEY_ADDR, keyslot_chn << 1);
    }

    /* Step 4. Klad lock. */
    ret = hal_klad_lock_romable();
    crypto_chk_goto(ret != CRYPTO_SUCCESS, exit_mutex_unlock);

    /* Step 5. Rkp lock. */
    ret = hal_rkp_lock_romable();
    crypto_chk_goto(ret != CRYPTO_SUCCESS, exit_klad_unlock);

    /* Step 6. Start rkp kdf calculation. */
    /* 6.1 Configure salt. */
    for (i = 0; i < RKP_SALT_LEN / CRYPTO_WORD_WIDTH; i++) {
        salt_word = 0;
        crypto_u8_to_word(salt, salt_word, i);
#if defined(CONFIG_HKM_SUPPORT)
        km_reg_write(RKP_PBKDF2_DATA(i), salt_word);
#else
        km_reg_write(RKP_SALT(i), salt_word);
#endif
    }

    /* 6.2 Config RKP_CMD_CFG and start calculate. */
    cfgval.u32 = rkp_cmd_cfg_val;
#if defined(CONFIG_KM_ADAPT_OLD_API)    /* adapt to ws53/ws63 secure upgrader only. */
    cfgval.bits.pbkdf2_key_len = key_len_parse(key_len); /* pbkdf2_key_len determined by input key_len. */
#else
    cfgval.bits.pbkdf2_key_len = 0; /* pbkdf2_key_len set to 0 as default. */
#endif
    cfgval.bits.sw_calc_req = 1;
    km_reg_write(RKP_CMD_CFG, cfgval.u32);

    /* Step 7. Wait rkp done. */
    ret = hal_rkp_kdf_wait_done_romable();
    crypto_chk_goto(ret != CRYPTO_SUCCESS, exit_rkp_unlock);

    /* Step 8. Klad send key. */
    com_ctrl.u32 = kl_com_ctrl_val;
    com_ctrl.bits.kl_com_key_size = key_len_parse(key_len);
    com_ctrl.bits.kl_com_start = 1;
    km_reg_write(KL_COM_CTRL, com_ctrl.u32);

    ret = hal_klad_com_route_wait_done_romable();
    crypto_chk_goto(ret != CRYPTO_SUCCESS, exit_rkp_unlock);
#if !defined(CONFIG_HKM_SUPPORT)
    if (key_len > HKL_KEY_LEN) {
        if (hal_kl_key_cfg_is_hmac(kl_key_cfg_val)) {
            km_reg_write(KL_KEY_ADDR, keyslot_chn);
        } else {
            km_reg_write(KL_KEY_ADDR, (keyslot_chn << 1) | 0x1);
        }

        km_reg_write(KL_COM_CTRL, com_ctrl.u32);

        ret = hal_klad_com_route_wait_done_romable();
        crypto_chk_goto(ret != CRYPTO_SUCCESS, exit_rkp_unlock);
    }
#endif

    ret = CRYPTO_SUCCESS;
exit_rkp_unlock:
    /* Step 8. Rkp unlock. */
    hal_rkp_unlock_romable();
exit_klad_unlock:
    if (ret != CRYPTO_SUCCESS) {
        hal_keyslot_debug_romable();
    }
    /* Step 9. Klad unlock. */
    hal_klad_unlock_romable();
exit_mutex_unlock:
    hal_keyslot_mutex_unlock_romable();
    return ret;
}

#ifndef CONFIG_KLAD_LOCK_TIMEOUT
#define CONFIG_KLAD_LOCK_TIMEOUT    6000
#endif

int32_t hal_klad_lock_romable(void)
{
    uint32_t i;
    uint32_t lock_stat_expected = HKL_COM_LOCK_STAT_TEE_LOCK;
    hkl_com_lock_info lock_info;
    hkl_com_lock_status lock_status;

    if (crypto_get_cpu_type() == CRYPTO_CPU_TYPE_ACPU) {
        lock_stat_expected = HKL_COM_LOCK_STAT_REE_LOCK;
    }

    for (i = 0; i < CONFIG_KLAD_LOCK_TIMEOUT; i++) {
        km_reg_write(KL_LOCK_CTRL, 0x1);
        lock_info.u32 = km_reg_read(KL_COM_LOCK_INFO);
        lock_status.u32 = km_reg_read(KL_COM_LOCK_STATUS);
        if (lock_info.bits.kl_com_lock_fail == 0x2 &&  // 0x2: lock success.
            lock_status.bits.kl_com_lock_stat == lock_stat_expected) {
            break;
        }
        crypto_udelay(1);
    }
    if (i >= CONFIG_KLAD_LOCK_TIMEOUT) {
        return ERROR_KLAD_LOCK;
    }
    return CRYPTO_SUCCESS;
}

void hal_klad_unlock_romable(void)
{
    km_reg_write(KL_UNLOCK_CTRL, 0x1);
}

#ifndef CONFIG_RKP_LOCK_TIMEOUT
#define CONFIG_RKP_LOCK_TIMEOUT    6000
#endif

int32_t hal_rkp_lock_romable(void)
{
    uint32_t i;
    rkp_lock lock_val;
    uint32_t rkp_config_val = RKP_LOCK_CPU_TEE;

    if (crypto_get_cpu_type() == CRYPTO_CPU_TYPE_ACPU) {
        rkp_config_val = RKP_LOCK_CPU_REE;
    }

    for (i = 0; i < CONFIG_RKP_LOCK_TIMEOUT; i++) {
        km_reg_write(RKP_LOCK, rkp_config_val);
        lock_val.u32 = km_reg_read(RKP_LOCK);
        if (lock_val.bits.km_lock_status == rkp_config_val) {
            break;
        }
        crypto_udelay(1);
    }
    if (i >= CONFIG_RKP_LOCK_TIMEOUT) {
        return ERROR_RKP_LOCK_TIMEOUT;
    }
    return CRYPTO_SUCCESS;
}

void hal_rkp_unlock_romable(void)
{
    km_reg_write(RKP_LOCK, RKP_LOCK_CPU_IDLE);
}

#ifndef CONFIG_RKP_WAIT_TIMEOUT
#define CONFIG_RKP_WAIT_TIMEOUT    (6000 * 1000)  /* max timeout for pbkdf2. */
#endif

int32_t hal_rkp_kdf_wait_done_romable(void)
{
    uint32_t i;
    rkp_cmd_cfg cmd_cfg;

    for (i = 0; i < CONFIG_RKP_WAIT_TIMEOUT; ++i) {
        cmd_cfg.u32 = km_reg_read(RKP_CMD_CFG);
        if (cmd_cfg.bits.sw_calc_req == 0x0) {
            km_reg_write(RKP_RAW_INT, 0x1);
            break;
        }
        crypto_udelay(1);
    }

    if (i >= CONFIG_RKP_WAIT_TIMEOUT) {
        return ERROR_RKP_CALC_TIMEOUT;
    }

    /* check kdf err. */
    if (km_reg_read(KDF_ERROR) != 0) {
        return ERROR_KM_RKP_LOGIC;
    }

    return CRYPTO_SUCCESS;
}

#ifndef CONFIG_KLAD_COM_ROUTE_TIMEOUT
#define CONFIG_KLAD_COM_ROUTE_TIMEOUT   6000
#endif

int32_t hal_klad_com_route_wait_done_romable(void)
{
    uint32_t i;
    kl_com_ctrl com_ctrl;
    kl_int_raw int_raw;

    for (i = 0; i < CONFIG_KLAD_COM_ROUTE_TIMEOUT; i++) {
        com_ctrl.u32 = km_reg_read(KL_COM_CTRL);
        if (com_ctrl.bits.kl_com_start == 0) {
            int_raw.u32 = km_reg_read(KL_INT_RAW);
            int_raw.bits.clr_kl_int_raw = 0x1;
            km_reg_write(KL_INT_RAW, int_raw.u32);
            break;
        }
        crypto_udelay(1);
    }
#ifdef CONFIG_CRYPTO_PRINT_ENABLE
    hal_keyslot_debug_romable();
#endif
    if (i >= CONFIG_KLAD_COM_ROUTE_TIMEOUT) {
        return ERROR_KLAD_TIMEOUT;
    }

    if (km_reg_read(KL_ERROR) != 0) {
        return ERROR_KM_KL_LOGIC;
    }
    if (km_reg_read(KC_ERROR) != 0) {
        return ERROR_KM_KC_LOGIC;
    }

    return CRYPTO_SUCCESS;
}

int hal_rkp_deob_update_romable(uint32_t rkp_deob_cfg_val)
{
    int32_t ret;
    rkp_deob_cfg deob_cfg;

    ret = hal_rkp_lock_romable();
    crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "rkp lock failed\n");

    deob_cfg.u32 = rkp_deob_cfg_val;
    deob_cfg.bits.deob_update_req = 0x1; /* start calculation */
    km_reg_write(RKP_DEOB_CFG, deob_cfg.u32);

    ret = hal_rkp_deob_wait_done_romable();
    if (ret != CRYPTO_SUCCESS) {
        hal_keyslot_debug_romable();
        goto exit_unlock;
    }

exit_unlock:
    hal_rkp_unlock_romable();
    return ret;
}

int32_t hal_rkp_deob_wait_done_romable(void)
{
    uint32_t i;
    uint32_t deob_err;
    rkp_deob_cfg deob_cfg;

    for (i = 0; i < CONFIG_RKP_WAIT_TIMEOUT; ++i) {
        deob_cfg.u32 = km_reg_read(RKP_DEOB_CFG);
        if (deob_cfg.bits.deob_update_req == 0x0) {
            break;
        }
        crypto_udelay(1);
    }
    if (i >= CONFIG_RKP_WAIT_TIMEOUT) {
        return ERROR_RKP_CALC_TIMEOUT;
    }

    /* check deob err. */
    deob_err = km_reg_read(DEOB_ERROR);
    if (deob_err != 0) {
        return ERROR_KM_RKP_LOGIC;
    }

    return CRYPTO_SUCCESS;
}

CRYPTO_STATIC void inner_klad_hmac_set_clr_key_data(const uint8_t key[HKL_KEY_LEN])
{
    uint32_t data_word;
    uint32_t i;

    /* set high 128 bits to KL_DATA_IN_0~3. */
    for (i = 0; i < HKL_KEY_LEN; i += CRYPTO_WORD_WIDTH) {
        data_word = ((uint32_t)(key)[i] |
            ((uint32_t)(key)[i + 1] << 8) |     // 1,8: bit8~15
            ((uint32_t)(key)[i + 2] << 16) |    // 2,16: bit16~bit23
            ((uint32_t)(key)[i + 3] << 24));    // 3,24: bit24~bit31
        km_reg_write(KL_DATA_IN_0 + i, data_word);
    }
}

int32_t hal_keyslot_hmac_send_clear_key_romable(uint32_t keyslot_chn, const uint8_t *key, uint32_t key_len,
    uint32_t kl_key_cfg_val, uint32_t hmac_cal_cnt)
{
    int32_t ret;
    uint32_t key_sec_cfg_val = TEE_KL_KEY_SEC_CFG_VAL;
    uint32_t i;
    uint8_t key_padding[128] = { 0 };
    kl_clr_ctrl clr_ctrl = {0};

    if (key != NULL) {
        ret = memcpy_s(key_padding, sizeof(key_padding), key, key_len);
        crypto_chk_return(ret != EOK, ERROR_MEMCPY_S);
    }

    hal_keyslot_mutex_lock_romable();
    /* Step 1. Configure KL_KEY_CFG. */
    km_reg_write(KL_KEY_CFG, kl_key_cfg_val);

    /* Step 2. Configure KL_KEY_SEC_CFG. */
    if (crypto_get_cpu_type() == CRYPTO_CPU_TYPE_ACPU) {
        key_sec_cfg_val = REE_KL_KEY_SEC_CFG_VAL;
    }
    km_reg_write(KL_KEY_SEC_CFG, key_sec_cfg_val);

    /* Step 3. Configure KL_KEY_ADDR. */
    km_reg_write(KL_KEY_ADDR, keyslot_chn);

    clr_ctrl.bits.kl_clr_start = 1;
    /* Step 4. Configure KL_DATA_IN. */
    for (i = 0; i < hmac_cal_cnt; i++) {
        inner_klad_hmac_set_clr_key_data(key_padding + i * HKL_KEY_LEN);

        /* Step 5. Start send key. */
        clr_ctrl.bits.kl_clr_key_cnt = i;
        km_reg_write(KL_CLR_CTRL, clr_ctrl.u32);

        /* Step 6. Wait finish. */
        ret = hal_keyslot_wait_done_romable();
        crypto_chk_goto(ret != CRYPTO_SUCCESS, exit_clean);
    }

exit_clean:
    hal_keyslot_mutex_unlock_romable();
    (void)memset_s(key_padding, sizeof(key_padding), 0, sizeof(key_padding));
    return CRYPTO_SUCCESS;
}

uint32_t hal_keyslot_get_klad_crc_romable(void)
{
    uint32_t crc;
#if !defined(CONFIG_HKM_SUPPORT)
    crc = km_reg_read(KL_CRC);
#else
    crc = km_reg_read(RKP_PBKDF2_RSLT_CRC);
#endif
    return crc;
}

#ifndef CONFIG_KEYSLOT_MCIPHER_KEYSLOT_CNT
#define CONFIG_KEYSLOT_MCIPHER_KEYSLOT_CNT     8
#endif

#ifndef CONFIG_KEYSLOT_HMAC_KEYSLOT_CNT
#define CONFIG_KEYSLOT_HMAC_KEYSLOT_CNT        2
#endif

/* mcipher */
#ifndef CONFIG_REE_MCIPHER_KEYSLOT_MASK
#define CONFIG_REE_MCIPHER_KEYSLOT_MASK    0x0C    /* keyslot0, 1 reserved for symc chn1, 2. */
#endif

#ifndef CONFIG_TEE_MCIPHER_KEYSLOT_MASK
#define CONFIG_TEE_MCIPHER_KEYSLOT_MASK    0xF0
#endif

/* hmac */
#ifndef CONFIG_REE_HMAC_KEYSLOT_MASK
#define CONFIG_REE_HMAC_KEYSLOT_MASK       0x1
#endif

#ifndef CONFIG_TEE_HMAC_KEYSLOT_MASK
#define CONFIG_TEE_HMAC_KEYSLOT_MASK       0x2
#endif

uint32_t g_keyslot_mcipher_bitmap = 0;
uint32_t g_keyslot_hmac_bitmap = 0;

static uint32_t inner_get_bitmap_bit(uint32_t bitmap, uint8_t bit)
{
    return (bitmap >> bit) & 0x1;
}

static void inner_set_bitmap_bit(uint32_t *bitmap, uint8_t bit)
{
    *bitmap  |= (1 << bit);
}

static void inner_clear_bitmap_bit(uint32_t *bitmap, uint8_t bit)
{
    *bitmap &= ~(1 << bit);
}

int32_t hal_keyslot_try_alloc_chn_default(uint32_t *chn_num, uint32_t keyslot_type)
{
    int32_t ret;
    uint32_t i;
    uint32_t keyslot_cnt = 0;
    uint32_t keyslot_mask = 0;
    uint32_t *bitmap = NULL;
    if (keyslot_type == KEYSLOT_TYPE_MCIPHER_VAL) {
        keyslot_cnt = CONFIG_KEYSLOT_MCIPHER_KEYSLOT_CNT;
        if (crypto_get_cpu_type() == CRYPTO_CPU_TYPE_ACPU) {
            keyslot_mask = CONFIG_REE_MCIPHER_KEYSLOT_MASK;
        } else {
            keyslot_mask = CONFIG_TEE_MCIPHER_KEYSLOT_MASK;
        }
        bitmap = &g_keyslot_mcipher_bitmap;
    } else if (keyslot_type == KEYSLOT_TYPE_HMAC_VAL) {
        keyslot_cnt = CONFIG_KEYSLOT_HMAC_KEYSLOT_CNT;
        if (crypto_get_cpu_type() == CRYPTO_CPU_TYPE_ACPU) {
            keyslot_mask = CONFIG_REE_HMAC_KEYSLOT_MASK;
        } else {
            keyslot_mask = CONFIG_TEE_HMAC_KEYSLOT_MASK;
        }
        bitmap = &g_keyslot_hmac_bitmap;
    } else {
        *chn_num = 0;
        return CRYPTO_SUCCESS;
    }

    hal_keyslot_mutex_lock_romable();
    for (i = 0; i < keyslot_cnt; i++) {
        if (inner_get_bitmap_bit(keyslot_mask, i) == 0) {
            continue;   /* never goes here. */
        }
        if (inner_get_bitmap_bit(*bitmap, i) == 1) {
            continue;
        }
        break;
    }
    if (i >= keyslot_cnt) {
        ret = ERROR_CHN_BUSY;
        goto exit_mutex_unlock;
    }

    if (keyslot_type == KEYSLOT_TYPE_MCIPHER_VAL) {
        ret = hal_keyslot_mcipher_lock_romable(i);
    } else {
        ret = hal_keyslot_hmac_lock_romable(i);
    }
    if (ret != CRYPTO_SUCCESS) {
        ret = ERROR_CHN_BUSY;
        goto exit_mutex_unlock;
    }
    inner_set_bitmap_bit(bitmap, i);
    *chn_num = i;
    ret = CRYPTO_SUCCESS;
exit_mutex_unlock:
    hal_keyslot_mutex_unlock_romable();
    return ret;
}

int32_t hal_keyslot_try_alloc_chn_romable(uint32_t *chn_num, uint32_t keyslot_type)
{
    if (g_keyslot_table.try_alloc_chn != NULL) {
        return g_keyslot_table.try_alloc_chn(chn_num, keyslot_type);
    }

    return hal_keyslot_try_alloc_chn_default(chn_num, keyslot_type);
}

int32_t hal_keyslot_free_chn_default(uint32_t chn_num, uint32_t keyslot_type)
{
    uint32_t *bitmap = NULL;
    if (keyslot_type == KEYSLOT_TYPE_MCIPHER_VAL) {
        bitmap = &g_keyslot_mcipher_bitmap;
    } else if (keyslot_type == KEYSLOT_TYPE_HMAC_VAL) {
        bitmap = &g_keyslot_hmac_bitmap;
    } else {
        return CRYPTO_SUCCESS;
    }

    hal_keyslot_mutex_lock_romable();
    if (inner_get_bitmap_bit(*bitmap, chn_num) == 1) {
        inner_clear_bitmap_bit(bitmap, chn_num);
        if (keyslot_type == KEYSLOT_TYPE_MCIPHER_VAL) {
            (void)hal_keyslot_mcipher_unlock_romable(chn_num);
        } else {
            (void)hal_keyslot_hmac_unlock_romable(chn_num);
        }
        hal_keyslot_mutex_unlock_romable();
        return CRYPTO_SUCCESS;
    }
    hal_keyslot_mutex_unlock_romable();
    return ERROR_INVALID_HANDLE;
}

void hal_keyslot_mutex_lock_romable(void)
{
    if (g_keyslot_table.mutex_lock != NULL) {
        g_keyslot_table.mutex_lock();
    }
}

void hal_keyslot_mutex_unlock_romable(void)
{
    if (g_keyslot_table.mutex_unlock != NULL) {
        g_keyslot_table.mutex_unlock();
    }
}

int32_t hal_keyslot_free_chn_romable(uint32_t chn_num, uint32_t keyslot_type)
{
    if (g_keyslot_table.free_chn != NULL) {
        return g_keyslot_table.free_chn(chn_num, keyslot_type);
    }
    return hal_keyslot_free_chn_default(chn_num, keyslot_type);
}