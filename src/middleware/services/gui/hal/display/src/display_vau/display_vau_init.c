/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: display vau hal layer
 * Author: Hisi Graphic Team
 */

#include "display_vau_internal.h"

GrallocFuncs* g_grallocFuncs = NULL;
td_handle g_hdl = INVALID_HDL;
static td_handle g_hdlLast = INVALID_HDL;

static VGUResult InitVgu(void)
{
    if (g_grallocFuncs == NULL) {
        if (GrallocInitialize(&g_grallocFuncs) != VGU_SUCCESS) {
            DISP_LOGE("GrallocInitialize failed.\n");
            return VGU_OPERATION_FAILED;
        }
    }
    return VGU_SUCCESS;
}

static VGUResult DeinitVgu(void)
{
    GrallocUninitialize(g_grallocFuncs);
    g_grallocFuncs = NULL;
    return VGU_SUCCESS;
}

VGUResult StartRender(void)
{
    CreateHandleIfNull();
    VGUResult ret = VGU_SUCCESS;
    return ret;
}

VGUResult RenderSubmit(bool isSync, int32_t *fence)
{
    (void)fence;
    int32_t ret;
    if (isSync) {
        td_handle hdlToSubmit = g_hdl;
        g_hdl = INVALID_HDL;
        ret = uapi_vau_submit(hdlToSubmit, TD_TRUE, 3000); // 3000: timeout
        SubmitBufferHdl();
        FreeSubmittedBufferHdl();
        uapi_vau_destroy(hdlToSubmit);
        if (g_hdlLast != INVALID_HDL) {
            uapi_vau_destroy(g_hdlLast);
            g_hdlLast = INVALID_HDL;
        }
    } else {
        td_handle hdlToSubmit = g_hdl;
        g_hdl = INVALID_HDL;
        ret = uapi_vau_submit(hdlToSubmit, TD_FALSE, 0);
        SubmitBufferHdl();
        if (g_hdlLast != INVALID_HDL) {
            DISP_LOGE("g_hdlLast %lld has not been destroyed, destroy now.\n", (int64_t)g_hdlLast);
            uapi_vau_destroy(g_hdlLast);
        }
        g_hdlLast = hdlToSubmit;
    }
    if (ret != VGU_SUCCESS) {
        DISP_LOGE("uapi_vau_submit failure, ret = 0x%x!\n", ret);
        ret = VGU_OPERATION_FAILED;
    }
    return ret;
}

VGUResult RenderWaitDone(int32_t timeOut)
{
    (void)timeOut;
    int32_t ret = VGU_SUCCESS;
    if (g_hdlLast != INVALID_HDL) {
        ret = uapi_vau_wait_for_done(g_hdlLast);
        if (ret != VGU_SUCCESS) {
            DISP_LOGE("uapi_vau_wait_for_done failed.\n");
            ret = VGU_OPERATION_FAILED;
        }
        FreeSubmittedBufferHdl();
        uapi_vau_destroy(g_hdlLast);
        g_hdlLast = INVALID_HDL;
    }
    return ret;
}

VGUResult VGUInitialize(VGUFuncs **funcs)
{
    CHECK_NULLPOINTER_RETURN_VALUE(funcs, VGU_INVALID_PARAMETER);
    VGUFuncs *gFuncs = (VGUFuncs *)malloc(sizeof(VGUFuncs));
    if (gFuncs == NULL) {
        DISP_LOGE("gFuncs is null\n");
        return VGU_OPERATION_FAILED;
    }
    if (memset_s(gFuncs, sizeof(VGUFuncs), 0, sizeof(VGUFuncs)) != EOK) {
        free(gFuncs);
        DISP_LOGE("memset_s gFuncs failed\n");
        return VGU_OPERATION_FAILED;
    }
    gFuncs->InitVgu = InitVgu;
    gFuncs->DeinitVgu = DeinitVgu;
    gFuncs->StartRender = StartRender;
    gFuncs->RenderBlit = RenderBlit;
    gFuncs->RenderBlur = RenderBlur;
    gFuncs->RenderBlurExt = RenderBlur;
    gFuncs->RenderClearRect = RenderClearRect;
    gFuncs->VGUPathInit = VGUPathInit;
    gFuncs->VGUPathAppendData = VGUPathAppendData;
    gFuncs->VGUPathSetData = VGUPathSetData;
    gFuncs->VGUPathAppendPath = VGUPathAppendPath;
    gFuncs->VGUPathClear = VGUPathClear;
    gFuncs->RenderPath = RenderPath;
    gFuncs->RenderSubmit = RenderSubmit;
    gFuncs->RenderWaitDone = RenderWaitDone;
    gFuncs->ClipImgByPath = ClipImgByPath;
    gFuncs->RenderLighting = RenderLighting;
    gFuncs->RenderHSV = RenderHSV;
    gFuncs->RenderRain = RenderRain;
    gFuncs->Render3DMesh = Render3DMesh;
    gFuncs->Render3DCylinder = Render3DCylinder;
    gFuncs->Render3DSphere = Render3DSphere;
    gFuncs->IsCacheRenderSupport = IsCacheRenderSupport;
    gFuncs->InitRenderCache = InitRenderCache;
    gFuncs->DeinitRenderCache = DeinitRenderCache;
    gFuncs->CacheRenderGroupBegin = CacheRenderGroupBegin;
    gFuncs->CacheRenderGroupEnd = CacheRenderGroupEnd;
    gFuncs->CacheOnePath = CacheOnePath;
    gFuncs->SetGroupRenderBufferAddr = SetGroupRenderBufferAddr;
    gFuncs->SetGroupRenderBuffer = SetGroupRenderBuffer;
    gFuncs->SetGroupRenderRect = SetGroupRenderRect;
    gFuncs->SetGroupRenderMatrix = SetGroupRenderMatrix;
    gFuncs->TranslateRenderNode = TranslateRenderNode;
    gFuncs->SetRenderNodeOpa = SetRenderNodeOpa;
    gFuncs->DoRenderByCache = DoRenderByCache;
    gFuncs->RenderBlitAYUV = RenderBlitAYUV;
    *funcs = gFuncs;
    DISP_LOGI("VGU initializes successfully\n");
    return VGU_SUCCESS;
}

VGUResult VGUUninitialize(VGUFuncs *funcs)
{
    CHECK_NULLPOINTER_RETURN_VALUE(funcs, VGU_INVALID_PARAMETER);
    free(funcs);
    funcs = NULL;
    DISP_LOGI("VGU uninitializes successfully\n");
    return VGU_SUCCESS;
}
