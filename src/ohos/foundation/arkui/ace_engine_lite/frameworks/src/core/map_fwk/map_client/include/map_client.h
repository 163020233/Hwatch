/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026. All rights reserved.
 * Description: map client.
 * Author: CompanyName
 * Create: 2026-01-07
 */
#ifndef MAP_CLIENT_IMPLEMENT_H
#define MAP_CLIENT_IMPLEMENT_H

#include <string>
#include <vector>
#include "httpclient.h"
#include "http/http_module_client.h"
#include "map_callback_manager.h"
#include "awk_map_controller.h"
#include "map_manager.h"
#include "map/map_render_interface.h"
#include "map_render_manager.h"
#include "map_common_defines.h"
#include "map_defines.h"
#include "mal_map_defines.h"

namespace OHOS {
namespace ACELite {

class MapClient : public MemoryHeap {
public:
    MapClient();
    MapClient(bool isMapInit);
    ~MapClient();

    MapClient(const MapClient&) = delete;
    MapClient& operator=(const MapClient&) = delete;

    // Http API
    bool SearchPOI(const std::map<std::string, std::string> &args, POISearchCallback callback);
    bool GetBikePath(const std::map<std::string, std::string> &args, MapCallback callback);
    bool GetWalkPath(const std::map<std::string, std::string> &args, MapCallback callback);
    bool GetPOIInfo(const std::map<std::string, std::string> &args, MapCallback callback);
    bool GetOfflineMapList(const std::map<std::string, std::string> &args, MapCallback callback);
    bool GetOfflineMapInfo(const std::map<std::string, std::string> &args, MapCallback callback);
    bool DownloadOfflineMap(const char *url, MapCallback callback, const char *path);

    // MapManager interface
    int32_t BindMapToView(int32_t mapId, IMapRender *mapView);
    int32_t UnBindMapToView(int32_t mapId);

    // IMapController interface
    int32_t MapInit(map_context context);
    int32_t MapUninit(void);
    int32_t MapActivateDevice();
    int32_t MapCreateView(map_view_create_param param);
    int32_t MapDestroyView(uint32_t map_id);
    int32_t MapDoRender(void);
    int32_t MapPauseRender(uint32_t map_id);
    int32_t MapResumeRender(uint32_t map_id);

    int32_t MapAddTexture(const map_texture_data *texture_data);
    int32_t MapUpdateTexture(uint32_t texture_id, const map_texture_data *texture_data);
    int32_t MapRemoveTexture(uint32_t texture_id);

    int32_t MapInitPointOverlay(map_point_overlay *point_overlay);
    int32_t MapInitLineOverlay(map_polyline_overlay *line_overlay);
    int32_t MapInitPolygonOverlay(map_polygon_overlay *polygon_overlay);
    int32_t MapAddOverlay(uint32_t map_id, map_base_overlay *overlay);
    // addMapOverlay start
    int32_t MapAddPolylineOverlay(uint32_t map_id, map_polyline_overlay *polyline_overlay);
    int32_t MapAddPointOverlay(uint32_t map_id, map_point_overlay *point_overlay);
    int32_t MapAddPolygonOverlay(uint32_t map_id, map_polygon_overlay *polygon_overlay);
    // addMapOverlay end
    int32_t MapUpdateOverlay(uint32_t map_id, map_base_overlay *overlay);

