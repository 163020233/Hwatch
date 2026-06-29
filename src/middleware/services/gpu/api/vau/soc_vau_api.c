
/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: the function interface define in vector accelerate unit api source file
 * Create: 2021-2-20
 */

#include "soc_vau_api.h"
#include <pthread.h>
#include <math.h>
#include "soc_osal.h"
#include "vau_debug.h"
#include "drv_vau_ioctl.h"
#include "vau_compress.h"
#ifdef GPU_SDF_SUPPORT
#include "vau_sdf.h"
#endif
#ifdef CONFIG_VAU_DRAW_DIRECTLY
#include "vau_draw.h"
#endif
#include "vau_rm_api.h"
#include "vau_matrix.h"
#include "vau_blur.h"
#include "rm_math.h"
#include "vau_mem.h"
#include "vau_dfx.h"

#define VAU_SRC_SURFACE_CNT_MAX  20
#define VAU_GRADIENT_RATIO       128

#define VAU_SURFACE_WIDTH_MIN    1
#define VAU_SURFACE_HEIGHT_MIN   1

#ifdef GPU_HIGH_RESOLUTION
#define VAU_SURFACE_WIDTH_MAX    4096
#define VAU_SURFACE_HEIGHT_MAX   4096
#define VAU_SURFACE_STRIDE_MAX   32768
#else
#define VAU_SURFACE_WIDTH_MAX    640
#define VAU_SURFACE_HEIGHT_MAX   1024
#define VAU_SURFACE_STRIDE_MAX   8192
#endif

#define VAU_SURFACE_STRIDE_MIN   8


STATIC td_s32 g_vau_dev_fd = -1;
STATIC td_u32 g_vau_dev_init_cnt = 0;
STATIC pthread_mutex_t g_vau_dev_mutex = PTHREAD_MUTEX_INITIALIZER;

STATIC td_void vau_dev_lock(td_void)
{
    (td_void)pthread_mutex_lock(&g_vau_dev_mutex);
}

STATIC td_void vau_dev_unlock(td_void)
{
    (td_void)pthread_mutex_unlock(&g_vau_dev_mutex);
}

#define check_vau_dev_fd_return()              \
    do {                                       \
        vau_dev_lock();                        \
        if (g_vau_dev_fd < 0) {                \
            gpu_err("VAU is not inited!");     \
            vau_dev_unlock();                  \
            return EXT_FAILURE;                \
        }                                      \
        vau_dev_unlock();                      \
    } while (0)

#define check_vau_handle_return(handle)                      \
    do {                                                     \
        if ((handle) <= 0) {                                 \
            gpu_err("invalid handle! hanlde:%u", handle);    \
            return EXT_ERR_GFX_INVALID_PARAM;                \
        }                                                    \
    } while (0)


typedef struct {
    ext_vau_blend_cmd cmd_api;
    drv_vau_blend_cmd cmd_drv;
} vau_blend_cmd_ref_table;

typedef struct {
    ext_vau_rop_mode mode_api;
    drv_vau_rop_mode mode_drv;
} vau_rop_mode_ref_table;

typedef struct {
    ext_gfx_fmt fmt_api;
    drv_gfx_fmt fmt_drv;
} vau_color_fmt_ref_table;

STATIC td_s32 vau_api_check_surface_resolution(const ext_vau_surface *surface)
{
    if ((surface->rect.x > VAU_SURFACE_WIDTH_MAX) || (surface->rect.y > VAU_SURFACE_HEIGHT_MAX) ||
        (surface->rect.x + surface->rect.width > VAU_SURFACE_WIDTH_MAX) ||
        (surface->rect.y + surface->rect.height > VAU_SURFACE_HEIGHT_MAX) ||
        (surface->rect.width < VAU_SURFACE_WIDTH_MIN) || (surface->rect.width > VAU_SURFACE_WIDTH_MAX) ||
        (surface->rect.height < VAU_SURFACE_HEIGHT_MIN) || (surface->rect.height > VAU_SURFACE_HEIGHT_MAX) ||
        (surface->stride < VAU_SURFACE_STRIDE_MIN) || (surface->stride > VAU_SURFACE_STRIDE_MAX)) {
        gpu_err("input param is invalid, width[%u], height[%u], stride[%u], rect[%d %d %u %u]!",
                surface->width, surface->height, surface->stride,
                surface->rect.x, surface->rect.y, surface->rect.width, surface->rect.height);
        return EXT_ERR_GFX_INVALID_PARAM;
    }

    return EXT_SUCCESS;
}

STATIC td_s32 vau_api_check_surface_rect(const ext_vau_surface *surface)
{
    if ((surface->rect.width > surface->width) || (surface->rect.height > surface->height)) {
        gpu_err("input param is invalid, rect[%d %d %u %u] > surface[%u %u]!",
                surface->rect.x, surface->rect.y, surface->rect.width, surface->rect.height,
                surface->width, surface->height);
        return EXT_ERR_GFX_INVALID_PARAM;
    }

    return EXT_SUCCESS;
}

STATIC td_s32 vau_api_check_surface(const ext_vau_surface *surface, td_bool skip_res_chk)
{
#ifdef GPU_SDF_SUPPORT
    if (surface->sdf_attr != TD_NULL) {
        return EXT_SUCCESS;
    }
#endif

    if (surface->draw != TD_NULL) {
        return EXT_SUCCESS;
    }

    if (surface->compress_mode >= EXT_GFX_COMPRESS_MAX) {
        gpu_err("invalid compress mode\n");
        return EXT_ERR_GFX_INVALID_PARAM;
    }

    if (surface->color_fmt >= EXT_GFX_FMT_MAX) {
        gpu_err("invalid color fmt\n");
        return EXT_ERR_GFX_INVALID_PARAM;
    }

    if (surface->color_fmt >= EXT_GFX_FMT_YUV400SP && surface->color_fmt <= EXT_GFX_FMT_YUV444SP) {
        return EXT_SUCCESS;
    }

    if (!skip_res_chk) {
        td_s32 ret = vau_api_check_surface_resolution(surface);
        if (ret != EXT_SUCCESS) {
            return ret;
        }
    }

    td_s32 ret = vau_api_check_surface_rect(surface);
    if (ret != EXT_SUCCESS) {
        return ret;
    }

    if ((surface->is_color_surface != TD_TRUE) && (surface->phy_addr == 0)) {
        gpu_err("input phy_addr is null!");
        return EXT_ERR_GFX_INVALID_PARAM;
    }

    return EXT_SUCCESS;
}

STATIC td_s32 vau_api_check_opt(ext_vau_blit_opt *opt)
{
    if (opt->lighting_opt.enable == TD_TRUE) {
        ext_vau_lighting_sw *attr = &(opt->lighting_opt.attr);
        if (attr->radius == 0) {
            gpu_err("error! radius is 0\n");
            return EXT_FAILURE;
        }
        if (attr->start_end_angle_rad == attr->start_angle_rad) {
            gpu_err("error! start_end_angle_rad is equal to start_angle_rad\n");
            return EXT_FAILURE;
        }
        if (attr->end_angle_rad == attr->end_start_angle_rad) {
            gpu_err("error! end_start_angle_rad is equal to end_angle_rad\n");
            return EXT_FAILURE;
        }
    }
    return EXT_SUCCESS;
}

STATIC td_s32 vau_check_compose_attr(const ext_vau_surface_list *surface_list, td_bool is_compose)
{
    if (is_compose == TD_FALSE) {
        return EXT_SUCCESS;
    }
    if (surface_list->dst_surface->compress_mode != EXT_GFX_COMPRESS_NONE) {
        if ((surface_list->opt == TD_NULL) ||
            (surface_list->opt != TD_NULL && surface_list->opt->blend_opt.blend_enable == TD_FALSE)) {
            gpu_err("vau check surface failed! fb cmp compose must open blend\n");
            return EXT_FAILURE;
        }
    }
    return EXT_SUCCESS;
}

td_bool vau_check_is_vector_draw(const ext_vau_surface_list *surface_list)
{
    ext_vau_surface *src2 = TD_NULL;

    src2 = (surface_list->src_surface_cnt == 2) ? /* 2: double surface */
            &surface_list->src_surface[1] : &surface_list->src_surface[0];
    if ((src2->draw == TD_NULL) || (src2->draw->path == TD_NULL) || (src2->draw->paint == TD_NULL)) {
        return TD_FALSE;
    }
    return TD_TRUE;
}

