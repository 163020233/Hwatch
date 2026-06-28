/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: display vau hal layer
 * Author: Hisi Graphic Team
 */

#include "display_vau_internal.h"

VGUResult RenderRain(VGUSurface *target, const VGUImage *rain)
{
#if CAPABILITY_HW_RAIN
    ext_vau_surface srcSurface[2] = {0};
    ext_vau_surface dstSurface = {0};
    CreateHandleIfNull();
 
    if (ConvertVGUImg2VauSurface(rain, &srcSurface[0]) != VGU_SUCCESS) {
        DISP_LOGE("ConvertVGUImg2VauSurface failed\n");
        return VGU_OPERATION_FAILED;
    }

    if (Convert2VauSurface(&target->buffer, &dstSurface) != VGU_SUCCESS) {
        return VGU_OPERATION_FAILED;
    }
    ext_rect fullRect = {0, 0, target->buffer.width, target->buffer.height};
    dstSurface.rect = fullRect;
    dstSurface.out_rect = fullRect;
    memcpy_s(&srcSurface[1], sizeof(ext_vau_surface), &dstSurface, sizeof(ext_vau_surface));

    ext_vau_blit_opt opt = {0};
    opt.rain_opt.enable   = TD_TRUE;
    opt.blend_opt.blend_enable   = TD_TRUE;
    opt.blend_opt.blend_cmd      = EXT_VAU_BLENDCMD_SRCOVER;
    opt.blend_opt.background_blend_mode      = EXT_VAU_BLEND_INVSRC1COLOR;
    opt.blend_opt.foreground_blend_mode      = EXT_VAU_BLEND_ONE;
 
    ext_vau_surface_list surfaceList = {0};
    surfaceList.src_surface_cnt = 2; // 2: src1 & src2
    surfaceList.src_surface = srcSurface;
    surfaceList.dst_surface = &dstSurface;
    surfaceList.opt = &opt;
 
    int32_t ret = uapi_vau_compose(g_hdl, &surfaceList);
    if (ret != VGU_SUCCESS) {
        DISP_LOGE("uapi_vau_render failure, ret = 0x%x!\n", ret);
        uapi_vau_cancel(g_hdl);
        g_hdl = INVALID_HDL;
        return VGU_OPERATION_FAILED;
    }
    return VGU_SUCCESS;
#else
    return VGU_NO_SUPPORT;
#endif
}