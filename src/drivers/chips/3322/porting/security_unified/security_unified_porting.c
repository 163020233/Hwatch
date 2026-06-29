/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: security unified porting implement.
 *
 * Create: 2025-06-17
 */

#include "limits.h"
#include "hal_common.h"
#include "hal_common_reg.h"
#include "hal_rkp_reg.h"
#include "hal_pke_reg.h"
#include "hal_trng_reg.h"
#include "crypto_drv_common.h"
#include "crypto_osal_adapt.h"
#include "efuse.h"
#include "hal_keyslot_romable.h"
#include "security_unified_porting.h"

#define SEC_SOC_CLK_OPEN_ALL    0x1054

/* Define the union SEC_CRG */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    spacc_rst_reg    : 1; /* [0] spacc reset registration. */
        unsigned int    spacc_cken    : 1; /* [1] spacc clock control. */
        unsigned int    spacc_rst_ok    : 1; /* [2] spacc reset status. */
        unsigned int    reserved_0    : 1; /* [3] */
        unsigned int    pke_rst_reg    : 1; /* [4] pke reset registration. */
        unsigned int    pke_cken    : 1; /* [5] pke clock control. */
        unsigned int    reserved_1    : 2; /* [6:7] */
        unsigned int    trng_cken    : 1; /* [8] trng clock control. */
        unsigned int    hkm_kc_cken    : 1; /* [9] hkm/kc clock control. */
        unsigned int    core_cken    : 1; /* [10] sce/hash clock control. */
        unsigned int    reserved_2    : 21; /* [11:31] */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} sec_crg;


/* Define the union M_CLKEN0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned short    sdio_cfg_apb_pclk_en    : 1; /* [0] */
        unsigned short    mdiag_clken             : 1; /* [1] */
        unsigned short    cfg_m_sec_pke_clken     : 1; /* [2] pke soc clock enable, default 0. */
        unsigned short    m_gpio_clken            : 1; /* [3] */
        unsigned short    cfg_m_sce_km_clken      : 1; /* [4] km soc clock enable, default 0. */
        unsigned short    m_gpio_perp_clken       : 1; /* [5] */
        unsigned short    cfg_m_sec_bus_clken   : 1; /* [6] sec H2H clock, default 0, open for read/write outter data */
        unsigned short    pwm_clken               : 1; /* [7] */
        unsigned short    cfg_m_glb_clken_mem_bus : 1; /* [8] */
        unsigned short    cfg_m_trigger_clken     : 1; /* [9] */
        unsigned short    cfg_m_hs_sub_clken      : 1; /* [10] */
        unsigned short    cfg_m_diag_p2p_clken    : 1; /* [11] */
        unsigned short    cfg_m_sec_p2p_clken   : 1; /* [12] sec p2p clock, default 1, control the MCU config for SEC */
        unsigned short    cfg_m_x2h_mem_bus_clken : 1; /* [13] */
        unsigned short    cfg_m_mem_bus_clken     : 1; /* [14] */
        unsigned short    cfg_m_dsp2mem_x2x_clken : 1; /* [15] */
    } bits;

    /* Define an unsigned member */
    unsigned short    u16;
} m_clken0;

static crypto_atomic g_trng_vote = {0};

/* realize in porting, for 3322/2827 different. */
void hal_sec_open_clk(unsigned int module_id)
{
    m_clken0 m_clken0_value = {.u16 = crypto_reg_read_u16(M_CLKEN0)};
    sec_crg sec_reg_value = {.u32 = ca_ctrl_reg_read(SEC_CRG)};
    unsigned int i = 0;
    crypto_unused(i);

    /* open soc register. */
    m_clken0_value.u16 |= SEC_SOC_CLK_OPEN_ALL;
    crypto_ex_reg_write_u16(M_CLKEN0, m_clken0_value.u16);
    /* open sec clock. */
    switch (module_id) {
        case SPACC_KM_MODULE_ID:
        case CRC_MODULE_ID:
        case SYMC_MODULE_ID:
        case HASH_MODULE_ID:
            sec_reg_value.bits.spacc_cken = 1;
            sec_reg_value.bits.core_cken = 1;
            ca_ctrl_reg_write(SEC_CRG, sec_reg_value.u32);
            /* 2: wait for SPACC ram initial ready. 64 ticks 64 * 1/32 000 000 = 2us. */
            crypto_udelay(2);
        /* no break, for that hmac & symc will use km. */
        case KM_MODULE_ID:
            sec_reg_value.bits.hkm_kc_cken = 1;
            sec_reg_value.bits.core_cken = 1;
            ca_ctrl_reg_write(SEC_CRG, sec_reg_value.u32);
            break;
        case PKE_HASH_MODULE_ID:
            sec_reg_value.bits.spacc_cken = 1;
            sec_reg_value.bits.core_cken = 1;
        case PKE_MODULE_ID:
            sec_reg_value.bits.pke_cken = 1;
            ca_ctrl_reg_write(SEC_CRG, sec_reg_value.u32);
            /* no nee wait for pke initial ready, lock will check. */
            break;
        case TRNG_MODULE_ID:
            /* trng process separately, for that pke will call drv trng. */
            break;
        default:
            crypto_log_err("wrong module id\r\n");
    }
}

