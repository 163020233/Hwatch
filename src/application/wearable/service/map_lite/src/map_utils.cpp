/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Create Date : 2025-11
 */

#include "map_utils.h"
#include <cstdio>
#include "common/image.h"
#include "securec.h"
#include "amap_log_adapter.h"

namespace OHOS {

static constexpr uint32_t STRIDE_ALIGN_MASK = 7;  // 8-byte alignment
static constexpr uint8_t BYTES_PER_PIXEL_1 = 1;
static constexpr uint8_t BYTES_PER_PIXEL_2 = 2;
static constexpr uint8_t BYTES_PER_PIXEL_3 = 3;
static constexpr uint8_t BYTES_PER_PIXEL_4 = 4;

// 允许产品覆写定制颜色模式
mal_pixel_mode_t getMapColorMode()
{
    // 未解决问题前，需要先使用该模式处理地图
    return MAL_PIXEL_MODE_BGRA_8888;
}

OHOS::ColorMode ConvertPixelModeToColorMode(mal_pixel_mode_t mode)
{
    switch (mode) {
        case MAL_PIXEL_MODE_ARGB_8888:
            return OHOS::ColorMode::ARGB8888;
        case MAL_PIXEL_MODE_RGBA_8888:
        case MAL_PIXEL_MODE_ABGR_8888:
        case MAL_PIXEL_MODE_BGRA_8888:
            // 注意：这些格式在 SoftEngine 中可能不被原生支持
            // 最安全做法是转换为 ARGB8888，此处先按 ARGB 处理
            return OHOS::ColorMode::ARGB8888;
        case MAL_PIXEL_MODE_RGB_565:
            return OHOS::ColorMode::RGB565;
        case MAL_PIXEL_MODE_RGB_888:
            return OHOS::ColorMode::RGB888;
        case MAL_PIXEL_MODE_GREY:
            return OHOS::ColorMode::L8;
        default:
            return OHOS::ColorMode::ARGB8888;
    }
}

mal_pixel_mode_t ConventColorModeToPixelMode(OHOS::ColorMode mode)
{
    switch (mode) {
        case OHOS::ARGB8888:
            return MAL_PIXEL_MODE_ARGB_8888;
        case OHOS::RGB888:
            return MAL_PIXEL_MODE_RGB_888;
        case OHOS::RGB565:
            return MAL_PIXEL_MODE_RGB_565;
        case OHOS::ARGB1555:
        case OHOS::ARGB4444:
        case OHOS::AL44:
        case OHOS::AL88:
            // 这些模式没有直接对应的MAL_PIXEL_MODE，考虑转换为灰度图或其他适合的格式
            return MAL_PIXEL_MODE_GREY;
        case OHOS::L1:
        case OHOS::L2:
        case OHOS::L4:
        case OHOS::L8:
            return MAL_PIXEL_MODE_GREY;
        case OHOS::A1:
        case OHOS::A2:
        case OHOS::A4:
        case OHOS::A8:
            // Alpha通道模式，这里简单地转换为灰度图
            return MAL_PIXEL_MODE_GREY;
        case OHOS::TSC6:
        case OHOS::TSC6A:
            // 特殊颜色模式，可能需要特殊处理或转换为默认格式
            return MAL_PIXEL_MODE_GREY;
        case OHOS::UNKNOWN:
        default:
            // 未知或不支持的颜色模式，返回默认值或抛出错误
            return MAL_PIXEL_MODE_GREY;  // 或者其他默认值
    }
}

// 获取每像素字节数
uint8_t GetBytesPerPixelByMalMode(mal_pixel_mode_t mode)
{
    switch (mode) {
        case MAL_PIXEL_MODE_GREY:
        case MAL_PIXEL_MODE_RGB_332:
        case MAL_PIXEL_MODE_BGR_233:
            return BYTES_PER_PIXEL_1;
        case MAL_PIXEL_MODE_RGB_565:
        case MAL_PIXEL_MODE_BGR_565:
            return BYTES_PER_PIXEL_2;
        case MAL_PIXEL_MODE_RGB_888:
        case MAL_PIXEL_MODE_BGR_888:
            return BYTES_PER_PIXEL_3;
        case MAL_PIXEL_MODE_ARGB_8888:
        case MAL_PIXEL_MODE_RGBA_8888:
        case MAL_PIXEL_MODE_ABGR_8888:
        case MAL_PIXEL_MODE_BGRA_8888:
            return BYTES_PER_PIXEL_4;
        default:
            return BYTES_PER_PIXEL_4;
    }
}

bool HasAlpha(mal_pixel_mode_t mode)
{
    switch (mode) {
        case MAL_PIXEL_MODE_ARGB_8888:
        case MAL_PIXEL_MODE_RGBA_8888:
        case MAL_PIXEL_MODE_ABGR_8888:
        case MAL_PIXEL_MODE_BGRA_8888:
            return true;
        default:
            return false;
    }
}

static const OHOS::ImageInfo *LoadAndValidateImage(const char *fileName, OHOS::Image **outImage)
{
    if (!fileName || !outImage) {
        return nullptr;
    }
    *outImage = new (std::nothrow) OHOS::Image();
    if (!*outImage || !(*outImage)->SetSrc(fileName)) {
        delete *outImage;
        *outImage = nullptr;
        return nullptr;
    }
    const OHOS::ImageInfo *info = (*outImage)->GetImageInfo();
    if (!info || !info->data || info->dataSize == 0 ||
        info->header.width == 0 || info->header.height == 0) {
        delete *outImage;
        *outImage = nullptr;
        return nullptr;
    }
    AMAP_LOGI("LoadTexture with GetImageInfo %p and colormode %d\n", info, info->header.colorMode);
    return info;
}

static uint8_t *CopyWithStride(const uint8_t *src, uint32_t width, uint32_t height,
                                uint32_t bytesPerPixel, uint32_t stride)
{
    uint32_t rowBytes = width * bytesPerPixel;
    uint8_t *dst = static_cast<uint8_t *>(UIMalloc(stride * height));
    if (!dst) {
        return nullptr;
    }
    for (uint32_t y = 0; y < height; ++y) {
        memcpy_s(dst + y * stride, rowBytes, src + y * rowBytes, rowBytes);
    }
    return dst;
}

bool LoadTexture(const char *fileName, mal_map_texture_data_t *texture)
{
    AMAP_LOGI("LoadTexture with fileName: %s, texture %p\n", fileName, texture);
    if (!fileName || !texture) {
        return false;
    }
    OHOS::Image *image = nullptr;
    const OHOS::ImageInfo *imgInfo = LoadAndValidateImage(fileName, &image);
    if (!imgInfo) {
        return false;
    }
    uint32_t width = imgInfo->header.width;
    uint32_t height = imgInfo->header.height;
    mal_pixel_mode_t colorMode =
        ConventColorModeToPixelMode(static_cast<OHOS::ColorMode>(imgInfo->header.colorMode));
    uint32_t bytesPerPixel = GetBytesPerPixelByMalMode(colorMode);
    AMAP_LOGI("LoadTexture with colorMode %d,  bytesPerPixel %d\n", colorMode, bytesPerPixel);
    uint32_t stride = (width * bytesPerPixel + STRIDE_ALIGN_MASK) & ~STRIDE_ALIGN_MASK;

    uint8_t *buffer = CopyWithStride(imgInfo->data, width, height, bytesPerPixel, stride);
    delete image;
    if (!buffer) {
        return false;
    }
    texture->pixel_mode = colorMode;
    texture->buffer = buffer;
    texture->buffer_size = stride * height;
    texture->width = width;
    texture->height = height;
    texture->stride = bytesPerPixel;
    texture->pre_multiplied = false;
    return true;
}

}  // namespace OHOS
