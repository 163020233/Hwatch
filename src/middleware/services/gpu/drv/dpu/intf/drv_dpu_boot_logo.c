/* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. */

#include "drv_dpu_intf.h"
#include "soc_lcd_api.h"
#include "dpu_disp.h"
#include "dpu_layer.h"
#include "dpu_debug.h"

td_s32 uapi_dpu_boot_logo(dpu_boot_logo_info *info)
{
    if (info == NULL) {
        gpu_err("dpu logo null ptr");
        return EXT_FAILURE;
    }
    const ext_rect rect = {0, 0, info->width, info->height};
    dpu_disp_timing timing = {};
    if (disp_get_panel_timing(&rect, &timing, TD_FALSE) != EXT_SUCCESS) {
        gpu_err("disp_get_panel_timing failed, rect[%d %d %u %u]", rect.x, rect.y, rect.width, rect.height);
        return EXT_FAILURE;
    }

    if (((rect.width != timing.hact) || (rect.height != timing.vact))) {
        gpu_err("panel timing not match rect, gp_wh[%u,%u], timing_hact/vact[%u,%u]!",
                rect.width, rect.height, timing.hact, timing.vact);
        return EXT_FAILURE;
    }

    gpu_crg_set_dpu_normal_pixel_clk(timing.pixel_clk);

    dpu_clk_scene_info scene_info = {.cfg_scene = GPU_CLK_CFG_INIT, .work_scene = DRV_DPU_WORK_SCENE_NORMAL,
        .is_cmp_out = TD_FALSE, .is_rotate = TD_FALSE};
    dpu_hal_init(&scene_info);

    disp_set_cbm_default_priority();
    dpu_layer_hal_init(DRV_DPU_LAYER_ID1);

    dpu_hal_disp_set_chnl_enable(TD_TRUE);
    dpu_hal_disp_set_dhd0_te_mode(DPU_DISP_VSYNC_TE_CMD2_MODE);

    dpu_hal_layer_set_global_alpha(DRV_DPU_LAYER_ID1, 0xFF);
    dpu_hal_layer_set_disp_rect(DRV_DPU_LAYER_ID1, &rect);
    dpu_hal_layer_set_crop_rect(DRV_DPU_LAYER_ID1, &rect);
    dpu_hal_layer_set_alpha0_alpha1(DRV_DPU_LAYER_ID1, 0, 0xFF);
    dpu_hal_layer_set_fmt(DRV_DPU_LAYER_ID1, dpu_layer_trans_fmt_drv_to_hal(info->color_fmt));
    dpu_hal_layer_set_width(DRV_DPU_LAYER_ID1, rect.width);
    dpu_hal_layer_set_height(DRV_DPU_LAYER_ID1, rect.height);
    dpu_hal_layer_set_stride(DRV_DPU_LAYER_ID1, info->stride);
    dpu_hal_layer_set_addr(DRV_DPU_LAYER_ID1, info->addr);
    dpu_hal_layer_set_enable(DRV_DPU_LAYER_ID1, TD_TRUE);
    dpu_hal_layer_set_reg_up(DRV_DPU_LAYER_ID1);

    dpu_hal_disp_set_start_pos(DPU_CLOCK_START_POS);
    dpu_hal_disp_set_timing(&timing);
    dpu_disp_set_chnl_reg_up();
    dpu_hal_disp_set_frame_ready();
    uapi_lcd_exit_ulps_internal();

    return EXT_SUCCESS;
}