/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Create Date : 2025-11
 */

#include "map_render_manager.h"
#include <climits>
#include <common/image.h>
#include <cstdint>
#include <los_tick.h>
#include <cstdio>
#include <fcntl.h>
#include <unistd.h>
#include "amap_log_adapter.h"
#include "gfx_utils/mem_api.h"
#include "gfx_utils/rect.h"
#include "map_utils.h"
#include "buffer_cache_manager.h"
#include "hals/gralloc_engines.h"

using OHOS::Rect;

#define USE_VE_DECODE 1  // 1: 地图走硬解；0: 走软解

/* 当前只有一个线程处理地图，不会出现冲突，后面扩展需要注意 */
typedef struct {
    char key[KEY_MAX_LEN];
    bool valid;
} MapTileKeyInfo;

static MapTileKeyInfo g_mapCurImageKey{};

static MapTileKeyInfo g_mapCurSvgKey{};

static constexpr int DASH_ARRAY_SIZE = 2;
static constexpr int HALF_DIVISOR = 2;
static constexpr float FULL_ANGLE_DEGREE = 360.0f;

static char *MapGetCurImgKey()
{
    return g_mapCurImageKey.valid ? g_mapCurImageKey.key : nullptr;
}

static char *MapGetCurTileKey()
{
    return g_mapCurSvgKey.valid ? g_mapCurSvgKey.key : nullptr;
}

