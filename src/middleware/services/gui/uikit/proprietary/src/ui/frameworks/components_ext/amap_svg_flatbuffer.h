/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026. All rights reserved.
 * Description: Lottie
 * Author: Hisi Graphic Team
 * Created: 2026-3
 */

#ifndef AMAP_SVG_FLATBUFFER_H_
#define AMAP_SVG_FLATBUFFER_H_

#include "flatbuffers/flatbuffers.h"

// Ensure the included flatbuffers.h is the same version as when this file was
// generated, otherwise it may not be compatible.
static_assert(FLATBUFFERS_VERSION_MAJOR == 24 && // 24: version
              FLATBUFFERS_VERSION_MINOR == 3 && // 3: version
              FLATBUFFERS_VERSION_REVISION == 25, // 25: version
             "Non-compatible flatbuffers version included");

namespace AMapSvgFlatBuffer {

enum SVGCommandType : uint8_t {
    SVG_CMD_CLOSE = 0,
    SVG_CMD_MOVE_TO = 2,
    SVG_CMD_LINE_TO = 4,
    SVG_CMD_HLINE_TO = 6,
    SVG_CMD_VLINE_TO = 8,
    SVG_CMD_QUARD_TO = 10,
    SVG_CMD_CUBIC_TO = 12,
    SVG_CMD_SQUARD_TO = 14,
    SVG_CMD_SCUBIC_TO = 16,
    SVG_MIN = SVG_CMD_CLOSE,
    SVG_MAX = SVG_CMD_SCUBIC_TO
};

inline const SVGCommandType (&EnumValuesSVGCommandType())[9]
{
    static const SVGCommandType VALUES[] = {
        SVG_CMD_CLOSE, SVG_CMD_MOVE_TO, SVG_CMD_LINE_TO,
        SVG_CMD_HLINE_TO, SVG_CMD_VLINE_TO, SVG_CMD_QUARD_TO,
        SVG_CMD_CUBIC_TO, SVG_CMD_SQUARD_TO, SVG_CMD_SCUBIC_TO
    };
    return VALUES;
}

inline const char * const *EnumNamesSVGCommandType()
{
    static const char * const NAMES[18] = {
        "CMD_CLOSE", "", "CMD_MOVE_TO", "", "CMD_LINE_TO", "",
        "CMD_HLINE_TO", "", "CMD_VLINE_TO", "", "CMD_QUARD_TO", "",
        "CMD_CUBIC_TO", "", "CMD_SQUARD_TO", "", "CMD_SCUBIC_TO",
        nullptr
    };
    return NAMES;
}

inline const char *EnumNameSVGCommandType(SVGCommandType e)
{
    if (::flatbuffers::IsOutRange(e, SVG_CMD_CLOSE, SVG_CMD_SCUBIC_TO)) {
        return "";
    }
    const size_t index = static_cast<size_t>(e);
    return EnumNamesSVGCommandType()[index];
}

enum LayerData : uint8_t {
    LAYER_DATA_NONE = 0,
    LAYER_DATA_SVGLAYER = 1,
    LAYER_DATA_MIN = LAYER_DATA_NONE,
    LAYER_DATA_MAX = LAYER_DATA_SVGLAYER
};

inline const LayerData (&EnumValuesLayerData())[2]
{
    static const LayerData VALUES[] = {
        LAYER_DATA_NONE,
        LAYER_DATA_SVGLAYER
    };
    return VALUES;
}

inline const char * const *EnumNamesLayerData()
{
    static const char * const NAMES[3] = {
        "NONE",
        "SVGLayer",
        nullptr
    };
    return NAMES;
}

inline const char *EnumNameLayerData(LayerData e)
{
    if (::flatbuffers::IsOutRange(e, LAYER_DATA_NONE, LAYER_DATA_SVGLAYER)) {
        return "";
    }
    const size_t index = static_cast<size_t>(e);
    return EnumNamesLayerData()[index];
}

bool VerifyLayerData(::flatbuffers::Verifier &verifier, const void *obj, LayerData type);

struct Color FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
    enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
        VT_RED = 4,
        VT_GREEN = 6,
        VT_BLUE = 8,
        VT_ALPHA = 10
    };
    uint8_t red() const
    {
        return GetField<uint8_t>(VT_RED, 0);
    }
    uint8_t green() const
    {
        return GetField<uint8_t>(VT_GREEN, 0);
    }
    uint8_t blue() const
    {
        return GetField<uint8_t>(VT_BLUE, 0);
    }
    float alpha() const
    {
        return GetField<float>(VT_ALPHA, 1.0f);
    }
    bool Verify(::flatbuffers::Verifier &verifier) const
    {
        return VerifyTableStart(verifier) &&
            VerifyField<uint8_t>(verifier, VT_RED, 1) &&
            VerifyField<uint8_t>(verifier, VT_GREEN, 1) &&
            VerifyField<uint8_t>(verifier, VT_BLUE, 1) &&
            VerifyField<float>(verifier, VT_ALPHA, 4) &&
            verifier.EndTable();
    }
};

