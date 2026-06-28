/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.
 * Description: direct draw
 */

#include "vau_draw.h"
#include "vau_arc.h"
#include "vau_debug.h"
#include "vau_dfx.h"
#include "vau_mem.h"
#include "vau_hal_type.h"
#include "vau_node.h"
#include "vau_hal_vector_draw.h"
#include "vau_stroke_dash.h"
#include "vau_stroke_path.h"

#define VAU_GRADIENT_STOP_RATIO 128
#define VAU_GRADIENT_AUXILIARY  (128 * 256)

/* Max block size for split drawing */
#define VAU_SPLIT_MAX_BLOCK_WIDTH  640
#define VAU_SPLIT_MAX_BLOCK_HEIGHT 600

static drv_vau_blend_mode g_vau_src1_blend_cmd_mode[EXT_VAU_BLENDCMD_MAX] = {
    DRV_VAU_BLEND_INVSRC2ALPHA,
    DRV_VAU_BLEND_ZERO,
    DRV_VAU_BLEND_ZERO,
    DRV_VAU_BLEND_INVSRC2ALPHA,
    DRV_VAU_BLEND_ONE,
    DRV_VAU_BLEND_ZERO,
    DRV_VAU_BLEND_SRC2ALPHA,
    DRV_VAU_BLEND_ZERO,
    DRV_VAU_BLEND_INVSRC2ALPHA,
    DRV_VAU_BLEND_INVSRC2ALPHA,
    DRV_VAU_BLEND_SRC2ALPHA,
    DRV_VAU_BLEND_ONE,
    DRV_VAU_BLEND_INVSRC2ALPHA,
    DRV_VAU_BLEND_ONE,
    0
};

static drv_vau_blend_mode g_vau_src2_blend_cmd_mode[EXT_VAU_BLENDCMD_MAX] = {
    DRV_VAU_BLEND_SRC2ALPHA,
    DRV_VAU_BLEND_ZERO,
    DRV_VAU_BLEND_ONE,
    DRV_VAU_BLEND_ONE,
    DRV_VAU_BLEND_INVSRC1ALPHA,
    DRV_VAU_BLEND_SRC1ALPHA,
    DRV_VAU_BLEND_ZERO,
    DRV_VAU_BLEND_INVSRC1ALPHA,
    DRV_VAU_BLEND_ZERO,
    DRV_VAU_BLEND_SRC1ALPHA,
    DRV_VAU_BLEND_INVSRC1ALPHA,
    DRV_VAU_BLEND_ONE,
    DRV_VAU_BLEND_INVSRC1ALPHA,
    DRV_VAU_BLEND_ZERO,
    0
};

STATIC td_s32 vau_draw_matrix_x(const ext_vau_matrix *m, td_s32 x, td_s32 y)
{
    return (td_s32)((td_float)x * m->matrix[0][0] + (td_float)y * m->matrix[0][1] + m->matrix[0][2]); /* 0, 2:index */
}
STATIC td_s32 vau_draw_matrix_y(const ext_vau_matrix *m, td_s32 x, td_s32 y)
{
    return (td_s32)((td_float)x * m->matrix[1][0] + (td_float)y * m->matrix[1][1] + m->matrix[1][2]); /* 1, 2:index */
}

STATIC td_void vau_draw_make_src1(const ext_vau_surface *surface, vau_hardware_node *node)
{
    if (surface == TD_NULL) {
        return;
    }
    node->vau_src1_ctrl.bits.src1_en   = 1;
    node->vau_src1_ctrl.bits.src1_mode = surface->is_color_surface ? DRV_VAU_MODE_REG : DRV_VAU_MODE_MEM;
    node->vau_src1_ctrl.bits.src1_fmt  = (td_u32)surface->color_fmt;
    node->vau_src1_ctrl.bits.src1_argb_order = (td_u32)surface->argb_order;
    node->vau_src1_ctrl.bits.src1_rgb_exp    = DRV_VAU_RGB_EXP_ZERO;

    node->vau_src1_addr.bits.src1_addr           = surface->phy_addr;
    node->vau_src1_stride.bits.src1_stride       = surface->stride;
    node->vau_src1_imgsize.bits.src1_width       = surface->width - 1;
    node->vau_src1_imgsize.bits.src1_height      = surface->height - 1;
    node->vau_src1_rect_xy.bits.src1_rect_x      = (td_u16)surface->rect.x;
    node->vau_src1_rect_xy.bits.src1_rect_y      = (td_u16)surface->rect.y;
    node->vau_src1_rect_wh.bits.src1_rect_width  = surface->rect.width - 1;
    node->vau_src1_rect_wh.bits.src1_rect_height = surface->rect.height - 1;
    node->vau_src1_fill.bits.src1_color_fill     = surface->color;

    if ((surface->compress_mode == EXT_GFX_COMPRESS_HFBC_2X) ||
        (surface->compress_mode == EXT_GFX_COMPRESS_HFBC_3X)) {
        node->vau_src1_ctrl.bits.src1_ice_en   = 1;
        node->vau_src1_ctrl.bits.src1_cmp_mode = 1; // online mode
        if (surface->color_fmt == EXT_GFX_FMT_RGB888) {
            node->vau_src1_htc_online_dcmp0.bits.src1_dcmp_pix_format = 0;
            node->vau_src1_htc_online_dcmp0.bits.src1_block_size =
                (surface->compress_mode == EXT_GFX_COMPRESS_HFBC_2X) ? 24 : 16; // 16,24:bpp
        } else {
            node->vau_src1_htc_online_dcmp0.bits.src1_dcmp_pix_format = 1;
            node->vau_src1_htc_online_dcmp0.bits.src1_block_size =
                (surface->compress_mode == EXT_GFX_COMPRESS_HFBC_2X) ? 32 : 24; // 24,32:bpp
        }

        node->vau_src1_htc_online_dcmp0.bits.src1_dcmp_bit_depth  = 0;
        node->vau_src1_htc_online_dcmp0.bits.src1_dcmp_mpde       = 0;
        node->vau_src1_htc_online_dcmp0.bits.src1_dcmp_pq_sel     =
            (surface->compress_mode == EXT_GFX_COMPRESS_HFBC_2X) ? 0 : 1;
    }
    return;
}

