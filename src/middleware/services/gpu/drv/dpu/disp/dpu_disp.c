/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: dpu disp process
 */
#include "platform_core.h"
#include "dpu_dfx.h"
#include "dpu_isr.h"
#include "lcd_bus.h"
#ifdef SUPPORT_GPU_QSPI
#include "qspi_display.h"
#endif
#include "dpu_disp.h"

#define DPU_VTHD_PERCENT_10  10
#define DPU_VTHD_PERCENT_90  90
#define DPU_VTHD_PERCENT_100 100
#define QSPI_BURST_TYPE      0
#define DEFAULT_BURST_TYPE   1
#define DPU_VTHD_NO_ACTION_ROW   16

#define DPU_IS_FLOAT_EQUAL(x, y) (fabsf((x) - (y)) <= 0.000001)

STATIC dpu_disp_info g_dpu_disp_info = { 0 };
STATIC td_u32 g_pre_lowband_cnt = 0;
STATIC td_u32 g_lowband_print_cnt = 0;
PLT_SEC_ITCM_T dpu_disp_info *dpu_disp_get_info(td_void)
{
    return &g_dpu_disp_info;
}

PLT_SEC_ITCM_T td_u32 dpu_disp_get_srceen_width(td_void)
{
    return g_dpu_disp_info.screen_width;
}

PLT_SEC_ITCM_T td_u32 dpu_disp_get_srceen_height(td_void)
{
    return g_dpu_disp_info.screen_height;
}

td_u32 dpu_disp_get_la(td_void)
{
    return g_dpu_disp_info.output_info.la;
}

td_bool dpu_disp_is_la_valid(td_void)
{
    return g_dpu_disp_info.output_info.is_la_valid;
}

PLT_SEC_ITCM_T INLINE td_s32 disp_get_panel_timing(const ext_rect *update_rect, dpu_disp_timing *drv_timing,
    td_bool cmp_output_mode)
{
    td_s32 ret;
    lcd_bus_api *bus_api = TD_NULL;
    bus_panel_info_t panel_timing = { 0 };

    bus_api = lcd_get_bus_api();
    if ((bus_api == TD_NULL) || (bus_api->bus_get_panel_timing == TD_NULL)) {
        gpu_err("bus_api/bus_get_panel_timing is NULL!");
        return EXT_FAILURE;
    }

    ret = bus_api->bus_get_panel_timing(update_rect, cmp_output_mode, &panel_timing);
    if (ret != EXT_SUCCESS) {
        gpu_err("uapi_panel_get_timing failed, ret = %#x!", ret);
        return ret;
    }

    drv_timing->is_cmd_mode       = panel_timing.is_cmd_mode;
    drv_timing->refresh_rate      = panel_timing.refresh_rate;

    drv_timing->vfb  = panel_timing.sync_info.vfp_lines;
    drv_timing->vbb  = panel_timing.sync_info.vbp_lines + panel_timing.sync_info.vsa_lines;
    drv_timing->vact = panel_timing.sync_info.vact_lines;

    drv_timing->hfb  = panel_timing.sync_info.hfp_pixels;
    drv_timing->hbb  = panel_timing.sync_info.hbp_pixels + panel_timing.sync_info.hsa_pixels;
    drv_timing->hact = panel_timing.sync_info.hact_pixels;

    drv_timing->vpw  = panel_timing.sync_info.vsa_lines;
    drv_timing->hpw  = panel_timing.sync_info.hsa_pixels;

    drv_timing->iop = 1;

    drv_timing->pixel_clk = panel_timing.pixel_clk;

    if (uapi_lcd_get_refresh_flag() == TD_TRUE) {
        gpu_crg_set_dpu_normal_pixel_clk(panel_timing.pixel_clk);
    }

    return EXT_SUCCESS;
}

PLT_SEC_ITCM_T STATIC td_void disp_set_interrupt_thd(dpu_disp_timing *timing)
{
    td_u32 vtotal = timing->vact + timing->vbb + timing->vfb;
    td_u32 vthd_percent = vtotal - DPU_VTHD_NO_ACTION_ROW;
    td_u32 vthd_percent_10 = vtotal * DPU_VTHD_PERCENT_10 / DPU_VTHD_PERCENT_100;

    dpu_hal_disp_set_int_thd(DPU_DISP_INT_TYPE_SETTING, vthd_percent_10);
    dpu_hal_disp_set_int_thd(DPU_DISP_INT_TYPE_FINISH, vthd_percent);
}

