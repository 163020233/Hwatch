/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: dpu layer
 */

#include "dpu_layer.h"
#include "platform_core.h"
#include "gpu_feature_config.h"
#include "dpu_disp.h"
#include "dpu_isr.h"
#include "dpu_hal.h"
#include "dpu_dfx.h"
#if (GPU_LOW_POWER_SUPPORT == YES)
#if (defined(TARGET_CHIP_BRANDY) && TARGET_CHIP_BRANDY == 1)
#include "pm.h"
#else
#include "pm_veto_porting.h"
#endif
#include "pm_veto.h"
#endif
#include "dpu_affine.h"
#include "soc_lcd_api.h"
#include "dpu_sre.h"
#ifdef SUPPORT_GPU_QSPI
#include "qspi_display.h"
#endif

#ifdef GPU_HIGH_RESOLUTION
#define DRV_DPU_MAX_LAYER_WIDTH  4096
#define DRV_DPU_MAX_LAYER_HEIGHT 4096
#else
#define DRV_DPU_MAX_LAYER_WIDTH  640
#define DRV_DPU_MAX_LAYER_HEIGHT 640
#endif

#define DRV_DPU_MIN_LAYER_WIDTH  32
#define DRV_DPU_MIN_LAYER_HEIGHT 32

typedef struct {
    td_bool is_create[DRV_DPU_MAX_LAYER_ID + 1];
    drv_dpu_layer_info layer_info[DRV_DPU_MAX_LAYER_ID + 1];
    td_bool is_rotate;
} dpu_context;

STATIC dpu_context g_dpu_context = { 0 };

STATIC dpu_layer_dev *g_dpu_layer_dev[DRV_DPU_MAX_LAYER_ID + 1] = { 0 };

#define get_dpu_layer_device(layer_id, layer_dev) do {  \
    layer_dev = g_dpu_layer_dev[layer_id];              \
    if ((layer_dev) == TD_NULL) {                       \
        gpu_err("layer%d is not inited!", layer_id); \
        return DRV_ERR_DPU_NULL_POINTER;                \
    }                                                   \
} while (0)

dpu_layer_dev *dpu_layer_get_dev(drv_dpu_layer_id layer_id)
{
    return g_dpu_layer_dev[layer_id];
}

PLT_SEC_ITCM_T STATIC INLINE td_bool dpu_check_layer_fmt_support(drv_gfx_fmt fmt)
{
    if ((fmt != DRV_GFX_FMT_ARGB8888) &&
        (fmt != DRV_GFX_FMT_ARGB4444) &&
        (fmt != DRV_GFX_FMT_ARGB1555) &&
        (fmt != DRV_GFX_FMT_RGB888) &&
        (fmt != DRV_GFX_FMT_RGB565) &&
        (fmt != DRV_GFX_FMT_YUV420SP) &&
        (fmt != DRV_GFX_FMT_RGB332) &&
        (fmt != DRV_GFX_FMT_R8) &&
        (fmt != DRV_GFX_FMT_G8) &&
        (fmt != DRV_GFX_FMT_B8) &&
        (fmt != DRV_GFX_FMT_ARGB2101010)) {
        gpu_err("layer fmt not support:%u", (td_u32)fmt);
        return TD_FALSE;
    }

    return TD_TRUE;
}

PLT_SEC_ITCM_T STATIC INLINE td_s32 dpu_layer_check_layer_info(const drv_dpu_layer_info *layer_info)
{
    if ((layer_info->width < DRV_DPU_MIN_LAYER_WIDTH) ||
        (layer_info->width > DRV_DPU_MAX_LAYER_WIDTH)) {
        gpu_err("layer width not support:%u", layer_info->width);
        return DRV_ERR_DPU_LAYER_NOT_SUPPORT;
    }
    if ((layer_info->height < DRV_DPU_MIN_LAYER_HEIGHT) ||
        (layer_info->height > DRV_DPU_MAX_LAYER_HEIGHT)) {
        gpu_err("layer height not support:%u", layer_info->height);
        return DRV_ERR_DPU_LAYER_NOT_SUPPORT;
    }
    if (dpu_check_layer_fmt_support(layer_info->layer_fmt) == TD_FALSE) {
        return DRV_ERR_DPU_LAYER_NOT_SUPPORT;
    }
    if ((layer_info->layer_id < DRV_DPU_LAYER_ID1) ||
        (layer_info->layer_id > DRV_DPU_MAX_LAYER_ID)) {
        gpu_err("layer not support:%u", (td_u32)layer_info->layer_id);
        return DRV_ERR_DPU_LAYER_NOT_SUPPORT;
    }
    if (layer_info->buf_mode != DRV_DPU_BUF_MODE_NORMAL) {
        gpu_err("flush mode not support:%u", (td_u32)layer_info->buf_mode);
        return DRV_ERR_DPU_LAYER_NOT_SUPPORT;
    }
    if (layer_info->compress_mode >= DRV_GFX_COMPRESS_MAX) {
        gpu_err("compress mode not support:%u", (td_u32)layer_info->compress_mode);
        return DRV_ERR_DPU_LAYER_NOT_SUPPORT;
    }

    if (((layer_info->layer_fmt == DRV_GFX_FMT_YUV420SP) && (layer_info->layer_id != DRV_DPU_LAYER_ID2)) ||
        ((layer_info->layer_fmt == DRV_GFX_FMT_ARGB2101010) && (layer_info->layer_id != DRV_DPU_LAYER_ID1))) {
        gpu_err("only layer_id[%u] support yuv fmt[%u]!", (td_u32)layer_info->layer_id,
                (td_u32)layer_info->layer_fmt);
        return DRV_ERR_DPU_LAYER_NOT_SUPPORT;
    }

    return EXT_SUCCESS;
}

PLT_SEC_ITCM_T STATIC INLINE td_void dpu_layer_set_run_info(dpu_layer_run_info *run_info,
    const drv_dpu_layer_info *layer_info)
{
    run_info->show          = TD_TRUE;
    run_info->alpha0        = 0x0;
    run_info->alpha1        = 0xFF;
    run_info->global_alpha  = 0xFF;
    run_info->width         = layer_info->width;
    run_info->height        = layer_info->height;
    run_info->fmt           = layer_info->layer_fmt;
    run_info->premulti_en   = 0; /* The output material of the VAU is premultiplication */
    run_info->is_mapping    = 0;
    run_info->flip_en       = 0;
    run_info->compress_mode = layer_info->compress_mode;
    run_info->rotation_mode =  layer_info->rotation_mode;
    run_info->pos_x = 0;
    run_info->pos_y = 0;

    run_info->disp_rect.x = 0;
    run_info->disp_rect.y = 0;
    run_info->disp_rect.width  = layer_info->width;
    run_info->disp_rect.height = layer_info->height;

    run_info->crop_rect.x = 0;
    run_info->crop_rect.y = 0;
    run_info->crop_rect.width  = layer_info->width;  /* crop_w means ireso_w */
    run_info->crop_rect.height = layer_info->height; /* crop_h means ireso_h */

    run_info->update_region.x = 0;
    run_info->update_region.y = 0;
    run_info->update_region.width  = layer_info->width;
    run_info->update_region.height = layer_info->height;
    return;
}

