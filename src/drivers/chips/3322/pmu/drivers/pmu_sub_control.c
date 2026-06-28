/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2022. All rights reserved.
 * Description: PM subsystem control interface
 */

#include "pmu_sub_control.h"
#include "pmu_interrupt.h"
#include "panic.h"
#include "securec.h"
#include "tcxo.h"
#include "chip_io.h"
#include "pmu_cmu.h"
#include "pm_pmu.h"
#include "pmu_mem.h"
#include "pmu_perp.h"
#include "clocks_ccrg.h"
#include "pmu_ldo.h"
#include "cpu_load.h"
#include "log_oam_logger.h"
#include "log_def.h"
#include "debug_print.h"
#include "product.h"
#include "soc_osal.h"
#include "pm_clocks.h"
#include "pm_veto.h"
#include "chip_io.h"
#include "diag_log.h"
#include "secure_verify_boot_porting.h"
#include "pm_veto_porting.h"
#include "time.h"
#include "sys/time.h"
#include "clocks_switch.h"
#include "ipc.h"
#include "ipc_porting.h"
#include "cmsis_os2.h"
#include "feature_table.h"
#include "feature_table_shared.h"
#include "pmp_config.h"

#define B_CORE_LOAD_ADDR                (BT_FLASH_REGION_START)

#define HS_SUB_RESET_CFG_NUM            2
#define HS_SUB_RESET_CFG_VALUE          3
#define PM_CORE_PWROFF_TIMEOUT_MS       10
#define BT_POWER_OFF_PREPARE_FINISH_FLAG  (0x5A5A)

uint8_t g_pmu_sub_poweron_flag[PMU_SUB_COUNT] = {0};
pmu_sub_info_t g_pmu_sub_info[PMU_SUB_COUNT] = {0};

static uint32_t get_sub_running_timestamp(void)
{
    struct timeval tv = {0};
    if (gettimeofday(&tv, NULL)) {
        return ERRCODE_FAIL;
    }
    return (uint32_t)tv.tv_sec;
}

static uint32_t pm_core_ipc_send_msg(uint8_t *msg, uint32_t len, uint8_t dst_core)
{
    ipc_msg_info_t msgInfo;
    msgInfo.dst_core = dst_core;
    msgInfo.priority = IPC_PRIORITY_LOWEST;
    msgInfo.channel = 1;
    msgInfo.msg_id = IPC_MSG_CORE_POWER_OFF;
    msgInfo.buf_addr = msg;
    msgInfo.buf_len = len;

    errcode_t ret = uapi_ipc_send_msg_sync(&msgInfo);
    if (ret != ERRCODE_SUCC) {
        PRINT("ipc acore->core[%u] failed. err=%u, len=%u"NEWLINE, dst_core, ret, len);
        return ERRCODE_FAIL;
    }
    PRINT("ipc acore->core[%u] success, systick ms = %llu"NEWLINE, dst_core, uapi_systick_get_ms());
    return ERRCODE_SUCC;
}

static void pm_mcu_hs_perp_power_on(void)
{
    uint32_t irq = osal_irq_lock();

    reg16_clrbit(GLB_CTL_M_RB_SOFT_RST_6_REG, GLB_CTL_M_RB_CFG_GLBM_SOFT_RST_GLB_SDIO_LGC_N_OFFSET);
    reg16_clrbit(GLB_CTL_M_RB_SOFT_RST_6_REG, GLB_CTL_M_RB_CFG_GLBM_SOFT_RST_GLB_SDIO_CRG_N_OFFSET);

    reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_2_REG, LP_CTL_MCU_RB_MCU_HS_PWR_EN_MAN_OFFSET);
    reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_SEL_2_REG, LP_CTL_MCU_RB_MCU_HS_PWR_EN_MAN_SEL_OFFSET);

    while (reg16_getbit(LP_CTL_MCU_RB_LP_CTL_MCU_PWR_ACK_REG, LP_CTL_MCU_RB_MCU_HS_PWR_ACK_OFFSET) != 1) {
    }

    reg16_clrbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_2_REG, LP_CTL_MCU_RB_MCU_HS_ISO_EN_MAN_OFFSET);
    reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_SEL_2_REG, LP_CTL_MCU_RB_MCU_HS_ISO_EN_MAN_SEL_OFFSET);

    reg16_clrbit(LP_CTL_MCU_RB_LP_CTL_MCU_FRC_ZERO_REG, LP_CTL_MCU_RB_RST_SYS_MCU_HS_CRG_N_DBG_FRC_ZERO_OFFSET);
    reg16_clrbit(LP_CTL_MCU_RB_LP_CTL_MCU_FRC_ZERO_REG, LP_CTL_MCU_RB_RST_SYS_MCU_HS_LGC_N_DBG_FRC_ZERO_OFFSET);

    reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_FRC_ONE_REG, LP_CTL_MCU_RB_RST_SYS_MCU_HS_CRG_N_DBG_FRC_ONE_OFFSET);
    reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_FRC_ONE_REG, LP_CTL_MCU_RB_RST_SYS_MCU_HS_LGC_N_DBG_FRC_ONE_OFFSET);

    reg16_setbit(GLB_CTL_M_RB_SOFT_RST_6_REG, GLB_CTL_M_RB_CFG_GLBM_SOFT_RST_GLB_SDIO_CRG_N_OFFSET);
    reg16_setbit(GLB_CTL_M_RB_SOFT_RST_6_REG, GLB_CTL_M_RB_CFG_GLBM_SOFT_RST_GLB_SDIO_LGC_N_OFFSET);

    // auto cg cfg
    writew(HS_CTL_RB_PWM_CFG_REG, 0);
    writew(HS_CTL_RB_HS_AXI_BRG_LP_CFG0_REG, 0);
    writew(HS_CTL_RB_HS_AXI_BRG_LP_CFG1_REG, 0);
    writew(HS_CTL_RB_HS_AXI_BRG_LP_CFG2_REG, 0);
    writew(HS_CTL_RB_HS_AXI_BRG_LP_CFG3_REG, 0);
    writew(HS_CTL_RB_HS_AXI_BRG_LP_CFG4_REG, 0);

    if (uapi_pmu_ldo_get_voltage(PMU_BUCK_LDO_BUCK_0P8) > PMU_0P9_VSET_0V8) {
        uapi_pmu_mem_hs_0p9_set();
    } else {
        uapi_pmu_mem_hs_0p8_set();
    }

    g_pmu_sub_info[PMU_MCU_HS_PERP_SUB].poweron_cnt++;
    osal_irq_restore(irq);
}

