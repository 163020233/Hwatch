/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: Provide security environment initital implement.
 *
*/

#include "hal_symc_romable.h"
#include "hal_hash_romable.h"
#include "hal_keyslot_romable.h"
#include "hal_pke_romable.h"
#include "hal_klad_reg.h"
#include "hal_rkp_reg.h"
#include "hal_pke_reg.h"

#include "hal_common.h"
#include "hal_common_reg.h"

#include "drv_hash.h"
#include "ecc_curve_param.h"
#include "crypto_drv_common.h"
#include "security_init.h"

hal_symc_entry_in_t __attribute__((aligned(4))) g_symc_entry_in_buf[CRYPTO_SYMC_MAX_DEPTH];
hal_symc_entry_out_t __attribute__((aligned(4))) g_symc_entry_out_buf[CRYPTO_SYMC_MAX_DEPTH];
hal_hash_entry_in_t __attribute__((aligned(4))) g_hash_entry_in_buf[CRYPTO_HASH_MAX_DEPTH];

#define CACHE_LINE_SIZE     32

#if defined(CONFIG_BUILD_ENV_BOOT)
static drv_hash_ctx g_hash_ctx;
#endif

static void *inner_malloc(uint32_t size)
{
    if (size > 0) {
#if defined(CONFIG_BUILD_ENV_BOOT)
        crypto_unused(size);
        return &g_hash_ctx;
#elif defined(CONFIG_BUILD_ENV_SELITEOS)
#ifdef LOSCFG_SUPPORT_TA
        return los_drvbox_malloc(size);
#else
        return los_kmalloc(size);
#endif  /* LOSCFG_SUPPORT_TA */
#else
        return osal_kmalloc_align(size, OSAL_GFP_KERNEL, CACHE_LINE_SIZE);
#endif
    }
    return NULL;
}

static void inner_free(void *ptr)
{
    if (ptr != NULL) {
#if defined(CONFIG_BUILD_ENV_BOOT)
        crypto_unused(ptr);
#elif defined(CONFIG_BUILD_ENV_SELITEOS)
#ifdef LOSCFG_SUPPORT_TA
        los_drvbox_free(ptr);
#else
        los_kfree(ptr);
#endif
#else
        osal_kfree(ptr);
#endif
    }
    return;
}

void crypto_udelay(uint32_t useconds)
{
    uapi_tcxo_delay_us(useconds);
}

#if defined(CONFIG_BUILD_ENV_BOOT)
#define SEC_SOC_CLK_OPEN_ALL                0x1054
#define SEC_CLK_OPEN_ALL_EXCEPT_TRNG        0x622
static void open_sec_clk(void)
{
    /* open soc register. */
    uint16_t m_clken0_value = crypto_reg_read_u16(M_CLKEN0);
    m_clken0_value |= SEC_SOC_CLK_OPEN_ALL;
    crypto_ex_reg_write_u16(M_CLKEN0, m_clken0_value);
    /* open all sec clk. */
    uint32_t sec_reg_value = ca_ctrl_reg_read(SEC_CRG);
    sec_reg_value |= SEC_CLK_OPEN_ALL_EXCEPT_TRNG;
    ca_ctrl_reg_write(SEC_CRG, sec_reg_value);
    /* disable vote km/spacc(hash/symc)/pke. */
    ca_ctrl_reg_write(VOTE_CLK_EN(1), 0);  // 1: km
    ca_ctrl_reg_write(VOTE_CLK_EN(2), 0);  // 2: spacc
    ca_ctrl_reg_write(VOTE_CLK_EN(3), 0);  // 3: pke

    /* wait for SPACC initial ready. read register. */
    crypto_udelay(1); /* 1: 1us wait for spacc initial ready. */
}
#endif

#if defined(CONFIG_BUILD_ENV_BOOT) || defined(CONFIG_BUILD_ENV_SELITEOS)
#define SYMC_CHN_NUM        1
#define HASH_CHN_NUM        1
#else
#define SYMC_CHN_NUM        2
#define HASH_CHN_NUM        2
#endif

#if defined(CONFIG_BUILD_ENV_LITEOS)
static void inner_mutex_lock(crypto_mutex *mutex)
{
    if (osal_in_interrupt() == 0) {
        crypto_mutex_lock(mutex);
    }
}

static void inner_mutex_unlock(crypto_mutex *mutex)
{
    if (osal_in_interrupt() == 0) {
        crypto_mutex_unlock(mutex);
    }
}

