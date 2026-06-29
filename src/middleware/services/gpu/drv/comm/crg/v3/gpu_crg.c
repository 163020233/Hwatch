/* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved. */

#include "gpu_crg.h"
#include "securec.h"
#include "osal_spinlock.h"
#include "platform_core.h"
#include "soc_reg.h"
#include "pm_clocks.h"
#include "crg_reg_define.h"
#include "chip_io.h"
#include "pm_pmu.h"
#include "dpu_reg_define.h"
#include "gpu_crg_comm.h"
#include "gpu_dfx.h"

td_u32 gpu_crg_get_clk_gpu_cfg(drv_dpu_work_scene work_scene, gpu_crg_freq_state freq_state);

PLT_SEC_ITCM_T td_void gpu_crg_set_vau_clken(td_bool clken)
{
    // GPU的clk en
    reg_video_crg_clken video_crg_clken;
    uapi_reg_read(CRG_VEDIO_CRG_CKEN_CTL, video_crg_clken.u32);
    video_crg_clken.bits.cfg_video_vau_axi_clken = clken;
    video_crg_clken.bits.cfg_video_vau_apb_clken = clken;
    video_crg_clken.bits.cfg_video_vau_ppc_clken = clken;
    uapi_reg_write(CRG_VEDIO_CRG_CKEN_CTL, video_crg_clken.u32);
}
PLT_SEC_ITCM_T td_void gpu_crg_vau_reset(td_bool reset_req)
{
    reg_vau_dpu_srst_req vau_dpu_srst_req;
    uapi_reg_read(CRG_VEDIO_SRST_REG, vau_dpu_srst_req.u32);
    vau_dpu_srst_req.bits.vau_srst_req = reset_req;
    uapi_reg_write(CRG_VEDIO_SRST_REG, vau_dpu_srst_req.u32);

    // vau reset成功回读
    gpu_crg_wait_vau_softrst_succ(reset_req);
}

PLT_SEC_ITCM_T td_void gpu_crg_set_dpu_clken(td_bool clken)
{
    reg_video_crg_clken video_crg_clken;
    uapi_reg_read(CRG_VEDIO_CRG_CKEN_CTL, video_crg_clken.u32);
    video_crg_clken.bits.cfg_video_dpu_axi_clken = clken;
    video_crg_clken.bits.cfg_video_dpu_apb_clken = clken;
    video_crg_clken.bits.cfg_video_dpu_vo_hd0_clken = clken;
    video_crg_clken.bits.cfg_video_dpu_cfg_clken = clken;
    video_crg_clken.bits.cfg_video_dpu_ppc_hd0_clken = clken;
    uapi_reg_write(CRG_VEDIO_CRG_CKEN_CTL, video_crg_clken.u32);
}

PLT_SEC_ITCM_T STATIC td_u32 gpu_crg_is_dpu_softrst_state(td_void)
{
    u_vosrststa vosrststa = {};
    uapi_reg_read(HAL_DPU_SOFTRST_STATE, vosrststa.u32);

    if (vosrststa.bits.srst_sta == HAL_DPU_SOFTRST_VAL) {
        return TD_TRUE;
    }
    return TD_FALSE;
}

PLT_SEC_ITCM_T td_void gpu_crg_dpu_reset(td_bool reset_req)
{
    // 解复位前需要保证之前已复位成功
    if (reset_req == TD_FALSE) {
        td_u32 retry = 0;
        while (gpu_crg_is_dpu_softrst_state() != TD_TRUE) {
            if (retry++ > 0xFF) {
                gpu_err("dpu soft reset fail!");
                break;
            }
        }
    }

    reg_vau_dpu_srst_req vau_dpu_srst_req;
    uapi_reg_read(CRG_VEDIO_SRST_REG, vau_dpu_srst_req.u32);
    vau_dpu_srst_req.bits.dpu_srst_req = reset_req;
    uapi_reg_write(CRG_VEDIO_SRST_REG, vau_dpu_srst_req.u32);
}