    // updateMapOverlay start
    int32_t MapUpdatePolylineOverlay(uint32_t map_id, map_polyline_overlay *polyline_overlay);
    int32_t MapUpdatePointOverlay(uint32_t map_id, map_point_overlay *point_overlay);
    int32_t MapUpdatePolygonOverlay(uint32_t map_id, map_polygon_overlay *polygon_overlay);
    // updateMapOverlay end
    int32_t MapRemoveOverlay(uint32_t map_id, uint32_t overlay_id);
    const map_base_overlay *MapFindOverlay(uint32_t map_id, uint32_t overlay_id);
    uint32_t MapGetOverlayCount(uint32_t map_id);
    const map_base_overlay *MapGetOverlay(uint32_t map_id, uint32_t index);
    int32_t MapGetMapBounds(
        uint32_t map_id, map_base_overlay *overlay, map_coord2d *north_east, map_coord2d *south_west, float *level);
    int32_t MapReleaseOverlay(map_base_overlay *overlay);
    int32_t MapSetViewPort(uint32_t map_id, map_view_port view_port);
    int32_t MapSetMapViewRect(uint32_t map_id, map_rect_area map_view_rect);
    int32_t MapSetCenter(uint32_t map_id, map_coord2d coord2d);
    int32_t MapSetLevel(uint32_t map_id, float level);
    float MapGetLevel(uint32_t map_id);
    int32_t MapSetMapViewAnchor(uint32_t map_id, map_view_anchor anchor);
    int32_t MapSetRollAngle(uint32_t map_id, float roll_angle);
    int32_t MapGetPosture(uint32_t map_id, map_posture *posture);
    int32_t MapSetVisibleBounds(uint32_t map_id, map_navi_point_bounds bounds, map_edge_insets insets);
    int32_t MapLonlatToXY(uint32_t map_id, map_coord2d lonlat, int32_t *x, int32_t *y);
    int32_t MapXYToLonlat(uint32_t map_id, int32_t x, int32_t y, map_coord2d *lonlat);
    int32_t MapTouchBegin(uint32_t map_id, int32_t x, int32_t y);
    int32_t MapTouchUpdate(uint32_t map_id, int32_t x, int32_t y);
    int32_t MapTouchEnd(uint32_t map_id, int32_t x, int32_t y);
    map_point_overlay **MapClickPoints(uint32_t map_id, int32_t x, int32_t y, int32_t *overlay_count);
    int32_t MapReleaseOverlays(map_base_overlay **overlays, int32_t overlay_count);
    map_track_info *MapTrackNaviParseGpx(const char *gpx_data, size_t length);
    map_track_info_count *MapTrackParserJson(const char *json_data, int mode);
    char *PoiAosJson(const char *search_name);
    char *RouteAosJson(char *origin_coord, char *dest_coord, int mode);
    map_poi *PoiSearchJson(const char *json_str);

    void MapPoiMemFree(map_poi *map_poi_info);
    void MapTrackNaviParseTrackInfoCountFree(map_track_info_count *info);

    void MapTrackNaviParseTrackInfoFree(map_track_info *track_info);
    int32_t MapInitTrackNaviOverlay(map_track_navi_overlay *overlay);
    int32_t MapTrackNaviAddOverlay(uint32_t map_id, map_track_navi_overlay *overlay);
    int32_t MapTrackNaviRemoveOverlay(uint32_t map_id, map_track_navi_overlay *overlay);
    void MapTrackNaviUpdateRealPoint(uint32_t map_id, map_coord2d point, float bear, map_track_navi_overlay *overlay);
    double MapTrackNaviGetDistance(
        uint32_t map_id, map_coord2d point, map_track_point_type point_type, map_track_navi_overlay *overlay);
    int32_t MapRequestTiles(map_rect_area req_rect, map_coord2d map_center, float map_level, int32_t priority);

    int32_t MapCancelRequestTiles(int32_t req_id);
    double MapCalcPointsDistance(double lon1, double lat1, double lon2, double lat2);
    void MapWgs84ToMgs(double lng_wgs, double lat_wgs, double *lng_mgs, double *lat_mgs);

    static void FreeTrackInfo(map_track_info *info);

    static void FreeTrackInfoCount(map_track_info_count *infoCount);

private:
    static HttpModuleClient *CreateHttpClient();

    static std::map<std::string, std::string> GetPOISearchCallback(const char *result);

    bool CheckDirectoryPath(const char* originPath, char*& resolvedPath);

    mal_map_track_navi_overlay_t* GetTrackNaviOverlay(map_track_navi_overlay* overlay);

    HttpModuleClient *httpClient_;

    CallbackManager *callbackManager_;

    IMapController *mapController_;

    MapManager *mapManager_;

    std::map<map_track_navi_overlay*, mal_map_track_navi_overlay_t*> trackNaviOverlayCache_;
};
}  // namespace ACELite
}  // namespace OHOS
#endif  // MAP_INTERFACE_IMPLEMENT_H