STATIC td_s32 vau_draw_make_path(const ext_vau_path *path, vau_hardware_node *node)
{
    if ((path->cmds == TD_NULL) || (path->datas == TD_NULL)) {
        gpu_err("null pointer");
        return EXT_ERR_GFX_NULL_POINTER;
    }

    if (path->cmds[0] != VAU_PATH_MOVE_TO_ABS) {
        gpu_err("path should start width move to abs, now:%u", path->cmds[0]);
        return EXT_ERR_GFX_INVALID_PARAM;
    }

    if (uapi_is_byte_align((td_u32)(uintptr_t)path->datas, 4) != TD_TRUE) { // 4: align
        gpu_err("path data addr align err");
        return EXT_ERR_GFX_INVALID_ADDR;
    }

    // buffer lock
    td_bool is_internal_buff = vau_mem_is_internal(VAU_MEM_TYPE_PATH_CMD, path->cmds);
    if (is_internal_buff == TD_TRUE) {
        vau_mem_ref_inc(VAU_MEM_TYPE_PATH_CMD, path->cmds);
    }
    is_internal_buff = vau_mem_is_internal(VAU_MEM_TYPE_PATH_DATA, (td_uchar *)path->datas);
    if (is_internal_buff == TD_TRUE) {
        vau_mem_ref_inc(VAU_MEM_TYPE_PATH_DATA, (td_uchar *)path->datas);
    }

    node->mix.draw.vau_draw_ctrl.bits.draw_en = 1;
    node->mix.draw.vau_draw_ctrl.bits.dyn_bdbx_en = 1;
    node->mix.draw.vau_draw_ctrl.bits.bdbx_adj_en = 1;

    node->mix.draw.vau_draw_cmd_addr.u32           = (td_u32)(uintptr_t)path->cmds;
    node->mix.draw.vau_draw_data_addr.u32          = (td_u32)(uintptr_t)path->datas;
    node->mix.draw.vau_draw_path_num.bits.cmd_num  = path->cmd_num;
    node->mix.draw.vau_draw_path_num.bits.data_num = path->data_num;

    return EXT_SUCCESS;
}

STATIC td_void vau_draw_make_matrix(const ext_vau_matrix *m, vau_hardware_node *node)
{
    node->mix.draw.vau_draw_matrix_00.bits.matrix_00 = m->matrix[0][0];
    node->mix.draw.vau_draw_matrix_01.bits.matrix_01 = m->matrix[0][1];
    node->mix.draw.vau_draw_matrix_02.bits.matrix_02 = m->matrix[0][2]; /* 0: row, 2: col */
    node->mix.draw.vau_draw_matrix_10.bits.matrix_10 = m->matrix[1][0];
    node->mix.draw.vau_draw_matrix_11.bits.matrix_11 = m->matrix[1][1];
    node->mix.draw.vau_draw_matrix_12.bits.matrix_12 = m->matrix[1][2]; /* 1: row, 2: col */
#ifdef GPU_3322
    node->mix.draw.vau_draw_matrix_20.bits.matrix_20 = m->matrix[2][0]; /* 2: row, 0: col */
    node->mix.draw.vau_draw_matrix_21.bits.matrix_21 = m->matrix[2][1]; /* 2: row, 1: col */
    node->mix.draw.vau_draw_matrix_22.bits.matrix_22 = m->matrix[2][2]; /* 2: row, 2: col */
#endif
}

STATIC td_s32 vau_draw_make_gradient_stops(const ext_vau_paint_attr *paint, vau_hardware_node *node)
{
    if (paint->gradient.stops.num < 2) { /* 2: at least 2 points: start and end */
        gpu_err("invalid offset num: %u", paint->gradient.stops.num);
        return EXT_ERR_GFX_INVALID_PARAM;
    }
    for (int i = 0; i < paint->gradient.stops.num; i++) {
        if ((paint->gradient.stops.offsets[i] < 0) || (paint->gradient.stops.offsets[i] > 1)) {
            gpu_err("invalid offset at %d: %f", i, paint->gradient.stops.offsets[i]);
            return EXT_ERR_GFX_INVALID_PARAM;
        }
        if (i < 4) { /* 4 offsets in offset0 */
            node->vau_gradient_stops_offset0.u32 |=
                // 8: 8 bits per offset
                (td_u32)(paint->gradient.stops.offsets[i] * VAU_GRADIENT_STOP_RATIO) << (td_u32)(i * 8);
        } else {
            node->vau_gradient_stops_offset1.u32 |=
                // 8: 8 bits per offset; 4: index to [0, 3]
                (td_u32)(paint->gradient.stops.offsets[i] * VAU_GRADIENT_STOP_RATIO) << (td_u32)((i - 4) * 8);
        }
    }

    for (td_s32 i = 0; i < paint->gradient.stops.num - 1; i++) {
        if (paint->gradient.stops.offsets[i + 1] > paint->gradient.stops.offsets[i]) {
            node->vau_gradient_color_reci[i].bits.color_reci01 =
                (1.0 /
                    (((td_u32)(paint->gradient.stops.offsets[i + 1] * VAU_GRADIENT_STOP_RATIO) -
                    (td_u32)(paint->gradient.stops.offsets[i] * VAU_GRADIENT_STOP_RATIO)) * 256)); // 256 scaling factor
        } else {
            gpu_err("offset need to be incremental at %d: %f, %d: %f", i, paint->gradient.stops.offsets[i],
                i + 1, paint->gradient.stops.offsets[i + 1]);
            return EXT_ERR_GFX_INVALID_PARAM;
        }
        node->vau_gradient_stops_color[i].bits.stops_color0 = paint->gradient.stops.colors[i];
    }
    node->vau_gradient_stops_color[paint->gradient.stops.num - 1].bits.stops_color0 =
        paint->gradient.stops.colors[paint->gradient.stops.num - 1];
    node->vau_gradient_stops_num.bits.stops_num = paint->gradient.stops.num;
    node->vau_gradient_tile_mode.bits.tile_mode = (td_u32)paint->gradient.tile_mode;
    return EXT_SUCCESS;
}

STATIC td_void vau_draw_make_gadient_linear(const ext_vau_linear_gradient_attr *linear,
                                            const ext_vau_matrix *m, vau_hardware_node *node)
{
    ext_vau_point start_point = { vau_draw_matrix_x(m, linear->start_point.x, linear->start_point.y),
        vau_draw_matrix_y(m, linear->start_point.x, linear->start_point.y) };
    ext_vau_point end_point = { vau_draw_matrix_x(m, linear->end_point.x, linear->end_point.y),
        vau_draw_matrix_y(m, linear->end_point.x, linear->end_point.y) };

    node->vau_gradient_linear_start.bits.linear_start_x = (td_s16)start_point.x;
    node->vau_gradient_linear_start.bits.linear_start_y = (td_s16)start_point.y;
    td_s32 dx = end_point.x - start_point.x;
    td_s32 dy = end_point.y - start_point.y;
    td_s32 usq = dx * dx + dy * dy;
    if ((dx == 0) && (dy == 0)) {
        node->vau_gradient_const_mode.bits.linear_const_mode = 0;
        node->vau_gradient_linear_const_x.bits.linear_const_x = 0;
        node->vau_gradient_linear_const_y.bits.linear_const_y = 0;
    } else if ((dx == 0) && (dy != 0)) {
        node->vau_gradient_const_mode.bits.linear_const_mode = 1; // 1: horizon
        node->vau_gradient_linear_const_x.bits.linear_const_x = 0;
        node->vau_gradient_linear_const_y.bits.linear_const_y = VAU_GRADIENT_AUXILIARY * (td_float)dy / usq;
    } else if ((dx != 0) && (dy == 0)) {
        node->vau_gradient_const_mode.bits.linear_const_mode = 2; // 2: vertical
        node->vau_gradient_linear_const_x.bits.linear_const_x = VAU_GRADIENT_AUXILIARY * (td_float)dx / usq;
        node->vau_gradient_linear_const_y.bits.linear_const_y = 0;
    } else {
        node->vau_gradient_const_mode.bits.linear_const_mode = 3; // 3: slash
        node->vau_gradient_linear_const_x.bits.linear_const_x = VAU_GRADIENT_AUXILIARY * (td_float)dx / usq;
        node->vau_gradient_linear_const_y.bits.linear_const_y = VAU_GRADIENT_AUXILIARY * (td_float)dy / usq;
    }
}

