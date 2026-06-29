/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: display vau hal layer
 * Author: Hisi Graphic Team
 */

#include "display_vau_internal.h"

#define BUFFER_HANDLE_COUNT 100

static const uint8_t BYTE_PER_PIXEL_FOR_ARGB8888 = 4;
static const uint8_t OPA_DIVIDER = 255;
static const uint8_t AGRB_ALPHA_SHIFT = 24;
static const uint8_t HTC_CMP_HEADER_SIZE = 16;
static struct BufferHandleInfo g_bufferHandles[BUFFER_HANDLE_COUNT] = {{0, NULL}};

bool VauEqual(float a, float b)
{
    const double minNum = 1e-5;
    if (fabs(a - b) < minNum) {
        return true;
    }
    return false;
}

int32_t VauMin(int32_t a, int32_t b)
{
    return a < b ? a : b;
}

int32_t VauMax(int32_t a, int32_t b)
{
    return a > b ? a : b;
}

uint32_t VauAlignByte(uint32_t byte, uint32_t align)
{
    return (byte + align - 1) & (~(align - 1));
}

static uint32_t HandleGlobalAlpha(uint8_t globalAlpha, uint32_t color)
{
    uint8_t alpha = (uint8_t)(((color >> AGRB_ALPHA_SHIFT) * globalAlpha) / OPA_DIVIDER);
    uint32_t mixedColor = (alpha << AGRB_ALPHA_SHIFT) | (color & 0x00FFFFFF);
    return mixedColor;
}

void Local2WorldMatrix(ext_vau_matrix* mat, VGURect* rect)
{
    /* Ap = ATp':convert affine transform from src rect to screen. */
    /* A:affine matrix  T:translation matrix  p':coordinate in screen  p:coordinate in src rect */
    mat->matrix[0][2] -= (mat->matrix[0][0] * rect->x + mat->matrix[0][1] * rect->y); // 2:index
    mat->matrix[1][2] -= (mat->matrix[1][0] * rect->x + mat->matrix[1][1] * rect->y); // 2:index
    mat->matrix[2][2] -= (mat->matrix[2][0] * rect->x + mat->matrix[2][1] * rect->y); // 2:index
}

ext_vau_blend_cmd ConvertBlendType(VGUBlendType blendType)
{
    switch (blendType) {
        case BLEND_SRC:
            return EXT_VAU_BLENDCMD_SRC;
        case BLEND_SRCOVER:
            return EXT_VAU_BLENDCMD_SRCOVER;
        case BLEND_DSTOVER:
            return EXT_VAU_BLENDCMD_DSTOVER;
        case BLEND_SRCIN:
            return EXT_VAU_BLENDCMD_SRCIN;
        case BLEND_DSTIN:
            return EXT_VAU_BLENDCMD_DSTIN;
        case BLEND_SRCATOP:
            return EXT_VAU_BLENDCMD_SRCATOP;
        case BLEND_DSTATOP:
            return EXT_VAU_BLENDCMD_DSTATOP;
        default:
            DISP_LOGW("blend type %d is not supported yet, use BLEND_SRCOVER instead\n", blendType);
            return EXT_VAU_BLENDCMD_SRCOVER;
    }
}

static VGUResult ConvertFormat(VGUPixelFormat src, ext_gfx_fmt *dst)
{
    switch (src) {
        case PIXEL_FMT_RGBA_8888:
            *dst = EXT_GFX_FMT_ARGB8888;
            break;
        case PIXEL_FMT_RGB_565:
            *dst = EXT_GFX_FMT_RGB565;
            break;
        case PIXEL_FMT_RGB_888:
            *dst = EXT_GFX_FMT_RGB888;
            break;
        case PIXEL_FMT_A1:
            *dst = EXT_GFX_FMT_A1;
            break;
        case PIXEL_FMT_A2:
            *dst = EXT_GFX_FMT_A2;
            break;
        case PIXEL_FMT_A4:
            *dst = EXT_GFX_FMT_A4;
            break;
        case PIXEL_FMT_A8:
            *dst = EXT_GFX_FMT_A8;
            break;
        case PIXEL_FMT_YCBCR_420_SP:
            *dst = EXT_GFX_FMT_YUV420SP;
            break;
        default:
            DISP_LOGE("Unsupported Format!\n");
            return VGU_NO_SUPPORT;
    }
    return VGU_SUCCESS;
}