PLT_SEC_ITCM_T STATIC INLINE td_void dpu_layer_init_run_info_change(dpu_layer_run_info_change *run_info_change)
{
    run_info_change->show          = TD_TRUE;
    run_info_change->alpha0_alpha1 = TD_TRUE;
    run_info_change->global_alpha  = TD_TRUE;
    run_info_change->width         = TD_TRUE;
    run_info_change->height        = TD_TRUE;
    run_info_change->stride        = TD_TRUE;
    run_info_change->addr          = TD_TRUE;
    run_info_change->disp_rect     = TD_TRUE;
    run_info_change->crop_rect     = TD_TRUE;
    run_info_change->fmt           = TD_TRUE;
    run_info_change->colorkey      = TD_TRUE;
    run_info_change->compress_mode = TD_TRUE;

    return;
}

dpu_hal_fmt dpu_layer_trans_fmt_drv_to_hal(drv_gfx_fmt fmt)
{
    dpu_hal_fmt hal_fmt;

    switch (fmt) {
        case DRV_GFX_FMT_ARGB8888:
            hal_fmt = DPU_HAL_FMT_ARGB8888;
            break;
        case DRV_GFX_FMT_ARGB1555:
            hal_fmt = DPU_HAL_FMT_ARGB1555;
            break;
        case DRV_GFX_FMT_ARGB4444:
            hal_fmt = DPU_HAL_FMT_ARGB4444;
            break;
        case DRV_GFX_FMT_RGB888:
            hal_fmt = DPU_HAL_FMT_RGB888;
            break;
        case DRV_GFX_FMT_RGB565:
            hal_fmt = DPU_HAL_FMT_RGB565;
            break;
        case DRV_GFX_FMT_YUV420SP:
            hal_fmt = DPU_HAL_FMT_YUV420SP;
            break;
        case DRV_GFX_FMT_RGB332:
            hal_fmt = DPU_HAL_FMT_RGB332;
            break;
        case DRV_GFX_FMT_R8:
            hal_fmt = DPU_HAL_FMT_R8;
            break;
        case DRV_GFX_FMT_G8:
            hal_fmt = DPU_HAL_FMT_G8;
            break;
        case DRV_GFX_FMT_B8:
            hal_fmt = DPU_HAL_FMT_B8;
            break;
        case DRV_GFX_FMT_ARGB2101010:
            hal_fmt = DPU_HAL_FMT_ARGB2101010;
            break;
        default:
            hal_fmt = DPU_HAL_FMT_RGB888;
            break;
    }

    return hal_fmt;
}

STATIC td_void dpu_layer_yuv_base_config(dpu_layer_dev *layer_dev)
{
    dpu_hal_yuv_info yuv_info = {0};

    if (layer_dev->run_info.fmt != DRV_GFX_FMT_YUV420SP) {
        return;
    }

    yuv_info.y_addr     = layer_dev->run_info.current_addr;
    yuv_info.out_height = layer_dev->run_info.height;
    yuv_info.uv_stride  = layer_dev->canvas_surface.uv_stride;
    yuv_info.uv_offset  = layer_dev->canvas_surface.uv_offset;

#if (defined(TARGET_CHIP_BRANDY) && TARGET_CHIP_BRANDY == 1)
    /* soft support yuv crop */
    if (layer_dev->run_info.crop_rect.y != 0) {
        yuv_info.y_addr    += (td_u32)layer_dev->run_info.crop_rect.y * yuv_info.uv_stride;
        yuv_info.uv_offset -= (td_u32)layer_dev->run_info.crop_rect.y * yuv_info.uv_stride / 2; /* 2:index */

        layer_dev->run_info.crop_rect.y = 0;
        dpu_hal_layer_set_crop_rect(layer_dev->layer_id, &layer_dev->run_info.crop_rect);
    }

    dpu_hal_layer_set_addr(layer_dev->layer_id, yuv_info.y_addr);
#endif

    dpu_hal_layer_set_yuv_base_info(layer_dev->layer_id, &yuv_info);

    return;
}

STATIC td_void dpu_layer_dcmp_config(const dpu_layer_dev *layer_dev)
{
    layer_gfx_dcmp_ctrl dcmp_ctrl = {0};
    layer_gfx_htc_online_dcmp0 dcmp0 = {0};
    layer_gfx_htc_online_dcmp1 dcmp1 = {0};

    dcmp_ctrl.dcmp_mode = layer_dev->run_info.compress_mode;
    dcmp_ctrl.ice_en = (layer_dev->run_info.compress_mode != DRV_GFX_COMPRESS_NONE) ? 1 : 0;
    dcmp_ctrl.dcmp_fmt = layer_dev->run_info.fmt;

    if ((layer_dev->canvas_surface.compress_mode == DRV_GFX_COMPRESS_HFBC_2X) ||
        (layer_dev->canvas_surface.compress_mode == DRV_GFX_COMPRESS_HFBC_3X)) {
        if (layer_dev->canvas_surface.color_fmt == DRV_GFX_FMT_RGB888) {
            dcmp0.pix_format = 0;
            // 24,16：block_size
            dcmp0.block_size = (layer_dev->canvas_surface.compress_mode == DRV_GFX_COMPRESS_HFBC_2X) ? 24 : 16;
        } else {
            dcmp0.pix_format = 1;
            // 24,32：block_size
            dcmp0.block_size = (layer_dev->canvas_surface.compress_mode == DRV_GFX_COMPRESS_HFBC_2X) ? 32 : 24;
        }

        dcmp0.round_offset           = 0x7FFF;
    }

    dpu_hal_layer_set_dcmp(layer_dev->layer_id, &dcmp_ctrl, &dcmp0, &dcmp1);
}

STATIC void dpu_layer_disp_rect_update(dpu_layer_dev *layer_dev)
{
    dpu_layer_run_info *run_info = &layer_dev->run_info;

    if (run_info->disp_rect.x + (td_s32)run_info->disp_rect.width > (td_s32)layer_dev->screen_width) {
        run_info->disp_rect.width = (td_u32)((td_s32)layer_dev->screen_width - run_info->disp_rect.x);
        run_info->crop_rect.width = run_info->disp_rect.width;
    }

    if (run_info->disp_rect.y + (td_s32)run_info->disp_rect.height > (td_s32)layer_dev->screen_height) {
        run_info->disp_rect.height = (td_u32)((td_s32)layer_dev->screen_height - run_info->disp_rect.y);
        run_info->crop_rect.height = run_info->disp_rect.height;
    }

    dpu_hal_layer_set_disp_rect(layer_dev->layer_id, &run_info->disp_rect);
}

STATIC void dpu_layer_mapping_config(dpu_layer_dev *layer_dev)
{
#ifdef GPU_1A22
    uapi_unused(layer_dev);
#else
    if (layer_dev->run_info.is_mapping == TD_TRUE) { // 当前层旋转，配置mapping
        dpu_hal_affine_config(layer_dev);
    } else if (g_dpu_context.is_rotate == TD_FALSE) { // 两层都没旋转，关mapping
        dpu_hal_disp_set_mapping_en(TD_FALSE);
    }
#endif
}

STATIC td_void dpu_layer_rotation_cfg_flip_mir(dpu_layer_dev *layer_dev)
{
    dpu_disp_info *disp_info = dpu_disp_get_info();
    layer_dev->run_info_change.flip_en = TD_TRUE;
    disp_info->output_info_change.mir_en = TD_TRUE;

    if (layer_dev->run_info.rotation_mode == DRV_GFX_ROTATION_180) {
        layer_dev->canvas_surface.flip_en = TD_TRUE;
        disp_info->output_info.mir_en = TD_TRUE;
    } else {
        layer_dev->canvas_surface.flip_en = TD_FALSE;
        disp_info->output_info.mir_en = TD_FALSE;
    }
}