STATIC td_void vau_draw_make_gadient_radial(const ext_vau_radial_gradient_attr *radial,
                                            const ext_vau_matrix *m, vau_hardware_node *node)
{
    node->vau_gradient_radial_center.bits.radial_center_x =
        (td_u32)vau_draw_matrix_x(m, radial->center.x, radial->center.y);
    node->vau_gradient_radial_center.bits.radial_center_y =
        (td_u32)vau_draw_matrix_y(m, radial->center.x, radial->center.y);
    node->vau_gradient_radial_const_r.bits.radial_const_r =
        VAU_GRADIENT_AUXILIARY / (td_float)radial->radius;
}

STATIC td_void vau_draw_make_gadient_sweep(const ext_vau_sweep_gradient_attr *sweep,
                                           const ext_vau_matrix *m, vau_hardware_node *node)
{
    node->vau_gradient_sweep_center.bits.sweep_center_x =
        (td_u32)vau_draw_matrix_x(m, sweep->center.x, sweep->center.y);
    node->vau_gradient_sweep_center.bits.sweep_center_y =
        (td_u32)vau_draw_matrix_y(m, sweep->center.x, sweep->center.y);
    node->vau_gradient_sweep_start_angle.bits.sweep_start_angle = sweep->start_angle;
}

STATIC td_s32 vau_draw_make_paint(const ext_vau_paint_attr *paint, td_u32 fill_rule,
    ext_vau_render_quality render_quality, const ext_vau_matrix *m, vau_hardware_node *node)
{
    node->mix.draw.vau_draw_ctrl.bits.msaa_mode = (render_quality == EXT_VAU_RENDER_QUALITY_NONE_AA) ?
        DRV_VAU_NONE_MSAA : DRV_VAU_8X_CFG_MSAA;
    node->mix.draw.vau_draw_ctrl.bits.fill_mode   = fill_rule;
    node->mix.draw.vau_draw_ctrl.bits.fill_effect = (td_u32)paint->paint_type;

    switch (paint->paint_type) {
        case EXT_VAU_PAINT_TYPE_COLOR:
        case EXT_VAU_PAINT_TYPE_COLOR_ALPHA_FF:
            node->vau_gradient_typical_color.u32 = paint->solid.color;
            return EXT_SUCCESS;
        case EXT_VAU_PAINT_TYPE_LINEAR_GRADIENT:
            vau_draw_make_gadient_linear(&paint->gradient.linear, m, node);
            break;
        case EXT_VAU_PAINT_TYPE_RADIAL_GRADIENT:
            if (paint->gradient.radial.radius == 0) {
                gpu_err("radial radius should not be 0");
                return EXT_ERR_GFX_INVALID_PARAM;
            }
            vau_draw_make_gadient_radial(&paint->gradient.radial, m, node);
            break;
        case EXT_VAU_PAINT_TYPE_SWEEP_GRADIENT:
            vau_draw_make_gadient_sweep(&paint->gradient.sweep, m, node);
            break;
        default:
            gpu_err("invalid paint_type:0x%x", paint->paint_type);
            return EXT_ERR_GFX_INVALID_PARAM;
    }
    return vau_draw_make_gradient_stops(paint, node);
}

STATIC td_void vau_draw_make_dst(const ext_vau_surface *surface, vau_hardware_node *node)
{
    node->vau_des_ctrl.bits.des_en = 1;
    node->vau_des_ctrl.bits.des_fmt = surface->color_fmt;
    node->vau_des_ctrl.bits.des_argb_order = surface->argb_order;
    node->vau_des_imgsize.bits.des_width = surface->width - 1;
    node->vau_des_imgsize.bits.des_height = surface->height - 1;
    node->vau_des_stride.bits.des_stride = surface->stride;
    node->vau_des_addr.u32 = surface->phy_addr;

    node->vau_des_rect_xy.bits.des_rect_x = (td_u32)surface->rect.x;
    node->vau_des_rect_xy.bits.des_rect_y = (td_u32)surface->rect.y;
    node->vau_des_rect_wh.bits.des_rect_width = surface->rect.width - 1;
    node->vau_des_rect_wh.bits.des_rect_height = surface->rect.height - 1;

    node->vau_des_alpha.bits.des_alpha_thd = 0x7F;

    node->mix.draw.vau_draw_st.bits.draw_x_start = (td_u32)surface->rect.x;
    node->mix.draw.vau_draw_st.bits.draw_y_start = (td_u32)surface->rect.y;
    node->mix.draw.vau_draw_end.bits.draw_x_end = surface->rect.x + surface->rect.width - 1;
    node->mix.draw.vau_draw_end.bits.draw_y_end = surface->rect.y + surface->rect.height - 1;

    if ((surface->compress_mode == EXT_GFX_COMPRESS_HFBC_2X) ||
        (surface->compress_mode == EXT_GFX_COMPRESS_HFBC_3X)) {
        node->vau_des_ctrl.bits.des_ice_en = 1;
        if (surface->color_fmt == EXT_GFX_FMT_RGB888) {
            node->vau_des_htc_online_cmp0.bits.pix_format = 0;
            node->vau_des_htc_online_cmp0.bits.des_block_size =
                (surface->compress_mode == EXT_GFX_COMPRESS_HFBC_2X) ? 24 : 16; // 16,24:bpp
        } else {
            node->vau_des_htc_online_cmp0.bits.pix_format = 1;
            node->vau_des_htc_online_cmp0.bits.des_block_size =
                (surface->compress_mode == EXT_GFX_COMPRESS_HFBC_2X) ? 32 : 24; // 24,32:bpp
        }

        node->vau_des_htc_online_cmp0.bits.bypass_en  = 0;
        node->vau_des_htc_online_cmp0.bits.bit_depth  = 0;
        node->vau_des_htc_online_cmp0.bits.cmp_pq_sel =
            (surface->compress_mode == EXT_GFX_COMPRESS_HFBC_2X) ? 0 : 1;
    }
}

