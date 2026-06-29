/*
 * Copyright (c) 2022 HiSilicon (Shanghai) Technologies Co., Ltd..
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "hals/display_dev.h"
#include <securec.h>
#include "gfx_utils/color.h"
#include "gfx_utils/graphic_log.h"
#include "gfx_utils/graphic_math.h"
#include "hals/gralloc_engines.h"
#include "imgdecode/image_load.h"
#include "display/monitor.h"
#include "engines/gfx/lite_m_gfx_engine.h"

namespace OHOS {
#define ALIGN_DOWN(byte, align) ((byte) - ((byte) % (align)))
DisplayDev::DisplayDev() : isDisplayDevInited_{false}, devId_(0), layerFuncs_{nullptr}
{
}

void DisplayDev::LcdFlush(uint8_t layerId)
{
    MonitorFlush();
}

void DisplayDev::WaitForVBlank()
{
}

LayerBuffer* DisplayDev::GetCurrentBuffer(uint8_t layerId)
{
    static LayerBuffer virLayer = {0};
    virLayer.width = HORIZONTAL_RESOLUTION;
    virLayer.height = VERTICAL_RESOLUTION;
    virLayer.pixFormat = PIXEL_FMT_RGBA_8888;
    virLayer.pitch = ALIGN_BYTE(HORIZONTAL_RESOLUTION, BYTE_ALIGNMENT) * 4; // 4: bpp
    virLayer.data.virAddr = GetFramBuff();
    virLayer.data.phyAddr = (uintptr_t)GetFramBuff();
    return &virLayer;
}

LayerBuffer* DisplayDev::GetNextBuffer(uint8_t layerId)
{
    return nullptr;
}

static uint32_t GetAlignedStride(uint32_t width, PixelFormat fmt, uint8_t cmpMode)
{
    uint32_t bpp;
    uint32_t stride;
    switch (fmt) {
        case PIXEL_FMT_RGBA_8888:
            if (cmpMode == COMPRESS_MODE_HFBC) {
                bpp = 12; /* 12 bits */
            } else if (cmpMode == COMPRESS_MODE_HFBC_ABYPASS) {
                bpp = 16; /* 16 bits */
            } else {
                bpp = 32; /* 32 bits */
            }
            break;
        case PIXEL_FMT_RGBA_5551:
        case PIXEL_FMT_RGB_565:
            bpp = 16; /* 16 bits */
            break;
        case PIXEL_FMT_RGB_888:
            bpp = (cmpMode != COMPRESS_MODE_NONE) ? 8 : 24; /* 8,24 bits */
            break;
        case PIXEL_FMT_A1:
            stride = width >> SHIFT_3;
            if (width & 0x7) {
                stride++;
            }
            stride = ALIGN_BYTE(stride, BYTE_ALIGNMENT);
            return stride;
        default:
            bpp = 32; /* 32 bits */
            break;
    }
    stride = ALIGN_BYTE(width, BYTE_ALIGNMENT) * bpp / STRIDE_DIVIDER;
    if (cmpMode != COMPRESS_MODE_NONE) {
        stride = stride * 4; /* 4: compress coef */
    }
    return stride;
}

uint16_t DisplayDev::CalcAlignCoordinate(uint16_t coordinate, uint8_t cmpMode)
{
    uint16_t alignCoordinate = coordinate;
    if ((cmpMode == COMPRESS_MODE_HFBC_2X) || (cmpMode == COMPRESS_MODE_HFBC_3X)) {
        alignCoordinate = ALIGN_DOWN(coordinate, 4); /* 4: bits */
    } else if ((cmpMode == COMPRESS_MODE_HFBC) || (cmpMode == COMPRESS_MODE_HFBC_ABYPASS) ||
        (cmpMode == COMPRESS_MODE_NONE)) {
        alignCoordinate = ALIGN_DOWN(coordinate, BYTE_ALIGNMENT);
    }
    return alignCoordinate;
}

uint32_t DisplayDev::CalcStride(uint32_t width, PixelFormat fmt, uint8_t cmpMode)
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
        case PIXEL_FMT_A1:
        case PIXEL_FMT_A2:
        case PIXEL_FMT_A4:
        case PIXEL_FMT_A8:
            bitPerPixel = 8; /* 8 bits */
            break;
        default:
            GRAPHIC_LOGE("unsupported PixelFormat: %d, just return 0.", fmt);
            return 0;
    }

    switch (cmpMode) {
        case COMPRESS_MODE_NONE:
            stride = ALIGN_BYTE(width, 16) * bitPerPixel / 8; /** 16-byte aligned  8 bits per byte */
            break;
        case COMPRESS_MODE_HFBC:
        case COMPRESS_MODE_HFBC_ABYPASS:
            stride = GetAlignedStride(width, fmt, cmpMode);
            break;
        default:
            GRAPHIC_LOGE("unsupported compress mode: %d, just return 0.", cmpMode);
            return 0;
    }

    return stride;
}