VGUResult ConvertCmpMode(VGUCompressMode src, ext_gfx_compress_mode *dst)
{
    switch (src) {
        case VGU_COMPRESS_NONE:
            *dst = EXT_GFX_COMPRESS_NONE;
            break;
        case VGU_COMPRESS_HFBC:
        case VGU_COMPRESS_HFBC_ABYPASS:
        case VGU_COMPRESS_HIMC:
            *dst = EXT_GFX_COMPRESS_HIMC;
            break;
        case VGU_COMPRESS_HFBC_2X:
            *dst = EXT_GFX_COMPRESS_HFBC_2X;
            break;
        case VGU_COMPRESS_HFBC_3X:
            *dst = EXT_GFX_COMPRESS_HFBC_3X;
            break;
        default:
            DISP_LOGE("unsupported compress mode, mode=%d\n", src);
            return VGU_NO_SUPPORT;
    }
    return VGU_SUCCESS;
}

uint32_t CalcStride(uint32_t width, PixelFormat fmt, VGUCompressMode cmpMode)
{
    uint32_t bitPerPixel;
    uint32_t stride;
    switch (fmt) {
        case PIXEL_FMT_RGBA_8888:
            bitPerPixel = 32; // 32: bit per pixel
            break;
        case PIXEL_FMT_RGBA_5551:
        case PIXEL_FMT_RGB_565:
            bitPerPixel = 16; // 16: bit per pixel
            break;
        case PIXEL_FMT_RGB_888:
            bitPerPixel = 24; // 24: bit per pixel
            break;
        case PIXEL_FMT_YCBCR_420_SP:
            bitPerPixel = 8; // 8: bit per pixel
            break;
        default:
            DISP_LOGE("unsupported PixelFormat: %d, just return 0.", fmt);
            return 0;
    }

    switch (cmpMode) {
        case VGU_COMPRESS_NONE:
            stride = VauAlignByte(width, 16) * bitPerPixel / 8; // 16:align value 8:bit per byte
            break;
        case VGU_COMPRESS_HFBC_2X:
            /* 4: align value 8:bit per byte 2:compress  coeff */
            stride = VauAlignByte(width, 4) / 4 * VauAlignByte(4 * 4 * bitPerPixel / 8 / 2, 8);
            break;
        case VGU_COMPRESS_HFBC_3X:
            /* 4: align value 8:bit per byte 3:compress  coeff */
            stride = VauAlignByte(width, 4) / 4 * VauAlignByte(4 * 4 * bitPerPixel / 8 / 3, 8);
            break;
        default:
            DISP_LOGE("unsupported compress mode: %d, just return 0.", cmpMode);
            return 0;
    }

    return stride;
}

uint32_t CalcSize(uint32_t stride, uint32_t height, VGUCompressMode cmpMode)
{
    uint32_t size;
    switch (cmpMode) {
        case VGU_COMPRESS_NONE:
            size = stride * height;
            break;
        case VGU_COMPRESS_HFBC_2X:
        case VGU_COMPRESS_HFBC_3X:
            size = stride * VauAlignByte(height, 4) / 4; // 4: align value
            break;
        default:
            DISP_LOGE("unsupported compress mode: %d, just return 0.", cmpMode);
            return 0;
    }

    return size;
}

bool HandleCompressMode(const VGUImage *img, ext_vau_surface *vauSurface)
{
    if (vauSurface->compress_mode == EXT_GFX_COMPRESS_HFBC_2X ||
        vauSurface->compress_mode == EXT_GFX_COMPRESS_HFBC_3X) {
        return true;
    }

    if (img->enCompressed) {
        ext_vau_cmp_header cmpHeader = {0};
        if (uapi_vau_parse_cmp_header((uint8_t*)(uintptr_t)img->buffer.memHandle,
            HTC_CMP_HEADER_SIZE, &cmpHeader) != VGU_SUCCESS) {
            DISP_LOGE("uapi_vau_parse_cmp_header failed\n");
            return false;
        }
        if (cmpHeader.alpha_bypass == TD_TRUE) {
#if TARGET_CHIP_BRANDY
            vauSurface->compress_mode = EXT_GFX_COMPRESS_HFBC_ABYPASS;
        } else {
            vauSurface->compress_mode = EXT_GFX_COMPRESS_HFBC;
#else
            vauSurface->compress_mode = EXT_GFX_COMPRESS_HIMC;
        } else {
            vauSurface->compress_mode = EXT_GFX_COMPRESS_HIMC;
#endif
        }
        vauSurface->stride = cmpHeader.stride;
    } else {
        vauSurface->compress_mode = EXT_GFX_COMPRESS_NONE;
    }
    return true;
}