static void pm_mcu_hs_perp_power_off(void)
{
    uint32_t irq = osal_irq_lock();

    reg16_clrbit(GLB_CTL_M_RB_SOFT_RST_6_REG, GLB_CTL_M_RB_CFG_GLBM_SOFT_RST_GLB_SDIO_LGC_N_OFFSET);
    reg16_clrbit(GLB_CTL_M_RB_SOFT_RST_6_REG, GLB_CTL_M_RB_CFG_GLBM_SOFT_RST_GLB_SDIO_CRG_N_OFFSET);

    reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_FRC_ZERO_REG, LP_CTL_MCU_RB_RST_SYS_MCU_HS_CRG_N_DBG_FRC_ZERO_OFFSET);
    reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_FRC_ZERO_REG, LP_CTL_MCU_RB_RST_SYS_MCU_HS_LGC_N_DBG_FRC_ZERO_OFFSET);

    reg16_clrbit(LP_CTL_MCU_RB_LP_CTL_MCU_FRC_ONE_REG, LP_CTL_MCU_RB_RST_SYS_MCU_HS_LGC_N_DBG_FRC_ONE_OFFSET);
    reg16_clrbit(LP_CTL_MCU_RB_LP_CTL_MCU_FRC_ONE_REG, LP_CTL_MCU_RB_RST_SYS_MCU_HS_CRG_N_DBG_FRC_ONE_OFFSET);

    reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_2_REG, LP_CTL_MCU_RB_MCU_HS_ISO_EN_MAN_OFFSET);
    reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_SEL_2_REG, LP_CTL_MCU_RB_MCU_HS_ISO_EN_MAN_SEL_OFFSET);

    reg16_clrbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_2_REG, LP_CTL_MCU_RB_MCU_HS_PWR_EN_MAN_OFFSET);
    reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_SEL_2_REG, LP_CTL_MCU_RB_MCU_HS_PWR_EN_MAN_SEL_OFFSET);

    g_pmu_sub_info[PMU_MCU_HS_PERP_SUB].poweroff_cnt++;

    osal_irq_restore(irq);
}

static void pm_display_power_on(void)
{
    uint32_t irq = osal_irq_lock();
    if (reg16_getbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_REG, LP_CTL_MCU_RB_DISPLAY_H264_PWR_EN_MAN_OFFSET) == 1) {
        PRINT("display on, but h264 on.\r\n");
    }
    reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_1_REG, LP_CTL_MCU_RB_DISPLAY_SUB_PWR_EN_MAN_OFFSET);
    reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_SEL_1_REG, LP_CTL_MCU_RB_DISPLAY_SUB_PWR_EN_MAN_SEL_OFFSET);

    reg16_clrbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_1_REG, LP_CTL_MCU_RB_DISPLAY_SUB_ISO_EN_MAN_OFFSET);
    reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_SEL_1_REG, LP_CTL_MCU_RB_DISPLAY_SUB_ISO_EN_MAN_SEL_OFFSET);

    while (reg16_getbit(LP_CTL_MCU_RB_LP_CTL_MCU_PWR_ACK_REG, LP_CTL_MCU_RB_DISPLAY_SUB_PWR_ACK_OFFSET) != 1) {
    }

    reg16_clrbit(LP_CTL_MCU_RB_LP_CTL_MCU_FRC_ZERO_REG, LP_CTL_MCU_RB_RST_SYS_DISPLAY_LGC_N_DBG_FRC_ZERO_OFFSET);
    reg16_clrbit(LP_CTL_MCU_RB_LP_CTL_MCU_FRC_ZERO_REG, LP_CTL_MCU_RB_RST_SYS_DISPLAY_CRG_N_DBG_FRC_ZERO_OFFSET);
    reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_FRC_ONE_REG, LP_CTL_MCU_RB_RST_SYS_DISPLAY_CRG_N_DBG_FRC_ONE_OFFSET);
    reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_FRC_ONE_REG, LP_CTL_MCU_RB_RST_SYS_DISPLAY_LGC_N_DBG_FRC_ONE_OFFSET);

    reg16_setbit(GLB_CTL_M_RB_SOFT_RST_4_REG, GLB_CTL_M_RB_CFG_GLBM_SOFT_RST_GLB_DISPLAY_CRG_N_OFFSET);
    reg16_setbit(GLB_CTL_M_RB_SOFT_RST_4_REG, GLB_CTL_M_RB_CFG_GLBM_SOFT_RST_GLB_DISPLAY_LGC_N_OFFSET);

    while (reg16_getbit(LP_CTL_MCU_RB_LP_CTL_MCU_STS_5_REG, LP_CTL_MCU_RB_RST_SYS_DISPLAY_CRG_N_STS_OFFSET) != 1) {
    }

    while (reg16_getbit(LP_CTL_MCU_RB_LP_CTL_MCU_STS_5_REG, LP_CTL_MCU_RB_RST_SYS_DISPLAY_LGC_N_STS_OFFSET) != 1) {
    }

    reg16_setbit(VIDEO_CTL_RB_VIDEO_CRG_CLKEN_REG, VIDEO_CTL_RB_CFG_VIDEO_DSP_SUB_CLKEN_OFFSET);

    reg16_setbit(VIDEO_CTL_RB_VIDEO_CRG_SOFT_RST_REG, VIDEO_CTL_RB_CFG_VIDEO_SOFT_RST_SYS_N_OFFSET); // vau & dpu

    writew(VIDEO_CTL_RB_MIPITX_CRG_CKEN_SRST_REQ_REG,
        VIDEO_CTL_RB_CFG_VIDEO_MIPITX_APB_SRST_REQ_MASK | VIDEO_CTL_RB_CFG_VIDEO_MIPITX_APB_CKEN_MASK);
    writew(VIDEO_CTL_RB_MIPITX_CRG_CKEN_SRST_REQ_REG, VIDEO_CTL_RB_CFG_VIDEO_MIPITX_APB_CKEN_MASK); // mipi clk & rst

    uapi_pmu_ldo_set_state(PMU_LDO_MIPILDO_0P8, TURN_ON);
    uapi_pmu_ldo_set_state(PMU_LDO_MIPILDO_1P8, TURN_ON);

    reg16_clrbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_3_REG, LP_CTL_MCU_RB_MIPI_AOD_ISO_EN_MAN_OFFSET);
    reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_SEL_3_REG, LP_CTL_MCU_RB_MIPI_AOD_ISO_EN_MAN_SEL_OFFSET);

#if defined(CONFIG_SUPPORT_AUTO_CG)
    reg16_clrbit(VIDEO_CTL_RB_VIDEO_2TO1_AXI_REG, VIDEO_CTL_RB_VIDEO_2TO1_AXI_SELF_GLB_CG_BYPASS_OFFSET);
    reg16_clrbit(VIDEO_CTL_RB_VIDEO_2TO1_AXI_REG, VIDEO_CTL_RB_VIDEO_2TO1_AXI_AUTO_CG_BYPASS_OFFSET);
    reg16_clrbit(VIDEO_CTL_RB_VIDEO_2TO1_AXI_REG, VIDEO_CTL_RB_VIDEO_2TO1_AXI_CSYSREQ_OFFSET);