STATIC td_s32 vau_api_check_surface_list(const ext_vau_surface_list * surface_list, td_bool is_compose)
{
    td_s32 ret;
    td_u32 max_surface_cnt;

    if (is_compose) {
        max_surface_cnt = VAU_SRC_SURFACE_CNT_MAX;
    } else {
        max_surface_cnt = 2; /* render only support 2 src */
    }

    if (surface_list == TD_NULL) {
        gpu_err("surface_list is null!");
        return EXT_FAILURE;
    }

    if ((surface_list->src_surface_cnt == 0) || (surface_list->src_surface_cnt > max_surface_cnt)) {
        gpu_err("surface_cnt[%u] reach max[%u]!", surface_list->src_surface_cnt, max_surface_cnt);
        return EXT_FAILURE;
    }

    if ((surface_list->src_surface == TD_NULL) || (surface_list->dst_surface == TD_NULL)) {
        gpu_err("null pointer");
        return EXT_FAILURE;
    }

    td_bool is_vector_draw = vau_check_is_vector_draw(surface_list);
    for (td_u32 i = 0; i < surface_list->src_surface_cnt; i++) {
        td_bool skip_res_chk = (is_vector_draw && (surface_list->src_surface_cnt == 2) && (i == 0)) ? TD_TRUE : TD_FALSE;
        ret = vau_api_check_surface(&surface_list->src_surface[i], skip_res_chk);
        if (ret != EXT_SUCCESS) {
            gpu_err("vau check src surface[%u] failed!", i);
            return ret;
        }
    }
    if (surface_list->opt != TD_NULL) {
        ret = vau_api_check_opt(surface_list->opt);
        if (ret != EXT_SUCCESS) {
            gpu_err("vau check surface opt failed!");
            return ret;
        }
    }
    td_bool skip_res_chk = is_vector_draw ? TD_TRUE : TD_FALSE;
    ret = vau_api_check_surface(surface_list->dst_surface, skip_res_chk);
    if (ret != EXT_SUCCESS) {
        gpu_err("vau check dst surface failed!");
        return ret;
    }

    ret = vau_check_compose_attr(surface_list, is_compose);
    if (ret != EXT_SUCCESS) {
        gpu_err("vau check compose param failed!");
        return ret;
    }

    return EXT_SUCCESS;
}

#ifdef GPU_SDF_SUPPORT
STATIC td_bool vau_check_is_sdf(const ext_vau_surface_list *surface_list)
{
    ext_vau_surface *src2 = TD_NULL;

    src2 = (surface_list->src_surface_cnt == 2) ? /* 2: double surface */
            &surface_list->src_surface[1] : &surface_list->src_surface[0];
    if (src2->sdf_attr != TD_NULL) {
        return TD_TRUE;
    }
    return TD_FALSE;
}
#endif

STATIC td_void make_default_hardware_sampling(const ext_vau_render_cfg *cfg, drv_vau_render_cfg *drv_cfg)
{
    td_u8 sample_num;

    switch (cfg->render_quality) {
        case EXT_VAU_RENDER_QUALITY_NONE_AA:
            drv_cfg->sample_mode = DRV_VAU_NONE_MSAA;
            /* set in hardware */
            return;
        case EXT_VAU_RENDER_QUALITY_FASTER:
            drv_cfg->sample_mode = DRV_VAU_4X_MSAA;
            sample_num = 4; /* 4x */
            drv_cfg->sample_attr.samples[0].x = 1; /* 1: default 4x sample pos 0 */
            drv_cfg->sample_attr.samples[1].x = 3; /* 3: default 4x sample pos 1 */
            drv_cfg->sample_attr.samples[2].x = 0; /* 0: default 4x sample pos 2 */
            drv_cfg->sample_attr.samples[3].x = 2; /* 2: default 4x sample pos 3 */
            break;
        case EXT_VAU_RENDER_QUALITY_BETTER:
            drv_cfg->sample_mode = DRV_VAU_8X_MSAA;
            sample_num = 8; /* 8x */
            drv_cfg->sample_attr.samples[0].x = 3; /* 3: default 8x sample pos 0 */
            drv_cfg->sample_attr.samples[1].x = 7; /* 7: default 8x sample pos 1 */
            drv_cfg->sample_attr.samples[2].x = 0; /* 0: default 8x sample pos 2 */
            drv_cfg->sample_attr.samples[3].x = 2; /* 2: default 8x sample pos 3 */
            drv_cfg->sample_attr.samples[4].x = 5; /* 5: default 8x sample pos 4 */
            drv_cfg->sample_attr.samples[5].x = 1; /* 1: default 8x sample pos 5 */
            drv_cfg->sample_attr.samples[6].x = 6; /* 6: default 8x sample pos 6 */
            drv_cfg->sample_attr.samples[7].x = 4; /* 4: default 8x sample pos 7 */
            break;
        default:
            drv_cfg->sample_mode = DRV_VAU_NONE_MSAA;
            return;
    }
    for (td_s32 i = 0; i < sample_num; i++) {
        /* 0.5 center of one pixel */
        drv_cfg->sample_attr.samples[i].x = (drv_cfg->sample_attr.samples[i].x + 0.5) / sample_num;
        /* 0.5 center of one pixel */
        drv_cfg->sample_attr.samples[i].y = (i + 0.5) / sample_num;
        drv_cfg->sample_attr.samples[i].weight = 0xFF / sample_num;
    }
    drv_cfg->sample_attr.samples[0].weight = 0xFF - (0xFF / sample_num) * (sample_num - 1);
    drv_cfg->sample_attr.sample_radius = 0.5f; /* 0.5 center of one pixel */

    return;
}

STATIC td_void make_default_hardware_bezier_err(drv_vau_render_cfg *drv_render_cfg)
{
    drv_render_cfg->bezier_err.err_quad = 0.125; /* 0.125 default err quad */
    drv_render_cfg->bezier_err.err_cubic = 0.125; /* 0.125 default err quad */
}

STATIC td_void make_default_hardware_render_cfg(const ext_vau_render_cfg *render_cfg,
                                                drv_vau_render_cfg *drv_render_cfg)
{
    make_default_hardware_bezier_err(drv_render_cfg);
    make_default_hardware_sampling(render_cfg, drv_render_cfg);
}

STATIC td_void vau_calc_auxiliary_value(const ext_vau_paint_attr *paint_attr,
                                        drv_auxiliary_calc_value *auxiliary_value)
{
    if (paint_attr->paint_type == EXT_VAU_PAINT_TYPE_LINEAR_GRADIENT) {
        td_s32 dx, dy, usq;
        dx = paint_attr->gradient.linear.end_point.x -
            paint_attr->gradient.linear.start_point.x;
        dy = paint_attr->gradient.linear.end_point.y -
            paint_attr->gradient.linear.start_point.y;
        usq = dx * dx + dy * dy;
        if ((dx == 0) && (dy == 0)) {
            auxiliary_value->linear.mode = LINEAR_POINT;
            auxiliary_value->linear.const_x = 0;
            auxiliary_value->linear.const_y = 0;
        } else if ((dx == 0) && (dy != 0)) { /* vertical */
            auxiliary_value->linear.mode = LINEAR_VERTICAL;
            auxiliary_value->linear.const_x = 0;
            auxiliary_value->linear.const_y = 128 * 256 * (td_float)dy / usq; /* 128 * 256: gradient ratio */
        } else if ((dx != 0) && (dy == 0)) { /* horizontal */
            auxiliary_value->linear.mode = LINEAR_HORIZONTAL;
            auxiliary_value->linear.const_x = 128 * 256 * (td_float)dx / usq; /* 128 * 256: gradient ratio */
            auxiliary_value->linear.const_y = 0;
        } else { // slash
            auxiliary_value->linear.mode = LINEAR_SLASH;
            auxiliary_value->linear.const_x = 128 * 256 * (td_float)dx / usq; /* 128 * 256: gradient ratio */
            auxiliary_value->linear.const_y = 128 * 256 * (td_float)dy / usq; /* 128 * 256: gradient ratio */
        }
    } else if (paint_attr->paint_type == EXT_VAU_PAINT_TYPE_RADIAL_GRADIENT) {
         /* 128 * 256: gradient ratio */
        auxiliary_value->radial.const_r = 128 * 256 / (td_float)paint_attr->gradient.radial.radius;
    } else if (paint_attr->paint_type == EXT_VAU_PAINT_TYPE_SWEEP_GRADIENT) {
        /* 180 degree */
        auxiliary_value->sweep.const_pi = 180.0f / M_PI;
         /* 128 * 256: gradient ratio; 360 degree */
        auxiliary_value->sweep.const_ratio = 128.0f * 256 / 360;
    }
}

