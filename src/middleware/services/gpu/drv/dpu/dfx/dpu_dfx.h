/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: dpu_interface
 */

#ifndef DRV_GRAPHICS_DFX_DPU_DFX_H
#define DRV_GRAPHICS_DFX_DPU_DFX_H

#include "gpu_dfx.h"
#include "drv_dpu_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define DRV_ERR_DPU_BASE ((td_s32)(((0x80UL + 0x20UL) << 24) | (100 << 16) | (4 << 13) | 1))

enum {
    DRV_ERR_DPU_LAYER_NOT_CREATED = DRV_ERR_DPU_BASE, // 0xA0648001
    DRV_ERR_DPU_NULL_POINTER,
    DRV_ERR_DPU_LAYER_NOT_SUPPORT,
    DRV_ERR_DPU_OSAL_FAILED,
    DRV_ERR_DPU_OPERATION_NOT_SUPPORT,
    DRV_ERR_DPU_MALLOC_FAILED,
    DRV_ERR_DPU_MEMCPY_FALIED,
    DRV_ERR_DPU_CHECK_PARAM_FAILED,
    DRV_ERR_DPU_WAIT_TIMEOUT,
};

enum {
    DPU_DFX_MASK_NONE             = (0 << 0),
    DPU_DFX_MASK_PRINT_FPS        = (1 << 0),
    DPU_DFX_MASK_PRINT_PARAMS     = (1 << 1),
    DPU_DFX_MASK_PRINT_ALL        = (DPU_DFX_MASK_PRINT_FPS | DPU_DFX_MASK_PRINT_PARAMS),
};

typedef struct {
    td_u32 phy_addr;
    td_u32 width;
    td_u32 height;
    td_u32 stride;
    drv_gfx_fmt color_fmt;
    drv_gfx_compress_mode compress_mode;
    ext_rect disp_rect;
    ext_rect crop_rect;
} dpu_dfx_layer_info;

typedef struct {
    td_bool save_en;
    td_bool save_all;
    td_bool is_skip_dpu_refresh;
    td_u32  print_mask;
    td_u32  save_layer;
    td_u32  save_cnt;

    td_float wait_frm_done;
    td_float refresh_intf_cost;

    td_u32 suspend_cnt;
    td_u32 resume_cnt;

    td_u32 lowband_cnt;
    td_u32 bus_err_cnt;

    td_u32 dcmp_cfg_err_cnt;
    td_u32 dcmp_bs_err_cnt;

    td_u32 refresh_cnt;
    td_u32 frm_done_cnt;
    td_u32 te_signal_cnt;

    td_u32 refresh_total;
    td_u32 frm_done_total;
    td_u32 timeout_total;
    td_u32 wbc_done_total;
    td_u32 frm_end_total;
    td_u32 te_signal_total;

    td_u32 draw_fps; /* refresh fps */
    td_u32 flip_fps; /* display fps */
    td_u32 te_fps;   /* te signal fps */

    td_float te_cost_us;
    td_u32 last_te_time_us;

    td_u32 one_sec_ago; /* for fps calculate */

    td_u32 last_fresh_time;
    td_u32 last_isr_done_time;
    dpu_dfx_layer_info layer_info[DRV_DPU_MAX_LAYER_ID + 1];
    td_u32 hact;
    td_u32 vact;
    td_u32 hfb;
    td_u32 vfb;
} dpu_dfx_info;

td_void dpu_dfx_print_info(void);
dpu_dfx_info *dpu_dfx_get_info(td_void);
td_bool dpu_dfx_check_mask(td_u32 mask);
td_void dfx_dpu_save_fb_to_file(drv_dpu_layer_id layer_id, drv_dpu_surface *surface);
td_s32 dpu_dfx_lowband_print(void);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
