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

#include <cmath>
#include <string>
#include "gfx_utils/file.h"
#include "gfx_utils/graphic_log.h"
#include "graphic_config.h"
#include "securec.h"
#include "font/ui_font_hw_vector.h"
#include FT_OUTLINE_H
#if ENABLE_MULTI_FONT
#include "font/ui_multi_font_manager.h"
#endif
#include "common/image_cache_manager.h"
#include "common/typed_text.h"
#include "display_vgu.h"

#define ALIGN_SIZE(size) (((size) + 15) & (~15)) // 16 byte align

namespace OHOS {
UIFontHWVector::UIFontHWVector()
{
    pathCache_ = new(std::nothrow) UIFontHWCache();
    if (pathCache_ == nullptr) {
        GRAPHIC_LOGE("new UIFontHWCache failed!");
    }
}
UIFontHWVector::~UIFontHWVector()
{
    if (pathCache_ != nullptr) {
        delete pathCache_;
    }
}

bool UIFontHWVector::TryToFreeFontCache()
{
    if (pathCache_ == nullptr) {
        return false;
    }

    return pathCache_->TryToFreeFontCache();
}

uint16_t UIFontHWVector::GetHeight(uint16_t fontId, uint8_t fontSize)
{
    FaceInfo faceInfo;
    int8_t ret = GetFaceInfo(fontId, fontSize, faceInfo);
    if (ret != RET_VALUE_OK) {
        return INVALID_RET_VALUE;
    }
    if (!freeTypeInited_ || (faceInfo.face == nullptr)) {
        return INVALID_RET_VALUE;
    }

    float scale = 1.0f * fontSize * FONT_PIXEL_IN_POINT / faceInfo.face->units_per_EM;
    return static_cast<uint16_t>(faceInfo.face->size->metrics.height * scale / FONT_PIXEL_IN_POINT);
}

int16_t UIFontHWVector::GetWidth(uint32_t unicode, uint16_t fontId, uint8_t fontSize)
{
    FaceInfo faceInfo;
    int8_t ret = GetFaceInfo(fontId, fontSize, faceInfo);
    if (ret != RET_VALUE_OK) {
        return INVALID_RET_VALUE;
    }

    if (!freeTypeInited_ || (faceInfo.face == nullptr) || (pathCache_ == nullptr)) {
        return INVALID_RET_VALUE;
    }

    GlyphPathCacheData* cache = reinterpret_cast<GlyphPathCacheData*>(pathCache_->GetCacheData(fontId, unicode));
    if (cache == nullptr) {
        cache = LoadAndCacheGlyph(faceInfo.face, fontId, unicode);
    }

    float scale = 1.0f * fontSize * FONT_PIXEL_IN_POINT / faceInfo.face->units_per_EM;
    if (cache != nullptr) {
        return static_cast<int16_t>(cache->advance * scale / FONT_PIXEL_IN_POINT);
    }

    return GetEmojiWidth(unicode, fontId, fontSize);
}

int8_t UIFontHWVector::GetFontHeader(FontHeader& fontHeader, uint16_t fontId, uint8_t fontSize)
{
    FaceInfo faceInfo;
    int8_t ret = GetFaceInfo(fontId, fontSize, faceInfo);
    if (ret != RET_VALUE_OK) {
        return INVALID_RET_VALUE;
    }

    if (!freeTypeInited_ || (faceInfo.face == nullptr) || (pathCache_ == nullptr)) {
        return INVALID_RET_VALUE;
    }

    float scale = 1.0f * fontSize * FONT_PIXEL_IN_POINT / faceInfo.face->units_per_EM;
    fontHeader.ascender = static_cast<int16_t>(faceInfo.face->size->metrics.ascender * scale / FONT_PIXEL_IN_POINT);
    fontHeader.descender = static_cast<int16_t>(faceInfo.face->size->metrics.descender * scale /
        FONT_PIXEL_IN_POINT);
    fontHeader.fontHeight = static_cast<uint16_t>(faceInfo.face->size->metrics.height * scale /
        FONT_PIXEL_IN_POINT);
    return RET_VALUE_OK;
}

int8_t UIFontHWVector::GetGlyphNode(uint32_t unicode, GlyphNode& glyphNode, uint16_t fontId, uint8_t fontSize)
{
    FaceInfo faceInfo;
    int8_t ret = GetFaceInfo(fontId, fontSize, faceInfo);
    if (ret != RET_VALUE_OK) {
        return INVALID_RET_VALUE;
    }

    if (!freeTypeInited_ || (faceInfo.face == nullptr) || (pathCache_ == nullptr)) {
        return INVALID_RET_VALUE;
    }
    GlyphPathCacheData* cache = reinterpret_cast<GlyphPathCacheData*>(pathCache_->GetCacheData(fontId, unicode));
    if (cache == nullptr) {
        cache = LoadAndCacheGlyph(faceInfo.face, fontId, unicode);
    }

    if (cache != nullptr) {
        if (cache->cmdsNum == 0) {
            return INVALID_RET_VALUE;
        }
        float scale = 1.0f * fontSize * FONT_PIXEL_IN_POINT / faceInfo.face->units_per_EM;
        glyphNode.unicode = unicode;
        glyphNode.left = static_cast<int16_t>(cache->left * scale / FONT_PIXEL_IN_POINT);
        glyphNode.top = static_cast<int16_t>(cache->top * scale / FONT_PIXEL_IN_POINT);
        glyphNode.cols = static_cast<uint16_t>(cache->width * scale / FONT_PIXEL_IN_POINT);
        glyphNode.rows = static_cast<uint16_t>(cache->height * scale / FONT_PIXEL_IN_POINT);
        glyphNode.advance = static_cast<uint16_t>(cache->advance * scale / FONT_PIXEL_IN_POINT);
        return RET_VALUE_OK;
    }

    return INVALID_RET_VALUE;
}

static int MoveTo(const FT_Vector* to, void* user)
{
    UIFontHWVector* font = (UIFontHWVector*)user;
    font->RecordCommand(VGU_PATH_CMD_MOVE);
    font->RecordPointData(to->x / 64.0f);
    font->RecordPointData(0 - (to->y / 64.0f));
    return 0;
}

static int LineTo(const FT_Vector* to, void* user)
{
    UIFontHWVector* font = (UIFontHWVector*)user;
    font->RecordCommand(VGU_PATH_CMD_LINE);
    font->RecordPointData(to->x / 64.0f);
    font->RecordPointData(-to->y / 64.0f);
    return 0;
}

static int ConicTo(const FT_Vector* control, const FT_Vector* to, void* user)
{
    UIFontHWVector* font = (UIFontHWVector*)user;
    font->RecordCommand(VGU_PATH_CMD_QUARD);
    font->RecordPointData(control->x / 64.0f);
    font->RecordPointData(-control->y / 64.0f);
    font->RecordPointData(to->x / 64.0f);
    font->RecordPointData(-to->y / 64.0f);
    return 0;
}

static int CubicTo(const FT_Vector* control1, const FT_Vector* control2, const FT_Vector* to, void* user)
{
    UIFontHWVector* font = (UIFontHWVector*)user;
    font->RecordCommand(VGU_PATH_CMD_CUBIC);
    font->RecordPointData(control1->x / 64.0f);
    font->RecordPointData(-control1->y / 64.0f);
    font->RecordPointData(control2->x / 64.0f);
    font->RecordPointData(-control2->y / 64.0f);
    font->RecordPointData(to->x / 64.0f);
    font->RecordPointData(-to->y / 64.0f);
    return 0;
}

bool UIFontHWVector::GetGlyphPathData(uint32_t unicode, uint16_t fontId, uint8_t fontSize, GlyphPathData& data)
{
    FaceInfo faceInfo;
    int8_t ret = GetFaceInfo(fontId, fontSize, faceInfo);
    if (ret != RET_VALUE_OK) {
        GRAPHIC_LOGE("GetFaceInfo failed: 0x%x", unicode);
        return false;
    }

    if (!freeTypeInited_ || (faceInfo.face == nullptr) || (pathCache_ == nullptr)) {
        GRAPHIC_LOGE("freeTypeInited_: %d, faceInfo.face : %p, pathCache_: %p",
            freeTypeInited_, faceInfo.face, pathCache_);
        return false;
    }
    GlyphPathCacheData* cache = reinterpret_cast<GlyphPathCacheData*>(pathCache_->GetCacheData(fontId, unicode));
    if (cache == nullptr) {
        cache = LoadAndCacheGlyph(faceInfo.face, fontId, unicode);
    }

    if (cache != nullptr) {
        float scale = 1.0f * fontSize * FONT_PIXEL_IN_POINT / faceInfo.face->units_per_EM;
        data.scale = scale;
        data.ascender = static_cast<uint16_t>(ceil(faceInfo.face->ascender * scale / FONT_PIXEL_IN_POINT));
        data.left = static_cast<int16_t>(ceil(cache->left * scale / FONT_PIXEL_IN_POINT));
        data.top = static_cast<int16_t>(ceil(cache->top * scale / FONT_PIXEL_IN_POINT));
        data.width = static_cast<uint16_t>(ceil(cache->width * scale / FONT_PIXEL_IN_POINT));
        data.height = static_cast<uint16_t>(ceil(cache->height * scale / FONT_PIXEL_IN_POINT));
        data.advance = static_cast<uint16_t>(ceil(cache->advance * scale / FONT_PIXEL_IN_POINT));
        data.cmdsNum = cache->cmdsNum;
        data.pointsNum = cache->pointsNum;
        data.cmds = cache->data;
        data.points = reinterpret_cast<float*>(cache->data + ALIGN_SIZE(cache->cmdsNum));
        return true;
    }

    return false;
}

uint8_t* UIFontHWVector::GetBitmap(uint32_t unicode, GlyphNode& glyphNode, uint16_t fontId, uint8_t fontSize)
{
    return nullptr;
}

UIFontHWVector::GlyphPathCacheData* UIFontHWVector::LoadAndCacheGlyph(FT_Face ftFace, uint16_t fontId, uint32_t unicode)
{
    int32_t error = 0;
    uint32_t rectifiedUnicode = unicode;
    if (IsGlyphFont(unicode) != 0) {
        if (fontId != GetFontId(unicode)) {
            return nullptr;
        }
        rectifiedUnicode = unicode & (0xFFFFFF); // Whether 0 ~24 bit storage is unicode
        error = FT_Load_Glyph(ftFace, rectifiedUnicode, FT_LOAD_NO_SCALE);
    } else {
        error = FT_Load_Char(ftFace, rectifiedUnicode, FT_LOAD_NO_SCALE);
    }

    if ((error != 0) || (ftFace->glyph->glyph_index == 0)) {
        return nullptr;
    }

    FT_Outline ftOutline = ftFace->glyph->outline;
    FT_Outline_Funcs funcs = {0};
    cmds_.Clear();
    pointData_.Clear();
    funcs.move_to = static_cast<FT_Outline_MoveTo_Func>(&MoveTo);
    funcs.line_to = static_cast<FT_Outline_LineTo_Func>(&LineTo);
    funcs.conic_to = static_cast<FT_Outline_ConicTo_Func>(&ConicTo);
    funcs.cubic_to = static_cast<FT_Outline_CubicTo_Func>(&CubicTo);
    FT_Outline_Decompose(&ftOutline, &funcs, this);

    uint32_t cmdDataSize = cmds_.Size();
    uint32_t alignCmdDataSize = ALIGN_SIZE(cmdDataSize);
    uint32_t pathDataSize = pointData_.Size() * sizeof(float);
    uint32_t totalSize = sizeof(GlyphPathCacheData) + alignCmdDataSize + pathDataSize;
    GlyphPathCacheData* cache = reinterpret_cast<GlyphPathCacheData*>(pathCache_->GetSpace(fontId, unicode, totalSize));
    if (cache != nullptr) {
        cache->left = ftFace->glyph->metrics.horiBearingX;
        cache->top = ftFace->glyph->metrics.horiBearingY;
        cache->width = static_cast<uint16_t>(ftFace->glyph->metrics.width);
        cache->height = static_cast<uint16_t>(ftFace->glyph->metrics.height);
        cache->advance = static_cast<uint16_t>(ftFace->glyph->advance.x);
        cache->cmdsNum = cmds_.Size();
        cache->pointsNum = pointData_.Size();
        if (cmdDataSize != 0) {
            if (memcpy_s(cache->data, cmdDataSize, cmds_.Begin(), cmdDataSize) != EOK) {
                GRAPHIC_LOGE("memcpy cmd data failed\n");
            }
            if (memcpy_s(cache->data + alignCmdDataSize, pathDataSize, pointData_.Begin(), pathDataSize) != EOK) {
                GRAPHIC_LOGE("memcpy path data failed\n");
            }
        }
    }

    return cache;
}
} // namespace OHOS