STATIC td_void vau_trans_vector_fillattr_type(const ext_vau_fill_attr *api, drv_vau_fill_attr *drv)
{
    switch (api->type) {
        case EXT_VAU_EVEN_ODD_FILL:
            drv->type = DRV_VAU_EVEN_ODD_FILL;
            break;
        case EXT_VAU_NON_ZERO_FILL:
            drv->type = DRV_VAU_NON_ZERO_FILL;
            break;
        default:
            drv->type = DRV_VAU_EVEN_ODD_FILL;
            gpu_err("invalid fill type! %u\n", api->type);
            break;
    }
}

STATIC td_void vau_trans_vector_fillattr_painttype(const ext_vau_paint_attr *api, drv_vau_paint_attr *drv)
{
    switch (api->paint_type) {
        case EXT_VAU_PAINT_TYPE_COLOR:
            drv->paint_type = DRV_VAU_PAINT_TYPE_COLOR;
            break;
        case EXT_VAU_PAINT_TYPE_COLOR_ALPHA_FF:
            drv->paint_type = DRV_VAU_PAINT_TYPE_COLOR_ALPHA_FF;
            break;
        case EXT_VAU_PAINT_TYPE_LINEAR_GRADIENT:
            drv->paint_type = DRV_VAU_PAINT_TYPE_LINEAR_GRADIENT;
            break;
        case EXT_VAU_PAINT_TYPE_RADIAL_GRADIENT:
            drv->paint_type = DRV_VAU_PAINT_TYPE_RADIAL_GRADIENT;
            break;
        case EXT_VAU_PAINT_TYPE_SWEEP_GRADIENT:
            drv->paint_type = DRV_VAU_PAINT_TYPE_SWEEP_GRADIENT;
            break;
        case EXT_VAU_PAINT_TYPE_PATTERN:
            drv->paint_type = DRV_VAU_PAINT_TYPE_PATTERN;
            break;
        default:
            drv->paint_type = DRV_VAU_PAINT_TYPE_COLOR;
            gpu_err("invalid paint type! %u\n", (td_u32)api->paint_type);
            break;
    }
}

STATIC td_void vau_trans_vector_gradient_solid_attr(const ext_vau_solid_attr *api, drv_vau_solid_attr *drv)
{
    drv->color = api->color;
}

STATIC td_void vau_trans_vector_gradient_linear_attr(const ext_vau_linear_gradient_attr *api,
                                                     drv_vau_linear_gradient_attr *drv)
{
    drv->start_point.x = api->start_point.x;
    drv->start_point.y = api->start_point.y;
    drv->end_point.x   = api->end_point.x;
    drv->end_point.y   = api->end_point.y;
}

STATIC td_void vau_trans_vector_gradient_radial_attr(const ext_vau_radial_gradient_attr *api,
                                                     drv_vau_radial_gradient_attr *drv)
{
    drv->center.x = api->center.x;
    drv->center.y = api->center.y;
    drv->radius   = api->radius;
}

STATIC td_void vau_trans_vector_gradient_sweep_attr(const ext_vau_sweep_gradient_attr *api,
                                                    drv_vau_sweep_gradient_attr *drv)
{
    drv->center.x    = api->center.x;
    drv->center.y    = api->center.y;
    drv->start_angle = api->start_angle;
}

STATIC td_void vau_trans_vector_fillattr_api2drv(const ext_vau_fill_attr *api, drv_vau_fill_attr *drv)
{
    vau_trans_vector_fillattr_type(api, drv);
    vau_trans_vector_fillattr_painttype(&api->paint_attr, &drv->paint_attr);

    if ((api->paint_attr.paint_type == EXT_VAU_PAINT_TYPE_COLOR) ||
        (api->paint_attr.paint_type == EXT_VAU_PAINT_TYPE_COLOR_ALPHA_FF)) {
        vau_trans_vector_gradient_solid_attr(&(api->paint_attr.solid), &(drv->paint_attr.solid));
        return;
    } else if (api->paint_attr.paint_type == EXT_VAU_PAINT_TYPE_LINEAR_GRADIENT) {
        vau_trans_vector_gradient_linear_attr(&(api->paint_attr.gradient.linear),
                                              &(drv->paint_attr.gradient.linear));
    } else if (api->paint_attr.paint_type == EXT_VAU_PAINT_TYPE_RADIAL_GRADIENT) {
        vau_trans_vector_gradient_radial_attr(&(api->paint_attr.gradient.radial),
                                              &(drv->paint_attr.gradient.radial));
    } else {
        vau_trans_vector_gradient_sweep_attr(&(api->paint_attr.gradient.sweep),
                                             &(drv->paint_attr.gradient.sweep));
    }
    drv->paint_attr.gradient.tile_mode = (drv_vau_paint_tile_mode)api->paint_attr.gradient.tile_mode;
    drv->paint_attr.gradient.stops.num = api->paint_attr.gradient.stops.num;
    for (td_u8 i = 0; i < api->paint_attr.gradient.stops.num; i++) {
        drv->paint_attr.gradient.stops.colors[i]  = api->paint_attr.gradient.stops.colors[i];
        drv->paint_attr.gradient.stops.offsets[i] =
            (td_u8)(api->paint_attr.gradient.stops.offsets[i] * VAU_GRADIENT_RATIO);
    }
}

STATIC td_void vau_trans_vector_draw_api2drv(const ext_vau_draw_attr *api, drv_vau_render_attr *drv)
{
    if ((api == TD_NULL) || (api->path == TD_NULL) || (api->paint == TD_NULL)) {
        drv->vector.enable = TD_FALSE;
        return;
    }
    drv->vector.enable     = TD_TRUE;
    drv->vector.cmds       = api->path->cmds;
    drv->vector.cmd_num    = api->path->cmd_num;
    drv->vector.datas      = api->path->datas;
    drv->vector.data_num   = api->path->data_num;
    drv->vector.paint_mode = (drv_vau_paint_mode)api->paint->paint_mode;

    if (api->paint->fill_attr != TD_NULL) {
        ext_vau_fill_attr *api_fill = api->paint->fill_attr;
        drv_vau_fill_attr *drv_fill = &drv->vector.fill_attr;
        vau_trans_vector_fillattr_api2drv(api_fill, drv_fill);
        make_default_hardware_render_cfg(&api->paint->render_cfg, &drv->vector.render_cfg);
        vau_calc_auxiliary_value(&api->paint->fill_attr->paint_attr, &drv->vector.render_cfg.auxiliary_value);
    }

    return;
}

STATIC td_void vau_trans_render_attr_api2drv(const ext_vau_surface *api_surface, drv_vau_surface *drv_surface)
{
    vau_trans_vector_draw_api2drv(api_surface->draw, &drv_surface->attr);

    if (drv_surface->attr.vector.enable == TD_TRUE) {
        drv_surface->attr.affine.affine_en = TD_FALSE;
    } else {
        drv_surface->attr.affine.affine_en = (api_surface->matrix == TD_NULL) ? TD_FALSE : TD_TRUE;
    }

    if (api_surface->matrix != TD_NULL) {
        for (int i = 0; i < 3; i++) {           /* 3：index */
            for (int j = 0; j < 3; j++) {       /* 3：index */
                drv_surface->attr.affine.matrix[i][j] = api_surface->matrix->matrix[i][j];
            }
        }
    }

    return;
}

static td_void vau_trans_hdr_attr_api2drv(const ext_hdr_attr *api, ext_hdr_attr *drv)
{
    if (api == TD_NULL) {
        drv->cicp_info.color_primaries          = (td_u8)HDR_COLOR_PRIMARIES_BT601;
        drv->cicp_info.transfer_characteristics = (td_u8)HDR_TRANS_CHARAC_GAMMA2_2_TYPE;
        drv->cicp_info.matrix_coefficients      = (td_u8)HDR_MATRIX_COEFFICIENTS_BT601;
        return;
    }

    *drv = *api;
}