PLT_SEC_ITCM_T STATIC td_void gpu_crg_set_dpu_pixel_clk(td_u32 pixel_clk_div_num, td_u32 pixel_clk_div_denom)
{
    reg_video_crg_vo_hd0_div1 video_crg_vo_hd0_div1;
    uapi_reg_read(CRG_VEDIO_DPU_V0_HD0_DIV1, video_crg_vo_hd0_div1.u32);
    video_crg_vo_hd0_div1.bits.cfg_video_vo_hd0_load_div_en = 0;
    uapi_reg_write(CRG_VEDIO_DPU_V0_HD0_DIV1, video_crg_vo_hd0_div1.u32);

    reg_video_crg_vo_hd0_div2 video_crg_vo_hd0_div2;
    uapi_reg_read(CRG_VEDIO_DPU_V0_HD0_DIV2, video_crg_vo_hd0_div2.u32);
    video_crg_vo_hd0_div2.bits.cfg_video_vo_hd0_div_denom = pixel_clk_div_denom;
    uapi_reg_write(CRG_VEDIO_DPU_V0_HD0_DIV2, video_crg_vo_hd0_div2.u32);

    video_crg_vo_hd0_div1.bits.cfg_video_vo_hd0_div_num = pixel_clk_div_num;
    uapi_reg_write(CRG_VEDIO_DPU_V0_HD0_DIV1, video_crg_vo_hd0_div1.u32);

    video_crg_vo_hd0_div1.bits.cfg_video_vo_hd0_load_div_en = 1;
    uapi_reg_write(CRG_VEDIO_DPU_V0_HD0_DIV1, video_crg_vo_hd0_div1.u32);
}

PLT_SEC_ITCM_T STATIC td_void gpu_crg_set_dpu_work_clk(td_u32 work_clk_div_num, td_u32 work_clk_div_denom)
{
    reg_video_crg_ppc_div video_crg_ppc_div;
    uapi_reg_read(CRG_VEDIO_DPU_PPC_DIV, video_crg_ppc_div.u32);
    video_crg_ppc_div.bits.cfg_video_dpu_ppc_load_div_en = 0;
    uapi_reg_write(CRG_VEDIO_DPU_PPC_DIV, video_crg_ppc_div.u32);

    video_crg_ppc_div.bits.cfg_video_dpu_ppc_div_denom = work_clk_div_denom;
    uapi_reg_write(CRG_VEDIO_DPU_PPC_DIV, video_crg_ppc_div.u32);

    reg_video_crg_ppc_div_num video_crg_ppc_div_num;
    video_crg_ppc_div_num.bits.cfg_video_dpu_ppc_div_num = work_clk_div_num;
    uapi_reg_write(CRG_VEDIO_DPU_PPC_DIV_NUM, video_crg_ppc_div_num.u32);

    video_crg_ppc_div.bits.cfg_video_dpu_ppc_load_div_en = 1;
    uapi_reg_write(CRG_VEDIO_DPU_PPC_DIV, video_crg_ppc_div.u32);
}

/*
高低频切换场景：
    1. 上电场景，初始化切高频
    2. 进AOD切低频，退AOD切高频
    3. 深睡唤醒：AOD场景，不需要切频；NORMAL场景，深睡切低频，唤醒切高频
    4. 亮灭屏：灭屏时，如果本来是高频，则切低频，亮屏时恢复高频；灭屏时，如果本来是低频，则不需要切频
*/

PLT_SEC_ITCM_T gpu_crg_freq_state gpu_crg_update_dpu_freq(const dpu_clk_info *cfg_info)
{
    static pm_dpu_mode_t last_mode = PM_MODE_DPU_NONE;
    pm_dpu_mode_t mode = (cfg_info->work_scene == DRV_DPU_WORK_SCENE_AOD) ? PM_MODE_DPU_AOD : PM_MODE_DPU_HIGH_SPEED;

    gpu_crg_freq_state state = GPU_CRG_FREQ_KEEP;
    /* 本来是低频，SUSPEND/RESUME不需要切频 */
    if ((last_mode == PM_MODE_DPU_AOD) &&
        (cfg_info->cfg_scene == GPU_CLK_CFG_SUSPEND || cfg_info->cfg_scene == GPU_CLK_CFG_RESUME)) {
        return state;
    }

    if (cfg_info->cfg_scene == GPU_CLK_CFG_SUSPEND) { // 显示下电切低频，保证整机进入低频
        uapi_pm_set_dpu_freq(PM_MODE_DPU_AOD);
        state = GPU_CRG_FREQ_CHANGED;
    } else if (cfg_info->cfg_scene == GPU_CLK_CFG_RESUME) {
        uapi_pm_set_dpu_freq(last_mode);
    } else { // 初始化 和 NORMAL&AOD场景切换按照传入配置切频
        uapi_pm_set_dpu_freq(mode);
    }
    last_mode = mode;
    return state;
}

