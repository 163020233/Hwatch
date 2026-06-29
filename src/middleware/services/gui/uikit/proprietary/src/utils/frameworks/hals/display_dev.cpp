/*
 * Copyright (c) 2022 HiSilicon (Shanghai) Technologies Co., Ltd. All rights reserved
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
#include "dfx/dfx_frame_trace.h"
#include "display_type.h"
#include "draw/hw_draw_utils.h"
#include "engines/gfx/lite_m_gfx_engine.h"
#include "gfx_utils/color.h"
#include "gfx_utils/graphic_log.h"
#include "gfx_utils/graphic_math.h"
#include "gfx_utils/mem_api.h"
#include "graphic_config.h"
#include "hals/gralloc_engines.h"
#include "imgdecode/image_load.h"

#ifndef _WIN32
#include "tcxo.h"
#ifndef QSPI_DISPLAY
#include "soc_mipi_tx.h"
#endif
#ifdef __cplusplus
extern "C" {
#endif
#ifdef __cplusplus
};
#endif
#endif

namespace OHOS {
#define ALIGN_DOWN(byte, align) ((byte) - ((byte) % (align)))
DisplayDev::DisplayDev() : isDisplayDevInited_(false), devId_(0), layerFuncs_(nullptr), exitUlps_(false)
{
    for (uint8_t i = LAYER_0; i < LAYER_MAX; i++) {
        currentBufferId_[i] = 0;
        layer_[i].layerHdl = 0;
        (void)memset_s(&layer_[i].layerInfo, sizeof(LayerInfo), 0, sizeof(LayerInfo));
        layer_[i].bufferCount = 0;
        layer_[i].buffer = nullptr;
#if defined(ROTATE_METHOD) && (ROTATE_METHOD == 0)
        (void)memset_s(&layer_[i].rotateInfo, sizeof(LayerInfo), 0, sizeof(LayerInfo));
        layer_[i].rotate = nullptr;
#endif
        layer_[i].rotateType = LayerRotateType::LAYER_ROTATE_NONE;
    }
}

void DisplayDev::LcdFlush(uint8_t layerId)
{
    if (layerId < LAYER_0 || layerId > LAYER_MAX) {
        GRAPHIC_LOGE("Invalid layerId[%u].", layerId);
        return;
    }

    if (layerFuncs_->Flush != nullptr) {
        if (layerId == LAYER_MAX) {
            TryExitUlps(true);
            if (layerFuncs_->Flush(0, 0, nullptr) != DISPLAY_SUCCESS) {
                GRAPHIC_LOGE("flush failed.");
            }
            exitUlps_ = false;
            return;
        }

        if (layer_[layerId].bufferCount == 0) {
            GRAPHIC_LOGE("flush failed. %u has no buffer", layerId);
            return;
        }
        LayerBuffer *buffer = &layer_[layerId].buffer[currentBufferId_[layerId]];
        SET_TRACE_FLUSH_TYPE(!((buffer->data.phyAddr >= PSRAM_BASE_ADDR) &&
                               (buffer->data.phyAddr < PSRAM_BASE_ADDR + PSRAM_LENTH)));
        if (layerFuncs_->Flush(devId_, layer_[layerId].layerHdl, buffer) != DISPLAY_SUCCESS) {
            GRAPHIC_LOGE("flush failed.");
            return;
        }
    }
}

void DisplayDev::WaitForVBlank()
{
    if (layerFuncs_->WaitForVBlank != nullptr) {
        if (layerFuncs_->WaitForVBlank(devId_, layer_[LAYER_0].layerHdl, 0) != DISPLAY_SUCCESS) {
            GRAPHIC_LOGE("WaitForVBlank failed.");
            return;
        }
    }
}

LayerRotateType DisplayDev::GetLayerRotateType(uint8_t layerId)
{
    if (layerId < LAYER_0 || layerId >= LAYER_MAX) {
        GRAPHIC_LOGE("Invalid layerId[%u].", layerId);
        return LayerRotateType::LAYER_ROTATE_BUTT;
    }

    if (layer_[layerId].bufferCount == 0) {
        GRAPHIC_LOGE("layer[%u] is not opened.", layerId);
        return LayerRotateType::LAYER_ROTATE_BUTT;
    }
    return layer_[layerId].rotateType;
}

LayerBuffer* DisplayDev::GetCurrentBuffer(uint8_t layerId)
{
    if (layerId < LAYER_0 || layerId >= LAYER_MAX) {
        GRAPHIC_LOGE("Invalid layerId[%u].", layerId);
        return nullptr;
    }
    if (layer_[layerId].bufferCount == 0) {
        GRAPHIC_LOGE("layer[%u] is not opened.", layerId);
        return nullptr;
    }
#if defined(ROTATE_METHOD) && (ROTATE_METHOD == 0)
    return &layer_[layerId].rotate[currentBufferId_[layerId]];
#else
    return &layer_[layerId].buffer[currentBufferId_[layerId]];
#endif
}

LayerBuffer* DisplayDev::GetNextBuffer(uint8_t layerId)
{
    if (layerId < LAYER_0 || layerId >= LAYER_MAX) {
        GRAPHIC_LOGE("Invalid layerId[%u].", layerId);
        return nullptr;
    }
    if (layer_[layerId].bufferCount == 0) {
        GRAPHIC_LOGE("layer[%u] is not opened.", layerId);
        return nullptr;
    }
    uint8_t currentBufferId = currentBufferId_[layerId];
    uint8_t nextBufferId;
    if (currentBufferId + 1 < layer_[layerId].bufferCount) {
        nextBufferId = currentBufferId + 1;
    } else {
        nextBufferId = 0;
    }
#if defined(ROTATE_METHOD) && (ROTATE_METHOD == 0)
    return &layer_[layerId].rotate[nextBufferId];
#else
    return &layer_[layerId].buffer[nextBufferId];
#endif
}

void DisplayDev::GetRotatePosition(int32_t width, int32_t height, int32_t &x, int32_t &y, LayerRotateType type) const
{
    int32_t outX = x;
    int32_t outY = y;
    switch (type) {
        case LayerRotateType::LAYER_ROTATE_90: {
            outX = height - y;
            outY = x;
            break;
        }
        case LayerRotateType::LAYER_ROTATE_180: {
            outX = width - x;
            outY = height - y;
            break;
        }
        case LayerRotateType::LAYER_ROTATE_270: {
            outX = y;
            outY = width - x;
            break;
        }
        default:
            return;
    }
    x = outX;
    y = outY;
}

void DisplayDev::GetRotateRect(int32_t width, int32_t height, Rect &rect, LayerRotateType type) const
{
#if defined(ROTATE_METHOD) && (ROTATE_METHOD == 2)
    return;
#endif
    int16_t x = rect.GetX();
    int16_t y = rect.GetY();
    int16_t x1 = rect.GetX();
    int16_t y1 = rect.GetY();
    int16_t w1 = rect.GetWidth();
    int16_t h1 = rect.GetHeight();
    switch (type) {
        case LayerRotateType::LAYER_ROTATE_90:
            x1 = height - y - h1;
            y1 = x;
            rect.SetPosition(x1, y1);
            rect.SetWidth(h1);
            rect.SetHeight(w1);
            break;
        case LayerRotateType::LAYER_ROTATE_180:
            x1 = width - x - w1;
            y1 = height - y - h1;
            rect.SetPosition(x1, y1);
            rect.SetWidth(w1);
            rect.SetHeight(h1);
            break;
        case LayerRotateType::LAYER_ROTATE_270:
            x1 = y;
            y1 = width - x - w1;
            rect.SetPosition(x1, y1);
            rect.SetWidth(h1);
            rect.SetHeight(w1);
            break;
        default:
            if (type != LayerRotateType::LAYER_ROTATE_NONE) {
                GRAPHIC_LOGE("error rotType! %u", type);
            }
    }
}

bool DisplayDev::SwapBuffer(uint8_t layerId)
{
    if (layerId < LAYER_0 || layerId >= LAYER_MAX) {
        GRAPHIC_LOGE("Invalid layerId[%u].", layerId);
        return false;
    }
    if (layer_[layerId].bufferCount < 2) { // 2: buffer count
        GRAPHIC_LOGE("layer[%u] is not opened or only has 1 buffer.", layerId);
        return false;
    }
    uint8_t currentBufferId = currentBufferId_[layerId];
    if (currentBufferId + 1 < layer_[layerId].bufferCount) {
        currentBufferId_[layerId] = currentBufferId + 1;
    } else {
        currentBufferId_[layerId] = 0;
    }
    return true;
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
    uint32_t bitPerPixel = 0;
    uint32_t stride = 0;
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
        case PIXEL_FMT_YCBCR_420_SP:
            bitPerPixel = 8; /* 8 bits */
            break;
        default:
            GRAPHIC_LOGE("unsupported PixelFormat: %d, just return 0.", fmt);
            return 0;
    }

    switch (cmpMode) {
        case COMPRESS_MODE_NONE:
            stride = ALIGN_BYTE(width, BYTE_ALIGNMENT) * bitPerPixel / STRIDE_DIVIDER; /* 8,16 bits */
            break;
        case COMPRESS_MODE_HFBC_2X:
            stride = ALIGN_BYTE(width, 4) / 4 * ALIGN_BYTE(4 * 4 * bitPerPixel / 8 / 2, 8); /* 4,8,16 bits,2:compress */
            break;
        case COMPRESS_MODE_HFBC_3X:
            stride = ALIGN_BYTE(width, 4) / 4 * ALIGN_BYTE(4 * 4 * bitPerPixel / 8 / 3, 8); /* 4,8,16 bits,3:compress */
            break;
        case COMPRESS_MODE_HFBC:
        case COMPRESS_MODE_HFBC_ABYPASS:
            stride = GetAlignedStride(width, fmt, cmpMode);
            break;
        case COMPRESS_MODE_HIMC:
            stride = 0; // stride will be set after cmp header is parsed.
            break;
        default:
            GRAPHIC_LOGE("unsupported compress mode: %d, just return 0.", cmpMode);
            return 0;
    }

    return stride;
}

