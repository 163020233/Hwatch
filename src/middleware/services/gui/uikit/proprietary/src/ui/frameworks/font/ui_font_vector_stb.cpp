/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: hw vector font by stb_truetype
 * Author: Hisi Graphic Team
 * Create: 2025-10
 */

#define STB_TRUETYPE_IMPLEMENTATION
#define STB_TRUETYPE_IMPLEMENTATION_STREAM

#include "font/ui_font_vector_stb.h"

#include "common/typed_text.h"
#include "draw/draw_utils.h"
#include "font/font_ram_allocator.h"
#include "font/ui_font_cache_manager.h"
#include "gfx_utils/file.h"
#include "gfx_utils/graphic_log.h"
#include "graphic_config.h"
#include "securec.h"
#if defined(ENABLE_MULTI_FONT) && ENABLE_MULTI_FONT
#include "font/ui_multi_font_manager.h"
#endif
#if defined(ENABLE_SHAPING) && ENABLE_SHAPING
#include "font/ui_text_shaping.h"
#endif
#include "common/image_cache_manager.h"
#include "common/ui_emoji_table.h"

namespace OHOS {
UIFontVectorStb::UIFontVectorStb()
{
#ifdef _WIN32
    ttfDir_ = _pgmptr;
    size_t len = ttfDir_.size();
    size_t pos = ttfDir_.find_last_of('\\');
    if (pos != std::string::npos) {
        ttfDir_.replace((pos + 1), (len - pos), VECTOR_FONT_DIR);
    }
#else
    ttfDir_ = VECTOR_FONT_DIR;
#endif // _WIN32
}

UIFontVectorStb::~UIFontVectorStb()
{
    UnregisterFontInfo(DEFAULT_VECTOR_FONT_FILENAME);
    if (IsEmojiSupport() && GetEmojiPath() != nullptr) {
        ImageCacheManager::GetInstance().UnloadAllInMultiRes(std::string{GetEmojiPath()});
    }
}

bool UIFontVectorStb::IsColorEmojiFont(const StbFace *face)
{
    (void)face;
    return false;
}

int8_t UIFontVectorStb::SetupColorFont(StbFace *face, uint8_t fontSize)
{
    (void)face;
    (void)fontSize;
    return INVALID_RET_VALUE;
}

int8_t UIFontVectorStb::SetupNormalFont(StbFace *face, uint8_t fontSize)
{
    StbSizeMetrics *metrics = &face->metrics;
    face->unitsPerEm =
        ttUSHORT(face->stbFontInfo.data, face->stbFontInfo.data + face->stbFontInfo.head + 18); // 18: units per em
    if (face->unitsPerEm == 0) {
        return INVALID_RET_VALUE;
    }
    stbtt_GetFontVMetrics(&face->stbFontInfo, &face->ascender, &face->descender, &face->lineGap);
    face->height = face->ascender - face->descender + face->lineGap;
    if (fontSize == 0) {
        fontSize = face->unitsPerEm;
    }
    metrics->yScale = stbtt_ScaleForMappingEmToPixels(&face->stbFontInfo, fontSize);
    metrics->xScale = metrics->yScale;
    metrics->ascender = static_cast<int32_t>(face->ascender * metrics->yScale);
    metrics->descender = static_cast<int32_t>(face->descender * metrics->yScale);
    metrics->height = static_cast<int32_t>(face->height * metrics->yScale);

    return 0;
}

uint8_t UIFontVectorStb::RegisterFontInfo(const char* ttfName, uint8_t shaping)
{
    if (ttfName == nullptr) {
        return FONT_INVALID_TTF_ID;
    }
    int32_t j = 0;
    while (j < FONT_ID_MAX) {
        if ((fontInfo_[j].ttfName != nullptr) && !strncmp(fontInfo_[j].ttfName, ttfName, TTF_NAME_LEN_MAX)) {
            return j;
        } else if (fontInfo_[j].ttfName == nullptr) {
            std::string ttfPath = ttfDir_;
            ttfPath.append(ttfName);
            FILE *ttfFile = fopen(ttfPath.c_str(), "rb");
            if (!ttfFile) {
                return FONT_INVALID_TTF_ID;
            }
            stbFaces_[j] = (StbFace *)UIMalloc(sizeof(StbFace));
            if (stbFaces_[j] == nullptr) {
                fclose (ttfFile);
                return FONT_INVALID_TTF_ID;
            }
            stbFaces_[j]->ttfFile = ttfFile;
            int32_t success = stbtt_InitFont(&stbFaces_[j]->stbFontInfo, stbFaces_[j]->ttfFile,
                stbtt_GetFontOffsetForIndex(stbFaces_[j]->ttfFile, 0));
            if (success != 1) {
                fclose(stbFaces_[j]->ttfFile);
                free(stbFaces_[j]);
                stbFaces_[j] = nullptr;
                return FONT_INVALID_TTF_ID;
            }
            fontInfo_[j].ttfName = ttfName;
            fontInfo_[j].shaping = shaping;
            fontInfo_[j].ttfId = j;
            fontInfo_[j].ttcIndex = FONT_TTC_MAX;
            currentFontInfoNum_ = j + 1;
            if (IsColorEmojiFont(stbFaces_[j])) {
                fontInfo_[j].fontWeight = BPP_BIT_32;
            } else {
                fontInfo_[j].fontWeight = BPP_BIT_8;
            }
#if defined(ENABLE_MULTI_FONT) && ENABLE_MULTI_FONT
            UIMultiFontManager::GetInstance()->UpdateScript(fontInfo_[j]);
#endif
            return j;
        }
        j++;
    }
    return FONT_INVALID_TTF_ID;
}

uint8_t UIFontVectorStb::RegisterFontInfo(const UITextLanguageFontParam* fontsTable, uint8_t num)
{
    if (fontsTable == nullptr) {
        return FONT_INVALID_TTF_ID;
    }
    uint8_t count = 0;
    for (uint8_t i = 0; i < num; i++) {
        uint8_t result = RegisterFontInfo(fontsTable[i].ttfName, fontsTable[i].shaping);
        if (result == FONT_INVALID_TTF_ID) {
            continue;
        }
        count++;
    }
    return count;
}

uint8_t UIFontVectorStb::RegisterTtcFontInfo(const char* ttcName, const TtfInfo* ttfInfo, uint8_t count)
{
    (void)ttcName;
    (void)ttfInfo;
    (void)count;
    return FONT_INVALID_TTF_ID;
}

uint8_t UIFontVectorStb::UnregisterTtcFontInfo(const char* ttcName, const TtfInfo* ttfInfo, uint8_t count)
{
    (void)ttcName;
    (void)ttfInfo;
    (void)count;
    return FONT_INVALID_TTF_ID;
}

uint8_t UIFontVectorStb::UnregisterFontInfo(const UITextLanguageFontParam* fontsTable, uint8_t num)
{
    if (fontsTable == nullptr) {
        return 0;
    }
    uint8_t count = 0;
    for (uint8_t i = 0; i < num; i++) {
        uint8_t result = UnregisterFontInfo(fontsTable[i].ttfName);
        if (result == FONT_INVALID_TTF_ID) {
            return FONT_INVALID_TTF_ID;
        }
        count++;
    }
    return count;
}

uint8_t UIFontVectorStb::UnregisterFontInfo(const char* ttfName)
{
    if (ttfName != nullptr) {
        int32_t i = 0;
        while (i < FONT_ID_MAX) {
            if ((fontInfo_[i].ttfName != nullptr) && !strncmp(fontInfo_[i].ttfName, ttfName, TTF_NAME_LEN_MAX)) {
                fontInfo_[i].ttfName = nullptr;
                fclose (stbFaces_[i]->ttfFile);
                stbtt_DeInitFont(&stbFaces_[i]->stbFontInfo);
                UIFree(stbFaces_[i]);
                stbFaces_[i] = nullptr;
                fontSize_[i] = 0;
                return static_cast<uint8_t>(i);
            }
            i++;
        }
    }
    return FONT_INVALID_TTF_ID;
}

const UITextLanguageFontParam* UIFontVectorStb::GetFontInfo(uint16_t fontId) const
{
    if (fontId < FONT_ID_MAX) {
        return static_cast<const UITextLanguageFontParam*>(&fontInfo_[fontId]);
    }
    return nullptr;
}

int32_t UIFontVectorStb::OpenVectorFont(uint8_t ttfId)
{
    int32_t i = 0;
    int32_t fp = 0;
    while (i < FONT_ID_MAX) {
        if (fontInfo_[i].ttfName == nullptr) {
            i++;
            continue;
        }
        if (fontInfo_[i].ttfId == ttfId) {
            std::string ttfPath = ttfDir_;
            ttfPath.append(fontInfo_[i].ttfName);
#ifdef _WIN32
            fp = open(ttfPath.c_str(), O_RDONLY | O_BINARY);
#else
            fp = open(ttfPath.c_str(), O_RDONLY);
#endif
            return fp;
        }
        i++;
    }
    return -1;
}

bool UIFontVectorStb::GetTtfInfo(uint8_t ttfId, uint8_t* ttfBuffer, uint32_t ttfBufferSize, TtfHeader& ttfHeader)
{
    if ((ttfBuffer == nullptr) || (ttfBufferSize == 0)) {
        return false;
    }
    for (int16_t i = 0; i < FONT_ID_MAX; i++) {
        if (fontInfo_[i].ttfName == nullptr) {
            continue;
        }
        if (fontInfo_[i].ttfId == ttfId) {
            if (fontInfo_[i].ttcIndex != FONT_TTC_MAX) {
                return GetTtfInfoFromTtc(ttfBuffer, ttfBufferSize, ttfHeader, fontInfo_[i]);
            } else {
                return GetTtfInfoFromTtf(ttfBuffer, ttfBufferSize, ttfHeader, fontInfo_[i]);
            }
        }
    }
    return false;
}

bool UIFontVectorStb::GetTtfInfoFromTtf(uint8_t* ttfBuffer,
                                        uint32_t ttfBufferSize,
                                        TtfHeader& ttfHeader,
                                        UITextLanguageFontParam fontInfo)
{
    if ((ttfBuffer == nullptr) || (ttfBufferSize == 0)) {
        return false;
    }
    std::string ttfPath = ttfDir_;
    ttfPath.append(fontInfo.ttfName);
    int32_t fpTtf = 0;
#ifdef _WIN32
    fpTtf = open(ttfPath.c_str(), O_RDONLY | O_BINARY);
#else
    fpTtf = open(ttfPath.c_str(), O_RDONLY);
#endif
    if (fpTtf < 0) {
        return false;
    }
    int32_t headerLength = lseek(fpTtf, 0, SEEK_END);
    if (headerLength < 0) {
        return false;
    }
    ttfHeader.len = static_cast<uint32_t>(headerLength);
    if (ttfHeader.len > ttfBufferSize) {
        close(fpTtf);
        return false;
    }
    int32_t ret = lseek(fpTtf, 0, SEEK_SET);
    if (ret != 0) {
        close(fpTtf);
        return false;
    }
    ret = read(fpTtf, reinterpret_cast<void*>(ttfBuffer), ttfHeader.len);
    if (ret != headerLength) {
        close(fpTtf);
        return false;
    }
    close(fpTtf);
    return true;
}

struct TtcHeader {
    uint32_t ttcTag;
    uint16_t major;
    uint16_t minor;
    int32_t numFonts;
};

bool UIFontVectorStb::GetTtfInfoFromTtc(uint8_t* ttfBuffer,
                                        uint32_t ttfBufferSize,
                                        TtfHeader& ttfHeader,
                                        UITextLanguageFontParam fontInfo)
{
    (void)ttfBuffer;
    (void)ttfBufferSize;
    (void)ttfHeader;
    (void)fontInfo;
    return false;
}

bool UIFontVectorStb::IsVectorFont() const
{
    return true;
}

uint8_t UIFontVectorStb::GetFontWeight(uint16_t fontId)
{
    if (fontId >= FONT_ID_MAX) {
        return BPP_BIT_8;
    }

    return fontInfo_[fontId].fontWeight;
}

int8_t UIFontVectorStb::SetFontPath(const char* path, FontType type)
{
    (void)type;
    if (path == nullptr) {
        return INVALID_RET_VALUE;
    }
    ttfDir_ = path;
    return RET_VALUE_OK;
}

int8_t UIFontVectorStb::GetFaceInfo(uint16_t fontId, uint8_t fontSize, FaceInfo& faceInfo)
{
    if ((fontId >= FONT_ID_MAX) || fontSize == 0 || stbFaces_[fontId] == nullptr) {
        return INVALID_RET_VALUE;
    }

    faceInfo.key = GetKey(fontId, fontSize);
    faceInfo.face = stbFaces_[fontId];

    if (fontSize_[fontId] == fontSize) {
        return RET_VALUE_OK;
    }
    const UITextLanguageFontParam* fontInfo = GetFontInfo(fontId);
    if ((fontInfo == nullptr) || (fontInfo->ttfName == nullptr)) {
        return INVALID_RET_VALUE;
    }

    // Set the size
    int8_t ret;
    if (IsEmojiFont(fontId)) {
        ret = SetupColorFont(stbFaces_[fontId], fontSize);
    } else {
#if CAPABILITY_HWDRAW_LETTER_VECTOR
        ret = SetupNormalFont(stbFaces_[fontId], faceInfo.face->unitsPerEm);
#else
        ret = SetupNormalFont(stbFaces_[fontId], fontSize * FONT_PIXEL_IN_POINT);
#endif
    }

    if (ret != 0) {
        return INVALID_RET_VALUE;
    }
    fontSize_[fontId] = fontSize;
    return RET_VALUE_OK;
}

uint16_t UIFontVectorStb::GetHeight(uint16_t fontId, uint8_t fontSize)
{
    FaceInfo faceInfo;
    int8_t ret = GetFaceInfo(fontId, fontSize, faceInfo);
    if (ret != RET_VALUE_OK) {
        return INVALID_RET_VALUE;
    }

    return static_cast<uint16_t>(faceInfo.face->metrics.height / FONT_PIXEL_IN_POINT);
}

uint16_t UIFontVectorStb::GetShapingFontId(char* text, uint8_t& ttfId,
    uint32_t& script, uint16_t fontId, uint8_t size) const
{
    (void)size;
#if defined(ENABLE_MULTI_FONT) && ENABLE_MULTI_FONT
    const UITextLanguageFontParam* fontParam1 = GetFontInfo(fontId);
    if (fontParam1 == nullptr) {
        return 0;
    }
    if (fontParam1->shaping == 0) {
        UIMultiFontManager* multiFontManager = UIMultiFontManager::GetInstance();
        if (!multiFontManager->IsNeedShaping(text, ttfId, script)) {
            return 0; // 0 means  no need to shape
        }
        uint16_t* searchLists = nullptr;
        int8_t length = multiFontManager->GetSearchFontList(fontId, &searchLists);
        const UITextLanguageFontParam* fontParam2 = nullptr;
        for (uint8_t i = 0; i < length; i++) {
            fontParam2 = GetFontInfo(searchLists[i]);
            if (fontParam2 == nullptr) {
                continue;
            }
            if (fontParam2->ttfId == ttfId) {
                return fontParam2->shaping;
            }
        }
        return 0;
    }
    ttfId = fontParam1->ttfId;

#if defined(ENABLE_SHAPING) && ENABLE_SHAPING
    script = UIMultiFontManager::GetInstance()->GetScriptByTtfId(ttfId);
#endif
    return fontParam1->shaping;
#else
    const UITextLanguageFontParam* fontInfo = GetFontInfo(fontId);
    if (fontInfo == nullptr) {
        return 0;
    }
    ttfId = fontInfo->ttfId;
    return fontInfo->shaping;
#endif
}

uint16_t UIFontVectorStb::GetFontId(const char* ttfName, uint8_t fontSize) const
{
    (void)fontSize;
    if (ttfName != nullptr) {
        int32_t i = 0;
        while (i < FONT_ID_MAX) {
            if ((fontInfo_[i].ttfName != nullptr) && (strstr(fontInfo_[i].ttfName, ttfName) != nullptr)) {
                return static_cast<uint16_t>(i);
            }
            i++;
        }
    }

    return static_cast<uint16_t>(FONT_ID_MAX);
}

uint16_t UIFontVectorStb::GetFontId(uint32_t unicode) const
{
    int32_t i = 0;
    uint8_t ttfId = ((unicode >> 24) & 0x1F); // 24: Whether 25 ~29 bit storage is ttfId 0x1F:5bit
    while (i < FONT_ID_MAX) {
        if (fontInfo_[i].ttfName == nullptr) {
            i++;
            continue;
        }
        if (fontInfo_[i].ttfId == ttfId) {
            return i;
        }
        i++;
    }
    return FONT_INVALID_TTF_ID;
}

int16_t UIFontVectorStb::GetWidth(uint32_t unicode, uint16_t fontId, uint8_t fontSize)
{
    if ((fontId >= FONT_ID_MAX) || (fontSize == 0)) {
        return INVALID_RET_VALUE;
    }

    GlyphNode node;
    int8_t ret = GetGlyphNode(unicode, node, fontId, fontSize);
    if (ret != RET_VALUE_OK) {
        return GetEmojiWidth(unicode, fontId, fontSize);
    }
    return node.advance;
}

int8_t UIFontVectorStb::GetFontHeader(FontHeader& fontHeader, uint16_t fontId, uint8_t fontSize)
{
    FaceInfo faceInfo;
    int8_t ret = GetFaceInfo(fontId, fontSize, faceInfo);
    if (ret != RET_VALUE_OK) {
        return INVALID_RET_VALUE;
    }

    fontHeader.ascender = static_cast<int16_t>(faceInfo.face->metrics.ascender / FONT_PIXEL_IN_POINT);
    fontHeader.descender = static_cast<int16_t>(faceInfo.face->metrics.descender / FONT_PIXEL_IN_POINT);
    fontHeader.fontHeight = static_cast<uint16_t>(faceInfo.face->metrics.height / FONT_PIXEL_IN_POINT);
    return RET_VALUE_OK;
}

void UIFontVectorStb::SaveGlyphNode(uint32_t unicode, uint16_t fontKey, Metric *metric)
{
    GlyphCacheNode* node = UIFontCacheManager::GetInstance()->GetNodeCacheSpace(unicode, fontKey);
    if (node == nullptr) {
        return;
    }
    node->node.left = metric->left;
    node->node.top = metric->top;
    node->node.cols = metric->cols;
    node->node.rows = metric->rows;
    node->node.advance = metric->advance;
    node->node.unicode = unicode;
    node->node.fontId = fontKey;
}

int8_t UIFontVectorStb::GetGlyphNode(uint32_t unicode, GlyphNode& glyphNode, uint16_t fontId, uint8_t fontSize)
{
    // get glyph from glyph cache
    uint16_t fontKey = GetKey(fontId, fontSize);
    UIFontCacheManager* fontCacheManager = UIFontCacheManager::GetInstance();
    GlyphCacheNode* cacheNode = fontCacheManager->GetNodeFromCache(unicode, fontKey, GlyphCacheType::CACHE_TYPE_NONE);
    if (cacheNode != nullptr) {
        glyphNode = cacheNode->node;
        return RET_VALUE_OK;
    }

#if defined(ENABLE_TEXT_STYLE) && ENABLE_TEXT_STYLE
    uint8_t* bitmap = fontCacheManager->GetBitmap(fontKey, unicode, glyphNode.textStyle);
#else
    uint8_t* bitmap = fontCacheManager->GetBitmap(fontKey, unicode, TEXT_STYLE_NORMAL);
#endif
    if (bitmap != nullptr) {
        Metric* f = reinterpret_cast<Metric*>(bitmap);
        glyphNode.left = f->left;
        glyphNode.top = f->top;
        glyphNode.cols = f->cols;
        glyphNode.rows = f->rows;
        glyphNode.advance = f->advance;
        glyphNode.fontId = fontId;

        SaveGlyphNode(unicode, fontKey, f);
        return RET_VALUE_OK;
    }

    FaceInfo faceInfo;
    int8_t ret = GetFaceInfo(fontId, fontSize, faceInfo);
    if (ret != RET_VALUE_OK) {
        return INVALID_RET_VALUE;
    }
    if (faceInfo.face == nullptr) {
        return INVALID_RET_VALUE;
    }

    int8_t error = LoadGlyphIntoFace(fontId, fontSize, unicode, glyphNode);
    if (error != RET_VALUE_OK) {
        return INVALID_RET_VALUE;
    }

    return RET_VALUE_OK;
}

uint8_t* UIFontVectorStb::GetBitmap(uint32_t unicode, GlyphNode& glyphNode, uint16_t fontId, uint8_t fontSize)
{
    uint16_t fontKey = GetKey(fontId, fontSize);
    UIFontCacheManager* fontCacheManager = UIFontCacheManager::GetInstance();
#if defined(ENABLE_TEXT_STYLE) && ENABLE_TEXT_STYLE
    uint8_t* bitmap = fontCacheManager->GetBitmap(fontKey, unicode, glyphNode.textStyle);
#else
    uint8_t* bitmap = fontCacheManager->GetBitmap(fontKey, unicode, TEXT_STYLE_NORMAL);
#endif
    if (bitmap != nullptr) {
        Metric* f = reinterpret_cast<Metric*>(bitmap);
        glyphNode.left = f->left;
        glyphNode.top = f->top;
        glyphNode.cols = f->cols;
        glyphNode.rows = f->rows;
        glyphNode.advance = f->advance;
        glyphNode.fontId = fontId;
        SaveGlyphNode(unicode, fontKey, f);
        return bitmap + sizeof(Metric);
    }
    FaceInfo faceInfo;
    int8_t ret = GetFaceInfo(fontId, fontSize, faceInfo);
    if (ret != RET_VALUE_OK) {
        return nullptr;
    }
    if (faceInfo.face == nullptr) {
        return nullptr;
    }

    int8_t error = LoadGlyphIntoFace(fontId, fontSize, unicode, glyphNode);
    if (error != RET_VALUE_OK) {
        return nullptr;
    }

#if defined(ENABLE_TEXT_STYLE) && ENABLE_TEXT_STYLE
    bitmap = fontCacheManager->GetBitmap(fontKey, unicode, glyphNode.textStyle);
#else
    bitmap = fontCacheManager->GetBitmap(fontKey, unicode, TEXT_STYLE_NORMAL);
#endif
    if (bitmap != nullptr) {
        return bitmap + sizeof(Metric);
    } else {
        return nullptr;
    }
}

bool UIFontVectorStb::IsEmojiFont(uint16_t fontId)
{
    if (fontId >= FONT_ID_MAX) {
        return false;
    }
    return (fontInfo_[fontId].fontWeight >= 16); // 16: rgb color font
}

bool UIFontVectorStb::StbLoadChar(StbFace *face, int32_t glyphIndex, uint8_t loadFlag)
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
    glyph->pixelMode = 0;

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