static td_s32 vau_trans_cmp_info(const ext_vau_surface *api, drv_vau_surface *drv)
{
    ext_vau_cmp_header info = {0};
    if (drv->compress_info.compress_mode != DRV_GFX_COMPRESS_HIMC) {
        return EXT_SUCCESS;
    }
    if (api->vir_addr == TD_NULL) {
        gpu_err("invalid addr!\n");
        return EXT_ERR_GFX_NULL_POINTER;
    }

    if ((uapi_vau_parse_cmp_header(api->vir_addr, VAU_CMP_HEADER_SIZE, &info) == EXT_SUCCESS) &&
        (info.stride == api->stride)) {
        drv->stride = info.stride;
    } else {
        gpu_err("uapi_vau_parse_cmp_header fail! stride %u %u\n", api->stride, info.stride);
        return EXT_ERR_GFX_INVALID_PARAM;
    }

    drv->cmp_addr = api->phy_addr;
    drv->phy_addr = api->phy_addr + VAU_CMP_HEADER_SIZE;
    drv->compress_info.tile_width   = (td_u32)info.tile_width;
    drv->compress_info.tile_size    = info.tile_size;
    drv->compress_info.alpha_bypass = info.alpha_bypass;
    drv->compress_info.alpha_value  = info.alpha_value;

    return EXT_SUCCESS;
}

static inline td_void vau_trans_pattern_api2drv(const ext_vau_pattern_attr *api_attr, drv_vau_pattern_attr *drv_attr)
{
    if (api_attr == TD_NULL) {
        return;
    }
    drv_attr->argb = api_attr->argb;
    drv_attr->mode = (drv_vau_pattern_mode)api_attr->mode;
    return;
}

STATIC td_s32 vau_trans_threed_api2drv(ext_vau_3d_attr *api_attr, drv_vau_3d_attr *drv_attr,
    ext_vau_mapping_mode mapping_mode)
{
    td_s32 ret;

    if (api_attr == TD_NULL) {
        return EXT_SUCCESS;
    }

    if (api_attr->enable == TD_FALSE) {
        drv_attr->enable = TD_FALSE;
        return EXT_SUCCESS;
    }

    drv_attr->enable = api_attr->enable;
    drv_attr->bg_color = api_attr->bg_color;
    drv_attr->shadow_color = api_attr->shadow_color;
    drv_attr->mapping_color = (drv_vau_sampling_flags)api_attr->mapping_color;
    drv_attr->mapping_alpha = (drv_vau_sampling_flags)api_attr->mapping_alpha;
    drv_attr->model_pos = (drv_vau_sampling_flags)api_attr->model_pos;
    drv_attr->model_flag = (drv_vau_sampling_flags)api_attr->model_flag;
    drv_attr->model_sur.width = api_attr->model_sur.width;
    drv_attr->model_sur.height = api_attr->model_sur.height;
    drv_attr->model_sur.stride = api_attr->model_sur.stride;
    drv_attr->model_sur.phy_addr = api_attr->model_sur.phy_addr;
    drv_attr->model_sur.vir_addr = api_attr->model_sur.vir_addr;
    drv_attr->model_sur.is_mesh_mem_set = TD_FALSE;
    drv_attr->mesh_type = (drv_vau_mesh_type)api_attr->mesh_attr.type;

    /* model data is modified by rm,  model-related config shoud be updated */
    if (mapping_mode == EXT_MAPPING_OPT_MESH) {
        ret = uapi_vau_rm_update_mesh_model_config(drv_attr->mesh_type, &drv_attr->model_sur);
        if (ret != EXT_SUCCESS) {
            return ret;
        }
    }

    return EXT_SUCCESS;
}

STATIC td_s32 vau_trans_surface_api2drv(const ext_vau_surface *api_surface, drv_vau_surface *drv_surface,
    td_bool is_dst_surface)
{
    td_s32 ret;

    drv_surface->width                       = api_surface->width;
    drv_surface->height                      = api_surface->height;
    drv_surface->stride                      = api_surface->stride;
    drv_surface->uv_offset                   = api_surface->uv_offset;
    drv_surface->phy_addr                    = api_surface->phy_addr;
    drv_surface->clut_addr                   = api_surface->clut_addr;
    drv_surface->fmt                         = (drv_gfx_fmt)api_surface->color_fmt;
    drv_surface->argb_order                  = (drv_gfx_argb_order)api_surface->argb_order;
    drv_surface->is_color_surface            = api_surface->is_color_surface;
    drv_surface->color                       = api_surface->color;
    drv_surface->rect                        = api_surface->rect;
    drv_surface->out_rect                    = api_surface->out_rect;
    drv_surface->compress_info.compress_mode = (drv_gfx_compress_mode)api_surface->compress_mode;

    if (memcpy_s(&drv_surface->hsv_info, sizeof(drv_surface->hsv_info),
                 &api_surface->hsv_info, sizeof(api_surface->hsv_info)) != EOK) {
        gpu_err("memcpy_s failure!");
        return EXT_FAILURE;
    }

    if (memcpy_s(&drv_surface->alpha_info, sizeof(drv_surface->alpha_info),
                 &api_surface->alpha_info, sizeof(api_surface->alpha_info)) != EOK) {
        gpu_err("memcpy_s failure!");
        return EXT_FAILURE;
    }

    if (api_surface->mapping_attr.mapping_en == TD_TRUE) {
        drv_surface->mapping_attr.mapping_en   = api_surface->mapping_attr.mapping_en;
        drv_surface->mapping_attr.mapping_mode = (drv_vau_mapping_mode)api_surface->mapping_attr.mapping_mode;
    }

    vau_trans_pattern_api2drv(api_surface->pattern, &(drv_surface->attr.pattern));

    ret = vau_trans_threed_api2drv(api_surface->threed_attr, &(drv_surface->threed_attr),
        api_surface->mapping_attr.mapping_mode);
    if (ret != EXT_SUCCESS) {
        gpu_err("vau_trans_threed_api2drv failed!\n");
        return ret;
    }
    vau_trans_render_attr_api2drv(api_surface, drv_surface);

    vau_trans_hdr_attr_api2drv(api_surface->hdr_attr, &(drv_surface->hdr_attr));

    if (is_dst_surface != TD_TRUE) {
        return vau_trans_cmp_info(api_surface, drv_surface);
    }

    return EXT_SUCCESS;
}

STATIC drv_vau_blend_cmd vau_trans_blendcmd(ext_vau_blend_cmd cmd)
{
    td_u32 idx;
    const vau_blend_cmd_ref_table ref[] = {
        { EXT_VAU_BLENDCMD_NONE,    DRV_VAU_BLENDCMD_NONE },
        { EXT_VAU_BLENDCMD_CLEAR,   DRV_VAU_BLENDCMD_CLEAR },
        { EXT_VAU_BLENDCMD_SRC,     DRV_VAU_BLENDCMD_SRC },
        { EXT_VAU_BLENDCMD_SRCOVER, DRV_VAU_BLENDCMD_SRCOVER },
        { EXT_VAU_BLENDCMD_DSTOVER, DRV_VAU_BLENDCMD_DSTOVER },
        { EXT_VAU_BLENDCMD_SRCIN,   DRV_VAU_BLENDCMD_SRCIN },
        { EXT_VAU_BLENDCMD_DSTIN,   DRV_VAU_BLENDCMD_DSTIN },
        { EXT_VAU_BLENDCMD_SRCOUT,  DRV_VAU_BLENDCMD_SRCOUT },
        { EXT_VAU_BLENDCMD_DSTOUT,  DRV_VAU_BLENDCMD_DSTOUT },
        { EXT_VAU_BLENDCMD_SRCATOP, DRV_VAU_BLENDCMD_SRCATOP },
        { EXT_VAU_BLENDCMD_DSTATOP, DRV_VAU_BLENDCMD_DSTATOP },
        { EXT_VAU_BLENDCMD_ADD,     DRV_VAU_BLENDCMD_ADD },
        { EXT_VAU_BLENDCMD_XOR,     DRV_VAU_BLENDCMD_XOR },
        { EXT_VAU_BLENDCMD_DST,     DRV_VAU_BLENDCMD_DST },
        { EXT_VAU_BLENDCMD_CONFIG,  DRV_VAU_BLENDCMD_CONFIG },
        { EXT_VAU_BLENDCMD_MAX,     DRV_VAU_BLENDCMD_MAX }
    };

    for (idx = 0; idx < sizeof(ref) / sizeof(ref[0]); idx++) {
        if (ref[idx].cmd_api == cmd) {
            return ref[idx].cmd_drv;
        }
    }

    gpu_err("invalid blend cmd:%d\n", cmd);
    return DRV_VAU_BLENDCMD_NONE;
}

