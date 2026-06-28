/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.
 * Description:  PMU DRIVER Source
 */

#include "pmu_mem.h"
#include "soc_reg.h"
#include "chip_io.h"

static void uapi_pmu_mem_itcm_1_control(uint8_t state)
{
    switch (state) {
        case PMU_MEM_SLEEP_SHUT_DOWN: // 硬控睡眠关
            reg16_clrbit(LP_CTL_MCU_RB_LP_CTL_MCU_SLP_VAL_REG, LP_CTL_MCU_RB_MCU_MEM_0_PWR_EN_SLP_VAL_OFFSET); // ce 0
            break;
        case PMU_MEM_SLEEP_RETENTION: // 硬控睡眠retention
            reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_SLP_VAL_REG, LP_CTL_MCU_RB_MCU_MEM_0_PWR_EN_SLP_VAL_OFFSET); // ce 1
            break;
        default:
            break;
    }
}

static void uapi_pmu_mem_itcm_2_control(uint8_t state)
{
    switch (state) {
        case PMU_MEM_SLEEP_SHUT_DOWN: // 硬控睡眠关
            reg16_clrbit(LP_CTL_MCU_RB_LP_CTL_MCU_SLP_VAL_REG, LP_CTL_MCU_RB_MCU_MEM_1_PWR_EN_SLP_VAL_OFFSET); // ce 0
            break;
        case PMU_MEM_SLEEP_RETENTION: // 硬控睡眠retention
            reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_SLP_VAL_REG, LP_CTL_MCU_RB_MCU_MEM_1_PWR_EN_SLP_VAL_OFFSET); // ce 1
            break;
        default:
            break;
    }
}

static void uapi_pmu_mem_dtcm_1_control(uint8_t state)
{
    switch (state) {
        case PMU_MEM_SLEEP_SHUT_DOWN: // 硬控睡眠关
            reg16_clrbit(LP_CTL_MCU_RB_LP_CTL_MCU_SLP_VAL_REG, LP_CTL_MCU_RB_MCU_MEM_2_PWR_EN_SLP_VAL_OFFSET); // ce 0
            break;
        case PMU_MEM_SLEEP_RETENTION: // 硬控睡眠retention
            reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_SLP_VAL_REG, LP_CTL_MCU_RB_MCU_MEM_2_PWR_EN_SLP_VAL_OFFSET); // ce 1
            break;
        default:
            break;
    }
}

static void uapi_pmu_mem_dtcm_2_control(uint8_t state)
{
    switch (state) {
        case PMU_MEM_SLEEP_SHUT_DOWN: // 硬控睡眠关
            reg16_clrbit(LP_CTL_MCU_RB_LP_CTL_MCU_SLP_VAL_REG, LP_CTL_MCU_RB_MCU_MEM_3_PWR_EN_SLP_VAL_OFFSET); // ce 0
            break;
        case PMU_MEM_SLEEP_RETENTION: // 硬控睡眠retention
            reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_SLP_VAL_REG, LP_CTL_MCU_RB_MCU_MEM_3_PWR_EN_SLP_VAL_OFFSET); // ce 1
            break;
        default:
            break;
    }
}

static void uapi_pmu_mem_display_l2m_control(uint8_t state)
{
    switch (state) {
        case PMU_MEM_SLEEP_SHUT_DOWN: // 硬控睡眠关
            // pe and ce 0
            reg16_clrbit(LP_CTL_MCU_RB_LP_CTL_MCU_SLP_VAL_1_REG, LP_CTL_MCU_RB_MEM_SUB_PE_3_PWR_EN_SLP_VAL_OFFSET);
            reg16_clrbit(LP_CTL_MCU_RB_LP_CTL_MCU_SLP_VAL_1_REG, LP_CTL_MCU_RB_MEM_SUB_CE_3_PWR_EN_SLP_VAL_OFFSET);
            break;
        case PMU_MEM_SLEEP_RETENTION: // 硬控睡眠retention
            // pe 0. ce 1
            reg16_clrbit(LP_CTL_MCU_RB_LP_CTL_MCU_SLP_VAL_1_REG, LP_CTL_MCU_RB_MEM_SUB_PE_3_PWR_EN_SLP_VAL_OFFSET);
            reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_SLP_VAL_1_REG, LP_CTL_MCU_RB_MEM_SUB_CE_3_PWR_EN_SLP_VAL_OFFSET);
            break;
        default:
            break;
    }
}