    glyph->bitmapBuffer = stbtt_GetGlyphBitmap(&face->stbFontInfo, face->metrics.xScale, face->metrics.yScale,
        glyphIndex, &glyph->bitmapWidth, &glyph->bitmapHeight, &glyph->bitmapLeft, &glyph->bitmapTop);

    return true;
}

int8_t UIFontVectorStb::LoadGlyphIntoFace(uint16_t& fontId, uint8_t fontSize, uint32_t unicode, GlyphNode& glyphNode)
{
    int32_t glyphIndex = 0;
    uint8_t loadFlag = STB_LOAD_RENDER;
    if (IsGlyphFont(unicode) != 0) {
        if (fontId >= FONT_ID_MAX || fontId != GetFontId(unicode)) {
            return INVALID_RET_VALUE;
        }
        glyphIndex = unicode & (0xFFFFFF);
    } else {
        glyphIndex = stbtt_FindGlyphIndex(&stbFaces_[fontId]->stbFontInfo, unicode);
        if (IsEmojiFont(fontId)) {
            loadFlag = STB_LOAD_COLOR;
        }
    }
    if (glyphIndex == 0) {
        return INVALID_RET_VALUE;
    }
    if (!StbLoadChar(stbFaces_[fontId], glyphIndex, loadFlag)) {
        return INVALID_RET_VALUE;
    }

    FaceInfo faceInfo;
    faceInfo.key = GetKey(fontId, fontSize);
    faceInfo.face = stbFaces_[fontId];

    glyphNode.left = faceInfo.face->glyph.bitmapLeft;
    glyphNode.top = faceInfo.face->glyph.bitmapTop;
    glyphNode.cols = faceInfo.face->glyph.bitmapWidth;
    glyphNode.rows = faceInfo.face->glyph.bitmapHeight;
    glyphNode.advance = static_cast<uint16_t>(faceInfo.face->glyph.advanceX / FONT_PIXEL_IN_POINT);
    glyphNode.fontId = fontId;

#if defined(ENABLE_TEXT_STYLE) && ENABLE_TEXT_STYLE
    SetFace(faceInfo, unicode, glyphNode.textStyle);
#else
    SetFace(faceInfo, unicode);
#endif
    return RET_VALUE_OK;
}

