/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: brandy drv hal
 * Author: audio
 */

#include "securec.h"
#include "soc_osal.h"
#include "audio_osal.h"
#include "audio_debug.h"
#include "sys_struct.h"
#include "mem_addr.h"
#include "reg_addr.h"
#include "common_dsp.h"
#include "core_msg.h"
#include "pm_clocks.h"
#include "pm_definition.h"
#include "pm.h"
#include "pm_pmu.h"
#include "drv_msg_sys.h"
#include "drv_hal.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define MSG_CHECK_ACK_PER_MS 1
#define MSG_CHECK_ACK_TIMEOUT_MS 10

#define HAL_DSP_CORE_NUM 1
#define HAL_SOC_PLL_HZ 884736000
#define HAL_INT_X2A_AUDIO (16 + 3)

#define REG_X2M_SOFT_INT_EN (SOC_M_CTL_RB + 0x020)
#define REG_X2M_SOFT_INT_STS (SOC_M_CTL_RB + 0x028)
#define REG_X2M_SOFT_INT_CLR (SOC_M_CTL_RB + 0x02C)

#define REG_DSP_DIV_NUM (SOC_AUD_DSP_CFG + 0x000)
#define REG_DSP_CLK_EN0 (SOC_AUD_DSP_CFG + 0x004)
#define REG_DSP_RST_EN0 (SOC_AUD_DSP_CFG + 0x008)
#define REG_AUDIO_CLK_CFG (SOC_AUD_DSP_CFG + 0x018)
#define REG_DSP_RUNSTALL (SOC_GLB_CTL_M_RB + 0x830) /* 0x57000830 */

#define dsp_check_core_id_valid(core_id) \
    (((core_id) >= AUDIO_CORE_DSP0) && ((core_id) < (AUDIO_CORE_DSP0 + HAL_DSP_CORE_NUM)))

typedef enum {
    DSP_OCR_NORM_MODE = 0, /* Normal mode */
    DSP_OCR_RTT_MODE, /* Retention mode */
    DSP_OCR_MODE_MAX,
} dsp_ocr_mode; /* On-Chip RAM work mode */

static td_bool g_dsp_first_power_on_done = TD_FALSE;

td_void hal_dsp_get_int_cfg(audio_core_id core_id, dsp_int_cfg *int_cfg)
{
    if (int_cfg != TD_NULL) {
        int_cfg->irq_no = HAL_INT_X2A_AUDIO;
        int_cfg->reg_int = REG_X2M_SOFT_INT_STS;
        int_cfg->reg_int_clr = REG_X2M_SOFT_INT_CLR;
        int_cfg->reg_int_mask = REG_X2M_SOFT_INT_EN;
        int_cfg->int_msg_bit = 0x00000008; /* REG_X2M_SOFT_INT_EN msg bit */
        int_cfg->int_vad_bit = 0x00000000;
        int_cfg->int_clr_bit = 0x00000008; /* REG_X2M_SOFT_INT_CLR msg bit */
    }
    audio_unused(core_id);
}

td_u32 hal_dsp_get_core_num(td_void)
{
    return HAL_DSP_CORE_NUM;
}

td_s32 hal_dsp_init(audio_core_id core_id)
{
    if (!dsp_check_core_id_valid(core_id)) {
        return ERR_SYS_NOT_SUPPORTED;
    }

    return AUDIO_SUCCESS;
}

td_s32 hal_dsp_deinit(audio_core_id core_id)
{
    if (!dsp_check_core_id_valid(core_id)) {
        return ERR_SYS_NOT_SUPPORTED;
    }

    return AUDIO_SUCCESS;
}

static td_void init_clock_attr(audio_clock_attr *clock_attr, const pm_hifi_freq_t *hifi_freq)
{
    switch (hifi_freq->dsp_src) {
        case CLOCKS_SRC_XO:
            clock_attr->clock_type = AUDIO_CLOCK_XO;
            break;

        case CLOCKS_SRC_DLL:
            clock_attr->clock_type = AUDIO_CLOCK_DLL;
            break;

        case CLOCKS_SRC_PLL:
            clock_attr->clock_type = AUDIO_CLOCK_PLL;
            break;

        default:
            clock_attr->clock_type = AUDIO_CLOCK_MAX;
            break;
    }

    clock_attr->clock_freq = (td_u32)hifi_freq->dsp_freq_val;
    sap_trace_log_u32(clock_attr->clock_freq);
}

td_void hal_dsp_set_clock_attr(td_void)
{
    td_s32 ret;
    audio_clock_attr clock_attr;
    pm_hifi_mode_t hifi_mode = uapi_pm_get_hifi_mode();
    pm_hifi_freq_t hifi_freq = uapi_pm_get_hifi_freq(hifi_mode);

    init_clock_attr(&clock_attr, &hifi_freq);

    ret = drv_msg_sys_set_clock_attr(0, &clock_attr);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_msg_sys_set_clock_attr, ret);
    }
}