#define INT_MAX         2147483647
static crypto_mutex g_vote_mutex;
static crypto_atomic g_deepsleep_vote[MAX_MODULE_ID] = {0};
const crypto_table_item g_module_id_table[] = {
    {KM_MODULE_ID, PM_SECURITY_KM_VETO_ID},
    {SPACC_KM_MODULE_ID, PM_SECURITY_KM_VETO_ID},  /* km send key to spacc. */
    {PKE_MODULE_ID, PM_SECURITY_PKE_VETO_ID},
    {TRNG_MODULE_ID, PM_SECURITY_TRNG_VETO_ID},
    {SYMC_MODULE_ID, PM_SECURITY_SYMC_VETO_ID},
    {HASH_MODULE_ID, PM_SECURITY_HASH_VETO_ID},
    {CRC_MODULE_ID, PM_SECURITY_SYMC_VETO_ID},  /* crc use symc chn. */
    {PKE_HASH_MODULE_ID, PM_SECURITY_PKE_VETO_ID},
};

void crypto_preprocess(unsigned int module_id)
{
    unsigned int ds_vote_id = 0;
    crypto_get_value_by_index(g_module_id_table, crypto_array_size(g_module_id_table), module_id, &ds_vote_id);
    inner_mutex_lock(&g_vote_mutex);

    if (crypto_atomic_read(&g_deepsleep_vote[module_id]) == 0) {
        /* 1.first vote to not sleep */
        uapi_pm_add_sleep_veto(ds_vote_id);
        /* 2.open module clock */
        drv_cipher_open_clk(module_id);
        /* 3.lock channel. */
        crypto_drv_resume(module_id);
    }
    if (crypto_atomic_read(&g_deepsleep_vote[module_id]) < INT_MAX) {
        crypto_atomic_inc(&g_deepsleep_vote[module_id]);
    }
    inner_mutex_unlock(&g_vote_mutex);
}

void crypto_postprocess(unsigned int module_id)
{
    unsigned int ds_vote_id = 0;
    crypto_get_value_by_index(g_module_id_table, crypto_array_size(g_module_id_table), module_id, &ds_vote_id);
    inner_mutex_lock(&g_vote_mutex);
    if (crypto_atomic_read(&g_deepsleep_vote[module_id]) > 0) {
        crypto_atomic_dec(&g_deepsleep_vote[module_id]);
    }
    if (crypto_atomic_read(&g_deepsleep_vote[module_id]) == 0) {
        /* 1.close module clock. */
        drv_cipher_close_clk(module_id);
        /* 2.vote to sleep. */
        uapi_pm_remove_sleep_veto(ds_vote_id);
    }
    inner_mutex_unlock(&g_vote_mutex);
}

static void inner_crypto_cache_flush(uintptr_t base_addr, uint32_t size)
{
    osal_dcache_region_clean((void *)base_addr, size);
}

static void inner_crypto_cache_inv(uintptr_t base_addr, uint32_t size)
{
    osal_dcache_region_inv((void *)base_addr, size);
}

static crypto_mutex g_hash_mutex;
static crypto_mutex g_symc_mutex;
static crypto_mutex g_km_mutex;

static int32_t inner_symc_try_alloc_chn_func(uint32_t *chn_num)
{
    inner_mutex_lock(&g_symc_mutex);
    *chn_num = SYMC_CHN_NUM;
    return CRYPTO_SUCCESS;
}

static int32_t inner_hash_try_alloc_chn_func(uint32_t *chn_num)
{
    inner_mutex_lock(&g_hash_mutex);
    *chn_num = HASH_CHN_NUM;
    return CRYPTO_SUCCESS;
}

static void inner_symc_free_chn_func(uint32_t chn_num)
{
    crypto_unused(chn_num);
    inner_mutex_unlock(&g_symc_mutex);
}

static void inner_hash_free_chn_func(uint32_t chn_num)
{
    crypto_unused(chn_num);
    inner_mutex_unlock(&g_hash_mutex);
}

static void inner_symc_debug(uint32_t chn_num)
{
    hal_symc_debug_default(chn_num);
}

static void inner_hash_debug(uint32_t chn_num)
{
    hal_hash_debug_default(chn_num);
}