uint32_t DisplayDev::CalcSize(uint32_t stride, uint32_t height, uint8_t cmpMode)
{
    uint32_t size = 0;
    switch (cmpMode) {
        case COMPRESS_MODE_NONE:
            size = stride * height;
            break;
        case COMPRESS_MODE_HFBC_2X:
        case COMPRESS_MODE_HFBC_3X:
            size = stride * ALIGN_BYTE(height, 4) / 4; // 4: Alignment length
            break;
        default:
            GRAPHIC_LOGE("unsupported compress mode: %d, just return 0.", cmpMode);
            return 0;
    }

    return size;
}

bool DisplayDev::OpenLayer(uint8_t layerId, const LayerInfo& info, uint8_t bufferCount)
{
    if (layerId < LAYER_0 || layerId >= LAYER_MAX || bufferCount == 0 || bufferCount > BUFFER_MAX) {
        GRAPHIC_LOGE("Invalid layerId[%d] or invalid bufferCount[%d].", layerId, bufferCount);
        return false;
    }

    if (layer_[layerId].bufferCount != 0) {
        GRAPHIC_LOGD("layer[%u] is already opened with bufferCount[%u].", layerId, layer_[layerId].bufferCount);
        return false;
    }

    if (layerFuncs_->CreateLayer == nullptr) {
        GRAPHIC_LOGE("OpenLayer is nullptr");
        return false;
    }

    uint32_t size = bufferCount * sizeof(LayerBuffer);
    layer_[layerId].buffer = static_cast<LayerBuffer*>(UIMalloc(size));
    if (layer_[layerId].buffer == nullptr) {
        GRAPHIC_LOGE("new LayerBuffer failed.");
        return false;
    }
    (void)memset_s(layer_[layerId].buffer, size, 0, size);

#if defined(ROTATE_METHOD) && (ROTATE_METHOD == 0)
    layer_[layerId].rotate = static_cast<LayerBuffer*>(UIMalloc(size));
    if (layer_[layerId].rotate == nullptr) {
        GRAPHIC_LOGE("new rotate layerBuffer failed.");
        UIFree(layer_[layerId].buffer);
        layer_[layerId].buffer = nullptr;
        return false;
    }
    layer_[layerId].rotateInfo = info;
#if (HW_ROTATE_ANGLE == 1 || HW_ROTATE_ANGLE == 3)
    layer_[layerId].rotateInfo.height = info.width;
    layer_[layerId].rotateInfo.width = info.height;
#endif
#endif

    if (layerFuncs_->CreateLayer(devId_, &info, &layer_[layerId].layerHdl) != DISPLAY_SUCCESS) {
        UIFree(layer_[layerId].buffer);
        layer_[layerId].buffer = nullptr;
#if defined(ROTATE_METHOD) && (ROTATE_METHOD == 0)
        UIFree(layer_[layerId].rotate);
        layer_[layerId].rotate = nullptr;
#endif
        GRAPHIC_LOGE("OpenLayer failed. LayerId: %u.", layerId);
        return false;
    }

    layer_[layerId].layerInfo = info;
    layer_[layerId].bufferCount = bufferCount;
    layer_[layerId].rotateType = static_cast<LayerRotateType>(HW_ROTATE_ANGLE);
    layer_[layerId].pos.x = 0;
    layer_[layerId].pos.y = 0;
#if defined(ROTATE_METHOD) && (ROTATE_METHOD == 2)
    if (layerFuncs_->SetTransformMode(devId_, layerId, static_cast<TransformType>(layer_[layerId].rotateType)) !=
        DISPLAY_SUCCESS) {
        GRAPHIC_LOGE("failed to init transmode %d", static_cast<int>(layer_[layerId].rotateType));
    }
#if defined(HW_ROTATE_ANGLE) && ((HW_ROTATE_ANGLE == 1) || (HW_ROTATE_ANGLE == 3))
    layer_[layerId].layerInfo.width = LAYER_RESOLUTION_HEIGHT;
    layer_[layerId].layerInfo.height = LAYER_RESOLUTION_WIDTH;
#endif
#endif
    return true;
}