PLT_SEC_ITCM_T STATIC INLINE td_s32 disp_config_timing(td_void)
{
    td_s32 ret;

    ret = disp_get_panel_timing(TD_NULL, &g_dpu_disp_info.timing, TD_FALSE);
    if (ret != EXT_SUCCESS) {
        return ret;
    }

    static td_bool is_first = TD_TRUE;
    if (is_first == TD_TRUE) {
        g_dpu_disp_info.disp_region.x = 0;
        g_dpu_disp_info.disp_region.y = 0;
        g_dpu_disp_info.disp_region.width  = g_dpu_disp_info.timing.hact;
        g_dpu_disp_info.disp_region.height = g_dpu_disp_info.timing.vact;
        is_first = TD_FALSE;
    }

    g_dpu_disp_info.screen_width  = g_dpu_disp_info.timing.hact;
    g_dpu_disp_info.screen_height = g_dpu_disp_info.timing.vact;
    dpu_hal_disp_set_start_pos(DPU_CLOCK_START_POS);
    dpu_hal_disp_set_timing(&g_dpu_disp_info.timing);
    disp_set_interrupt_thd(&g_dpu_disp_info.timing);

    return EXT_SUCCESS;
}

PLT_SEC_ITCM_T td_void disp_set_chnl_enable(td_bool enable)
{
    dpu_hal_disp_set_chnl_enable(enable);
}

PLT_SEC_ITCM_T INLINE td_void disp_set_cbm_default_priority(td_void)
{
    dpu_hal_disp_set_cbm_priority(DPU_DISP_CBM_MIX_LAYER_G0, DPU_DISP_CBM_MIX_PRIORITY_1);
    dpu_hal_disp_set_cbm_priority(DPU_DISP_CBM_MIX_LAYER_G1, DPU_DISP_CBM_MIX_PRIORITY_0);
}

PLT_SEC_ITCM_T STATIC INLINE td_void disp_set_isr_enable(td_bool enable)
{
#ifdef GPU_SUPPORT_VIDEO_MODE
    dpu_hal_disp_set_dhd0_te_mode(DPU_DISP_VSYNC_TE_VODEO_MODE);
    dpu_hal_disp_set_int_enable(DPU_DISP_INT_TYPE_SETTING, enable);
    dpu_hal_disp_set_int_enable(DPU_DISP_INT_TYPE_FINISH, enable);
#else
#ifdef MIPI_ULPS_SUPPORT
    dpu_disp_vsync_te_mode vsync_te_mode = DPU_DISP_VSYNC_TE_CMD2_MODE;
#else
    dpu_disp_vsync_te_mode vsync_te_mode = DPU_DISP_VSYNC_TE_CMD0_MODE;
#endif
    if (enable == TD_FALSE) {
        vsync_te_mode = DPU_DISP_VSYNC_TE_VODEO_MODE;
    }
    dpu_hal_disp_set_dhd0_te_mode(vsync_te_mode);
#endif

#ifndef GPU_1A21
    dpu_hal_disp_set_int_enable(DPU_DISP_INT_TYPE_TE_INT, enable);
#endif
    dpu_hal_disp_set_int_enable(DPU_DISP_INT_TYPE_LOWBAND, enable);
    dpu_hal_disp_set_int_enable(DPU_DISP_INT_TYPE_BUS_ERR, enable);
    dpu_hal_disp_set_int_enable(DPU_DISP_INT_TYPE_FRM_DONE, enable);
#ifdef GPU_1A22
    dpu_hal_disp_set_int_enable(DPU_DISP_INT_TYPE_DCMP_CFG_ERR, enable);
    dpu_hal_disp_set_int_enable(DPU_DISP_INT_TYPE_DCMP_BS_ERR, enable);
#endif
#ifdef SUPPORT_GPU_QSPI
    dpu_hal_disp_set_int_enable(DPU_DISP_INT_TYPE_WBC_DONE, enable);
#endif
}