STATIC drv_vau_blend_mode vau_trans_blendmode(ext_vau_blend_mode mode)
{
    switch (mode) {
        case EXT_VAU_BLEND_ZERO:
            return DRV_VAU_BLEND_ZERO;
        case EXT_VAU_BLEND_ONE:
            return DRV_VAU_BLEND_ONE;
        case EXT_VAU_BLEND_SRC2COLOR:
            return DRV_VAU_BLEND_SRC2COLOR;
        case EXT_VAU_BLEND_INVSRC2COLOR:
            return DRV_VAU_BLEND_INVSRC2COLOR;
        case EXT_VAU_BLEND_SRC2ALPHA:
            return DRV_VAU_BLEND_SRC2ALPHA;
        case EXT_VAU_BLEND_INVSRC2ALPHA:
            return DRV_VAU_BLEND_INVSRC2ALPHA;
        case EXT_VAU_BLEND_SRC1COLOR:
            return DRV_VAU_BLEND_SRC1COLOR;
        case EXT_VAU_BLEND_INVSRC1COLOR:
            return DRV_VAU_BLEND_INVSRC1COLOR;
        case EXT_VAU_BLEND_SRC1ALPHA:
            return DRV_VAU_BLEND_SRC1ALPHA;
        case EXT_VAU_BLEND_INVSRC1ALPHA:
            return DRV_VAU_BLEND_INVSRC1ALPHA;
        case EXT_VAU_BLEND_SRC2ALPHASAT:
            return DRV_VAU_BLEND_SRC2ALPHASAT;
        default:
            gpu_err("invalid blend mode:%d\n", mode);
            return DRV_VAU_BLEND_ZERO;
    }
}

STATIC drv_vau_out_alpha_mode vau_trans_alphamode(ext_vau_out_alpha_mode mode)
{
    switch (mode) {
        case EXT_VAU_OUTALPHA_FROM_NORM:
            return DRV_VAU_OUTALPHA_FROM_NORM;
        case EXT_VAU_OUTALPHA_FROM_BACKGROUND:
            return DRV_VAU_OUTALPHA_FROM_BACKGROUND;
        case EXT_VAU_OUTALPHA_FROM_FOREGROUND:
            return DRV_VAU_OUTALPHA_FROM_FOREGROUND;
        case EXT_VAU_OUTALPHA_FROM_GLOBALALPHA:
            return DRV_VAU_OUTALPHA_FROM_GLOBALALPHA;
        default:
            gpu_err("invalid alpha mode %d\n", mode);
            return DRV_VAU_OUTALPHA_FROM_NORM;
    }
}


STATIC drv_vau_rop_mode vau_trans_rop_mode(ext_vau_rop_mode mode)
{
    td_u32 idx;
    const vau_rop_mode_ref_table ref[] = {
        {EXT_VAU_ROP_BLACK,       DRV_VAU_ROP_BLACK},
        {EXT_VAU_ROP_NOTMERGEPEN, DRV_VAU_ROP_NOTMERGEPEN},
        {EXT_VAU_ROP_MASKNOTPEN,  DRV_VAU_ROP_MASKNOTPEN},
        {EXT_VAU_ROP_NOTCOPYPEN,  DRV_VAU_ROP_NOTCOPYPEN},
        {EXT_VAU_ROP_MASKPENNOT,  DRV_VAU_ROP_MASKPENNOT},
        {EXT_VAU_ROP_NOT,         DRV_VAU_ROP_NOT},
        {EXT_VAU_ROP_XORPEN,      DRV_VAU_ROP_XORPEN},
        {EXT_VAU_ROP_NOTMASKPEN,  DRV_VAU_ROP_NOTMASKPEN},
        {EXT_VAU_ROP_MASKPEN,     DRV_VAU_ROP_MASKPEN},
        {EXT_VAU_ROP_NOTXORPEN,   DRV_VAU_ROP_NOTXORPEN},
        {EXT_VAU_ROP_NOP,         DRV_VAU_ROP_NOP},
        {EXT_VAU_ROP_MERGENOTPEN, DRV_VAU_ROP_MERGENOTPEN},
        {EXT_VAU_ROP_COPYPEN,     DRV_VAU_ROP_COPYPEN},
        {EXT_VAU_ROP_MERGEPENNOT, DRV_VAU_ROP_MERGEPENNOT},
        {EXT_VAU_ROP_MERGEPEN,    DRV_VAU_ROP_MERGEPEN},
        {EXT_VAU_ROP_WHITE,       DRV_VAU_ROP_WHITE},
        {EXT_VAU_ROP_MAX,         DRV_VAU_ROP_MAX}
    };

    for (idx = 0; idx < sizeof(ref) / sizeof(ref[0]); idx++) {
        if (ref[idx].mode_api == mode) {
            return ref[idx].mode_drv;
        }
    }

    gpu_err("invalid rop mode:%d\n", mode);
    return DRV_VAU_ROP_BLACK;
}

STATIC td_void vau_trans_colorkey_component(const ext_vau_colorkey_component *api,
                                            drv_vau_colorkey_component *drv)
{
    drv->component_min       = api->component_min;
    drv->component_max       = api->component_max;
    drv->is_component_out    = api->is_component_out;
    drv->is_component_ignore = api->is_component_ignore;
    drv->component_mask      = api->component_mask;

    return;
}

STATIC td_void vau_trans_colorkey_value(const ext_vau_colorkey_value *api, drv_vau_colorkey_value *drv)
{
    vau_trans_colorkey_component(&api->alpha, &drv->alpha);
    vau_trans_colorkey_component(&api->red, &drv->red);
    vau_trans_colorkey_component(&api->green, &drv->green);
    vau_trans_colorkey_component(&api->blue, &drv->blue);

    return;
}

STATIC drv_vau_colorkey_mode vau_trans_colorkey_mode(ext_vau_colorkey_mode mode)
{
    switch (mode) {
        case EXT_VAU_COLORKEY_NONE:
            return DRV_VAU_COLORKEY_NONE;
        case EXT_VAU_COLORKEY_FOREGROUND:
            return DRV_VAU_COLORKEY_FOREGROUND;
        case EXT_VAU_COLORKEY_BACKGROUND:
            return DRV_VAU_COLORKEY_BACKGROUND;
        default:
            gpu_err("invalid colorkey mode:%d\n", mode);
            return DRV_VAU_COLORKEY_NONE;
    }
}

STATIC td_void vau_trans_opt_api2drv(const ext_vau_blit_opt *api, drv_vau_blit_opt *drv)
{
    drv->colorize_en = api->colorize_en;

    drv->blend_opt.blend_enable = api->blend_opt.blend_enable;
    if (api->blend_opt.blend_enable == TD_TRUE) {
        drv->blend_opt.blend_cmd = vau_trans_blendcmd(api->blend_opt.blend_cmd);
        drv->blend_opt.background_blend_mode = vau_trans_blendmode(api->blend_opt.background_blend_mode);
        drv->blend_opt.foreground_blend_mode = vau_trans_blendmode(api->blend_opt.foreground_blend_mode);
        drv->blend_opt.out_alpha_mode = vau_trans_alphamode(api->blend_opt.out_alpha_mode);
    }

    drv->rop_opt.rop_enable = api->rop_opt.rop_enable;
    if (api->rop_opt.rop_enable == TD_TRUE) {
        drv->rop_opt.alpha_rop_mode = vau_trans_rop_mode(api->rop_opt.alpha_rop_mode);
        drv->rop_opt.color_rop_mode = vau_trans_rop_mode(api->rop_opt.color_rop_mode);
    }

    drv->colorkey_opt.colorkey_enable = api->colorkey_opt.colorkey_enable;
    if (api->colorkey_opt.colorkey_enable == TD_TRUE) {
        drv->colorkey_opt.colorkey_mode = vau_trans_colorkey_mode(api->colorkey_opt.colorkey_mode);
        vau_trans_colorkey_value(&api->colorkey_opt.colorkey_value, &drv->colorkey_opt.colorkey_value);
    }

    if (api->blur_opt.blur_en == TD_TRUE) {
        drv->blur_opt.blur_en  = TD_TRUE;
        drv->blur_opt.alpha_en = api->blur_opt.alpha_en;
        drv->blur_opt.mask_en  = (api->blur_opt.area == EXT_VAU_BLUR_AREA_MASK_IMG) ? TD_TRUE : TD_FALSE;
        td_float sigma = vau_math_float_is_equal(api->blur_opt.sigma_x, 0.0) ?
                         api->blur_opt.sigma_y : api->blur_opt.sigma_x;
        uapi_vau_rm_product_blur_coef(sigma, &(drv->blur_opt));
    }

    drv->lighting_opt.enable = api->lighting_opt.enable;
    if (drv->lighting_opt.enable == TD_TRUE) {
        memcpy_s(&drv->lighting_opt, sizeof(drv->lighting_opt), &api->lighting_opt, sizeof(api->lighting_opt));
    }

    drv->rain_opt.enable = api->rain_opt.enable;
    return;
}