STATIC td_void dpu_layer_config_update(dpu_layer_dev *layer_dev)
{
    dpu_dfx_info *dfx_info = dpu_dfx_get_info();
    dpu_dfx_layer_info *dfx_run_info = &dfx_info->layer_info[layer_dev->layer_id];

    if (layer_dev->run_info_change.global_alpha) {
        dpu_hal_layer_set_global_alpha(layer_dev->layer_id, layer_dev->run_info.global_alpha);
        layer_dev->run_info_change.global_alpha = TD_FALSE;
    }
    if (layer_dev->run_info_change.disp_rect) {
        dfx_run_info->disp_rect = layer_dev->run_info.disp_rect;
        dpu_layer_disp_rect_update(layer_dev);
        layer_dev->run_info_change.disp_rect = TD_FALSE;
    }
    if (layer_dev->run_info_change.crop_rect) {
        dfx_run_info->crop_rect = layer_dev->run_info.crop_rect;
        dpu_hal_layer_set_crop_rect(layer_dev->layer_id, &layer_dev->run_info.crop_rect);
        layer_dev->run_info_change.crop_rect = TD_FALSE;
    }
    if (layer_dev->run_info_change.alpha0_alpha1) {
        dpu_hal_layer_set_alpha0_alpha1(layer_dev->layer_id, layer_dev->run_info.alpha0, layer_dev->run_info.alpha1);
        layer_dev->run_info_change.alpha0_alpha1 = TD_FALSE;
    }
    if (layer_dev->run_info_change.fmt) {
        dfx_run_info->color_fmt = layer_dev->run_info.fmt;
        dpu_hal_fmt hal_fmt = dpu_layer_trans_fmt_drv_to_hal(layer_dev->run_info.fmt);
        dpu_hal_layer_set_fmt(layer_dev->layer_id, hal_fmt);
        layer_dev->run_info_change.fmt = TD_FALSE;
    }
    if (layer_dev->run_info_change.width) {
        dpu_hal_layer_set_width(layer_dev->layer_id, layer_dev->run_info.width);
        layer_dev->run_info_change.width = TD_FALSE;
    }
    if (layer_dev->run_info_change.height) {
        dpu_hal_layer_set_height(layer_dev->layer_id, layer_dev->run_info.height);
        layer_dev->run_info_change.height = TD_FALSE;
    }
    if (layer_dev->run_info_change.stride) {
        dpu_hal_layer_set_stride(layer_dev->layer_id, layer_dev->run_info.stride);
        layer_dev->run_info_change.stride = TD_FALSE;
    }
    if (layer_dev->run_info_change.addr) {
        dpu_hal_layer_set_addr(layer_dev->layer_id, layer_dev->run_info.current_addr);
        layer_dev->run_info_change.addr = TD_FALSE;
    }
    if (layer_dev->run_info_change.compress_mode) {
        dfx_run_info->compress_mode = layer_dev->run_info.compress_mode;
        dpu_layer_dcmp_config(layer_dev);
        layer_dev->run_info_change.compress_mode = TD_FALSE;
    }
    if (layer_dev->run_info_change.show) {
        dpu_hal_layer_set_enable(layer_dev->layer_id, layer_dev->run_info.show);
        layer_dev->run_info_change.show = TD_FALSE;
    }
    if (layer_dev->run_info_change.colorkey) {
        dpu_hal_layer_set_colorkey(layer_dev->layer_id, layer_dev->run_info.colorkey.key_en,
                                   layer_dev->run_info.colorkey.key_value);
        layer_dev->run_info_change.colorkey = TD_FALSE;
    }
    if (layer_dev->run_info_change.premulti_en || layer_dev->run_info_change.is_mapping) {
        dpu_hal_layer_set_premulti_en(layer_dev->layer_id, layer_dev->canvas_surface.premulti_en,
                                      layer_dev->run_info.is_mapping);
        layer_dev->run_info_change.premulti_en = TD_FALSE;
        layer_dev->run_info_change.is_mapping = TD_FALSE;
    }

    if (layer_dev->run_info_change.rotation_mode) {
        dpu_layer_rotation_cfg_flip_mir(layer_dev);
        dpu_hal_set_rotation_cfg(layer_dev->layer_id, layer_dev->canvas_surface.rotation_mode);
        layer_dev->run_info_change.rotation_mode = TD_FALSE;
    }

    if (layer_dev->run_info_change.flip_en) {
        dpu_hal_layer_set_flip_en(layer_dev->layer_id, layer_dev->canvas_surface.flip_en);
        layer_dev->run_info_change.flip_en = TD_FALSE;
    }

    dpu_layer_mapping_config(layer_dev);
    dpu_layer_yuv_base_config(layer_dev);
    dpu_hal_layer_set_reg_up(layer_dev->layer_id);
}

#ifndef GPU_SUPPORT_VIDEO_MODE
STATIC void dpu_layer_cmp_out_align_check(ext_rect *update_region)
{
    dpu_disp_info *disp_info = dpu_disp_get_info();
    drv_dpu_output *output_info = &disp_info->output_info;
    if (output_info->compress_output_mode != TD_TRUE) {
        return;
    }

    td_u32 screen_width  = dpu_disp_get_srceen_width();
    td_u32 screen_height = dpu_disp_get_srceen_height();
    if (update_region->width == screen_width && update_region->height == screen_height) {
        return;
    }

    if (update_region->x % LCD_CMP_OUT_WIDTH_ALIGN != 0 || update_region->y % LCD_CMP_OUT_HEIGHT_ALIGN != 0 ||
        update_region->width % LCD_CMP_OUT_WIDTH_ALIGN != 0 || update_region->height % LCD_CMP_OUT_HEIGHT_ALIGN != 0) {
        gpu_err("cmp region not align [%d %d %d %d]\n",
            update_region->x, update_region->y, update_region->width, update_region->height);
    }
}