td_s32 dpu_disp_init(td_void)
{
    td_s32 ret;

    (td_void)memset_s(&g_dpu_disp_info, sizeof(g_dpu_disp_info), 0, sizeof(dpu_disp_info));

    ret = osal_spin_lock_init(&g_dpu_disp_info.disp_lock);
    if (ret != EXT_SUCCESS) {
        gpu_err("osal_spin_lock_init failed");
        return DRV_ERR_DPU_OSAL_FAILED;
    }

    disp_set_cbm_default_priority();

    return EXT_SUCCESS;
}

td_void dpu_disp_deinit(td_void)
{
    dpu_disp_close();

    if (g_dpu_disp_info.disp_lock.lock != TD_NULL) {
        osal_spin_lock_destroy(&g_dpu_disp_info.disp_lock);
    }

    return;
}

PLT_SEC_ITCM_T td_void dpu_disp_chl_cfg(td_void)
{
    if (g_dpu_disp_info.is_open == TD_TRUE) {
        return;
    }
    disp_config_timing();
    disp_set_isr_enable(TD_TRUE);
    disp_set_cbm_default_priority();
#ifdef SUPPORT_GPU_QSPI
    dpu_hal_set_dhd0_ctrl_uf_offline_en();
#endif
#ifndef GPU_SUPPORT_VIDEO_MODE
    disp_set_chnl_enable(TD_TRUE);
#endif
    dpu_disp_set_chnl_reg_up();
    g_dpu_disp_info.region_change = TD_TRUE;
}

PLT_SEC_ITCM_T static td_void dpu_disp_init_output_info(td_void)
{
    g_dpu_disp_info.output_info.work_scene = DRV_DPU_WORK_SCENE_NORMAL;
    g_dpu_disp_info.output_info.is_la_valid = TD_FALSE;
    g_dpu_disp_info.output_info.mir_en = TD_FALSE;
    g_dpu_disp_info.output_info.compress_output_mode = TD_FALSE;
    g_dpu_disp_info.output_info.wbc_info = NULL;
    g_dpu_disp_info.output_info.color_type = DRV_DPU_COLOR_NONE;
    g_dpu_disp_info.output_info.color_sat = DRV_DPU_SAT_MEDIUM;
}

PLT_SEC_ITCM_T static td_void dpu_disp_init_output_info_change(td_void)
{
    g_dpu_disp_info.output_info_change.mir_en = TD_TRUE;
    g_dpu_disp_info.output_info_change.compress_output_mode = TD_TRUE;
    g_dpu_disp_info.output_info_change.color_type = TD_FALSE;
    g_dpu_disp_info.output_info_change.color_sat = TD_FALSE;
}

PLT_SEC_ITCM_T static td_void dpu_disp_output_info_init(td_void)
{
    dpu_disp_init_output_info();
    dpu_disp_init_output_info_change();
}

PLT_SEC_ITCM_T td_void dpu_disp_open(td_void)
{
    unsigned long flag = 0;

    osal_spin_lock_irqsave(&g_dpu_disp_info.disp_lock, &flag);

    dpu_disp_chl_cfg();
    dpu_disp_output_info_init();

    g_dpu_disp_info.open_cnt++;
    g_dpu_disp_info.is_open = TD_TRUE;

    osal_spin_unlock_irqrestore(&g_dpu_disp_info.disp_lock, &flag);

    return;
}

td_void dpu_disp_close(td_void)
{
    unsigned long flag = 0;

    osal_spin_lock_irqsave(&g_dpu_disp_info.disp_lock, &flag);

    if (g_dpu_disp_info.is_open != TD_TRUE) {
        osal_spin_unlock_irqrestore(&g_dpu_disp_info.disp_lock, &flag);
        return;
    }

    g_dpu_disp_info.open_cnt--;
    g_dpu_disp_info.is_open = TD_FALSE;
    osal_spin_unlock_irqrestore(&g_dpu_disp_info.disp_lock, &flag);

    return;
}