#endif

    if (uapi_pmu_ldo_get_voltage(PMU_BUCK_LDO_BUCK_0P8) > PMU_0P9_VSET_0V8) {
        uapi_pmu_mem_video_0p9_set();
    } else {
        uapi_pmu_mem_video_0p8_set();
    }

    g_pmu_sub_info[PMU_DISPLAY_SUB].poweron_cnt++;
    osal_irq_restore(irq);
}

static void pm_display_power_off(void)
{
    uint32_t irq = osal_irq_lock();
    if (reg16_getbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_REG, LP_CTL_MCU_RB_DISPLAY_H264_PWR_EN_MAN_OFFSET) == 1) {
        PRINT("display off, but h264 on.\r\n");
    }
    reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_3_REG, LP_CTL_MCU_RB_MIPI_AOD_ISO_EN_MAN_OFFSET);
    reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_SEL_3_REG, LP_CTL_MCU_RB_MIPI_AOD_ISO_EN_MAN_SEL_OFFSET);

    uapi_pmu_ldo_set_state(PMU_LDO_MIPILDO_1P8, TURN_OFF);
    uapi_pmu_ldo_set_state(PMU_LDO_MIPILDO_0P8, TURN_OFF);

    reg16_clrbit(VIDEO_CTL_RB_VIDEO_CRG_SOFT_RST_REG, VIDEO_CTL_RB_CFG_VIDEO_SOFT_RST_SYS_N_OFFSET);

    reg16_clrbit(VIDEO_CTL_RB_VIDEO_CRG_CLKEN_REG, VIDEO_CTL_RB_CFG_VIDEO_DSP_SUB_CLKEN_OFFSET);

    reg16_clrbit(GLB_CTL_M_RB_SOFT_RST_4_REG, GLB_CTL_M_RB_CFG_GLBM_SOFT_RST_GLB_DISPLAY_LGC_N_OFFSET);
    reg16_clrbit(GLB_CTL_M_RB_SOFT_RST_4_REG, GLB_CTL_M_RB_CFG_GLBM_SOFT_RST_GLB_DISPLAY_CRG_N_OFFSET);

    reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_FRC_ZERO_REG, LP_CTL_MCU_RB_RST_SYS_DISPLAY_LGC_N_DBG_FRC_ZERO_OFFSET);
    reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_FRC_ZERO_REG, LP_CTL_MCU_RB_RST_SYS_DISPLAY_CRG_N_DBG_FRC_ZERO_OFFSET);
    reg16_clrbit(LP_CTL_MCU_RB_LP_CTL_MCU_FRC_ONE_REG, LP_CTL_MCU_RB_RST_SYS_DISPLAY_LGC_N_DBG_FRC_ONE_OFFSET);
    reg16_clrbit(LP_CTL_MCU_RB_LP_CTL_MCU_FRC_ONE_REG, LP_CTL_MCU_RB_RST_SYS_DISPLAY_CRG_N_DBG_FRC_ONE_OFFSET);

    reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_1_REG, LP_CTL_MCU_RB_DISPLAY_SUB_ISO_EN_MAN_OFFSET);
    reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_SEL_1_REG, LP_CTL_MCU_RB_DISPLAY_SUB_ISO_EN_MAN_SEL_OFFSET);

    reg16_clrbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_1_REG, LP_CTL_MCU_RB_DISPLAY_SUB_PWR_EN_MAN_OFFSET);
    reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_SEL_1_REG, LP_CTL_MCU_RB_DISPLAY_SUB_PWR_EN_MAN_SEL_OFFSET);

    g_pmu_sub_info[PMU_DISPLAY_SUB].poweroff_cnt++;
    osal_irq_restore(irq);
}

static void pm_vau_power_on(void)
{
    uint32_t irq = osal_irq_lock();

    reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_1_REG, LP_CTL_MCU_RB_DISPLAY_VAU_PWR_EN_MAN_OFFSET);
    reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_SEL_1_REG, LP_CTL_MCU_RB_DISPLAY_VAU_PWR_EN_MAN_SEL_OFFSET);

    reg16_clrbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_1_REG, LP_CTL_MCU_RB_DISPLAY_VAU_ISO_EN_MAN_OFFSET);
    reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_SEL_1_REG, LP_CTL_MCU_RB_DISPLAY_VAU_ISO_EN_MAN_SEL_OFFSET);

    while (reg16_getbit(LP_CTL_MCU_RB_LP_CTL_MCU_PWR_ACK_REG, LP_CTL_MCU_RB_DISPLAY_VAU_PWR_ACK_OFFSET) != 1) {
    }

    reg16_clrbit(LP_CTL_MCU_RB_LP_CTL_MCU_FRC_ZERO_REG, LP_CTL_MCU_RB_RST_SYS_DISPLAY_VAU_LGC_N_DBG_FRC_ZERO_OFFSET);
    reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_FRC_ONE_REG, LP_CTL_MCU_RB_RST_SYS_DISPLAY_VAU_LGC_N_DBG_FRC_ONE_OFFSET);

    reg16_setbit(GLB_CTL_M_RB_SOFT_RST_4_REG, GLB_CTL_M_RB_CFG_GLBM_SOFT_RST_GLB_DISPLAY_VAU_LGC_N_OFFSET);

    while (reg16_getbit(LP_CTL_MCU_RB_LP_CTL_MCU_STS_5_REG, LP_CTL_MCU_RB_RST_SYS_DISPLAY_VAU_LGC_N_STS_OFFSET) != 1) {
    }

    reg16_setbit(VIDEO_CTL_RB_VIDEO_CRG_SOFT_RST_REG, VIDEO_CTL_RB_CFG_VAU_SOFT_RST_SYS_N_OFFSET);

    g_pmu_sub_info[PMU_VAU_SUB].poweron_cnt++;
    osal_irq_restore(irq);
}

