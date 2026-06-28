/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: hw vector font by stb_truetype
 * Author: Hisi Graphic Team
 * Create: 2025-10
 */

#ifndef UI_FONT_VECTOR_STB_H
#define UI_FONT_VECTOR_STB_H
#include <memory>
#include "font/base_font.h"
#include "graphic_config.h"
#include "stb_truetype.h"
#include "font/glyphs_cache.h"
#include "font/ui_font_cache.h"
namespace OHOS {
class UIFontVectorStb : public BaseFont {
public:
    UIFontVectorStb();

    ~UIFontVectorStb();
    UIFontVectorStb(const UIFontVectorStb&) = delete;
    UIFontVectorStb& operator=(const UIFontVectorStb&) noexcept = delete;
    bool IsVectorFont() const override;
    int8_t SetFontPath(const char* path, FontType type) override;
    uint16_t GetHeight(uint16_t fontId, uint8_t fontSize) override;
    uint16_t GetFontId(const char* ttfName, uint8_t fontSize = 0) const override;
    int16_t GetWidth(uint32_t unicode, uint16_t fontId, uint8_t fontSize) override;
    uint8_t* GetBitmap(uint32_t unicode, GlyphNode& glyphNode, uint16_t fontId, uint8_t fontSize) override;
    int8_t GetFontHeader(FontHeader& fontHeader, uint16_t fontId, uint8_t fontSize) override;
    int8_t GetGlyphNode(uint32_t unicode, GlyphNode& glyphNode, uint16_t fontId, uint8_t fontSize) override;
    uint8_t GetFontWeight(uint16_t fontId) override;
    uint16_t
        GetShapingFontId(char* text, uint8_t& ttfId, uint32_t& script, uint16_t fontId, uint8_t size) const override;
    uint8_t RegisterFontInfo(const char* ttfName, uint8_t shaping = 0) override;
    uint8_t RegisterFontInfo(const UITextLanguageFontParam* fontsTable, uint8_t num) override;
    uint8_t UnregisterFontInfo(const char* ttfName) override;
    uint8_t UnregisterFontInfo(const UITextLanguageFontParam* fontsTable, uint8_t num) override;
    uint8_t RegisterTtcFontInfo(const char* ttcName, const TtfInfo* ttfInfo, uint8_t count) override;
    uint8_t UnregisterTtcFontInfo(const char* ttcName, const TtfInfo* ttfInfo, uint8_t count) override;
    const UITextLanguageFontParam* GetFontInfo(uint16_t fontId) const override;
    int32_t OpenVectorFont(uint8_t ttfId) override;
    uint16_t GetOffsetPosY(const char* text, uint16_t lineLength,
                           bool& isEmojiLarge, uint16_t fontId, uint8_t fontSize) override;
    uint16_t GetLineMaxHeight(const char* text, uint16_t lineLength, uint16_t fontId, uint8_t fontSize,
                              uint16_t& letterIndex, SpannableString* spannableString)  override;
    bool GetTtfInfo(uint8_t ttfId, uint8_t* ttfBuffer, uint32_t ttfBufferSize, TtfHeader& ttfHeader) override;
    void SetPsramMemory(uintptr_t psramAddr, uint32_t psramLen) override;
    int8_t SetCurrentLangId(uint8_t langId) override;
    bool IsEmojiFont(uint16_t fontId) override;

    bool GetEmoji(uint32_t unicode, ImageInfo &unicodeInfo);
    int16_t GetEmojiHeight(uint16_t fontId, uint8_t fontSize);
    int16_t GetEmojiWidth(uint32_t unicode, uint16_t fontId, uint8_t fontSize);

private:
    friend class UIFontHWVectorStb;
    static constexpr uint8_t FONT_ID_MAX = 0xFF;
    static constexpr uint8_t FONT_TTC_MAX = 0x20;
    static constexpr uint8_t FONT_INVALID_TTF_ID = 0xFF;
    static constexpr uint8_t TTF_NAME_LEN_MAX = 128;
    static constexpr uint8_t STB_LOAD_RENDER = 0;
    static constexpr uint8_t STB_LOAD_COLOR = 1;
    static constexpr uint8_t STB_PIXEL_MODE_BGRA = 1;
    UITextLanguageFontParam fontInfo_[FONT_ID_MAX] = {{}};
    std::string ttfDir_;

    typedef struct {
        int32_t           advanceX;
        int32_t           advanceY;
        int32_t           width;
        int32_t           height;
        int32_t           horiBearingX;
        int32_t           horiBearingY;
        int32_t           bitmapWidth;
        int32_t           bitmapHeight;
        int32_t           bitmapLeft;
        int32_t           bitmapTop;
        uint8_t           pixelMode;
        uint8_t           *bitmapBuffer;
        int32_t           numVerts;
        stbtt_vertex      *vertices;
    } StbGlyph;

    typedef struct {
        int width;
        int32_t height;
        int32_t ascender;
        int32_t descender;
        int32_t lineGap;
        float xScale;
        float yScale;
    } StbSizeMetrics;

    typedef struct {
        int horiBearingX, horiBearingY, horiAdvance;
        int vertBearingX, vertBearingY, vertAdvance;
        int width;
        int32_t height;
        int32_t ascender;
        int32_t descender;
        int32_t lineGap;
        float scale;
        float size;
        uint16_t unitsPerEm;
        StbSizeMetrics metrics;
        StbGlyph glyph;
        stbtt_fontinfo stbFontInfo;
        uint64_t ttfDataSize;
        FILE *ttfFile;
    } StbFace;

    struct FaceInfo {
        StbFace *face;
        uint16_t key;
    };

    StbFace *stbFaces_[FONT_ID_MAX] = {0};
    uint8_t fontSize_[FONT_ID_MAX] = {0};
    uint8_t currentFontInfoNum_ = 0;

    void SetFace(FaceInfo& faceInfo, uint32_t unicode);
    void SetFace(FaceInfo& faceInfo, uint32_t unicode, TextStyle textStyle);
    uint16_t GetFontId(uint32_t unicode) const;
    uint16_t GetKey(uint16_t fontId, uint8_t size);
    int8_t LoadGlyphIntoFace(uint16_t& fontId, uint8_t fontSize, uint32_t unicode, GlyphNode& glyphNode);
    void SaveGlyphNode(uint32_t unicode, uint16_t fontKey, Metric* metric);
    uint8_t IsGlyphFont(uint32_t unicode);
    int8_t GetFaceInfo(uint16_t fontId, uint8_t fontSize, FaceInfo& faceInfo);
    uint16_t GetMaxSubLineHeight(uint16_t textNum, uint16_t loopNum, uint16_t maxHeight, uint16_t emojiNum);
    bool GetTtfInfoFromTtf(uint8_t* ttfBuffer,
                           uint32_t ttfBufferSize,
                           TtfHeader& ttfHeader,
                           UITextLanguageFontParam fontInfo);
    bool GetTtfInfoFromTtc(uint8_t* ttfBuffer,
                           uint32_t ttfBufferSize,
                           TtfHeader& ttfHeader,
                           UITextLanguageFontParam fontInfo);
    void ClearFontGlyph(StbFace *face);
    bool IsColorEmojiFont(const StbFace *face);
    int8_t SetupColorFont(StbFace *face, uint8_t fontSize);
    int8_t SetupNormalFont(StbFace *face, uint8_t fontSize);
    virtual bool StbLoadChar(StbFace *face, int32_t glyphIndex, uint8_t loadFlag);
};
} // namespace OHOS
#endif