struct Path FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
    enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
        VT_D = 4
    };
    const ::flatbuffers::Vector<uint8_t> *d() const
    {
        return GetPointer<const ::flatbuffers::Vector<uint8_t> *>(VT_D);
    }
    bool Verify(::flatbuffers::Verifier &verifier) const
    {
        return VerifyTableStart(verifier) &&
            VerifyOffset(verifier, VT_D) &&
            verifier.VerifyVector(d()) &&
            verifier.EndTable();
    }
};

struct Group FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
    enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
        VT_PATHS = 4,
        VT_FILL = 6,
        VT_STROKE = 8,
        VT_STROKE_WIDTH = 10,
        VT_STROKE_LINEJOIN = 12,
        VT_STROKE_LINECAP = 14,
        VT_STROKE_DASHARRAY = 16
    };
    const ::flatbuffers::Vector<::flatbuffers::Offset<AMapSvgFlatBuffer::Path>> *paths() const
    {
        return GetPointer<const ::flatbuffers::Vector<::flatbuffers::Offset<AMapSvgFlatBuffer::Path>> *>(VT_PATHS);
    }
    const AMapSvgFlatBuffer::Color *fill() const
    {
        return GetPointer<const AMapSvgFlatBuffer::Color *>(VT_FILL);
    }
    const AMapSvgFlatBuffer::Color *stroke() const
    {
        return GetPointer<const AMapSvgFlatBuffer::Color *>(VT_STROKE);
    }
    uint8_t stroke_width() const
    {
        return GetField<uint8_t>(VT_STROKE_WIDTH, 0);
    }
    const ::flatbuffers::String *stroke_linejoin() const
    {
        return GetPointer<const ::flatbuffers::String *>(VT_STROKE_LINEJOIN);
    }
    const ::flatbuffers::String *stroke_linecap() const
    {
        return GetPointer<const ::flatbuffers::String *>(VT_STROKE_LINECAP);
    }
    const ::flatbuffers::Vector<uint8_t> *stroke_dasharray() const
    {
        return GetPointer<const ::flatbuffers::Vector<uint8_t> *>(VT_STROKE_DASHARRAY);
    }
    bool Verify(::flatbuffers::Verifier &verifier) const
    {
        return VerifyTableStart(verifier) &&
            VerifyOffset(verifier, VT_PATHS) &&
            verifier.VerifyVector(paths()) &&
            verifier.VerifyVectorOfTables(paths()) &&
            VerifyOffset(verifier, VT_FILL) &&
            verifier.VerifyTable(fill()) &&
            VerifyOffset(verifier, VT_STROKE) &&
            verifier.VerifyTable(stroke()) &&
            VerifyField<uint8_t>(verifier, VT_STROKE_WIDTH, 1) &&
            VerifyOffset(verifier, VT_STROKE_LINEJOIN) &&
            verifier.VerifyString(stroke_linejoin()) &&
            VerifyOffset(verifier, VT_STROKE_LINECAP) &&
            verifier.VerifyString(stroke_linecap()) &&
            VerifyOffset(verifier, VT_STROKE_DASHARRAY) &&
            verifier.VerifyVector(stroke_dasharray()) &&
            verifier.EndTable();
    }
};

struct ViewBox FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
    enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
        VT_MIN_X = 4,
        VT_MIN_Y = 6,
        VT_MAX_X = 8,
        VT_MAX_Y = 10
    };
    uint16_t min_x() const
    {
        return GetField<uint16_t>(VT_MIN_X, 0);
    }
    uint16_t min_y() const
    {
        return GetField<uint16_t>(VT_MIN_Y, 0);
    }
    uint16_t max_x() const
    {
        return GetField<uint16_t>(VT_MAX_X, 0);
    }
    uint16_t max_y() const
    {
        return GetField<uint16_t>(VT_MAX_Y, 0);
    }
    bool Verify(::flatbuffers::Verifier &verifier) const
    {
        return VerifyTableStart(verifier) &&
               VerifyField<uint16_t>(verifier, VT_MIN_X, 2) &&
               VerifyField<uint16_t>(verifier, VT_MIN_Y, 2) &&
               VerifyField<uint16_t>(verifier, VT_MAX_X, 2) &&
               VerifyField<uint16_t>(verifier, VT_MAX_Y, 2) &&
               verifier.EndTable();
    }
};

