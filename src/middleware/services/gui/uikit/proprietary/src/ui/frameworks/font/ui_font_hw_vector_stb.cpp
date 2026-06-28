/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: hw vector font by stb_truetype
 * Author: Hisi Graphic Team
 * Create: 2025-10
 */

#include <cmath>
#include <string>
#include "gfx_utils/file.h"
#include "gfx_utils/graphic_log.h"
#include "graphic_config.h"
#include "securec.h"
#include "font/ui_font_hw_vector_stb.h"
#if ENABLE_MULTI_FONT
#include "font/ui_multi_font_manager.h"
#endif
#include "common/image_cache_manager.h"
#include "common/typed_text.h"
#include "display_vgu.h"

#define ALIGN_SIZE(size) (((size) + 15) & (~15)) // 16 byte align

namespace OHOS {
UIFontHWVectorStb::UIFontHWVectorStb()
{
    pathCache_ = new(std::nothrow) UIFontHWCache();
    if (pathCache_ == nullptr) {
        GRAPHIC_LOGE("new UIFontHWCache failed!");
    }
}

UIFontHWVectorStb::~UIFontHWVectorStb()
{
    if (pathCache_ != nullptr) {
        delete pathCache_;
    }
}

bool UIFontHWVectorStb::TryToFreeFontCache()
{
    if (pathCache_ == nullptr) {
        return false;
    }

    return pathCache_->TryToFreeFontCache();
}

uint16_t UIFontHWVectorStb::GetHeight(uint16_t fontId, uint8_t fontSize)
{
    FaceInfo faceInfo;
    int8_t ret = GetFaceInfo(fontId, fontSize, faceInfo);
    if (ret != RET_VALUE_OK) {
        return INVALID_RET_VALUE;
    }

    float scale = 1.0f * fontSize * FONT_PIXEL_IN_POINT / faceInfo.face->unitsPerEm;
    return static_cast<uint16_t>(faceInfo.face->height * scale / FONT_PIXEL_IN_POINT);
}

int16_t UIFontHWVectorStb::GetWidth(uint32_t unicode, uint16_t fontId, uint8_t fontSize)
{
    FaceInfo faceInfo;
    int8_t ret = GetFaceInfo(fontId, fontSize, faceInfo);
    if (ret != RET_VALUE_OK) {
        return INVALID_RET_VALUE;
    }

    if (pathCache_ == nullptr) {
        return INVALID_RET_VALUE;
    }

    GlyphPathCacheData* cache = reinterpret_cast<GlyphPathCacheData*>(pathCache_->GetCacheData(fontId, unicode));
    if (cache == nullptr) {
        cache = LoadAndCacheGlyph(faceInfo.face, fontId, unicode);
    }

    float scale = 1.0f * fontSize * FONT_PIXEL_IN_POINT / faceInfo.face->unitsPerEm;
    if (cache != nullptr) {
        return static_cast<int16_t>(cache->advance * scale / FONT_PIXEL_IN_POINT);
    }

    return GetEmojiWidth(unicode, fontId, fontSize);
}

int8_t UIFontHWVectorStb::GetFontHeader(FontHeader& fontHeader, uint16_t fontId, uint8_t fontSize)
{
    FaceInfo faceInfo;
    int8_t ret = GetFaceInfo(fontId, fontSize, faceInfo);
    if (ret != RET_VALUE_OK) {
        return INVALID_RET_VALUE;
    }

    if (pathCache_ == nullptr) {
        return INVALID_RET_VALUE;
    }

    float scale = 1.0f * fontSize * FONT_PIXEL_IN_POINT / faceInfo.face->unitsPerEm;
    fontHeader.ascender = static_cast<int16_t>(faceInfo.face->ascender * scale / FONT_PIXEL_IN_POINT);
    fontHeader.descender = static_cast<int16_t>(faceInfo.face->descender * scale /
        FONT_PIXEL_IN_POINT);
    fontHeader.fontHeight = static_cast<uint16_t>(faceInfo.face->height * scale /
        FONT_PIXEL_IN_POINT);
    return RET_VALUE_OK;
}

int8_t UIFontHWVectorStb::GetGlyphNode(uint32_t unicode, GlyphNode& glyphNode, uint16_t fontId, uint8_t fontSize)
{
    FaceInfo faceInfo;
    int8_t ret = GetFaceInfo(fontId, fontSize, faceInfo);
    if (ret != RET_VALUE_OK) {
        return INVALID_RET_VALUE;
    }

    if (pathCache_ == nullptr) {
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
        float scale = 1.0f * fontSize * FONT_PIXEL_IN_POINT / faceInfo.face->unitsPerEm;
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

static int MoveTo(stbtt_vertex* vertices, UIFontHWVectorStb* font)
{
    font->RecordCommand(VGU_PATH_CMD_MOVE);
    font->RecordPointData(vertices->x / 64.0f);
    font->RecordPointData(0 - (vertices->y / 64.0f));
    return 0;
}

static int LineTo(stbtt_vertex* vertices, UIFontHWVectorStb* font)
{
    font->RecordCommand(VGU_PATH_CMD_LINE);
    font->RecordPointData(vertices->x / 64.0f);
    font->RecordPointData(-vertices->y / 64.0f);
    return 0;
}

static int ConicTo(stbtt_vertex* vertices, UIFontHWVectorStb* font)
{
    font->RecordCommand(VGU_PATH_CMD_QUARD);
    font->RecordPointData(vertices->cx / 64.0f);
    font->RecordPointData(-vertices->cy / 64.0f);
    font->RecordPointData(vertices->x / 64.0f);
    font->RecordPointData(-vertices->y / 64.0f);
    return 0;
}

static int CubicTo(stbtt_vertex* vertices, UIFontHWVectorStb* font)
{
    font->RecordCommand(VGU_PATH_CMD_CUBIC);
    font->RecordPointData(vertices->cx / 64.0f);
    font->RecordPointData(-vertices->cy / 64.0f);
    font->RecordPointData(vertices->cx1 / 64.0f);
    font->RecordPointData(-vertices->cy1 / 64.0f);
    font->RecordPointData(vertices->x / 64.0f);
    font->RecordPointData(-vertices->y / 64.0f);
    return 0;
}

bool UIFontHWVectorStb::GetGlyphPathData(uint32_t unicode, uint16_t fontId, uint8_t fontSize, GlyphPathData& data)
{
    FaceInfo faceInfo;
    int8_t ret = GetFaceInfo(fontId, fontSize, faceInfo);
    if (ret != RET_VALUE_OK) {
        GRAPHIC_LOGE("GetFaceInfo failed: 0x%x", unicode);
        return false;
    }

    if (pathCache_ == nullptr) {
        GRAPHIC_LOGE("pathCache_ is null");
        return false;
    }
    GlyphPathCacheData* cache = reinterpret_cast<GlyphPathCacheData*>(pathCache_->GetCacheData(fontId, unicode));
    if (cache == nullptr) {
        cache = LoadAndCacheGlyph(faceInfo.face, fontId, unicode);
    }

    if (cache != nullptr) {
        float scale = 1.0f * fontSize * FONT_PIXEL_IN_POINT / faceInfo.face->unitsPerEm;
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

uint8_t* UIFontHWVectorStb::GetBitmap(uint32_t unicode, GlyphNode& glyphNode, uint16_t fontId, uint8_t fontSize)
{
    return nullptr;
}

void UIFontHWVectorStb::StbOutLineDecompose(stbtt_vertex* vertices, int32_t numVerts)
{
    for (int i = 0; i < numVerts; i++) {
        switch (vertices[i].type) {
            case STBTT_vmove:
                MoveTo(&vertices[i], this);
                break;
            case STBTT_vline:
                LineTo(&vertices[i], this);
                break;
            case STBTT_vcurve:
                ConicTo(&vertices[i], this);
                break;
            case STBTT_vcubic:
                CubicTo(&vertices[i], this);
                break;
            default:
                break;
        }
    }
}

bool UIFontHWVectorStb::StbLoadChar(StbFace *face, int32_t glyphIndex, uint8_t loadFlag)
{
    int32_t x0 = 0;
    int32_t y0 = 0;
    int32_t x1 = 0;
    int32_t y1 = 0;

    if (loadFlag != STB_LOAD_RENDER) {
        return false;
    }
    StbGlyph *glyph = &face->glyph;

    stbtt_GetGlyphHMetrics(&face->stbFontInfo, glyphIndex, &glyph->advanceX, &glyph->horiBearingX);
    // 1. 获取边界框（用于 width/height 和部分 bearing 值）
    if (stbtt_GetGlyphBox(&face->stbFontInfo, glyphIndex, &x0, &y0, &x1, &y1) == 1) {
        glyph->width = x1 - x0;
        glyph->height = y1 - y0;
        glyph->horiBearingY = y1; // 在 stb_truetype 坐标系中，y1 > 0 表示基线之上
    } else {
        glyph->width = 0;
        glyph->height = 0;
        glyph->horiBearingY = 0; // 在 stb_truetype 坐标系中，y1 > 0 表示基线之上
    }

    glyph->numVerts = stbtt_GetGlyphShape(&face->stbFontInfo, glyphIndex, &glyph->vertices);

    return true;
}

UIFontHWVectorStb::GlyphPathCacheData* UIFontHWVectorStb::LoadAndCacheGlyph(StbFace *face,
                                                                            uint16_t fontId, uint32_t unicode)
{
    int32_t glyphIndex = 0;
    if (IsGlyphFont(unicode) != 0) {
        if (fontId != GetFontId(unicode)) {
            return nullptr;
        }
        glyphIndex = unicode & (0xFFFFFF);
    } else {
        glyphIndex = stbtt_FindGlyphIndex(&face->stbFontInfo, unicode);
    }
    if (glyphIndex == 0) {
        return nullptr;
    }
    if (!StbLoadChar(face, glyphIndex, STB_LOAD_RENDER)) {
        return nullptr;
    }

    cmds_.Clear();
    pointData_.Clear();

    if (face->glyph.vertices != nullptr) {
        StbOutLineDecompose(face->glyph.vertices, face->glyph.numVerts);
        free(face->glyph.vertices);
    }

    uint32_t cmdDataSize = cmds_.Size();
    uint32_t alignCmdDataSize = ALIGN_SIZE(cmdDataSize);
    uint32_t pathDataSize = pointData_.Size() * sizeof(float);
    uint32_t totalSize = sizeof(GlyphPathCacheData) + alignCmdDataSize + pathDataSize;
    GlyphPathCacheData* cache = reinterpret_cast<GlyphPathCacheData*>(pathCache_->GetSpace(fontId, unicode, totalSize));
    if (cache != nullptr) {
        cache->left = face->glyph.horiBearingX;
        cache->top = face->glyph.horiBearingY;
        cache->width = static_cast<uint16_t>(face->glyph.width);
        cache->height = static_cast<uint16_t>(face->glyph.height);
        cache->advance = static_cast<uint16_t>(face->glyph.advanceX);
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