td_void dpu_disp_set_cbm_priority(drv_dpu_layer_id layer_id, drv_dpu_zorder_mode zorder)
{
    td_bool is_g0_up = TD_TRUE;

    if (layer_id == DRV_DPU_LAYER_ID1) {
        if ((zorder == DRV_DPU_ZORDER_MODE_MOVETOP) ||
            (zorder == DRV_DPU_ZORDER_MODE_MOVEUP)) {
            is_g0_up = TD_TRUE;
        } else {
            is_g0_up = TD_FALSE;
        }
    } else {
        if ((zorder == DRV_DPU_ZORDER_MODE_MOVETOP) ||
            (zorder == DRV_DPU_ZORDER_MODE_MOVEUP)) {
            is_g0_up = TD_FALSE;
        } else {
            is_g0_up = TD_TRUE;
        }
    }

    if (is_g0_up) {
        dpu_hal_disp_set_cbm_priority(DPU_DISP_CBM_MIX_LAYER_G0, DPU_DISP_CBM_MIX_PRIORITY_1);
        dpu_hal_disp_set_cbm_priority(DPU_DISP_CBM_MIX_LAYER_G1, DPU_DISP_CBM_MIX_PRIORITY_0);
    } else {
        dpu_hal_disp_set_cbm_priority(DPU_DISP_CBM_MIX_LAYER_G0, DPU_DISP_CBM_MIX_PRIORITY_0);
        dpu_hal_disp_set_cbm_priority(DPU_DISP_CBM_MIX_LAYER_G1, DPU_DISP_CBM_MIX_PRIORITY_1);
    }

    return;
}

td_u32 dpu_disp_get_layer_zorder(td_void)
{
    return dpu_hal_disp_get_cbm_priority();
}

td_s32 dpu_disp_wait_frame_done(td_void)
{
    td_u32 time_start = 0;
    td_u32 time_end   = 0;
    dpu_dfx_info *dfx_info = dpu_dfx_get_info();

    gpu_dfx_get_time_us(&time_start);
    td_s32 ret = dpu_isr_wait_finish();
    gpu_dfx_get_time_us(&time_end);

    dfx_info->wait_frm_done = (td_float)(time_end - time_start);

    td_u32 cnt = dfx_info->lowband_cnt;
    if (cnt > g_pre_lowband_cnt && g_lowband_print_cnt < 10) { // 10 is max times
        dpu_dfx_lowband_print();
        g_lowband_print_cnt++;
    }
    g_pre_lowband_cnt = cnt;
    return ret;
}

td_void dpu_disp_wait_te_signal(td_void)
{
    dpu_isr_wait_te_signal();
}

#ifdef GPU_SUPPORT_VIDEO_MODE
td_void dpu_disp_wait_vthd_percent_signal(td_void)
{
    dpu_isr_wait_vthd_percent_signal();
}
#endif

td_s32 dpu_disp_update_dhd_region(ext_rect *gp_region)
{
    /* 1.check whether gp_region and disp_region is equal */
    if ((memcmp(&g_dpu_disp_info.disp_region, gp_region, sizeof(ext_rect)) == 0) &&
        (g_dpu_disp_info.output_info_change.compress_output_mode == TD_FALSE) &&
        uapi_lcd_get_refresh_flag() != TD_TRUE) {
        return EXT_SUCCESS;
    }

    td_bool cmp_output_mode = g_dpu_disp_info.output_info.compress_output_mode;
    /* 2.if disp_region need update, first update mipi timing */
    if (disp_get_panel_timing(gp_region, &g_dpu_disp_info.timing, cmp_output_mode) != EXT_SUCCESS) {
        gpu_err("disp_get_panel_timing failed, gp_region[%d %d %u %u]",
                gp_region->x, gp_region->y, gp_region->width, gp_region->height);
        return EXT_FAILURE;
    }
    /* 3.check panel timing whether match gp_region */
    if (((gp_region->width != g_dpu_disp_info.timing.hact) || (gp_region->height != g_dpu_disp_info.timing.vact)) &&
          cmp_output_mode == TD_FALSE) {
        gpu_err("panel timing not match gp_region, gp_wh[%u,%u], timing_hact/vact[%u,%u]!",
                gp_region->width, gp_region->height, g_dpu_disp_info.timing.hact, g_dpu_disp_info.timing.vact);
        return EXT_FAILURE;
    }

    /* 4.update disp_region by gp_region */
    g_dpu_disp_info.disp_region = *gp_region;
    g_dpu_disp_info.region_change = TD_TRUE;

    return EXT_SUCCESS;
}