PLT_SEC_ITCM_T td_u32 gpu_crg_set_vau_clk(const vau_clk_info *cfg_info)
{
    uapi_unused(cfg_info);
    reg_video_crg_vau_div crg_vau_div;
    uapi_reg_read(CRG_VEDIO_VAU_DIV, crg_vau_div.u32);
    crg_vau_div.bits.cfg_video_vau_load_div_en = 0;
    uapi_reg_write(CRG_VEDIO_VAU_DIV, crg_vau_div.u32);

    // VAU时钟需求为200M，1分频
    crg_vau_div.bits.cfg_video_vau_div_denom = 1;
    uapi_reg_write(CRG_VEDIO_VAU_DIV, crg_vau_div.u32);

    crg_vau_div.bits.cfg_video_vau_load_div_en = 1;
    uapi_reg_write(CRG_VEDIO_VAU_DIV, crg_vau_div.u32);
    return EXT_SUCCESS;
}

PLT_SEC_ITCM_T td_u32 gpu_crg_set_dpu_clk(dpu_clk_info *cfg_info)
{
    gpu_crg_freq_state freq_state = gpu_crg_update_dpu_freq(cfg_info);
    cfg_info->clk_gpu = gpu_crg_get_clk_gpu_cfg(cfg_info->work_scene, freq_state);
    if (cfg_info->clk_gpu == 0) {
        gpu_err("get clk_gpu err! work_scene %d freq_state %d\n", cfg_info->work_scene, freq_state);
        return EXT_FAILURE;
    }

    cfg_info->work_clk = MIN(cfg_info->work_clk, cfg_info->clk_gpu * FREQ_K_PER_M);
    gpu_crg_set_dpu_pixel_clk(cfg_info->pixel_clk * 10 / FREQ_K_PER_M, cfg_info->clk_gpu * 10);  // 10 分频系数提高精度
    gpu_crg_set_dpu_work_clk(GPU_ROUND_DIV_UP(cfg_info->work_clk, FREQ_K_PER_M), cfg_info->clk_gpu);
    return EXT_SUCCESS;
}

STATIC td_u32 gpu_crg_get_dpu_pixel_clk(td_u32 clk_gpu)
{
    reg_video_crg_vo_hd0_div2 video_crg_vo_hd0_div2;
    uapi_reg_read(CRG_VEDIO_DPU_V0_HD0_DIV2, video_crg_vo_hd0_div2.u32);

    reg_video_crg_vo_hd0_div1 video_crg_vo_hd0_div1;
    uapi_reg_read(CRG_VEDIO_DPU_V0_HD0_DIV1, video_crg_vo_hd0_div1.u32);

    td_u32 pixel_clk_div_num = video_crg_vo_hd0_div1.bits.cfg_video_vo_hd0_div_num;
    td_u32 pixel_clk_div_denom = video_crg_vo_hd0_div2.bits.cfg_video_vo_hd0_div_denom;

    if (pixel_clk_div_denom == 0) {
        gpu_err("Divide by zero\n");
        return 0;
    }

    return clk_gpu * pixel_clk_div_num * FREQ_K_PER_M / pixel_clk_div_denom;
}

STATIC td_u32 gpu_crg_get_dpu_work_clk(td_u32 clk_gpu)
{
    reg_video_crg_ppc_div video_crg_ppc_div;
    uapi_reg_read(CRG_VEDIO_DPU_PPC_DIV, video_crg_ppc_div.u32);

    reg_video_crg_ppc_div_num video_crg_ppc_div_num;
    uapi_reg_read(CRG_VEDIO_DPU_PPC_DIV_NUM, video_crg_ppc_div_num.u32);

    if (video_crg_ppc_div.bits.cfg_video_dpu_ppc_div_denom == 0) {
        gpu_err("Divide by zero\n");
        return 0;
    }

    return clk_gpu * FREQ_K_PER_M * video_crg_ppc_div_num.bits.cfg_video_dpu_ppc_div_num /
        video_crg_ppc_div.bits.cfg_video_dpu_ppc_div_denom;
}