static void pm_vau_power_off(void)
{
    uint32_t irq = osal_irq_lock();

    reg16_clrbit(VIDEO_CTL_RB_VIDEO_CRG_SOFT_RST_REG, VIDEO_CTL_RB_CFG_VAU_SOFT_RST_SYS_N_OFFSET);

    reg16_clrbit(GLB_CTL_M_RB_SOFT_RST_4_REG, GLB_CTL_M_RB_CFG_GLBM_SOFT_RST_GLB_DISPLAY_VAU_LGC_N_OFFSET);

    reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_FRC_ZERO_REG, LP_CTL_MCU_RB_RST_SYS_DISPLAY_VAU_LGC_N_DBG_FRC_ZERO_OFFSET);
    reg16_clrbit(LP_CTL_MCU_RB_LP_CTL_MCU_FRC_ONE_REG, LP_CTL_MCU_RB_RST_SYS_DISPLAY_VAU_LGC_N_DBG_FRC_ONE_OFFSET);

    reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_1_REG, LP_CTL_MCU_RB_DISPLAY_VAU_ISO_EN_MAN_OFFSET);
    reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_SEL_1_REG, LP_CTL_MCU_RB_DISPLAY_VAU_ISO_EN_MAN_SEL_OFFSET);

    reg16_clrbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_1_REG, LP_CTL_MCU_RB_DISPLAY_VAU_PWR_EN_MAN_OFFSET);
    reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_SEL_1_REG, LP_CTL_MCU_RB_DISPLAY_VAU_PWR_EN_MAN_SEL_OFFSET);

    g_pmu_sub_info[PMU_VAU_SUB].poweroff_cnt++;
    osal_irq_restore(irq);
}

static void pm_h264_power_on(void)
{
    uint32_t irq = osal_irq_lock();
    if (reg16_getbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_1_REG, LP_CTL_MCU_RB_DISPLAY_SUB_PWR_EN_MAN_OFFSET) == 0) {
        PRINT("h264 on, but display off.\r\n");
    }
    reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_REG, LP_CTL_MCU_RB_DISPLAY_H264_PWR_EN_MAN_OFFSET);
    reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_SEL_REG, LP_CTL_MCU_RB_DISPLAY_H264_PWR_EN_MAN_SEL_OFFSET);

    reg16_clrbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_REG, LP_CTL_MCU_RB_DISPLAY_H264_ISO_EN_MAN_OFFSET);
    reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_SEL_REG, LP_CTL_MCU_RB_DISPLAY_H264_ISO_EN_MAN_SEL_OFFSET);

    while (reg16_getbit(LP_CTL_MCU_RB_LP_CTL_MCU_PWR_ACK_REG, LP_CTL_MCU_RB_DISPLAY_H264_PWR_ACK_OFFSET) != 1) {
    }

    reg16_clrbit(LP_CTL_MCU_RB_LP_CTL_MCU_FRC_ZERO_REG, LP_CTL_MCU_RB_RST_SYS_DISPLAY_H264_LGC_N_DBG_FRC_ZERO_OFFSET);
    reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_FRC_ONE_REG, LP_CTL_MCU_RB_RST_SYS_DISPLAY_H264_LGC_N_DBG_FRC_ONE_OFFSET);

    reg16_setbit(GLB_CTL_M_RB_SOFT_RST_4_REG, GLB_CTL_M_RB_CFG_GLBM_SOFT_RST_GLB_DISPLAY_H264_LGC_N_OFFSET);

    while (reg16_getbit(LP_CTL_MCU_RB_LP_CTL_MCU_STS_5_REG, LP_CTL_MCU_RB_RST_SYS_DISPLAY_H264_LGC_N_STS_OFFSET) != 1) {
    }

    reg16_setbit(VIDEO_CTL_RB_VIDEO_CRG_SOFT_RST_REG, VIDEO_CTL_RB_CFG_VIDEO_SOFT_RST_JPGD_N_OFFSET);
    reg16_setbit(VIDEO_CTL_RB_VIDEO_CRG_SOFT_RST_REG, VIDEO_CTL_RB_CFG_VIDEO_SOFT_RST_H264_N_OFFSET);
    reg16_setbit(VIDEO_CTL_RB_VIDEO_CRG_SOFT_RST_REG, VIDEO_CTL_RB_CFG_VIDEO_SOFT_VIDEO_AXI2TO1_N_OFFSET);

    g_pmu_sub_info[PMU_H264_SUB].poweron_cnt++;
    osal_irq_restore(irq);
}

static void pm_h264_power_off(void)
{
    uint32_t irq = osal_irq_lock();
    if (reg16_getbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_1_REG, LP_CTL_MCU_RB_DISPLAY_SUB_PWR_EN_MAN_OFFSET) == 0) {
        PRINT("h264 off, but display off.\r\n");
    }
    reg16_clrbit(VIDEO_CTL_RB_VIDEO_CRG_SOFT_RST_REG, VIDEO_CTL_RB_CFG_VIDEO_SOFT_RST_JPGD_N_OFFSET);
    reg16_clrbit(VIDEO_CTL_RB_VIDEO_CRG_SOFT_RST_REG, VIDEO_CTL_RB_CFG_VIDEO_SOFT_RST_H264_N_OFFSET);
    reg16_clrbit(VIDEO_CTL_RB_VIDEO_CRG_SOFT_RST_REG, VIDEO_CTL_RB_CFG_VIDEO_SOFT_VIDEO_AXI2TO1_N_OFFSET);

    reg16_clrbit(GLB_CTL_M_RB_SOFT_RST_4_REG, GLB_CTL_M_RB_CFG_GLBM_SOFT_RST_GLB_DISPLAY_H264_LGC_N_OFFSET);

    reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_FRC_ZERO_REG, LP_CTL_MCU_RB_RST_SYS_DISPLAY_H264_LGC_N_DBG_FRC_ZERO_OFFSET);
    reg16_clrbit(LP_CTL_MCU_RB_LP_CTL_MCU_FRC_ONE_REG, LP_CTL_MCU_RB_RST_SYS_DISPLAY_H264_LGC_N_DBG_FRC_ONE_OFFSET);

    reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_REG, LP_CTL_MCU_RB_DISPLAY_H264_ISO_EN_MAN_OFFSET);
    reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_SEL_REG, LP_CTL_MCU_RB_DISPLAY_H264_ISO_EN_MAN_SEL_OFFSET);

    reg16_clrbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_REG, LP_CTL_MCU_RB_DISPLAY_H264_PWR_EN_MAN_OFFSET);
    reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_SEL_REG, LP_CTL_MCU_RB_DISPLAY_H264_PWR_EN_MAN_SEL_OFFSET);

    g_pmu_sub_info[PMU_H264_SUB].poweroff_cnt++;
    osal_irq_restore(irq);
}