static void uapi_pmu_mem_npu_l2m_0_control(uint8_t state)
{
    switch (state) {
        case PMU_MEM_SLEEP_SHUT_DOWN: // 硬控睡眠关
            reg16_clrbit(LP_CTL_MCU_RB_LP_CTL_MCU_SLP_VAL_1_REG, LP_CTL_MCU_RB_MEM_SUB_PE_0_PWR_EN_SLP_VAL_OFFSET);
            reg16_clrbit(LP_CTL_MCU_RB_LP_CTL_MCU_SLP_VAL_REG, LP_CTL_MCU_RB_MEM_SUB_CE_0_PWR_EN_SLP_VAL_OFFSET);
            break;
        case PMU_MEM_SLEEP_RETENTION: // 硬控睡眠retention
            reg16_clrbit(LP_CTL_MCU_RB_LP_CTL_MCU_SLP_VAL_1_REG, LP_CTL_MCU_RB_MEM_SUB_PE_0_PWR_EN_SLP_VAL_OFFSET);
            reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_SLP_VAL_REG, LP_CTL_MCU_RB_MEM_SUB_CE_0_PWR_EN_SLP_VAL_OFFSET);
            break;
        default:
            break;
    }
}

static void uapi_pmu_mem_npu_l2m_1_control(uint8_t state)
{
    switch (state) {
        case PMU_MEM_SLEEP_SHUT_DOWN: // 硬控睡眠关
            reg16_clrbit(LP_CTL_MCU_RB_LP_CTL_MCU_SLP_VAL_1_REG, LP_CTL_MCU_RB_MEM_SUB_PE_1_PWR_EN_SLP_VAL_OFFSET);
            reg16_clrbit(LP_CTL_MCU_RB_LP_CTL_MCU_SLP_VAL_REG, LP_CTL_MCU_RB_MEM_SUB_CE_1_PWR_EN_SLP_VAL_OFFSET);
            break;
        case PMU_MEM_SLEEP_RETENTION: // 硬控睡眠retention
            reg16_clrbit(LP_CTL_MCU_RB_LP_CTL_MCU_SLP_VAL_1_REG, LP_CTL_MCU_RB_MEM_SUB_PE_1_PWR_EN_SLP_VAL_OFFSET);
            reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_SLP_VAL_REG, LP_CTL_MCU_RB_MEM_SUB_CE_1_PWR_EN_SLP_VAL_OFFSET);
            break;
        default:
            break;
    }
}

static void uapi_pmu_mem_npu_l2m_2_control(uint8_t state)
{
    switch (state) {
        case PMU_MEM_SLEEP_SHUT_DOWN: // 硬控睡眠关
            reg16_clrbit(LP_CTL_MCU_RB_LP_CTL_MCU_SLP_VAL_1_REG, LP_CTL_MCU_RB_MEM_SUB_PE_2_PWR_EN_SLP_VAL_OFFSET);
            reg16_clrbit(LP_CTL_MCU_RB_LP_CTL_MCU_SLP_VAL_REG, LP_CTL_MCU_RB_MEM_SUB_CE_2_PWR_EN_SLP_VAL_OFFSET);
            break;
        case PMU_MEM_SLEEP_RETENTION: // 硬控睡眠retention
            reg16_clrbit(LP_CTL_MCU_RB_LP_CTL_MCU_SLP_VAL_1_REG, LP_CTL_MCU_RB_MEM_SUB_PE_2_PWR_EN_SLP_VAL_OFFSET);
            reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_SLP_VAL_REG, LP_CTL_MCU_RB_MEM_SUB_CE_2_PWR_EN_SLP_VAL_OFFSET);
            break;
        default:
            break;
    }
}

static void uapi_pmu_mem_gmmu_lut_control(uint8_t state)
{
    switch (state) {
        case PMU_MEM_SLEEP_SHUT_DOWN: // 硬控睡眠关
            // pe and ce 0
            reg16_clrbit(LP_CTL_MCU_RB_LP_CTL_MCU_SLP_VAL_1_REG, LP_CTL_MCU_RB_MEM_SUB_PE_3_PWR_EN_SLP_VAL_OFFSET);
            reg16_clrbit(LP_CTL_MCU_RB_LP_CTL_MCU_SLP_VAL_1_REG, LP_CTL_MCU_RB_MEM_SUB_CE_4_PWR_EN_SLP_VAL_OFFSET);
            break;
        case PMU_MEM_SLEEP_RETENTION: // 硬控睡眠retention
            // pe 1. ce 0
            reg16_clrbit(LP_CTL_MCU_RB_LP_CTL_MCU_SLP_VAL_1_REG, LP_CTL_MCU_RB_MEM_SUB_PE_3_PWR_EN_SLP_VAL_OFFSET);
            reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_SLP_VAL_1_REG, LP_CTL_MCU_RB_MEM_SUB_CE_4_PWR_EN_SLP_VAL_OFFSET);
            break;
        default:
            break;
    }
}