STATIC td_void vau_draw_make_cbm(const ext_vau_surface_list *surface_list, vau_hardware_node *node)
{
    node->vau_cbm_ctrl.bits.cbm_en = 1;
    if (surface_list->src_surface_cnt == 1 || surface_list->opt == TD_NULL) {
        node->vau_cbm_ctrl.bits.alu_mode = DRV_VAU_ALU_MODE_SRC2_BYPASS;
        return;
    }
    node->vau_cbm_ctrl.bits.alu_mode = DRV_VAU_ALU_MODE_BLEND;
    node->vau_cbm_src1_cbm_para.bits.s1_premult_en = surface_list->src_surface[0].alpha_info.premult_en;
    node->vau_cbm_src1_cbm_para.bits.s1_palpha_en = surface_list->src_surface[0].alpha_info.pixel_alpha_en;
    node->vau_cbm_src2_cbm_para.bits.s2_premult_en = surface_list->src_surface[1].alpha_info.premult_en;
    node->vau_cbm_src2_cbm_para.bits.s2_palpha_en = surface_list->src_surface[1].alpha_info.pixel_alpha_en;

    td_u32 src1, src2;
    if (surface_list->opt->blend_opt.blend_cmd == EXT_VAU_BLENDCMD_CONFIG) {
        src1 = (td_u32)surface_list->opt->blend_opt.background_blend_mode;
        src2 = (td_u32)surface_list->opt->blend_opt.foreground_blend_mode;
    } else {
        src1 = g_vau_src1_blend_cmd_mode[(td_u32)surface_list->opt->blend_opt.blend_cmd % EXT_VAU_BLENDCMD_MAX];
        src2 = g_vau_src2_blend_cmd_mode[(td_u32)surface_list->opt->blend_opt.blend_cmd % EXT_VAU_BLENDCMD_MAX];
    }
    node->vau_cbm_src1_cbm_para.bits.s1_blend_mode = src1;
    node->vau_cbm_src2_cbm_para.bits.s2_blend_mode = src2;

    node->vau_cbm_alu_para.bits.alpha_from = (td_u32)surface_list->opt->blend_opt.out_alpha_mode;
}

STATIC td_void vau_draw_set_hw_stroke_para(const ext_vau_stroke_attr *stroke_attr, vau_hardware_node *node)
{
    node->mix.draw.vau_draw_ctrl.bits.stroke_en = 1;
    node->mix.draw.vau_draw_ctrl.bits.cap_mode  = (td_u32)stroke_attr->cap;
    node->mix.draw.vau_draw_ctrl.bits.join_mode = (td_u32)stroke_attr->join;
    node->mix.draw.vau_draw_half_width.bits.half_width = stroke_attr->width / 2.0; // 2.0 div

    // default APB: RND_CAP_TES_NUM/TES_ANGLE/TES_SIN/TES_COS
    node->mix.draw.vau_draw_miter_limit.bits.miter_limit =
        ((stroke_attr->miter_limit * stroke_attr->miter_limit) - 1.0f) *
        (stroke_attr->width / 2.0) * (stroke_attr->width / 2.0);  // 2.0 div
}

STATIC td_void vau_draw_set_hw_stroke(const ext_vau_stroke_attr *stroke_attr, vau_hardware_node *node,
    const ext_vau_matrix *m)
{
#ifndef GPU_SUPPORT_RM_STROKE
    const td_float tes_angle = (5.0f * 3.1415926 / 180.0f);

    td_float a11 = m->matrix[0][0];
    td_float a12 = m->matrix[0][1];
    td_float a13 = m->matrix[0][2];
    td_float a21 = m->matrix[1][0];
    td_float a22 = m->matrix[1][1];
    td_float a23 = m->matrix[1][2];

    td_float a_determinant = (a11 * a22 - a12 * a21);
    if (vau_math_float_is_zero(a_determinant) == TD_TRUE) {
        gpu_err("a_determinant zero");
        return;
    }

    td_float inv_a_determinant = 1.0f / a_determinant;
    td_float cos_value = cosf(-tes_angle);
    td_float sin_value = sinf(-tes_angle);
    td_float b11 = cos_value;
    td_float b12 = -sin_value;
    td_float b21 = sin_value;
    td_float b22 = cos_value;
    td_float c11 = a22 * inv_a_determinant;
    td_float c12 = -a12 * inv_a_determinant;
    td_float c13 = (a12 * a23 - a13 * a22) * inv_a_determinant;
    td_float c21 = -a21 * inv_a_determinant;
    td_float c22 = a11 * inv_a_determinant;
    td_float c23 = (a13 * a21 - a11 * a23) * inv_a_determinant;
    td_float t1  = (b11 * c11 + b12 * c21);
    td_float t2  = (b21 * c11 + b22 * c21);
    td_float t3  = (b11 * c12 + b12 * c22);
    td_float t4  = (b21 * c12 + b22 * c22);
    td_float t5  = (b11 * c13 + b12 * c23);
    td_float t6  = (b21 * c13 + b22 * c23);

    node->mix.draw.vau_draw_rnd_m11.bits.rnd_m11 = (a11 * t1 + a12 * t2);
    node->mix.draw.vau_draw_rnd_m12.bits.rnd_m12 = (a11 * t3 + a12 * t4);
    node->mix.draw.vau_draw_rnd_m21.bits.rnd_m21 = (a21 * t1 + a22 * t2);
    node->mix.draw.vau_draw_rnd_m22.bits.rnd_m22 = (a21 * t3 + a22 * t4);
    node->mix.draw.vau_draw_rnd_d1.bits.rnd_d1   = (a11 * t5 + a12 * t6 + a13);
    node->mix.draw.vau_draw_rnd_d2.bits.rnd_d2   = (a21 * t5 + a22 * t6 + a23);
    node->mix.draw.vau_draw_rnd_e11.bits.rnd_e11 = (-a11 * cos_value - a12 * sin_value + a11);
    node->mix.draw.vau_draw_rnd_e12.bits.rnd_e12 = (a11 * sin_value - a12 * cos_value + a12);
    node->mix.draw.vau_draw_rnd_e21.bits.rnd_e21 = (-a21 * cos_value - a22 * sin_value + a21);
    node->mix.draw.vau_draw_rnd_e22.bits.rnd_e22 = (a21 * sin_value - a22 * cos_value + a22);

    vau_draw_set_hw_stroke_para(stroke_attr, node);
#endif
}

