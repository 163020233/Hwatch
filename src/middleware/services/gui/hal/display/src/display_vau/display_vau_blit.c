/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: display vau hal layer
 * Author: Hisi Graphic Team
 */

#include <stdio.h>
#include "display_vau_internal.h"

static ext_vau_surface g_srcSurface[BLIT_COUNT_MAX] = {0};
static ext_vau_matrix g_matrix[BLIT_COUNT_MAX] = {0};

VGUResult RenderBlitOpt(VGUSurface *target, const VGUImage *src, ext_vau_surface *srcSurface, uint16_t count)
{
    CreateHandleIfNull();

    for (int i = 0; i < count; i++) {
        if (!src[i].buffer.isSolid) {
            if (VauEqual(src[i].matrix.m[0][0], 1.0f) && VauEqual(src[i].matrix.m[0][1], 0.0f) &&
                VauEqual(src[i].matrix.m[1][0], 0.0f) && VauEqual(src[i].matrix.m[1][1], 1.0f) &&
                VauEqual(src[i].matrix.m[2][0], 0.0f) && VauEqual(src[i].matrix.m[2][1], 0.0f) && // 2: row/col
                VauEqual(src[i].matrix.m[2][2], 1.0f)) { // 2: row/col
                srcSurface[i].matrix = NULL;
            } else {
                ext_vau_matrix mat = {{{src[i].matrix.m[0][0], src[i].matrix.m[0][1], src[i].matrix.m[0][2]},
                    {src[i].matrix.m[1][0], src[i].matrix.m[1][1], src[i].matrix.m[1][2]},
                    {src[i].matrix.m[2][0], src[i].matrix.m[2][1], src[i].matrix.m[2][2]}}};

#if MATRIX_TRANSFORM_SPACE_WORLD
                Local2WorldMatrix(&mat, &src[i].rect);
#endif
                g_matrix[i] = mat;
                srcSurface[i].matrix = &g_matrix[i];
                if (srcSurface[i].alpha_info.global_alpha != 0xff) {
                    srcSurface[i].alpha_info.golbal_premult_en = TD_TRUE;
                }
#if !CAPABILITY_HW_OUT_RECT
                ext_rect outRect = {0, 0, target->buffer.width, target->buffer.height};
                srcSurface[i].out_rect = outRect;
#endif
            }
        }
    }
    ext_vau_surface dstSurface = {0};
    if (Convert2VauSurface(&target->buffer, &dstSurface) != VGU_SUCCESS) {
        return VGU_OPERATION_FAILED;
    }
    ext_rect fullRect = {0, 0, target->buffer.width, target->buffer.height};
    dstSurface.rect = fullRect;

    ext_vau_blit_opt blitOpt = {0};
    blitOpt.blend_opt.blend_enable = TD_TRUE;
    blitOpt.blend_opt.blend_cmd = ConvertBlendType(src[0].blend);
    blitOpt.blend_opt.out_alpha_mode = EXT_VAU_OUTALPHA_FROM_NORM;
#if CAPABILITY_HW_COLORIZE
    blitOpt.colorize_en = src[0].enColorize;
#endif
    ext_vau_surface_list surfaceList = {0};
    surfaceList.src_surface_cnt = count;
    surfaceList.src_surface = srcSurface;
    surfaceList.dst_surface = &dstSurface;
    surfaceList.opt = &blitOpt;

    int32_t ret = uapi_vau_compose(g_hdl, &surfaceList);
    if (ret != VGU_SUCCESS) {
        DISP_LOGE("uapi_vau_compose failure, ret = 0x%x!\n", ret);
        for (int i = 0; i < count; i++) {
            DISP_LOGE("%d: src w:%u h:%u str:%u fmt:%d cmp:%d rect:[%d,%d,%u,%u] outRect:[%d,%d,%u,%u]\n", i,
                srcSurface[i].width, srcSurface[i].height,
                srcSurface[i].stride, srcSurface[i].color_fmt, srcSurface[i].compress_mode,
                srcSurface[i].rect.x, srcSurface[i].rect.y, srcSurface[i].rect.width,
                srcSurface[i].rect.height, srcSurface[i].out_rect.x, srcSurface[i].out_rect.y,
                srcSurface[i].out_rect.width, srcSurface[i].out_rect.height);
            ext_vau_matrix *matrix = srcSurface[i].matrix;
            if (matrix != NULL) {
                DISP_LOGE("matrix:\n");
                for (int j = 0; j < 4; ++j) {  // 4: num
                    DISP_LOGE("%f %f %f %f\n",
                        matrix->matrix[j][0],
                        matrix->matrix[j][1],
                        matrix->matrix[j][2],   // 2: index
                        matrix->matrix[j][3]);  // 3: index
                }
            } else {
                DISP_LOGE("matrix: NULL\n");
            }
        }
        uapi_vau_cancel(g_hdl);
        g_hdl = INVALID_HDL;
        return VGU_OPERATION_FAILED;
    }

    return VGU_SUCCESS;
}