struct Rect FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
    enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
        VT_WIDTH = 4,
        VT_HEIGHT = 6,
        VT_FILL = 8
    };
    const ::flatbuffers::String *width() const
    {
        return GetPointer<const ::flatbuffers::String *>(VT_WIDTH);
    }
    const ::flatbuffers::String *height() const
    {
        return GetPointer<const ::flatbuffers::String *>(VT_HEIGHT);
    }
    const AMapSvgFlatBuffer::Color *fill() const
    {
        return GetPointer<const AMapSvgFlatBuffer::Color *>(VT_FILL);
    }
    bool Verify(::flatbuffers::Verifier &verifier) const
    {
        return VerifyTableStart(verifier) &&
               VerifyOffset(verifier, VT_WIDTH) &&
               verifier.VerifyString(width()) &&
               VerifyOffset(verifier, VT_HEIGHT) &&
               verifier.VerifyString(height()) &&
               VerifyOffset(verifier, VT_FILL) &&
               verifier.VerifyTable(fill()) &&
               verifier.EndTable();
    }
};

struct SVGLayer FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
    enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
        VT_WIDTH = 4,
        VT_HEIGHT = 6,
        VT_VIEW_BOX = 8,
        VT_PRESERVE_ASPECT_RATIO = 10,
        VT_RECT = 12,
        VT_GROUPS = 14
    };
    float width() const
    {
        return GetField<float>(VT_WIDTH, 0.0f);
    }
    float height() const
    {
        return GetField<float>(VT_HEIGHT, 0.0f);
    }
    const AMapSvgFlatBuffer::ViewBox *view_box() const
    {
        return GetPointer<const AMapSvgFlatBuffer::ViewBox *>(VT_VIEW_BOX);
    }
    const ::flatbuffers::String *preserve_aspect_ratio() const
    {
        return GetPointer<const ::flatbuffers::String *>(VT_PRESERVE_ASPECT_RATIO);
    }
    const AMapSvgFlatBuffer::Rect *rect() const
    {
        return GetPointer<const AMapSvgFlatBuffer::Rect *>(VT_RECT);
    }
    const ::flatbuffers::Vector<::flatbuffers::Offset<AMapSvgFlatBuffer::Group>> *groups() const
    {
        return GetPointer<const ::flatbuffers::Vector<::flatbuffers::Offset<AMapSvgFlatBuffer::Group>> *>(VT_GROUPS);
    }
    bool Verify(::flatbuffers::Verifier &verifier) const
    {
        return VerifyTableStart(verifier) &&
               VerifyField<float>(verifier, VT_WIDTH, 4) &&
               VerifyField<float>(verifier, VT_HEIGHT, 4) &&
               VerifyOffset(verifier, VT_VIEW_BOX) &&
               verifier.VerifyTable(view_box()) &&
               VerifyOffset(verifier, VT_PRESERVE_ASPECT_RATIO) &&
               verifier.VerifyString(preserve_aspect_ratio()) &&
               VerifyOffset(verifier, VT_RECT) &&
               verifier.VerifyTable(rect()) &&
               VerifyOffset(verifier, VT_GROUPS) &&
               verifier.VerifyVector(groups()) &&
               verifier.VerifyVectorOfTables(groups()) &&
               verifier.EndTable();
    }
};