namespace OHOS {
static BufferCache g_mapDecImageCache;

Rect MalRectToExpandedOHOSRect(mal_rect_area_t area)
{
    // 左右各扩 1 像素，上下各扩 1 像素，总尺寸增加 2 像素
    Rect area_t(area.x - 1, area.y - 1, area.x + area.width + 1, area.y + area.height + 1);
    return area_t;
}

Rect MalRectToOHOSRect(mal_rect_area_t area)
{
    Rect area_t(area.x, area.y, area.x + area.width - 1, area.y + area.height - 1);
    return area_t;
}

void convertFillStyleToPaintType(const mal_paint_style_t *style, IMapRender::MapPaintStyle &style_t)
{
    switch (style->fill_style) {
        case MAL_FILL_STYLE_DRAWING_ONLY:
            style_t.paintType = Paint::PaintStyle::FILL_STYLE;
            break;
        case MAL_FILL_STYLE_STROKE_ONLY:
            style_t.paintType = Paint::PaintStyle::STROKE_STYLE;
            break;
        case MAL_FILL_STYLE_DRAWING_AND_STROKE:
            style_t.paintType = Paint::PaintStyle::STROKE_FILL_STYLE;
            break;
        default:
            style_t.paintType = Paint::PaintStyle::FILL_STYLE;
            break;
    }
}

MapRenderManager::~MapRenderManager()
{
    g_mapDecImageCache.FreeAllRes();
}

MapRenderManager::MapViewEntry MapRenderManager::mapViews[MAX_MAP_VIEW_COUNT]{};

void MapRenderManager::AddMapView(int32_t mapId, IMapRender *view)
{
    AMAP_LOGD("AddMapView with mapId %d, view %p", mapId, view);
    // 先查找是否已存在相同 mapId，存在则更新
    for (int i = 0; i < MAX_MAP_VIEW_COUNT; ++i) {
        if (mapViews[i].used && mapViews[i].mapId == mapId) {
            mapViews[i].view = view;
            AMAP_LOGI("Updated existing mapId %d", mapId);
            return;
        }
    }

    // 未找到，尝试插入到空闲槽
    for (int i = 0; i < MAX_MAP_VIEW_COUNT; ++i) {
        if (!mapViews[i].used) {
            mapViews[i].mapId = mapId;
            mapViews[i].view = view;
            mapViews[i].used = true;
            AMAP_LOGD("Inserted mapId %d at slot %d", mapId, i);
            return;
        }
    }

    // 没有空闲槽了
    AMAP_LOGE("AddMapView failed: no free slot for mapId %d (max=%d)", mapId, MAX_MAP_VIEW_COUNT);
}

IMapRender *MapRenderManager::GetMapView(int32_t mapId)
{
    for (int i = 0; i < MAX_MAP_VIEW_COUNT; ++i) {
        if (mapViews[i].used && mapViews[i].mapId == mapId) {
            return mapViews[i].view;
        }
    }
    AMAP_LOGD("GetMapView: mapId %d NOT FOUND! (total slots: %d)", mapId, MAX_MAP_VIEW_COUNT);
    return nullptr;
}

void MapRenderManager::RemoveMapView(int32_t mapId)
{
    for (int i = 0; i < MAX_MAP_VIEW_COUNT; ++i) {
        if (mapViews[i].used && mapViews[i].mapId == mapId) {
            mapViews[i].used = false;
            // 可选：清空字段（非必须）
            mapViews[i].mapId = 0;
            mapViews[i].view = nullptr;
            AMAP_LOGI("Removed mapId %d from slot %d", mapId, i);
            return;
        }
    }
    AMAP_LOGW("RemoveMapView: mapId %d not found", mapId);
}

void MapRenderManager::ClearAll()
{
    for (int i = 0; i < MAX_MAP_VIEW_COUNT; ++i) {
        mapViews[i].used = false;
        mapViews[i].mapId = 0;
        mapViews[i].view = nullptr;
    }
    AMAP_LOGI("Cleared all map views");
}

void MapRenderManager::BeginDraw(uint32_t map_id, mal_render_context_t status)
{
    AMAP_LOGD("MapRenderManager::BeginDraw");
    IMapRender *mapView = GetMapView(map_id);
    if (!mapView) {
        AMAP_LOGE("mapView is null");
        return;
    }

    Rect area_t = MalRectToOHOSRect(status.map_view_rect);
    mapView->BeginDraw(status.width, status.height, area_t);
}

void MapRenderManager::CommitDraw(uint32_t map_id)
{
    AMAP_LOGD("MapRenderManager::CommitDraw");
    IMapRender *mapView = GetMapView(map_id);
    if (!mapView) {
        AMAP_LOGE("mapView is null");
        return;
    }

    g_mapDecImageCache.ReleaseUsed();
    // 直接调视图的刷新
    mapView->CommitDraw();
}

static inline float mod(float x, float n)
{
    return x - floor(x / n) * n;
}

static float CounterClockwiseToClockwise(float angle)
{
    return FULL_ANGLE_DEGREE - mod(angle, FULL_ANGLE_DEGREE);
}

void MapRenderManager::DrawPoints(uint32_t map_id, mal_point_t *point, uint32_t point_size,
                                  const mal_paint_style_t *style)
{
    if (!style) {
        AMAP_LOGE("style is null");
        return;
    }
    IMapRender *mapView = GetMapView(map_id);
    if (!mapView) {
        AMAP_LOGE("mapView is null");
        return;
    }
    if (point_size <= 0 || !point || !style) {
        AMAP_LOGE("DrawPoints failed with invalid params");
        return;
    }
    Point *points = (Point *)UIMalloc(sizeof(Point) * point_size);
    if (!points) {
        AMAP_LOGE("DrawPoints failed with malloc mem null");
        return;
    }
    for (size_t i = 0; i < point_size; i++) {
        points[i].x = point[i].x;
        points[i].y = point[i].y;
    }

    IMapRender::PointStyle style_t{};
    style_t.radius = style->width;
    style_t.color.full = style->color;
    mapView->DrawPoints(points, point_size, &style_t);

    UIFree(points);
}

void MapRenderManager::DrawPolyline(uint32_t map_id, mal_point_t *points, uint32_t point_size,
                                    const mal_paint_style_t *style)
{
    AMAP_LOGD("MapRenderManager::DrawPolyline");
    if (!style) {
        AMAP_LOGE("style is null");
        return;
    }
    IMapRender *mapView = GetMapView(map_id);
    if (!mapView) {
        AMAP_LOGE("mapView is null");
        return;
    }
    Point *points_t = (Point *)UIMalloc(point_size * sizeof(Point));
    if (!points_t) {
        AMAP_LOGE("malloc failed");
        return;
    }
    for (uint32_t i = 0; i < point_size; i++) {
        points_t[i].x = points[i].x;
        points_t[i].y = points[i].y;
    }
    IMapRender::StrokeStyle style_t{};
    float dashArr[DASH_ARRAY_SIZE] = {(float)style->dash_style.painted_length,
                                      (float)style->dash_style.unpainted_length};
    if (style->dash_style.painted_length != 0 && style->dash_style.unpainted_length != 0) {
        style_t.dashCount = DASH_ARRAY_SIZE;
        style_t.dashArray = dashArr;
        style_t.dashOffset = style->dash_style.offset;
    }
    style_t.strokeColor.full = style->color;
    style_t.strokeWidth = style->width;

    mapView->DrawPolyline(points_t, point_size, &style_t);
    UIFree(points_t);
    AMAP_LOGD("MapRenderManager::DrawPolyline end");
}

void MapRenderManager::DrawPolygon(uint32_t map_id, mal_point_t *points, uint32_t point_size,
                                   const mal_paint_style_t *style)
{
    if (!style) {
        AMAP_LOGE("style is null");
        return;
    }
    IMapRender *mapView = GetMapView(map_id);
    if (!mapView) {
        AMAP_LOGE("mapView is null");
        return;
    }
    Point *points_t = (Point *)UIMalloc(point_size * sizeof(Point));
    if (!points_t) {
        AMAP_LOGE("malloc failed");
        return;
    }
    for (uint32_t i = 0; i < point_size; i++) {
        points_t[i].x = points[i].x;
        points_t[i].y = points[i].y;
    }
    IMapRender::MapPaintStyle style_t{};
    float dashArr[DASH_ARRAY_SIZE] = {(float)style->dash_style.painted_length,
                                      (float)style->dash_style.unpainted_length};
    if (style->dash_style.painted_length != 0 && style->dash_style.unpainted_length != 0) {
        style_t.strokeStyle.dashCount = DASH_ARRAY_SIZE;
        style_t.strokeStyle.dashArray = dashArr;
        style_t.strokeStyle.dashOffset = style->dash_style.offset;
    }
    style_t.strokeStyle.strokeColor.full = style->color;
    style_t.strokeStyle.strokeWidth = style->width;
    style_t.fillColor.full = style->color;
    convertFillStyleToPaintType(style, style_t);
    mapView->DrawPolygon(points_t, point_size, &style_t);
    UIFree(points_t);
}

void MapRenderManager::DrawImage(uint32_t map_id, mal_rect_area_t area, const mal_image_buf_info_t *image_info,
                                 const mal_paint_style_t *style)
{
    AMAP_LOGD("MapRenderManager::DrawImage");
    IMapRender *mapView = GetMapView(map_id);
    if (!mapView || !image_info || !style) {
        AMAP_LOGE("input is null mapView = %p", mapView);
        return;
    }
    if (static_cast<IMapRender::ImageType>(image_info->image_format) != OHOS::IMapRender::ImageType::IMG_TYPE_JPEG) {
        AMAP_LOGE("DrawImage failed with not support image");
        return;
    }
    IMapRender::ImageBufferInfo imageInfo{};
    imageInfo.buffer = image_info->buffer;
    imageInfo.bufferSize = image_info->buffer_size;
    imageInfo.imageType = static_cast<IMapRender::ImageType>(image_info->image_format);
    imageInfo.uuid = MapGetCurImgKey();

    Rect area_t = MalRectToExpandedOHOSRect(area);
    mapView->DrawImage(&imageInfo, area_t, CounterClockwiseToClockwise(style->angle));
}

void MapRenderManager::DrawBitmap(uint32_t map_id, mal_rect_area_t area, mal_bitmap_t bitmap,
                                  const mal_paint_style_t *style)
{
    AMAP_LOGD("MapRenderManager::DrawBitmap bitmap.buffer_size = %d", bitmap.buffer_size);
    if (!style) {
        AMAP_LOGE("style is null");
        return;
    }
    IMapRender *mapView = GetMapView(map_id);
    if (!mapView) {
        AMAP_LOGE("mapView is null");
        return;
    }

    IMapRender::BitmapBufferInfo bit_map{};

    bit_map.buffer = bitmap.buffer;
    bit_map.bufferSize = bitmap.buffer_size;
    bit_map.height = bitmap.height;
    bit_map.colorMode = OHOS::ConvertPixelModeToColorMode(bitmap.pixel_mode);
    bit_map.compressMode = 0;
    bit_map.preMultiplied = bitmap.pre_multiplied;
    bit_map.stride = bitmap.stride * bitmap.width;

    bit_map.width = bitmap.width;
    bit_map.uuid = 0;
    AMAP_LOGD("DrawBitmap bitmap.height = %d, bitmap.width = %d", bitmap.height, bitmap.width);
    AMAP_LOGD("DrawBitmap x = %d, y = %d, w = %d, h = %d", area.x, area.y, area.x + area.width - 1,
             area.y + area.height - 1);
    Rect area_t = MalRectToOHOSRect(area);

    mapView->DrawBitmap(&bit_map, area_t, CounterClockwiseToClockwise(style->angle));
}

void MapRenderManager::DrawText(uint32_t map_id, mal_point_t center, const char *text, const mal_paint_style_t *style)
{
    if (!style) {
        AMAP_LOGE("style is null");
        return;
    }
    IMapRender *mapView = GetMapView(map_id);
    if (!mapView) {
        AMAP_LOGE("mapView is null");
        return;
    }
    Point point_t{};
    point_t.x = center.x;
    point_t.y = center.y;

    IMapRender::TextStyle style_t{};
    style_t.fontSize = style->text_style.font_size;
    style_t.color.full = style->color;

    mapView->DrawText(point_t, text, &style_t, CounterClockwiseToClockwise(style->angle));
}

void MapRenderManager::DrawColor(uint32_t map_id, mal_rect_area_t area, const mal_paint_style_t *style)
{
    if (!style) {
        AMAP_LOGE("style is null");
        return;
    }
    IMapRender *mapView = GetMapView(map_id);
    if (!mapView) {
        AMAP_LOGE("mapView is null");
        return;
    }
    Rect area_t = MalRectToExpandedOHOSRect(area);

    IMapRender::MapPaintStyle style_t{};
    float dashArr[DASH_ARRAY_SIZE] = {(float)style->dash_style.painted_length,
                                      (float)style->dash_style.unpainted_length};
    if (style->dash_style.painted_length != 0 && style->dash_style.unpainted_length != 0) {
        style_t.strokeStyle.dashCount = DASH_ARRAY_SIZE;
        style_t.strokeStyle.dashArray = dashArr;
        style_t.strokeStyle.dashOffset = style->dash_style.offset;
    }
    style_t.strokeStyle.strokeColor.full = style->color;
    style_t.strokeStyle.strokeWidth = style->width;
    style_t.fillColor.full = style->color;
    convertFillStyleToPaintType(style, style_t);
    mapView->DrawRect(area_t, &style_t, CounterClockwiseToClockwise(style->angle));
}

bool MapRenderManager::MeasureText(uint32_t map_id, const char *text, const mal_paint_style_t *style, int32_t *width,
                                   int32_t *ascender, int32_t *descender)
{
    if (!style) {
        AMAP_LOGE("style is null");
        return false;
    }
    IMapRender *mapView = GetMapView(map_id);
    if (!mapView) {
        AMAP_LOGE("mapView is null");
        return false;
    }
    IMapRender::TextStyle style_t{};
    style_t.fontSize = style->text_style.font_size;
    style_t.color.full = style->color;
    int32_t height = 0;
    mapView->MeasureText(text, &style_t, width, &height);
    *ascender = height / HALF_DIVISOR;
    *descender = height / HALF_DIVISOR;
    return true;
}

void MapRenderManager::DrawSvg(uint32_t map_id, mal_rect_area_t area, mal_bitmap_t bitmap,
                               const mal_paint_style_t *style)
{
    IMapRender *mapView = GetMapView(map_id);
    if (!mapView) {
        AMAP_LOGE("mapView is null");
        return;
    }
    if (!bitmap.buffer) {
        AMAP_LOGE("bitmap.buffer is null");
        return;
    }
    Rect area_t = MalRectToExpandedOHOSRect(area);
    IMapRender::SvgBufferInfo bit_map{};
    bit_map.buffer = bitmap.buffer;
    bit_map.bufferSize = bitmap.buffer_size;
    bit_map.svgType = IMapRender::SvgType::SVG_TYPE_SERIALIZED;
    bit_map.uuid = MapGetCurTileKey();
    mapView->DrawSvg(&bit_map, area_t, CounterClockwiseToClockwise(style->angle));
}

// 生命周期控制，当前就是打印
bool MapRenderManager::OnPointBeginDraw(uint32_t map_id, int32_t guid)
{
    AMAP_LOGD("OnPointBeginDraw with map_id = %d guid = %d", map_id, guid);
    return true;
}

void MapRenderManager::OnPointEndDraw(uint32_t map_id, int32_t guid, int32_t status)
{
    AMAP_LOGD("OnPointEndDraw with map_id %d and status %d", map_id, status);
}

bool MapRenderManager::OnLineBeginDraw(uint32_t map_id, int32_t guid)
{
    AMAP_LOGD("OnLineBeginDraw with map_id %d", map_id);
    return true;
}

void MapRenderManager::OnLineEndDraw(uint32_t map_id, int32_t guid, int32_t status)
{
    AMAP_LOGD("OnLineEndDraw with map_id %d and status %d", map_id, status);
}

bool MapRenderManager::OnPolygonBeginDraw(uint32_t map_id, int32_t guid)
{
    AMAP_LOGD("OnPolygonBeginDraw with map_id %d", map_id);
    return true;
}

void MapRenderManager::OnPolygonEndDraw(uint32_t map_id, int32_t guid, int32_t status)
{
    AMAP_LOGD("OnPolygonEndDraw with map_id %d and status %d", map_id, status);
}

bool MapRenderManager::OnTileBeginDraw(uint32_t map_id, uint32_t tile_x, uint32_t tile_y, uint32_t zoom)
{
    int len = snprintf_s(g_mapCurSvgKey.key, sizeof(g_mapCurSvgKey.key), sizeof(g_mapCurSvgKey.key) - 1, "%x_%x_%x",
                         tile_x, tile_y, zoom);
    AMAP_LOGD("OnTileBeginDraw with map_id %d, key = %s", map_id, g_mapCurSvgKey.key);
    if (len < 0 || len >= (int)sizeof(g_mapCurSvgKey.key)) {
        g_mapCurSvgKey.valid = false;
        return false;
    }
    g_mapCurSvgKey.valid = true;
    return true;
}

void MapRenderManager::OnTileEndDraw(uint32_t map_id, uint32_t tile_x, uint32_t tile_y, uint32_t zoom, int32_t status)
{
    AMAP_LOGD("OnTileEndDraw with map_id %d and status %d", map_id, status);
    g_mapCurSvgKey.valid = false;
}

bool MapRenderManager::OnPoiBeginDraw(uint32_t map_id, uint32_t tile_x, uint32_t tile_y, uint32_t zoom)
{
    AMAP_LOGD("OnPoiBeginDraw with map_id %d", map_id);
    return true;
}

void MapRenderManager::OnPoiEndDraw(uint32_t map_id, uint32_t tile_x, uint32_t tile_y, uint32_t zoom, int32_t status)
{
    AMAP_LOGD("OnPoiEndDraw with map_id %d and status %d", map_id, status);
}

bool MapRenderManager::OnImgBinBeginDraw(uint32_t map_id, uint32_t tile_x, uint32_t tile_y, uint32_t zoom)
{
    int len = snprintf_s(g_mapCurImageKey.key, sizeof(g_mapCurImageKey.key), sizeof(g_mapCurImageKey.key) - 1,
                         "%x_%x_%x", tile_x, tile_y, zoom);
    AMAP_LOGD("OnImgBinBeginDraw with map_id %d, key = %s", map_id, g_mapCurImageKey.key);
    if (len < 0 || len >= (int)sizeof(g_mapCurImageKey.key)) {
        g_mapCurImageKey.valid = false;
        return false;
    }
    g_mapCurImageKey.valid = true;
    return true;
}

void MapRenderManager::OnImgBinEndDraw(uint32_t map_id, uint32_t tile_x, uint32_t tile_y, uint32_t zoom, int32_t status)
{
    AMAP_LOGD("OnImgBinEndDraw with map_id %d and status %d", map_id, status);
    g_mapCurImageKey.valid = false;
}

bool MapRenderManager::OnImgBinLayerBegin(uint32_t map_id)
{
    AMAP_LOGD("OnImgBinLayerBegin map_id = %d", map_id);
    return true;
}

void MapRenderManager::OnImgBinLayerEnd(uint32_t map_id, int32_t status)
{
    AMAP_LOGD("OnImgBinLayerEnd map_id = %d status = %d", map_id, status);
}

}  // namespace OHOS

void ShowMapImageCache(void)
{
    OHOS::g_mapDecImageCache.DumpValidSlot();
}
