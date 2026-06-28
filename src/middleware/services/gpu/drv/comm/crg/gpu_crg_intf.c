/* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved. */

#include "gpu_crg_intf.h"
#include "platform_core.h"
#include "gpu_dfx.h"
#include "gpu_crg.h"
#include "crg_reg_define.h"

#define DPU_WORK_CLK_MARGIN_MULTIPLE 15 // 工作时钟配置50%余量
#define DPU_WORK_CLK_MARGIN_BASE 10
#define DPU_WORK_CLK_CMP_OUT_MULTIPLE 3
#define DPU_WORK_CLK_ROT_MULTIPLE 5
#define DPU_DEFAULT_PIXEL_CLK 20000

td_u32 g_dpu_normal_pixel_clk = DPU_DEFAULT_PIXEL_CLK;

td_void gpu_crg_set_dpu_normal_pixel_clk(td_u32 pixel_clk)
{
    g_dpu_normal_pixel_clk = pixel_clk;
}

PLT_SEC_ITCM_T STATIC td_u32 gpu_crg_update_dpu_pixel_clk(dpu_clk_scene_info scene_info)
{
    if (g_dpu_normal_pixel_clk == 0) {
        gpu_err("err pixel_clk 0\n");
        g_dpu_normal_pixel_clk = DPU_DEFAULT_PIXEL_CLK;
    }

    // AOD压缩旋转场景pixel_clk固定配置
    if (scene_info.work_scene == DRV_DPU_WORK_SCENE_AOD) {
        if (scene_info.is_rotate || scene_info.is_cmp_out) {
            return DPU_AOD_ROT_PIXEL_CLK;
        }
        return g_dpu_normal_pixel_clk;
    }

    // video mode有严格时序要求，按实际pixel_clk配置
#ifndef GPU_SUPPORT_VIDEO_MODE
    // NORMAL旋转场景性能受限, pixel_clk受work_clk实际最大可配置值限制, 场景上约束不可极小范围局刷
    if (scene_info.is_rotate) {
        return MIN(g_dpu_normal_pixel_clk, DPU_NOR_ROT_MAX_PIXEL_CLK);
    }
#endif

    return g_dpu_normal_pixel_clk;
}

PLT_SEC_ITCM_T td_void gpu_crg_get_dpu_clk_by_scene(dpu_clk_scene_info scene_info, dpu_clk_info *cfg_info)
{
    cfg_info->cfg_scene = scene_info.cfg_scene;
    cfg_info->work_scene = scene_info.work_scene;
    cfg_info->pixel_clk = gpu_crg_update_dpu_pixel_clk(scene_info);

    // 旋转场景 work_clk > pixel_clk * 5 * 1.5
    if (scene_info.is_rotate) {
        cfg_info->work_clk = GPU_ROUND_DIV_UP(cfg_info->pixel_clk * DPU_WORK_CLK_ROT_MULTIPLE *
            DPU_WORK_CLK_MARGIN_MULTIPLE, DPU_WORK_CLK_MARGIN_BASE);
    } else if (scene_info.is_cmp_out) { // 压缩场景 work_clk > pixel_clk * 3 * 1.5
        cfg_info->work_clk = GPU_ROUND_DIV_UP(cfg_info->pixel_clk * DPU_WORK_CLK_CMP_OUT_MULTIPLE *
            DPU_WORK_CLK_MARGIN_MULTIPLE, DPU_WORK_CLK_MARGIN_BASE);
    } else { // 非压非旋转场景 work_clk > pixel_clk * 1.5
        cfg_info->work_clk = GPU_ROUND_DIV_UP(cfg_info->pixel_clk * DPU_WORK_CLK_MARGIN_MULTIPLE,
            DPU_WORK_CLK_MARGIN_BASE);
    }

    td_u32 max_work_clk = (scene_info.work_scene == DRV_DPU_WORK_SCENE_AOD) ? GPU_CLK_AOD * FREQ_K_PER_M : DPU_NOR_MAX_WORK_CLK;
    cfg_info->work_clk = MIN(cfg_info->work_clk, max_work_clk);
}

PLT_SEC_ITCM_T td_void gpu_crg_init(gpu_module_id id, td_void *private_data)
{
    if (id >= GPU_ID_MAX) {
        gpu_err("module id %d is invalid\n", id);
        return;
    }
    td_u32 irq = osal_irq_lock();
    if (id == GPU_ID_VAU) {
        gpu_crg_set_vau_clken(TD_TRUE);
        gpu_crg_set_clk(GPU_ID_VAU, private_data);
        gpu_crg_vau_reset(TD_TRUE);
        gpu_crg_vau_reset(TD_FALSE);
    } else if (id == GPU_ID_DPU) {
        gpu_crg_set_dpu_clken(TD_TRUE);
        gpu_crg_set_clk(GPU_ID_DPU, private_data);
        gpu_crg_dpu_reset(TD_TRUE);
        gpu_crg_dpu_reset(TD_FALSE);
    } else if (id == GPU_ID_JPEG) {
        gpu_crg_set_jpeg_clken(TD_TRUE);
        gpu_crg_jpeg_reset(TD_TRUE);
        gpu_crg_jpeg_reset(TD_FALSE);
    }
    osal_irq_restore(irq);
}