STATIC void vau_draw_set_quad_split_thd(ext_vau_matrix *m, vau_hardware_node *node)
{
#ifdef GPU_3322
    td_float thrd_hard = 1.189207;  // 1.189207 thrd_quad  0.25f
    td_float scale = (m->matrix[0][0] + m->matrix[1][1]) / 2.0f;  // 2.0

    if (scale <= 2.6f) {         // 0 ~ 2.6
        thrd_hard = 1.189207;    // 1.189207 0.25f
    } else if (scale <= 7.5f) {  // 2.7 ~ 7.5
        thrd_hard = 1.681792;    // 1.681792 0.125f
    } else {                     // > 7.5
        thrd_hard = 4.854917;    // 4.854917 0.015f
    }

    node->mix.draw.vau_draw_threshold_quad.bits.threshold_quad = thrd_hard;
#else
    uapi_unused(m);
    uapi_unused(node);
#endif
}

STATIC td_s32 vau_draw_hardware_node_cfg(td_handle handle, const ext_vau_surface_list *surface_list,
    ext_vau_draw_attr *draw_attr, td_bool is_stroke)
{
    ext_vau_surface *src1 = (surface_list->src_surface_cnt == 1) ? TD_NULL : &surface_list->src_surface[0];
    ext_vau_surface *src2 = (surface_list->src_surface_cnt == 1) ? &surface_list->src_surface[0] : &surface_list->src_surface[1];

    ext_vau_matrix identity;
    vau_matrix_identity(&identity);
    ext_vau_matrix *m = (src2->matrix == TD_NULL) ? &identity : src2->matrix;

    vau_sw_node *sw_node = vau_node_create(VAU_MIX_NODE_DRAW);
    if (sw_node == TD_NULL) {
        gpu_err("vau_node_create failed");
        return DRV_ERR_VAU_NO_MEM;
    }

    vau_hardware_node *hw_node = (vau_hardware_node *)sw_node->hw_node;
    if (hw_node == TD_NULL) {
        gpu_err("invalid hw node!");
        return DRV_ERR_VAU_NULL_PTR;
    }

    td_s32 ret = vau_draw_make_path(draw_attr->path, hw_node);
    if (ret != EXT_SUCCESS) {
        vau_node_free(sw_node);
        gpu_err("vau_draw_make_path failed");
        return ret;
    }

    td_u32 type;
    ext_vau_paint_attr *paint_attr;
    if (is_stroke) {
        type = EXT_VAU_NON_ZERO_FILL;
        paint_attr = &draw_attr->paint->stroke_attr->paint_attr;
        vau_draw_set_hw_stroke(draw_attr->paint->stroke_attr, hw_node, m);
    } else {
        type = draw_attr->paint->fill_attr->type;
        paint_attr = &draw_attr->paint->fill_attr->paint_attr;
    }

    ret = vau_draw_make_paint(paint_attr, type, draw_attr->paint->render_cfg.render_quality, m, hw_node);
    if (ret != EXT_SUCCESS) {
        vau_node_free(sw_node);
        gpu_err("vau_draw_make_paint failed");
        return ret;
    }
    vau_draw_set_quad_split_thd(m, sw_node->hw_node);
    vau_draw_make_matrix(m, hw_node);
    vau_draw_make_src1(src1, hw_node);
    vau_draw_make_dst(surface_list->dst_surface, hw_node);
    vau_draw_make_cbm(surface_list, hw_node);
    if (vau_node_add(handle, sw_node) != EXT_SUCCESS) {
        vau_node_free(sw_node);
        gpu_err("vau_node_add failed");
        return EXT_FAILURE;
    }
    return EXT_SUCCESS;
}

STATIC td_u32 vau_get_bpp_from_colorfmt(ext_gfx_fmt color_fmt)
{
    td_u32 bpp = 8;

    switch (color_fmt) {
        case EXT_GFX_FMT_ARGB8888:
        case EXT_GFX_FMT_ARGB2101010:
            bpp = 32; /* 32 bits */
            break;
        case EXT_GFX_FMT_RGB888:
        case EXT_GFX_FMT_ARGB8565:
            bpp = 24; /* 24 bits */
            break;
        case EXT_GFX_FMT_ARGB1555:
        case EXT_GFX_FMT_ARGB4444:
        case EXT_GFX_FMT_RGB565:
            bpp = 16; /* 16 bits */
            break;
        case EXT_GFX_FMT_A8:
        case EXT_GFX_FMT_R8:
        case EXT_GFX_FMT_G8:
        case EXT_GFX_FMT_B8:
        case EXT_GFX_FMT_RGB332:
        case EXT_GFX_FMT_YUV400SP:
        case EXT_GFX_FMT_YUV422SP:
        case EXT_GFX_FMT_YUV420SP:
        case EXT_GFX_FMT_YUV444SP:
            bpp = 8; /* 8 bits */
            break;
        case EXT_GFX_FMT_A4:
            bpp = 4; /* 4 bits */
            break;
        case EXT_GFX_FMT_A2:
            bpp = 2; /* 2 bits */
            break;
        case EXT_GFX_FMT_A1:
            bpp = 1; /* 1 bits */
            break;
        default:
            bpp = 32; /* 32 bits */
            gpu_err("invalid format: %d", color_fmt);
            break;
    }

    return bpp;
}

/* Block drawing context for split rendering */
typedef struct {
    td_u32 block_w;
    td_u32 block_h;
    td_u32 block_num_x;
    td_u32 block_num_y;
    td_u32 surface_width;
    td_u32 surface_height;
    td_u32 dx;
    td_u32 dy;
    td_u32 cur_block_w;
    td_u32 cur_block_h;
    ext_rect block_rect;
    const ext_vau_surface *dst_surface;
    const ext_vau_surface *src1;
    const ext_vau_surface *src2;
} vau_split_ctx;

/* Calculate block size for split drawing */
STATIC td_void vau_calc_split_block_size(const ext_vau_surface *dst_surface, vau_split_ctx *ctx)
{
    ctx->block_w = MIN(dst_surface->rect.width, VAU_SPLIT_MAX_BLOCK_WIDTH);
    ctx->block_h = MIN(dst_surface->rect.height, VAU_SPLIT_MAX_BLOCK_HEIGHT);
    ctx->block_num_x = (dst_surface->rect.width + ctx->block_w - 1) / ctx->block_w;
    ctx->block_num_y = (dst_surface->rect.height + ctx->block_h - 1) / ctx->block_h;
    ctx->surface_width = dst_surface->rect.width;
    ctx->surface_height = dst_surface->rect.height;
}