#if !defined(SUPPORT_SOFT_SEND_FRM) && !defined(SUPPORT_GPU_QSPI)
STATIC td_void disp_set_partial_cmd_info(td_void)
{
    td_s32 ret;
    partial_display_cmd_t lcd_cmd_info = {0};
    dpu_hal_partial_cmd_info dpu_cmd_info = {0};

    lcd_bus_api *bus_api = lcd_get_bus_api();
    if ((bus_api == TD_NULL) || (bus_api->bus_get_partial_display_cmd == TD_NULL)) {
        gpu_err("bus_api/bus_get_partial_display_cmd is NULL!");
        return;
    }

    ret = bus_api->bus_get_partial_display_cmd(&g_dpu_disp_info.disp_region, &lcd_cmd_info);
    if (ret != EXT_SUCCESS) {
        gpu_err("uapi_panel_get_cmd_info failed !");
        return;
    }

    dpu_cmd_info.hdr0       = lcd_cmd_info.cmd_set_col_h;
    dpu_cmd_info.pld_data00 = lcd_cmd_info.set_col_p0;
    dpu_cmd_info.pld_data01 = lcd_cmd_info.set_col_p1;

    dpu_cmd_info.hdr1       = lcd_cmd_info.cmd_set_raw_h;
    dpu_cmd_info.pld_data10 = lcd_cmd_info.set_raw_p0;
    dpu_cmd_info.pld_data11 = lcd_cmd_info.set_raw_p1;

    dpu_hal_disp_set_partial_cmd_info(&dpu_cmd_info);

    return;
}
#endif

STATIC td_bool dpu_disp_is_clk_scene_change(const dpu_clk_scene_info *old_scene_info,
    const dpu_clk_scene_info *new_scene_info)
{
    if (old_scene_info->work_scene == new_scene_info->work_scene &&
        old_scene_info->is_cmp_out == new_scene_info->is_cmp_out &&
        old_scene_info->is_rotate == new_scene_info->is_rotate) {
        return TD_FALSE;
    }
    return TD_TRUE;
}

td_bool dpu_disp_update_clk(td_bool is_rotate)
{
    td_bool clk_change = TD_FALSE;
    static dpu_clk_scene_info old_scene_info = {.work_scene = DRV_DPU_WORK_SCENE_NORMAL,
        .is_cmp_out = TD_FALSE, .is_rotate = TD_FALSE};
    dpu_clk_scene_info new_scene_info = {.work_scene = g_dpu_disp_info.output_info.work_scene,
        .is_cmp_out = g_dpu_disp_info.output_info.compress_output_mode, .is_rotate = is_rotate};

    if (dpu_disp_is_clk_scene_change(&old_scene_info, &new_scene_info) || uapi_lcd_get_refresh_flag() == TD_TRUE) {
        new_scene_info.cfg_scene = GPU_CLK_CFG_SWITCH;
        gpu_crg_set_clk(GPU_ID_DPU, &new_scene_info);
        old_scene_info = new_scene_info;
        clk_change = TD_TRUE;
        uapi_lcd_set_refresh_flag(TD_FALSE);
    }

    g_dpu_disp_info.scene_info = new_scene_info;
    dpu_clk_info clk_info = {};
    gpu_crg_get_clk(GPU_ID_DPU, &g_dpu_disp_info.scene_info.work_scene, &clk_info);
    g_dpu_disp_info.timing.pixel_clk = clk_info.pixel_clk;
    g_dpu_disp_info.work_clk = clk_info.work_clk;
    g_dpu_disp_info.clk_gpu = clk_info.clk_gpu;
    return clk_change;
}