void DisplayDev::CloseLayer(uint8_t layerId)
{
    if (layerId < LAYER_0 || layerId >= LAYER_MAX) {
        GRAPHIC_LOGE("Invalid layerId[%u].", layerId);
        return;
    }

    if (layer_[layerId].bufferCount == 0) {
        GRAPHIC_LOGE("layer[%u] is already closed.", layerId);
        return;
    }

    if (layerFuncs_->CloseLayer == nullptr) {
        GRAPHIC_LOGE("CloseLayer is null.");
        return;
    }

    if (layerFuncs_->CloseLayer(devId_, layer_[layerId].layerHdl) != DISPLAY_SUCCESS) {
        GRAPHIC_LOGE("CloseLayer %u failed.", layerId);
        return;
    }

    UIFree(layer_[layerId].buffer);
    layer_[layerId].buffer = nullptr;

#if defined(ROTATE_METHOD) && (ROTATE_METHOD == 0)
    if (layer_[layerId].rotate != nullptr) {
        UIFree(layer_[layerId].rotate);
        layer_[layerId].rotate = nullptr;
    }
    (void)memset_s(&layer_[layerId].rotateInfo, sizeof(LayerInfo), 0, sizeof(LayerInfo));
#endif

    layer_[layerId].layerHdl = 0;
    layer_[layerId].bufferCount = 0;
    layer_[layerId].rotateType = LayerRotateType::LAYER_ROTATE_NONE;
    layer_[layerId].pos.x = 0;
    layer_[layerId].pos.y = 0;
    (void)memset_s(&layer_[layerId].layerInfo, sizeof(LayerInfo), 0, sizeof(LayerInfo));
}