/* Calculate block rectangle dimensions */
STATIC td_void vau_calc_block_rect(td_u32 x, td_u32 y, vau_split_ctx *ctx)
{
    ctx->dx = x * ctx->block_w + ctx->dst_surface->rect.x;
    ctx->dy = y * ctx->block_h + ctx->dst_surface->rect.y;
    ctx->cur_block_w = (x == ctx->block_num_x - 1) ?
        (ctx->surface_width - (ctx->block_num_x - 1) * ctx->block_w) : ctx->block_w;
    ctx->cur_block_h = (y == ctx->block_num_y - 1) ?
        (ctx->surface_height - (ctx->block_num_y - 1) * ctx->block_h) : ctx->block_h;
    ctx->block_rect.x = 0;
    ctx->block_rect.y = 0;
    ctx->block_rect.width = ctx->cur_block_w;
    ctx->block_rect.height = ctx->cur_block_h;
}

/* Update src1 surface address for block */
STATIC td_void vau_update_src1_for_block(const vau_split_ctx *ctx, ext_vau_surface *modified_src)
{
    if (ctx->src1 == TD_NULL) {
        return;
    }

    td_u32 dx_src1 = ctx->dx + ctx->src1->rect.x - ctx->dst_surface->rect.x;
    td_u32 dy_src1 = ctx->dy + ctx->src1->rect.y - ctx->dst_surface->rect.y;
    td_u32 src1_bpp = vau_get_bpp_from_colorfmt(ctx->src1->color_fmt);

    modified_src[0] = *ctx->src1;
    modified_src[0].phy_addr = ctx->src1->phy_addr + dy_src1 * ctx->src1->stride + dx_src1 * src1_bpp / BITS_PER_BYTE;
    modified_src[0].width = ctx->cur_block_w;
    modified_src[0].height = ctx->cur_block_h;
    modified_src[0].rect = ctx->block_rect;
}

/* Update dst surface address for block */
STATIC td_void vau_update_dst_for_block(const vau_split_ctx *ctx, ext_vau_surface *block_dst)
{
    td_u32 bpp = vau_get_bpp_from_colorfmt(ctx->dst_surface->color_fmt);
    *block_dst = *ctx->dst_surface;
    block_dst->phy_addr = ctx->dst_surface->phy_addr + ctx->dy * ctx->dst_surface->stride +
                          ctx->dx * bpp / BITS_PER_BYTE;
    block_dst->width = ctx->cur_block_w;
    block_dst->height = ctx->cur_block_h;
    block_dst->rect = ctx->block_rect;
}

STATIC td_s32 vau_draw_hardware_with_split(td_handle handle, const ext_vau_surface_list *surface_list,
    ext_vau_draw_attr *draw_attr, td_bool is_stroke)
{
    ext_vau_surface *dst_surface = surface_list->dst_surface;
    ext_vau_surface *src1 = (surface_list->src_surface_cnt == 1) ? TD_NULL : &surface_list->src_surface[0];
    ext_vau_surface *src2 = (surface_list->src_surface_cnt == 1) ?
                             &surface_list->src_surface[0] : &surface_list->src_surface[1];
    ext_vau_matrix identity = {0};
    vau_matrix_identity(&identity);
    ext_vau_matrix original_matrix = (src2->matrix == TD_NULL) ? identity : *(src2->matrix);

    /* Initialize context */
    vau_split_ctx ctx = {0};
    ctx.dst_surface = dst_surface;
    ctx.src1 = src1;
    ctx.src2 = src2;

    /* Calculate block size */
    vau_calc_split_block_size(dst_surface, &ctx);

    /* Iterate through each block */
    for (td_u32 y = 0; y < ctx.block_num_y; y++) {
        for (td_u32 x = 0; x < ctx.block_num_x; x++) {
            /* Calculate block dimensions */
            vau_calc_block_rect(x, y, &ctx);

            /* Prepare matrix for this block */
            ext_vau_matrix block_matrix = {0};
            vau_matrix_identity(&block_matrix);
            vau_matrix_trans(&block_matrix, -(td_float)ctx.dx, -(td_float)ctx.dy);
            vau_matrix_mult(&block_matrix, &original_matrix);

            /* Prepare surface list for this block */
            ext_vau_surface_list block_surface_list = *surface_list;
            ext_vau_surface modified_src[2] = {0};

            /* Update src2 matrix pointer */
            modified_src[1] = *src2;
            modified_src[1].matrix = &block_matrix;

            /* Update src1 if exists */
            vau_update_src1_for_block(&ctx, modified_src);
            block_surface_list.src_surface = (src1 != TD_NULL) ? modified_src : &modified_src[1];

            /* Update dst surface */
            ext_vau_surface block_dst = {0};
            vau_update_dst_for_block(&ctx, &block_dst);
            block_surface_list.dst_surface = &block_dst;

            /* Debug print if enabled */
            if (vau_dfx_check_mask(VAU_DFX_MASK_SURFACE) == TD_TRUE) {
                gpu_print("*************************draw block(%dx%d) surface: x %d y %d*************************\n",
                    ctx.block_num_x, ctx.block_num_y, x, y);
                vau_debug_print_surface_list(&block_surface_list);
            }

            /* Draw this block */
            td_s32 ret = vau_draw_hardware_node_cfg(handle, &block_surface_list, draw_attr, is_stroke);
            if (ret != EXT_SUCCESS) {
                gpu_err("vau_draw_hardware_node_cfg failed for block[%u,%u], ret=0x%x", x, y, ret);
                return ret;
            }
        }
    }

    return EXT_SUCCESS;
}

STATIC td_s32 vau_draw_hardware(td_handle handle, const ext_vau_surface_list *surface_list,
                                ext_vau_draw_attr *draw_attr, td_bool is_stroke)
{
    ext_vau_surface *dst_surface = surface_list->dst_surface;

    /* Check if split is needed: width > 640 or height > 600 */
    if ((dst_surface->width > VAU_SPLIT_MAX_BLOCK_WIDTH) ||
        (dst_surface->height > VAU_SPLIT_MAX_BLOCK_HEIGHT)) {
        return vau_draw_hardware_with_split(handle, surface_list, draw_attr, is_stroke);
    }

    return vau_draw_hardware_node_cfg(handle, surface_list, draw_attr, is_stroke);
}

STATIC td_s32 vau_draw_fill_path(td_handle handle, const ext_vau_surface_list *surface_list,
                                 ext_vau_draw_attr *draw_attr)
{
    ext_vau_fill_attr *fill_attr = draw_attr->paint->fill_attr;
    if (fill_attr == TD_NULL) {
        gpu_err("fill_attr is null!");
        return DRV_ERR_VAU_INVALID_PARA;
    }

    return vau_draw_hardware(handle, surface_list, draw_attr, TD_FALSE);
}