STATIC td_void dpu_layer_calculate_gp_region(ext_rect *gp_region)
{
    td_u32 screen_width  = dpu_disp_get_srceen_width();
    td_u32 screen_height = dpu_disp_get_srceen_height();

    td_s32 end_x = 0, end_y = 0;
    td_s32 start_x = (td_s32)screen_width;
    td_s32 start_y = (td_s32)screen_height;

    for (td_u32 layer_id = 0; layer_id <= (td_u32)DRV_DPU_MAX_LAYER_ID; layer_id++) {
        if ((g_dpu_layer_dev[layer_id] == TD_NULL) || (g_dpu_layer_dev[layer_id]->is_create != TD_TRUE)) {
            continue;
        }

        dpu_layer_run_info *run_info = &g_dpu_layer_dev[layer_id]->run_info;
        dpu_layer_cmp_out_align_check(&run_info->update_region);
        /* if exists full screen layer, gp_reion is full */
        if (((run_info->update_region.width == screen_width) && (run_info->update_region.height == screen_height)) ||
            (dpu_disp_is_la_valid() == TD_TRUE)) {
            ext_rect full_rect = {0, 0, screen_width, screen_height};
            *gp_region = full_rect;
            return;
        }

        if (run_info->is_mapping) {
            *gp_region = run_info->update_region;
            return;
        }

        if (start_x > run_info->pos_x + run_info->update_region.x) {
            start_x = run_info->pos_x + run_info->update_region.x;
            start_x = (start_x < 0) ? 0 : start_x;
        }

        if (start_y > run_info->pos_y + run_info->update_region.y) {
            start_y = run_info->pos_y + run_info->update_region.y;
            start_y = (start_y < 0) ? 0 : start_y;
        }

        if (end_x < run_info->pos_x + run_info->update_region.x + (td_s32)run_info->update_region.width) {
            end_x = run_info->pos_x + run_info->update_region.x + (td_s32)run_info->update_region.width;
            end_x = (end_x > (td_s32)screen_width) ? (td_s32)screen_width : end_x;
        }

        if (end_y < run_info->pos_y + run_info->update_region.y + (td_s32)run_info->update_region.height) {
            end_y = run_info->pos_y + run_info->update_region.y + (td_s32)run_info->update_region.height;
            end_y = (end_y > (td_s32)screen_height) ? (td_s32)screen_height : end_y;
        }

        if ((end_x < start_x) || (end_y < start_y)) {
            gpu_err("invalid param, layer[%u] start[%d,%d] end[%d,%d] pos[%d,%d] region[%d,%d,%u,%u]",
                    layer_id, start_x, start_y, end_x, end_y, run_info->pos_x, run_info->pos_y,
                    run_info->update_region.x, run_info->update_region.y,
                    run_info->update_region.width, run_info->update_region.height);
        }
    }

    gp_region->x = start_x;
    gp_region->y = start_y;
    gp_region->width  = (td_u32)(end_x - start_x);
    gp_region->height = (td_u32)(end_y - start_y);
}
#endif

static td_bool dpu_layer_get_rotate_flag(void)
{
    td_bool is_rotate = TD_FALSE;

    for (td_u32 layer_id = 0; layer_id <= (td_u32)DRV_DPU_MAX_LAYER_ID; layer_id++) {
        if ((g_dpu_layer_dev[layer_id] == TD_NULL) || (g_dpu_layer_dev[layer_id]->is_create != TD_TRUE)) {
            continue;
        }

        if (g_dpu_layer_dev[layer_id]->run_info.is_mapping != 0) {
            is_rotate = TD_TRUE;
        }
    }
    return is_rotate;
}

static td_void dpu_layer_revise_update_region(dpu_layer_dev *layer_dev, const ext_rect *gp_region)
{
    layer_dev->run_info_change.disp_rect = TD_TRUE;
    layer_dev->run_info_change.crop_rect = TD_TRUE;

    dpu_layer_run_info *run_info = &layer_dev->run_info;
    if (run_info->is_mapping) {
        run_info->disp_rect.x      = 0;
        run_info->disp_rect.y      = 0;
        run_info->disp_rect.width  = gp_region->width;
        run_info->disp_rect.height = gp_region->height;
        run_info->crop_rect.x      = 0;
        run_info->crop_rect.y      = 0;
        run_info->crop_rect.width  = run_info->width;
        run_info->crop_rect.height = run_info->height;
        return;
    }

    ext_rect common_area = {};
    /* calculate the layer region and gp_region comon area */
    common_area.x = (run_info->pos_x > gp_region->x) ? run_info->pos_x : gp_region->x;
    common_area.y = (run_info->pos_y > gp_region->y) ? run_info->pos_y : gp_region->y;
    common_area.width  = (td_u32)(((run_info->pos_x + (td_s32)run_info->width) <
                         (gp_region->x + (td_s32)gp_region->width)) ?
                         (run_info->pos_x + (td_s32)run_info->width - common_area.x) :
                         (gp_region->x + (td_s32)gp_region->width - common_area.x));
    common_area.height = (td_u32)(((run_info->pos_y + (td_s32)run_info->height) <
                         (gp_region->y + (td_s32)gp_region->height)) ?
                         (run_info->pos_y + (td_s32)run_info->height - common_area.y) :
                         (gp_region->y + (td_s32)gp_region->height - common_area.y));

    /* calculate the layer display position in the gp region */
    run_info->disp_rect.x      = common_area.x - gp_region->x;
    run_info->disp_rect.y      = common_area.y - gp_region->y;
    run_info->disp_rect.width  = common_area.width;
    run_info->disp_rect.height = common_area.height;

    run_info->crop_rect.x      = common_area.x - run_info->pos_x;
    run_info->crop_rect.y      = common_area.y - run_info->pos_y;
    run_info->crop_rect.width  = common_area.width;
    run_info->crop_rect.height = common_area.height;

    /* revise the update region */
    run_info->update_region = run_info->crop_rect;
    return;
}

STATIC td_s32 dpu_check_update_rect(dpu_layer_run_info *run_info, const drv_dpu_surface *surface)
{
#ifdef GPU_SUPPORT_VIDEO_MODE
    uapi_unused(surface);
    run_info->update_region.x = 0;
    run_info->update_region.y = 0;
    run_info->update_region.width  = dpu_disp_get_srceen_width();
    run_info->update_region.height = dpu_disp_get_srceen_height();
#else
    if (surface->update_rect == TD_NULL) {
        run_info->update_region.x = 0;
        run_info->update_region.y = 0;
        run_info->update_region.width  = surface->width;
        run_info->update_region.height = surface->height;
        return EXT_SUCCESS;
    }

    /* check surface update rect */
    if (((surface->update_rect->x < 0) || (surface->update_rect->y < 0) ||
         (surface->update_rect->width == 0) || (surface->update_rect->height == 0) ||
         ((td_u32)surface->update_rect->x + surface->update_rect->width > surface->width) ||
         ((td_u32)surface->update_rect->y + surface->update_rect->height > surface->height) ||
         ((td_u32)surface->update_rect->x >= dpu_disp_get_srceen_width()) ||
         ((td_u32)surface->update_rect->y >= dpu_disp_get_srceen_height())) &&
         (surface->matrix == NULL)) {
        gpu_err("invalid param, update_rect[%d,%d,%u,%u], surface_wh[%u,%u]!",
                surface->update_rect->x, surface->update_rect->y,
                surface->update_rect->width, surface->update_rect->height,
                surface->width, surface->height);
        return DRV_ERR_DPU_OPERATION_NOT_SUPPORT;
    }

    /* revise update region */
    run_info->update_region = *surface->update_rect;
#endif
    return EXT_SUCCESS;
}