VGUResult RenderBlit(VGUSurface *target, const VGUImage *src, uint16_t count)
{
    CHECK_NULLPOINTER_RETURN_VALUE(target, VGU_INVALID_PARAMETER);
    CHECK_NULLPOINTER_RETURN_VALUE(src, VGU_INVALID_PARAMETER);

    memset_s(g_srcSurface, sizeof(ext_vau_surface) * count, 0, sizeof(ext_vau_surface) * count);
    memset_s(g_matrix, sizeof(ext_vau_matrix) * count, 0, sizeof(ext_vau_matrix) * count);

    VGUBlendType vguBlend = src[0].blend;
    for (int i = 0; i < count; i++) {
        if (src[i].blend != vguBlend) {
            DISP_LOGE("Blend type of each image must be same when blitting them together.\n");
            return VGU_OPERATION_FAILED;
        }
        if (ConvertVGUImg2VauSurface(&src[i], &g_srcSurface[i]) != VGU_SUCCESS) {
            DISP_LOGE("ConvertVGUImg2VauSurface failed\n");
            return VGU_OPERATION_FAILED;
        }
    }

    return RenderBlitOpt(target, src, g_srcSurface, count);
}

static VGUResult BlitAYUVToARGB(VGUSurface *target, const VGUImage *alpha, const VGUImage *yuv,
    ext_vau_surface* dstSurface, ext_vau_surface* srcSurface)
{
    if (ConvertVGUImg2VauSurface(alpha, &srcSurface[0]) != VGU_SUCCESS) {
        DISP_LOGE("ConvertVGUImg2VauSurface alpha surface failed!\n");
        return VGU_OPERATION_FAILED;
    }
    if (ConvertVGUImg2VauSurface(yuv, &srcSurface[1]) != VGU_SUCCESS) {
        DISP_LOGE("ConvertVGUImg2VauSurface yuv surface failed!\n");
        return VGU_OPERATION_FAILED;
    }

    uint32_t tempW = target->clipRects[0].w;
    uint32_t tempH = target->clipRects[0].h;
    VGUCompressMode tempCmp = VGU_COMPRESS_NONE;

    uint32_t tempStride = CalcStride(tempW, PIXEL_FMT_RGBA_8888, tempCmp);
    uint32_t dataSize = CalcSize(tempStride, tempH, tempCmp);
    uint32_t tempAddr = (uint32_t)(uintptr_t)DispMalloc(dataSize);
    if (tempAddr == 0) {
        DISP_LOGE("DispMalloc for tempSurface failed: %u!\n", dataSize);
        return VGU_OPERATION_FAILED;
    }

    if (ConvertCmpMode(tempCmp, &dstSurface->compress_mode) != VGU_SUCCESS) {
        UpdateBufferHdlState((void *)(uintptr_t)tempAddr, BUFFER_HDL_UNUSE);
        DISP_LOGE("ConvertCmpMode failed!\n");
        return VGU_NO_SUPPORT;
    }
    dstSurface->width = tempW;
    dstSurface->height = tempH;
    dstSurface->color_fmt = EXT_GFX_FMT_ARGB8888;
    dstSurface->argb_order = EXT_GFX_FMT_ORDER_ARGB;
    dstSurface->stride = tempStride;
    dstSurface->phy_addr = (td_u32)tempAddr;
    dstSurface->vir_addr = (td_uchar*)(uintptr_t)dstSurface->phy_addr;

    ext_rect dstFullRect = {0, 0, tempW, tempH};
    dstSurface->rect = dstFullRect;

    ext_vau_surface_list surfaceList = { 0 };
    ext_vau_blit_opt blitOpt = {0};
    blitOpt.blend_opt.blend_enable = TD_TRUE;
    blitOpt.blend_opt.blend_cmd = ConvertBlendType(yuv->blend);
    blitOpt.blend_opt.out_alpha_mode = EXT_VAU_OUTALPHA_FROM_BACKGROUND;
    surfaceList.opt = &blitOpt;

    surfaceList.src_surface_cnt = 2; // 2: src surface count
    surfaceList.src_surface = srcSurface;
    surfaceList.dst_surface = dstSurface;

    int32_t ret = uapi_vau_render(g_hdl, &surfaceList);
    if (ret != VGU_SUCCESS) {
        uapi_vau_cancel(g_hdl);
        g_hdl = INVALID_HDL;
        UpdateBufferHdlState((void *)(uintptr_t)tempAddr, BUFFER_HDL_UNUSE);
        DISP_LOGE("uapi_vau_render failed!\n");
        return VGU_OPERATION_FAILED;
    }
    UpdateBufferHdlState((void *)(uintptr_t)tempAddr, BUFFER_HDL_RENDER);
    return VGU_SUCCESS;
}