static void HandleAlphaBlendInfoForFill(ext_vau_surface_list* list, VGURect *rect,
    uint32_t color, VGUBlendType blendType, uint8_t globalAlpha)
{
    list->src_surface[1].width = rect->w;
    list->src_surface[1].height = rect->h;
    list->src_surface[1].stride = VauAlignByte(list->src_surface[1].width, DISP_BYTE_ALIGNMENT) *
        BYTE_PER_PIXEL_FOR_ARGB8888;

    list->src_surface[1].color_fmt = EXT_GFX_FMT_ARGB8888;
    list->src_surface[1].is_color_surface = TD_TRUE;
    list->src_surface[1].color = color;
    list->src_surface[1].compress_mode = EXT_GFX_COMPRESS_NONE;

    list->src_surface[1].alpha_info.pixel_alpha_en = TD_TRUE;
    list->src_surface[1].alpha_info.premult_en = TD_TRUE;

    list->src_surface[1].alpha_info.global_alpha_en = TD_TRUE;
    list->src_surface[1].alpha_info.global_alpha = globalAlpha;

    list->src_surface[0].alpha_info.global_alpha = 0xff;

    list->opt->blend_opt.blend_enable = TD_TRUE;
    list->opt->blend_opt.blend_cmd = ConvertBlendType(blendType);
    list->opt->blend_opt.out_alpha_mode = EXT_VAU_OUTALPHA_FROM_NORM;
}

int32_t Convert2VauSurface(const VGUBuffer *surface, ext_vau_surface *vauSurface)
{
    if (surface == NULL) {
        DISP_LOGE("surface is null!\n");
        return VGU_OPERATION_FAILED;
    }
    if (ConvertFormat(surface->pixelFormat, &vauSurface->color_fmt) != VGU_SUCCESS) {
        DISP_LOGE("unsupported color format: %d\n", surface->pixelFormat);
        return VGU_NO_SUPPORT;
    }

    if (ConvertCmpMode(surface->compressMode, &vauSurface->compress_mode) != VGU_SUCCESS) {
        DISP_LOGE("unsupported compressMode: %d\n", surface->compressMode);
        return VGU_NO_SUPPORT;
    }

    vauSurface->width = surface->width;
    vauSurface->height = surface->height;
    vauSurface->stride = surface->stride;
    vauSurface->phy_addr = (td_u32)(surface->memHandle);
    vauSurface->vir_addr = (td_uchar *)(uintptr_t)vauSurface->phy_addr;
    return VGU_SUCCESS;
}

VGUResult ConvertVGUImg2VauSurface(const VGUImage *img, ext_vau_surface *vauSurface)
{
    if (ConvertFormat(img->buffer.pixelFormat, &vauSurface->color_fmt) != VGU_SUCCESS) {
        DISP_LOGE("unsupported color format\n");
        return VGU_NO_SUPPORT;
    }
    if (ConvertCmpMode(img->buffer.compressMode, &vauSurface->compress_mode) != VGU_SUCCESS) {
        DISP_LOGE("unsupported color format\n");
        return VGU_NO_SUPPORT;
    }

    if (img->buffer.isSolid) {
        vauSurface->is_color_surface = TD_TRUE;
        vauSurface->color = HandleGlobalAlpha(img->globalAlpha, img->buffer.color);
        vauSurface->width = img->rect.w;
        vauSurface->height = img->rect.h;
        vauSurface->stride = CalcStride(vauSurface->width, PIXEL_FMT_RGBA_8888, img->buffer.compressMode);
        vauSurface->alpha_info.global_alpha = 0xff;
    } else {
        vauSurface->width = img->buffer.width;
        vauSurface->height = img->buffer.height;
        vauSurface->stride = img->buffer.stride;
        vauSurface->uv_offset = img->buffer.uvOffset;
        vauSurface->phy_addr = (td_u32)(img->buffer.memHandle);
        vauSurface->vir_addr = (td_uchar *)(uintptr_t)img->buffer.memHandle;
        vauSurface->alpha_info.global_alpha = img->globalAlpha;
        vauSurface->color = img->buffer.color;
    }

    if (!HandleCompressMode(img, vauSurface)) {
        return VGU_OPERATION_FAILED;
    }

    vauSurface->alpha_info.pixel_alpha_en = TD_TRUE;
    vauSurface->alpha_info.premult_en = TD_TRUE;
    vauSurface->alpha_info.global_alpha_en = TD_TRUE;

    VGURect vguSrcRect = img->rect;
    VGURect vguDstRect = img->dstRect;
    ext_rect srcRect = {vguSrcRect.x, vguSrcRect.y, vguSrcRect.w, vguSrcRect.h};
    vauSurface->rect = srcRect;
    ext_rect dstRect = {vguDstRect.x, vguDstRect.y, vguDstRect.w, vguDstRect.h};
    vauSurface->out_rect = dstRect;
    return VGU_SUCCESS;
}