static void uapi_pmu_mem_com_ram_control(uint8_t state)
{
    switch (state) {
        case PMU_MEM_SLEEP_SHUT_DOWN: // 硬控睡眠关 ce config slp val
            reg16_clrbit(LP_CTL_GLB_RB_LP_CTL_GLB_SLP_VAL_1_REG, LP_CTL_GLB_RB_COM_MEM_CE_PWR_EN_SLP_VAL_OFFSET);
            break;
        case PMU_MEM_SLEEP_RETENTION: // 硬控睡眠retention
            reg16_setbit(LP_CTL_GLB_RB_LP_CTL_GLB_SLP_VAL_1_REG, LP_CTL_GLB_RB_COM_MEM_CE_PWR_EN_SLP_VAL_OFFSET);
            break;
        default:
            break;
    }
}

static void uapi_pmu_mem_bt_1_ram_control(uint8_t state)
{
    switch (state) {
        case PMU_MEM_SLEEP_SHUT_DOWN: // 硬控睡眠关 ce config slp val
            reg16_clrbit(LP_CTL_BT_RB_LP_CTL_BT_SLP_VAL_REG, LP_CTL_BT_RB_BSOC_MEM_0_PWR_EN_SLP_VAL_OFFSET);
            break;
        case PMU_MEM_SLEEP_RETENTION: // 硬控睡眠retention
            reg16_setbit(LP_CTL_BT_RB_LP_CTL_BT_SLP_VAL_REG, LP_CTL_BT_RB_BSOC_MEM_0_PWR_EN_SLP_VAL_OFFSET);
            break;
        case PMU_MEM_SHUT_DOWN: // MCU man sel 下电
            reg16_clrbit(LP_CTL_BT_RB_LP_CTL_BT_MAN_REG, LP_CTL_BT_RB_BSOC_MEM_0_PWR_EN_MAN_OFFSET);
            reg16_setbit(LP_CTL_BT_RB_LP_CTL_BT_MAN_SEL_REG, LP_CTL_BT_RB_BSOC_MEM_0_PWR_EN_MAN_SEL_OFFSET);
            break;
        case PMU_MEM_TURN_ON: // MCU配置 恢复硬控
            reg16_clrbit(LP_CTL_BT_RB_LP_CTL_BT_MAN_SEL_REG, LP_CTL_BT_RB_BSOC_MEM_0_PWR_EN_MAN_SEL_OFFSET);
            break;
        default:
            break;
    }
}

static void uapi_pmu_mem_bt_2_ram_control(uint8_t state)
{
    switch (state) {
        case PMU_MEM_SLEEP_SHUT_DOWN: // 硬控睡眠关 ce config slp val
            reg16_clrbit(LP_CTL_BT_RB_LP_CTL_BT_SLP_VAL_REG, LP_CTL_BT_RB_BSOC_MEM_1_PWR_EN_SLP_VAL_OFFSET);
            break;
        case PMU_MEM_SLEEP_RETENTION: // 硬控睡眠retention
            reg16_setbit(LP_CTL_BT_RB_LP_CTL_BT_SLP_VAL_REG, LP_CTL_BT_RB_BSOC_MEM_1_PWR_EN_SLP_VAL_OFFSET);
            break;
        case PMU_MEM_SHUT_DOWN: // MCU man sel 下电
            reg16_clrbit(LP_CTL_BT_RB_LP_CTL_BT_MAN_REG, LP_CTL_BT_RB_BSOC_MEM_1_PWR_EN_MAN_OFFSET);
            reg16_setbit(LP_CTL_BT_RB_LP_CTL_BT_MAN_SEL_REG, LP_CTL_BT_RB_BSOC_MEM_1_PWR_EN_MAN_SEL_OFFSET);
            break;
        case PMU_MEM_TURN_ON: // MCU配置 恢复硬控
            reg16_clrbit(LP_CTL_BT_RB_LP_CTL_BT_MAN_SEL_REG, LP_CTL_BT_RB_BSOC_MEM_1_PWR_EN_MAN_SEL_OFFSET);
            break;
        default:
            break;
    }
}