td_void dpu_disp_set_timing_and_screen_region(td_bool is_rotate)
{
    lcd_bus_api *bus_api = lcd_get_bus_api();
    if ((bus_api == TD_NULL) || (bus_api->bus_update_panel == TD_NULL)) {
        gpu_err("bus_api/bus_update_panel is NULL!");
        return;
    }
    td_bool clk_change = dpu_disp_update_clk(is_rotate);
#if !defined(SUPPORT_SOFT_SEND_FRM) && !defined(SUPPORT_GPU_QSPI)
    dpu_hal_disp_set_partial_cfg_vld(g_dpu_disp_info.region_change);

    if ((g_dpu_disp_info.region_change == TD_TRUE) || (clk_change == TD_TRUE)) {
        dpu_hal_disp_set_timing(&g_dpu_disp_info.timing);  /* update dpu timing */
        bus_api->bus_update_panel(PANEL_UPDATE_TIMING, g_dpu_disp_info.timing.pixel_clk); /* update mipi timing */
        disp_set_partial_cmd_info();                    /* update screen region by dpu logic */
        g_dpu_disp_info.region_change = TD_FALSE;
    }
#else
    if ((g_dpu_disp_info.region_change == TD_TRUE) || (clk_change == TD_TRUE)) {
        dpu_hal_disp_set_timing(&g_dpu_disp_info.timing);  /* update dpu timing */
        bus_api->bus_update_panel(PANEL_UPDATE_TIMING, g_dpu_disp_info.timing.pixel_clk); /* update mipi timing */
        bus_api->bus_update_panel(PANEL_UPDATE_SCREEN, g_dpu_disp_info.timing.pixel_clk); /* update screen region by mipi software */
        g_dpu_disp_info.region_change = TD_FALSE;
    }
#endif

    return;
}

td_void dpu_disp_set_chnl_reg_up(td_void)
{
    dpu_hal_disp_set_chnl_regup();
}

static td_void dpu_disp_wbc_config(td_void)
{
    drv_dpu_output *output_info = &g_dpu_disp_info.output_info;
    if (output_info->wbc_info == NULL) {
        return;
    }
    dpu_hal_wbc_info hal_wbc = {0};

    hal_wbc.iop = 1;
    hal_wbc.d_wbc_dhd_ctrl.des_en = output_info->wbc_info->wbc_bind_mode == 0 ? 0 : 1;
    hal_wbc.des_addr = output_info->wbc_info->wbc_addr;
    hal_wbc.des_width = output_info->wbc_info->wbc_width;
    hal_wbc.des_height = output_info->wbc_info->wbc_height;
    hal_wbc.des_stride = output_info->wbc_info->wbc_stride;
    hal_wbc.d_wbc_dhd_ctrl.des_fmt = output_info->wbc_info->color_fmt;
    hal_wbc.d_wbc_dhd_ctrl.des_argb_order = 0;
    hal_wbc.wbc_bind_mode = output_info->wbc_info->wbc_bind_mode == 0 ? 0 :
                            output_info->wbc_info->wbc_bind_mode - 1;
    hal_wbc.burst_type = DEFAULT_BURST_TYPE;
    dpu_hal_disp_set_wbc_info(&hal_wbc);
}

#ifdef SUPPORT_GPU_QSPI
td_void dpu_disp_qspi_config(ext_rect *gp_region, drv_gfx_fmt color_fmt)
{
    drv_dpu_output *output_info = &g_dpu_disp_info.output_info;
    if (output_info->wbc_info != NULL) {
        return;
    }
    dpu_hal_wbc_info hal_wbc = {0};
    hal_wbc.des_width = gp_region->width;
    hal_wbc.des_height = gp_region->height;
    hal_wbc.wbc_bind_mode = 2; // 2 is cbm
    hal_wbc.iop = 1;
    hal_wbc.d_wbc_dhd_ctrl.des_en = 1;

    hal_wbc.des_addr = DPU_QSPI_DR_ADDR;
    hal_wbc.d_wbc_dhd_ctrl.des_hold_mode = 1;
    hal_wbc.d_wbc_dhd_ctrl.des_fast_req = 1;
    hal_wbc.burst_type = QSPI_BURST_TYPE;
    hal_wbc.d_wbc_dhd_ctrl.des_fmt = color_fmt;
    if (color_fmt == DRV_GFX_FMT_RGB888 || color_fmt == DRV_GFX_FMT_RGB444) {
        hal_wbc.d_wbc_dhd_ctrl.des_argb_order = 0x5;
    }
    dpu_hal_disp_set_wbc_info(&hal_wbc);
}
#endif