uint32_t DisplayDev::CalcSize(uint32_t stride, uint32_t height, uint8_t cmpMode)
{
    uint32_t size;
    switch (cmpMode) {
        case COMPRESS_MODE_NONE:
            size = stride * height;
            break;
        default:
            GRAPHIC_LOGE("unsupported compress mode: %d, just return 0.", cmpMode);
            return 0;
            break;
    }

    return size;
}

bool DisplayDev::IsUlpsEnabled(void)
{
    return false;
}

bool DisplayDev::TryExitUlps(bool isFlushing)
{
    if (IsUlpsEnabled() && !exitUlps_) {
        exitUlps_ = true;
        return true;
    }
    return false;
}

bool DisplayDev::IsDoubleBuffer(void)
{
    return false;
}

bool DisplayDev::SwapBuffer(uint8_t layerId)
{
    return true;
}

void DisplayDev::SetLayerDirtyRegion(uint8_t layerId, const Rect& rect)
{
    return;
}

void DisplayDev::DisplayDevInit()
{
    return;
}

void DisplayDev::SetLayerPositon(uint8_t layerId, int16_t x, int16_t y)
{
    return;
}

void DisplayDev::SetLayerColorKey(uint8_t layerId, bool enColorKey, uint32_t color)
{
    return;
}

void DisplayDev::SetLayerZOrder(uint8_t layerId, uint8_t order)
{
    return;
}

void DisplayDev::SetHardwareLayerBuffer(uint8_t layerId, uint8_t bufferId, const LayerBuffer& layer)
{
    return;
}

bool DisplayDev::OpenLayer(uint8_t layerId, const LayerInfo& info, uint8_t bufferCount)
{
    return true;
}

void DisplayDev::CloseLayer(uint8_t layerId)
{
    return;
}

bool DisplayDev::GetCurrentBufferInfo(uint8_t layerId, BufferInfo& info)
{
    LayerBuffer* layer = GetCurrentBuffer(layerId);
    if (layer == nullptr) {
        return false;
    }

    Rect rect = {0, 0, static_cast<short>(layer->width - 1), static_cast<short>(layer->height - 1)};
    info.rect = rect;
    info.stride = layer->pitch;
    info.width = static_cast<uint16_t>(layer->width);
    info.height = static_cast<uint16_t>(layer->height);
#if ENABLE_FB_COMPRESS
    info.compressMode = COMPRESS_MODE_HFBC_2X;
#else
    info.compressMode = COMPRESS_MODE_NONE;
#endif
    switch (layer->pixFormat) {
        case PIXEL_FMT_RGBA_8888:
            info.mode = ARGB8888;
            break;
        case PIXEL_FMT_RGB_888:
            info.mode = RGB888;
            break;
        case PIXEL_FMT_RGB_565:
            info.mode = RGB565;
            break;
        default:
            GRAPHIC_LOGE("unsupported PixelFormat: %d", layer->pixFormat);
            info.mode = UNKNOWN;
            break;
    }
    info.virAddr = layer->data.virAddr;
    info.phyAddr = reinterpret_cast<void*>(layer->data.phyAddr);
#ifdef VERSION_IOT
    info.rotType = (ENABLE_PREROTATE_CAPABILITY == 0) ? 0 : static_cast<uint8_t>(HW_ROTATE_ANGLE);
    if (info.rotType != GRAPHIC_BUFFER_ROTATE_0 && info.rotType != GRAPHIC_BUFFER_ROTATE_180) {
        info.width = layer->height;
        info.height = layer->width;
        info.rect.SetWidth(info.width);
        info.rect.SetHeight(info.height);
    }
#endif
    return true;
}

RotateMethod DisplayDev::GetRotateMethod() const
{
    return RotateMethod::ROTATE_METHOD_DPU_ROTATE;
}

bool DisplayDev::RotateBufferIfNecessary()
{
    return false;
}

void DisplayDev::SetRotation(LayerRotateType rotation)
{
    return;
}
} // namespace OHOS