STATIC td_bool vau_trans_surface_is_dma(const ext_vau_surface_list *surface_list)
{
    if ((surface_list->src_surface_cnt == 1) &&
        (surface_list->opt == TD_NULL) &&
        (surface_list->src_surface[0].matrix == TD_NULL) &&
        (surface_list->src_surface[0].pattern == TD_NULL) &&
        (surface_list->src_surface[0].draw == TD_NULL) &&
        (surface_list->src_surface[0].color_fmt == surface_list->dst_surface->color_fmt) &&
        (surface_list->src_surface[0].rect.width == surface_list->dst_surface->rect.width) &&
        (surface_list->src_surface[0].rect.height == surface_list->dst_surface->rect.height) &&
        (surface_list->src_surface[0].argb_order == surface_list->dst_surface->argb_order) &&
        (surface_list->src_surface[0].compress_mode == surface_list->dst_surface->compress_mode) &&
        (surface_list->src_surface[0].mapping_attr.mapping_en != TD_TRUE) &&
        (surface_list->src_surface[0].rect.width == surface_list->src_surface[0].width) &&
        (surface_list->src_surface[0].rect.height == surface_list->src_surface[0].height) &&
        (surface_list->dst_surface->compress_mode == EXT_GFX_COMPRESS_NONE)) {
        return TD_TRUE;
    }
    return TD_FALSE;
}

STATIC td_s32 vau_trans_surface_list_api2drv(const ext_vau_surface_list *api_list,
                                             drv_vau_surface_list *drv_list)
{
    td_s32 ret;

    drv_list->src_surface_cnt = api_list->src_surface_cnt;

    if (vau_trans_surface_is_dma(api_list) == TD_TRUE) {
        drv_list->src_surface[0].is_dma = TD_TRUE;
    }

    for (td_u32 i = 0; i < api_list->src_surface_cnt; i++) {
        ret = vau_trans_surface_api2drv(&(api_list->src_surface[i]), &(drv_list->src_surface[i]), TD_FALSE);
        if (ret != EXT_SUCCESS) {
            return ret;
        }
        uapi_vau_rm_product_mapping_matrix(&(api_list->src_surface[i]), &(drv_list->src_surface[i]));
        uapi_vau_rm_product_mapping_info(&(api_list->src_surface[i]), api_list->dst_surface,
                                         &(drv_list->src_surface[i]));
    }

    vau_trans_surface_api2drv(api_list->dst_surface, drv_list->dst_surface, TD_TRUE);

    if (api_list->opt != TD_NULL) {
        vau_trans_opt_api2drv(api_list->opt, drv_list->opt);
#ifndef GPU_BLUR_POSTIVE_MATRIX
        if (api_list->opt->blur_opt.blur_en == TD_TRUE) {
            for (td_u32 i = 0; i < api_list->src_surface_cnt; i++) {
                drv_list->src_surface[i].attr.affine.is_inv = TD_TRUE;
            }
        }
#endif
    } else {
        drv_list->opt = TD_NULL;
    }

    return EXT_SUCCESS;
}

STATIC td_void vau_reset_context(drv_vau_surface_list *drv_list)
{
    td_u32 index = drv_list->src_surface_cnt > 1 ? 1 : 0;

    uapi_vau_rm_update_mesh_model_deconfig(&drv_list->src_surface[index].threed_attr.model_sur);
}

STATIC td_void vau_modify_fb_cmp_surface(drv_vau_surface_list *surface_list, drv_vau_blit_opt *opt)
{
    if (surface_list->src_surface_cnt == 1 &&
        surface_list->dst_surface->compress_info.compress_mode != DRV_GFX_COMPRESS_NONE) {
        if ((surface_list->opt != TD_NULL && surface_list->opt->blur_opt.blur_en == TD_TRUE) ||        // blur no blend
            (surface_list->src_surface->is_color_surface && surface_list->src_surface->color == 0)) {  // clear buffer
            return;
        }

        surface_list->src_surface_cnt = 2; // 2:BLEND
        surface_list->src_surface[1] = surface_list->src_surface[0];
        surface_list->src_surface[1].alpha_info.pixel_alpha_en = TD_TRUE;
        surface_list->src_surface[0] = *(surface_list->dst_surface);

        if (surface_list->opt == TD_NULL) {
            opt->blend_opt.blend_enable   = TD_TRUE;
            opt->blend_opt.blend_cmd      = DRV_VAU_BLENDCMD_SRCOVER;
            opt->blend_opt.out_alpha_mode = DRV_VAU_OUTALPHA_FROM_NORM;
            surface_list->opt = opt;
        } else if (surface_list->opt->blend_opt.blend_enable == TD_FALSE) {
            surface_list->opt->blend_opt.blend_enable = TD_TRUE;
            surface_list->opt->blend_opt.blend_cmd = DRV_VAU_BLENDCMD_SRCOVER;
            surface_list->opt->blend_opt.out_alpha_mode = DRV_VAU_OUTALPHA_FROM_NORM;
        }
    }
    return;
}

td_handle uapi_vau_create(td_void)
{
    td_s32 ret;
    td_handle handle = 0;
    vau_dev_lock();

    if (g_vau_dev_init_cnt == 0) {
        g_vau_dev_fd = vau_open(VAU_DEV_NAME, OSAL_O_RDWR, 0);
    }

    if (g_vau_dev_fd < 0) {
        g_vau_dev_fd = -1;
        vau_dev_unlock();
        gpu_chr_log(MT_VAU_MANAGER, MT_VAU_CREATE, MT_VAU_OPEN_FAIL, 0);
        gpu_err("open vau device failed!");
        return EXT_FAILURE;
    }

    g_vau_dev_init_cnt++;

    ret = vau_ioctl(g_vau_dev_fd, DRV_VAU_CMD_CREATE, &handle);
    if (ret != EXT_SUCCESS) {
        vau_dev_unlock();
        uapi_vau_destroy(handle);
        gpu_chr_log(MT_VAU_MANAGER, MT_VAU_CREATE, MT_VAU_CREATE_HANDLE_FAIL, 0);
        gpu_err("uapi_vau_create failed, ret = %#x!", ret);
        return EXT_FAILURE;
    }

    vau_dev_unlock();
    return handle;
}

td_s32 uapi_vau_destroy(td_handle handle)
{
    td_s32 ret;
    vau_dev_lock();

    if (g_vau_dev_init_cnt == 0) {
        vau_dev_unlock();
        return EXT_FAILURE;
    }
    g_vau_dev_init_cnt--;

    if (g_vau_dev_fd < 0) {
        vau_dev_unlock();
        return EXT_FAILURE;
    }

    if (handle > 0) {
        ret = vau_ioctl(g_vau_dev_fd, DRV_VAU_CMD_DESTROY, &handle);
        if (ret != EXT_SUCCESS) {
            vau_dev_unlock();
            gpu_chr_log(MT_VAU_MANAGER, MT_VAU_DESTROY, MT_VAU_DESTROY_FAIL, 0);
            gpu_err("uapi_vau_destroy failed, ret = %#x!", ret);
            return ret;
        }
    }

    if (g_vau_dev_init_cnt == 0) {
        vau_close(g_vau_dev_fd);
        g_vau_dev_fd = -1;
    }

    vau_dev_unlock();
    return EXT_SUCCESS;
}

