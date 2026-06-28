/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: display vau hal layer
 * Author: Hisi Graphic Team
 */

#include "display_vau_internal.h"

static const uint8_t M_PI_NUM_2 = 2;

#if CAPABILITY_HW_LIGHT
static VGUResult CalLightOptAndRender(ext_vau_blit_opt optIn, ext_vau_surface_list surfaceList)
{
    ext_vau_blit_opt opt = optIn;
    int32_t result = VGU_SUCCESS;
    if (!optIn.lighting_opt.attr.is_bi_dir && optIn.lighting_opt.mode > EXT_VAU_CIRCLE_CONST) {
        if (opt.lighting_opt.attr.start_angle_rad < M_PI * M_PI_NUM_2 &&
            opt.lighting_opt.attr.end_angle_rad > M_PI * M_PI_NUM_2) {
            opt.lighting_opt.attr.end_angle_rad = M_PI * M_PI_NUM_2;
            surfaceList.opt = &opt;
            result = uapi_vau_render(g_hdl, &surfaceList);
            if (result != VGU_SUCCESS) {
                DISP_LOGE("uapi_vau_render failure, ret = 0x%x!\n", result);
                return result;
            }
            opt.lighting_opt.attr.start_angle_rad = 0;
            opt.lighting_opt.attr.end_angle_rad = optIn.lighting_opt.attr.end_angle_rad - M_PI * M_PI_NUM_2;
        }
    } else if (optIn.lighting_opt.attr.is_bi_dir && optIn.lighting_opt.mode > EXT_VAU_CIRCLE_CONST) {
        if (opt.lighting_opt.attr.start_angle_rad > M_PI && opt.lighting_opt.attr.start_angle_rad < M_PI * M_PI_NUM_2) {
            opt.lighting_opt.attr.start_angle_rad -= M_PI;
            opt.lighting_opt.attr.end_angle_rad -= M_PI;
        }
        if (opt.lighting_opt.attr.start_angle_rad < M_PI && opt.lighting_opt.attr.end_angle_rad > M_PI) {
            float endAngleRad = opt.lighting_opt.attr.end_angle_rad;
            opt.lighting_opt.attr.end_angle_rad = M_PI;
            surfaceList.opt = &opt;
            result = uapi_vau_render(g_hdl, &surfaceList);
            if (result != VGU_SUCCESS) {
                DISP_LOGE("uapi_vau_render failure, ret = 0x%x!\n", result);
                return result;
            }
            opt.lighting_opt.attr.start_angle_rad = 0;
            opt.lighting_opt.attr.end_angle_rad = endAngleRad - M_PI;
        }
    }
    surfaceList.opt = &opt;
    result = uapi_vau_render(g_hdl, &surfaceList);
    if (result != VGU_SUCCESS) {
        DISP_LOGE("uapi_vau_render failure, ret = 0x%x!\n", result);
        return result;
    }
    return result;
}
#endif

VGUResult RenderLighting(VGUSurface *target, VGULightStyle lightStyle)
{
#if CAPABILITY_HW_LIGHT
    CreateHandleIfNull();
    ext_vau_surface srcSurface = {0};

    if (Convert2VauSurface(&target->buffer, &srcSurface) != VGU_SUCCESS) {
        return VGU_OPERATION_FAILED;
    }

    ext_rect srcRect = {target->clipRects[0].x, target->clipRects[0].y,
        target->clipRects[0].w, target->clipRects[0].h};
    srcSurface.rect = srcRect;
    srcSurface.out_rect = srcRect;

    ext_vau_surface dstSurface = srcSurface;
    dstSurface.rect = srcRect;

    ext_vau_blit_opt blitOpt = {0};
    blitOpt.lighting_opt.enable                    = TD_TRUE;
    blitOpt.lighting_opt.attr.image_mode           = EXT_VAU_IMAGE_SDR;
    blitOpt.lighting_opt.mode                      = lightStyle.lightMode;
    blitOpt.lighting_opt.attr.cx                   = lightStyle.cx;
    blitOpt.lighting_opt.attr.cy                   = lightStyle.cy;
    blitOpt.lighting_opt.attr.radius               = lightStyle.radius;
    blitOpt.lighting_opt.attr.spe_amp              = lightStyle.speAmp;
    blitOpt.lighting_opt.attr.amb_amp              = lightStyle.ambAmp;
    blitOpt.lighting_opt.attr.start_angle_rad      = lightStyle.startAngle;
    blitOpt.lighting_opt.attr.end_angle_rad        = lightStyle.endAngle;
    blitOpt.lighting_opt.attr.start_end_angle_rad  = (lightStyle.startAngle + lightStyle.endAngle) / 2; // 2 : half
    blitOpt.lighting_opt.attr.end_start_angle_rad  = (lightStyle.startAngle + lightStyle.endAngle) / 2; // 2 : half
    blitOpt.lighting_opt.attr.is_bi_dir            = lightStyle.isSymmetrical;

    ext_vau_surface_list surfaceList = {0};
    surfaceList.src_surface_cnt = 1;
    surfaceList.src_surface = &srcSurface;
    surfaceList.dst_surface = &dstSurface;
    surfaceList.opt = &blitOpt;

    return CalLightOptAndRender(blitOpt, surfaceList);
#else
    return VGU_NO_SUPPORT;
#endif
}