struct Layer FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
    enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
        VT_Z = 4,
        VT_X = 6,
        VT_Y = 8,
        VT_DATA_TYPE = 10,
        VT_DATA = 12,
        VT_OPEN_PLATFORM_VERSION = 14,
        VT_DATA_PROTOCOL_VERSION = 16
    };
    uint32_t z() const
    {
        return GetField<uint32_t>(VT_Z, 0);
    }
    uint32_t x() const
    {
        return GetField<uint32_t>(VT_X, 0);
    }
    uint32_t y() const
    {
        return GetField<uint32_t>(VT_Y, 0);
    }
    AMapSvgFlatBuffer::LayerData data_type() const
    {
        return static_cast<AMapSvgFlatBuffer::LayerData>(GetField<uint8_t>(VT_DATA_TYPE, 0));
    }
    const void *data() const
    {
        return GetPointer<const void *>(VT_DATA);
    }
    template<typename T> const T *data_as() const;
    const AMapSvgFlatBuffer::SVGLayer *data_as_SVGLayer() const
    {
        return data_type() ==
            AMapSvgFlatBuffer::LAYER_DATA_SVGLAYER ? static_cast<const AMapSvgFlatBuffer::SVGLayer *>(data()) : nullptr;
    }
    const ::flatbuffers::String *open_platform_version() const
    {
        return GetPointer<const ::flatbuffers::String *>(VT_OPEN_PLATFORM_VERSION);
    }
    const ::flatbuffers::String *data_protocol_version() const
    {
        return GetPointer<const ::flatbuffers::String *>(VT_DATA_PROTOCOL_VERSION);
    }
    bool Verify(::flatbuffers::Verifier &verifier) const
    {
        return VerifyTableStart(verifier) &&
               VerifyField<uint32_t>(verifier, VT_Z, 4) &&
               VerifyField<uint32_t>(verifier, VT_X, 4) &&
               VerifyField<uint32_t>(verifier, VT_Y, 4) &&
               VerifyField<uint8_t>(verifier, VT_DATA_TYPE, 1) &&
               VerifyOffset(verifier, VT_DATA) &&
               VerifyLayerData(verifier, data(), data_type()) &&
               VerifyOffset(verifier, VT_OPEN_PLATFORM_VERSION) &&
               verifier.VerifyString(open_platform_version()) &&
               VerifyOffset(verifier, VT_DATA_PROTOCOL_VERSION) &&
               verifier.VerifyString(data_protocol_version()) &&
               verifier.EndTable();
    }
};

template<> inline const AMapSvgFlatBuffer::SVGLayer *Layer::data_as<AMapSvgFlatBuffer::SVGLayer>() const
{
    return data_as_SVGLayer();
}

struct Tile FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
    enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
        VT_X = 4,
        VT_Y = 6,
        VT_Z = 8,
        VT_LAYERS = 10,
        VT_VERSION = 12
    };
    uint32_t x() const
    {
        return GetField<uint32_t>(VT_X, 0);
    }
    uint32_t y() const
    {
        return GetField<uint32_t>(VT_Y, 0);
    }
    uint32_t z() const
    {
        return GetField<uint32_t>(VT_Z, 0);
    }
    const ::flatbuffers::Vector<::flatbuffers::Offset<AMapSvgFlatBuffer::Layer>> *layers() const
    {
        return GetPointer<const ::flatbuffers::Vector<::flatbuffers::Offset<AMapSvgFlatBuffer::Layer>> *>(VT_LAYERS);
    }
    const ::flatbuffers::String *version() const
    {
        return GetPointer<const ::flatbuffers::String *>(VT_VERSION);
    }
    bool Verify(::flatbuffers::Verifier &verifier) const
    {
        return VerifyTableStart(verifier) &&
               VerifyField<uint32_t>(verifier, VT_X, 4) &&
               VerifyField<uint32_t>(verifier, VT_Y, 4) &&
               VerifyField<uint32_t>(verifier, VT_Z, 4) &&
               VerifyOffset(verifier, VT_LAYERS) &&
               verifier.VerifyVector(layers()) &&
               verifier.VerifyVectorOfTables(layers()) &&
               VerifyOffset(verifier, VT_VERSION) &&
               verifier.VerifyString(version()) &&
               verifier.EndTable();
    }
};

inline bool VerifyLayerData(::flatbuffers::Verifier &verifier, const void *obj, LayerData type)
{
    switch (type) {
        case LAYER_DATA_NONE: {
            return true;
        }
        case LAYER_DATA_SVGLAYER: {
            auto ptr = reinterpret_cast<const AMapSvgFlatBuffer::SVGLayer *>(obj);
            return verifier.VerifyTable(ptr);
        }
        default: return true;
    }
}

inline const AMapSvgFlatBuffer::Tile *GetTile(const void *buf)
{
    return ::flatbuffers::GetRoot<AMapSvgFlatBuffer::Tile>(buf);
}

inline const AMapSvgFlatBuffer::Tile *GetSizePrefixedTile(const void *buf)
{
    return ::flatbuffers::GetSizePrefixedRoot<AMapSvgFlatBuffer::Tile>(buf);
}

inline bool VerifyTileBuffer(
    ::flatbuffers::Verifier &verifier)
{
    return verifier.VerifyBuffer<AMapSvgFlatBuffer::Tile>(nullptr);
}
}

#endif  // AMAP_SVG_FLATBUFFER_H_