void hal_sec_close_all_clk(void)
{
    trng_sec_cfg ring_en = {.u16 = crypto_reg_read_u16(TRNG_SEC_CFB)};
    /* enable vote, for lowpower. */
    ca_ctrl_reg_write(TRNG_VOTE_CLK_EN, 1);
    ca_ctrl_reg_write(KM_VOTE_CLK_EN, 1);
    ca_ctrl_reg_write(SPACC_VOTE_CLK_EN, 1);
    ca_ctrl_reg_write(PKE_VOTE_CLK_EN, 1);
    /* close all clk. */
    ca_ctrl_reg_write(TRNG_VOTE_CFG, 0);
    ca_ctrl_reg_write(KM_VOTE_CFG, 0);
    ca_ctrl_reg_write(SPACC_VOTE_CFG, 0);
    ca_ctrl_reg_write(PKE_VOTE_CFG, 0);
    /* vote trng ring. */
    if (ring_en.bits.trng_ring_en != TRNG_RING_DISABLE) {
        ring_en.bits.trng_ring_en = TRNG_RING_DISABLE;
        crypto_ex_reg_write_u16(TRNG_SEC_CFB, ring_en.u16);
    }
}

void hal_sec_vote_enable(unsigned int module_id)
{
    /* 1.get vote enable value. */
    vote_clk_en vote_clk_en_value;
    unsigned char is_hash = 0;
    unsigned char is_spacc = 0;
    if (module_id == SYMC_MODULE_ID || module_id == HASH_MODULE_ID) {
        /* if is spacc will vote km. */
        is_spacc = 1;
        module_id = SPACC_MODULE_ID;
    } else if (module_id == CRC_MODULE_ID) {
        module_id = SPACC_MODULE_ID;
    }
    if (module_id == PKE_HASH_MODULE_ID) {
        /* if is hash will enable spacc. */
        is_hash = 1;
        module_id = PKE_MODULE_ID;
    }
    vote_clk_en_value.u32 = ca_ctrl_reg_read(VOTE_CLK_EN(module_id));
    /* 2.enable vote according efuse. */
    if (crypto_sec_vote_enable() == TD_TRUE && vote_clk_en_value.bits.vote_clk_en != 1) {
        vote_clk_en_value.bits.vote_clk_en = 1;
        ca_ctrl_reg_write(VOTE_CLK_EN(module_id), vote_clk_en_value.u32);
        if (is_hash) {
            vote_clk_en_value.u32 = ca_ctrl_reg_read(VOTE_CLK_EN(SPACC_MODULE_ID));
            vote_clk_en_value.bits.vote_clk_en = 1;
            ca_ctrl_reg_write(VOTE_CLK_EN(SPACC_MODULE_ID), vote_clk_en_value.u32);
        }
        if (is_spacc) {
            vote_clk_en_value.u32 = ca_ctrl_reg_read(VOTE_CLK_EN(KM_MODULE_ID));
            vote_clk_en_value.bits.vote_clk_en = 1;
            ca_ctrl_reg_write(VOTE_CLK_EN(KM_MODULE_ID), vote_clk_en_value.u32);
        }
    } else if (crypto_sec_vote_enable() != TD_TRUE && vote_clk_en_value.bits.vote_clk_en != 0) {
        vote_clk_en_value.bits.vote_clk_en = 0;
        ca_ctrl_reg_write(VOTE_CLK_EN(module_id), vote_clk_en_value.u32);
        if (is_hash) {
            vote_clk_en_value.u32 = ca_ctrl_reg_read(VOTE_CLK_EN(SPACC_MODULE_ID));
            vote_clk_en_value.bits.vote_clk_en = 0;
            ca_ctrl_reg_write(VOTE_CLK_EN(SPACC_MODULE_ID), vote_clk_en_value.u32);
        }
        if (is_spacc) {
            vote_clk_en_value.u32 = ca_ctrl_reg_read(VOTE_CLK_EN(KM_MODULE_ID));
            vote_clk_en_value.bits.vote_clk_en = 0;
            ca_ctrl_reg_write(VOTE_CLK_EN(KM_MODULE_ID), vote_clk_en_value.u32);
        }
    }
}