uint8_t UIFontVectorStb::IsGlyphFont(uint32_t unicode)
{
    uint16_t unicodeFontId = GetFontId(unicode);
    if (unicodeFontId == FONT_INVALID_TTF_ID) {
        return 0;
    } else {
        return fontInfo_[unicodeFontId].shaping;
    }
}

void UIFontVectorStb::SetFace(FaceInfo& faceInfo, uint32_t unicode)
{
    SetFace(faceInfo, unicode, TEXT_STYLE_NORMAL);
}

void UIFontVectorStb::SetFace(FaceInfo& faceInfo, uint32_t unicode, TextStyle textStyle)
{
    Metric* f = reinterpret_cast<Metric*>(UIMalloc(sizeof(Metric)));
    if (f == nullptr) {
        return;
    }
    f->advance = static_cast<uint16_t>(faceInfo.face->glyph.advanceX / FONT_PIXEL_IN_POINT);
    f->left = faceInfo.face->glyph.bitmapLeft;
    f->top = faceInfo.face->glyph.bitmapTop;
    f->cols = faceInfo.face->glyph.bitmapWidth;
    f->rows = faceInfo.face->glyph.bitmapHeight;

    // cache glyph
    SaveGlyphNode(unicode, faceInfo.key, f);

    int16_t pixSize;
    ColorMode mode;
    if (faceInfo.face->glyph.pixelMode == STB_PIXEL_MODE_BGRA) {
        pixSize = 0x04; // 4 Byte
        mode = ARGB8888;
    } else {
        pixSize = 1;
        mode = A8;
    }

    GlyphNode glyphNode;
    glyphNode.left = f->left;
    glyphNode.top = f->top;
    glyphNode.cols = f->cols;
    glyphNode.rows = f->rows;
    glyphNode.advance = f->advance;
    glyphNode.unicode = unicode;
    glyphNode.fontId = faceInfo.key;
    BufferInfo bufInfo = UIFontAllocator::GetCacheBuffer(faceInfo.key, unicode, mode, glyphNode, true, textStyle);
    uint32_t bitmapSize = bufInfo.stride * bufInfo.height;
    uint32_t rawSize = glyphNode.cols * glyphNode.rows * pixSize;

    if (bufInfo.virAddr != nullptr) {
        if (memcpy_s(bufInfo.virAddr, sizeof(Metric), f, sizeof(Metric)) != EOK) {
            UIFontCacheManager::GetInstance()->PutSpace(reinterpret_cast<uint8_t*>(bufInfo.virAddr));
            ClearFontGlyph(faceInfo.face);
            UIFree(f);
            return;
        }
        if ((faceInfo.face->glyph.bitmapBuffer != nullptr) &&
            (memcpy_s(reinterpret_cast<uint8_t*>(bufInfo.virAddr) + sizeof(Metric), bitmapSize,
                      faceInfo.face->glyph.bitmapBuffer, rawSize) != EOK)) {
            UIFontCacheManager::GetInstance()->PutSpace(reinterpret_cast<uint8_t*>(bufInfo.virAddr));
            ClearFontGlyph(faceInfo.face);
            UIFree(f);
            return;
        }
        UIFontAllocator::RearrangeBitmap(bufInfo, rawSize, true);
        ClearFontGlyph(faceInfo.face);
    }
    UIFree(f);
}