#define CLK_GPU_AOD_SEL             1
#define CLK_GPU_HIGH_SPEED_SEL      2
#define CLK_GPU_HIGH_SPEED_DIV_NUM  4
td_u32 gpu_crg_get_clk_gpu_cfg(drv_dpu_work_scene work_scene, gpu_crg_freq_state freq_state)
{
    reg_dpu_cr dpu_cr;
    uapi_reg_read(COM_CTL_RB_DPU_CR_REG, dpu_cr.u32);
    if (dpu_cr.bits.ch_enable == 0) {
        gpu_err("clk gpu ch disabled!\n");
        return 0;
    }

    if (freq_state == GPU_CRG_FREQ_KEEP) {
        if ((work_scene == DRV_DPU_WORK_SCENE_AOD && dpu_cr.bits.clk_sel != CLK_GPU_AOD_SEL) ||
            (work_scene == DRV_DPU_WORK_SCENE_NORMAL && dpu_cr.bits.clk_sel != CLK_GPU_HIGH_SPEED_SEL)) {
            gpu_err("clk_sel error! clk_sel %u\n", dpu_cr.bits.clk_sel);
            return 0;
        }
    }

    if (dpu_cr.bits.clk_sel == CLK_GPU_HIGH_SPEED_SEL) {
        td_u32 fnpll_lock;
        uapi_reg_read(COM_CTL_RB_GLB_CLKEN_REG, fnpll_lock);
        fnpll_lock &= 0x401;  // get bit10 and bit0
        if (fnpll_lock == 0) {
            gpu_err("clk gpu fnpll_lock not set!\n");
            return 0;
        }

        if ((dpu_cr.bits.div_en != 1) || (dpu_cr.bits.div_num == 0)) {
            gpu_err("get clk gpu cfg fail! clk_sel %u div_en %u div_num %u\n", dpu_cr.bits.clk_sel, dpu_cr.bits.div_en,
                dpu_cr.bits.div_num);
            return 0;
        }
    }

    return GPU_ROUND_DIV_UP(clocks_ccrg_get_frequency(HAL_CLOCKS_MODULE_DPU), FREQ_PER_M);
}

td_void gpu_crg_get_dpu_clk(drv_dpu_work_scene work_scene, dpu_clk_info *clk_info)
{
#ifdef PRE_FPGA
    uapi_unused(work_scene);
    clk_info->pixel_clk = 15000; // 15000:15M
    return;
#endif

    clk_info->clk_gpu = gpu_crg_get_clk_gpu_cfg(work_scene, GPU_CRG_FREQ_KEEP);
    clk_info->pixel_clk = gpu_crg_get_dpu_pixel_clk(clk_info->clk_gpu);
    clk_info->work_clk = gpu_crg_get_dpu_work_clk(clk_info->clk_gpu);
}

PLT_SEC_ITCM_T td_void gpu_crg_set_jpeg_clken(td_bool clken)
{
    reg_video_crg_clken video_crg_clken;
    uapi_reg_read(CRG_VEDIO_CRG_CKEN_CTL, video_crg_clken.u32);
    video_crg_clken.bits.cfg_video_jpgd_ppc_clken = clken;
    video_crg_clken.bits.cfg_video_jpgd_apb_clken = clken;
    video_crg_clken.bits.cfg_video_jpgd_axi_clken = clken;
    uapi_reg_write(CRG_VEDIO_CRG_CKEN_CTL, video_crg_clken.u32);

    reg_jpgd_clken jpgd_clken;
    uapi_reg_read(CRG_VEDIO_JPGD_CLKEN, jpgd_clken.u32);
    jpgd_clken.bits.jpgd_cken = clken;
    uapi_reg_write(CRG_VEDIO_JPGD_CLKEN, jpgd_clken.u32);
}

td_void gpu_crg_set_jpeg_apben(td_bool clken)
{
    uapi_unused(clken);
}

PLT_SEC_ITCM_T td_void gpu_crg_jpeg_reset(td_bool reset_req)
{
    reg_jpgd_clken jpgd_clken;
    uapi_reg_read(CRG_VEDIO_JPGD_CLKEN, jpgd_clken.u32);
    jpgd_clken.bits.jpgd_soft_rst_req = reset_req;
    uapi_reg_write(CRG_VEDIO_JPGD_CLKEN, jpgd_clken.u32);
}

typedef struct {
    osal_spinlock pwr_lock;
    video_power_ctrl_mode pwr_sta[GPU_ID_MAX];
} video_pwr_ctrl_info;