td_s32 uapi_vau_fill(td_handle handle, ext_vau_surface *surface)
{
    td_s32 ret;
    drv_vau_surface src_surface = {0};
    drv_vau_surface dst_surface = {0};
    drv_vau_blit_cmd blit_cmd = {0};
    ext_vau_surface_list surface_list = {0};

    check_vau_dev_fd_return();
    check_vau_handle_return(handle);

    surface_list.src_surface_cnt = 1;
    surface_list.src_surface = surface;
    surface_list.dst_surface = surface;
    surface_list.opt = TD_NULL;

    vau_debug_print_surface_list(&surface_list);

    ret = vau_api_check_surface_list(&surface_list, TD_FALSE);
    if (ret != EXT_SUCCESS) {
        gpu_chr_log(MT_VAU_MANAGER, MT_VAU_FILL, MT_VAU_FILL_PARAMETER_CHECK_FAIL, 0);
        gpu_err("vau fill check_surface_list fail ret = %#x!", ret);
        return ret;
    }

    blit_cmd.hanlde = handle;
    blit_cmd.surface_list.src_surface = &src_surface;
    blit_cmd.surface_list.dst_surface = &dst_surface;
    blit_cmd.surface_list.opt = TD_NULL;

    ret = vau_trans_surface_list_api2drv(&surface_list, &(blit_cmd.surface_list));
    if (ret != EXT_SUCCESS) {
        vau_reset_context(&(blit_cmd.surface_list));
        gpu_chr_log(MT_VAU_MANAGER, MT_VAU_FILL, MT_VAU_FILL_PARAMETER_CHECK_FAIL, 0);
        gpu_err("vau_conver_api2drv fail ret = %#x!", ret);
        return ret;
    }

    /* src fmt fix ARGB8888 */
    blit_cmd.surface_list.src_surface->fmt = DRV_GFX_FMT_ARGB8888;
    blit_cmd.surface_list.dst_surface->is_color_surface = TD_FALSE;

    ret = vau_ioctl(g_vau_dev_fd, DRV_VAU_CMD_FILL, &blit_cmd);
    if (ret != EXT_SUCCESS) {
        gpu_chr_log(MT_VAU_MANAGER, MT_VAU_FILL, MT_VAU_FILL_FAIL, 0);
        gpu_err("call DRV_VAU_CMD_BLIT failed, ret = %#x!", ret);
        return ret;
    }

    return EXT_SUCCESS;
}

td_s32 uapi_vau_blur(td_handle handle, ext_vau_surface_list *surface_list)
{
    td_s32 ret;
    check_vau_dev_fd_return();
    check_vau_handle_return(handle);
    vau_debug_print_surface_list(surface_list);

    ret = vau_api_check_surface_list(surface_list, TD_FALSE);
    if (ret != EXT_SUCCESS) {
        gpu_chr_log(MT_VAU_MANAGER, MT_VAU_BLUR, MT_VAU_BLUR_PARAMETER_CHECK_FAIL, 0);
        gpu_err("call vau_check_surface_list fail ret = %#x!", ret);
        return ret;
    }

    ret = vau_blur_process(handle, surface_list);
    if (ret != EXT_SUCCESS) {
        gpu_chr_log(MT_VAU_MANAGER, MT_VAU_BLUR, MT_VAU_BLUR_FAIL, 0);
        gpu_err("call vau_blur_process fail ret = %#x!", ret);
        return ret;
    }

    return EXT_SUCCESS;
}

td_s32 uapi_vau_render(td_handle handle, const ext_vau_surface_list *surface_list)
{
    td_s32 ret;
    drv_vau_surface src_surface[2] = {0}; /* render support at most 2 src surfaces */
    drv_vau_surface dst_surface = {0};
    drv_vau_blit_opt opt = {0};
    drv_vau_blit_cmd blit_cmd = {0};

    check_vau_dev_fd_return();
    check_vau_handle_return(handle);
    vau_debug_print_surface_list(surface_list);

    ret = vau_api_check_surface_list(surface_list, TD_FALSE);
    if (ret != EXT_SUCCESS) {
        gpu_chr_log(MT_VAU_MANAGER, MT_VAU_RENDER, MT_VAU_BLUR_PARAMETER_CHECK_FAIL, 0);
        gpu_err("call vau_check_surface_list fail ret = %#x!", ret);
        return ret;
    }

#ifdef GPU_SDF_SUPPORT
    if (vau_check_is_sdf(surface_list) == TD_TRUE) {
        return vau_sdf_directly(handle, surface_list);
    }
#endif

#ifdef CONFIG_VAU_DRAW_DIRECTLY
    if (vau_check_is_vector_draw(surface_list) == TD_TRUE) {
        return vau_draw_directly(handle, surface_list);
    }
#endif

    blit_cmd.hanlde = handle;
    blit_cmd.surface_list.src_surface = src_surface;
    blit_cmd.surface_list.dst_surface = &dst_surface;
    blit_cmd.surface_list.opt = &opt;

    ret = vau_trans_surface_list_api2drv(surface_list, &(blit_cmd.surface_list));
    if (ret != EXT_SUCCESS) {
        gpu_err("vau_conver_api2drv fail ret = %#x!", ret);
        vau_reset_context(&(blit_cmd.surface_list));
        return ret;
    }
    vau_modify_fb_cmp_surface(&blit_cmd.surface_list, &opt);

    ret = vau_ioctl(g_vau_dev_fd, DRV_VAU_CMD_BLIT, &blit_cmd);
    if (ret != EXT_SUCCESS) {
        gpu_chr_log(MT_VAU_MANAGER, MT_VAU_RENDER, MT_VAU_RENDER_FAIL, 0);
        /* no need to call vau_reset_context, context will be reset in cancel process */
        gpu_err("call DRV_VAU_CMD_BLIT failed, ret = %#x!", ret);
    }

    return ret;
}

td_s32 uapi_vau_compose(td_handle handle, const ext_vau_surface_list *surface_list)
{
    td_s32 ret;
    drv_vau_surface tmp_src[2] = {0};
    drv_vau_surface *src_surface = TD_NULL;
    drv_vau_surface dst_surface = {0};
    drv_vau_blit_opt opt = {0};
    drv_vau_blit_cmd blit_cmd = {0};

    check_vau_dev_fd_return();
    check_vau_handle_return(handle);
    vau_debug_print_surface_list(surface_list);

    ret = vau_api_check_surface_list(surface_list, TD_TRUE);
    if (ret != EXT_SUCCESS) {
        gpu_err("call vau_check_surface_list fail ret = %#x!", ret);
        return ret;
    }

    if (surface_list->src_surface_cnt == 1) {
        blit_cmd.surface_list.src_surface = tmp_src;
    } else {
        src_surface = (drv_vau_surface *)vau_psram_alloc(surface_list->src_surface_cnt * sizeof(drv_vau_surface));
        if (src_surface == NULL) {
            gpu_err("vau_psram_alloc failure!");
            return EXT_FAILURE;
        }

        ret = memset_s((td_void *)src_surface, VAU_SRC_SURFACE_CNT_MAX * sizeof(drv_vau_surface),
            0x0, surface_list->src_surface_cnt * sizeof(drv_vau_surface));
        if (ret != EOK) {
            gpu_err("call memset_s failure");
            vau_psram_free((td_uchar *)src_surface);
            return EXT_FAILURE;
        }

        blit_cmd.surface_list.src_surface = src_surface;
    }

    blit_cmd.hanlde = handle;
    blit_cmd.surface_list.dst_surface = &dst_surface;
    blit_cmd.surface_list.opt = &opt;

    ret = vau_trans_surface_list_api2drv(surface_list, &(blit_cmd.surface_list));
    if (ret != EXT_SUCCESS) {
        gpu_err("vau_conver_api2drv fail ret = %#x!", ret);
        gpu_chr_log(MT_VAU_MANAGER, MT_VAU_COMPOSE, MT_VAU_COMPOSE_CHECK_FAIL, 0);
        vau_reset_context(&(blit_cmd.surface_list));
        if (surface_list->src_surface_cnt != 1) {
            vau_psram_free((td_uchar *)src_surface);
        }
        return ret;
    }

    ret = vau_ioctl(g_vau_dev_fd, DRV_VAU_CMD_COMPOSE, &blit_cmd);
    if (ret != EXT_SUCCESS) {
        gpu_chr_log(MT_VAU_MANAGER, MT_VAU_COMPOSE, MT_VAU_COMPOSE_FAIL, 0);
        gpu_err("call DRV_VAU_CMD_COMPOSE failed, ret = %#x!", ret);
        if (surface_list->src_surface_cnt != 1) {
            vau_psram_free((td_uchar *)src_surface);
        }
        return ret;
    }

    if (surface_list->src_surface_cnt != 1) {
        vau_psram_free((td_uchar *)src_surface);
    }

    return EXT_SUCCESS;
}

