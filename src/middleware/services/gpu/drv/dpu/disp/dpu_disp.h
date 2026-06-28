/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: dpu disp header
 */

#ifndef DRV_GRAPHIC_DPU_DISP_DPU_DISP_H
#define DRV_GRAPHIC_DPU_DISP_DPU_DISP_H

#include "drv_dpu_type.h"
#include "soc_osal.h"
#include "dpu_hal.h"
#include "gpu_crg_intf.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    td_bool mir_en;
    td_bool compress_output_mode;
    td_bool color_type;
    td_bool color_sat;
} drv_dpu_output_change;

typedef struct {
    drv_dpu_layer_id layer_id;
    td_bool is_create;
    td_u32 screen_width;
    td_u32 screen_height;
    ext_rect disp_rect;
} dpu_disp_layer_info;

typedef struct {
    td_bool is_open;
    td_bool region_change;
    td_bool set_csc_en;
    td_u32 open_cnt;
    td_u32 screen_width;
    td_u32 screen_height;
    ext_rect disp_region;
    dpu_disp_timing timing;
    osal_spinlock disp_lock;
    drv_dpu_output output_info;
    drv_dpu_output_change output_info_change;
    dpu_disp_layer_info disp_layer_info[DRV_DPU_MAX_LAYER_ID + 1];
    dpu_clk_scene_info scene_info;
    td_u32 work_clk;
    td_u32 clk_gpu;
} dpu_disp_info;

td_s32 dpu_disp_init(td_void);
td_void dpu_disp_deinit(td_void);

td_void dpu_disp_open(td_void);
td_void dpu_disp_close(td_void);

td_void dpu_disp_chl_cfg(td_void);
td_void disp_set_chnl_enable(td_bool enable);

td_void dpu_disp_wait_te_signal(td_void);
td_s32 dpu_disp_wait_frame_done(td_void);
td_u32 dpu_disp_get_srceen_width(td_void);
td_u32 dpu_disp_get_srceen_height(td_void);
td_u32 dpu_disp_get_la(td_void);
td_bool dpu_disp_is_la_valid(td_void);
td_void disp_set_cbm_default_priority(td_void);
td_s32 disp_get_panel_timing(const ext_rect *update_rect, dpu_disp_timing *drv_timing, td_bool cmp_output_mode);
td_s32 dpu_disp_update_dhd_region(ext_rect *gp_region);
td_u32 dpu_disp_get_layer_zorder(td_void);
td_void dpu_disp_set_cbm_priority(drv_dpu_layer_id layer_id, drv_dpu_zorder_mode zorder);
td_void dpu_disp_set_timing_and_screen_region(td_bool is_rotate);
td_bool dpu_disp_update_clk(td_bool is_rotate);
td_void dpu_disp_set_chnl_reg_up(td_void);

dpu_disp_info *dpu_disp_get_info(td_void);
td_s32 dpu_disp_set_output_info(const drv_dpu_output *output_info);
td_void dpu_disp_output_info_config(td_void);
td_void disp_set_uf_offline_en(td_void);
td_void dpu_disp_save_timing_to_dfx(td_void);

#ifdef SUPPORT_GPU_QSPI
td_void dpu_disp_qspi_config(ext_rect *gp_region, drv_gfx_fmt color_fmt);
#endif

#ifdef GPU_SUPPORT_VIDEO_MODE
td_void dpu_disp_wait_vthd_percent_signal(td_void);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