STATIC td_void dpu_layer_update_layer_info(dpu_layer_dev *layer_dev, const drv_dpu_surface *surface)
{
    layer_dev->update_flag = TD_TRUE;
    layer_dev->canvas_surface = *surface;

    if (surface->width != layer_dev->run_info.width) {
        layer_dev->run_info.width = surface->width;
        layer_dev->run_info_change.width = TD_TRUE;
    }

    if (surface->height != layer_dev->run_info.height) {
        layer_dev->run_info.height = surface->height;
        layer_dev->run_info_change.height = TD_TRUE;
    }

    if (surface->stride != layer_dev->run_info.stride) {
        layer_dev->run_info.stride = surface->stride;
        layer_dev->run_info_change.stride = TD_TRUE;
    }

    if (surface->phy_addr != layer_dev->run_info.current_addr) {
        layer_dev->run_info.current_addr  = surface->phy_addr;
        layer_dev->run_info_change.addr = TD_TRUE;
    }

    if (surface->color_fmt != layer_dev->run_info.fmt) {
        layer_dev->run_info.fmt = surface->color_fmt;
        layer_dev->run_info_change.fmt = TD_TRUE;
    }

    if (surface->premulti_en != layer_dev->run_info.premulti_en) {
        layer_dev->run_info.premulti_en = surface->premulti_en;
        layer_dev->run_info_change.premulti_en = TD_TRUE;
    }

    td_bool is_mapping = (surface->matrix != TD_NULL);
    if (is_mapping != layer_dev->run_info.is_mapping) {
        layer_dev->run_info.is_mapping = is_mapping;
        layer_dev->run_info_change.is_mapping = TD_TRUE;
    }
    if (is_mapping) {
        layer_dev->run_info.matrix = *(layer_dev->canvas_surface.matrix);
    }
    if ((surface->compress_mode != layer_dev->run_info.compress_mode) || (layer_dev->run_info_change.fmt == TD_TRUE)) {
        layer_dev->run_info.compress_mode = surface->compress_mode;
        layer_dev->run_info_change.compress_mode = TD_TRUE;
    }

    if (surface->flip_en != layer_dev->run_info.flip_en) {
        layer_dev->run_info.flip_en = surface->flip_en;
        layer_dev->run_info_change.flip_en = TD_TRUE;
    }

    if (surface->rotation_mode != layer_dev->run_info.rotation_mode) {
        layer_dev->run_info.rotation_mode = surface->rotation_mode;
        layer_dev->run_info_change.rotation_mode = TD_TRUE;
    }

    return;
}

PLT_SEC_ITCM_T INLINE td_void dpu_layer_hal_init(td_u32 layer_id)
{
    dpu_hal_layer_set_palpha_en(layer_id, TD_TRUE);
    dpu_hal_layer_set_fdr_chksum_en(layer_id, TD_TRUE);
}

STATIC td_void dpu_layer_hal_deinit(drv_dpu_layer_id layer_id)
{
    dpu_hal_layer_set_ck_gt_en(layer_id, TD_FALSE);
}

td_s32 dpu_layer_init(void)
{
    td_s32 ret;
    td_u32 layer_id;

    for (layer_id = (td_u32)DRV_DPU_LAYER_ID1; layer_id <= (td_u32)DRV_DPU_MAX_LAYER_ID; layer_id++) {
        g_dpu_layer_dev[layer_id] = (dpu_layer_dev *)osal_vmalloc(sizeof(dpu_layer_dev));
        if (g_dpu_layer_dev[layer_id] == TD_NULL) {
            dpu_layer_deinit();
            gpu_err("osal_vmalloc %u failed", (td_u32)sizeof(dpu_layer_dev));
            return DRV_ERR_DPU_MALLOC_FAILED;
        }
        (void)memset_s(g_dpu_layer_dev[layer_id], sizeof(dpu_layer_dev), 0, sizeof(dpu_layer_dev));
        ret = osal_spin_lock_init(&g_dpu_layer_dev[layer_id]->layer_lock);
        if (ret != EXT_SUCCESS) {
            dpu_layer_deinit();
            gpu_err("osal_spin_lock_init failed");
            return DRV_ERR_DPU_OSAL_FAILED;
        }
        dpu_layer_hal_init(layer_id);
    }

    return EXT_SUCCESS;
}

td_void dpu_layer_deinit(void)
{
    td_u32 layer_id;

    for (layer_id = (td_u32)DRV_DPU_LAYER_ID1; layer_id <= (td_u32)DRV_DPU_MAX_LAYER_ID; layer_id++) {
        if (g_dpu_layer_dev[layer_id] != TD_NULL) {
            dpu_layer_hal_deinit((drv_dpu_layer_id)layer_id);

            if (g_dpu_layer_dev[layer_id]->layer_lock.lock != TD_NULL) {
                osal_spin_lock_destroy(&g_dpu_layer_dev[layer_id]->layer_lock);
            }

            osal_vfree((td_void *)g_dpu_layer_dev[layer_id]);
            g_dpu_layer_dev[layer_id] = TD_NULL;
        }
    }

    return;
}

td_void dpu_layer_suspend(td_void)
{
    td_u32 layer_id;

    for (layer_id = 0; layer_id <= (td_u32)DRV_DPU_MAX_LAYER_ID; layer_id++) {
        if (g_dpu_layer_dev[layer_id] != TD_NULL) {
            g_dpu_context.is_create[layer_id] = g_dpu_layer_dev[layer_id]->is_create;
            if (g_dpu_context.is_create[layer_id]) {
                dpu_layer_destroy(layer_id);
            }
        }
    }
}

PLT_SEC_ITCM_T td_void dpu_layer_resume(td_void)
{
    td_u32 layer_id;

    for (layer_id = 0; layer_id <= (td_u32)DRV_DPU_MAX_LAYER_ID; layer_id++) {
        dpu_layer_hal_init(layer_id);
        if (g_dpu_context.is_create[layer_id]) {
            if (dpu_layer_create(&g_dpu_context.layer_info[layer_id]) != EXT_SUCCESS) {
                gpu_err("layer create err");
            }
        }
    }
}

PLT_SEC_ITCM_T td_s32 dpu_layer_create(const drv_dpu_layer_info *layer_info)
{
    td_s32 ret;
    unsigned long flag = 0;
    dpu_layer_dev *layer_dev = TD_NULL;

    if (layer_info == TD_NULL) {
        gpu_err("layer_info is null");
        return DRV_ERR_DPU_NULL_POINTER;
    }

    get_dpu_layer_device(layer_info->layer_id, layer_dev);

    ret = dpu_layer_check_layer_info(layer_info);
    if (ret != EXT_SUCCESS) {
        return ret;
    }

    osal_spin_lock_irqsave(&layer_dev->layer_lock, &flag);

    layer_dev->layer_id = layer_info->layer_id;
    dpu_layer_set_run_info(&layer_dev->run_info, layer_info);
    dpu_layer_init_run_info_change(&layer_dev->run_info_change);

    layer_dev->create_cnt++;
    layer_dev->is_create = TD_TRUE;

    osal_spin_unlock_irqrestore(&layer_dev->layer_lock, &flag);

    dpu_disp_open();

    layer_dev->screen_width  = dpu_disp_get_srceen_width();
    layer_dev->screen_height = dpu_disp_get_srceen_height();

    g_dpu_context.layer_info[layer_info->layer_id] = *layer_info;

    return EXT_SUCCESS;
}

td_s32 dpu_layer_destroy(drv_dpu_layer_id layer_id)
{
    unsigned long flag = 0;
    dpu_layer_dev *layer_dev = TD_NULL;

    get_dpu_layer_device(layer_id, layer_dev);

    osal_spin_lock_irqsave(&layer_dev->layer_lock, &flag);
    /* the layer has not been created */
    if (layer_dev->is_create != TD_TRUE) {
        osal_spin_unlock_irqrestore(&layer_dev->layer_lock, &flag);
        return EXT_SUCCESS;
    }

    layer_dev->create_cnt--;
    layer_dev->is_create = TD_FALSE;
    osal_spin_unlock_irqrestore(&layer_dev->layer_lock, &flag);

    dpu_disp_close();

    return EXT_SUCCESS;
}