td_void hal_dsp_set_power_on(audio_core_id core_id, td_bool enable)
{
    audio_func_enter();
    if (enable) {
        audio_reg_bit_wr(LP_CTL_AUDIO_SLEEPING, AUDIO_BIT2, AUDIO_BIT2, 0x0);
        if (g_dsp_first_power_on_done == TD_FALSE) {
            uapi_pmu_control(PMU_AUDIO_SUB, PMU_POWER_ON_AND_RUN);
            g_dsp_first_power_on_done = TD_TRUE;
        } else {
            uapi_pmu_control(PMU_AUDIO_SUB, PMU_POWER_RESUME);
        }
#ifndef SAP_MCU_PSRAM_DYNAMIC_VETO_SUPPORT
        uapi_pm_add_sleep_veto(PM_ID_AUDIO);
#endif
    } else {
        uapi_pmu_control(PMU_AUDIO_SUB, PMU_POWER_SLEEP);
        audio_reg_write(REG_DSP_RUNSTALL, 0x1); /* Stall: suspend dsp's pipeline */
#ifndef SAP_MCU_PSRAM_DYNAMIC_VETO_SUPPORT
        uapi_pm_remove_sleep_veto(PM_ID_AUDIO);
#endif
    }

    audio_unused(core_id);
}

static td_void audio_crg_clk_rst(td_void)
{
    audio_func_enter();

    /* AUDIO_SOFT_CLK_RST_INIT_1STAGE */
    /* cfg_dsp_ocram1_clk_en */
    audio_reg_write(0x58007004, 0x111);
    /* cfg_dsp_ocram1_clk_en */
    audio_reg_write(0x58007004, 0x801111);
    /* cfg_dsp_hifi3z_auto_cg_bypass，hifi时钟自动cg关闭 */
    audio_reg_write(0x5800758c, 0x00fe0f47);
    /*
     * cfg_dsp_timer_wdt_rst_en、cfg_dsp_codec_dig_dspif_clken、codec_h2p_async_clken、
     * dspif_rx_clken、dspif_tx_clken、cfg_dsp_wdg_clk_en、cfg_dsp_timer_clk_en、cfg_dsp_dmac0_clk_en
     */
    audio_reg_write(0x58007008, 0x64330015);
    /* cfg_dsp_bus_d2m_x2x_clken */
    audio_reg_write(0x58007004, 0x801111);
    /* cfg_dsp_ocrom_clk_en */
    audio_reg_write(0x5800714c, 0x1);
    /*
     * cfg_dsp_dsp_tcxo_cnt_div_en、cfg_dsp_dsp_tcxo_cnt_load_div_en、cfg_dsp_wdt0_ip_perp_en、
     * cfg_dsp_wdt0_ip_clk_en、cfg_dsp_timer_clk_force_on、cfg_dsp_timer_ip_prep_en、cfg_dsp_timer_ip_clk_en
     */
    audio_reg_write(0x58007594, 0x83600);
    /* cfg_dsp_apb_load_div_en、cfg_dsp_axi_ahb_load_div_en */
    audio_reg_write(0x58007000, 0x10410101);
    /* cfg_dsp_timer_sel_1、cfg_dsp_timer_sel_2、cfg_dsp_wdt_sel_1、cfg_dsp_wdt_sel_2 */
    audio_reg_write(0x5800700c, 0x0);
    /* cfg_dsp_codec_apb_clken，打开codec apb的clken */
    audio_reg_write(0x58007004, 0x801111);
    /* codec_clk_force_on，开启codec_clk_force_on */
    audio_reg_write(0x58007178, 0x1);
    /* cfg_glbm_soft_rst_glb_codec_lgc_n，解复位codec 1：解复位 0：复位 */
    audio_reg_write(0x570000bc, 0x1);
    /* hifi_ocdhaltonreset，hifi_runstall 置为0，可以执行DSP */
    audio_reg_write(0x5800701c, 0x1);
    /*  Codec inner clk en */
#if (defined SAP_CODEC_UNI)
    audio_reg_write(0x580075A0, 0x00000102); /* dsp_to_coedc_reserved0 0x580075A0 0x00000102 24.576M */
#else
    audio_reg_write(0x580075A0, 0x00000152); /* dsp_to_coedc_reserved0 0x580075A0 0x00000152 */
#endif
}