static void uapi_pmu_mem_bt_3_ram_control(uint8_t state)
{
    switch (state) {
        case PMU_MEM_SLEEP_SHUT_DOWN: // 硬控睡眠关 ce config slp val
            reg16_clrbit(LP_CTL_BT_RB_LP_CTL_BT_SLP_VAL_REG, LP_CTL_BT_RB_BSOC_MEM_2_PWR_EN_SLP_VAL_OFFSET);
            break;
        case PMU_MEM_SLEEP_RETENTION: // 硬控睡眠retention
            reg16_setbit(LP_CTL_BT_RB_LP_CTL_BT_SLP_VAL_REG, LP_CTL_BT_RB_BSOC_MEM_2_PWR_EN_SLP_VAL_OFFSET);
            break;
        case PMU_MEM_SHUT_DOWN: // MCU man sel 下电
            reg16_clrbit(LP_CTL_BT_RB_LP_CTL_BT_MAN_REG, LP_CTL_BT_RB_BSOC_MEM_2_PWR_EN_MAN_OFFSET);
            reg16_setbit(LP_CTL_BT_RB_LP_CTL_BT_MAN_SEL_REG, LP_CTL_BT_RB_BSOC_MEM_2_PWR_EN_MAN_SEL_OFFSET);
            break;
        case PMU_MEM_TURN_ON: // MCU配置 恢复硬控
            reg16_clrbit(LP_CTL_BT_RB_LP_CTL_BT_MAN_SEL_REG, LP_CTL_BT_RB_BSOC_MEM_2_PWR_EN_MAN_SEL_OFFSET);
            break;
        default:
            break;
    }
}

static void uapi_pmu_mem_bt_4_ram_control(uint8_t state)
{
    switch (state) {
        case PMU_MEM_SLEEP_SHUT_DOWN: // 硬控睡眠关 ce config slp val
            reg16_clrbit(LP_CTL_BT_RB_LP_CTL_BT_SLP_VAL_REG, LP_CTL_BT_RB_BSOC_MEM_3_PWR_EN_SLP_VAL_OFFSET);
            break;
        case PMU_MEM_SLEEP_RETENTION: // 硬控睡眠retention
            reg16_setbit(LP_CTL_BT_RB_LP_CTL_BT_SLP_VAL_REG, LP_CTL_BT_RB_BSOC_MEM_3_PWR_EN_SLP_VAL_OFFSET);
            break;
        case PMU_MEM_SHUT_DOWN: // MCU man sel 下电
            reg16_clrbit(LP_CTL_BT_RB_LP_CTL_BT_MAN_REG, LP_CTL_BT_RB_BSOC_MEM_3_PWR_EN_MAN_OFFSET);
            reg16_setbit(LP_CTL_BT_RB_LP_CTL_BT_MAN_SEL_REG, LP_CTL_BT_RB_BSOC_MEM_3_PWR_EN_MAN_SEL_OFFSET);
            break;
        case PMU_MEM_TURN_ON: // MCU配置 恢复硬控
            reg16_clrbit(LP_CTL_BT_RB_LP_CTL_BT_MAN_SEL_REG, LP_CTL_BT_RB_BSOC_MEM_3_PWR_EN_MAN_SEL_OFFSET);
            break;
        default:
            break;
    }
}

static void uapi_pmu_mem_bt_1_em_control(uint8_t state)
{
    switch (state) {
        case PMU_MEM_SLEEP_SHUT_DOWN: // 硬控睡眠关 ce config slp val
            reg16_clrbit(LP_CTL_BT_RB_LP_CTL_BT_SLP_VAL_REG, LP_CTL_BT_RB_BT_EM_MEM_0_PWR_EN_SLP_VAL_OFFSET);
            break;
        case PMU_MEM_SLEEP_RETENTION: // 硬控睡眠retention
            reg16_setbit(LP_CTL_BT_RB_LP_CTL_BT_SLP_VAL_REG, LP_CTL_BT_RB_BT_EM_MEM_0_PWR_EN_SLP_VAL_OFFSET);
            break;
        case PMU_MEM_SHUT_DOWN: // MCU man sel 下电
            reg16_clrbit(LP_CTL_BT_RB_LP_CTL_BT_MAN_REG, LP_CTL_BT_RB_BT_EM_MEM_0_PWR_EN_MAN_OFFSET);
            reg16_setbit(LP_CTL_BT_RB_LP_CTL_BT_MAN_SEL_REG, LP_CTL_BT_RB_BT_EM_MEM_0_PWR_EN_MAN_SEL_OFFSET);
            break;
        case PMU_MEM_TURN_ON: // MCU配置 恢复硬控
            reg16_clrbit(LP_CTL_BT_RB_LP_CTL_BT_MAN_SEL_REG, LP_CTL_BT_RB_BT_EM_MEM_0_PWR_EN_MAN_SEL_OFFSET);
            break;
        default:
            break;
    }
}