td_s32 dpu_layer_set_alpha(drv_dpu_layer_id layer_id, td_u8 alpha)
{
    unsigned long flag = 0;
    dpu_layer_dev *layer_dev = TD_NULL;

    get_dpu_layer_device(layer_id, layer_dev);

    osal_spin_lock_irqsave(&layer_dev->layer_lock, &flag);
    layer_dev->update_flag = TD_TRUE;
    layer_dev->run_info.global_alpha = alpha;
    layer_dev->run_info_change.global_alpha = TD_TRUE;
    osal_spin_unlock_irqrestore(&layer_dev->layer_lock, &flag);

    return EXT_SUCCESS;
}

td_s32 dpu_layer_get_alpha(drv_dpu_layer_id layer_id, td_u8 *alpha)
{
    unsigned long flag = 0;
    dpu_layer_dev *layer_dev = TD_NULL;

    get_dpu_layer_device(layer_id, layer_dev);

    osal_spin_lock_irqsave(&layer_dev->layer_lock, &flag);
    *alpha = layer_dev->run_info.global_alpha;
    osal_spin_unlock_irqrestore(&layer_dev->layer_lock, &flag);

    return EXT_SUCCESS;
}

td_s32 dpu_layer_set_pos(drv_dpu_layer_id layer_id, td_s32 pos_x, td_s32 pos_y)
{
    unsigned long flag = 0;
    dpu_layer_dev *layer_dev = TD_NULL;

    get_dpu_layer_device(layer_id, layer_dev);

    if ((pos_x > (td_s32)layer_dev->screen_width) || (pos_y > (td_s32)layer_dev->screen_height) ||
        (pos_x % 2 != 0) || (pos_y % 2 != 0)) { /* 2: yuv data pos_xy is even */
        gpu_err("Invalid param, xy[%d,%d], screen_wh[%u,%u]\n",
                pos_x, pos_y, layer_dev->screen_width, layer_dev->screen_height);
        return DRV_ERR_DPU_OPERATION_NOT_SUPPORT;
    }

    osal_spin_lock_irqsave(&layer_dev->layer_lock, &flag);
    layer_dev->update_flag = TD_TRUE;
    layer_dev->run_info.pos_x = pos_x;
    layer_dev->run_info.pos_y = pos_y;

    /* position setting need change update region to full layer */
    layer_dev->run_info.update_region.x = 0;
    layer_dev->run_info.update_region.y = 0;
    layer_dev->run_info.update_region.width  = layer_dev->run_info.width;
    layer_dev->run_info.update_region.height = layer_dev->run_info.height;
    osal_spin_unlock_irqrestore(&layer_dev->layer_lock, &flag);

    return EXT_SUCCESS;
}

td_s32 dpu_layer_get_pos(drv_dpu_layer_id layer_id, td_s32 *pos_x, td_s32 *pos_y)
{
    unsigned long flag = 0;
    dpu_layer_dev *layer_dev = TD_NULL;

    get_dpu_layer_device(layer_id, layer_dev);

    osal_spin_lock_irqsave(&layer_dev->layer_lock, &flag);
    *pos_x = layer_dev->run_info.pos_x;
    *pos_y = layer_dev->run_info.pos_y;
    osal_spin_unlock_irqrestore(&layer_dev->layer_lock, &flag);

    return EXT_SUCCESS;
}

td_s32 dpu_layer_set_zorder(drv_dpu_layer_id layer_id, drv_dpu_zorder_mode zorder)
{
    unsigned long flag = 0;
    dpu_layer_dev *layer_dev = TD_NULL;

    get_dpu_layer_device(layer_id, layer_dev);

    osal_spin_lock_irqsave(&layer_dev->layer_lock, &flag);
    layer_dev->update_flag = TD_TRUE;
    dpu_disp_set_cbm_priority(layer_id, zorder);

    /* position setting need change update region to full layer */
    layer_dev->run_info.update_region.x = 0;
    layer_dev->run_info.update_region.y = 0;
    layer_dev->run_info.update_region.width  = layer_dev->run_info.width;
    layer_dev->run_info.update_region.height = layer_dev->run_info.height;
    osal_spin_unlock_irqrestore(&layer_dev->layer_lock, &flag);

    return EXT_SUCCESS;
}

td_s32 dpu_layer_get_zorder(drv_dpu_layer_id layer_id, td_u32 *zorder)
{
    unsigned long flag = 0;
    dpu_layer_dev *layer_dev = TD_NULL;

    get_dpu_layer_device(layer_id, layer_dev);

    osal_spin_lock_irqsave(&layer_dev->layer_lock, &flag);
    *zorder = dpu_disp_get_layer_zorder();
    osal_spin_unlock_irqrestore(&layer_dev->layer_lock, &flag);

    return EXT_SUCCESS;
}

td_s32 dpu_layer_show(drv_dpu_layer_id layer_id)
{
    unsigned long flag = 0;
    dpu_layer_dev *layer_dev = TD_NULL;

    get_dpu_layer_device(layer_id, layer_dev);

    osal_spin_lock_irqsave(&layer_dev->layer_lock, &flag);
    layer_dev->update_flag = TD_TRUE;
    layer_dev->run_info.show = TD_TRUE;
    layer_dev->run_info_change.show = TD_TRUE;
    osal_spin_unlock_irqrestore(&layer_dev->layer_lock, &flag);

    return EXT_SUCCESS;
}

td_s32 dpu_layer_hide(drv_dpu_layer_id layer_id)
{
    unsigned long flag = 0;
    dpu_layer_dev *layer_dev = TD_NULL;

    get_dpu_layer_device(layer_id, layer_dev);

    osal_spin_lock_irqsave(&layer_dev->layer_lock, &flag);
    layer_dev->update_flag = TD_TRUE;
    layer_dev->run_info.show = TD_FALSE;
    layer_dev->run_info_change.show = TD_TRUE;
    osal_spin_unlock_irqrestore(&layer_dev->layer_lock, &flag);

    return EXT_SUCCESS;
}

STATIC td_s32 dpu_layer_check_fmt(const drv_dpu_surface *surface)
{
    if ((surface->compress_mode == DRV_GFX_COMPRESS_HFBC_2X) || (surface->compress_mode == DRV_GFX_COMPRESS_HFBC_3X)) {
        if ((surface->color_fmt != DRV_GFX_FMT_ARGB8888) && (surface->color_fmt != DRV_GFX_FMT_RGB888)) {
            gpu_err("HFBC_2X or HFBC_3X only support ARGB8888 & RGB888. input fmt = %u", surface->color_fmt);
            return DRV_ERR_DPU_CHECK_PARAM_FAILED;
        }
    }

    if (surface->rotation_mode != DRV_GFX_ROTATION_NONE && surface->color_fmt != DRV_GFX_FMT_RGB888) {
        gpu_err("ratation only support RGB888. input = %u", surface->color_fmt);
        return DRV_ERR_DPU_CHECK_PARAM_FAILED;
    }

    return EXT_SUCCESS;
}

