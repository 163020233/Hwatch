/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: blur operation
 */

#include "vau_blur.h"
#include "vau_debug.h"
#include "vau_matrix.h"
#include "gpu_mem_config.h"
#include "vau_rm_api.h"

STATIC td_u32 blur_calc_stride(const ext_vau_surface *surface)
{
    td_u32 bpp;
    td_u32 stride;

    switch (surface->color_fmt) {
        case EXT_GFX_FMT_ARGB8888:
            bpp = 32; /* 32 bits */
            break;
        case EXT_GFX_FMT_RGB888:
            bpp = 24; /* 24 bits */
            break;
        case EXT_GFX_FMT_A8:
            bpp = 8; /* 8 bits */
            break;
        default:
            bpp = 32; /* 32 bits */
            break;
    }

    switch (surface->compress_mode) {
        case EXT_GFX_COMPRESS_NONE:
            stride = uapi_byte_align(surface->width, 16) * bpp / 8; /* aligned with 16, 8 bits per byte */
            break;
        case EXT_GFX_COMPRESS_HFBC_2X:
            /* aligned with 4, 8 bits per byte, 2 is compressed ratio */
            stride = uapi_byte_align(surface->width, 4) / 4 * uapi_byte_align(4 * 4 * bpp / 8 / 2, 8);
            break;
        case EXT_GFX_COMPRESS_HFBC_3X:
            /* aligned with 4, 8 bits per byte, 3 is compressed ratio */
            stride = uapi_byte_align(surface->width, 4) / 4 * uapi_byte_align(4 * 4 * bpp / 8 / 3, 8);
            break;
        default:
            /* aligned with 16, 8 bits per byte */
            stride = uapi_byte_align(surface->width, 16) * bpp / 8;
            break;
    }

    return stride;
}