static void pm_bt_power_on(void)
{
    if (readw(GLB_CTL_M_RB_SOFT_RST_2_REG) != 0) {
        return;
    }

    pmp_mcu_to_btram_config(1);
    uint32_t irq = osal_irq_lock();
    reg16_setbit(GLB_CTL_M_RB_CLB_CLKEN1_REG, GLB_CTL_M_RB_CFG_GLBM_BT_32M_XO_SEL_OFFSET);
    uapi_pmu_mem_control(PMU_MEM_BT_1_RAM, PMU_MEM_TURN_ON);
    uapi_pmu_mem_control(PMU_MEM_BT_2_RAM, PMU_MEM_TURN_ON);
    uapi_pmu_mem_control(PMU_MEM_BT_3_RAM, PMU_MEM_TURN_ON);
    uapi_pmu_mem_control(PMU_MEM_BT_4_RAM, PMU_MEM_TURN_ON);
    uapi_pmu_mem_control(PMU_MEM_BT_1_EM, PMU_MEM_TURN_ON);
    uapi_pmu_mem_control(PMU_MEM_BT_2_EM, PMU_MEM_TURN_ON);

    reg16_setbit(LP_CTL_BT_RB_LP_CTL_BT_DEF_2_REG, LP_CTL_BT_RB_BT_FAST_WKUP_MODE_OFFSET);
    reg16_setbits(LP_CTL_BT_RB_LP_CTL_BT_SEQ_DELAY_CNT_REG, LP_CTL_BT_RB_B_SUB_PWR_EN_ON_SEQ_DELAY_CNT_OFFSET,
                  LP_CTL_BT_RB_B_SUB_PWR_EN_ON_SEQ_DELAY_CNT_LEN, 0x3);

    reg16_setbit(GLB_CTL_B_RB_BCPU_CORE_WAIT_REG, GLB_CTL_B_RB_BCPU_CORE_WAIT_OFFSET);
    reg16_setbit(GLB_CTL_M_RB_SOFT_RST_2_REG, GLB_CTL_M_RB_CFG_GLBM_SOFT_RST_GLB_B_CRG_N_OFFSET);
    reg16_setbit(GLB_CTL_M_RB_SOFT_RST_2_REG, GLB_CTL_M_RB_CFG_GLBM_SOFT_RST_GLB_B_LGC_N_OFFSET);
    reg16_setbit(GLB_CTL_M_RB_SOFT_RST_2_REG, GLB_CTL_M_RB_CFG_GLBM_SOFT_RST_GLB_B_CPU_N_OFFSET);

    reg16_setbit(LP_CTL_GLB_RB_LP_CTL_GLB_PWR_EN_REG, LP_CTL_GLB_RB_BT_PWR_EN_OFFSET);

    // 表示bcpu解复位完成
    while (reg16_getbit(LP_CTL_BT_RB_LP_CTL_BT_STS_4_REG, LP_CTL_BT_RB_RST_SYS_BT_CPU_N_STS_OFFSET) != 1) {
    }
    osal_irq_restore(irq);
    writew(LP_CTL_BT_RB_CFG_LP_CTL_BT_GP_REG3_REG, 0);

#ifdef LOAD_IMAGE_FROM_FS
    cpu_load_image_from_fs(FLASH_SIGN_HEADER, CORE_IMAGES_BT);
#else
    cpu_load_image((void *)(uintptr_t)(B_CORE_LOAD_ADDR + FLASH_SIGN_HEADER), CORE_IMAGES_BT);
#endif
#ifdef CONFIG_PRODUCT_EVB_DITING
    feature_table_set(FT_BOARD_ID, EVB_DITING);
#else
    feature_table_set(FT_BOARD_ID, EVB_3322);
#endif
    g_pmu_sub_info[PMU_BT_SUB].poweron_cnt++;
    g_pmu_sub_poweron_flag[PMU_BT_SUB] = 1;
    reg16_clrbit(GLB_CTL_B_RB_BCPU_CORE_WAIT_REG, GLB_CTL_B_RB_BCPU_CORE_WAIT_OFFSET);
    pmp_mcu_to_btram_config(0);
}

static void pm_bt_power_off_prepare(void)
{
#define PM_WAIT_BT_STATE_TIMEOUT_MS 100
    uint32_t msg = 0;
    pm_core_ipc_send_msg((uint8_t *)(&msg), sizeof(uint32_t), CORES_BT_CORE);
    osDelay(PM_CORE_PWROFF_TIMEOUT_MS);
    uint64_t start_stamp = uapi_tcxo_get_ms();
    while (1) {
        if (readw(LP_CTL_BT_RB_CFG_LP_CTL_BT_GP_REG3_REG) == BT_POWER_OFF_PREPARE_FINISH_FLAG) {
            PRINT("bt sleep state:0x%x.\r\n", readw(GLB_CTL_B_RB_TGLP_STATUS_REG));
            break;
        }
        if ((uapi_tcxo_get_ms() - start_stamp) > PM_WAIT_BT_STATE_TIMEOUT_MS) {
            PRINT("timeout,b state:0x%x.\r\n", readw(GLB_CTL_B_RB_TGLP_STATUS_REG));
            break;
        }
    }
    return;
}

static void pm_bt_power_off(void)
{
    if (readw(GLB_CTL_M_RB_SOFT_RST_2_REG) == 0) {
        return;
    }
    pm_bt_power_off_prepare();
    uint32_t irq = osal_irq_lock();
    reg16_clrbit(GLB_CTL_M_RB_SOFT_RST_2_REG, GLB_CTL_M_RB_CFG_GLBM_SOFT_RST_GLB_B_CPU_N_OFFSET);
    reg16_clrbit(GLB_CTL_M_RB_SOFT_RST_2_REG, GLB_CTL_M_RB_CFG_GLBM_SOFT_RST_GLB_B_LGC_N_OFFSET);
    reg16_clrbit(GLB_CTL_M_RB_SOFT_RST_2_REG, GLB_CTL_M_RB_CFG_GLBM_SOFT_RST_GLB_B_CRG_N_OFFSET);

    reg16_clrbit(LP_CTL_GLB_RB_LP_CTL_GLB_PWR_EN_REG, LP_CTL_GLB_RB_BT_PWR_EN_OFFSET);
    while (reg16_getbit(LP_CTL_BT_RB_LP_CTL_BT_STS_4_REG, LP_CTL_BT_RB_RST_SYS_BT_CPU_N_STS_OFFSET) != 0) {
    }
    // bt ram and em shut down
    uapi_pmu_mem_control(PMU_MEM_BT_1_RAM, PMU_MEM_SHUT_DOWN);
    uapi_pmu_mem_control(PMU_MEM_BT_2_RAM, PMU_MEM_SHUT_DOWN);
    uapi_pmu_mem_control(PMU_MEM_BT_3_RAM, PMU_MEM_SHUT_DOWN);
    uapi_pmu_mem_control(PMU_MEM_BT_4_RAM, PMU_MEM_SHUT_DOWN);
    uapi_pmu_mem_control(PMU_MEM_BT_1_EM, PMU_MEM_SHUT_DOWN);
    uapi_pmu_mem_control(PMU_MEM_BT_2_EM, PMU_MEM_SHUT_DOWN);

    g_pmu_sub_info[PMU_BT_SUB].poweroff_cnt++;
    g_pmu_sub_poweron_flag[PMU_BT_SUB] = 0;
    osal_irq_restore(irq);
}

