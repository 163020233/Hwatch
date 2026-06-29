/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#include "draw/draw_image.h"

#include "gfx_utils/color.h"
#include "gfx_utils/graphic_log.h"
#include "imgdecode/cache_manager.h"
#ifdef VERSION_IOT
#include "imgdecode/image_load.h"
#endif
#include "draw/hw_draw_utils.h"

namespace OHOS {
void DrawImage::DrawCommon(BufferInfo& gfxDstBuffer, const Rect& coords, const Rect& mask,
    const ImageInfo* img, const Style& style, uint8_t opaScale)
{
    if (img == nullptr) {
        return;
    }
    OpacityType opa = DrawUtils::GetMixOpacity(opaScale, style.imageOpa_);
    uint8_t pxBitSize = DrawUtils::GetPxSizeByColorMode(img->header.colorMode);
    gfxDstBuffer.color = img->color;
#ifdef VERSION_IOT
    DrawUtils::GetInstance()->DrawImage(gfxDstBuffer,
        coords,
        mask,
        img->data,
        opa,
        pxBitSize,
        static_cast<ColorMode>(img->header.colorMode),
#if defined(ROTATE_METHOD) && (ROTATE_METHOD == 1)
        // 4: bit
        img->header.compressMode | (HWDrawUtils::GetInstance()->GetBufferInfoRotType(img->header.reserved) << 4),
#else
        img->header.compressMode,
#endif
        img->phyAddr);
#else
    DrawUtils::GetInstance()->DrawImage(gfxDstBuffer, coords, mask, img->data, opa, pxBitSize,
                                        static_cast<ColorMode>(img->header.colorMode));
#endif
}

void DrawImage::DrawCommon(BufferInfo& gfxDstBuffer, const Rect& coords, const Rect& mask,
    const char* path, const Style& style, uint8_t opaScale)
{
    if (path == nullptr) {
        return;
    }
    OpacityType opa = DrawUtils::GetMixOpacity(opaScale, style.imageOpa_);

    CacheEntry entry;
    if (CacheManager::GetInstance().Open(path, style, entry) != RetCode::OK) {
        return;
    }

    uint8_t pxBitSize = DrawUtils::GetPxSizeByColorMode(entry.GetImageInfo().header.colorMode);
    if (entry.InCache()) {
#ifdef VERSION_IOT
        DrawUtils::GetInstance()->DrawImage(gfxDstBuffer,
            coords,
            mask,
            entry.GetImgData(),
            opa,
            pxBitSize,
            static_cast<ColorMode>(entry.GetImageInfo().header.colorMode),
#if defined(ROTATE_METHOD) && (ROTATE_METHOD == 1)
        // 4: bit
            entry.GetImgHeader().compressMode | (HWDrawUtils::GetInstance()->GetBufferInfoRotType(entry.GetImgHeader().reserved) << 4),
#else
            entry.GetImgHeader().compressMode,
#endif
            entry.GetImageInfo().phyAddr);
#elif defined ENABLE_GFX_ENGINES
        DrawUtils::GetInstance()->DrawImage(gfxDstBuffer, coords, mask, entry.GetImgData(), opa, pxBitSize,
                                            static_cast<ColorMode>(entry.GetImageInfo().header.colorMode),
                                            entry.GetImageInfo().phyAddr);
#else
        DrawUtils::GetInstance()->DrawImage(gfxDstBuffer, coords, mask, entry.GetImgData(), opa, pxBitSize,
                                            static_cast<ColorMode>(entry.GetImageInfo().header.colorMode));
#endif
    } else {
#ifdef VERSION_IOT
#if defined(ROTATE_METHOD) && (ROTATE_METHOD == 1)
        if (entry.GetImgHeader().compressMode != COMPRESS_MODE_NONE ||
            (entry.GetImgHeader().reserved != 0 && entry.GetImgHeader().reserved != GRAPHIC_BUFFER_ROTATE_MAGIC)) {
#else
        if (entry.GetImgHeader().compressMode != COMPRESS_MODE_NONE) {
#endif
            GRAPHIC_LOGE("Invalid Compress Mode: %d or reserved %u\n",
                entry.GetImgHeader().compressMode, entry.GetImgHeader().reserved);
            return;
        }
#endif
        Rect valid = coords;
        if (!valid.Intersect(valid, mask)) {
            return;
        }

        int16_t width = valid.GetWidth();
        if (width <= 0) {
            return;
        }
        uint8_t* buf = static_cast<uint8_t*>(UIMalloc(static_cast<uint32_t>(width) * ((COLOR_DEPTH >> SHIFT_3) + 1)));
        if (buf == nullptr) {
            return;
        }

        Rect line = valid;
        line.SetHeight(1);
        Point start;
        start.x = valid.GetLeft() - coords.GetLeft();
        start.y = valid.GetTop() - coords.GetTop();
        for (int16_t row = valid.GetTop(); row <= valid.GetBottom(); row++) {
            if (entry.ReadLine(start, width, buf) != RetCode::OK) {
                CacheManager::GetInstance().Close(path);
                UIFree(buf);
                return;
            }
#ifdef VERSION_IOT
            DrawUtils::GetInstance()->DrawImage(gfxDstBuffer,
                line,
                mask,
                buf,
                opa,
                pxBitSize,
                static_cast<ColorMode>(entry.GetImageInfo().header.colorMode),
#if defined(ROTATE_METHOD) && (ROTATE_METHOD == 1)
                entry.GetImageInfo().header.compressMode |
                    (HWDrawUtils::GetInstance()->GetBufferInfoRotType(entry.GetImageInfo().header.reserved))
                        << 4);  // 4: bit
#else
                entry.GetImageInfo().header.compressMode);
#endif
#else
            DrawUtils::GetInstance()->DrawImage(gfxDstBuffer, line, mask, buf, opa, pxBitSize,
                                                static_cast<ColorMode>(entry.GetImageInfo().header.colorMode));
#endif
            line.SetTop(line.GetTop() + 1);
            line.SetBottom(line.GetBottom() + 1);
            start.y++;
        }
        UIFree(buf);
    }
}
} // namespace OHOS