bool AllocBufferForSurface(ext_vau_surface* surface, VGUPixelFormat fmt)
{
    surface->compress_mode = EXT_GFX_COMPRESS_NONE;
    surface->stride = CalcStride(surface->width, fmt, VGU_COMPRESS_NONE);
    uint32_t datasize = CalcSize(surface->stride, surface->height, VGU_COMPRESS_NONE);
    if (IsFBCompress()) {
        surface->compress_mode = EXT_GFX_COMPRESS_HFBC_3X;
        surface->stride = CalcStride(surface->width, fmt, VGU_COMPRESS_HFBC_3X);
        datasize = CalcSize(surface->stride, surface->height, VGU_COMPRESS_HFBC_3X);
    }
    surface->phy_addr = (td_u32)(uintptr_t)DispMalloc(datasize);
    if (surface->phy_addr == 0) {
        return false;
    }
    surface->vir_addr = (td_uchar *)(uintptr_t)surface->phy_addr;
    return true;
}

void CreateHandleIfNull(void)
{
    if (g_hdl == INVALID_HDL) {
        g_hdl = uapi_vau_create();
        if (g_hdl == INVALID_HDL) {
            DISP_LOGE("uapi_vau_create failure!\n");
        }
    }
}

void UpdateBufferHdlState(void* addr, enum BufferHandleState state)
{
    if (addr == NULL) {
        return;
    }
    for (uint8_t i = 0; i < BUFFER_HANDLE_COUNT; i++) {
        if ((g_bufferHandles[i].bufferHandle != NULL) && (g_bufferHandles[i].bufferHandle->virAddr == addr)) {
            g_bufferHandles[i].state = state;
            break;
        }
    }
}

void FreeSubmittedBufferHdl(void)
{
    for (uint8_t i = 0; i < BUFFER_HANDLE_COUNT; i++) {
        if (g_bufferHandles[i].bufferHandle == NULL) {
            continue;
        }
        if (g_bufferHandles[i].state == BUFFER_HDL_SUBMIT || g_bufferHandles[i].state == BUFFER_HDL_UNUSE) {
            g_grallocFuncs->FreeMem(g_bufferHandles[i].bufferHandle);
            g_bufferHandles[i].bufferHandle = NULL;
            g_bufferHandles[i].state = BUFFER_HDL_UNUSE;
        }
    }
}

void SubmitBufferHdl(void)
{
    for (uint8_t i = 0; i < BUFFER_HANDLE_COUNT; i++) {
        if (g_bufferHandles[i].bufferHandle == NULL) {
            continue;
        }
        if (g_bufferHandles[i].state == BUFFER_HDL_RENDER) {
            g_bufferHandles[i].state = BUFFER_HDL_SUBMIT;
        }
    }
}

VGUResult HandleRenderError(td_handle handle)
{
    uapi_vau_cancel(handle);
    g_hdl = INVALID_HDL;
    FreeSubmittedBufferHdl();
    return VGU_OPERATION_FAILED;
}

void* DispMalloc(uint32_t size)
{
    AllocInfo allocInfo;
    allocInfo.expectedSize = size;
    allocInfo.usage = HBM_USE_ASSIGN_SIZE;
    for (uint8_t i = 0; i < BUFFER_HANDLE_COUNT; i++) {
        if (g_bufferHandles[i].bufferHandle == NULL) {
            if (g_grallocFuncs->AllocMem(&allocInfo, &g_bufferHandles[i].bufferHandle) != VGU_SUCCESS) {
                DISP_LOGE("AllocMem failed.\n");
                return NULL;
            }
            g_bufferHandles[i].state = BUFFER_HDL_CREATE;
            CreateHandleIfNull(); // AllocMem may invoke RenderSubmit and destroy cur g_hdl.
            return g_bufferHandles[i].bufferHandle->virAddr;
        }
    }
    DISP_LOGE("DispMalloc failed due to handle limit.\n");
    return NULL;
}

void DispFree(void* addr)
{
    if (addr == NULL) {
        return;
    }
    for (uint8_t i = 0; i < BUFFER_HANDLE_COUNT; i++) {
        if ((g_bufferHandles[i].bufferHandle != NULL) && (g_bufferHandles[i].bufferHandle->virAddr == addr)) {
            g_grallocFuncs->FreeMem(g_bufferHandles[i].bufferHandle);
            g_bufferHandles[i].bufferHandle = NULL;
            g_bufferHandles[i].state = BUFFER_HDL_UNUSE;
            break;
        }
    }
}

bool DispCheckAddr(void* addr)
{
    if (addr == NULL) {
        return false;
    }
    for (uint8_t i = 0; i < BUFFER_HANDLE_COUNT; i++) {
        if ((g_bufferHandles[i].bufferHandle != NULL) && (g_bufferHandles[i].bufferHandle->virAddr == addr)) {
            return true;
        }
    }
    return false;
}