#define HIFI3Z_M_REMAP_P0_CTL         (SOC_AUD_DSP_CFG + 0x0700)
#define HIFI3Z_M_REMAP_P0_SRC_ADDR    (SOC_AUD_DSP_CFG + 0x0704)
#define HIFI3Z_M_REMAP_P0_DES_ADDR    (SOC_AUD_DSP_CFG + 0x0708)
#define HIFI3Z_M_REMAP_P1_CTL         (SOC_AUD_DSP_CFG + 0x070C)
#define HIFI3Z_M_REMAP_P1_SRC_ADDR    (SOC_AUD_DSP_CFG + 0x0710)
#define HIFI3Z_M_REMAP_P1_DES_ADDR    (SOC_AUD_DSP_CFG + 0x0714)
#define HIFI3Z_M_REMAP_P2_CTL         (SOC_AUD_DSP_CFG + 0x0718)
#define HIFI3Z_M_REMAP_P2_SRC_ADDR    (SOC_AUD_DSP_CFG + 0x071C)
#define HIFI3Z_M_REMAP_P2_DES_ADDR    (SOC_AUD_DSP_CFG + 0x0720)
#define HIFI3Z_M_REMAP_CTL            (SOC_AUD_DSP_CFG + 0x0724)

#define REMAP_P0_SRC_ADDR             (SOC_PSRAM_AUDIO_PHYS_ADDR >> AUDIO_BIT12)           /* 0xE8500000 */
#define REMAP_P0_DES_ADDR             (SOC_PSRAM_ACORE_PHYS_ADDR >> AUDIO_BIT12)           /* 0x22000000 */
#define REMAP_P0_REMAP_LEN            (SOC_PSRAM_AUDIO_SIZE / SIZE_04K - 1)               /* 4K*(remap_length+1)=512K */

static td_void hifi3z_m_remap_cfg(td_u32 idx, td_u32 src_addr, td_u32 dst_addr, td_u32 remap_len)
{
    struct {
        td_u32 remap_ctl;
        td_u32 src_addr;
        td_u32 des_addr;
    } reg_remap[] = {
        {HIFI3Z_M_REMAP_P0_CTL, HIFI3Z_M_REMAP_P0_SRC_ADDR, HIFI3Z_M_REMAP_P0_DES_ADDR},
        {HIFI3Z_M_REMAP_P1_CTL, HIFI3Z_M_REMAP_P1_SRC_ADDR, HIFI3Z_M_REMAP_P1_DES_ADDR},
        {HIFI3Z_M_REMAP_P2_CTL, HIFI3Z_M_REMAP_P2_SRC_ADDR, HIFI3Z_M_REMAP_P2_DES_ADDR},
    };

    if (idx >= sizeof(reg_remap) / sizeof(reg_remap[0])) {
        sap_err_log_ret(ERR_SYS_INVALID_PARAM);
        return;
    }

    audio_log4(AUDIO_LOG_LEVEL_ALERT, "[addr]idx: %x|src: %x|dst: %x|len: %x", idx, src_addr, dst_addr, remap_len);

    /* remap_addr_len */
    audio_reg_write(reg_remap[idx].remap_ctl, remap_len);
    /* remap_src_base_addr */
    audio_reg_bit_wr(reg_remap[idx].src_addr, AUDIO_BIT0, AUDIO_BIT19, src_addr);
    /* remap_des_base_addr */
    audio_reg_bit_wr(reg_remap[idx].des_addr, AUDIO_BIT0, AUDIO_BIT19, dst_addr);
    /* remap_en */
    audio_reg_bit_wr(HIFI3Z_M_REMAP_CTL, idx, idx, TD_TRUE);
}

static td_void audio_remap_init(td_void)
{
    hifi3z_m_remap_cfg(0x0, REMAP_P0_SRC_ADDR, REMAP_P0_DES_ADDR, REMAP_P0_REMAP_LEN);
}

td_s32 hal_dsp_boot_prepare(audio_core_id core_id)
{
    audio_func_enter();

    if (!dsp_check_core_id_valid(core_id)) {
        return ERR_SYS_NOT_SUPPORTED;
    }

    uapi_register_hifi_inform_callback(hal_dsp_set_clock_attr);

    audio_crg_clk_rst();
    audio_remap_init();
    return AUDIO_SUCCESS;
}

td_s32 hal_dsp_boot_up(audio_core_id core_id)
{
    audio_func_enter();

    if (!dsp_check_core_id_valid(core_id)) {
        return ERR_SYS_NOT_SUPPORTED;
    }

    audio_reg_write(REG_DSP_RUNSTALL, 0x0); /* Run: resume dsp's pipeline */

    return AUDIO_SUCCESS;
}

td_s32 hal_dsp_reset(audio_core_id core_id)
{
    if (!dsp_check_core_id_valid(core_id)) {
        return ERR_SYS_NOT_SUPPORTED;
    }

    return AUDIO_SUCCESS;
}

td_void hal_dsp_pd_prepare(td_void)
{
    audio_reg_bit_wr(LP_CTL_AUDIO_SLEEPING, AUDIO_BIT2, AUDIO_BIT2, 0x1);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