static void uapi_pmu_mem_bt_2_em_control(uint8_t state)
{
    switch (state) {
        case PMU_MEM_SLEEP_SHUT_DOWN: // 硬控睡眠关 ce config slp val
            reg16_clrbit(LP_CTL_BT_RB_LP_CTL_BT_SLP_VAL_REG, LP_CTL_BT_RB_BT_EM_MEM_1_PWR_EN_SLP_VAL_OFFSET);
            break;
        case PMU_MEM_SLEEP_RETENTION: // 硬控睡眠retention
            reg16_setbit(LP_CTL_BT_RB_LP_CTL_BT_SLP_VAL_REG, LP_CTL_BT_RB_BT_EM_MEM_1_PWR_EN_SLP_VAL_OFFSET);
            break;
        case PMU_MEM_SHUT_DOWN: // MCU man sel 下电
            reg16_clrbit(LP_CTL_BT_RB_LP_CTL_BT_MAN_REG, LP_CTL_BT_RB_BT_EM_MEM_1_PWR_EN_MAN_OFFSET);
            reg16_setbit(LP_CTL_BT_RB_LP_CTL_BT_MAN_SEL_REG, LP_CTL_BT_RB_BT_EM_MEM_1_PWR_EN_MAN_SEL_OFFSET);
            break;
        case PMU_MEM_TURN_ON: // MCU配置 恢复硬控
            reg16_clrbit(LP_CTL_BT_RB_LP_CTL_BT_MAN_SEL_REG, LP_CTL_BT_RB_BT_EM_MEM_1_PWR_EN_MAN_SEL_OFFSET);
            break;
        default:
            break;
    }
}

typedef void (*mem_control_func)(uint8_t);

static const mem_control_func g_mem_control_func[] = {
    uapi_pmu_mem_itcm_1_control,
    uapi_pmu_mem_itcm_2_control,
    uapi_pmu_mem_dtcm_1_control,
    uapi_pmu_mem_dtcm_2_control,
    uapi_pmu_mem_display_l2m_control,
    uapi_pmu_mem_npu_l2m_0_control,
    uapi_pmu_mem_npu_l2m_1_control,
    uapi_pmu_mem_npu_l2m_2_control,
    uapi_pmu_mem_gmmu_lut_control,
    uapi_pmu_mem_com_ram_control,
    uapi_pmu_mem_bt_1_ram_control,
    uapi_pmu_mem_bt_2_ram_control,
    uapi_pmu_mem_bt_3_ram_control,
    uapi_pmu_mem_bt_4_ram_control,
    uapi_pmu_mem_bt_1_em_control,
    uapi_pmu_mem_bt_2_em_control,
};

errcode_t uapi_pmu_mem_control(pmu_mem_type_t id, uint8_t state)
{
    if (id > PMU_MEM_BT_2_EM) {
        return ERRCODE_INVALID_PARAM;
    }

    if (g_mem_control_func[id] != NULL) {
        g_mem_control_func[id](state);
        return ERRCODE_SUCC;
    }
    return ERRCODE_NOT_SUPPORT;
}

errcode_t uapi_pmu_mem_fb_on(void)
{
    // fb sleep power off
    uapi_pmu_mem_control(PMU_MEM_DISPLAY_L2M_RAM, PMU_MEM_SLEEP_RETENTION);
#ifdef SUPPORT_GPU_GMMU
    uapi_pmu_mem_control(PMU_MEM_GMMU_LUT_L2M_RAM, PMU_MEM_SLEEP_RETENTION);
#endif
    uapi_pmu_mem_control(PMU_MEM_NPU_L2M_0_RAM, PMU_MEM_SLEEP_RETENTION);
    uapi_pmu_mem_control(PMU_MEM_NPU_L2M_1_RAM, PMU_MEM_SLEEP_RETENTION);
    uapi_pmu_mem_control(PMU_MEM_NPU_L2M_2_RAM, PMU_MEM_SLEEP_RETENTION);
    return ERRCODE_SUCC;
}

errcode_t uapi_pmu_mem_fb_off(void)
{
    // fb sleep retention
    uapi_pmu_mem_control(PMU_MEM_DISPLAY_L2M_RAM, PMU_MEM_SLEEP_SHUT_DOWN);
#ifdef SUPPORT_GPU_GMMU
    uapi_pmu_mem_control(PMU_MEM_GMMU_LUT_L2M_RAM, PMU_MEM_SLEEP_SHUT_DOWN);
#endif
    uapi_pmu_mem_control(PMU_MEM_NPU_L2M_0_RAM, PMU_MEM_SLEEP_SHUT_DOWN);
    uapi_pmu_mem_control(PMU_MEM_NPU_L2M_1_RAM, PMU_MEM_SLEEP_SHUT_DOWN);
    uapi_pmu_mem_control(PMU_MEM_NPU_L2M_2_RAM, PMU_MEM_SLEEP_SHUT_DOWN);
    return ERRCODE_SUCC;
}

