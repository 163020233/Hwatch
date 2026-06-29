/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Create Date : 2025-11
 */

// 作为wrapper连接地图视图和渲染视图
#ifndef MAP_RENDER_MANAGER_H
#define MAP_RENDER_MANAGER_H

#include <cstdint>
#include <map>
#include "map/map_render_interface.h"
#include "mal_map_defines.h"

#define MAX_MAP_VIEW_COUNT 4

namespace OHOS {
/**
 * @brief 实现渲染函数由C接口到C++渲染对象的转换，标准的thunk函数类,由于要支持C接口回调，只能静态方法转换
 *
 */
class MapRenderManager {
private:
    // 地图视图Mapid和渲染视图IMapView的映射
    struct MapViewEntry {
        int32_t mapId;
        IMapRender *view;
        bool used;  // 标记是否被占用
    };
    static MapViewEntry mapViews[MAX_MAP_VIEW_COUNT];

private:
    // 私有化默认构造函数
    MapRenderManager() = default;
    // 私有化析构函数
    ~MapRenderManager();

    // 删除拷贝构造函数
    MapRenderManager(const MapRenderManager &) = delete;
    // 删除赋值操作符
    MapRenderManager &operator=(const MapRenderManager &) = delete;

public:
    static void AddMapView(int32_t mapId, IMapRender *view);
    static IMapRender *GetMapView(int32_t mapId);
    static void RemoveMapView(int32_t mapId);
    static void ClearAll();

    static void BeginDraw(uint32_t map_id, mal_render_context_t status);
    static void CommitDraw(uint32_t map_id);
    static void DrawPoints(uint32_t map_id, mal_point_t *point, uint32_t point_size, const mal_paint_style_t *style);
    static void DrawPolyline(uint32_t map_id, mal_point_t *points, uint32_t point_size, const mal_paint_style_t *style);
    static void DrawPolygon(uint32_t map_id, mal_point_t *points, uint32_t point_size, const mal_paint_style_t *style);
    static void DrawImage(uint32_t map_id, mal_rect_area_t area, const mal_image_buf_info_t *image_info,
                          const mal_paint_style_t *style);
    static void DrawBitmap(uint32_t map_id, mal_rect_area_t area, mal_bitmap_t bitmap, const mal_paint_style_t *style);
    static void DrawText(uint32_t map_id, mal_point_t center, const char *text, const mal_paint_style_t *style);
    static void DrawColor(uint32_t map_id, mal_rect_area_t area, const mal_paint_style_t *style);
    static bool MeasureText(uint32_t map_id, const char *text, const mal_paint_style_t *style, int32_t *width,
                            int32_t *ascender, int32_t *descender);
    static void DrawSvg(uint32_t map_id, mal_rect_area_t area, mal_bitmap_t bitmap, const mal_paint_style_t *style);

    static bool OnPointBeginDraw(uint32_t map_id, int32_t guid);
    static void OnPointEndDraw(uint32_t map_id, int32_t guid, int32_t status);
    static bool OnLineBeginDraw(uint32_t map_id, int32_t guid);
    static void OnLineEndDraw(uint32_t map_id, int32_t guid, int32_t status);
    static bool OnPolygonBeginDraw(uint32_t map_id, int32_t guid);
    static void OnPolygonEndDraw(uint32_t map_id, int32_t guid, int32_t status);
    static bool OnTileBeginDraw(uint32_t map_id, uint32_t tile_x, uint32_t tile_y, uint32_t zoom);
    static void OnTileEndDraw(uint32_t map_id, uint32_t tile_x, uint32_t tile_y, uint32_t zoom, int32_t status);
    static bool OnPoiBeginDraw(uint32_t map_id, uint32_t tile_x, uint32_t tile_y, uint32_t zoom);
    static void OnPoiEndDraw(uint32_t map_id, uint32_t tile_x, uint32_t tile_y, uint32_t zoom, int32_t status);
    static bool OnImgBinBeginDraw(uint32_t map_id, uint32_t tile_x, uint32_t tile_y, uint32_t zoom);
    static void OnImgBinEndDraw(uint32_t map_id, uint32_t tile_x, uint32_t tile_y, uint32_t zoom, int32_t status);
    static bool OnImgBinLayerBegin(uint32_t map_id);
    static void OnImgBinLayerEnd(uint32_t map_id, int32_t status);
};
}  // namespace OHOS
#endif