static void dpu_disp_cmp_output_disp_rect_set(dpu_disp_layer_info *disp_layer_info)
{
    ext_rect *disp_rect = &disp_layer_info->disp_rect;
    td_s32 x_end, y_end;

    x_end = disp_rect->x + (td_s32)disp_rect->width;
    y_end = disp_rect->y + (td_s32)disp_rect->height;

    disp_rect->x = disp_rect->x / LCD_CMP_OUT_WIDTH_ALIGN * LCD_CMP_OUT_WIDTH_ALIGN;
    disp_rect->y = disp_rect->y / LCD_CMP_OUT_HEIGHT_ALIGN * LCD_CMP_OUT_HEIGHT_ALIGN;

    x_end = uapi_byte_align(x_end, LCD_CMP_OUT_WIDTH_ALIGN);
    if (x_end > (td_s32)disp_layer_info->screen_width) {
        x_end = (td_s32)disp_layer_info->screen_width;
    }
    disp_rect->width = (td_u32)(x_end - disp_rect->x);

    y_end = uapi_byte_align(y_end, LCD_CMP_OUT_HEIGHT_ALIGN);
    if (y_end > (td_s32)disp_layer_info->screen_height) {
        y_end = (td_s32)disp_layer_info->screen_height;
    }
    disp_rect->height = (td_u32)(y_end - disp_rect->y);

    dpu_hal_layer_set_disp_rect(disp_layer_info->layer_id, disp_rect);
}

static td_void dpu_disp_cmp_out_config(const td_bool enable)
{
    disp_cmp_ctrl cmp_ctrl = {0};
    td_bool ice_en;
    dpu_disp_layer_info *disp_layer_info = NULL;
    ice_en = (enable == TD_TRUE) ? 1 : 0;

    // logic restriction: The compressed display supports only the 3x compressed display in the RGB888 color format.
    if (ice_en != 0) {
        // before compression: block_size_before = 4 * 4 * 3;
        cmp_ctrl.des_block_size = 16;  // block_size:16  after compression: block_size_after = block_size_before / 3;
        cmp_ctrl.pix_format = 0;

        cmp_ctrl.iter_protect_csc_en    = 1;
        cmp_ctrl.csc_metria_en          = 255; // csc metria of 3x cmp rate is 255; 2x is 1
        cmp_ctrl.htc_cmp_ck_gt_en       = 1;

        for (td_u32 layer_id = (td_u32)DRV_DPU_LAYER_ID1; layer_id <= (td_u32)DRV_DPU_MAX_LAYER_ID; layer_id++) {
            disp_layer_info = &(g_dpu_disp_info.disp_layer_info[layer_id]);
            if (disp_layer_info->is_create == TD_TRUE) {
                dpu_disp_cmp_output_disp_rect_set(disp_layer_info); // compressed output: 12 x 4 alignment
            }
        }
    }

    dpu_hal_layer_set_cmp(0, ice_en, &cmp_ctrl);
}

