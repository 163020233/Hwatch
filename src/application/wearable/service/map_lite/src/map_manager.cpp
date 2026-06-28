/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Create Date : 2025-11
 */

#include "map_manager.h"
#include <cstring>
#include "map_controller_proxy.h"
#include "map_render_manager.h"
#include "map_utils.h"

#define AMAP_LOG_LEVEL 1
#include "amap_log_adapter.h"

namespace OHOS {
MapManager *MapManager::GetInstance()
{
    static MapManager instance;
    return &instance;
}

MapManager::MapManager() : isInitialized_(false), mapControllerProxy_(nullptr), realController_(nullptr) {}

IMapController *MapManager::GetMapController()
{
    return mapControllerProxy_;
}

int32_t MapManager::InitMapContext()
{
    AMAP_LOGD("MapManager::InitMapContext\n");
    if (isInitialized_) {
        // 已初始化则直接成功
        return MAL_API_SUCCESS;
    }
    mal_context_t mal_context;
    memset_s(&mal_context, sizeof(mal_context_t), 0, sizeof(mal_context_t));
    mal_context.tile_pixel_mode = getMapColorMode();
    mal_context.map_render_callback.on_point_begin_draw = MapRenderManager::OnPointBeginDraw;
    mal_context.map_render_callback.on_point_end_draw = MapRenderManager::OnPointEndDraw;
    mal_context.map_render_callback.on_line_begin_draw = MapRenderManager::OnLineBeginDraw;
    mal_context.map_render_callback.on_line_end_draw = MapRenderManager::OnLineEndDraw;
    mal_context.map_render_callback.on_polygon_begin_draw = MapRenderManager::OnPolygonBeginDraw;
    mal_context.map_render_callback.on_polygon_end_draw = MapRenderManager::OnPolygonEndDraw;
    mal_context.map_render_callback.on_tile_begin_draw = MapRenderManager::OnTileBeginDraw;
    mal_context.map_render_callback.on_tile_end_draw = MapRenderManager::OnTileEndDraw;
    mal_context.map_render_callback.on_poi_begin_draw = MapRenderManager::OnPoiBeginDraw;
    mal_context.map_render_callback.on_poi_end_draw = MapRenderManager::OnPoiEndDraw;
    mal_context.map_render_callback.on_img_bin_begin_draw = MapRenderManager::OnImgBinBeginDraw;
    mal_context.map_render_callback.on_img_bin_end_draw = MapRenderManager::OnImgBinEndDraw;
    mal_context.map_render_callback.on_img_bin_layer_begin = MapRenderManager::OnImgBinLayerBegin;
    mal_context.map_render_callback.on_img_bin_layer_end = MapRenderManager::OnImgBinLayerEnd;
    mal_context.render_adapter.begin_drawing = MapRenderManager::BeginDraw;
    mal_context.render_adapter.commit_drawing = MapRenderManager::CommitDraw;
    mal_context.render_adapter.draw_point = MapRenderManager::DrawPoints;
    mal_context.render_adapter.draw_polyline = MapRenderManager::DrawPolyline;
    mal_context.render_adapter.draw_polygon = MapRenderManager::DrawPolygon;
    mal_context.render_adapter.draw_image_buffer = MapRenderManager::DrawImage;
    mal_context.render_adapter.draw_bitmap = MapRenderManager::DrawBitmap;
    mal_context.render_adapter.draw_text = MapRenderManager::DrawText;
    mal_context.render_adapter.draw_color = MapRenderManager::DrawColor;
    mal_context.render_adapter.measure_text = MapRenderManager::MeasureText;
    mal_context.render_adapter.draw_svg_bitmap = MapRenderManager::DrawSvg;

    realController_ = ProvideMapController();

    AMAP_LOGD("finish ProvideMapController with ctr: %p", realController_);
    if (!realController_) {
        AMAP_LOGE("InitMapContext failed with realController_ null");
        return MAL_API_NULL_PARAM;
    }

    mapControllerProxy_ = new MapControllerProxy(realController_);
    if(!mapControllerProxy_) {
        delete realController_;
        realController_ = nullptr;
        AMAP_LOGE("InitMapContext failed with mapControllerProxy_ null");
        return MAL_API_NULL_PARAM;
    }
    AMAP_LOGD("finish create MapControllerProxy with proxy %p", mapControllerProxy_);
    int32_t ret = mapControllerProxy_->mal_map_init(mal_context);
    if (ret != MAL_API_SUCCESS) {
        AMAP_LOGE("InitMapContext failed with ret: %d", ret);
        delete mapControllerProxy_;
        mapControllerProxy_ = nullptr;
        delete realController_;
        realController_ = nullptr;
        return ret;
    }
    isInitialized_ = true;
    return MAL_API_SUCCESS;
}

int32_t MapManager::DeInitMapContext()
{
    if (!isInitialized_) {
        return MAL_API_SUCCESS;
    }

    int32_t ret = mapControllerProxy_->mal_map_uninit();
    if (ret != MAL_API_SUCCESS) {
        AMAP_LOGE("DeInitMapContext failed with ret %d", ret);
    }
    // 清理映射关系释放视图
    MapRenderManager::ClearAll();
    // 清理控制器资源
    delete mapControllerProxy_;
    mapControllerProxy_ = nullptr;
    delete realController_;
    realController_ = nullptr;
    isInitialized_ = false;
    return ret;
}

int32_t MapManager::BindMapToView(int32_t mapId, IMapRender *mapView)
{
    if (!mapView) {
        AMAP_LOGE("BindMapToView failed with mapView Null");
        return MAL_API_NULL_PARAM;
    }
    MapRenderManager::AddMapView(mapId, mapView);
    return MAL_API_SUCCESS;
}

int32_t MapManager::UnBindMapToView(int32_t mapId)
{
    MapRenderManager::RemoveMapView(mapId);
    return MAL_API_SUCCESS;
}
}  // namespace OHOS