static void pm_audio_power_on(void)
{
    PRINT("pm_audio_power_on start.\r\n");
    if (reg16_getbit(LP_CTL_GLB_RB_LP_CTL_GLB_PWR_EN_REG, LP_CTL_GLB_RB_AUDIO_PWR_EN_OFFSET) == 1) {
        return;
    }

    uint32_t irq = osal_irq_lock();

    uint32_t hifi_power_mode = clocks_get_last_dsp_freq_level();
    if ((hifi_power_mode > HIFI_LPM_CLK_LOW_LEVEL) && (hifi_power_mode < HIFI_LPM_CLK_NONE)) {
        uapi_set_hifi_mode(hifi_power_mode);
    } else {
        uapi_set_hifi_mode(HIFI_LPM_CLK_LOW_LEVEL);
    }

    reg16_setbit(COM_CTL_RB_GLB_CLKEN_REG, COM_CTL_RB_CFG_COM_BUS_DSP_CLKEN_OFFSET);
    reg16_setbit(M_CTL_RB_M_CLKEN0_REG, M_CTL_RB_CFG_M_DSP2MEM_X2X_CLKEN_OFFSET);

    reg16_setbits(LP_CTL_AUDIO_RB_LP_CTL_AUDIO_SEQ_DELAY_CNT_REG,
                  LP_CTL_AUDIO_RB_AUDIO_SUB_PWR_EN_ON_SEQ_DELAY_CNT_OFFSET,
                  LP_CTL_AUDIO_RB_AUDIO_SUB_PWR_EN_ON_SEQ_DELAY_CNT_LEN,
                  0x3);

    reg16_setbit(GLB_CTL_D_RB_AUDIO_SOFT_RST_CFG_REG, GLB_CTL_D_RB_CFG_GLBD_SOFT_RST_GLB_AUDIO_CRG_N_OFFSET);
    reg16_setbit(GLB_CTL_D_RB_AUDIO_SOFT_RST_CFG_REG, GLB_CTL_D_RB_CFG_GLBD_SOFT_RST_GLB_AUDIO_LGC_N_OFFSET);
    reg16_setbit(GLB_CTL_D_RB_AUDIO_SOFT_RST_CFG_REG, GLB_CTL_D_RB_CFG_GLBD_SOFT_RST_GLB_AUDIO_CPU_N_OFFSET);

    reg16_setbit(GLB_CTL_M_RB_GLB_CRG_CLKEN_REG, GLB_CTL_M_RB_CFG_GLBM_AUDIO_32M_CLKEN_OFFSET);

    writew(COM_CTL_RB_CODEC_DIG_RW_REG_L_REG, 0xFFFF);
    writew(COM_CTL_RB_CODEC_DIG_RW_REG_H_REG, 0xFFFF);

    reg16_setbit(LP_CTL_AUDIO_RB_LP_CTL_AUDIO_DEF_REG, LP_CTL_AUDIO_RB_AUDIO_FAST_WKUP_MODE_OFFSET);

    reg16_setbit(LP_CTL_GLB_RB_LP_CTL_GLB_PWR_EN_REG, LP_CTL_GLB_RB_AUDIO_PWR_EN_OFFSET);
    while (reg16_getbit(LP_CTL_AUDIO_RB_LP_CTL_AUDIO_STS_1_REG, LP_CTL_AUDIO_RB_RST_SYS_AUDIO_CPU_N_STS_OFFSET) != 1) {
    }

    if (uapi_pmu_ldo_get_voltage(PMU_BUCK_LDO_BUCK_0P8) > PMU_0P9_VSET_0V8) {
        uapi_pmu_mem_codec_0p9_set();
    } else {
        uapi_pmu_mem_codec_0p8_set();
    }

#ifdef CFG_DRIVERS_QSPI
    clocks_ccrg_set_frequency(HAL_CLOCKS_MODULE_XIP_QSPI2, CLOCKS_SRC_XO, 0x1);
#endif
    g_pmu_sub_info[PMU_AUDIO_SUB].poweron_cnt++;
    g_pmu_sub_poweron_flag[PMU_AUDIO_SUB] = 1;
    g_pmu_sub_info[PMU_AUDIO_SUB].time_stamp = get_sub_running_timestamp();
    osal_irq_restore(irq);

    PRINT("pm_audio_power_on end.\r\n");
}

static void pm_audio_power_off(void)
{
    PRINT("pm_audio_power_off start.\r\n");
    if (reg16_getbit(LP_CTL_GLB_RB_LP_CTL_GLB_PWR_EN_REG, LP_CTL_GLB_RB_AUDIO_PWR_EN_OFFSET) == 0) {
        return;
    }

    uint32_t irq = osal_irq_lock();
    writew(COM_CTL_RB_CODEC_DIG_RW_REG_L_REG, 0);
    writew(COM_CTL_RB_CODEC_DIG_RW_REG_H_REG, 0);

    reg16_clrbit(GLB_CTL_M_RB_GLB_CRG_CLKEN_REG, GLB_CTL_M_RB_CFG_GLBM_AUDIO_32M_CLKEN_OFFSET);

    reg16_clrbit(GLB_CTL_D_RB_AUDIO_SOFT_RST_CFG_REG, GLB_CTL_D_RB_CFG_GLBD_SOFT_RST_GLB_AUDIO_CPU_N_OFFSET);
    reg16_clrbit(GLB_CTL_D_RB_AUDIO_SOFT_RST_CFG_REG, GLB_CTL_D_RB_CFG_GLBD_SOFT_RST_GLB_AUDIO_LGC_N_OFFSET);
    reg16_clrbit(GLB_CTL_D_RB_AUDIO_SOFT_RST_CFG_REG, GLB_CTL_D_RB_CFG_GLBD_SOFT_RST_GLB_AUDIO_CRG_N_OFFSET);
    reg16_clrbit(LP_CTL_GLB_RB_LP_CTL_GLB_PWR_EN_REG, LP_CTL_GLB_RB_AUDIO_PWR_EN_OFFSET);
    while (reg16_getbit(LP_CTL_AUDIO_RB_LP_CTL_AUDIO_STS_1_REG, LP_CTL_AUDIO_RB_RST_SYS_AUDIO_CPU_N_STS_OFFSET) != 0) {
    }

#ifdef CFG_DRIVERS_QSPI
    clocks_ccrg_set_frequency(HAL_CLOCKS_MODULE_XIP_QSPI2, CLOCKS_SRC_MAX, 0x1);
#endif
    uapi_set_hifi_mode(HIFI_LPM_CLK_NONE);
    reg16_clrbit(COM_CTL_RB_GLB_CLKEN_REG, COM_CTL_RB_CFG_COM_BUS_DSP_CLKEN_OFFSET);
    reg16_clrbit(M_CTL_RB_M_CLKEN0_REG, M_CTL_RB_CFG_M_DSP2MEM_X2X_CLKEN_OFFSET);
    osal_irq_restore(irq);

    g_pmu_sub_info[PMU_AUDIO_SUB].poweroff_cnt++;
    g_pmu_sub_poweron_flag[PMU_AUDIO_SUB] = 0;
    g_pmu_sub_info[PMU_AUDIO_SUB].runtime = get_sub_running_timestamp() - g_pmu_sub_info[PMU_AUDIO_SUB].time_stamp;
    uapi_diag_info_log(0, "DSP0 power off\n");
    PRINT("pm_audio_power_off end.\r\n");
}

