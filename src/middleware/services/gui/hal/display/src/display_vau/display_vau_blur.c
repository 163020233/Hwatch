/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: display vau hal layer
 * Author: Hisi Graphic Team
 */

#include "display_vau_internal.h"
#include "vau_rm_api.h"
#include "vau_matrix.h"

#if CAPABILITY_HW_GAUSSIAN_BLUR
static void GetBlurScaleAndSigma(ext_vau_surface_list *api_list, td_u32 *scale)
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
}

static void BlurGetVMatrix(ext_vau_surface *src_sur, td_u32 scale, ext_vau_matrix *matrix)
{
    ext_rect dst_rect;
    dst_rect.x = 0;
    dst_rect.y = 0;
    dst_rect.width  = src_sur->rect.height / scale;
    dst_rect.height = src_sur->rect.width  / scale;
    uapi_vau_rm_product_blur_matrix(src_sur->rect, dst_rect, TD_TRUE, 1.0f / scale, matrix);
    src_sur->matrix = matrix;
}

static void BlurGetHMatrix(ext_vau_surface *internal_sur, td_u32 scale, ext_vau_matrix *matrix)
{
    ext_rect dst_rect;
    dst_rect.x = 0;
    dst_rect.y = 0;
    dst_rect.width  = internal_sur->rect.height * scale;
    dst_rect.height = internal_sur->rect.width  * scale;
    uapi_vau_rm_product_blur_matrix(internal_sur->rect, dst_rect, TD_FALSE, scale, matrix);
    internal_sur->matrix = matrix;
}

static td_s32 RenderBlurVertical(td_handle handle, ext_vau_surface_list *api_list,
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
    BlurGetVMatrix(&tmp_src, scale, &matrix);

    tmp_list.src_surface_cnt = 1;
    tmp_list.src_surface     = &tmp_src;
    tmp_list.dst_surface     = internal_sur;
    tmp_list.opt             = &tmp_opt;

    return uapi_vau_render(handle, &tmp_list);
}