td_s32 uapi_vau_render_hdr(td_handle handle, const ext_vau_surface_list *surface_list)
{
    td_s32 ret;
    drv_vau_surface src_surface[2] = {0}; /* 2 src surfaces are in use */
    drv_vau_surface dst_surface = {0};
    drv_vau_blit_opt opt = {0};
    drv_vau_blit_cmd hdr_cmd = {0};

    check_vau_dev_fd_return();
    check_vau_handle_return(handle);
    vau_debug_print_surface_list(surface_list);

    ret = vau_api_check_surface_list(surface_list, TD_FALSE);
    if (ret != EXT_SUCCESS) {
        gpu_err("call vau_check_surface_list fail ret = %#x!", ret);
        return ret;
    }

    hdr_cmd.hanlde = handle;
    hdr_cmd.surface_list.src_surface = src_surface;
    hdr_cmd.surface_list.dst_surface = &dst_surface;
    hdr_cmd.surface_list.opt = &opt;

    ret = vau_trans_surface_list_api2drv(surface_list, &(hdr_cmd.surface_list));
    if (ret != EXT_SUCCESS) {
        gpu_err("vau_conver_api2drv fail ret = %#x!", ret);
        return ret;
    }

    ret = vau_ioctl(g_vau_dev_fd, DRV_VAU_CMD_BLIT_HDR, &hdr_cmd);
    if (ret != EXT_SUCCESS) {
        gpu_err("call DRV_VAU_CMD_BLIT_HDR failed, ret = %#x!", ret);
        return ret;
    }

    return EXT_SUCCESS;
}

td_s32 uapi_vau_submit(td_handle handle, td_bool is_sync, td_u32 time_out)
{
    td_s32 ret;
    drv_vau_submit_cmd submit_cmd = {0};

    check_vau_dev_fd_return();
    check_vau_handle_return(handle);

    submit_cmd.handle   = handle;
    submit_cmd.is_sync  = is_sync;
    submit_cmd.time_out = time_out;

    ret = vau_ioctl(g_vau_dev_fd, DRV_VAU_CMD_SUBMIT, &submit_cmd);
    if (ret != EXT_SUCCESS) {
        gpu_err("uapi_vau_submit failed, ret = %#x!", ret);
        return ret;
    }

    return EXT_SUCCESS;
}

td_s32 uapi_vau_cancel(td_handle handle)
{
    td_s32 ret;

    check_vau_dev_fd_return();
    check_vau_handle_return(handle);

    ret = vau_ioctl(g_vau_dev_fd, DRV_VAU_CMD_CANCEL, &handle);
    if (ret != EXT_SUCCESS) {
        gpu_chr_log(MT_VAU_MANAGER, MT_VAU_CANCEL, MT_VAU_CANCEL_FAIL, 0);
        gpu_err("uapi_vau_cancel failed, ret = %#x!", ret);
        return ret;
    }

    return EXT_SUCCESS;
}

td_s32 uapi_vau_parse_cmp_header(td_u8 *head, td_u32 size, ext_vau_cmp_header *info)
{
    if ((head == TD_NULL) || (info == TD_NULL)) {
        gpu_err("invaliure pointe\n");
        return EXT_FAILURE;
    }
    return vau_parse_cmp_header(head, size, info);
}

td_s32 uapi_vau_parse_3d_model_header(td_u8 *head, td_u32 size, ext_3d_header *info)
{
    if ((head == TD_NULL) || (info == TD_NULL) || size < 0xd) {
        gpu_err("invaliure pointe\n");
        return EXT_FAILURE;
    }
    info->width  = (head[0x0] << 0x8) | head[0x1];
    info->height = (head[0x2] << 0x8) | head[0x3];
    info->stride = (head[0x4] << 0x8) | head[0x5];
    info->radius = (head[0x6] << 0x8) | head[0x7];
    info->mask_width = (head[0x8] << 0x8) | head[0x9];
    info->mask_height = (head[0xa] << 0x8) | head[0xb];

    return EXT_SUCCESS;
}

td_s32 uapi_vau_wait_for_done(td_handle handle)
{
    td_s32 ret;

    check_vau_dev_fd_return();
    check_vau_handle_return(handle);

    ret = vau_ioctl(g_vau_dev_fd, DRV_VAU_CMD_WAIT_FOR_DONE, &handle);
    if (ret != EXT_SUCCESS) {
        gpu_chr_log(MT_VAU_MANAGER, MT_VAU_WAIT_FOR_DONE, MT_VAU_WAIT_FOR_DONE_TIMEOUT, 0);
        gpu_err("uapi_vau_wait_for_done failed, ret = %#x!", ret);
        return ret;
    }

    return EXT_SUCCESS;
}

td_s32 uapi_vau_set_cubic_threshold(td_float cubic_threshold)
{
    td_s32 ret;
    check_vau_dev_fd_return();

    ret = vau_ioctl(g_vau_dev_fd, DRV_VAU_CMD_SET_CUBIC_THD, &cubic_threshold);
    if (ret != EXT_SUCCESS) {
        gpu_err("uapi_vau_set_cubic_threshold failed, ret = %#x!", ret);
        return ret;
    }

    return EXT_SUCCESS;
}

td_s32 uapi_vau_get_draw_rect(ext_rect *rect)
{
    td_s32 ret;
    check_vau_dev_fd_return();

    if (rect == TD_NULL) {
        gpu_err("input rect is null pointer!");
        return EXT_FAILURE;
    }

    ret = vau_ioctl(g_vau_dev_fd, DRV_VAU_CMD_GET_DRAW_RECT, rect);
    if (ret != EXT_SUCCESS) {
        gpu_err("uapi_vau_get_draw_rect failed, ret = %#x!", ret);
        return ret;
    }

    return EXT_SUCCESS;
}

td_s32 uapi_vau_get_sphere_matrix(ext_vau_matrix *matrix, td_float radius, td_float x_angle, td_float y_angle,
    td_float z_angle)
{
    if (matrix == TD_NULL) {
        gpu_err("matrix null!\n");
        return EXT_FAILURE;
    }
    if (vau_math_float_is_zero(radius) == TD_TRUE) {
        gpu_err("invalid radius!\n");
        return EXT_FAILURE;
    }
    return uapi_vau_matrix_sphere(matrix, radius, x_angle, y_angle, z_angle);
}

td_s32 vau_subnode_check_surface_list_para(const ext_vau_surface_list *surface_list, td_bool is_compose)
{
    td_u32 max_surface_cnt;

    if (is_compose) {
        max_surface_cnt = VAU_SRC_SURFACE_CNT_MAX;
    } else {
        max_surface_cnt = 2; /* render only support 2 src */
    }

    if (surface_list == TD_NULL) {
        gpu_err("surface_list is null!");
        return EXT_FAILURE;
    }

    if ((surface_list->src_surface_cnt == 0) || (surface_list->src_surface_cnt > max_surface_cnt)) {
        gpu_err("surface_cnt[%u] reach max[%u]!", surface_list->src_surface_cnt, max_surface_cnt);
        return EXT_FAILURE;
    }

    if (surface_list->src_surface == TD_NULL) {
        gpu_err("null pointer");
        return EXT_FAILURE;
    }
    return EXT_SUCCESS;
}

td_s32 uapi_vau_get_render_subnode(td_u32 node_addr, td_bool is_full_node, const ext_vau_surface_list *surface_list)
{
#ifdef GPU_SUBNODE_SUPPORT
    if (node_addr == 0) {
        gpu_err("input node_addr is null!");
        return EXT_ERR_GFX_INVALID_PARAM;
    }

    if (vau_subnode_check_surface_list_para(surface_list, TD_FALSE) != EXT_SUCCESS) {
        return EXT_FAILURE;
    }

    if (is_full_node) {
        td_s32 ret = vau_api_check_surface_list(surface_list, TD_FALSE);
        if (ret != EXT_SUCCESS) {
            gpu_err("call vau_check_surface_list fail ret = %#x!", ret);
            return ret;
        }
    }

    if (vau_dfx_check_mask(VAU_DFX_MASK_SURFACE) == TD_TRUE) {
        gpu_print("************** render subnode is_full_node %d *********************\n", is_full_node);
    }

    vau_debug_print_surface_list(surface_list);

    return vau_draw_get_subnode(node_addr, is_full_node, surface_list);
#else
    uapi_unused(node_addr);
    uapi_unused(is_full_node);
    uapi_unused(surface_list);
    gpu_err("not support render subnode!");
    return EXT_SUCCESS;
#endif
}

td_s32 uapi_vau_wait_all_done(td_void)
{
    td_s32 ret = vau_ioctl(g_vau_dev_fd, DRV_VAU_CMD_WAIT_ALL_DONE, TD_NULL);
    if (ret != EXT_SUCCESS) {
        gpu_chr_log(MT_VAU_MANAGER, MT_VAU_WAIT_ALL_DONE, MT_VAU_WAIT_FOR_DONE_TIMEOUT, 0);
        gpu_err("uapi_vau_wait_all_done failed, ret = %#x!", ret);
        return ret;
    }
    return EXT_SUCCESS;
}