#define CODEC_REG_SP_RAM_TMOD_S28_L             0x583837F8
#define CODEC_REG_SP_RAM_TMOD_S28_H             0x583837FC
#define CODEC_REG_TP_RAM_TMOD                   0x583837F4

void uapi_pmu_mem_codec_0p8_set(void)
{
    /* codec must power on before config */
    if (readl(LP_CTL_AUDIO_RB_LP_CTL_AUDIO_STS_REG) & LP_CTL_AUDIO_RB_AUDIO_SUB_PWR_EN_STS_MASK) {
        writel(CODEC_REG_SP_RAM_TMOD_S28_L, 0x000C0054);
        writel(CODEC_REG_SP_RAM_TMOD_S28_H, 0x000C0054);
        writel(CODEC_REG_TP_RAM_TMOD, 0x44002D);
    }
}

void uapi_pmu_mem_codec_0p9_set(void)
{
    /* codec must power on before config */
    if (readl(LP_CTL_AUDIO_RB_LP_CTL_AUDIO_STS_REG) & LP_CTL_AUDIO_RB_AUDIO_SUB_PWR_EN_STS_MASK) {
        writel(CODEC_REG_SP_RAM_TMOD_S28_L, 0x000B000B);
        writel(CODEC_REG_SP_RAM_TMOD_S28_H, 0x000B0003);
        writel(CODEC_REG_TP_RAM_TMOD, 0x430024);
    }
}

void uapi_pmu_mem_video_0p8_set(void)
{
    if (reg16_getbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_1_REG, LP_CTL_MCU_RB_DISPLAY_SUB_PWR_EN_MAN_OFFSET)) {
        writew(VIDEO_CTL_RB_VIDEO_TP_RAM_TMOD_L_REG, 0x2D); // 状态回读
        writew(VIDEO_CTL_RB_VIDEO_TP_RAM_TMOD_H_REG, 0x44);
        writew(VIDEO_CTL_RB_VIDEO_SP_RAM_TMOD_HH_REG, 0x54);
        writew(VIDEO_CTL_RB_VIDEO_SP_RAM_TMOD_HL_REG, 0x0C);
        writew(VIDEO_CTL_RB_VIDEO_SP_RAM_TMOD_LH_REG, 0x54);
        writew(VIDEO_CTL_RB_VIDEO_SP_RAM_TMOD_LL_REG, 0x0C);
    }
}

void uapi_pmu_mem_video_0p9_set(void)
{
    if (reg16_getbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_1_REG, LP_CTL_MCU_RB_DISPLAY_SUB_PWR_EN_MAN_OFFSET)) {
        writew(VIDEO_CTL_RB_VIDEO_TP_RAM_TMOD_L_REG, 0x24);
        writew(VIDEO_CTL_RB_VIDEO_TP_RAM_TMOD_H_REG, 0x43);
        writew(VIDEO_CTL_RB_VIDEO_SP_RAM_TMOD_HH_REG, 0x0B);
        writew(VIDEO_CTL_RB_VIDEO_SP_RAM_TMOD_HL_REG, 0x03);
        writew(VIDEO_CTL_RB_VIDEO_SP_RAM_TMOD_LH_REG, 0x0B);
        writew(VIDEO_CTL_RB_VIDEO_SP_RAM_TMOD_LL_REG, 0x0B);
    }
}

void uapi_pmu_mem_hs_0p8_set(void)
{
    if (reg16_getbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_2_REG, LP_CTL_MCU_RB_MCU_HS_PWR_EN_MAN_OFFSET)) {
        writew(HS_CTL_RB_USB_CTRL_SP_TMOD_H_REG, 0x54);  // 状态回读
        writew(HS_CTL_RB_USB_CTRL_SP_TMOD_MH_REG, 0x0C);
        writew(HS_CTL_RB_USB_CTRL_SP_TMOD_ML_REG, 0x54);
        writew(HS_CTL_RB_USB_CTRL_SP_TMOD_L_REG, 0x0C);

        writew(HS_CTL_RB_HS_PERP_TCM_RAM_TMOD_CFG0_REG, 0x54);
        writew(HS_CTL_RB_HS_PERP_TCM_RAM_TMOD_CFG1_REG, 0x0C);
        writew(HS_CTL_RB_HS_PERP_TCM_RAM_TMOD_CFG2_REG, 0x54);
        writew(HS_CTL_RB_HS_PERP_TCM_RAM_TMOD_CFG3_REG, 0x0C);
        writew(HS_CTL_RB_HS_PERP_TCM_TP_TMOD_CFG2_REG, 0x2D);
        writew(HS_CTL_RB_HS_PERP_TCM_TP_TMOD_CFG3_REG, 0x44);
    }
}