STATIC td_s32 vau_draw_stroke_path(td_handle handle, const ext_vau_surface_list *surface_list,
    ext_vau_draw_attr *draw_attr)
{
    td_s32 ret;
    ext_vau_path dash_path = { 0 };
    ext_vau_stroke_attr *stroke_attr = draw_attr->paint->stroke_attr;
    if (stroke_attr == TD_NULL) {
        gpu_err("stroke_attr is null!");
        return DRV_ERR_VAU_INVALID_PARA;
    }

    if ((stroke_attr->cap >= EXT_VAU_STROKE_LINE_CAP_MAX) ||
        (stroke_attr->join >= EXT_VAU_STROKE_LINE_JOIN_MAX)) {
        gpu_err("invalid cap/join mode, cap[%d], join[%d]!", stroke_attr->cap, stroke_attr->join);
        return DRV_ERR_VAU_INVALID_PARA;
    }

    if (stroke_attr->dash.enable) {
        ret = vau_create_stroke_dash_path(draw_attr->path, stroke_attr, &dash_path);
        if (ret != EXT_SUCCESS) {
            gpu_err("call vau_create_stroke_dash_path failed!");
            return ret;
        }
        draw_attr->path = &dash_path;
    }
#ifdef GPU_SUPPORT_RM_STROKE
    ext_vau_path stroke_path = { 0 };
    td_u32 mode = (draw_attr->paint->render_cfg.render_quality == EXT_VAU_RENDER_QUALITY_BETTER) ?
                  (td_u32)EXT_BEZIER_MODE_LINE_AVG : (td_u32)EXT_BEZIER_MODE_BEZIER_4;

    ret = vau_create_stroke_to_fill_path(draw_attr->path, stroke_attr, &stroke_path, mode);
    if (ret != EXT_SUCCESS) {
        gpu_err("call vau_create_stroke_to_fill_path failed!");
        if (stroke_attr->dash.enable) {
            vau_destroy_stroke_dash_path(&dash_path);
        }
        return ret;
    }
    draw_attr->path = &stroke_path;
#endif
    ret = vau_draw_hardware(handle, surface_list, draw_attr, TD_TRUE);
    if (ret != EXT_SUCCESS) {
        gpu_err("vau_draw_hardware failed!");
    }
    draw_attr->path = TD_NULL;
#ifdef GPU_SUPPORT_RM_STROKE
    vau_destroy_stroke_to_fill_path(&stroke_path);
#endif

    if (stroke_attr->dash.enable) {
        vau_destroy_stroke_dash_path(&dash_path);
    }

    return ret;
}

STATIC td_bool vau_draw_has_arc(const ext_vau_path *path, td_u32 paint_mode)
{
    if ((paint_mode & EXT_VAU_PAINT_MODE_STROKE) != EXT_VAU_PAINT_MODE_STROKE) {
        return TD_FALSE; /* current only support stroke ecllipse arc */
    }

    for (td_u32 i = 0; i < path->cmd_num; i++) {
        if (path->cmds[i] >= VAU_PATH_SCCWARC_TO_ABS && path->cmds[i] <= VAU_PATH_LCWARC_TO_REL) {
            return TD_TRUE;
        }
    }

    return TD_FALSE;
}

STATIC td_s32 vau_draw_point(const ext_vau_draw_attr *draw_attr, const ext_vau_surface *dst)
{
    td_u32 paint_mode = (td_u32)draw_attr->paint->paint_mode;
    if ((paint_mode & EXT_VAU_PAINT_MODE_FILL) != EXT_VAU_PAINT_MODE_FILL) {
        gpu_err("only_support paint_mode_fill");
        return EXT_FAILURE;
    }
    if ((draw_attr->paint->fill_attr == TD_NULL) ||
        (draw_attr->paint->fill_attr->paint_attr.paint_type > EXT_VAU_PAINT_TYPE_COLOR_ALPHA_FF)) {
        gpu_err("only support paint type solid color");
        return EXT_FAILURE;
    }
    td_u32 color = draw_attr->paint->fill_attr->paint_attr.solid.color; /* get filled color */

    td_u32 bpp;
    if (dst->color_fmt == EXT_GFX_FMT_ARGB8888) {
        bpp = 32; /* 32 bits */
    } else if (dst->color_fmt == EXT_GFX_FMT_RGB888) {
        bpp = 24; /* 24 bits */
    } else if (dst->color_fmt == EXT_GFX_FMT_RGB565) {
        bpp = 16; /* 16 bits */
    } else {
        gpu_err("not support fmt[%d]", dst->color_fmt);
        return EXT_FAILURE;
    }

    ext_vau_path *path = draw_attr->path;
    /* 8: bit to byte */
    td_u32 addr = dst->phy_addr + (td_u32)path->datas[1] * dst->stride + (td_u32)path->datas[0] * bpp / 8;
    td_uchar *cur_addr = (td_uchar *)(uintptr_t)addr;

    if (dst->color_fmt == EXT_GFX_FMT_ARGB8888) {
        *(td_u32 *)cur_addr = color;
    } else if (dst->color_fmt == EXT_GFX_FMT_RGB888) {
        *(cur_addr + 0) = (td_u8)color; /* b */
        *(cur_addr + 1) = (td_u8)(color >> 8); /* g, 8:offset */
        *(cur_addr + 2) = (td_u8)(color >> 16); /* r, 16:offset, 2:offset */
    } else { /* RGB565 */
        td_u8 r = (td_u8)(color >> 16); /* 16: offset */
        td_u8 g = (td_u8)(color >> 8); /* 8: offset */
        td_u8 b = (td_u8)(color);

        td_u16 color_b = (b >> 0x3) & 0x001F;
        td_u16 color_g = ((g >> 0x2) << 0x5) & 0x07E0;
        td_u16 color_r = ((r >> 0x3) << 0xb) & 0xF800;
        td_u16 color_565 = color_r | color_g | color_b;
        *(td_u16 *)cur_addr = color_565;
    }

    return EXT_SUCCESS;
}

STATIC td_s32 vau_draw_check(ext_vau_draw_attr *draw_attr, const ext_vau_surface_list *surface_list)
{
    if ((draw_attr == TD_NULL) || (draw_attr->path == TD_NULL) || (draw_attr->paint == TD_NULL)) {
        gpu_err("draw info is null!");
        return DRV_ERR_VAU_INVALID_PARA;
    }

    if (surface_list->src_surface_cnt == 2) { /* 2 is surface count */
        if (surface_list->src_surface[0].compress_mode != surface_list->dst_surface->compress_mode) {
            gpu_err("src0 compress mode[%d] is not equal dst compress mode[%d]!",
                    surface_list->src_surface[0].compress_mode, surface_list->dst_surface->compress_mode);
            return DRV_ERR_VAU_INVALID_PARA;
        }
    }

    ext_vau_surface *dst = surface_list->dst_surface;
    if ((dst->rect.x < 0) || (dst->rect.x >= (td_s32)dst->width) ||
        (dst->rect.y < 0) || (dst->rect.y >= (td_s32)dst->height) ||
        (dst->rect.width == 0) ||  (dst->rect.width > dst->width) ||
        (dst->rect.height == 0) || (dst->rect.height > dst->height)) {
        gpu_err("invalid dst rect, rect[%d,%d,%u,%u], surface_wh[%u,%u]!",
                dst->rect.x, dst->rect.y, dst->rect.width, dst->rect.height, dst->width, dst->height);
        return DRV_ERR_VAU_INVALID_PARA;
    }

    return EXT_SUCCESS;
}