void UIFontVectorStb::ClearFontGlyph(StbFace *face)
{
    if ((face != nullptr) && (face->glyph.bitmapBuffer != nullptr)) {
        // free unicode buffer immediately to save memory in multi font file load
        // if not, it will be freed in next glyph load
        free(face->glyph.bitmapBuffer);
        face->glyph.bitmapBuffer = nullptr;
    }
}

inline uint16_t UIFontVectorStb::GetKey(uint16_t fontId, uint8_t size)
{
    return ((static_cast<uint16_t>(fontId)) << 8) + size; // fontId store at the (8+1)th bit
}

uint16_t UIFontVectorStb::GetOffsetPosY(const char* text,
                                        uint16_t lineLength,
                                        bool& isEmojiLarge,
                                        uint16_t fontId,
                                        uint8_t fontSize)
{
    uint32_t i = 0;
    uint16_t textNum = 0;
    uint16_t emojiNum = 0;
    uint16_t loopNum = 0;
    GlyphNode glyphNode;
    GlyphNode emojiMaxNode = {};
    uint8_t maxFontSize = fontSize;
    while (i < lineLength) {
        uint32_t unicode = TypedText::GetUTF8Next(text, i, i);
        uint8_t ret = GetGlyphNode(unicode, glyphNode, fontId, fontSize);
        if (ret == RET_VALUE_OK) {
            uint8_t weight = GetFontWeight(glyphNode.fontId);
            // 16: bit rgb565 rgba8888
            if (weight >= 16) {
                emojiMaxNode = glyphNode.rows > emojiMaxNode.rows ? glyphNode : emojiMaxNode;
                emojiNum++;
            } else {
                textNum++;
            }
            loopNum++;
        }
    }
    // The number of emoji is the same as the number of cycles, indicating that this line is all emoji
    // The number of words is the same as the number of cycles, which means that this line is all words
    if ((emojiNum == loopNum) || (textNum == loopNum)) {
        isEmojiLarge = true;
        return 0;
    }
    isEmojiLarge = emojiMaxNode.rows > maxFontSize;
    uint16_t offset = 0;
    if (isEmojiLarge) {
        // If the emoji is higher than the text
        if (emojiMaxNode.top >= maxFontSize) {
            offset = emojiMaxNode.top - maxFontSize;
        }
    } else {
        // If text are higher than emoji
        if (maxFontSize >= emojiMaxNode.rows) {
            offset = maxFontSize - emojiMaxNode.rows;
        }
    }
    return offset;
}