VGUResult RenderBlitAYUV(VGUSurface *target, const VGUImage *alpha, const VGUImage *yuv)
{
    CHECK_NULLPOINTER_RETURN_VALUE(target, VGU_INVALID_PARAMETER);
    CHECK_NULLPOINTER_RETURN_VALUE(alpha, VGU_INVALID_PARAMETER);
    CHECK_NULLPOINTER_RETURN_VALUE(yuv, VGU_INVALID_PARAMETER);

    ext_vau_surface dstSurface = {0};
    ext_vau_surface srcSurface[2] = {0};

    VGUResult result = BlitAYUVToARGB(target, alpha, yuv, &dstSurface, srcSurface);
    if (result != VGU_SUCCESS) {
        DISP_LOGE("BlitAYUVToARGB failed!\n");
        return result;
    }

    memcpy_s(&srcSurface[1], sizeof(ext_vau_surface), &dstSurface, sizeof(ext_vau_surface));
    srcSurface[1].alpha_info.global_alpha_en = TD_TRUE;
    srcSurface[1].alpha_info.global_alpha = 0xff;
    srcSurface[1].alpha_info.pixel_alpha_en = TD_TRUE;
    srcSurface[1].alpha_info.premult_en = TD_TRUE;

    if (Convert2VauSurface(&target->buffer, &dstSurface) != VGU_SUCCESS) {
        UpdateBufferHdlState((void *)(uintptr_t)srcSurface[1].phy_addr, BUFFER_HDL_UNUSE);
        DISP_LOGE("Convert2VauSurface dstSurface failed!\n");
        return VGU_OPERATION_FAILED;
    }
    ext_rect dstRect = {target->clipRects[0].x, target->clipRects[0].y, target->clipRects[0].w, target->clipRects[0].h};
    dstSurface.rect = dstRect;
    memcpy_s(&srcSurface[0], sizeof(ext_vau_surface), &dstSurface, sizeof(ext_vau_surface));

    ext_vau_surface_list surfaceList = { 0 };
    ext_vau_blit_opt blitOpt = {0};
    blitOpt.blend_opt.blend_enable = TD_TRUE;
    blitOpt.blend_opt.blend_cmd = EXT_VAU_BLENDCMD_SRCOVER;
    blitOpt.blend_opt.out_alpha_mode = EXT_VAU_OUTALPHA_FROM_NORM;
    surfaceList.opt = &blitOpt;

    surfaceList.src_surface_cnt = 2; // 2: src surface count
    surfaceList.src_surface = srcSurface;
    surfaceList.dst_surface = &dstSurface;
    int32_t ret = uapi_vau_render(g_hdl, &surfaceList);
    if (ret != VGU_SUCCESS) {
        uapi_vau_cancel(g_hdl);
        g_hdl = INVALID_HDL;
        UpdateBufferHdlState((void *)(uintptr_t)srcSurface[1].phy_addr, BUFFER_HDL_UNUSE);
        DISP_LOGE("uapi_vau_render failed!\n");
        return VGU_OPERATION_FAILED;
    }

    UpdateBufferHdlState((void *)(uintptr_t)srcSurface[1].phy_addr, BUFFER_HDL_RENDER);
    return VGU_SUCCESS;
}