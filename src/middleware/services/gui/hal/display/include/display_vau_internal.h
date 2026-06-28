/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: display vau hal layer
 * Author: Hisi Graphic Team
 */

#ifndef DISPLAY_VAU_INTERNAL_H
#define DISPLAY_VAU_INTERNAL_H

#include <math.h>
#include <securec.h>
#include "display_vgu.h"
#include "display_gfx_internal.h"
#include "display_gralloc.h"
#include "display_type.h"
#include "graphic_hardware_config.h"
#include "soc_vau_api.h"
#include "soc_vau_type.h"
#include "td_base.h"
#if !defined(_WIN32) && !defined(QSPI_DISPLAY)
#include "soc_mipi_tx.h"
#include "soc_osal.h"
#endif

#define INVALID_HDL 0xFFFFFFFF
#define MAX_GRADIENT_STOPS_NUM 7

enum BufferHandleState {
    BUFFER_HDL_UNUSE,
    BUFFER_HDL_CREATE,
    BUFFER_HDL_RENDER,
    BUFFER_HDL_SUBMIT,
};

struct BufferHandleInfo {
    enum BufferHandleState state;
    BufferHandle* bufferHandle;
};

typedef struct {
    ext_vau_paint paint;
    ext_vau_fill_attr vaufillAttr;
    ext_vau_stroke_attr vauStrokeAttr;
    ext_vau_color fillColors[MAX_GRADIENT_STOPS_NUM];
    td_float fillOffsets[MAX_GRADIENT_STOPS_NUM];
    ext_vau_color strokeColors[MAX_GRADIENT_STOPS_NUM];
    td_float strokeOffsets[MAX_GRADIENT_STOPS_NUM];
} DrawAttrParams;

typedef struct {
    int16_t nodeSize;
    uint32_t renderNum;
    uint32_t tmpCachedNum;
    uint32_t totalCachedNum;
    uint8_t *nodeCaches;
    bool cacheAdded;
    VGURenderGroupShareType subShareType;
} RenderNodeCache;

typedef struct {
    ext_vau_matrix vauMatrix;
    ext_vau_draw_attr drawAttr;
    DrawAttrParams drawParams;
    ext_vau_blit_opt opt;
    ext_vau_surface srcs[2]; // 2: src count
    ext_vau_surface dst;
} SurfaceListParams;

extern td_handle g_hdl;
extern GrallocFuncs* g_grallocFuncs;

VGUResult StartRender(void);
VGUResult RenderWaitDone(int32_t timeOut);
VGUResult RenderSubmit(bool isSync, int32_t *fence);
uintptr_t VGUPathInit(VGUPathDataType type, bool enAlias);
VGUResult RenderPath(VGUSurface *target, uintptr_t path, const VGUMatrix3 *matrix, const VGUFillAttr *fillAttr,
    const VGUStrokeAttr *strokeAttr);
VGUResult ClipImgByPath(VGUSurface *target, const VGUImage *src, uintptr_t path,
    const VGUMatrix3 *matrix, const VGUFillAttr *fillAttr);
VGUResult RenderClearRect(VGUSurface *target, const VGURect *rect, uint32_t color);
VGUResult VGUPathAppendData(uintptr_t path, uint32_t num, const uint8_t *cmd, uint32_t dataNum,
    const void *data);
VGUResult VGUPathSetData(uintptr_t path, uint32_t num, const uint8_t *cmd, uint32_t dataNum,
    const void *data);