uint16_t UIFontVectorStb::GetLineMaxHeight(const char* text,
                                           uint16_t lineLength,
                                           uint16_t fontId,
                                           uint8_t fontSize,
                                           uint16_t& letterIndex,
                                           SpannableString* spannableString)
{
    uint32_t i = 0;
    uint16_t textNum = 0;
    uint16_t emojiNum = 0;
    uint16_t loopNum = 0;
    uint16_t maxHeight = GetHeight(fontId, fontSize);
    while (i < lineLength) {
        uint32_t unicode = TypedText::GetUTF8Next(text, i, i);
        TypedText::IsColourWord(unicode, fontId, fontSize) ? emojiNum++ : textNum++;
        loopNum++;
        if (spannableString != nullptr && spannableString->GetSpannable(letterIndex)) {
            int16_t spannableHeight = 0;
            spannableString->GetFontHeight(letterIndex, spannableHeight, fontId, fontSize);
            uint16_t tempHeight = static_cast<uint16_t>(spannableHeight);
            maxHeight = tempHeight > maxHeight ? tempHeight : maxHeight;
        }
        letterIndex++;
        if (i > 0 && ((text[i - 1] == '\r') || (text[i - 1] == '\n'))) {
            break;
        }
    }
    return GetMaxSubLineHeight(textNum, loopNum, maxHeight, emojiNum);
}