inline static void ClearLayerBuffer(LayerBuffer& buffer)
{
    buffer.fenceId = 0;
    buffer.width = 0;
    buffer.height = 0;
    buffer.pitch = 0;
    buffer.pixFormat = PIXEL_FMT_BUTT;
    buffer.data.phyAddr = 0;
    buffer.data.virAddr = nullptr;
}

bool DisplayDev::IsUlpsEnabled(void)
{
#if ENABLE_ULPS
    return true;
#else
    return false;
#endif
}

bool DisplayDev::TryExitUlps(bool isFlushing)
{
    if (IsUlpsEnabled() && !exitUlps_) {
#if !defined(_WIN32) && !defined(QSPI_DISPLAY)
        uapi_mipi_tx_exit_ulps();
        if (isFlushing) {
            uapi_tcxo_delay_ms(1);
        }
#endif
        exitUlps_ = true;
        return true;
    }
    return false;
}

bool DisplayDev::IsDoubleBuffer(void)
{
#if DOUBLE_BUFFER
    return true;
#else
    return false;
#endif
}

bool DisplayDev::AllocBuffer(LayerBuffer *layerBuffer, const LayerInfo &layerInfo, uint8_t count, bool isFb)
{
    if (layerBuffer == nullptr || count == 0) {
        return false;
    }

#if ENABLE_FB_COMPRESS
    CompressMode cmpMode = COMPRESS_MODE_HFBC_2X;
#else
    CompressMode cmpMode = COMPRESS_MODE_NONE;
#endif

    uint32_t stride = CalcStride(layerInfo.width, layerInfo.pixFormat, cmpMode);
    if (stride == 0) {
        return false;
    }

    uint8_t index = 0;
    while (index < count) {
        AllocInfo info;
        if (isFb) {
#if ENABLE_GMMU
            info.usage = HBM_USE_MEM_FB;
#else
            info.expectedSize = CalcSize(stride, layerInfo.height, cmpMode);
            info.usage = HBM_USE_ASSIGN_SIZE | HBM_USE_MEM_FB;
#endif
        } else {
            info.usage = HBM_USE_ASSIGN_SIZE;
            info.expectedSize = CalcSize(stride, layerInfo.height, cmpMode);
        }

        GrallocBuffer buffer;
        if (!GrallocEngines::GetInstance()->AllocBuffer(info, buffer)) {
            GRAPHIC_LOGE("AllocMem failed.");
            FreeBuffer(layerBuffer, index);
            return false;
        }

        layerBuffer[index].data.virAddr = buffer.virAddr;
        layerBuffer[index].data.phyAddr = static_cast<uint64_t>(uintptr_t(buffer.virAddr));
        layerBuffer[index].width = layerInfo.width;
        layerBuffer[index].height = layerInfo.height;
        if (isFb) {
#if ENABLE_GMMU
            layerBuffer[index].pitch = static_cast<int32_t>(buffer.stride);
#else
            layerBuffer[index].pitch = (int32_t)stride;
#endif
        } else {
            layerBuffer[index].pitch = (int32_t)stride;
        }
        layerBuffer[index].pixFormat = layerInfo.pixFormat;
        ++index;
    }

    return true;
}