VGUResult VGUPathClear(uintptr_t path);
VGUResult VGUPathAppendPath(uintptr_t path, uintptr_t subPath);
VGUResult RenderBlit(VGUSurface *target, const VGUImage *src, uint16_t count);
VGUResult RenderBlur(VGUSurface *target, float blur, uint32_t quality, VGUSurface *maskInfo);
VGUResult RenderLighting(VGUSurface *target, VGULightStyle lightStyle);
VGUResult RenderHSV(VGUSurface *target, const VGUImage *src, const VGUImage *mask, const VGUHSVInfo *hsvInfo);
VGUResult Render3DMesh(VGUSurface *target, VGUImage *texture, VGUModelSurface *model, VGUMeshAttr *mesh);
VGUResult Render3DCylinder(VGUSurface *target, VGUImage *texture, VGUModelSurface *model, VGUCameraAttr* cam);
VGUResult Render3DSphere(VGUSurface *target, VGUImage *texture, VGUModelSurface *model,
    VGUSphereAttr *sphere, const VGUImage *mask);
VGUResult RenderBlitOpt(VGUSurface *target, const VGUImage *src, ext_vau_surface *srcSurface, uint16_t count);
VGUResult RenderRain(VGUSurface *target, const VGUImage *rain);
bool IsCacheRenderSupport(void);
uint32_t InitRenderCache(int32_t renderNum);
void DeinitRenderCache(uint32_t cacheKey);
VGUResult CacheRenderGroupBegin(uint32_t cacheKey,
    VGURenderGroupShareType preShareType, VGURenderGroupShareType subShareType);
uint16_t CacheRenderGroupEnd(uint32_t cacheKey);
VGUResult CacheOnePath(uint32_t cacheKey, VGUSurface *target, const VGUMatrix3 *matrix, const VGUFillAttr *fillAttr,
    const VGUStrokeAttr *strokeAttr);
VGUResult SetGroupRenderBufferAddr(uint32_t cacheKey, uint32_t nodeId, uint64_t memHandle);
VGUResult SetGroupRenderRect(uint32_t cacheKey, uint32_t nodeId, VGURect *clipRect);
VGUResult SetGroupRenderBuffer(uint32_t cacheKey, uint32_t nodeId, VGUBuffer *buffer, VGURect *clipRect);
VGUResult SetGroupRenderMatrix(uint32_t cacheKey, uint32_t nodeId, const VGUMatrix3 *matrix);
VGUResult TranslateRenderNode(uint32_t cacheKey, uint32_t nodeId, int16_t offsetX, int16_t offsetY);
VGUResult SetRenderNodeOpa(uint32_t cacheKey, uint32_t nodeId, uint8_t opa);
VGUResult DoRenderByCache(uint32_t cacheKey);
VGUResult RenderBlitAYUV(VGUSurface *target, const VGUImage *alpha, const VGUImage *yuv);

bool VauEqual(float a, float b);
int32_t VauMin(int32_t a, int32_t b);
int32_t VauMax(int32_t a, int32_t b);
uint32_t VauAlignByte(uint32_t byte, uint32_t align);
void Local2WorldMatrix(ext_vau_matrix* mat, VGURect* rect);

ext_vau_blend_cmd ConvertBlendType(VGUBlendType blendType);
bool HandleCompressMode(const VGUImage *img, ext_vau_surface *vauSurface);
uint32_t CalcSize(uint32_t stride, uint32_t height, VGUCompressMode cmpMode);
uint32_t CalcStride(uint32_t width, PixelFormat fmt, VGUCompressMode cmpMode);
int32_t Convert2VauSurface(const VGUBuffer *surface, ext_vau_surface *vauSurface);
VGUResult ConvertVGUImg2VauSurface(const VGUImage *img, ext_vau_surface *vauSurface);
VGUResult ConvertCmpMode(VGUCompressMode src, ext_gfx_compress_mode *dst);

void CreateHandleIfNull(void);
void UpdateBufferHdlState(void* addr, enum BufferHandleState state);
void FreeSubmittedBufferHdl(void);
void SubmitBufferHdl(void);

void* DispMalloc(uint32_t size);
void DispFree(void* addr);
bool DispCheckAddr(void* addr);
VGUResult HandleRenderError(td_handle handle);
bool AllocBufferForSurface(ext_vau_surface* surface, VGUPixelFormat fmt);

#endif /* DISPLAY_VAU_INTERNAL_H */