td_void dpu_disp_output_info_config(td_void)
{
    if (g_dpu_disp_info.output_info_change.compress_output_mode) {
        dpu_disp_cmp_out_config(g_dpu_disp_info.output_info.compress_output_mode);
        g_dpu_disp_info.output_info_change.compress_output_mode = TD_FALSE;
    }

    if (g_dpu_disp_info.output_info_change.mir_en) {
        dpu_hal_disp_set_mir_en(g_dpu_disp_info.output_info.mir_en);
        g_dpu_disp_info.output_info_change.mir_en = TD_FALSE;
    }

    dpu_disp_wbc_config();

    if (g_dpu_disp_info.output_info_change.color_type) {
        dpu_hal_disp_set_csc_bind(g_dpu_disp_info.output_info.color_type);
        g_dpu_disp_info.output_info_change.color_type = TD_FALSE;
    }

    if (g_dpu_disp_info.output_info_change.color_sat) {
        dpu_hal_disp_set_csc_sat(g_dpu_disp_info.output_info.color_sat);
        g_dpu_disp_info.output_info_change.color_sat = TD_FALSE;
    }

    static td_bool old_csc_en = TD_FALSE;
    g_dpu_disp_info.set_csc_en = TD_FALSE;
    if ((g_dpu_disp_info.output_info.color_type > DRV_DPU_COLOR_NONE) ||
        (DPU_IS_FLOAT_EQUAL(g_dpu_disp_info.output_info.color_sat, DRV_DPU_SAT_MEDIUM) == TD_FALSE) ||
        (g_dpu_disp_info.output_info.is_la_valid == TD_TRUE)) {
        g_dpu_disp_info.set_csc_en = TD_TRUE;
    }

    if (g_dpu_disp_info.set_csc_en != old_csc_en) {
        dpu_hal_set_dhd_csc_en(g_dpu_disp_info.set_csc_en);
        old_csc_en = g_dpu_disp_info.set_csc_en;
    }
}

STATIC td_s32 dpu_check_output_info(const drv_dpu_output *output_info)
{
    if (output_info == TD_NULL) {
        gpu_err("dpu_check_output_info input null\n");
        return DRV_ERR_DPU_NULL_POINTER;
    }

    if (output_info->color_type > DRV_DPU_COLOR_INVERT) {
        gpu_err("invalid color_type %d", output_info->color_type);
        return DRV_ERR_DPU_CHECK_PARAM_FAILED;
    }

    if (output_info->color_sat < 0 || output_info->color_sat > 1) {
        gpu_err("invalid color_sat %f", output_info->color_sat);
        return DRV_ERR_DPU_CHECK_PARAM_FAILED;
    }

    return EXT_SUCCESS;
}

td_s32 dpu_disp_set_output_info(const drv_dpu_output *output_info)
{
    if (dpu_check_output_info(output_info) != EXT_SUCCESS) {
        gpu_err("dpu_check_output_info fail\n");
        return EXT_FAILURE;
    }

    unsigned long flag = 0;
    osal_spin_lock_irqsave(&g_dpu_disp_info.disp_lock, &flag);

    g_dpu_disp_info.output_info.work_scene = output_info->work_scene;

    if (output_info->compress_output_mode != g_dpu_disp_info.output_info.compress_output_mode) {
        g_dpu_disp_info.output_info.compress_output_mode = output_info->compress_output_mode;
        g_dpu_disp_info.output_info_change.compress_output_mode = TD_TRUE;
    }
    if (output_info->mir_en != g_dpu_disp_info.output_info.mir_en) {
        g_dpu_disp_info.output_info.mir_en = output_info->mir_en;
        g_dpu_disp_info.output_info_change.mir_en = TD_TRUE;
    }

    g_dpu_disp_info.output_info.wbc_info = output_info->wbc_info;
    g_dpu_disp_info.output_info.is_la_valid = output_info->is_la_valid;
    g_dpu_disp_info.output_info.la = output_info->la;

    if (output_info->color_type != g_dpu_disp_info.output_info.color_type) {
        g_dpu_disp_info.output_info.color_type = output_info->color_type;
        g_dpu_disp_info.output_info_change.color_type = TD_TRUE;
    }

    if (DPU_IS_FLOAT_EQUAL(output_info->color_sat, g_dpu_disp_info.output_info.color_sat) == TD_FALSE) {
        g_dpu_disp_info.output_info.color_sat = output_info->color_sat;
        g_dpu_disp_info.output_info_change.color_sat = TD_TRUE;
    }

    osal_spin_unlock_irqrestore(&g_dpu_disp_info.disp_lock, &flag);
    return EXT_SUCCESS;
}

td_void dpu_disp_save_timing_to_dfx(td_void)
{
    dpu_dfx_info *dfx_info = dpu_dfx_get_info();
    dfx_info->hact = g_dpu_disp_info.timing.hact;
    dfx_info->vact = g_dpu_disp_info.timing.vact;
    dfx_info->hfb  = g_dpu_disp_info.timing.hfb;
    dfx_info->vfb  = g_dpu_disp_info.timing.vfb;
}