td_s32 vau_draw_directly(td_handle handle, const ext_vau_surface_list *surface_list)
{
    td_s32 ret = EXT_SUCCESS;
    ext_vau_path arc_path = { 0 };

    ext_vau_draw_attr *draw_attr = (surface_list->src_surface_cnt == 1) ?
                                    surface_list->src_surface[0].draw : surface_list->src_surface[1].draw;
    ret = vau_draw_check(draw_attr, surface_list);
    if (ret != EXT_SUCCESS) {
        return DRV_ERR_VAU_INVALID_PARA;
    }

    if (draw_attr->path->cmd_num == 0 || draw_attr->path->data_num == 0) {
        gpu_err("cmd/data num is zero, no need draw path!");
        return EXT_SUCCESS;
    }

    if ((draw_attr->path->cmd_num == 1) && (draw_attr->path->data_num == 2) && // 2:data num
        (draw_attr->path->cmds[0] == VAU_PATH_MOVE_TO_ABS)) {
        return vau_draw_point(draw_attr, surface_list->dst_surface);
    }

    ext_vau_draw_attr tmp_draw_attr = *draw_attr;
    td_u32 paint_mode = (td_u32)tmp_draw_attr.paint->paint_mode;
    td_bool has_arc = vau_draw_has_arc(tmp_draw_attr.path, paint_mode);
    if (has_arc) {
        ret = vau_draw_create_arc_fit_path(tmp_draw_attr.path, &arc_path);
        if (ret != EXT_SUCCESS) {
            gpu_err("parse arc path failed:0x%x!", ret);
            return ret;
        }
        tmp_draw_attr.path = &arc_path;
    }

    if ((paint_mode & EXT_VAU_PAINT_MODE_FILL) == EXT_VAU_PAINT_MODE_FILL) {
        ret = vau_draw_fill_path(handle, surface_list, &tmp_draw_attr);
        if (ret != EXT_SUCCESS) {
            gpu_err("call vau_draw_fill_path failed!");
            goto out;
        }
    }

    if ((paint_mode & EXT_VAU_PAINT_MODE_STROKE) == EXT_VAU_PAINT_MODE_STROKE) {
        ret = vau_draw_stroke_path(handle, surface_list, &tmp_draw_attr);
        if (ret != EXT_SUCCESS) {
            gpu_err("call vau_draw_stroke_path failed!");
            goto out;
        }
    }

out:
    if (has_arc) {
        vau_draw_destroy_arc_fit_path(&arc_path);
    }

    return ret;
}

#ifdef GPU_SUBNODE_SUPPORT
td_void vau_draw_node_init(vau_hardware_node *hw_node)
{
    /* node default value from logic reg manager */
    hw_node->local_lighting_cfg.u32    = 0x00000002;
    hw_node->gauss_blur_ctrl.u32       = 0x80000000;

    hw_node->vau_src1_t2r_ctrl.u32     = 0x80001000;
    hw_node->vau_src2_t2r_ctrl.u32     = 0x80001000;
    hw_node->vau_des_r2t_ctrl.u32      = 0x80001000;
    hw_node->vau_cbm_ctrl.u32          = 0x02000000;

    hw_node->vau_node_cfg.u32          = 0x00000004;
    hw_node->vau_hdr_sys_ctrl.u32      = 0xFFFF1001;

    hw_node->mix.draw.vau_draw_ctrl.u32            = 0x00010000;
    hw_node->mix.draw.vau_tess_line_num_max.u32    = 0x00000100;
    hw_node->mix.draw.vau_draw_threshold_quad.u32  = 0x3F983700;
    hw_node->mix.draw.vau_draw_threshold_cubic.u32 = 0x4003D300;

    /* set node mode */
    hw_node->vau_node_cfg.bits.node_mode = (td_u32)VAU_MIX_NODE_DRAW;
}

td_s32 vau_draw_get_subnode(td_u32 node_addr, td_bool is_full_node, const ext_vau_surface_list *surface_list)
{
    vau_hardware_node *hw_node = (vau_hardware_node *)(uintptr_t)node_addr;
    ext_vau_surface *src1 = (surface_list->src_surface_cnt == 1) ? TD_NULL : &surface_list->src_surface[0];
    ext_vau_surface *src2 = (surface_list->src_surface_cnt == 1) ?
                             &surface_list->src_surface[0] : &surface_list->src_surface[1];

    ext_vau_matrix identity;
    vau_matrix_identity(&identity);
    ext_vau_matrix *m = (src2->matrix == TD_NULL) ? &identity : src2->matrix;

    ext_vau_draw_attr *draw_attr = src2->draw;
    if ((draw_attr == TD_NULL) || (draw_attr->path == TD_NULL) || (draw_attr->paint == TD_NULL)) {
        gpu_err("draw info is null!");
        return DRV_ERR_VAU_INVALID_PARA;
    }

    if (draw_attr->path->cmd_num == 0 || draw_attr->path->data_num == 0) {
        gpu_err("cmd/data num is zero, no need draw path!");
        return EXT_SUCCESS;
    }

    if (is_full_node) {
        vau_draw_node_init(hw_node);
    }

    td_s32 ret = vau_draw_make_path(draw_attr->path, hw_node);
    if (ret != EXT_SUCCESS) {
        gpu_err("vau_draw_make_path failed");
        return ret;
    }

    td_u32 type;
    ext_vau_paint_attr *paint_attr;
    if (draw_attr->paint->paint_mode == EXT_VAU_PAINT_MODE_STROKE) {
        type = EXT_VAU_NON_ZERO_FILL;
        paint_attr = &draw_attr->paint->stroke_attr->paint_attr;
        vau_draw_set_hw_stroke(draw_attr->paint->stroke_attr, hw_node, m);
    } else {
        type = draw_attr->paint->fill_attr->type;
        paint_attr = &draw_attr->paint->fill_attr->paint_attr;
    }

    ret = vau_draw_make_paint(paint_attr, type, draw_attr->paint->render_cfg.render_quality, m, hw_node);
    if (ret != EXT_SUCCESS) {
        gpu_err("vau_draw_make_paint failed");
        return ret;
    }
    vau_draw_make_matrix(m, hw_node);

    if (is_full_node) {
        vau_draw_make_src1(src1, hw_node);
        vau_draw_make_dst(surface_list->dst_surface, hw_node);
        vau_draw_make_cbm(surface_list, hw_node);
        vau_node_set_finish_isr(hw_node, TD_TRUE);
    }

    return EXT_SUCCESS;
}
#endif