td_s32 dpu_layer_set_info(drv_dpu_layer_id layer_id, const drv_dpu_surface *surface)
{
    td_s32 ret;
    unsigned long flag = 0;
    dpu_layer_dev *layer_dev = TD_NULL;

    if (surface == TD_NULL) {
        gpu_err("null pointer!");
        return DRV_ERR_DPU_NULL_POINTER;
    }
    if (dpu_layer_check_fmt(surface) != EXT_SUCCESS) {
        gpu_err("dpu_layer_check_fmt failed");
        return DRV_ERR_DPU_CHECK_PARAM_FAILED;
    }

    get_dpu_layer_device(layer_id, layer_dev);

    osal_spin_lock_irqsave(&layer_dev->layer_lock, &flag);
    if (layer_dev->is_create != TD_TRUE) {
        gpu_err("layer%d is not created!\n", layer_id);
        osal_spin_unlock_irqrestore(&layer_dev->layer_lock, &flag);
        return DRV_ERR_DPU_LAYER_NOT_CREATED;
    }

    ret = dpu_check_update_rect(&layer_dev->run_info, surface);
    if (ret != EXT_SUCCESS) {
        gpu_err("update_rect is invalid, ret=%#x!\n", ret);
        osal_spin_unlock_irqrestore(&layer_dev->layer_lock, &flag);
        return ret;
    }

    dpu_layer_update_layer_info(layer_dev, surface);
    osal_spin_unlock_irqrestore(&layer_dev->layer_lock, &flag);

    dpu_dfx_info *dfx_info = dpu_dfx_get_info();
    dfx_info->layer_info[layer_id].phy_addr = surface->phy_addr;
    dfx_info->layer_info[layer_id].width = surface->width;
    dfx_info->layer_info[layer_id].height = surface->height;
    dfx_info->layer_info[layer_id].stride = surface->stride;
    dfx_info->layer_info[layer_id].compress_mode = surface->compress_mode;
    dfx_info->layer_info[layer_id].color_fmt = surface->color_fmt;

    return EXT_SUCCESS;
}

td_s32 dpu_layer_get_info(drv_dpu_layer_id layer_id, drv_dpu_surface *surface)
{
    unsigned long flag = 0;
    dpu_layer_dev *layer_dev = TD_NULL;

    if (surface == TD_NULL) {
        gpu_err("null pointer!");
        return DRV_ERR_DPU_NULL_POINTER;
    }

    get_dpu_layer_device(layer_id, layer_dev);

    osal_spin_lock_irqsave(&layer_dev->layer_lock, &flag);
    *surface = layer_dev->canvas_surface;
    osal_spin_unlock_irqrestore(&layer_dev->layer_lock, &flag);

    return EXT_SUCCESS;
}

td_s32 dpu_layer_set_colorkey(drv_dpu_layer_id layer_id, const drv_dpu_colorkey *colorkey)
{
    unsigned long flag = 0;
    dpu_layer_dev *layer_dev = TD_NULL;

    if (colorkey == TD_NULL) {
        gpu_err("null pointer!");
        return DRV_ERR_DPU_NULL_POINTER;
    }

    get_dpu_layer_device(layer_id, layer_dev);

    osal_spin_lock_irqsave(&layer_dev->layer_lock, &flag);
    layer_dev->update_flag = TD_TRUE;
    layer_dev->run_info.colorkey = *colorkey;
    layer_dev->run_info_change.colorkey = TD_TRUE;
    osal_spin_unlock_irqrestore(&layer_dev->layer_lock, &flag);

    return EXT_SUCCESS;
}

td_s32 dpu_layer_get_colorkey(drv_dpu_layer_id layer_id, drv_dpu_colorkey *colorkey)
{
    unsigned long flag = 0;
    dpu_layer_dev *layer_dev = TD_NULL;

    if (colorkey == TD_NULL) {
        gpu_err("null pointer!");
        return DRV_ERR_DPU_NULL_POINTER;
    }

    get_dpu_layer_device(layer_id, layer_dev);

    osal_spin_lock_irqsave(&layer_dev->layer_lock, &flag);
    *colorkey = layer_dev->run_info.colorkey;
    osal_spin_unlock_irqrestore(&layer_dev->layer_lock, &flag);

    return EXT_SUCCESS;
}

static td_void dpu_layer_set_info_to_disp(td_void)
{
    dpu_disp_info *disp_info = dpu_disp_get_info();
    drv_dpu_output *output_info = &disp_info->output_info;
    td_u32 layer_id;

    if (output_info->wbc_info != NULL) {
            if ((output_info->wbc_info->wbc_bind_mode - 1) >= 0 && (output_info->wbc_info->wbc_bind_mode - 1) <= 1) {
                layer_id =
                    (td_u32)((output_info->wbc_info->wbc_bind_mode - 1) == 0 ? DRV_DPU_LAYER_ID1 : DRV_DPU_LAYER_ID2);
                output_info->wbc_info->color_fmt = g_dpu_layer_dev[layer_id]->canvas_surface.color_fmt;
            } else {
                output_info->wbc_info->color_fmt = DRV_GFX_FMT_RGB888;
            }
    }

    for (layer_id = (td_u32)DRV_DPU_LAYER_ID1; layer_id <= (td_u32)DRV_DPU_MAX_LAYER_ID; layer_id++) {
        if (g_dpu_layer_dev[layer_id]->is_create == TD_TRUE) {
            disp_info->disp_layer_info[layer_id].layer_id      = g_dpu_layer_dev[layer_id]->layer_id;
            disp_info->disp_layer_info[layer_id].screen_width  = g_dpu_layer_dev[layer_id]->screen_width;
            disp_info->disp_layer_info[layer_id].screen_height = g_dpu_layer_dev[layer_id]->screen_height;
            disp_info->disp_layer_info[layer_id].is_create     = g_dpu_layer_dev[layer_id]->is_create;
            disp_info->disp_layer_info[layer_id].disp_rect     = g_dpu_layer_dev[layer_id]->run_info.disp_rect;
        }
    }
}

#ifdef SUPPORT_GPU_QSPI
static drv_gfx_fmt dpu_trans_lcd_fmt_2gfx(out_format_t lcd_type)
{
    drv_gfx_fmt api_type;

    switch (lcd_type) {
        case OUT_FORMAT_RGB_16BIT:
            api_type = DRV_GFX_FMT_RGB565;
            break;
        case OUT_FORMAT_RGB_24BIT:
            api_type = DRV_GFX_FMT_RGB888;
            break;
        case OUT_FORMAT_RGB_8BIT:
            api_type = DRV_GFX_FMT_RGB332;
            break;
        case OUT_FORMAT_RGB_12BIT:
            api_type = DRV_GFX_FMT_RGB444;
            break;
        case OUT_FORMAT_R_8BIT:
            api_type = DRV_GFX_FMT_R8;
            break;
        case OUT_FORMAT_G_8BIT:
            api_type = DRV_GFX_FMT_G8;
            break;
        case OUT_FORMAT_B_8BIT:
            api_type = DRV_GFX_FMT_B8;
            break;
        case OUT_FORMAT_R_4BIT:
            api_type = DRV_GFX_FMT_R4;
            break;
        case OUT_FORMAT_G_4BIT:
            api_type = DRV_GFX_FMT_G4;
            break;
        case OUT_FORMAT_B_4BIT:
            api_type = DRV_GFX_FMT_B4;
            break;
        default:
            api_type = DRV_GFX_FMT_MAX;
    }
    return api_type;
}

td_void dpu_layer_qspi_refresh(ext_rect *gp_region)
{
    dpu_disp_info* disp_info = dpu_disp_get_info();
    if (disp_info->output_info.wbc_info != NULL) {
        return;
    }
    out_format_t out_fmt = uapi_qspi_get_color_fmt();
    drv_gfx_fmt dpu_color_fmt = dpu_trans_lcd_fmt_2gfx(out_fmt);
    dpu_hal_set_dhd0_des_chksum_en(1);
    dpu_disp_qspi_config(gp_region, dpu_color_fmt);
    dpu_disp_set_chnl_reg_up();
    uapi_qspi_display_set_channel_cfg();
}
#endif