void DisplayDev::FreeBuffer(LayerBuffer *buffer, uint8_t count)
{
    if (buffer == nullptr || count == 0) {
        return;
    }

    for (uint8_t i = 0; i < count; i++) {
        GrallocEngines::GetInstance()->FreeBuffer(reinterpret_cast<uint8_t*>(buffer[i].data.virAddr));
        ClearLayerBuffer(buffer[i]);
    }
}

bool DisplayDev::AllocLayerBuffer(uint8_t layerId)
{
    if (layerId < LAYER_0 || layerId >= LAYER_MAX || layer_[layerId].bufferCount == 0) {
        GRAPHIC_LOGE("Invalid layerId[%u] or layer is not opened.", layerId);
        return false;
    }

    if (layer_[layerId].buffer[0].data.virAddr != nullptr) {
        GRAPHIC_LOGE("layer[%u] : Memory of buffers is already allocated.", layerId);
        return false;
    }

    if (!AllocBuffer(layer_[layerId].buffer, layer_[layerId].layerInfo, layer_[layerId].bufferCount)) {
        return false;
    }

#if defined(ROTATE_METHOD) && (ROTATE_METHOD == 0)
    if (layer_[layerId].rotate != nullptr) {
        if (!AllocBuffer(layer_[layerId].rotate, layer_[layerId].rotateInfo, layer_[layerId].bufferCount, false)) {
            FreeBuffer(layer_[layerId].buffer, layer_[layerId].bufferCount);
            return false;
        }
    }
#endif // defined(ROTATE_METHOD) && (ROTATE_METHOD == 0)
    return true;
}

void DisplayDev::FreeLayerBuffer(uint8_t layerId)
{
    if (layerId < LAYER_0 || layerId >= LAYER_MAX || layer_[layerId].bufferCount == 0) {
        GRAPHIC_LOGE("Invalid layerId[%u] or layer is not opened.", layerId);
        return;
    }

    if (layer_[layerId].buffer[0].data.virAddr == nullptr) {
        GRAPHIC_LOGE("layer[%u] : Memory is already released.", layerId);
        return;
    }

    FreeBuffer(layer_[layerId].buffer, layer_[layerId].bufferCount);
#if defined(ROTATE_METHOD) && (ROTATE_METHOD == 0)
    if (layer_[layerId].rotate != nullptr) {
        FreeBuffer(layer_[layerId].rotate, layer_[layerId].bufferCount);
    }
#endif // defined(ROTATE_METHOD) && (ROTATE_METHOD == 0)
}

void DisplayDev::SetLayerVisible(uint8_t layerId, bool visible)
{
    if (layerId < LAYER_0 || layerId >= LAYER_MAX) {
        GRAPHIC_LOGE("Invalid layerId[%u].", layerId);
        return;
    }

    if (layerFuncs_->SetLayerVisible != nullptr) {
        if (layerFuncs_->SetLayerVisible(devId_, layer_[layerId].layerHdl, visible) != DISPLAY_SUCCESS) {
            GRAPHIC_LOGE("SetLayerVisible failed. LayerId: %u, visible: %d.", layerId, visible);
        }
    }
}

void DisplayDev::SetLayerDirtyRegion(uint8_t layerId, const Rect& rect)
{
    if (layerId < LAYER_0 || layerId >= LAYER_MAX) {
        GRAPHIC_LOGE("Invalid layerId[%u].", layerId);
        return;
    }

    if (layerFuncs_->SetLayerDirtyRegion != nullptr) {
        FRAME_TRACE_SET_RECT(rect);
        Rect outRect = rect;
        if (layer_[layerId].rotateType == LayerRotateType::LAYER_ROTATE_NONE ||
            layer_[layerId].rotateType == LayerRotateType::LAYER_ROTATE_180) {
            GetRotateRect(layer_[layerId].layerInfo.width, layer_[layerId].layerInfo.height,
                outRect, layer_[layerId].rotateType);
        } else {
            GetRotateRect(layer_[layerId].layerInfo.height, layer_[layerId].layerInfo.width,
                outRect, layer_[layerId].rotateType);
        }
        IRect iRect = {outRect.GetX(), outRect.GetY(), outRect.GetWidth(), outRect.GetHeight()};
        if (layerFuncs_->SetLayerDirtyRegion(devId_, layer_[layerId].layerHdl, &iRect) !=
            DISPLAY_SUCCESS) {
            GRAPHIC_LOGE("SetLayerDirtyRegion failed. LayerId: %u.", layerId);
        }
    }
}

