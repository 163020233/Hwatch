/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: display vau hal layer
 * Author: Hisi Graphic Team
 */

#include "display_vau_internal.h"

VGUResult RenderHSV(VGUSurface *target, const VGUImage *src, const VGUImage *mask, const VGUHSVInfo *hsvInfo)
{
#if CAPABILITY_HW_HSV
    ext_vau_surface srcSurface[2] = {0};
    CreateHandleIfNull();

    if (ConvertVGUImg2VauSurface(src, &srcSurface[1]) != VGU_SUCCESS ||
        ConvertVGUImg2VauSurface(mask, &srcSurface[0]) != VGU_SUCCESS) {
        DISP_LOGE("ConvertVGUImg2VauSurface failed\n");
        return VGU_OPERATION_FAILED;
    }

    srcSurface[0].rect = srcSurface[1].rect;
    ext_vau_surface tempSurface = srcSurface[1];
    tempSurface.width = srcSurface[1].rect.width;
    tempSurface.height = srcSurface[1].rect.height;
    tempSurface.rect.x = 0;
    tempSurface.rect.y = 0;
    tempSurface.rect.width = tempSurface.width;
    tempSurface.rect.height = tempSurface.height;
    tempSurface.color_fmt = srcSurface[1].color_fmt;

    if (!AllocBufferForSurface(&tempSurface, src->buffer.pixelFormat)) {
        DISP_LOGE("DispMalloc for surface->failed!\n");
        return VGU_OPERATION_FAILED;
    }

    srcSurface[1].hsv_info.hvs_en = true;
    srcSurface[1].hsv_info.mask_en = true;
    srcSurface[1].hsv_info.mask_threshold = hsvInfo->threshold;
    srcSurface[1].hsv_info.h_adj = hsvInfo->hAdj;
    srcSurface[1].hsv_info.s_adj = hsvInfo->sAdj;
    srcSurface[1].hsv_info.v_adj = hsvInfo->vAdj;

    ext_vau_surface_list surfaceList = {0};
    surfaceList.src_surface_cnt = 2; // 2: src & mask
    surfaceList.src_surface = srcSurface;
    surfaceList.dst_surface = &tempSurface;
    surfaceList.opt = NULL;

    int32_t ret = uapi_vau_render(g_hdl, &surfaceList);
    if (ret != VGU_SUCCESS) {
        DISP_LOGE("uapi_vau_render failure, ret = 0x%x!\n", ret);
        UpdateBufferHdlState((void *)(uintptr_t)tempSurface.phy_addr, BUFFER_HDL_UNUSE);
        return HandleRenderError(g_hdl);
    }

    ext_rect dstRect = {src->dstRect.x, src->dstRect.y, src->dstRect.w, src->dstRect.h};
    tempSurface.out_rect = dstRect;

    ret = RenderBlitOpt(target, src, &tempSurface, 1);
    if (ret != VGU_SUCCESS) {
        DISP_LOGE("uapi_vau_render failure, ret = 0x%x!\n", ret);
        UpdateBufferHdlState((void *)(uintptr_t)tempSurface.phy_addr, BUFFER_HDL_UNUSE);
        return HandleRenderError(g_hdl);
    }

    UpdateBufferHdlState((void *)(uintptr_t)tempSurface.phy_addr, BUFFER_HDL_RENDER);
    return VGU_SUCCESS;
#else
    return VGU_NO_SUPPORT;
#endif
}