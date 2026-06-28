/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: bitmap font
 * Author: Hisi Graphic Team
 */

#ifndef UI_FONT_BITMAP_CUSTOM_H
#define UI_FONT_BITMAP_CUSTOM_H
#include "font/base_font.h"
#include "graphic_config.h"

namespace OHOS {
class UIFontBitmapCustom : public BaseFont {
public:
    UIFontBitmapCustom();
    ~UIFontBitmapCustom() {}

    bool IsVectorFont() const override
    {
        return false;
    }

    /**
     * @brief Get the height of the current font setting
     *
     * @return uint16_t font height
     */
    uint16_t GetHeight(uint16_t fontId, uint8_t fontSize = 0) override;

    /**
     * @brief Get the resId that matches the font size in the font library.
     *
     * @param ttfName This parameter is not used.
     * @param size Bitmap Font Size
     * @return uint16_t font res id
     */
    uint16_t GetFontId(const char* ttfName, uint8_t fontSize = 0) const override;

    /**
     * @brief Get character width
     *
     * @param unicode Unicode code of the character
     * @param fontId  Font resource id, The font width is taken at this ID
     * @return int16_t character width
     */
    int16_t GetWidth(uint32_t unicode, uint16_t fontId, uint8_t fontSize = 0) override;

    /**
     * @brief Get bitmap for specific unicode
     *
     * @param unicode Unicode code of the character
     * @param glyphNode Character information description
     * @param fontId Font resource id, The font info is taken at this ID
     * @return uint8_t* character start addr in the resource memory
     */
    uint8_t* GetBitmap(uint32_t unicode, GlyphNode& glyphNode, uint16_t fontId, uint8_t fontSize = 0) override;
    
    /**
     * @brief Get font header
     *
     * @param fontHeader The header description of the font library
     * @return int8_t Whether the obtained result is successful
     */
    int8_t GetFontHeader(FontHeader& fontHeader, uint16_t fontId, uint8_t fontSize = 0) override;

    /**
     * @brief Get the glyph node
     *
     * @param unicode Unicode code of the character
     * @param glyphNode Character information description
     * @return int8_t Whether the obtained result is successful
     */
    int8_t GetGlyphNode(uint32_t unicode, GlyphNode& glyphNode, uint16_t fontId, uint8_t fontSize = 0) override;

    /**
     * @brief Get the glyph node, this interface does not take effect. The return value is FONT_WEIGHT_8.
     *
     */
    uint8_t GetFontWeight(uint16_t fontId) override
    {
        return FONT_WEIGHT_8;
    }

    /**
     * @brief Set the Font Path, this interface does not take effect in bitmap.
     */
    int8_t SetFontPath(const char* path, FontType type) override
    {
        return RET_VALUE_OK;
    }

    bool IsEmojiFont(uint16_t fontId) override
    {
        return false;
    }

    uint16_t GetOffsetPosY(const char* text, uint16_t lineLength, bool& isEmoijLarge,
                                   uint16_t fontId, uint8_t fontSize) override;

    uint16_t GetLineMaxHeight(const char* text, uint16_t lineLength, uint16_t fontId, uint8_t fontSize,
                                      uint16_t& letterIndex, SpannableString* spannableString) override;
private:
    struct BitmapFontInfo {
        uint8_t size;
        uint16_t fontHeight;
        uint16_t width;
        uint16_t height;
        uint16_t glyphNum;
        uint32_t dataSize;
        uint32_t dataAddrOffset;
    };

    struct CharInfo {
        uint32_t unicode;
        uint16_t x;
        uint16_t y;
        uint16_t width;
        uint16_t height;
        int8_t xOffset;
        int8_t yOffset;
        uint16_t xadvance;
    };

    uint16_t FindUnicodeIndex(uint32_t unicode, uint8_t fontId);

    uint32_t* fontPackageOffset_ = nullptr;
    uint8_t fontPackageNum_ = 0;
    uint8_t* resAddr_ = reinterpret_cast<uint8_t*>(MEM_RESOURCE_START());
};
}
#endif