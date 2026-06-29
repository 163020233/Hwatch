/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: hw vector font by stb_truetype
 * Author: Hisi Graphic Team
 * Create: 2025-10
 */

#ifndef UI_FONT_HW_VECTOR_H
#define UI_FONT_HW_VECTOR_H
#include <memory>
#include "font/ui_font_vector_stb.h"
#include "graphic_config.h"
#include "font/ui_font_hw_cache.h"
#include "gfx_utils/vector.h"

namespace OHOS {
class UIFontHWVectorStb : public UIFontVectorStb {
public:
    UIFontHWVectorStb();
    ~UIFontHWVectorStb() override;
    UIFontHWVectorStb(const UIFontHWVectorStb&) = delete;
    UIFontHWVectorStb& operator=(const UIFontHWVectorStb&) noexcept = delete;

    void RecordCommand(uint8_t cmd)
    {
        cmds_.PushBack(cmd);
    }

    void RecordPointData(float data)
    {
        pointData_.PushBack(data);
    }

    bool TryToFreeFontCache() override;

    uint16_t GetHeight(uint16_t fontId, uint8_t fontSize) override;
    int16_t GetWidth(uint32_t unicode, uint16_t fontId, uint8_t fontSize) override;
    bool GetGlyphPathData(uint32_t unicode, uint16_t fontId, uint8_t fontSize, GlyphPathData& data) override;
    uint8_t* GetBitmap(uint32_t unicode, GlyphNode& glyphNode, uint16_t fontId, uint8_t fontSize) override;
    int8_t GetFontHeader(FontHeader& fontHeader, uint16_t fontId, uint8_t fontSize) override;
    int8_t GetGlyphNode(uint32_t unicode, GlyphNode& glyphNode, uint16_t fontId, uint8_t fontSize) override;

private:
    UIFontHWCache* pathCache_ = nullptr;
    struct GlyphPathCacheData {
        int16_t left;
        int16_t top;
        uint16_t width;
        uint16_t height;
        uint16_t advance;
        uint16_t cmdsNum;
        uint16_t pointsNum;
        uint16_t reserve; // used for 16 byte align
        uint8_t data[0];  // data of cmds and points
    };

    Graphic::Vector<uint8_t> cmds_;
    Graphic::Vector<float> pointData_;
    void StbOutLineDecompose(stbtt_vertex* vertices, int32_t numVerts);
    GlyphPathCacheData* LoadAndCacheGlyph(StbFace *face, uint16_t fontId, uint32_t unicode);
    bool StbLoadChar(StbFace *face, int32_t glyphIndex, uint8_t loadFlag) override;
};
} // namespace OHOS
#endif