#ifdef GPU_SUPPORT_VIDEO_MODE // for mipi video mode
td_s32 dpu_layer_refresh(td_void)
{
    unsigned long flag = 0;
    ext_rect gp_region = { 0, 0, dpu_disp_get_srceen_width(), dpu_disp_get_srceen_height()};

    td_u32 time_start = 0, time_end = 0;
    dpu_dfx_info *dfx_info = dpu_dfx_get_info();
    if (dfx_info->is_skip_dpu_refresh) {
        return EXT_SUCCESS;
    }

    dfx_info->refresh_cnt++;
    dfx_info->refresh_total++;
    gpu_dfx_get_time_us(&time_start);

    dpu_disp_wait_vthd_percent_signal();
    dpu_isr_clear_finish_flag();
    dpu_isr_clear_vthd_percent_flag();

#if (GPU_LOW_POWER_SUPPORT == YES)
    uapi_pm_add_sleep_veto_with_symmetry(PM_ID_DPU);
#endif
    g_dpu_context.is_rotate = dpu_layer_get_rotate_flag();

    for (td_u32 layer_id = (td_u32)DRV_DPU_LAYER_ID1; layer_id <= (td_u32)DRV_DPU_MAX_LAYER_ID; layer_id++) {
        if (g_dpu_layer_dev[layer_id] == TD_NULL) {
            continue;
        }

        if ((g_dpu_layer_dev[layer_id]->is_create != TD_TRUE) || (g_dpu_layer_dev[layer_id]->update_flag != TD_TRUE)) {
            continue;
        }

        osal_spin_lock_irqsave(&g_dpu_layer_dev[layer_id]->layer_lock, &flag);
        dpu_layer_revise_update_region(g_dpu_layer_dev[layer_id], &gp_region);
        dpu_layer_config_update(g_dpu_layer_dev[layer_id]);

        g_dpu_layer_dev[layer_id]->update_flag = TD_FALSE;
        osal_spin_unlock_irqrestore(&g_dpu_layer_dev[layer_id]->layer_lock, &flag);
    }
    dpu_disp_update_clk(g_dpu_context.is_rotate);
    dpu_layer_set_info_to_disp();
    dpu_sre_process();
    dpu_disp_output_info_config();
    disp_set_chnl_enable(TD_TRUE);
    dpu_disp_set_chnl_reg_up();
    td_s32 ret = dpu_disp_wait_frame_done();
    gpu_dfx_get_time_us(&time_end);
    dfx_info->refresh_intf_cost = (td_float)(time_end - time_start);
    dpu_disp_save_timing_to_dfx();
    return ret;
}
#else

static void dpu_disp_and_layer_set(ext_rect *gp_region)
{
    dpu_disp_update_dhd_region(gp_region);
    dpu_disp_set_timing_and_screen_region(g_dpu_context.is_rotate);
    dpu_layer_set_info_to_disp();
    dpu_sre_process();
    dpu_disp_output_info_config();
    dpu_disp_set_chnl_reg_up();
    dpu_isr_clear_finish_flag();
    dpu_disp_save_timing_to_dfx();
}

td_s32 dpu_layer_refresh(void)
{
    unsigned long flag = 0;
    td_s32 ret = EXT_SUCCESS;
    ext_rect gp_region = { 0 };
    td_bool need_cur_done = TD_FALSE;

    td_u32 time_start = 0, time_end = 0;
    dpu_dfx_info *dfx_info = dpu_dfx_get_info();

    if (dfx_info->is_skip_dpu_refresh) {
        return EXT_SUCCESS;
    }

    dfx_info->refresh_cnt++;
    dfx_info->refresh_total++;
    gpu_dfx_get_time_us(&time_start);
    dfx_info->last_fresh_time = time_start;
#if (GPU_LOW_POWER_SUPPORT == YES)
    uapi_pm_add_sleep_veto_with_symmetry(PM_ID_DPU);
#endif
    dpu_layer_calculate_gp_region(&gp_region);
    g_dpu_context.is_rotate = dpu_layer_get_rotate_flag();

    for (td_u32 layer_id = (td_u32)DRV_DPU_LAYER_ID1; layer_id <= (td_u32)DRV_DPU_MAX_LAYER_ID; layer_id++) {
        if (g_dpu_layer_dev[layer_id] == TD_NULL) {
            continue;
        }

        if ((g_dpu_layer_dev[layer_id]->is_create != TD_TRUE) || (g_dpu_layer_dev[layer_id]->update_flag != TD_TRUE)) {
            continue;
        }

        if (g_dpu_layer_dev[layer_id]->vblank == TD_TRUE) {
            need_cur_done = TD_TRUE;
        } else {
            ret = dpu_disp_wait_frame_done(); /* wait last frame done */
        }

        osal_spin_lock_irqsave(&g_dpu_layer_dev[layer_id]->layer_lock, &flag);
        dpu_layer_revise_update_region(g_dpu_layer_dev[layer_id], &gp_region);
        dpu_layer_config_update(g_dpu_layer_dev[layer_id]);

        g_dpu_layer_dev[layer_id]->update_flag = TD_FALSE;
        osal_spin_unlock_irqrestore(&g_dpu_layer_dev[layer_id]->layer_lock, &flag);
    }
    dpu_disp_and_layer_set(&gp_region);
#ifdef SUPPORT_GPU_QSPI
    dpu_layer_qspi_refresh(&gp_region);
#endif
    dpu_hal_disp_set_frame_ready();
    uapi_lcd_exit_ulps_internal();

    if (need_cur_done == TD_TRUE) {
        ret = dpu_disp_wait_frame_done(); /* wait current frame done */
    }

    gpu_dfx_get_time_us(&time_end);
    dfx_info->refresh_intf_cost = (td_float)(time_end - time_start);

    return ret;
}
#endif // endif of GPU_SUPPORT_VIDEO_MODE

td_s32 dpu_layer_wait_vsync(void)
{
    dpu_disp_wait_te_signal();
    return EXT_SUCCESS;
}

td_s32 dpu_layer_set_vblank(drv_dpu_layer_id layer_id, td_bool vblank)
{
    unsigned long flag = 0;
    dpu_layer_dev *layer_dev = TD_NULL;

    get_dpu_layer_device(layer_id, layer_dev);

    osal_spin_lock_irqsave(&layer_dev->layer_lock, &flag);
    layer_dev->vblank = vblank;
    osal_spin_unlock_irqrestore(&layer_dev->layer_lock, &flag);

    return EXT_SUCCESS;
}

td_s32 dpu_layer_get_vblank(drv_dpu_layer_id layer_id, td_bool *vblank)
{
    unsigned long flag = 0;
    dpu_layer_dev *layer_dev = TD_NULL;

    get_dpu_layer_device(layer_id, layer_dev);

    osal_spin_lock_irqsave(&layer_dev->layer_lock, &flag);
    *vblank = layer_dev->vblank;
    osal_spin_unlock_irqrestore(&layer_dev->layer_lock, &flag);

    return  EXT_SUCCESS;
}

td_s32 dpu_layer_query_state(drv_dpu_layer_id layer_id)
{
    uapi_unused(layer_id);

    return TD_TRUE;
}