void hal_sec_vote_disable(unsigned int module_id)
{
    /* 1.get vote enable value. */
    vote_clk_en vote_clk_en_value;
    unsigned char is_hash = 0;
    unsigned char is_spacc = 0;
    if (module_id == SYMC_MODULE_ID || module_id == HASH_MODULE_ID) {
        /* if is spacc will vote km. */
        is_spacc = 1;
        module_id = SPACC_MODULE_ID;
    } else if (module_id == CRC_MODULE_ID) {
        module_id = SPACC_MODULE_ID;
    }
    if (module_id == PKE_HASH_MODULE_ID) {
        /* if is hash will enable spacc. */
        is_hash = 1;
        module_id = PKE_MODULE_ID;
    }
    vote_clk_en_value.u32 = ca_ctrl_reg_read(VOTE_CLK_EN(module_id));
    /* 2.disable vote. */
    vote_clk_en_value.bits.vote_clk_en = 0;
    ca_ctrl_reg_write(VOTE_CLK_EN(module_id), vote_clk_en_value.u32);
    if (is_hash) {
        vote_clk_en_value.u32 = ca_ctrl_reg_read(VOTE_CLK_EN(SPACC_MODULE_ID));
        vote_clk_en_value.bits.vote_clk_en = 0;
        ca_ctrl_reg_write(VOTE_CLK_EN(SPACC_MODULE_ID), vote_clk_en_value.u32);
    }
    if (is_spacc) {
        vote_clk_en_value.u32 = ca_ctrl_reg_read(VOTE_CLK_EN(KM_MODULE_ID));
        vote_clk_en_value.bits.vote_clk_en = 0;
        ca_ctrl_reg_write(VOTE_CLK_EN(KM_MODULE_ID), vote_clk_en_value.u32);
    }
}

static crypto_atomic g_clk_vote[MAX_CLK_MODULE_ID] = {0};
static void inner_hal_sec_vote(td_bool enable, unsigned int module_id)
{
    vote_cfg vote_cfg_value;
    /* 1. get run env. */
    crypto_cpu_type cpu_type = crypto_get_cpu_type();
    /* vote for clock open. */
    if (enable && crypto_atomic_inc_return(&g_clk_vote[module_id]) == 1) {
        vote_cfg_value.u32 = ca_ctrl_reg_read(VOTE_CFG(module_id));
        if (cpu_type == CRYPTO_CPU_TYPE_SCPU) {
            vote_cfg_value.bits.tee_vote = 1;
        } else {
            vote_cfg_value.bits.ree_vote = 1;
        }
        ca_ctrl_reg_write(VOTE_CFG(module_id), vote_cfg_value.u32);
    }
    /* vote for clock close. */
    if (!enable && crypto_atomic_dec_return(&g_clk_vote[module_id]) == 0) {
        vote_cfg_value.u32 = ca_ctrl_reg_read(VOTE_CFG(module_id));
        if (cpu_type == CRYPTO_CPU_TYPE_SCPU) {
            vote_cfg_value.bits.tee_vote = 0;
        } else {
            vote_cfg_value.bits.ree_vote = 0;
        }
        ca_ctrl_reg_write(VOTE_CFG(module_id), vote_cfg_value.u32);
    }
}

void hal_sec_vote(td_bool enable, unsigned int module_id)
{
    unsigned char is_hash = 0;
    unsigned char is_spacc = 0;
    unsigned int set_module_id = module_id;
    if (module_id == SYMC_MODULE_ID || module_id == HASH_MODULE_ID) {
        /* if is spacc will vote km. */
        is_spacc = 1;
        set_module_id = SPACC_MODULE_ID;
    } else if (module_id == CRC_MODULE_ID) {
        set_module_id = SPACC_MODULE_ID;
    }
    if (module_id == PKE_HASH_MODULE_ID) {
        /* if is hash will enable spacc. */
        is_hash = 1;
        set_module_id = PKE_MODULE_ID;
    }
    inner_hal_sec_vote(enable, set_module_id);
    if (is_spacc) {
        inner_hal_sec_vote(enable, KM_MODULE_ID);
    }
    if (is_hash) {
        inner_hal_sec_vote(enable, SPACC_MODULE_ID);
    }
}