uint16_t UIFontVectorStb::GetMaxSubLineHeight(uint16_t textNum, uint16_t loopNum, uint16_t maxHeight, uint16_t emojiNum)
{
    // The number of words is the same as the number of cycles, which means that this line is all words
    if (textNum == loopNum) {
        for (uint8_t i = 0; i < currentFontInfoNum_; i++) {
            if (IsEmojiFont(i)) {
                continue;
            }
            uint16_t height = static_cast<uint16_t>(stbFaces_[i]->metrics.height / FONT_PIXEL_IN_POINT);
            if (height > maxHeight) {
                maxHeight = height;
            }
            return maxHeight;
        }
    }
    // The number of emoji is the same as the number of cycles, indicating that this line is all emoji
    if (emojiNum == loopNum) {
        for (uint8_t i = 0; i < currentFontInfoNum_; i++) {
            if (IsEmojiFont(i)) {
                return static_cast<uint16_t>(stbFaces_[i]->metrics.height / FONT_PIXEL_IN_POINT);
            }
        }
    }
    // A line has both emoji and words
    if ((textNum > 0) && (emojiNum > 0)) {
        for (uint8_t i = 0; i < currentFontInfoNum_; i++) {
            uint16_t tmpHeight = static_cast<uint16_t>(stbFaces_[i]->metrics.height / FONT_PIXEL_IN_POINT);
            maxHeight = tmpHeight > maxHeight ? tmpHeight : maxHeight;
        }
    }
    return maxHeight;
}