void uapi_pmu_mem_hs_0p9_set(void)
{
    if (reg16_getbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_2_REG, LP_CTL_MCU_RB_MCU_HS_PWR_EN_MAN_OFFSET)) {
        writew(HS_CTL_RB_USB_CTRL_SP_TMOD_H_REG, 0x0B);
        writew(HS_CTL_RB_USB_CTRL_SP_TMOD_MH_REG, 0x03);
        writew(HS_CTL_RB_USB_CTRL_SP_TMOD_ML_REG, 0x0B);
        writew(HS_CTL_RB_USB_CTRL_SP_TMOD_L_REG, 0x0B);

        writew(HS_CTL_RB_HS_PERP_TCM_RAM_TMOD_CFG0_REG, 0x0B);
        writew(HS_CTL_RB_HS_PERP_TCM_RAM_TMOD_CFG1_REG, 0x0B);
        writew(HS_CTL_RB_HS_PERP_TCM_RAM_TMOD_CFG2_REG, 0x03);
        writew(HS_CTL_RB_HS_PERP_TCM_RAM_TMOD_CFG3_REG, 0x0B);
        writew(HS_CTL_RB_HS_PERP_TCM_TP_TMOD_CFG2_REG, 0x24);
        writew(HS_CTL_RB_HS_PERP_TCM_TP_TMOD_CFG3_REG, 0x43);
    }
}

void uapi_pmu_mem_0p8_set(void)
{
    writew(GLB_CTL_M_RB_AUDIO_RAM_TMOD_0_REG, 0x54);
    writew(GLB_CTL_M_RB_AUDIO_RAM_TMOD_1_REG, 0x0C);
    writew(GLB_CTL_M_RB_AUDIO_RAM_TMOD_2_REG, 0x54);
    writew(GLB_CTL_M_RB_AUDIO_RAM_TMOD_3_REG, 0x0C);

    writew(GLB_CTL_M_RB_BT_RAM_TMOD_0_REG, 0x54);
    writew(GLB_CTL_M_RB_BT_RAM_TMOD_1_REG, 0x0C);
    writew(GLB_CTL_M_RB_BT_RAM_TMOD_2_REG, 0x54);
    writew(GLB_CTL_M_RB_BT_RAM_TMOD_3_REG, 0x0C);

    writew(GLB_CTL_M_RB_COM_RAM_TMOD_0_REG, 0x54);
    writew(GLB_CTL_M_RB_COM_RAM_TMOD_1_REG, 0x0C);
    writew(GLB_CTL_M_RB_COM_RAM_TMOD_2_REG, 0x54);
    writew(GLB_CTL_M_RB_COM_RAM_TMOD_3_REG, 0x0C);

    writew(GLB_CTL_M_RB_MCU_RAM_TMOD_0_REG, 0x54);
    writew(GLB_CTL_M_RB_MCU_RAM_TMOD_1_REG, 0x0C);
    writew(GLB_CTL_M_RB_MCU_RAM_TMOD_2_REG, 0x54);
    writew(GLB_CTL_M_RB_MCU_RAM_TMOD_3_REG, 0x0C);

    writew(GLB_CTL_M_RB_MEM_RAM_TMOD_0_REG, 0x54);
    writew(GLB_CTL_M_RB_MEM_RAM_TMOD_1_REG, 0x0C);
    writew(GLB_CTL_M_RB_MEM_RAM_TMOD_2_REG, 0x54);
    writew(GLB_CTL_M_RB_MEM_RAM_TMOD_3_REG, 0x0C);

    writew(GLB_CTL_M_RB_NPU_SP_RAM_TMOD_0_REG, 0x54);
    writew(GLB_CTL_M_RB_NPU_SP_RAM_TMOD_1_REG, 0x0C);
    writew(GLB_CTL_M_RB_NPU_SP_RAM_TMOD_2_REG, 0x54);
    writew(GLB_CTL_M_RB_NPU_SP_RAM_TMOD_3_REG, 0x0C);
    writew(GLB_CTL_M_RB_NPU_TP_RAM_TMOD_0_REG, 0x2D);
    writew(GLB_CTL_M_RB_NPU_TP_RAM_TMOD_1_REG, 0x44);
    writew(GLB_CTL_M_RB_MEM_TP_RAM_TMOD_0_REG, 0x2D);
    writew(GLB_CTL_M_RB_MEM_TP_RAM_TMOD_1_REG, 0x44);

    uapi_pmu_mem_hs_0p8_set();

    writew(M_CTL_RB_RAM_TMOD_CFG0_REG, 0x54);
    writew(M_CTL_RB_RAM_TMOD_CFG1_REG, 0x0C);
    writew(M_CTL_RB_RAM_TMOD_CFG2_REG, 0x54);
    writew(M_CTL_RB_RAM_TMOD_CFG3_REG, 0x0C);

    uapi_pmu_mem_video_0p8_set();

    writew(GLB_CTL_M_RB_AUDIO_ROM_CTL_REG, 0xC);
    writew(GLB_CTL_M_RB_BT_ROM_CTL_REG, 0xC);
    writew(GLB_CTL_M_RB_MCU_ROM_CTL_REG, 0xC);
    writew(M_CTL_RB_MCPU_ROM_CFG_REG, 0xC);

    uapi_pmu_mem_codec_0p8_set();
}

