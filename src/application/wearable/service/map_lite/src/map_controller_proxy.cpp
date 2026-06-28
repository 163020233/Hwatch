/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Create Date : 2025-11
 */

#include "map_controller_proxy.h"
#include "amap_log_adapter.h"

namespace OHOS {
// ============ 私有上下文结构（统一定义）============
struct InitCtx {
    IMapController *impl;
    mal_context_t ctx;
};

struct NoParamsContext {
    IMapController *impl;
};

struct ActivateContext {
    IMapController *impl;
};

struct CreateViewContext {
    IMapController *impl;
    mal_map_view_create_param_t param;
};

struct DestroyViewContext {
    IMapController *impl;
    uint32_t mapId;
};

struct TextureDataContext {
    IMapController *impl;
    const mal_map_texture_data_t *texture_data;
    uint32_t texture_id;
};

struct PointOverlayContext {
    IMapController *impl;
    mal_map_point_overlay_t *point_overlay;
};

struct PolylineOverlayContext {
    IMapController *impl;
    mal_map_polyline_overlay_t *line_overlay;
};

struct PolygonOverlayContext {
    IMapController *impl;
    mal_map_polygon_overlay_t *polygon_overlay;
};

struct BaseOverlayContext {
    IMapController *impl;
    uint32_t map_id;
    mal_map_base_overlay_t *overlay;
};

struct OverlayContext {
    IMapController *impl;
    uint32_t map_id;
    uint32_t overlay_id;
};

struct GetOverlayContext {
    IMapController *impl;
    uint32_t map_id;
    uint32_t index;
};

struct GetMapBoundsContext {
    IMapController *impl;
    uint32_t map_id;
    mal_map_base_overlay_t *overlay;
    mal_map_coord2d_t *north_east;
    mal_map_coord2d_t *south_west;
    float *level;
};

struct SetViewPortContext {
    IMapController *impl;
    uint32_t map_id;
    mal_map_view_port_t view_port;
};

struct SetMapViewRectContext {
    IMapController *impl;
    uint32_t map_id;
    mal_rect_area_t map_view_rect;
};

struct SetRenderCallbackContext {
    IMapController *impl;
    mal_map_render_callback_t callback;
};

struct SetCenterContext {
    IMapController *impl;
    uint32_t map_id;
    mal_map_coord2d_t coord2d;
};

struct SetLevelContext {
    IMapController *impl;
    uint32_t map_id;
    float level;
};

struct SetMapViewAnchorContext {
    IMapController *impl;
    uint32_t map_id;
    mal_map_view_anchor_t anchor;
};

struct SetRollAngleContext {
    IMapController *impl;
    uint32_t map_id;
    float roll_angle;
};

struct GetPostureContext {
    IMapController *impl;
    uint32_t map_id;
    mal_map_posture_t *posture;
};

struct SetVisibleBoundsContext {
    IMapController *impl;
    uint32_t map_id;
    mal_navi_point_bounds_t bounds;
    mal_edge_insets_t insets;
};

struct LonlatToXYContext {
    IMapController *impl;
    uint32_t map_id;
    mal_map_coord2d_t lonlat;
    int32_t *x;
    int32_t *y;
};

struct XYToLonlatContext {
    IMapController *impl;
    uint32_t map_id;
    int32_t x;
    int32_t y;
    mal_map_coord2d_t *lonlat;
};

struct ClickPointsContext {
    IMapController *impl;
    uint32_t map_id;
    int32_t x;
    int32_t y;
    int32_t *overlay_count;
};

struct ReleaseOverlaysContext {
    IMapController *impl;
    mal_map_base_overlay_t **overlays;
    int32_t overlay_count;
};

struct TrackNaviParseContext {
    IMapController *impl;
    const char *data;
    size_t length;
};

struct TrackNaviParseJsonContext {
    IMapController *impl;
    const char *data;
    int32_t mode;
};

struct PoiMemFreeContext {
    IMapController *impl;
    mal_map_poi_t *mal_map_poi_info;
};

struct TrackNaviParseTrackInfoFreeFreeContext {
    IMapController *impl;
    mal_map_track_info_t *track_info;
};

struct TrackNaviParseTrackInfoCountFreeFreeContext {
    IMapController *impl;
    mal_map_track_info_count_t *track_info;
};

struct TrackNaviOverlayContext {
    IMapController *impl;
    mal_map_track_navi_overlay_t *overlay;
    uint32_t map_id;
};

struct TrackNaviUpdateRealPointContext {
    IMapController *impl;
    uint32_t map_id;
    mal_map_coord2d_t point;
    float bear;
    mal_map_track_navi_overlay_t *overlay;
};

struct TrackNaviGetDistanceContext {
    IMapController *impl;
    uint32_t map_id;
    mal_map_coord2d_t point;
    mal_map_track_point_type point_type;
    mal_map_track_navi_overlay_t *overlay;
    double result;
};

struct CalcPointsDistanceContext {
    IMapController *impl;
    double lon1;
    double lat1;
    double lon2;
    double lat2;
};

// ============ 静态包装函数 ============
// MCU场景面临Lamba表达示无法支持的情况，需要静态包装函数实现封装
static int32_t MalMapInitWrapper(void *arg)
{
    auto *ctx = static_cast<InitCtx *>(arg);
    if (!ctx || !ctx->impl) {
        AMAP_LOGE("MalMapInitWrapper with null params");
        return MAL_API_NULL_PARAM;
    }
    AMAP_LOGD("begin ctx->impl->mal_map_init\n");
    return ctx->impl->mal_map_init(ctx->ctx);
}

static int32_t MalMapUninitWrapper(void *arg)
{
    auto *ctx = static_cast<NoParamsContext *>(arg);
    return ctx->impl->mal_map_uninit();
}

static int32_t MalMapActivateDeviceWrapper(void *arg)
{
    auto *ctx = static_cast<ActivateContext *>(arg);
    return ctx->impl->mal_map_activate_device();
}

static int32_t MalMapCreateViewWrapper(void *arg)
{
    auto *ctx = static_cast<CreateViewContext *>(arg);
    return ctx->impl->mal_map_create_view(ctx->param);
}

static int32_t MalMapDestroyViewWrapper(void *arg)
{
    auto *ctx = static_cast<DestroyViewContext *>(arg);
    return ctx->impl->mal_map_destroy_view(ctx->mapId);
}

static int32_t MalMapDoRenderWrapper(void *arg)
{
    auto *ctx = static_cast<NoParamsContext *>(arg);
    AMAP_LOGD("MalMapDoRenderWrapper with ctx %p and impl %p", ctx, ctx->impl);
    int32_t ret = ctx->impl->mal_map_do_render();
    AMAP_LOGD("MalMapDoRenderWrapper with ret %d", ret);
    return ret;
}

static int32_t MalMapPauseRenderWrapper(void *arg)
{
    auto *ctx = static_cast<DestroyViewContext *>(arg);
    return ctx->impl->mal_map_pause_render(ctx->mapId);
}

static int32_t MalMapResumeRenderWrapper(void *arg)
{
    auto *ctx = static_cast<DestroyViewContext *>(arg);
    return ctx->impl->mal_map_resume_render(ctx->mapId);
}

static int32_t MalMapAddTextureWrapper(void *arg)
{
    auto *ctx = static_cast<TextureDataContext *>(arg);
    return ctx->impl->mal_map_add_texture(ctx->texture_data);
}

static int32_t MalMapUpdateTextureWrapper(void *arg)
{
    auto *ctx = static_cast<TextureDataContext *>(arg);
    return ctx->impl->mal_map_update_texture(ctx->texture_id, ctx->texture_data);
}

static int32_t MalMapRemoveTextureWrapper(void *arg)
{
    auto *ctx = static_cast<TextureDataContext *>(arg);
    return ctx->impl->mal_map_remove_texture(ctx->texture_id);
}

static int32_t MalMapInitPointOverlayWrapper(void *arg)
{
    auto *ctx = static_cast<PointOverlayContext *>(arg);
    return ctx->impl->mal_map_init_point_overlay(ctx->point_overlay);
}

static int32_t MalMapInitLineOverlayWrapper(void *arg)
{
    auto *ctx = static_cast<PolylineOverlayContext *>(arg);
    return ctx->impl->mal_map_init_line_overlay(ctx->line_overlay);
}

static int32_t MalMapInitPolygonOverlayWrapper(void *arg)
{
    auto *ctx = static_cast<PolygonOverlayContext *>(arg);
    return ctx->impl->mal_map_init_polygon_overlay(ctx->polygon_overlay);
}

static int32_t MalMapAddOverlayWrapper(void *arg)
{
    auto *ctx = static_cast<BaseOverlayContext *>(arg);
    return ctx->impl->mal_map_add_overlay(ctx->map_id, ctx->overlay);
}

static int32_t MalMapUpdateOverlayWrapper(void *arg)
{
    auto *ctx = static_cast<BaseOverlayContext *>(arg);
    return ctx->impl->mal_map_update_overlay(ctx->map_id, ctx->overlay);
}

static int32_t MalMapRemoveOverlayWrapper(void *arg)
{
    auto *ctx = static_cast<OverlayContext *>(arg);
    return ctx->impl->mal_map_remove_overlay(ctx->map_id, ctx->overlay_id);
}

static void *MalMapFindOverlayWrapper(void *arg)
{
    auto *ctx = static_cast<OverlayContext *>(arg);
    const mal_map_base_overlay_t *result = ctx->impl->mal_map_find_overlay(ctx->map_id, ctx->overlay_id);
    return const_cast<mal_map_base_overlay_t *>(result);
}

static int32_t MalMapGetOverlayCountWrapper(void *arg)
{
    auto *ctx = static_cast<DestroyViewContext *>(arg);
    return ctx->impl->mal_map_get_overlay_count(ctx->mapId);
}

static void *MalMapGetOverlayWrapper(void *arg)
{
    auto *ctx = static_cast<GetOverlayContext *>(arg);
    const mal_map_base_overlay_t *result = ctx->impl->mal_map_get_overlay(ctx->map_id, ctx->index);
    return const_cast<mal_map_base_overlay_t *>(result);
}

static int32_t MalMapGetMapBoundsWrapper(void *arg)
{
    auto *ctx = static_cast<GetMapBoundsContext *>(arg);
    return ctx->impl->mal_map_get_map_bounds(ctx->map_id, ctx->overlay, ctx->north_east, ctx->south_west, ctx->level);
}

static int32_t MalMapReleaseOverlayWrapper(void *arg)
{
    auto *ctx = static_cast<BaseOverlayContext *>(arg);
    return ctx->impl->mal_map_release_overlay(ctx->overlay);
}

static int32_t MalMapSetViewPortWrapper(void *arg)
{
    auto *ctx = static_cast<SetViewPortContext *>(arg);
    return ctx->impl->mal_map_set_view_port(ctx->map_id, ctx->view_port);
}

static int32_t MalMapSetMapViewRectWrapper(void *arg)
{
    auto *ctx = static_cast<SetMapViewRectContext *>(arg);
    return ctx->impl->mal_map_set_map_view_rect(ctx->map_id, ctx->map_view_rect);
}

static int32_t MalMapSetRenderCallbackWrapper(void *arg)
{
    auto *ctx = static_cast<SetRenderCallbackContext *>(arg);
    return ctx->impl->mal_map_set_render_callback(ctx->callback);
}

static int32_t MalMapSetCenterWrapper(void *arg)
{
    auto *ctx = static_cast<SetCenterContext *>(arg);
    return ctx->impl->mal_map_set_center(ctx->map_id, ctx->coord2d);
}

static int32_t MalMapSetLevelWrapper(void *arg)
{
    auto *ctx = static_cast<SetLevelContext *>(arg);
    return ctx->impl->mal_map_set_level(ctx->map_id, ctx->level);
}

static float MalMapGetLevelWrapper(void *arg)
{
    auto *ctx = static_cast<DestroyViewContext *>(arg);
    return ctx->impl->mal_map_get_level(ctx->mapId);
}

static int32_t MalMapSetMapViewAnchorWrapper(void *arg)
{
    auto *ctx = static_cast<SetMapViewAnchorContext *>(arg);
    return ctx->impl->mal_map_set_map_view_anchor(ctx->map_id, ctx->anchor);
}

static int32_t MalMapSetRollAngleWrapper(void *arg)
{
    auto *ctx = static_cast<SetRollAngleContext *>(arg);
    return ctx->impl->mal_map_set_roll_angle(ctx->map_id, ctx->roll_angle);
}

static int32_t MalMapGetPostureWrapper(void *arg)
{
    auto *ctx = static_cast<GetPostureContext *>(arg);
    return ctx->impl->mal_map_get_posture(ctx->map_id, ctx->posture);
}

static int32_t MalMapSetVisibleBoundsWrapper(void *arg)
{
    auto *ctx = static_cast<SetVisibleBoundsContext *>(arg);
    return ctx->impl->mal_map_set_visible_bounds(ctx->map_id, ctx->bounds, ctx->insets);
}

static int32_t MalMapLonlatToXYWrapper(void *arg)
{
    auto *ctx = static_cast<LonlatToXYContext *>(arg);
    return ctx->impl->mal_map_lonlat_to_xy(ctx->map_id, ctx->lonlat, ctx->x, ctx->y);
}

static int32_t MalMapXYToLonlatWrapper(void *arg)
{
    auto *ctx = static_cast<XYToLonlatContext *>(arg);
    return ctx->impl->mal_map_xy_to_lonlat(ctx->map_id, ctx->x, ctx->y, ctx->lonlat);
}

static int32_t MalMapTouchBeginWrapper(void *arg)
{
    auto *ctx = static_cast<XYToLonlatContext *>(arg);
    return ctx->impl->mal_map_touch_begin(ctx->map_id, ctx->x, ctx->y);
}

static int32_t MalMapTouchUpdateWrapper(void *arg)
{
    auto *ctx = static_cast<XYToLonlatContext *>(arg);
    return ctx->impl->mal_map_touch_update(ctx->map_id, ctx->x, ctx->y);
}

static int32_t MalMapTouchEndWrapper(void *arg)
{
    auto *ctx = static_cast<XYToLonlatContext *>(arg);
    return ctx->impl->mal_map_touch_end(ctx->map_id, ctx->x, ctx->y);
}

static void *MalMapClickPointsWrapper(void *arg)
{
    auto *ctx = static_cast<ClickPointsContext *>(arg);
    mal_map_point_overlay_t **result = ctx->impl->mal_map_click_points(ctx->map_id, ctx->x, ctx->y, ctx->overlay_count);
    return static_cast<void *>(result);
}

static int32_t MalMapReleaseOverlaysWrapper(void *arg)
{
    auto *ctx = static_cast<ReleaseOverlaysContext *>(arg);
    return ctx->impl->mal_map_release_overlays(ctx->overlays, ctx->overlay_count);
}

static void *MalMapTrackNaviParseGpxWrapper(void *arg)
{
    auto *ctx = static_cast<TrackNaviParseContext *>(arg);
    mal_map_track_info_t *result = ctx->impl->mal_map_track_navi_parse_gpx(ctx->data, ctx->length);
    return static_cast<void *>(result);
}

static void *MalMapTrckParserJsonWrapper(void *arg)
{
    auto *ctx = static_cast<TrackNaviParseJsonContext *>(arg);
    mal_map_track_info_count_t *result = ctx->impl->mal_map_track_parser_json(ctx->data, ctx->mode);
    return static_cast<void *>(result);
}

static void *MalMapPoiSearchJsonWrapper(void *arg)
{
    auto *ctx = static_cast<TrackNaviParseContext *>(arg);
    mal_map_poi_t *result = ctx->impl->mal_poi_search_json(ctx->data);
    return static_cast<void *>(result);
}

static void MalMapPoiMemFreeWrapper(void *arg)
{
    auto *ctx = static_cast<PoiMemFreeContext *>(arg);
    ctx->impl->mal_map_poi_mem_free(ctx->mal_map_poi_info);
}

static void MalMapTrackNaviParseTrackInfoFreeWrapper(void *arg)
{
    auto *ctx = static_cast<TrackNaviParseTrackInfoFreeFreeContext *>(arg);
    ctx->impl->mal_map_track_navi_parse_track_info_free(ctx->track_info);
}

static void MalMapTrackNaviParseTrackInfoCountFreeWrapper(void *arg)
{
    auto *ctx = static_cast<TrackNaviParseTrackInfoCountFreeFreeContext *>(arg);
    ctx->impl->mal_map_track_navi_parse_track_info_count_free(ctx->track_info);
}

static int32_t MalMapInitTrackNaviOverlayWrapper(void *arg)
{
    auto *ctx = static_cast<TrackNaviOverlayContext *>(arg);
    return ctx->impl->mal_map_init_track_navi_overlay(ctx->overlay);
}

static int32_t MalMapTrackNaviAddOverlayWrapper(void *arg)
{
    auto *ctx = static_cast<TrackNaviOverlayContext *>(arg);
    return ctx->impl->mal_map_track_navi_add_overlay(ctx->map_id, ctx->overlay);
}

static int32_t MalMapTrackNaviRemoveOverlayWrapper(void *arg)
{
    auto *ctx = static_cast<GetOverlayContext *>(arg);
    return ctx->impl->mal_map_track_navi_remove_overlay(ctx->map_id, ctx->index);
}

static void MalMapTrackNaviUpdateRealPointWrapper(void *arg)
{
    auto *ctx = static_cast<TrackNaviUpdateRealPointContext *>(arg);
    ctx->impl->mal_map_track_navi_update_real_point(ctx->map_id, ctx->point, ctx->bear, ctx->overlay);
}

static int32_t MalMapTrackNaviGetDistanceWrapper(void *arg)
{
    auto *ctx = static_cast<TrackNaviGetDistanceContext *>(arg);
    ctx->result = ctx->impl->mal_map_track_navi_get_distance(ctx->map_id, ctx->point, ctx->point_type, ctx->overlay);
    return 0;
}

static float MalMapCalcPointsDistanceWrapper(void *arg)
{
    auto *ctx = static_cast<CalcPointsDistanceContext *>(arg);
    if (!ctx || !ctx->impl) {
        AMAP_LOGE("MalMapCalcPointsDistanceWrapper with null params");
        return 0.0f;
    }

    return static_cast<float>(ctx->impl->mal_map_calc_points_distance(ctx->lon1, ctx->lat1, ctx->lon2, ctx->lat2));
}

// ============ MapControllerProxy 方法实现 ============
MapControllerProxy::MapControllerProxy(IMapController *impl, const char *taskName, UINT16 priority)
    : realImpl_(impl),
      runner_(taskName, priority)
{
    if (realImpl_) {
        runner_.Start();
    }
}

MapControllerProxy::~MapControllerProxy()
{
    // runner自动析构执行stop
}

int32_t MapControllerProxy::mal_map_init(mal_context_t mal_context)
{
    if (!realImpl_) {
        return MAL_API_NO_INIT;
    }
    InitCtx ctx{realImpl_, mal_context};
    int32_t ret = runner_.PostSyncTaskInt32(MalMapInitWrapper, &ctx);
    return ret;
}

int32_t MapControllerProxy::mal_map_uninit(void)
{
    if (!realImpl_) {
        return MAL_API_NO_INIT;
    }
    NoParamsContext ctx{realImpl_};
    return runner_.PostSyncTaskInt32(MalMapUninitWrapper, &ctx);
}

int32_t MapControllerProxy::mal_map_activate_device()
{
    if (!realImpl_) {
        return MAL_API_NO_INIT;
    }
    ActivateContext ctx{realImpl_};
    return runner_.PostSyncTaskInt32(MalMapActivateDeviceWrapper, &ctx);
}

int32_t MapControllerProxy::mal_map_create_view(mal_map_view_create_param_t param)
{
    if (!realImpl_) {
        return MAL_API_NO_INIT;
    }
    CreateViewContext ctx{realImpl_, param};
    return runner_.PostSyncTaskInt32(MalMapCreateViewWrapper, &ctx);
}

int32_t MapControllerProxy::mal_map_destroy_view(uint32_t map_id)
{
    if (!realImpl_) {
        return MAL_API_NO_INIT;
    }
    DestroyViewContext ctx{realImpl_, map_id};
    return runner_.PostSyncTaskInt32(MalMapDestroyViewWrapper, &ctx);
}

int32_t MapControllerProxy::mal_map_do_render(void)
{
    if (!realImpl_) {
        return MAL_API_NO_INIT;
    }
    NoParamsContext ctx{realImpl_};
    AMAP_LOGD("MapControllerProxy::mal_map_do_render post sync task");
    int32_t ret = runner_.PostSyncTaskInt32(MalMapDoRenderWrapper, &ctx);
    AMAP_LOGD("MapControllerProxy::mal_map_do_render with ret %d", ret);
    return ret;
}

int32_t MapControllerProxy::mal_map_pause_render(uint32_t map_id)
{
    if (!realImpl_) {
        return MAL_API_NO_INIT;
    }
    DestroyViewContext ctx{realImpl_, map_id};
    return runner_.PostSyncTaskInt32(MalMapPauseRenderWrapper, &ctx);
}

int32_t MapControllerProxy::mal_map_resume_render(uint32_t map_id)
{
    if (!realImpl_) {
        return MAL_API_NO_INIT;
    }
    DestroyViewContext ctx{realImpl_, map_id};
    return runner_.PostSyncTaskInt32(MalMapResumeRenderWrapper, &ctx);
}

int32_t MapControllerProxy::mal_map_add_texture(const mal_map_texture_data_t *texture_data)
{
    if (!realImpl_) {
        return MAL_API_NO_INIT;
    }
    TextureDataContext ctx{realImpl_, texture_data, 0};
    return runner_.PostSyncTaskInt32(MalMapAddTextureWrapper, &ctx);
}

int32_t MapControllerProxy::mal_map_update_texture(uint32_t texture_id, const mal_map_texture_data_t *texture_data)
{
    if (!realImpl_) {
        return MAL_API_NO_INIT;
    }
    TextureDataContext ctx{realImpl_, texture_data, texture_id};
    return runner_.PostSyncTaskInt32(MalMapUpdateTextureWrapper, &ctx);
}

int32_t MapControllerProxy::mal_map_remove_texture(uint32_t texture_id)
{
    if (!realImpl_) {
        return MAL_API_NO_INIT;
    }
    TextureDataContext ctx{realImpl_, nullptr, texture_id};
    return runner_.PostSyncTaskInt32(MalMapRemoveTextureWrapper, &ctx);
}

int32_t MapControllerProxy::mal_map_init_point_overlay(mal_map_point_overlay_t *point_overlay)
{
    if (!realImpl_) {
        return MAL_API_NO_INIT;
    }
    PointOverlayContext ctx{realImpl_, point_overlay};
    return runner_.PostSyncTaskInt32(MalMapInitPointOverlayWrapper, &ctx);
}

int32_t MapControllerProxy::mal_map_init_line_overlay(mal_map_polyline_overlay_t *line_overlay)
{
    if (!realImpl_) {
        return MAL_API_NO_INIT;
    }
    PolylineOverlayContext ctx{realImpl_, line_overlay};
    return runner_.PostSyncTaskInt32(MalMapInitLineOverlayWrapper, &ctx);
}

int32_t MapControllerProxy::mal_map_init_polygon_overlay(mal_map_polygon_overlay_t *polygon_overlay)
{
    if (!realImpl_) {
        return MAL_API_NO_INIT;
    }
    PolygonOverlayContext ctx{realImpl_, polygon_overlay};
    return runner_.PostSyncTaskInt32(MalMapInitPolygonOverlayWrapper, &ctx);
}

int32_t MapControllerProxy::mal_map_add_overlay(uint32_t map_id, mal_map_base_overlay_t *overlay)
{
    if (!realImpl_) {
        return MAL_API_NO_INIT;
    }
    BaseOverlayContext ctx{realImpl_, map_id, overlay};
    return runner_.PostSyncTaskInt32(MalMapAddOverlayWrapper, &ctx);
}

int32_t MapControllerProxy::mal_map_update_overlay(uint32_t map_id, mal_map_base_overlay_t *overlay)
{
    if (!realImpl_) {
        return MAL_API_NO_INIT;
    }
    BaseOverlayContext ctx{realImpl_, map_id, overlay};
    return runner_.PostSyncTaskInt32(MalMapUpdateOverlayWrapper, &ctx);
}

int32_t MapControllerProxy::mal_map_remove_overlay(uint32_t map_id, uint32_t overlay_id)
{
    if (!realImpl_) {
        return MAL_API_NO_INIT;
    }
    OverlayContext ctx{realImpl_, map_id, overlay_id};
    return runner_.PostSyncTaskInt32(MalMapRemoveOverlayWrapper, &ctx);
}

const mal_map_base_overlay_t *MapControllerProxy::mal_map_find_overlay(uint32_t map_id, uint32_t overlay_id)
{
    if (!realImpl_) {
        return NULL;
    }
    OverlayContext ctx{realImpl_, map_id, overlay_id};
    void *taskResult = runner_.PostSyncTaskPtr(MalMapFindOverlayWrapper, &ctx);
    return static_cast<const mal_map_base_overlay_t *>(taskResult);
}
uint32_t MapControllerProxy::mal_map_get_overlay_count(uint32_t map_id)
{
    if (!realImpl_) {
        return MAL_API_NO_INIT;
    }
    DestroyViewContext ctx{realImpl_, map_id};
    return runner_.PostSyncTaskInt32(MalMapGetOverlayCountWrapper, &ctx);
}
const mal_map_base_overlay_t *MapControllerProxy::mal_map_get_overlay(uint32_t map_id, uint32_t index)
{
    if (!realImpl_) {
        return NULL;
    }
    GetOverlayContext ctx{realImpl_, map_id, index};
    void *taskResult = runner_.PostSyncTaskPtr(MalMapGetOverlayWrapper, &ctx);
    return static_cast<const mal_map_base_overlay_t *>(taskResult);
}

int32_t MapControllerProxy::mal_map_get_map_bounds(uint32_t map_id, mal_map_base_overlay_t *overlay,
                                                   mal_map_coord2d_t *north_east, mal_map_coord2d_t *south_west,
                                                   float *level)
{
    if (!realImpl_) {
        return MAL_API_NO_INIT;
    }
    GetMapBoundsContext ctx{realImpl_, map_id, overlay, north_east, south_west, level};
    return runner_.PostSyncTaskInt32(MalMapGetMapBoundsWrapper, &ctx);
}
int32_t MapControllerProxy::mal_map_release_overlay(mal_map_base_overlay_t *overlay)
{
    if (!realImpl_) {
        return MAL_API_NO_INIT;
    }
    BaseOverlayContext ctx{realImpl_, 0, overlay};
    return runner_.PostSyncTaskInt32(MalMapReleaseOverlayWrapper, &ctx);
}

int32_t MapControllerProxy::mal_map_set_view_port(uint32_t map_id, mal_map_view_port_t view_port)
{
    if (!realImpl_) {
        return MAL_API_NO_INIT;
    }
    SetViewPortContext ctx{realImpl_, map_id, view_port};
    return runner_.PostSyncTaskInt32(MalMapSetViewPortWrapper, &ctx);
}

int32_t MapControllerProxy::mal_map_set_map_view_rect(uint32_t map_id, mal_rect_area_t map_view_rect)
{
    if (!realImpl_) {
        return MAL_API_NO_INIT;
    }
    SetMapViewRectContext ctx{realImpl_, map_id, map_view_rect};
    return runner_.PostSyncTaskInt32(MalMapSetMapViewRectWrapper, &ctx);
}

int32_t MapControllerProxy::mal_map_set_render_callback(mal_map_render_callback_t callback)
{
    if (!realImpl_) {
        return MAL_API_NO_INIT;
    }
    SetRenderCallbackContext ctx{realImpl_, callback};
    return runner_.PostSyncTaskInt32(MalMapSetRenderCallbackWrapper, &ctx);
}
int32_t MapControllerProxy::mal_map_set_center(uint32_t map_id, mal_map_coord2d_t coord2d)
{
    if (!realImpl_) {
        return MAL_API_NO_INIT;
    }
    SetCenterContext ctx{realImpl_, map_id, coord2d};
    return runner_.PostSyncTaskInt32(MalMapSetCenterWrapper, &ctx);
}
int32_t MapControllerProxy::mal_map_set_level(uint32_t map_id, float level)
{
    if (!realImpl_) {
        return MAL_API_NO_INIT;
    }
    SetLevelContext ctx{realImpl_, map_id, level};
    return runner_.PostSyncTaskInt32(MalMapSetLevelWrapper, &ctx);
}
float MapControllerProxy::mal_map_get_level(uint32_t map_id)
{
    if (!realImpl_) {
        return MAL_API_NO_INIT;
    }
    DestroyViewContext ctx{realImpl_, map_id};
    return runner_.PostSyncTaskFloat(MalMapGetLevelWrapper, &ctx);
}
int32_t MapControllerProxy::mal_map_set_map_view_anchor(uint32_t map_id, mal_map_view_anchor_t anchor)
{
    if (!realImpl_) {
        return MAL_API_NO_INIT;
    }
    SetMapViewAnchorContext ctx{realImpl_, map_id, anchor};
    return runner_.PostSyncTaskInt32(MalMapSetMapViewAnchorWrapper, &ctx);
}
int32_t MapControllerProxy::mal_map_set_roll_angle(uint32_t map_id, float roll_angle)
{
    if (!realImpl_) {
        return MAL_API_NO_INIT;
    }
    SetRollAngleContext ctx{realImpl_, map_id, roll_angle};
    return runner_.PostSyncTaskInt32(MalMapSetRollAngleWrapper, &ctx);
}
int32_t MapControllerProxy::mal_map_get_posture(uint32_t map_id, mal_map_posture_t *posture)
{
    if (!realImpl_) {
        return MAL_API_NO_INIT;
    }
    GetPostureContext ctx{realImpl_, map_id, posture};
    return runner_.PostSyncTaskInt32(MalMapGetPostureWrapper, &ctx);
}

int32_t MapControllerProxy::mal_map_set_visible_bounds(uint32_t map_id, mal_navi_point_bounds_t bounds,
                                                       mal_edge_insets_t insets)
{
    if (!realImpl_) {
        return MAL_API_NO_INIT;
    }
    SetVisibleBoundsContext ctx{realImpl_, map_id, bounds, insets};
    return runner_.PostSyncTaskInt32(MalMapSetVisibleBoundsWrapper, &ctx);
}
int32_t MapControllerProxy::mal_map_lonlat_to_xy(uint32_t map_id, mal_map_coord2d_t lonlat, int32_t *x, int32_t *y)
{
    if (!realImpl_) {
        return MAL_API_NO_INIT;
    }
    LonlatToXYContext ctx{realImpl_, map_id, lonlat, x, y};
    return runner_.PostSyncTaskInt32(MalMapLonlatToXYWrapper, &ctx);
}
int32_t MapControllerProxy::mal_map_xy_to_lonlat(uint32_t map_id, int32_t x, int32_t y, mal_map_coord2d_t *lonlat)
{
    if (!realImpl_) {
        return MAL_API_NO_INIT;
    }
    XYToLonlatContext ctx{realImpl_, map_id, x, y, lonlat};
    return runner_.PostSyncTaskInt32(MalMapXYToLonlatWrapper, &ctx);
}
int32_t MapControllerProxy::mal_map_touch_begin(uint32_t map_id, int32_t x, int32_t y)
{
    if (!realImpl_) {
        return MAL_API_NO_INIT;
    }
    XYToLonlatContext ctx{realImpl_, map_id, x, y, nullptr};
    return runner_.PostSyncTaskInt32(MalMapTouchBeginWrapper, &ctx);
}
int32_t MapControllerProxy::mal_map_touch_update(uint32_t map_id, int32_t x, int32_t y)
{
    if (!realImpl_) {
        return MAL_API_NO_INIT;
    }
    XYToLonlatContext ctx{realImpl_, map_id, x, y, nullptr};
    return runner_.PostSyncTaskInt32(MalMapTouchUpdateWrapper, &ctx);
}
int32_t MapControllerProxy::mal_map_touch_end(uint32_t map_id, int32_t x, int32_t y)
{
    if (!realImpl_) {
        return MAL_API_NO_INIT;
    }
    XYToLonlatContext ctx{realImpl_, map_id, x, y, nullptr};
    return runner_.PostSyncTaskInt32(MalMapTouchEndWrapper, &ctx);
}
mal_map_point_overlay_t **MapControllerProxy::mal_map_click_points(uint32_t map_id, int32_t x, int32_t y,
                                                                   int32_t *overlay_count)
{
    if (!realImpl_) {
        return NULL;
    }
    ClickPointsContext ctx{realImpl_, map_id, x, y, overlay_count};
    void *taskResult = runner_.PostSyncTaskPtr(MalMapClickPointsWrapper, &ctx);
    return static_cast<mal_map_point_overlay_t **>(taskResult);
}
int32_t MapControllerProxy::mal_map_release_overlays(mal_map_base_overlay_t **overlays, int32_t overlay_count)
{
    if (!realImpl_) {
        return MAL_API_NO_INIT;
    }
    ReleaseOverlaysContext ctx{realImpl_, overlays, overlay_count};
    return runner_.PostSyncTaskInt32(MalMapReleaseOverlaysWrapper, &ctx);
}
mal_map_track_info_t *MapControllerProxy::mal_map_track_navi_parse_gpx(const char *gpx_data, size_t length)
{
    if (!realImpl_) {
        return NULL;
    }
    TrackNaviParseContext ctx{realImpl_, gpx_data, length};
    void *taskResult = runner_.PostSyncTaskPtr(MalMapTrackNaviParseGpxWrapper, &ctx);
    return static_cast<mal_map_track_info_t *>(taskResult);
}
mal_map_track_info_count_t *MapControllerProxy::mal_map_track_parser_json(const char *json_data, int mode)
{
    if (!realImpl_) {
        return NULL;
    }
    TrackNaviParseJsonContext ctx{realImpl_, json_data, mode};
    void *taskResult = runner_.PostSyncTaskPtr(MalMapTrckParserJsonWrapper, &ctx);
    return static_cast<mal_map_track_info_count_t *>(taskResult);
}
mal_map_poi_t *MapControllerProxy::mal_poi_search_json(const char *json_str)
{
    if (!realImpl_) {
        return NULL;
    }
    TrackNaviParseContext ctx{realImpl_, json_str, strlen(json_str)};
    void *taskResult = runner_.PostSyncTaskPtr(MalMapPoiSearchJsonWrapper, &ctx);
    return static_cast<mal_map_poi_t *>(taskResult);
}
void MapControllerProxy::mal_map_poi_mem_free(mal_map_poi_t *mal_map_poi_info)
{
    if (!realImpl_) {
        return;
    }
    PoiMemFreeContext ctx{realImpl_, mal_map_poi_info};
    runner_.PostSyncTaskVoid(MalMapPoiMemFreeWrapper, &ctx);
}
void MapControllerProxy::mal_map_track_navi_parse_track_info_count_free(mal_map_track_info_count_t *info)
{
    if (!realImpl_) {
        return;
    }
    TrackNaviParseTrackInfoCountFreeFreeContext ctx{realImpl_, info};
    runner_.PostSyncTaskVoid(MalMapTrackNaviParseTrackInfoCountFreeWrapper, &ctx);
}
void MapControllerProxy::mal_map_track_navi_parse_track_info_free(mal_map_track_info_t *track_info)
{
    if (!realImpl_) {
        return;
    }
    TrackNaviParseTrackInfoFreeFreeContext ctx{realImpl_, track_info};
    runner_.PostSyncTaskVoid(MalMapTrackNaviParseTrackInfoFreeWrapper, &ctx);
}
int32_t MapControllerProxy::mal_map_init_track_navi_overlay(mal_map_track_navi_overlay_t *overlay)
{
    if (!realImpl_) {
        return MAL_API_NO_INIT;
    }
    TrackNaviOverlayContext ctx{realImpl_, overlay, 0};
    return runner_.PostSyncTaskInt32(MalMapInitTrackNaviOverlayWrapper, &ctx);
}
int32_t MapControllerProxy::mal_map_track_navi_add_overlay(uint32_t map_id, mal_map_track_navi_overlay_t *overlay)
{
    if (!realImpl_) {
        return MAL_API_NO_INIT;
    }
    TrackNaviOverlayContext ctx{realImpl_, overlay, map_id};
    return runner_.PostSyncTaskInt32(MalMapTrackNaviAddOverlayWrapper, &ctx);
}
int32_t MapControllerProxy::mal_map_track_navi_remove_overlay(uint32_t map_id, uint32_t overlay_id)
{
    if (!realImpl_) {
        return MAL_API_NO_INIT;
    }
    GetOverlayContext ctx{realImpl_, map_id, overlay_id};
    return runner_.PostSyncTaskInt32(MalMapTrackNaviRemoveOverlayWrapper, &ctx);
}

void MapControllerProxy::mal_map_track_navi_update_real_point(uint32_t map_id, mal_map_coord2d_t point, float bear,
                                                              mal_map_track_navi_overlay_t *overlay)
{
    if (!realImpl_) {
        return;
    }
    TrackNaviUpdateRealPointContext ctx{realImpl_, map_id, point, bear, overlay};
    runner_.PostSyncTaskVoid(MalMapTrackNaviUpdateRealPointWrapper, &ctx);
}
double MapControllerProxy::mal_map_track_navi_get_distance(uint32_t map_id, mal_map_coord2d_t point,
                                                           mal_map_track_point_type point_type,
                                                           mal_map_track_navi_overlay_t *overlay)
{
    if (!realImpl_) {
        return MAL_API_NO_INIT;
    }
    TrackNaviGetDistanceContext ctx{realImpl_, map_id, point, point_type, overlay, 0.0};
    int32_t taskRet = runner_.PostSyncTaskInt32(MalMapTrackNaviGetDistanceWrapper, &ctx);
    if (taskRet != 0) {
        return MAL_API_NO_INIT;  // 自定义错误码
    }
    return ctx.result;
}

char *MapControllerProxy::mal_poi_aos_json(const char *search_name)
{
    // POI无须切线程
    if (!realImpl_) {
        return nullptr;
    }
    return realImpl_->mal_poi_aos_json(search_name);
}
char *MapControllerProxy::mal_route_aos_json(char *origin_coord, char *dest_coord, int mode)
{
    if (!realImpl_) {
        return nullptr;
    }
    return realImpl_->mal_route_aos_json(origin_coord, dest_coord, mode);
}

int32_t MapControllerProxy::mal_map_request_tiles(mal_rect_area_t req_rect, mal_map_coord2d_t map_center,
                                                  float map_level, int32_t priority)
{
    // jpeg暂不使用，待后续适配
    return 0;
}
int32_t MapControllerProxy::mal_map_cancel_request_tiles(int32_t req_id)
{
    // jpeg暂不使用，待后续适配
    return 0;
}

float MapControllerProxy::mal_map_calc_points_distance(double lon1, double lat1, double lon2, double lat2)
{
    if (!realImpl_) {
        return 0.0f;
    }

    CalcPointsDistanceContext ctx{realImpl_, lon1, lat1, lon2, lat2};

    return runner_.PostSyncTaskFloat(MalMapCalcPointsDistanceWrapper, &ctx);
}

void MapControllerProxy::mal_map_wgs84_to_mgs(double lng_wgs, double lat_wgs, double *lng_mgs, double *lat_mgs)
{
    // 暂不使用，待后续适配
}

}  // namespace OHOS