static void inner_hal_keyslot_debug(void)
{
    osal_printk("KL_KEY_SEC_CFG is 0x%x\r\n", km_reg_read(KL_KEY_SEC_CFG));
    osal_printk("KL_KEY_ADDR is 0x%x\r\n", km_reg_read(KL_KEY_ADDR));
    osal_printk("KL_KEY_CFG is 0x%x\r\n", km_reg_read(KL_KEY_CFG));

    osal_printk("KL_COM_LOCK_INFO is 0x%x\r\n", km_reg_read(KL_COM_LOCK_INFO));
    osal_printk("KL_COM_LOCK_STATUS is 0x%x\r\n", km_reg_read(KL_COM_LOCK_STATUS));
    osal_printk("RKP_LOCK is 0x%x\r\n", km_reg_read(RKP_LOCK));
    osal_printk("RKP_CMD_CFG is 0x%x\r\n", km_reg_read(RKP_CMD_CFG));
    osal_printk("KL_COM_CTRL is 0x%x\r\n", km_reg_read(KL_COM_CTRL));
    osal_printk("RKP_PBKDF2_RSLT_CRC is 0x%x\r\n", km_reg_read(RKP_PBKDF2_RSLT_CRC));

    osal_printk("KL_CLR_CTRL is 0x%x\r\n", km_reg_read(KL_CLR_CTRL));
    osal_printk("KL_INT_RAW is 0x%x\r\n", km_reg_read(KL_INT_RAW));
    osal_printk("KL_ERROR is 0x%x\r\n", km_reg_read(KL_ERROR));
    osal_printk("KC_ERROR is 0x%x\r\n", km_reg_read(KC_ERROR));
    osal_printk("KDF_ERROR is 0x%x\r\n", km_reg_read(KDF_ERROR));
}
#define LOCK_TIMEOUT_IN_MS              5000

static crypto_mutex g_pke_mutex;

static int32_t inner_pke_lock(void)
{
    int32_t ret;

    inner_mutex_lock(&g_pke_mutex);

    pke_reg_write(PKE_LOCK_CTRL, 0x1);
    ret = hal_pke_lock_wait_done_romable();
    if (ret == CRYPTO_SUCCESS) {
        return ret;
    }
    inner_mutex_unlock(&g_pke_mutex);
    return ERROR_CHN_BUSY;
}

static void inner_pke_unlock(void)
{
    pke_reg_write(PKE_DRAM_CLR, 0x1);
    (void)hal_pke_wait_done_romable();

    /* Step 2. Unock pke. */
    pke_reg_write(PKE_LOCK_CTRL, 0x3);
    inner_mutex_unlock(&g_pke_mutex);
}

#define KEYSLOT_MCIPHER_USED_CNT        2
#define KEYSLOT_HMAC_USED_CNT           1

osal_semaphore g_keyslot_mcipher_sem;
osal_semaphore g_keyslot_hmac_sem;

static int32_t inner_keyslot_try_alloc_chn(uint32_t *chn_num, uint32_t keyslot_type)
{
    int32_t ret;
    osal_semaphore *sem = NULL;
    if (keyslot_type == KEYSLOT_TYPE_MCIPHER_VAL) {
        sem = &g_keyslot_mcipher_sem;
    } else if (keyslot_type == KEYSLOT_TYPE_HMAC_VAL) {
        sem = &g_keyslot_hmac_sem;
    } else {
        return hal_keyslot_try_alloc_chn_default(chn_num, keyslot_type);
    }

    ret = osal_sem_down_timeout(sem, LOCK_TIMEOUT_IN_MS);
    if (ret != OSAL_SUCCESS) {
        return ERROR_KEYSLOT_TIMEOUT;
    }

    ret = hal_keyslot_try_alloc_chn_default(chn_num, keyslot_type);
    if (ret != CRYPTO_SUCCESS) {
        osal_sem_up(sem);
    }
    return ret;
}

static int32_t inner_keyslot_free_chn(uint32_t chn_num, uint32_t keyslot_type)
{
    int32_t ret;
    osal_semaphore *sem = NULL;
    if (keyslot_type == KEYSLOT_TYPE_MCIPHER_VAL) {
        sem = &g_keyslot_mcipher_sem;
    } else if (keyslot_type == KEYSLOT_TYPE_HMAC_VAL) {
        sem = &g_keyslot_hmac_sem;
    } else {
        return hal_keyslot_free_chn_default(chn_num, keyslot_type);
    }

    ret = hal_keyslot_free_chn_default(chn_num, keyslot_type);
    if (ret == CRYPTO_SUCCESS) {
        osal_sem_up(sem);
    }
    return ret;
}

static void inner_keyslot_mutex_lock(void)
{
    (void)inner_mutex_lock(&g_km_mutex);
}

static void inner_keyslot_mutex_unlock(void)
{
    (void)inner_mutex_unlock(&g_km_mutex);
}
#endif