void uapi_pmu_mem_0p9_set(void)
{
    writew(GLB_CTL_M_RB_AUDIO_RAM_TMOD_0_REG, 0x0B);
    writew(GLB_CTL_M_RB_AUDIO_RAM_TMOD_1_REG, 0x0B);
    writew(GLB_CTL_M_RB_AUDIO_RAM_TMOD_2_REG, 0x03);
    writew(GLB_CTL_M_RB_AUDIO_RAM_TMOD_3_REG, 0x0B);

    writew(GLB_CTL_M_RB_BT_RAM_TMOD_0_REG, 0x0B);
    writew(GLB_CTL_M_RB_BT_RAM_TMOD_1_REG, 0x0B);
    writew(GLB_CTL_M_RB_BT_RAM_TMOD_2_REG, 0x03);
    writew(GLB_CTL_M_RB_BT_RAM_TMOD_3_REG, 0x0B);

    writew(GLB_CTL_M_RB_COM_RAM_TMOD_0_REG, 0x0B);
    writew(GLB_CTL_M_RB_COM_RAM_TMOD_1_REG, 0x0B);
    writew(GLB_CTL_M_RB_COM_RAM_TMOD_2_REG, 0x03);
    writew(GLB_CTL_M_RB_COM_RAM_TMOD_3_REG, 0x0B);

    writew(GLB_CTL_M_RB_MCU_RAM_TMOD_0_REG, 0x0B);
    writew(GLB_CTL_M_RB_MCU_RAM_TMOD_1_REG, 0x0B);
    writew(GLB_CTL_M_RB_MCU_RAM_TMOD_2_REG, 0x03);
    writew(GLB_CTL_M_RB_MCU_RAM_TMOD_3_REG, 0x0B);

    writew(GLB_CTL_M_RB_MEM_RAM_TMOD_0_REG, 0x0B);
    writew(GLB_CTL_M_RB_MEM_RAM_TMOD_1_REG, 0x0B);
    writew(GLB_CTL_M_RB_MEM_RAM_TMOD_2_REG, 0x03);
    writew(GLB_CTL_M_RB_MEM_RAM_TMOD_3_REG, 0x0B);

    writew(GLB_CTL_M_RB_NPU_SP_RAM_TMOD_0_REG, 0x0B);
    writew(GLB_CTL_M_RB_NPU_SP_RAM_TMOD_1_REG, 0x0B);
    writew(GLB_CTL_M_RB_NPU_SP_RAM_TMOD_2_REG, 0x03);
    writew(GLB_CTL_M_RB_NPU_SP_RAM_TMOD_3_REG, 0x0B);
    writew(GLB_CTL_M_RB_NPU_TP_RAM_TMOD_0_REG, 0x24);
    writew(GLB_CTL_M_RB_NPU_TP_RAM_TMOD_1_REG, 0x43);
    writew(GLB_CTL_M_RB_MEM_TP_RAM_TMOD_0_REG, 0x24);
    writew(GLB_CTL_M_RB_MEM_TP_RAM_TMOD_1_REG, 0x43);

    uapi_pmu_mem_hs_0p9_set();

    writew(M_CTL_RB_RAM_TMOD_CFG0_REG, 0x0B);
    writew(M_CTL_RB_RAM_TMOD_CFG1_REG, 0x0B);
    writew(M_CTL_RB_RAM_TMOD_CFG2_REG, 0x03);
    writew(M_CTL_RB_RAM_TMOD_CFG3_REG, 0x0B);

    uapi_pmu_mem_video_0p9_set();

    writew(GLB_CTL_M_RB_AUDIO_ROM_CTL_REG, 0xB);
    writew(GLB_CTL_M_RB_BT_ROM_CTL_REG, 0xB);
    writew(GLB_CTL_M_RB_MCU_ROM_CTL_REG, 0xB);
    writew(M_CTL_RB_MCPU_ROM_CFG_REG, 0xB);

    uapi_pmu_mem_codec_0p9_set();
}
