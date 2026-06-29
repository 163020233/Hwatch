/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026. All rights reserved.
 * Description: AMapSVGParser
 * Author:
 * Create: 2026-01
 */

#ifndef AMAP_SVG_PARSER_H
#define AMAP_SVG_PARSER_H
#include <cstdint>
#include <vector>
#include <memory>
#include <string>
#include "amap_svg_flatbuffer.h"
#include "common/graphic_hardware_types.h"
#include "gfx_utils/color.h"
#include "gfx_utils/rect.h"

namespace OHOS {
enum StrokeCap : uint8_t {
    STROKE_CAP_BUTT = 0,
    STROKE_CAP_ROUND = 1,
    STROKE_CAP_SQUARE = 2
};

enum StrokeJoin : uint8_t {
    STROKE_JOIN_MITER = 0,
    STROKE_JOIN_ROUND = 1,
    STROKE_JOIN_BEVEL = 2
};

struct AMapPathStyleBase {
    uint8_t strokeCap : 4;
    uint8_t strokeJoin : 4;
    Color32 strokeColor;
    Color32 fillColor;
    float strokeWidth;
    uint32_t dashCount;
};

struct AMapPathStyle {
    AMapPathStyleBase pathStyleBase;
    float *dashLengths;
};

struct AMapPathBase {
    uint8_t type;
    uint16_t pathStyleIndex;
    uint16_t cmdNum;
    uint16_t dataNum;
};

struct AMapPathDetail {
    AMapPathBase base;
    uint8_t* cmd;
    float* data;
};

struct SVGBaseInfo {
    Rect viewBox = {0, 0, 0, 0};
    Color32 bgColor = {0, 0, 0, 0};
};

class AMapSVGParser {
public:
    AMapSVGParser();
    ~AMapSVGParser();

    bool SetSVG(uint8_t* data, uint32_t length);
    void ClearSVG();

    Color32 GetBackGroundColor(void);
    int16_t GetWidth();
    int16_t GetHeight();

    uint32_t GetPaths(AMapPathDetail*& pathList);
    const AMapPathStyle* GetPathStyleByIndex(uint16_t index);

    AMapSVGParser(const AMapSVGParser&) = delete;
    AMapSVGParser& operator=(const AMapSVGParser&) = delete;

private:
    uint32_t pathCount_ = 0;
    uint16_t styleCount_ = 0;
    AMapPathDetail* pathArray_ = nullptr;
    AMapPathStyle* styleArray_ = nullptr;
    uint8_t *pathDataAddr_ = nullptr;
    uint32_t pathDataSize_ = 0;
    uint8_t curPathStyle_;
    SVGBaseInfo baseInfo_;

    struct CachedPath {
        std::vector<uint8_t> cmdVec;
        std::vector<float> dataVec;
        int styleIndex;
        uint8_t styleType;
    };
    std::vector<CachedPath> cachedPaths_;

    // 辅助函数
    bool VerifyAndGetSvgLayer(const void* data, size_t length,
                              const AMapSvgFlatBuffer::SVGLayer*& outSvgLayer);
    void ParseBaseInfo(const AMapSvgFlatBuffer::SVGLayer* svgLayer);
    bool ParseGroups(const AMapSvgFlatBuffer::SVGLayer* svgLayer);
    bool ParseSingleGroup(const AMapSvgFlatBuffer::Group* group);
    int AddStyle(const AMapSvgFlatBuffer::Group* group);  // 直接添加样式，返回索引
    bool ParsePathData(const flatbuffers::Vector<uint8_t>& d,
        std::vector<uint8_t>& outCmd, std::vector<float>& outData);
    bool RestorePathData(std::vector<CachedPath> &cachedPaths, uint32_t expBufferSize, uint16_t pathNum);

    static Color32 ConvertColor(const AMapSvgFlatBuffer::Color* fbColor);
    static uint8_t MapLineCap(const std::string& cap);
    static uint8_t MapLineJoin(const std::string& join);
    static int GetParamCount(PathCmd cmd);
};
}
#endif // AMAP_SVG_PARSER_H