static void pm_audio_exit_deepsleep(void)
{
    PRINT("pm_audio_exit_deepsleep start.\r\n");
    if (reg16_getbits(LP_CTL_AUDIO_RB_LP_CTL_AUDIO_STS_REG, LP_CTL_AUDIO_RB_AUDIO_SYS_CUR_STS_OFFSET,
        LP_CTL_AUDIO_RB_AUDIO_SYS_CUR_STS_LEN) == 0x3) {
        return;
    }

    uint32_t irq = osal_irq_lock();

    uint32_t hifi_power_mode = clocks_get_last_dsp_freq_level();
    if ((hifi_power_mode > HIFI_LPM_CLK_LOW_LEVEL) && (hifi_power_mode < HIFI_LPM_CLK_NONE)) {
        uapi_set_hifi_mode(hifi_power_mode);
    } else {
        uapi_set_hifi_mode(HIFI_LPM_CLK_LOW_LEVEL);
    }
    // 去睡眠投票
    reg16_clrbit(LP_CTL_AUDIO_RB_LP_CTL_AUDIO_ALLOW_TO_SLEEP_REG, LP_CTL_AUDIO_RB_AUDIO_ALLOW_TO_SLEEP_OFFSET);
    // ipc中断唤醒dsp
    reg16_setbit(M_CTL_RB_M2X_SOFT_INT_SET_REG, M_CTL_RB_M2D_INT0_SET_OFFSET);
    // 等work态
    while (reg16_getbits(LP_CTL_AUDIO_RB_LP_CTL_AUDIO_STS_REG, LP_CTL_AUDIO_RB_AUDIO_SYS_CUR_STS_OFFSET,
        LP_CTL_AUDIO_RB_AUDIO_SYS_CUR_STS_LEN) != 0x3) {
        ;
    }
    // 关闭IPC唤醒使能和中断使能
    writew(LP_CTL_AUDIO_RB_LP_CTL_AUDIO_WKUP_EN_REG, 0);
    writew(LP_CTL_AUDIO_RB_LP_CTL_AUDIO_INTR_EN_REG, 0x0);
    // 清除IPC事件和中断
    writew(LP_CTL_AUDIO_RB_LP_CTL_AUDIO_WKUP_CLR_REG, 0x8);
    writew(LP_CTL_AUDIO_RB_LP_CTL_AUDIO_WKUP_INT_CLR_REG, 0x8);

    if (uapi_pmu_ldo_get_voltage(PMU_BUCK_LDO_BUCK_0P8) > PMU_0P9_VSET_0V8) {
        uapi_pmu_mem_codec_0p9_set();
    } else {
        uapi_pmu_mem_codec_0p8_set();
    }

#ifdef CFG_DRIVERS_QSPI
    clocks_ccrg_set_frequency(HAL_CLOCKS_MODULE_XIP_QSPI2, CLOCKS_SRC_XO, 0x1);
#endif
    g_pmu_sub_info[PMU_AUDIO_SUB].poweron_cnt++;
    g_pmu_sub_poweron_flag[PMU_AUDIO_SUB] = 1;
    g_pmu_sub_info[PMU_AUDIO_SUB].time_stamp = get_sub_running_timestamp();
    osal_irq_restore(irq);

    PRINT("pm_audio_exit_deepsleep end.\r\n");
}

static void pm_audio_enter_deepsleep(void)
{
    PRINT("pm_audio_enter_deepsleep start.\r\n");
    if (reg16_getbits(LP_CTL_AUDIO_RB_LP_CTL_AUDIO_STS_REG, LP_CTL_AUDIO_RB_AUDIO_SYS_CUR_STS_OFFSET,
        LP_CTL_AUDIO_RB_AUDIO_SYS_CUR_STS_LEN) == 0x6) {
        return;
    }

    uint32_t irq = osal_irq_lock();
    // 关闭其他唤醒源
    writew(LP_CTL_AUDIO_RB_LP_CTL_AUDIO_WKUP_EN_REG, 0);
    // 打开IPC唤醒使能和中断使能
    reg16_setbit(LP_CTL_AUDIO_RB_LP_CTL_AUDIO_WKUP_EN_REG, LP_CTL_AUDIO_RB_AUDIO_IPC_INTR_WKUP_EN_OFFSET);
    writew(LP_CTL_AUDIO_RB_LP_CTL_AUDIO_INTR_EN_REG, 0x30);
    // 配置sub下电
    reg16_setbit(LP_CTL_AUDIO_RB_LP_CTL_AUDIO_SLP_VAL_REG, LP_CTL_AUDIO_RB_AUDIO_SUB_ISO_EN_SLP_VAL_OFFSET);
    reg16_clrbit(LP_CTL_AUDIO_RB_LP_CTL_AUDIO_SLP_VAL_REG, LP_CTL_AUDIO_RB_AUDIO_SUB_PWR_EN_SLP_VAL_OFFSET);
    // 配置允许睡眠
    reg16_setbit(LP_CTL_AUDIO_RB_LP_CTL_AUDIO_ALLOW_TO_SLEEP_REG, LP_CTL_AUDIO_RB_AUDIO_ALLOW_TO_SLEEP_OFFSET);
    // 等待sleep态
    while (reg16_getbits(LP_CTL_AUDIO_RB_LP_CTL_AUDIO_STS_REG, LP_CTL_AUDIO_RB_AUDIO_SYS_CUR_STS_OFFSET,
        LP_CTL_AUDIO_RB_AUDIO_SYS_CUR_STS_LEN) != 0x6) {
        ;
    }
    // 清除睡眠事件
    reg16_setbit(LP_CTL_AUDIO_RB_LP_CTL_AUDIO_SLEEPING_REG, LP_CTL_AUDIO_RB_AUDIO_CPU_SLEEPING_CLR_OFFSET);

#ifdef CFG_DRIVERS_QSPI
    clocks_ccrg_set_frequency(HAL_CLOCKS_MODULE_XIP_QSPI2, CLOCKS_SRC_MAX, 0x1);
#endif
    uapi_set_hifi_mode(HIFI_LPM_CLK_NONE);
    g_pmu_sub_info[PMU_AUDIO_SUB].poweroff_cnt++;
    g_pmu_sub_poweron_flag[PMU_AUDIO_SUB] = 0;
    g_pmu_sub_info[PMU_AUDIO_SUB].runtime = get_sub_running_timestamp() - g_pmu_sub_info[PMU_AUDIO_SUB].time_stamp;
    osal_irq_restore(irq);

    uapi_diag_info_log(0, "DSP0 enter deepsleep\n");
    PRINT("pm_audio_enter_deepsleep end.\r\n");
}