void DisplayDev::SetLayerColorKey(uint8_t layerId, bool enColorKey, uint32_t color)
{
    if (layerId < LAYER_0 || layerId >= LAYER_MAX) {
        GRAPHIC_LOGE("Invalid layerId[%u].", layerId);
        return;
    }

    if (layerFuncs_->SetLayerColorKey != nullptr) {
        if (layerFuncs_->SetLayerColorKey(devId_, layer_[layerId].layerHdl, enColorKey, color) !=
            DISPLAY_SUCCESS) {
            GRAPHIC_LOGE("SetLayerColorKey failed. LayerId: %u, enColorKey: %d.", layerId, enColorKey);
        }
    }
}

void DisplayDev::SetLayerZOrder(uint8_t layerId, uint8_t order)
{
    if (layerId < LAYER_0 || layerId >= LAYER_MAX) {
        GRAPHIC_LOGE("Invalid layerId[%u].", layerId);
        return;
    }

    if (layerFuncs_->SetLayerZorder != nullptr) {
        if (layerFuncs_->SetLayerZorder(devId_, layer_[layerId].layerHdl, order) !=
            DISPLAY_SUCCESS) {
            GRAPHIC_LOGE("SetLayerZOrder failed. LayerId: %u, order: %u.", layerId, order);
        }
    }
}

void DisplayDev::SetLayerPositon(uint8_t layerId, int16_t x, int16_t y)
{
    if (layerId < LAYER_0 || layerId >= LAYER_MAX) {
        GRAPHIC_LOGE("Invalid layerId[%u].", layerId);
        return;
    }

    layer_[layerId].pos.x = x;
    layer_[layerId].pos.y = y;
    if (layerFuncs_->SetLayerSize != nullptr) {
        IRect rect = {x, y, 0, 0};
        if (layerFuncs_->SetLayerSize(devId_, layer_[layerId].layerHdl, &rect) != DISPLAY_SUCCESS) {
            GRAPHIC_LOGE("SetLayerSize failed.");
        }
    }
}

void DisplayDev::SetHardwareLayerBuffer(uint8_t layerId, uint8_t bufferId, const LayerBuffer& layer)
{
    if (layerId < LAYER_0 || layerId >= LAYER_MAX || bufferId >= layer_[layerId].bufferCount) {
        GRAPHIC_LOGE("Invalid layerId[%u] or invliad bufferId[%u].", layerId, bufferId);
        return;
    }

    if (layerId == LAYER_0) {
        GRAPHIC_LOGE("Not allowed to set LAYER_0.");
    } else {
        layer_[layerId].buffer[bufferId].data.virAddr = layer.data.virAddr;
        layer_[layerId].buffer[bufferId].data.phyAddr = layer.data.phyAddr;
        layer_[layerId].buffer[bufferId].width = layer.width;
        layer_[layerId].buffer[bufferId].height = layer.height;
        layer_[layerId].buffer[bufferId].pitch = layer.pitch;
        layer_[layerId].buffer[bufferId].pixFormat = layer.pixFormat;
    }
}

void DisplayDev::DisplayDevInit()
{
    if (isDisplayDevInited_) {
        GRAPHIC_LOGE("DisplayDev is already inited.");
        return;
    }

    if (LayerInitialize(&layerFuncs_) != DISPLAY_SUCCESS) {
        GRAPHIC_LOGE("layer initialize failed.");
        return;
    }

    if (layerFuncs_->InitDisplay == nullptr || layerFuncs_->InitDisplay(devId_) != DISPLAY_SUCCESS) {
        GRAPHIC_LOGE("InitDisplay is nullptr or InitDisplay failed.");
        return;
    }

    LayerInfo layerInfo;
    layerInfo.width = LAYER_RESOLUTION_WIDTH;
    layerInfo.height = LAYER_RESOLUTION_HEIGHT;
    layerInfo.pixFormat = layerPixFmt_;
    layerInfo.bpp = layerBpp_;
    layerInfo.type = LAYER_TYPE_GRAPHIC;

    uint8_t bufferCount = IsDoubleBuffer() ? 2 : 1;
    if (!OpenLayer(LAYER_0, layerInfo, bufferCount)) {
        GRAPHIC_LOGE("OpenLayer failed.");
        return;
    }

    if (!AllocLayerBuffer(LAYER_0)) {
        CloseLayer(LAYER_0);
        GRAPHIC_LOGE("AllocLayerBuffer failed.");
        return;
    }

    IRect dirtyRect = {0, 0, layer_[LAYER_0].layerInfo.width, layer_[LAYER_0].layerInfo.height};
    if ((layerFuncs_->SetLayerDirtyRegion != nullptr) &&
        (layerFuncs_->SetLayerDirtyRegion(devId_, layer_[LAYER_0].layerHdl, &dirtyRect) != DISPLAY_SUCCESS)) {
        GRAPHIC_LOGE("SetLayerDirtyRegion failed.");
    }
    isDisplayDevInited_ = true;
}