STATIC td_s32 blur_check_param(const ext_vau_surface_list *api_list)
{
    if (api_list->opt == TD_NULL) {
        gpu_err("input opt is null!");
        return EXT_FAILURE;
    }

    if (api_list->opt->blur_opt.blur_en != TD_TRUE) {
        gpu_err("blur_en is not enable!");
        return EXT_FAILURE;
    }

    /* only support RGB888/ARGB8888 fmt blur */
    if (api_list->src_surface[0].color_fmt != EXT_GFX_FMT_ARGB8888 &&
        api_list->src_surface[0].color_fmt != EXT_GFX_FMT_RGB888) {
        gpu_err("color_fmt(%d) is not supported!", api_list->src_surface[0].color_fmt);
        return EXT_FAILURE;
    }

    if ((api_list->opt->blur_opt.area == EXT_VAU_BLUR_AREA_RECT) &&
        (api_list->src_surface_cnt != 1)) {
        gpu_err("blur rect mode, surface cnt only support 1!");
        return EXT_FAILURE;
    }

    if (((api_list->opt->blur_opt.area == EXT_VAU_BLUR_AREA_MASK_DRAW) ||
         (api_list->opt->blur_opt.area == EXT_VAU_BLUR_AREA_MASK_IMG)) &&
        (api_list->src_surface_cnt != 2)) { /* 2: surface cnt */
        gpu_err("blur draw/img mode, surface cnt only support 2!");
        return EXT_FAILURE;
    }

    if (api_list->opt->blur_opt.blur_quality >= EXT_VAU_BLUR_QUALITY_MAX) {
        gpu_err("%d is invalide blur quality!", api_list->opt->blur_opt.blur_quality);
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

STATIC td_void blur_set_internal_mask(const ext_vau_surface *src_sur, ext_vau_surface *mask)
{
    mask->width         = src_sur->width;
    mask->height        = src_sur->height;
    mask->phy_addr      = GPU_PSRAM_BLUR_MASK_A8;
    mask->vir_addr      = (td_uchar *)(uintptr_t)mask->phy_addr;
    mask->color_fmt     = EXT_GFX_FMT_A8;
    mask->argb_order    = EXT_GFX_FMT_ORDER_ARGB;
    mask->compress_mode = EXT_GFX_COMPRESS_NONE;
    mask->stride        = blur_calc_stride(mask);
    mask->rect.x        = 0;
    mask->rect.y        = 0;
    mask->rect.width    = mask->width;
    mask->rect.height   = mask->height;
}

STATIC td_void blur_set_internal_buff(const ext_vau_surface *src_sur, ext_vau_surface *buffer, td_u32 scale)
{
    buffer->width         = src_sur->rect.height / scale;
    buffer->height        = src_sur->rect.width  / scale;
    buffer->phy_addr      = GPU_PSRAM_BLUR_INT_BUF;
    buffer->vir_addr      = (td_uchar *)(uintptr_t)buffer->phy_addr;
    buffer->color_fmt     = EXT_GFX_FMT_RGB888;
    buffer->argb_order    = EXT_GFX_FMT_ORDER_ARGB;
    buffer->compress_mode = EXT_GFX_COMPRESS_HFBC_2X;
    buffer->stride        = blur_calc_stride(buffer);
    buffer->rect.x        = 0;
    buffer->rect.y        = 0;
    buffer->rect.width    = buffer->width;
    buffer->rect.height   = buffer->height;
}

STATIC td_void blur_get_scale_factor(ext_vau_surface_list *api_list, td_u32 *scale)
{
    const td_float sigma_limit = 0.5f;
    const td_u32 expect_scale = 4; // 4: exp scale factor
    const td_u32 expect_scale2 = 2; // 2: second exp scale factor

    if (api_list->opt->blur_opt.blur_quality == EXT_VAU_BLUR_QUALITY_H) {
        *scale = 1;
    } else if ((api_list->src_surface[0].rect.width % expect_scale == 0) &&
        (api_list->src_surface[0].rect.height % expect_scale == 0)) {
        *scale = expect_scale;
    } else if ((api_list->src_surface[0].rect.width % expect_scale2 == 0) &&
        (api_list->src_surface[0].rect.height % expect_scale2 == 0)) {
        *scale = expect_scale2;
    } else {
        *scale = 1;
    }

    // sigma should be over 0.5 to maintain proper fluzzy performance
    if (api_list->opt->blur_opt.sigma_x / (td_float)*scale < sigma_limit) {
        api_list->opt->blur_opt.sigma_x = *scale * sigma_limit;
    }
    if (api_list->opt->blur_opt.sigma_y / (td_float)*scale < sigma_limit) {
        api_list->opt->blur_opt.sigma_y = *scale * sigma_limit;
    }

    return;
}

STATIC td_s32 blur_draw_mask(ext_vau_surface_list *api_list)
{
    td_s32 ret;
    ext_rect bbox = {0};
    ext_vau_surface internal_mask = {0};
    ext_vau_surface_list tmp_list = {0};

    blur_set_internal_mask(&(api_list->src_surface[0]), &internal_mask);

    tmp_list.src_surface_cnt = 1;
    tmp_list.src_surface     = &(api_list->src_surface[1]);
    tmp_list.dst_surface     = &internal_mask;
    tmp_list.opt             = TD_NULL;

    td_handle tmp_handle = uapi_vau_create();
    if (tmp_handle <= 0) {
        gpu_err("uapi_vau_create failed!");
        return EXT_FAILURE;
    }

    ret = uapi_vau_render(tmp_handle, &tmp_list);
    if (ret != EXT_SUCCESS) {
        uapi_vau_cancel(tmp_handle);
        gpu_err("uapi_vau_render failed!");
        return EXT_FAILURE;
    }

    ret = uapi_vau_submit(tmp_handle, TD_TRUE, 1000); /* 1000 ms timeout */
    if (ret != EXT_SUCCESS) {
        uapi_vau_cancel(tmp_handle);
        gpu_err("uapi_vau_submit failed!");
        return EXT_FAILURE;
    }

    ret = uapi_vau_get_draw_rect(&bbox);
    if (ret != EXT_SUCCESS) {
        gpu_err("uapi_vau_get_draw_rect failed!");
        return EXT_FAILURE;
    }

    internal_mask.rect = bbox;
    api_list->src_surface[1] = internal_mask;
    api_list->src_surface[0].rect = bbox;

    // revise dst resolution
    api_list->dst_surface->width  = bbox.width;
    api_list->dst_surface->height = bbox.height;
    api_list->dst_surface->stride = blur_calc_stride(api_list->dst_surface);
    api_list->dst_surface->rect.x = 0;
    api_list->dst_surface->rect.y = 0;
    api_list->dst_surface->rect.width  = api_list->dst_surface->width;
    api_list->dst_surface->rect.height = api_list->dst_surface->height;

    return uapi_vau_destroy(tmp_handle);
}

STATIC td_void blur_get_v_matrix(ext_vau_surface *src_sur, td_u32 scale, ext_vau_matrix *matrix)
{
#ifndef GPU_BLUR_POSTIVE_MATRIX
    ext_rect dst_rect;
    dst_rect.x = 0;
    dst_rect.y = 0;
    dst_rect.width  = src_sur->rect.height / scale;
    dst_rect.height = src_sur->rect.width  / scale;
    uapi_vau_rm_product_blur_matrix(src_sur->rect, dst_rect, TD_TRUE, 1.0f / scale, matrix);
#else
    td_float src_cx = src_sur->rect.x + src_sur->rect.width  / 2.0f;
    td_float src_cy = src_sur->rect.y + src_sur->rect.height / 2.0f;

    td_float dst_cx = (src_sur->rect.height / scale) / 2.0f;
    td_float dst_cy = (src_sur->rect.width  / scale) / 2.0f;

    uapi_vau_matrix_trans(matrix, dst_cx, dst_cy);
    uapi_vau_matrix_reflect_y(matrix);
    uapi_vau_matrix_rotate(matrix, 90.0f);
    uapi_vau_matrix_scale(matrix, 1.0f / scale, 1.0f / scale);
    uapi_vau_matrix_trans(matrix, -src_cx, -src_cy);
#endif
    src_sur->matrix = matrix;
}

STATIC td_void blur_get_h_matrix(ext_vau_surface *internal_sur, td_u32 scale, ext_vau_matrix *matrix)
{
#ifndef GPU_BLUR_POSTIVE_MATRIX
    ext_rect dst_rect;
    dst_rect.x = 0;
    dst_rect.y = 0;
    dst_rect.width  = internal_sur->rect.height * scale;
    dst_rect.height = internal_sur->rect.width  * scale;

    uapi_vau_rm_product_blur_matrix(internal_sur->rect, dst_rect, TD_FALSE, scale, matrix);
#else
    td_float src_cx = internal_sur->rect.width  / 2.0f;
    td_float src_cy = internal_sur->rect.height / 2.0f;

    td_float dst_cx = (internal_sur->rect.height * scale) / 2.0f;
    td_float dst_cy = (internal_sur->rect.width  * scale) / 2.0f;

    uapi_vau_matrix_trans(matrix, dst_cx, dst_cy);
    uapi_vau_matrix_reflect_y(matrix);
    uapi_vau_matrix_rotate(matrix, -90.0f);
    uapi_vau_matrix_scale(matrix, scale, scale);
    uapi_vau_matrix_trans(matrix, -src_cx, -src_cy);
#endif
    internal_sur->matrix = matrix;
}

STATIC td_s32 blur_vertical_render(td_handle handle, ext_vau_surface_list *api_list,
                                   ext_vau_surface *internal_sur, td_u32 scale)
{
    ext_vau_matrix matrix;
    ext_vau_surface tmp_src = { 0 };
    ext_vau_surface_list tmp_list = { 0 };
    ext_vau_blit_opt tmp_opt = { 0 };

    tmp_opt.blur_opt = api_list->opt->blur_opt;
    tmp_opt.blur_opt.area = EXT_VAU_BLUR_AREA_RECT;
    tmp_opt.blur_opt.sigma_x = 0; // keep sigmaY value to blur v
    tmp_opt.blur_opt.sigma_y = api_list->opt->blur_opt.sigma_y / scale;

    tmp_src = api_list->src_surface[0];
    uapi_vau_matrix_identity(&matrix);
    blur_get_v_matrix(&tmp_src, scale, &matrix);

    tmp_list.src_surface_cnt = 1;
    tmp_list.src_surface     = &tmp_src;
    tmp_list.dst_surface     = internal_sur;
    tmp_list.opt             = &tmp_opt;

    return uapi_vau_render(handle, &tmp_list);
}

STATIC td_s32 blur_horizontal_render(td_handle handle, ext_vau_surface_list *api_list,
                                     ext_vau_surface *internal_sur, td_u32 scale)
{
    ext_vau_matrix matrix;
    ext_vau_surface tmp_src[2] = { 0 }; // 2:index
    ext_vau_surface_list tmp_list = { 0 };
    ext_vau_blit_opt tmp_opt = { 0 };

    tmp_opt.blur_opt = api_list->opt->blur_opt;
    tmp_opt.blur_opt.area = (api_list->opt->blur_opt.area != EXT_VAU_BLUR_AREA_RECT) ?
                             EXT_VAU_BLUR_AREA_MASK_IMG : EXT_VAU_BLUR_AREA_RECT;
    tmp_opt.blur_opt.sigma_y = 0; // keep sigmaX value to blur h
    tmp_opt.blur_opt.sigma_x = api_list->opt->blur_opt.sigma_x; // V reduced, H enlarge to original

    uapi_vau_matrix_identity(&matrix);
    blur_get_h_matrix(internal_sur, scale, &matrix);

    if (tmp_opt.blur_opt.area == EXT_VAU_BLUR_AREA_RECT) {
        tmp_list.src_surface_cnt = 1;
        tmp_list.src_surface = internal_sur;
    } else {
        tmp_list.src_surface_cnt = 2; // 2:s1 mask, s2 internal buf
        tmp_list.src_surface     = tmp_src;
        tmp_list.src_surface[0]  = api_list->src_surface[1];
        tmp_list.src_surface[1]  = *internal_sur;
    }

    tmp_list.dst_surface = api_list->dst_surface;
    tmp_list.opt = &tmp_opt;

    return uapi_vau_render(handle, &tmp_list);
}

td_s32 vau_blur_process(td_handle handle, ext_vau_surface_list *api_list)
{
    td_s32 ret;
    td_u32 scale = 1;
    ext_vau_surface internal_buff = {0};

    if (blur_check_param(api_list) != EXT_SUCCESS) {
        return EXT_FAILURE;
    }

    if (api_list->opt->blur_opt.area == EXT_VAU_BLUR_AREA_MASK_DRAW) {
        ret = blur_draw_mask(api_list); // draw mask first
        if (ret != EXT_SUCCESS) {
            gpu_err("blur_draw_mask fail, ret[%#x]!", ret);
            return ret;
        }
    }

    blur_get_scale_factor(api_list, &scale);
    blur_set_internal_buff(&api_list->src_surface[0], &internal_buff, scale);

    ret = blur_vertical_render(handle, api_list, &internal_buff, scale);
    if (ret != EXT_SUCCESS) {
        gpu_err("blur_vertical_render fail, ret[%#x]!", ret);
        return ret;
    }

    ret = blur_horizontal_render(handle, api_list, &internal_buff, scale);
    if (ret != EXT_SUCCESS) {
        gpu_err("blur_horizontal_render fail, ret[%#x]!", ret);
        return ret;
    }

    return EXT_SUCCESS;
}