typedef void (*pmu_sub_control_func)(void);

typedef struct {
    pmu_sub_control_func power_on;
    pmu_sub_control_func power_off;
} pmu_sub_control_t;

typedef struct {
    pmu_sub_control_func exit_ds;
    pmu_sub_control_func enter_ds;
} pmu_sub_lowpower_t;

static const pmu_sub_control_t g_pmu_sub_control_func[] = {
    { pm_bt_power_on, pm_bt_power_off },
    { pm_audio_power_on, pm_audio_power_off },
    { pm_display_power_on, pm_display_power_off },
    { pm_vau_power_on, pm_vau_power_off },
    { pm_h264_power_on, pm_h264_power_off }, // H264
    { pm_mcu_hs_perp_power_on, pm_mcu_hs_perp_power_off },
};

static const pmu_sub_lowpower_t g_pmu_sub_deepsleep_func[] = {
    { NULL, NULL },
    { pm_audio_exit_deepsleep, pm_audio_enter_deepsleep },
};

errcode_t uapi_pmu_control(pmu_control_type_t type, uint8_t param)
{
    if (type >= PMU_CONTROL_MAX) {
        return ERRCODE_INVALID_PARAM;
    }

    if (type >= PMU_DEV_M_BASE) {
        uapi_pmu_perp_control(type, param);
        return ERRCODE_SUCC;
    }

    if (param == PMU_POWER_ON_AND_RUN) {
        g_pmu_sub_control_func[type].power_on();
    } else if (param == PMU_POWER_SLEEP) {
        if (type < sizeof(g_pmu_sub_deepsleep_func) / sizeof(pmu_sub_lowpower_t) &&
            g_pmu_sub_deepsleep_func[type].enter_ds != NULL) {
            g_pmu_sub_deepsleep_func[type].enter_ds();
        }
    } else if (param == PMU_POWER_RESUME) {
        if (type < sizeof(g_pmu_sub_deepsleep_func) / sizeof(pmu_sub_lowpower_t) &&
            g_pmu_sub_deepsleep_func[type].exit_ds != NULL) {
            g_pmu_sub_deepsleep_func[type].exit_ds();
        }
    } else {
        g_pmu_sub_control_func[type].power_off();
    }

    return ERRCODE_SUCC;
}

pmu_sub_info_t *get_pmu_sub_info(void)
{
    return g_pmu_sub_info;
}

uint8_t get_pmu_audio_poweron_flag(void)
{
    return g_pmu_sub_poweron_flag[PMU_AUDIO_SUB];
}

void startup_other_core(void)
{
    uapi_pmu_control(PMU_BT_SUB, PMU_POWER_ON_AND_RUN);
#ifdef SUPPORT_DISPLAY_TECH
    uapi_pmu_control(PMU_DISPLAY_SUB, PMU_POWER_ON_AND_RUN);
    uapi_pmu_control(PMU_VAU_SUB, PMU_POWER_ON_AND_RUN);
#endif

#ifdef CFG_DRIVERS_SPINOR
    pmp_norflash_config(0); // 关闭norflash总线访问
#endif
}

typedef struct {
    uint32_t reg_addr;
    uint32_t reg_off;
} mcu_hs_sub_clk_en_set_t;

static const mcu_hs_sub_clk_en_set_t g_mcu_hs_sub_clken_reg[HS_SUB_MAX_ID] = {
    { HS_CTL_RB_HS_CLK_EN_REG, HS_CTL_RB_CFG_HS_FMC_BUS_CLKEN_OFFSET },
    { HS_CTL_RB_HS_CLK_EN_REG, HS_CTL_RB_CFG_HS_EMMC_CLKEN_OFFSET },
    { HS_CTL_RB_HS_CLK_EN_REG, HS_CTL_RB_CFG_HS_SDIO_CLKEN_OFFSET },
    { HS_CTL_RB_USBC_CRG_CTL_REG, HS_CTL_RB_CFG_HS_USB2_BUS_CKEN_OFFSET },
    { HS_CTL_RB_HS_SUB_CAN_CFG_REG, HS_CTL_RB_CFG_HS_CAN_CLKEN_OFFSET },
    { HS_CTL_RB_VICAP_CTRL_CFG0_REG, HS_CTL_RB_CFG_HS_VICAP_BUS_CLKEN_OFFSET },
};
static uint8_t g_mcu_hs_sub_pwr_flag = 0;

errcode_t uapi_pmu_hs_sub_control(uint8_t sid, uint8_t param)
{
    if (sid >= HS_SUB_MAX_ID) {
        return ERRCODE_INVALID_PARAM;
    }

    if (sid == HS_SUB_VICAP_ID) {
        if (param == PMU_POWER_ON_AND_RUN) {
            uapi_pm_add_sleep_veto_with_symmetry(PM_ID_VICAP);
            uapi_clocks_config_mode(PM_ID_VICAP, PM_MODE_PERFORMACE);
            clocks_ccrg_set_frequency(HAL_CLOCKS_MODULE_SDIOM, CLOCKS_SRC_PLL, 0x4); // 200MHz
        } else {
            uapi_pm_remove_sleep_veto_with_symmetry(PM_ID_VICAP);
            clocks_ccrg_set_frequency(HAL_CLOCKS_MODULE_SDIOM, CLOCKS_SRC_MAX, 0x1); // close
            uapi_clocks_config_mode(PM_ID_VICAP, PM_MODE_LP);
        }
    }

    uint32_t irq = osal_irq_lock();
    if (param == PMU_POWER_ON_AND_RUN) {
        if (g_mcu_hs_sub_pwr_flag == 0) {
            uapi_pmu_control(PMU_MCU_HS_PERP_SUB, PMU_POWER_ON_AND_RUN);
        }
        reg16_setbit(g_mcu_hs_sub_clken_reg[sid].reg_addr, g_mcu_hs_sub_clken_reg[sid].reg_off);
        g_mcu_hs_sub_pwr_flag |= (uint16_t)(1U << sid);
    } else {
        g_mcu_hs_sub_pwr_flag &= ~(uint16_t)(1U << sid);
        reg16_clrbit(g_mcu_hs_sub_clken_reg[sid].reg_addr, g_mcu_hs_sub_clken_reg[sid].reg_off);
        if (g_mcu_hs_sub_pwr_flag == 0) {
            uapi_pmu_control(PMU_MCU_HS_PERP_SUB, PMU_POWER_OFF);
        }
    }
    osal_irq_restore(irq);

    return ERRCODE_SUCC;
}