crypto_cpu_type hal_sec_get_cpu_type(void)
{
    cpu_type_cfg cpu_type_value = {.u32 = ca_ctrl_reg_read(CPU_TYPE)};
    crypto_log_dbg("cpu_type_value: 0x%x\r\n", cpu_type_value.u32);
    if (cpu_type_value.bits.cpu_type == 0xaa) {
        return CRYPTO_CPU_TYPE_ACPU;
    } else if (cpu_type_value.bits.cpu_type == 0xa5) {
        return CRYPTO_CPU_TYPE_SCPU;
    } else {
        crypto_log_err("invalid CPU type\r\n");
        return CRYPTO_CPU_TYPE_INVALID;
    }
}

void hal_trng_open_clk(void)
{
    /* 1. get run env. */
    crypto_cpu_type cpu_type = crypto_get_cpu_type();
    sec_crg sec_reg_value = {.u32 = ca_ctrl_reg_read(SEC_CRG)};
    trng_sample_ckdiv trng_ckdiv;
    trng_sec_cfg ring_en = {0};
    vote_cfg vote_trng_cfg_value;
    trng_ckdiv.u16 = crypto_reg_read_u16(TRNG_SAMPLE_CKDIV);
    ring_en.u16 = crypto_reg_read_u16(TRNG_SEC_CFB);

    /* 2. if not first, increase index & return. */
    if (crypto_atomic_read(&g_trng_vote) == 0) {
        crypto_atomic_inc(&g_trng_vote);
    } else if (crypto_atomic_read(&g_trng_vote) < INT_MAX) {
        crypto_atomic_inc(&g_trng_vote);
        return;
    }
    /* 3. set div. */
    if (trng_ckdiv.bits.cfg_m_clk_trng_sample_div_clken != TRNG_SAMPLE_DIV_CLKEN) {
        trng_ckdiv.bits.cfg_m_clk_trng_sample_div_clken = 0;
        trng_ckdiv.bits.cfg_m_trng_sample_load_div_en = 0;
        trng_ckdiv.bits.cfg_m_trng_sample_div_num = TRNG_SAMPLE_DIV_NUM;
        trng_ckdiv.bits.cfg_m_trng_sample_load_div_en = TRNG_SAMPLE_LOAD_DIV_EN;
        trng_ckdiv.bits.cfg_m_clk_trng_sample_div_clken = TRNG_SAMPLE_DIV_CLKEN;
        crypto_ex_reg_write_u16(TRNG_SAMPLE_CKDIV, trng_ckdiv.u16);
    }
    /* 4. ring_en. */
    if (ring_en.bits.trng_ring_en != TRNG_RING_ENABLE) {
        ring_en.bits.trng_ring_en = TRNG_RING_ENABLE;
        crypto_ex_reg_write_u16(TRNG_SEC_CFB, ring_en.u16);
    }
    /* 5. open trng clk. */
    sec_reg_value.bits.trng_cken = 1;
    ca_ctrl_reg_write(SEC_CRG, sec_reg_value.u32);
    /* 6. according to efuse, set trng vote enable or disable. */
    hal_sec_vote_enable(TRNG_MODULE_ID);
    /* 7. trng clk vote open. */
    if (cpu_type == CRYPTO_CPU_TYPE_SCPU) {
        vote_trng_cfg_value.u32 = ca_ctrl_reg_read(VOTE_CFG(TRNG_MODULE_ID));
        vote_trng_cfg_value.bits.tee_vote = 1;  /* tee vote open clock. */
        ca_ctrl_reg_write(VOTE_CFG(TRNG_MODULE_ID), vote_trng_cfg_value.u32);
    } else {
        vote_trng_cfg_value.u32 = ca_ctrl_reg_read(VOTE_CFG(TRNG_MODULE_ID));
        vote_trng_cfg_value.bits.ree_vote = 1;  /* ree vote open clock. */
        ca_ctrl_reg_write(VOTE_CFG(TRNG_MODULE_ID), vote_trng_cfg_value.u32);
    }
}