static td_s32 RenderBlurHorizontal(td_handle handle, ext_vau_surface_list *api_list,
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
    BlurGetHMatrix(internal_sur, scale, &matrix);

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

static VGUResult RenderBlurTrans(td_handle handle, ext_vau_surface_list *surface_list)
{
    td_u32 scale = 1;
    GetBlurScaleAndSigma(surface_list, &scale);
    // Init temp bufffer
    ext_vau_surface matTransTempSurface = {0};
    matTransTempSurface.width = surface_list->src_surface[0].rect.height / scale; // 2: scaler
    matTransTempSurface.height = surface_list->src_surface[0].rect.width  / scale; // 2: scaler
    VGUCompressMode compMode = VGU_COMPRESS_NONE;
    matTransTempSurface.compress_mode = EXT_GFX_COMPRESS_NONE;
    if (IsFBCompress()) {
        compMode = VGU_COMPRESS_HFBC_2X;
        matTransTempSurface.compress_mode = EXT_GFX_COMPRESS_HFBC_2X;
    }
#ifdef _WIN32
    // at now, the software algorithm supports only ARGB8888.
    matTransTempSurface.stride = CalcStride(matTransTempSurface.width, PIXEL_FMT_RGBA_8888, compMode);
    matTransTempSurface.color_fmt = EXT_GFX_FMT_ARGB8888;
#else
    matTransTempSurface.stride = CalcStride(matTransTempSurface.width, PIXEL_FMT_RGB_888, compMode);
    matTransTempSurface.color_fmt = EXT_GFX_FMT_RGB888;
#endif
    ext_rect matTransTempRect = {0, 0, matTransTempSurface.width, matTransTempSurface.height};
    matTransTempSurface.rect = matTransTempRect;
    uint32_t datasize = CalcSize(matTransTempSurface.stride, matTransTempSurface.height, compMode);
    matTransTempSurface.phy_addr = (td_u32)(uintptr_t)DispMalloc(datasize);
    if (matTransTempSurface.phy_addr == 0) {
        DISP_LOGE("DispMalloc for matTransTempSurface failed!\n");
        return VGU_OPERATION_FAILED;
    }
    matTransTempSurface.vir_addr = (td_uchar *)(uintptr_t)matTransTempSurface.phy_addr;

    if (RenderBlurVertical(g_hdl, surface_list, &matTransTempSurface, scale) != VGU_SUCCESS) {
        DISP_LOGE("RenderBlurVertical failure!\n");
        UpdateBufferHdlState((void *)(uintptr_t)matTransTempSurface.phy_addr, BUFFER_HDL_UNUSE);
        uapi_vau_cancel(g_hdl);
        return HandleRenderError(g_hdl);
    }

    if (RenderBlurHorizontal(g_hdl, surface_list, &matTransTempSurface, scale) != VGU_SUCCESS) {
        DISP_LOGE("RenderBlurHorizontal failure!\n");
        UpdateBufferHdlState((void *)(uintptr_t)matTransTempSurface.phy_addr, BUFFER_HDL_UNUSE);
        uapi_vau_cancel(g_hdl);
        return HandleRenderError(g_hdl);
    }
    UpdateBufferHdlState((void *)(uintptr_t)matTransTempSurface.phy_addr, BUFFER_HDL_RENDER);

    return EXT_SUCCESS;
}

static VGUResult RenderBlurOpt(VGUSurface *target, ext_vau_surface *srcSurface, td_u32 srcSurfaceCnt,
    ext_vau_surface *tempSurface, ext_vau_blit_opt blitOpt)
{
    ext_rect srcRect = {target->clipRects[0].x, target->clipRects[0].y,
        target->clipRects[0].w, target->clipRects[0].h};

    ext_vau_surface_list surfaceList1 = {0};
    surfaceList1.src_surface_cnt = srcSurfaceCnt;
    surfaceList1.src_surface = srcSurface;
    surfaceList1.dst_surface = tempSurface;
    surfaceList1.opt = &blitOpt;

    if (RenderBlurTrans(g_hdl, &surfaceList1) != VGU_SUCCESS) {
        DISP_LOGE("RenderBlurTrans failed!!!!\n");
        UpdateBufferHdlState((void *)(uintptr_t)tempSurface->phy_addr, BUFFER_HDL_UNUSE);
        return HandleRenderError(g_hdl);
    }

    ext_vau_surface dstSurface = srcSurface[0];
    dstSurface.rect = srcRect;
    tempSurface->out_rect = srcRect;

    ext_vau_blit_opt blitOpt2 = {0};
    blitOpt2.blend_opt.blend_enable = TD_TRUE;
    blitOpt2.blend_opt.blend_cmd = EXT_VAU_BLENDCMD_SRCOVER;
    blitOpt2.blend_opt.out_alpha_mode = EXT_VAU_OUTALPHA_FROM_BACKGROUND;

    ext_vau_surface srcSurface2[2] = {0};
    srcSurface2[0] = dstSurface;
    srcSurface2[1] = *tempSurface;
    srcSurface2[1].alpha_info.pixel_alpha_en = TD_TRUE;
    srcSurface2[1].alpha_info.premult_en     = TD_TRUE;

    ext_vau_surface_list surfaceList2 = {0};
    surfaceList2.src_surface_cnt = 2; // 2: two surface
    surfaceList2.src_surface = srcSurface2;
    surfaceList2.dst_surface = &dstSurface;
    surfaceList2.opt = &blitOpt2;

    if (uapi_vau_render(g_hdl, &surfaceList2) != VGU_SUCCESS) {
        DISP_LOGE("uapi_vau_render failure!\n");
        UpdateBufferHdlState((void *)(uintptr_t)tempSurface->phy_addr, BUFFER_HDL_UNUSE);
        uapi_vau_cancel(g_hdl);
        return HandleRenderError(g_hdl);
    }

    UpdateBufferHdlState((void *)(uintptr_t)tempSurface->phy_addr, BUFFER_HDL_RENDER);
    return VGU_SUCCESS;
}

static VGUResult RenderBlurByRect(VGUSurface *target, float blur, uint32_t quality)
{
    ext_vau_surface srcSurface = {0};

    if (Convert2VauSurface(&target->buffer, &srcSurface) != VGU_SUCCESS) {
        return VGU_OPERATION_FAILED;
    }

    ext_rect srcRect = {target->clipRects[0].x, target->clipRects[0].y,
        target->clipRects[0].w, target->clipRects[0].h};
    ext_rect outRect = {0, 0, srcRect.width, srcRect.height};
    srcSurface.rect = srcRect;
    srcSurface.out_rect = outRect;

    ext_vau_surface tempSurface = srcSurface;
    tempSurface.width = srcRect.width;
    tempSurface.height = srcRect.height;
    tempSurface.rect.x = 0;
    tempSurface.rect.y = 0;
    tempSurface.rect.width = tempSurface.width;
    tempSurface.rect.height = tempSurface.height;
    tempSurface.color_fmt = srcSurface.color_fmt;
    VGUCompressMode compMode = VGU_COMPRESS_NONE;
    tempSurface.compress_mode = EXT_GFX_COMPRESS_NONE;
    if (IsFBCompress()) {
        compMode = VGU_COMPRESS_HFBC_3X;
        tempSurface.compress_mode = EXT_GFX_COMPRESS_HFBC_3X;
    }
    tempSurface.stride = CalcStride(tempSurface.width, target->buffer.pixelFormat, compMode);
    uint32_t datasize = CalcSize(tempSurface.stride, tempSurface.height, compMode);
    tempSurface.phy_addr = (td_u32)(uintptr_t)DispMalloc(datasize);
    if (tempSurface.phy_addr == 0) {
        DISP_LOGE("DispMalloc for tempSurface failed!\n");
        return VGU_OPERATION_FAILED;
    }
    tempSurface.vir_addr = (td_uchar *)(uintptr_t)tempSurface.phy_addr;

    ext_vau_blit_opt blitOpt = {0};
    blitOpt.blur_opt.blur_en = TD_TRUE;
    blitOpt.blur_opt.alpha_en = TD_FALSE;
    blitOpt.blur_opt.sigma_x = blur;
    blitOpt.blur_opt.sigma_y = blur;
    blitOpt.blur_opt.area = EXT_VAU_BLUR_AREA_RECT;
    blitOpt.blur_opt.blur_quality = quality;

    return RenderBlurOpt(target, &srcSurface, 1, &tempSurface, blitOpt);
}

#if CAPABILITY_HW_GAUSSIAN_BLUR_BY_MASK
static VGUResult RenderBlurByMask(VGUSurface *target, float blur, uint32_t quality, VGUSurface *maskInfo)
{
    ext_vau_surface srcSurface[2] = {0};

    if (Convert2VauSurface(&target->buffer, &srcSurface[0]) != VGU_SUCCESS) {
        return VGU_OPERATION_FAILED;
    }

    if (Convert2VauSurface(&maskInfo->buffer, &srcSurface[1]) != VGU_SUCCESS) {
        return VGU_OPERATION_FAILED;
    }

    ext_rect srcRect = {target->clipRects[0].x, target->clipRects[0].y,
        target->clipRects[0].w, target->clipRects[0].h};
    srcSurface[0].rect = srcRect;
    srcSurface[1].rect = srcRect;

    ext_vau_surface tempSurface = srcSurface[0];
    tempSurface.width = srcRect.width;
    tempSurface.height = srcRect.height;
    tempSurface.rect.x = 0;
    tempSurface.rect.y = 0;
    tempSurface.rect.width = tempSurface.width;
    tempSurface.rect.height = tempSurface.height;
    tempSurface.color_fmt = EXT_GFX_FMT_ARGB8888;
    VGUCompressMode compMode = VGU_COMPRESS_NONE;
    tempSurface.compress_mode = EXT_GFX_COMPRESS_NONE;
    if (IsFBCompress()) {
        compMode = VGU_COMPRESS_HFBC_2X;
        tempSurface.compress_mode = EXT_GFX_COMPRESS_HFBC_2X;
    }
    tempSurface.stride = CalcStride(tempSurface.width, PIXEL_FMT_RGBA_8888, compMode);
    uint32_t datasize = CalcSize(tempSurface.stride, tempSurface.height, compMode);
    tempSurface.phy_addr = (td_u32)(uintptr_t)DispMalloc(datasize);
    if (tempSurface.phy_addr == 0) {
        DISP_LOGE("DispMalloc for tempSurface failed!\n");
        return VGU_OPERATION_FAILED;
    }
    tempSurface.vir_addr = (td_uchar *)(uintptr_t)tempSurface.phy_addr;

    ext_vau_blit_opt blitOpt = {0};
    blitOpt.blur_opt.blur_en = TD_TRUE;
    blitOpt.blur_opt.alpha_en = TD_FALSE;
    blitOpt.blur_opt.sigma_x = blur;
    blitOpt.blur_opt.sigma_y = blur;
    blitOpt.blur_opt.area = EXT_VAU_BLUR_AREA_MASK_IMG;
    blitOpt.blur_opt.blur_quality = quality;

    return RenderBlurOpt(target, srcSurface, 2, &tempSurface, blitOpt);  // 2: two surface
}
#endif // CAPABILITY_HW_GAUSSIAN_BLUR_BY_MASK
#endif // CAPABILITY_HW_GAUSSIAN_BLUR

VGUResult RenderBlur(VGUSurface *target, float blur, uint32_t quality, VGUSurface *maskInfo)
{
#if CAPABILITY_HW_GAUSSIAN_BLUR
    CreateHandleIfNull();
#if CAPABILITY_HW_GAUSSIAN_BLUR_BY_MASK
    if (maskInfo != NULL) {
        return RenderBlurByMask(target, blur, quality, maskInfo);
    }
#endif
    return RenderBlurByRect(target, blur, quality);
#else
    return VGU_NO_SUPPORT;
#endif
}