void UIFontVectorStb::SetPsramMemory(uintptr_t psramAddr, uint32_t psramLen)
{
    BaseFont::SetPsramMemory(psramAddr, psramLen);
    FontRamAllocator::GetInstance().SetRamAddr(psramAddr, psramLen);
}

int8_t UIFontVectorStb::SetCurrentLangId(uint8_t langId)
{
    (void)langId;
    FontRamAllocator::GetInstance().ClearRam();
#if defined(ENABLE_SHAPING) && ENABLE_SHAPING
    UITextShaping::GetInstance()->ClearTtfHeader();
#endif
    UIFontCacheManager* fontCacheManager = UIFontCacheManager::GetInstance();
    fontCacheManager->ClearCacheFlag();
    fontCacheManager->BitmapCacheClear();

    if (fontCacheManager->GlyphsCacheInit() != RET_VALUE_OK) {
        GRAPHIC_LOGE("UIFontCacheManager::GlyphsCacheInit init failed");
        return INVALID_RET_VALUE;
    }

    fontCacheManager->BitmapCacheInit();
    return RET_VALUE_OK;
}

bool UIFontVectorStb::GetEmoji(uint32_t unicode, ImageInfo &unicodeInfo)
{
    if (!IsEmojiSupport()) {
        return false;
    }
    const int len = GetEmojiTableLen();
    const EmojiTable *EMOJI_TABLE = GetEmojiTable();
    const char *EMOJI_PATH = GetEmojiPath();
    if (len == 0 || EMOJI_TABLE == nullptr || EMOJI_PATH == nullptr) {
        return false;
    }
    int index = -1;
    for (int i = 0; i < len; ++i) {
        if (EMOJI_TABLE[i].unicode == unicode) {
            index = i;
            break;
        }
    }
    if (index == -1) {
        return false;
    }
    ImageInfo *info = ImageCacheManager::GetInstance().LoadOneInMultiRes(EMOJI_TABLE[index].resId, EMOJI_PATH);
    if (info == nullptr) {
        return false;
    }

    memcpy_s(&unicodeInfo, sizeof(unicodeInfo), info, sizeof(ImageInfo));
    return true;
}

int16_t UIFontVectorStb::GetEmojiHeight(uint16_t fontId, uint8_t fontSize)
{
    return GetHeight(fontId, fontSize);
}

int16_t UIFontVectorStb::GetEmojiWidth(uint32_t unicode, uint16_t fontId, uint8_t fontSize)
{
    ImageInfo info;
    if (!GetEmoji(unicode, info)) {
        return INVALID_RET_VALUE;
    }
    float h = GetEmojiHeight(fontId, fontSize);
    float scale = h / info.header.height;
    float w = scale * info.header.width;
    return w;
}
} // namespace OHOS