void DisplayDev::DisplayDevClose()
{
    if (layerFuncs_ == nullptr || layerFuncs_->CloseLayer == nullptr || layerFuncs_->DeinitDisplay == nullptr) {
        return;
    }

    FreeLayerBuffer(LAYER_0);
    CloseLayer(LAYER_0);
    if (layerFuncs_->DeinitDisplay(devId_) != DISPLAY_SUCCESS) {
        GRAPHIC_LOGE("DeinitDisplay failed.");
        return;
    }
    if (LayerUninitialize(layerFuncs_) != DISPLAY_SUCCESS) {
        GRAPHIC_LOGE("LayerUninitialize failed.");
        return;
    }
    isDisplayDevInited_ = false;
}

bool DisplayDev::GetCurrentBufferInfo(uint8_t layerId, BufferInfo& info)
{
    LayerBuffer* layer = GetCurrentBuffer(layerId);
    if (layer == nullptr) {
        return false;
    }

    Rect rect = {0, 0, static_cast<short>(layer->width - 1), static_cast<short>(layer->height - 1)};
    info.rect = rect;
    info.stride = static_cast<uint32_t>(layer->pitch);
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
    info.phyAddr = reinterpret_cast<void*>((uintptr_t)layer->data.phyAddr);
    info.rotType = 0;

#if defined(ROTATE_METHOD) && (ROTATE_METHOD == 1)
    info.rotType = static_cast<uint8_t>(HW_ROTATE_ANGLE);
    if ((HW_ROTATE_ANGLE != GRAPHIC_BUFFER_ROTATE_0) && (HW_ROTATE_ANGLE != GRAPHIC_BUFFER_ROTATE_180)) {
        info.width = layer->height;
        info.height = layer->width;
        info.rect.SetWidth(info.width);
        info.rect.SetHeight(info.height);
    }
#endif
    return true;
}

#if (defined(ROTATE_METHOD) && (ROTATE_METHOD == 0))
static void ConverLayerBuf2BufInfo(LayerBuffer* layerBuf, BufferInfo& info)
{
    if (layerBuf == nullptr) {
        return;
    }

    info.rect = {0, 0, static_cast<int16_t>(layerBuf->width - 1), static_cast<int16_t>(layerBuf->height - 1)};
    info.virAddr = layerBuf->data.virAddr;
    info.phyAddr = info.virAddr;
    info.stride = static_cast<uint32_t>(layerBuf->pitch);
    info.mode = HWDrawUtils::ConvertPixelFmt2ColorMode(layerBuf->pixFormat);
    info.color = 0;
    info.width = layerBuf->width;
    info.height = layerBuf->height;
#if ENABLE_FB_COMPRESS
    info.compressMode = COMPRESS_MODE_HFBC_2X;
#else
    info.compressMode = COMPRESS_MODE_NONE;
#endif
    info.rotType = 0;
}
#endif

bool DisplayDev::RotateBufferIfNecessary()
{
#if (defined(ROTATE_METHOD) && (ROTATE_METHOD == 0)) && ENABLE_VGU_ENGINE
    LayerRotateType rotateType = DisplayDev::GetInstance()->GetLayerRotateType(LAYER_0);
    if ((rotateType == LayerRotateType::LAYER_ROTATE_NONE) || (rotateType == LayerRotateType::LAYER_ROTATE_BUTT)) {
        return true;
    }
    uint8_t curBufferId = currentBufferId_[LAYER_0];
    if (curBufferId >= layer_[LAYER_0].bufferCount) {
        GRAPHIC_LOGE("The current buffer ID exceeds the maximum number of IDs.");
        return false;
    }

    LayerBuffer* rotateBuffer = &layer_[LAYER_0].rotate[curBufferId];
    LayerBuffer* buffer = &layer_[LAYER_0].buffer[curBufferId];
    if (rotateBuffer == nullptr || rotateBuffer->data.virAddr == nullptr ||
        buffer == nullptr || buffer->data.virAddr == nullptr) {
        return false;
    }

    BufferInfo src;
    BufferInfo dst;
    ConverLayerBuf2BufInfo(rotateBuffer, src);
    ConverLayerBuf2BufInfo(buffer, dst);

    BlendOption blendOption;
    switch (rotateType) {
        case LayerRotateType::LAYER_ROTATE_90: {
            float rotAngle = 90;
            blendOption.transMap.Rotate(rotAngle, Vector2<float>{0, 0});
            blendOption.transMap.Translate(Vector2<int16_t>{static_cast<int16_t>(src.height), 0});
            break;
        }
        case LayerRotateType::LAYER_ROTATE_180: {
            float rotAngle = 180;
            blendOption.transMap.Rotate(rotAngle, Vector2<float>{0, 0});
            blendOption.transMap.Translate(
                Vector2<int16_t>{static_cast<int16_t>(src.width), static_cast<int16_t>(src.height)});
            break;
        }
        case LayerRotateType::LAYER_ROTATE_270: {
            float rotAngle = 270;
            blendOption.transMap.Rotate(rotAngle, Vector2<float>{0, 0});
            blendOption.transMap.Translate(Vector2<int16_t>{0, static_cast<int16_t>(src.width)});
            break;
        }
        default:
            GRAPHIC_LOGE("error rotate type %d", rotateType);
            return false;
    }

    blendOption.opacity = OPA_OPAQUE;
    blendOption.mode = BLEND_SRC_OVER;
    BaseGfxEngine::GetInstance()->Blit(dst, {0, 0}, src, dst.rect, blendOption);
#endif // (defined(ROTATE_METHOD) && (ROTATE_METHOD == 0)) && ENABLE_VGU_ENGINE
    return true;
}