STATIC video_pwr_ctrl_info g_pwr_ctrl_info = {};

STATIC td_s32 video_power_ctrl_init(td_void)
{
    (td_void)memset_s(&g_pwr_ctrl_info, sizeof(g_pwr_ctrl_info), 0, sizeof(g_pwr_ctrl_info));

    td_s32 ret = osal_spin_lock_init(&g_pwr_ctrl_info.pwr_lock);
    if (ret != EXT_SUCCESS) {
        gpu_err("osal_spin_lock_init failed");
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

STATIC td_void video_power_ctrl_deinit(td_void)
{
    if (g_pwr_ctrl_info.pwr_lock.lock != TD_NULL) {
        osal_spin_lock_destroy(&g_pwr_ctrl_info.pwr_lock);
    }
}

STATIC td_bool video_check_need_perform_vdec_sub_pwr_ctrl(gpu_module_id id, video_power_ctrl_mode mode)
{
    video_power_ctrl_mode jpeg_pwr_sta = g_pwr_ctrl_info.pwr_sta[GPU_ID_JPEG];
    video_power_ctrl_mode vdec_pwr_sta = g_pwr_ctrl_info.pwr_sta[GPU_ID_VDEC];
    video_power_ctrl_mode jpge_pwr_sta = g_pwr_ctrl_info.pwr_sta[GPU_ID_JPGE];
    if (mode == VIDEO_POWER_OFF) {
        if ((id == GPU_ID_JPEG && (vdec_pwr_sta == VIDEO_POWER_ON || jpge_pwr_sta == VIDEO_POWER_ON)) ||
            (id == GPU_ID_VDEC && (jpeg_pwr_sta == VIDEO_POWER_ON || jpge_pwr_sta == VIDEO_POWER_ON)) ||
            (id == GPU_ID_JPGE && (jpeg_pwr_sta == VIDEO_POWER_ON || vdec_pwr_sta == VIDEO_POWER_ON))) {
            gpu_dbg("module %d cannot perform pwr off! jpeg sta: %d vdec sta: %d jpge sta: %d",
                    id, jpeg_pwr_sta, vdec_pwr_sta, jpge_pwr_sta);
            return TD_FALSE;
        }
    }

    if (mode == VIDEO_POWER_ON) {
        if ((vdec_pwr_sta == VIDEO_POWER_ON) || (jpeg_pwr_sta == VIDEO_POWER_ON) || (jpge_pwr_sta == VIDEO_POWER_ON)) {
            gpu_dbg("module %d no need perform pwr on repeatedly! jpeg sta: %d vdec sta: %d jpge sta: %d",
                id, jpeg_pwr_sta, vdec_pwr_sta, jpge_pwr_sta);
            return TD_FALSE;
        }
    }
    return TD_TRUE;
}

td_void video_power_ctrl(gpu_module_id id, video_power_ctrl_mode mode)
{
    static td_bool is_init = TD_FALSE;
    if (is_init == TD_FALSE) {
        td_s32 rslt = video_power_ctrl_init();
        if (rslt != EXT_SUCCESS) {
            video_power_ctrl_deinit();
            gpu_err("video power ctrl init call osal_spin_lock_init failed!");
            return;
        }
        is_init = TD_TRUE;
    }
    unsigned long flag = 0;
    osal_spin_lock_irqsave(&g_pwr_ctrl_info.pwr_lock, &flag);
    uint8_t param = (mode == VIDEO_POWER_ON) ? PMU_POWER_ON_AND_RUN : PMU_POWER_OFF;
    if (id == GPU_ID_JPEG || id == GPU_ID_VDEC || id == GPU_ID_JPGE) {
        if (video_check_need_perform_vdec_sub_pwr_ctrl(id, mode) == TD_TRUE) {
            uapi_pmu_control(PMU_H264_SUB, param);
        }
        g_pwr_ctrl_info.pwr_sta[id] = mode;
    } else if (id == GPU_ID_VAU) {
        uapi_pmu_control(PMU_VAU_SUB, param);
        g_pwr_ctrl_info.pwr_sta[id] = mode;
    }

    osal_spin_unlock_irqrestore(&g_pwr_ctrl_info.pwr_lock, &flag);
}

td_void gpu_crg_get_vau_clk(td_u32 cfg_clk_vau, vau_clk_info *clk_info)
{
    uapi_unused(cfg_clk_vau);
    uapi_unused(clk_info);
}