void uapi_drv_cipher_env_init(void)
{
#if defined(CONFIG_BUILD_ENV_LITEOS)
    hal_symc_func_table_t symc_func_table = { 0 };
    hal_hash_func_table_t hash_func_table = { 0 };
    hal_keyslot_func_table_t keyslot_func_table = { 0 };
    hal_pke_func_table_t pke_func_table = { 0 };
#endif

    /* Register Opertaions. */
    crypto_register_mem_func(inner_malloc, inner_free);
    crypto_register_get_cpu_type_func(hal_sec_get_cpu_type);
#if defined(CONFIG_BUILD_ENV_LITEOS)
    crypto_register_cache_flush_func(inner_crypto_cache_flush);
    crypto_register_cache_inv_func(inner_crypto_cache_inv);

    symc_func_table.try_alloc_chn_func = inner_symc_try_alloc_chn_func;
    symc_func_table.free_chn_func = inner_symc_free_chn_func;
    symc_func_table.debug_func = inner_symc_debug;
    hal_symc_register_romable(&symc_func_table);

    hash_func_table.try_alloc_chn_func = inner_hash_try_alloc_chn_func;
    hash_func_table.free_chn_func = inner_hash_free_chn_func;
    hash_func_table.debug_func = inner_hash_debug;
    hal_hash_register_romable(&hash_func_table);

    keyslot_func_table.debug_func = inner_hal_keyslot_debug;
    keyslot_func_table.try_alloc_chn = inner_keyslot_try_alloc_chn;
    keyslot_func_table.free_chn = inner_keyslot_free_chn;
    keyslot_func_table.mutex_lock = inner_keyslot_mutex_lock;
    keyslot_func_table.mutex_unlock = inner_keyslot_mutex_unlock;
    hal_keyslot_register_romable(&keyslot_func_table);

    pke_func_table.lock = inner_pke_lock;
    pke_func_table.unlock = inner_pke_unlock;
    hal_pke_register_romable(&pke_func_table);
#endif

#if defined(CONFIG_BUILD_ENV_BOOT)
    open_sec_clk();
    hal_trng_set_entropy_source();
#endif
    /* release symc/hash chn locked by bootrom/ssb. */
    (void)hal_symc_chn_deinit_romable(SYMC_CHN_NUM);
    (void)hal_hash_chn_deinit_romable(HASH_CHN_NUM);

    /* lock chn. */
    (void)hal_symc_chn_init_romable(SYMC_CHN_NUM, g_symc_entry_in_buf, g_symc_entry_out_buf);
    (void)hal_hash_chn_init_romable(HASH_CHN_NUM, g_hash_entry_in_buf);

    crypto_curve_param_init();
#if defined(CONFIG_BUILD_ENV_SELITEOS)
    drv_cipher_close_all_clk();
#endif
    /* Global Resources Init. */
#if defined(CONFIG_BUILD_ENV_LITEOS)
    (void)crypto_mutex_init(&g_symc_mutex);
    (void)crypto_mutex_init(&g_hash_mutex);
    (void)crypto_mutex_init(&g_km_mutex);
    (void)crypto_mutex_init(&g_pke_mutex);
    (void)crypto_mutex_init(&g_vote_mutex);

    (void)osal_sem_init(&g_keyslot_mcipher_sem, KEYSLOT_MCIPHER_USED_CNT);
    (void)osal_sem_init(&g_keyslot_hmac_sem, KEYSLOT_HMAC_USED_CNT);
#endif
}

void crypto_drv_resume(unsigned int module_id)
{
    /* 1. lock channel. */
    switch (module_id) {
        case SPACC_KM_MODULE_ID:
            (void)hal_symc_chn_init_romable(SYMC_CHN_NUM, g_symc_entry_in_buf, g_symc_entry_out_buf);
            (void)hal_hash_chn_init_romable(HASH_CHN_NUM, g_hash_entry_in_buf);
            break;
        case SYMC_MODULE_ID:
        case CRC_MODULE_ID:
            (void)hal_symc_chn_init_romable(SYMC_CHN_NUM, g_symc_entry_in_buf, g_symc_entry_out_buf);
            break;
        case HASH_MODULE_ID:
            (void)hal_hash_chn_init_romable(HASH_CHN_NUM, g_hash_entry_in_buf);
            break;
        case PKE_HASH_MODULE_ID:
            (void)hal_hash_chn_init_romable(HASH_CHN_NUM, g_hash_entry_in_buf);
            break;
        case FLASH_ONLINE_KM_MODULE_ID:
        case PKE_MODULE_ID:
        case TRNG_MODULE_ID:
            /* no need process. */
            break;
        default:
            crypto_log_err("wrong module id\r\n");
    }
}

void crypto_drv_suspend(unsigned int module_id)
{
    crypto_unused(module_id);
}

void drv_cipher_open_clk(unsigned int module_id)
{
    hal_sec_vote_enable(module_id);
    hal_sec_open_clk(module_id);
    hal_sec_vote(TD_TRUE, module_id);
}

void drv_cipher_close_clk(unsigned int module_id)
{
    hal_sec_vote(TD_FALSE, module_id);
}

void drv_cipher_close_all_clk(void)
{
    hal_sec_close_all_clk();
}

td_bool crypto_sec_vote_enable(void)
{
    return TD_TRUE;
}