RotateMethod DisplayDev::GetRotateMethod() const
{
    return static_cast<RotateMethod>(ROTATE_METHOD);
}

void DisplayDev::SetRotation(LayerRotateType rotation)
{
    TransformType type = ROTATE_NONE;
    switch (rotation) {
        case LayerRotateType::LAYER_ROTATE_NONE:
            type = ROTATE_NONE;
            break;
        case LayerRotateType::LAYER_ROTATE_90:
            type = ROTATE_90;
            break;
        case LayerRotateType::LAYER_ROTATE_180:
            type = ROTATE_180;
            break;
        case LayerRotateType::LAYER_ROTATE_270:
            type = ROTATE_270;
            break;
        default :
            GRAPHIC_LOGE("rotate type is invalid: %d", rotation);
            return;
    }

    // 旋转前保证上一帧送显完成
    LiteMGfxEngine::GetInstance()->AsyncFlush();
    int32_t width = LAYER_RESOLUTION_WIDTH;
    int32_t height = LAYER_RESOLUTION_HEIGHT;
    if (rotation == LayerRotateType::LAYER_ROTATE_90 || rotation == LayerRotateType::LAYER_ROTATE_270) {
        width = LAYER_RESOLUTION_HEIGHT;
        height = LAYER_RESOLUTION_WIDTH;
    }
    UpdateLayers(width, height, rotation);
    // 保证旋转后的第一帧是全屏刷新
    RootView::GetInstance()->SetWidth(width);
    RootView::GetInstance()->SetHeight(height);
    RootView::GetInstance()->GetChildrenHead()->SetWidth(width);
    RootView::GetInstance()->GetChildrenHead()->SetHeight(height);
    RootView::GetInstance()->GetChildrenTail()->SetWidth(width);
    RootView::GetInstance()->GetChildrenTail()->SetHeight(height);
    LiteMGfxEngine::GetInstance()->SetScreenWidth(width);
    LiteMGfxEngine::GetInstance()->SetScreenHeight(height);
    layerFuncs_->SetTransformMode(0, 0, type); // first and second param is not used.
    RootView::GetInstance()->Invalidate();
}

void DisplayDev::UpdateLayers(int32_t width, int32_t height, LayerRotateType rotation)
{
    for (uint8_t layerId = 0; layerId < LAYER_MAX; layerId++) {
        if (layer_[layerId].bufferCount == 0) {
            continue;
        }
        for (uint8_t index = 0; index < layer_[layerId].bufferCount; index++) {
            if (layer_[layerId].buffer[index].width == width &&
                layer_[layerId].buffer[index].height == height) {
                continue;
            }
            layer_[layerId].buffer[index].width = width;
            layer_[layerId].buffer[index].height = height;
#if ENABLE_FB_COMPRESS
            CompressMode cmpMode = COMPRESS_MODE_HFBC_2X;
#else
            CompressMode cmpMode = COMPRESS_MODE_NONE;
#endif
            uint32_t stride = CalcStride(width, layer_[layerId].buffer[index].pixFormat, cmpMode);
            if (stride == 0) {
                GRAPHIC_LOGE("Update layerbuffer:[%u, %u] failed. Calculated Stride is 0.", layerId, index);
            }
            layer_[layerId].buffer[index].pitch = static_cast<int32_t>(stride);
        }
        // 更新layer的旋转方向，input事件模块会获取LAYER_0的旋转方向进行坐标适配
        layer_[layerId].rotateType = rotation;
    }
}

} // namespace OHOS