void hal_trng_close_clk(void)
{
    /* 1. get run env. */
    crypto_cpu_type cpu_type = crypto_get_cpu_type();
    trng_sec_cfg ring_en = {0};
    vote_cfg vote_trng_cfg_value;
    ring_en.u16 = crypto_reg_read_u16(TRNG_SEC_CFB);
    /* 2. if not final, decrease index & return. */
    if (crypto_atomic_read(&g_trng_vote) > 0) {
        crypto_atomic_dec(&g_trng_vote);
    }
    if (crypto_atomic_read(&g_trng_vote) != 0) {
        return;
    }
    /* 3. ring_en disable. */
    if (ring_en.bits.trng_ring_en != TRNG_RING_DISABLE) {
        ring_en.bits.trng_ring_en = TRNG_RING_DISABLE;
        crypto_ex_reg_write_u16(TRNG_SEC_CFB, ring_en.u16);
    }
    /* 4. trng clk vote close. */
    if (cpu_type == CRYPTO_CPU_TYPE_SCPU) {
        vote_trng_cfg_value.u32 = ca_ctrl_reg_read(VOTE_CFG(TRNG_MODULE_ID));
        vote_trng_cfg_value.bits.tee_vote = 0;  /* tee vote close clock. */
        ca_ctrl_reg_write(VOTE_CFG(TRNG_MODULE_ID), vote_trng_cfg_value.u32);
    } else {
        vote_trng_cfg_value.u32 = ca_ctrl_reg_read(VOTE_CFG(TRNG_MODULE_ID));
        vote_trng_cfg_value.bits.ree_vote = 0;  /* ree vote close clock. */
        ca_ctrl_reg_write(VOTE_CFG(TRNG_MODULE_ID), vote_trng_cfg_value.u32);
    }
}

void get_sec_vote_enable_value(uint8_t *is_sec_vote_disable)
{
    uint8_t is_sec_vote_enable = 0;
    (void)uapi_efuse_read_bit(&is_sec_vote_enable, EFUSE_SEC_LP_VOTE_DIS_BYTE_OFFSET,
        EFUSE_SEC_LP_VOTE_DIS_BIT_OFFSET);
    *is_sec_vote_disable = is_sec_vote_enable == 0 ? 1 : 0;
}

errcode_t __attribute__((weak)) uapi_efuse_read_bit(uint8_t *value, uint32_t byte_number, uint8_t bit_pos)
{
    crypto_unused(value);
    crypto_unused(byte_number);
    crypto_unused(bit_pos);
    return ERRCODE_SUCC;
}

int32_t crypto_km_common_get_hard_key_type_item(crypto_keyslot_hard_key_type hard_key_type,
    uint32_t *rkp_cmd_cfg_val, uint32_t *kl_com_ctrl_val)
{
    switch (hard_key_type) {
        case CRYPTO_KEYSLOT_HARD_KEY_TYPE_USER_REE:
            *rkp_cmd_cfg_val = hal_compat_rkp_cmd_cfg(HAL_MASTER_KEY_SEL_OEM_ROOTKEY0,
                HAL_RKP_KEY_SEL_SOC_ERK_REE, HAL_RKP_ALG_SEL_SHA256);
            *kl_com_ctrl_val = hal_compat_kl_com_ctrl(HAL_KL_RK_CHOOSE_SOC_ERK_REE);
            break;
        case CRYPTO_KEYSLOT_HARD_KEY_TYPE_USER_TEE:
            *rkp_cmd_cfg_val = hal_compat_rkp_cmd_cfg(HAL_MASTER_KEY_SEL_OEM_ROOTKEY0,
                HAL_RKP_KEY_SEL_SOC_ERK_TEE, HAL_RKP_ALG_SEL_SHA256);
            *kl_com_ctrl_val = hal_compat_kl_com_ctrl(HAL_KL_RK_CHOOSE_SOC_ERK_TEE);
            break;
        case CRYPTO_KEYSLOT_HARD_KEY_TYPE_SEC_STORE:
            *rkp_cmd_cfg_val = hal_compat_rkp_cmd_cfg(HAL_MASTER_KEY_SEL_OEM_ROOTKEY0,
                HAL_RKP_KEY_SEL_SOC_ERK_TEE, HAL_RKP_ALG_SEL_SHA256);
            *kl_com_ctrl_val = hal_compat_kl_com_ctrl(HAL_KL_RK_CHOOSE_SOC_ERK_TEE);
            break;
        case CRYPTO_KEYSLOT_HARD_KEY_TYPE_ODRK_TEE:
            *rkp_cmd_cfg_val = hal_compat_rkp_cmd_cfg(HAL_MASTER_KEY_SEL_OEM_ROOTKEY0,
                HAL_RKP_KEY_SEL_ODRK0_TEE, HAL_RKP_ALG_SEL_SHA256);
            *kl_com_ctrl_val = hal_compat_kl_com_ctrl(HAL_KL_RK_CHOOSE_ODRK_TEE);
            break;
        default:
            return ERROR_INVALID_PARAM;
    }
    return CRYPTO_SUCCESS;
}