PLT_SEC_ITCM_T td_void gpu_crg_deinit(gpu_module_id id, td_void *private_data)
{
    if (id >= GPU_ID_MAX) {
        gpu_err("module id %d is invalid\n", id);
        return;
    }
    td_u32 irq = osal_irq_lock();
    if (id == GPU_ID_VAU) {
        gpu_crg_vau_reset(TD_TRUE);
        gpu_crg_set_clk(GPU_ID_VAU, private_data);
        gpu_crg_set_vau_clken(TD_FALSE);
    } else if (id == GPU_ID_DPU) {
        gpu_crg_dpu_reset(TD_TRUE);
        gpu_crg_set_clk(GPU_ID_DPU, private_data);
        gpu_crg_set_dpu_clken(TD_FALSE);
    } else if (id == GPU_ID_JPEG) {
        gpu_crg_jpeg_reset(TD_TRUE);
        gpu_crg_set_jpeg_clken(TD_FALSE);
    }
    osal_irq_restore(irq);
}

PLT_SEC_ITCM_T td_u32 gpu_crg_set_clk(gpu_module_id id, td_void *private_data)
{
    if (id >= GPU_ID_MAX) {
        gpu_err("module id %d is invalid\n", id);
        return EXT_FAILURE;
    }
    if (id == GPU_ID_DPU) {
        if (private_data == TD_NULL) {
            gpu_err("input dpu_clk_cfg_info null\n");
            return EXT_FAILURE;
        }
        dpu_clk_scene_info *scene_info = (dpu_clk_scene_info*)private_data;
        if (scene_info->cfg_scene >= GPU_CLK_CFG_MAX || scene_info->work_scene >= DRV_DPU_WORK_SCENE_MAX) {
            gpu_err("err cfg_scene %d work_scene %d \n", scene_info->cfg_scene, scene_info->work_scene);
            return EXT_FAILURE;
        }

        dpu_clk_info cfg_info = {};
        gpu_crg_get_dpu_clk_by_scene(*scene_info, &cfg_info);
        return gpu_crg_set_dpu_clk(&cfg_info);
    } else if (id == GPU_ID_VAU) {
        if (private_data == TD_NULL) {
            gpu_err("input vau_clk_cfg_info null\n");
            return EXT_FAILURE;
        }
        vau_clk_scene_info *scene_info = (vau_clk_scene_info*)private_data;
        if (scene_info->cfg_scene >= GPU_CLK_CFG_MAX || scene_info->work_scene >= DRV_VAU_WORK_SCENE_MAX) {
            gpu_err("err cfg_scene %d work_scene %d \n", scene_info->cfg_scene, scene_info->work_scene);
            return EXT_FAILURE;
        }

        vau_clk_info cfg_info = {};
        cfg_info.cfg_scene = scene_info->cfg_scene;
        cfg_info.work_scene = scene_info->work_scene;
        return gpu_crg_set_vau_clk(&cfg_info);
    }

    return EXT_SUCCESS;
}

td_void gpu_crg_get_clk(gpu_module_id id, td_void *private_data, td_void *clk_data)
{
    uapi_unused(private_data);
    if (id >= GPU_ID_MAX) {
        gpu_err("module id %d is invalid\n", id);
        return;
    }

    if (id == GPU_ID_DPU) {
        drv_dpu_work_scene work_scene = *((drv_dpu_work_scene*)private_data);
        dpu_clk_info *clk_info = (dpu_clk_info*)clk_data;
        gpu_crg_get_dpu_clk(work_scene, clk_info);
    } else if (id == GPU_ID_VAU) {
        drv_vau_work_scene *work_scene = (drv_vau_work_scene*)private_data;
        td_u32 cfg_clk_vau = (*work_scene == DRV_VAU_WORK_SCENE_BALANCE_MODE) ? VAU_CLK_LOW_SPEED : VAU_CLK_HIGH_SPEED;
        vau_clk_info *clk_info = (vau_clk_info*)clk_data;
        gpu_crg_get_vau_clk(cfg_clk_vau, clk_info);
    }
}

td_void uapi_video_power_ctrl(gpu_module_id id, video_power_ctrl_mode mode)
{
    video_power_ctrl(id, mode);
}

td_void gpu_crg_apb_enable(gpu_module_id id, td_bool apb_en)
{
    if (id >= GPU_ID_MAX) {
        gpu_err("module id %d is invalid\n", id);
        return;
    }

    if (id == GPU_ID_JPEG) {
        gpu_crg_set_jpeg_apben(apb_en);
    }
}
