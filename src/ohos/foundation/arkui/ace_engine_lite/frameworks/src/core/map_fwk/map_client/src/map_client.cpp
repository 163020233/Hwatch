/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026. All rights reserved.
 * Description: map client implement.
 * Author: CompanyName
 * Create: 2026-01-07
 */

#include "map_client.h"
#include "ace_log.h"
#include "map_interface_impl.h"
#include "http/http_callback.h"
#include "cJSON.h"
#include "map_convert.h"
#include "ace_mem_base.h"

namespace OHOS {
namespace ACELite {

MapClient::MapClient() : MapClient(true)
{}

MapClient::MapClient(bool isMapInit)
    : httpClient_(nullptr), callbackManager_(nullptr), mapController_(nullptr), mapManager_(nullptr)
{
    httpClient_ = CreateHttpClient();
    if (httpClient_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "HttpMapClient structure failed!");
        return;
    }
    callbackManager_ = new CallbackManager();
    if (callbackManager_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "CallbackManager structure failed!");
        return;
    }
    if (isMapInit) {
        mapManager_ = MapManager::GetInstance();
        if (mapManager_ == nullptr) {
            HILOG_ERROR(HILOG_MODULE_ACE, "Get MapManager singleton failed!");
            return;
        }
        int32_t ret = mapManager_->InitMapContext();
        if (ret != MAP_API_SUCCESS) {
            HILOG_ERROR(HILOG_MODULE_ACE, "MapManager init failed!");
            return;
        }
        mapController_ = mapManager_->GetMapController();
        if (mapController_ == nullptr) {
            HILOG_ERROR(HILOG_MODULE_ACE, "Get MapController singleton failed!");
            return;
        }
    }

    httpClient_->SetMapCallback([this](uint32_t id, const char *result) {
        if (callbackManager_->GetMapCallbackType(id) == MapCallbackType::MAP_CALLBACK) {
            callbackManager_->ExecuteCallback(id, result);
        } else if (callbackManager_->GetMapCallbackType(id) == MapCallbackType::MAP_POI_CALLBACK) {
            callbackManager_->ExecutePOICallback(id, MapClient::GetPOISearchCallback(result));
        }
    });
}

MapClient::~MapClient()
{
    if (httpClient_ != nullptr) {
        HttpMessage finishMsg{};
        finishMsg.requestId = OHOS::ACELite::HTTP_FINISH;
        httpClient_->HttpMessagePut(&finishMsg);
        httpClient_->StopThread();
        delete httpClient_;
        httpClient_ = nullptr;
    }

    if (callbackManager_ != nullptr) {
        delete callbackManager_;
        callbackManager_ = nullptr;
    }

    if (mapManager_ != nullptr) {
        mapManager_->DeInitMapContext();
    }

    if (mapController_ != nullptr) {
        mapController_ = nullptr;
    }

    for (auto& pair : trackNaviOverlayCache_) {
        if (pair.second) {
            MapConverter::GetInstance().FreeMalTrackNaviOverlay(pair.second);
            ace_free(pair.second);
        }
    }
    trackNaviOverlayCache_.clear();
}

HttpModuleClient *MapClient::CreateHttpClient()
{
    HttpModuleClient *client = new HttpModuleClient{};
    if (client == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapClient Create HttpModuleClient failed!");
        return nullptr;
    }

    if (!client->InitClient()) {
        delete client;
        HILOG_ERROR(HILOG_MODULE_ACE, "MapClient Init HttpModuleClient failed!");
        return nullptr;
    }

    if (!client->CreateHttpThread()) {
        delete client;
        HILOG_ERROR(HILOG_MODULE_ACE, "MapClient CreateHttpThread failed!");
        return nullptr;
    }

    return client;
}

std::map<std::string, std::string> MapClient::GetPOISearchCallback(const char *result)
{
    std::map<std::string, std::string> poiResult;

    if (!result) {
        HILOG_ERROR(HILOG_MODULE_ACE, "GetPOISearchCallback: result is null");
        return poiResult;
    }
    // parse json
    cJSON *root = cJSON_Parse(result);
    if (!root) {
        HILOG_ERROR(HILOG_MODULE_ACE, "GetPOISearchCallback root is null ");
        return poiResult;
    }
    // check status success 1 or fail 0
    char *status = cJSON_GetStringValue(cJSON_GetObjectItem(root, "status"));
    if (!status || strcmp(status, "0") == 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "GetPOISearchCallback request is fail : %s ", result);
        cJSON_Delete(root);
        return poiResult;
    }
    // get pois
    cJSON *pois_array = cJSON_GetObjectItem(root, "pois");
    if (!pois_array || !cJSON_IsArray(pois_array)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "GetPOISearchCallback pois is not array ");
        cJSON_Delete(root);
        return poiResult;
    }
    // parse pois
    int count = cJSON_GetArraySize(pois_array);
    for (int i = 0; i < count; i++) {
        cJSON *item = cJSON_GetArrayItem(pois_array, i);
        const char *name = cJSON_GetStringValue(cJSON_GetObjectItem(item, "name"));
        const char *cityname = cJSON_GetStringValue(cJSON_GetObjectItem(item, "cityname"));
        const char *adname = cJSON_GetStringValue(cJSON_GetObjectItem(item, "adname"));
        const char *charLocation = cJSON_GetStringValue(cJSON_GetObjectItem(item, "location"));
        if (name && cityname && adname && charLocation) {
            std::string addresname = std::string(cityname) + "-" + std::string(adname) + "-" + std::string(name);
            poiResult[addresname] = charLocation;
        }
    }

    // clear json
    cJSON_Delete(root);
    return poiResult;
}

bool MapClient::GetPOIInfo(const std::map<std::string, std::string> &args, MapCallback callback)
{
    bool ret = false;

    if (!callbackManager_) {
        HILOG_ERROR(HILOG_MODULE_ACE, "GetPOIInfo Request callbackManager_ is nullptr");
        return false;
    }

    uint32_t callbackId = callbackManager_->AddCallback(callback);

    ret = MapInterfaceImpl::MapPOISearchRequests(args, httpClient_, callbackId);
    if (!ret) {
        HILOG_ERROR(HILOG_MODULE_ACE, "GetPOIInfo Request failed");
        callbackManager_->RemoveCallback(callbackId);
    }
    return ret;
}

bool MapClient::GetBikePath(const std::map<std::string, std::string> &args, MapCallback callback)
{
    bool ret = false;

    if (!callbackManager_) {
        HILOG_ERROR(HILOG_MODULE_ACE, "GetBikePath Request callbackManager_ is nullptr");
        return false;
    }

    uint32_t callbackId = callbackManager_->AddCallback(callback);

    ret = MapInterfaceImpl::MapBikePathRequests(args, httpClient_, callbackId);
    if (!ret) {
        HILOG_ERROR(HILOG_MODULE_ACE, "GetBikePath Request failed");
        callbackManager_->RemoveCallback(callbackId);
    }
    return ret;
}

bool MapClient::GetWalkPath(const std::map<std::string, std::string> &args, MapCallback callback)
{
    bool ret = false;

    if (!callbackManager_) {
        HILOG_ERROR(HILOG_MODULE_ACE, "GetWalkPath Request callbackManager_ is nullptr");
        return false;
    }

    uint32_t callbackId = callbackManager_->AddCallback(callback);

    ret = MapInterfaceImpl::MapWalkPathRequests(args, httpClient_, callbackId);
    if (!ret) {
        HILOG_ERROR(HILOG_MODULE_ACE, "GetWalkPath Request failed");
        callbackManager_->RemoveCallback(callbackId);
    }
    return ret;
}

bool MapClient::SearchPOI(const std::map<std::string, std::string> &args, POISearchCallback callback)
{
    bool ret = false;

    if (!callbackManager_) {
        HILOG_ERROR(HILOG_MODULE_ACE, "SearchPOI Request callbackManager_ is nullptr");
        return false;
    }

    uint32_t callbackId = callbackManager_->AddPOICallback(callback);

    ret = MapInterfaceImpl::MapPOISearchRequests(args, httpClient_, callbackId);
    if (!ret) {
        HILOG_ERROR(HILOG_MODULE_ACE, "SearchPOI Request failed");
        callbackManager_->RemoveCallback(callbackId);
    }
    return ret;
}

bool MapClient::GetOfflineMapList(const std::map<std::string, std::string> &args, MapCallback callback)
{
    bool ret = false;

    if (!callbackManager_) {
        HILOG_ERROR(HILOG_MODULE_ACE, "GetOfflineMapList Request callbackManager_ is nullptr");
        return false;
    }

    uint32_t callbackId = callbackManager_->AddCallback(callback);

    ret = MapInterfaceImpl::GetMapOfflineMapListRequests(args, httpClient_, callbackId);
    if (!ret) {
        HILOG_ERROR(HILOG_MODULE_ACE, "GetOfflineMapList Request failed");
        callbackManager_->RemoveCallback(callbackId);
    }
    return ret;
}

bool MapClient::GetOfflineMapInfo(const std::map<std::string, std::string> &args, MapCallback callback)
{
    bool ret = false;

    if (!callbackManager_) {
        HILOG_ERROR(HILOG_MODULE_ACE, "GetOfflineMapInfo Request callbackManager_ is nullptr");
        return false;
    }

    uint32_t callbackId = callbackManager_->AddCallback(callback);

    ret = MapInterfaceImpl::DownOfflineMapRequests(args, httpClient_, callbackId);
    if (!ret) {
        HILOG_ERROR(HILOG_MODULE_ACE, "GetOfflineMapInfo Request failed");
        callbackManager_->RemoveCallback(callbackId);
    }
    return ret;
}

bool MapClient::DownloadOfflineMap(const char *url, MapCallback callback, const char *path)
{
    bool ret = false;

    if (!callbackManager_ || !url || !path) {
        HILOG_ERROR(HILOG_MODULE_ACE, "DownloadOfflineMap Request params error");
        return false;
    }

    char *resolvedPath = nullptr;
    if (!CheckDirectoryPath(path, resolvedPath)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "DownloadOfflineMap: error path %s", path);
        return ret;
    }

    uint32_t callbackId = callbackManager_->AddCallback(callback);

    ret = MapInterfaceImpl::DownOfflineMapRequests(url, httpClient_, callbackId, resolvedPath);
    if (!ret) {
        HILOG_ERROR(HILOG_MODULE_ACE, "DownloadOfflineMap Request failed");
        callbackManager_->RemoveCallback(callbackId);
    }

    if (resolvedPath != nullptr) {
        ace_free(resolvedPath);
        resolvedPath = nullptr;
    }
    return ret;
}

int32_t MapClient::MapInit(map_context context)
{
    int32_t ret = -1;
    if (mapController_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapInit mapController is nullptr");
        return ret;
    }
    mal_context_t mal_context;
    mal_context.tile_pixel_mode = MapConverter::GetInstance().convert_pixel_mode(context.tile_pixel_mode);
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
    ret = mapController_->mal_map_init(mal_context);
    return ret;
}

int32_t MapClient::MapUninit(void)
{
    int32_t ret = -1;
    if (mapController_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapUninit mapController is nullptr");
        return ret;
    }
    ret = mapController_->mal_map_uninit();
    return ret;
}

int32_t MapClient::MapActivateDevice()
{
    int32_t ret = -1;
    if (mapController_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapActivateDevice mapController is nullptr");
        return ret;
    }
    ret = mapController_->mal_map_activate_device();
    return ret;
}

int32_t MapClient::MapCreateView(map_view_create_param param)
{
    int32_t ret = -1;
    if (mapController_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapCreateView mapController is nullptr");
        return ret;
    }
    mal_map_view_create_param_t mal_param;
    mal_param.view_param.port.width = param.view_param.port.width;
    mal_param.view_param.port.height = param.view_param.port.height;
    mal_param.map_center.lon = param.map_center.lon;
    mal_param.map_center.lat = param.map_center.lat;
    mal_param.view_param.map_view_rect.x = param.view_param.map_view_rect.x;
    mal_param.view_param.map_view_rect.y = param.view_param.map_view_rect.y;
    mal_param.view_param.map_view_rect.width = param.view_param.map_view_rect.width;
    mal_param.view_param.map_view_rect.height = param.view_param.map_view_rect.height;
    mal_param.zoom = param.zoom;
    ret = mapController_->mal_map_create_view(mal_param);
    return ret;
}

int32_t MapClient::MapDestroyView(uint32_t map_id)
{
    int32_t ret = -1;
    if (mapController_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapDestroyView mapController is nullptr");
        return ret;
    }

    ret = mapController_->mal_map_destroy_view(map_id);
    return ret;
}

int32_t MapClient::MapDoRender(void)
{
    int32_t ret = -1;
    if (mapController_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapDoRender mapController is nullptr");
        return ret;
    }
    ret = mapController_->mal_map_do_render();
    return ret;
}

int32_t MapClient::MapPauseRender(uint32_t map_id)
{
    int32_t ret = -1;
    if (mapController_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapPauseRender mapController is nullptr");
        return ret;
    }
    ret = mapController_->mal_map_pause_render(map_id);
    return ret;
}

int32_t MapClient::MapResumeRender(uint32_t map_id)
{
    int32_t ret = -1;
    if (mapController_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapResumeRender mapController is nullptr");
        return ret;
    }
    ret = mapController_->mal_map_resume_render(map_id);
    return ret;
}

int32_t MapClient::MapAddTexture(const map_texture_data *texture_data)
{
    int32_t ret = -1;
    if (mapController_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapAddTexture mapController is nullptr");
        return ret;
    }
    mal_map_texture_data_t mal_data = MapConverter::GetInstance().convert_texture(texture_data);

    ret = mapController_->mal_map_add_texture(&mal_data);

    return ret;
}

int32_t MapClient::MapUpdateTexture(uint32_t texture_id, const map_texture_data *texture_data)
{
    int32_t ret = -1;
    if (mapController_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapUpdateTexture mapController is nullptr");
        return ret;
    }
    mal_map_texture_data_t mal_data = MapConverter::GetInstance().convert_texture(texture_data);
    ret = mapController_->mal_map_update_texture(texture_id, &mal_data);
    return ret;
}

int32_t MapClient::MapRemoveTexture(uint32_t texture_id)
{
    int32_t ret = -1;
    if (mapController_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapRemoveTexture mapController is nullptr");
        return ret;
    }
    ret = mapController_->mal_map_remove_texture(texture_id);
    return ret;
}

int32_t MapClient::MapInitPointOverlay(map_point_overlay *point_overlay)
{
    int32_t ret = -1;
    if (mapController_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapInitPointOverlay mapController is nullptr");
        return ret;
    }
    mal_map_point_overlay_t mal_overlay;
    mal_overlay.base_overlay = MapConverter::GetInstance().convert_base_overlay(point_overlay->base_overlay);
    mal_overlay.position = MapConverter::GetInstance().convert_coord2d(point_overlay->position);
    mal_overlay.angle = point_overlay->angle;
    mal_overlay.normal_marker = MapConverter::GetInstance().convert_point_overlay_marker(point_overlay->normal_marker);
    mal_overlay.focus_marker = MapConverter::GetInstance().convert_point_overlay_marker(point_overlay->focus_marker);
    mal_overlay.icon_visible = point_overlay->icon_visible;
    mal_overlay.name_visible = point_overlay->name_visible;
    mal_overlay.bubble_visible = point_overlay->bubble_visible;

    ret = mapController_->mal_map_init_point_overlay(&mal_overlay);
    if (ret == MAP_API_SUCCESS) {
        point_overlay->base_overlay.geometry_type =
            MapConverter::convert_map_overlay_type(mal_overlay.base_overlay.geometry_type);
        point_overlay->base_overlay.group_id = mal_overlay.base_overlay.group_id;
        point_overlay->base_overlay.map_id = mal_overlay.base_overlay.map_id;
        point_overlay->base_overlay.priority = mal_overlay.base_overlay.priority;
        point_overlay->base_overlay.visible = mal_overlay.base_overlay.visible;
        point_overlay->base_overlay.is_focus = mal_overlay.base_overlay.is_focus;
        point_overlay->base_overlay.min_level = mal_overlay.base_overlay.min_level;
        point_overlay->base_overlay.max_level = mal_overlay.base_overlay.max_level;
        point_overlay->base_overlay.guid = mal_overlay.base_overlay.guid;
    }

    return ret;
}

int32_t MapClient::MapInitLineOverlay(map_polyline_overlay *line_overlay)
{
    int32_t ret = -1;
    if (mapController_ == nullptr || line_overlay == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapInitLineOverlay invalid parameters");
        return ret;
    }
    mal_map_polyline_overlay_t *overlay = (mal_map_polyline_overlay_t *)ace_malloc(sizeof(mal_map_polyline_overlay_t));
    if (!overlay) {
        return ret;
    }
    errno_t ret_er = memset_s(overlay, sizeof(mal_map_polyline_overlay_t), 0, sizeof(mal_map_polyline_overlay_t));
    if (ret_er != EOK) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapInitLineOverlay memset_s failed, ret: %d", ret_er);
        return ret;
    }

    overlay->base_overlay = MapConverter::GetInstance().convert_base_overlay(line_overlay->base_overlay);

    mal_map_coord2d_t *mal_points = nullptr;
    if (line_overlay->points && line_overlay->point_size > 0) {
        mal_points = (mal_map_coord2d_t *)ace_malloc(line_overlay->point_size * sizeof(mal_map_coord2d_t));
        if (mal_points) {
            for (size_t i = 0; i < line_overlay->point_size; i++) {
                mal_points[i] = MapConverter::GetInstance().convert_coord2d(line_overlay->points[i]);
            }
        }
    }
    overlay->points = mal_points;
    overlay->point_size = line_overlay->point_size;

    mal_point_64_t *mal_map_points = nullptr;
    if (line_overlay->map_points && line_overlay->point_size > 0) {
        mal_map_points = (mal_point_64_t *)ace_malloc(line_overlay->point_size * sizeof(mal_point_64_t));
        if (mal_map_points) {
            for (size_t i = 0; i < line_overlay->point_size; i++) {
                mal_map_points[i] = MapConverter::GetInstance().convert_point_64(line_overlay->map_points[i]);
            }
        }
    }
    overlay->map_points = mal_map_points;

    overlay->point_flags = line_overlay->point_flags;
    overlay->point_flags_size = line_overlay->point_flags_size;
    overlay->tolerance = line_overlay->tolerance;
    overlay->max_keep_point_size = line_overlay->max_keep_point_size;
    overlay->normal_marker = MapConverter::GetInstance().convert_line_overlay(line_overlay->normal_marker);
    overlay->focus_marker = MapConverter::GetInstance().convert_line_overlay(line_overlay->focus_marker);
    overlay->arrow_marker = MapConverter::GetInstance().convert_point_overlay_marker(line_overlay->arrow_marker);
    overlay->show_arrow = line_overlay->show_arrow;
    overlay->pre_index = line_overlay->pre_index;
    overlay->location = MapConverter::GetInstance().convert_coord2d(line_overlay->location);

    ret = mapController_->mal_map_init_line_overlay(overlay);
    if (ret == MAP_API_SUCCESS) {
        line_overlay->base_overlay.geometry_type =
            MapConverter::convert_map_overlay_type(overlay->base_overlay.geometry_type);
        line_overlay->base_overlay.group_id = overlay->base_overlay.group_id;
        line_overlay->base_overlay.map_id = overlay->base_overlay.map_id;
        line_overlay->base_overlay.priority = overlay->base_overlay.priority;
        line_overlay->base_overlay.visible = overlay->base_overlay.visible;
        line_overlay->base_overlay.is_focus = overlay->base_overlay.is_focus;
        line_overlay->base_overlay.min_level = overlay->base_overlay.min_level;
        line_overlay->base_overlay.max_level = overlay->base_overlay.max_level;
        line_overlay->base_overlay.guid = overlay->base_overlay.guid;
    }

    if (overlay->map_points) {
        ace_free(overlay->map_points);
    }
    if (overlay->points) {
        ace_free(overlay->points);
    }
    if (overlay) {
        ace_free(overlay);
    }
    return ret;
}

int32_t MapClient::MapInitPolygonOverlay(map_polygon_overlay *polygon_overlay)
{
    int32_t ret = -1;
    if (mapController_ == nullptr || polygon_overlay == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapInitPolygonOverlay invalid parameters");
        return ret;
    }

    mal_map_polygon_overlay_t overlay;
    overlay.base_overlay = MapConverter::GetInstance().convert_base_overlay(polygon_overlay->base_overlay);

    mal_map_coord2d_t *mal_points = nullptr;
    if (polygon_overlay->points && polygon_overlay->point_size > 0) {
        mal_points = (mal_map_coord2d_t *)ace_malloc(polygon_overlay->point_size * sizeof(mal_map_coord2d_t));
        if (mal_points) {
            for (size_t i = 0; i < polygon_overlay->point_size; i++) {
                mal_points[i] = MapConverter::GetInstance().convert_coord2d(polygon_overlay->points[i]);
            }
        }
    }
    overlay.points = mal_points;
    overlay.point_size = polygon_overlay->point_size;
    overlay.color = polygon_overlay->color;

    ret = mapController_->mal_map_init_polygon_overlay(&overlay);
    if (ret == MAP_API_SUCCESS) {
        polygon_overlay->base_overlay.geometry_type =
            MapConverter::convert_map_overlay_type(overlay.base_overlay.geometry_type);
        polygon_overlay->base_overlay.group_id = overlay.base_overlay.group_id;
        polygon_overlay->base_overlay.map_id = overlay.base_overlay.map_id;
        polygon_overlay->base_overlay.priority = overlay.base_overlay.priority;
        polygon_overlay->base_overlay.visible = overlay.base_overlay.visible;
        polygon_overlay->base_overlay.is_focus = overlay.base_overlay.is_focus;
        polygon_overlay->base_overlay.min_level = overlay.base_overlay.min_level;
        polygon_overlay->base_overlay.max_level = overlay.base_overlay.max_level;
        polygon_overlay->base_overlay.guid = overlay.base_overlay.guid;
    }

    if (mal_points) {
        ace_free(mal_points);
    }
    return ret;
}

int32_t MapClient::MapAddOverlay(uint32_t map_id, map_base_overlay *overlay)
{
    int32_t ret = -1;
    if (mapController_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapAddOverlay mapController is nullptr");
        return ret;
    }
    mal_map_base_overlay_t base_overlay = MapConverter::GetInstance().convert_base_overlay(*overlay);

    ret = mapController_->mal_map_add_overlay(map_id, &base_overlay);
    if (ret == MAP_API_SUCCESS) {
        overlay->geometry_type = MapConverter::convert_map_overlay_type(base_overlay.geometry_type);
        overlay->group_id = base_overlay.group_id;
        overlay->map_id = base_overlay.map_id;
        overlay->priority = base_overlay.priority;
        overlay->visible = base_overlay.visible;
        overlay->is_focus = base_overlay.is_focus;
        overlay->min_level = base_overlay.min_level;
        overlay->max_level = base_overlay.max_level;
        overlay->guid = base_overlay.guid;
    }
    return ret;
}

int32_t MapClient::MapAddPolylineOverlay(uint32_t map_id, map_polyline_overlay *line_overlay)
{
    int32_t ret = -1;
    if (mapController_ == nullptr || line_overlay == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapAddPolylineOverlay invalid parameters");
        return ret;
    }

    mal_map_polyline_overlay_t *overlay = (mal_map_polyline_overlay_t *)ace_malloc(sizeof(mal_map_polyline_overlay_t));
    if (!overlay) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapAddPolylineOverlay malloc failed");
        return ret;
    }
    errno_t ret_er = memset_s(overlay, sizeof(mal_map_polyline_overlay_t), 0, sizeof(mal_map_polyline_overlay_t));
    if (ret_er != EOK) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapAddPolylineOverlay memset_s failed, ret: %d", ret_er);
        return ret;
    }

    overlay->base_overlay = MapConverter::GetInstance().convert_base_overlay(line_overlay->base_overlay);

    mal_map_coord2d_t *mal_points = nullptr;
    if (line_overlay->points && line_overlay->point_size > 0) {
        mal_points = (mal_map_coord2d_t *)ace_malloc(line_overlay->point_size * sizeof(mal_map_coord2d_t));
        if (mal_points) {
            for (size_t i = 0; i < line_overlay->point_size; i++) {
                mal_points[i] = MapConverter::GetInstance().convert_coord2d(line_overlay->points[i]);
            }
        }
    }
    overlay->points = mal_points;
    overlay->point_size = line_overlay->point_size;

    mal_point_64_t *mal_map_points = nullptr;
    if (line_overlay->map_points && line_overlay->point_size > 0) {
        mal_map_points = (mal_point_64_t *)ace_malloc(line_overlay->point_size * sizeof(mal_point_64_t));
        if (mal_map_points) {
            for (size_t i = 0; i < line_overlay->point_size; i++) {
                mal_map_points[i] = MapConverter::GetInstance().convert_point_64(line_overlay->map_points[i]);
            }
        }
    }
    overlay->map_points = mal_map_points;

    overlay->point_flags = line_overlay->point_flags;
    overlay->point_flags_size = line_overlay->point_flags_size;
    overlay->tolerance = line_overlay->tolerance;
    overlay->max_keep_point_size = line_overlay->max_keep_point_size;
    overlay->normal_marker = MapConverter::GetInstance().convert_line_overlay(line_overlay->normal_marker);
    overlay->focus_marker = MapConverter::GetInstance().convert_line_overlay(line_overlay->focus_marker);
    overlay->arrow_marker = MapConverter::GetInstance().convert_point_overlay_marker(line_overlay->arrow_marker);
    overlay->show_arrow = line_overlay->show_arrow;
    overlay->pre_index = line_overlay->pre_index;
    overlay->location = MapConverter::GetInstance().convert_coord2d(line_overlay->location);

    ret = mapController_->mal_map_add_overlay(map_id, &overlay->base_overlay);

    if (overlay->points) {
        ace_free(overlay->points);
    }
    if (overlay->map_points) {
        ace_free(overlay->map_points);
    }
    if (overlay) {
        ace_free(overlay);
    }

    return ret;
}

int32_t MapClient::MapAddPointOverlay(uint32_t map_id, map_point_overlay *point_overlay)
{
    int32_t ret = -1;
    if (mapController_ == nullptr || point_overlay == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapAddPointOverlay invalid parameters");
        return ret;
    }

    mal_map_point_overlay_t mal_overlay;
    mal_overlay.base_overlay = MapConverter::GetInstance().convert_base_overlay(point_overlay->base_overlay);
    mal_overlay.position = MapConverter::GetInstance().convert_coord2d(point_overlay->position);
    mal_overlay.angle = point_overlay->angle;
    mal_overlay.normal_marker = MapConverter::GetInstance().convert_point_overlay_marker(point_overlay->normal_marker);
    mal_overlay.focus_marker = MapConverter::GetInstance().convert_point_overlay_marker(point_overlay->focus_marker);
    mal_overlay.icon_visible = point_overlay->icon_visible;
    mal_overlay.name_visible = point_overlay->name_visible;
    mal_overlay.bubble_visible = point_overlay->bubble_visible;

    ret = mapController_->mal_map_add_overlay(map_id, &mal_overlay.base_overlay);

    return ret;
}

int32_t MapClient::MapAddPolygonOverlay(uint32_t map_id, map_polygon_overlay *polygon_overlay)
{
    int32_t ret = -1;
    if (mapController_ == nullptr || polygon_overlay == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapInitPolygonOverlay invalid parameters");
        return ret;
    }

    mal_map_polygon_overlay_t overlay;
    overlay.base_overlay = MapConverter::GetInstance().convert_base_overlay(polygon_overlay->base_overlay);

    mal_map_coord2d_t *mal_points = nullptr;
    if (polygon_overlay->points && polygon_overlay->point_size > 0) {
        mal_points = (mal_map_coord2d_t *)ace_malloc(polygon_overlay->point_size * sizeof(mal_map_coord2d_t));
        if (mal_points) {
            for (size_t i = 0; i < polygon_overlay->point_size; i++) {
                mal_points[i] = MapConverter::GetInstance().convert_coord2d(polygon_overlay->points[i]);
            }
        }
    }

    overlay.points = mal_points;
    overlay.point_size = polygon_overlay->point_size;
    overlay.color = polygon_overlay->color;

    ret = mapController_->mal_map_add_overlay(map_id, &overlay.base_overlay);

    if (mal_points) {
        ace_free(mal_points);
    }
    return ret;
}

int32_t MapClient::MapUpdateOverlay(uint32_t map_id, map_base_overlay *overlay)
{
    int32_t ret = -1;
    if (mapController_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapUpdateOverlay mapController is nullptr");
        return ret;
    }
    mal_map_base_overlay_t base_overlay = MapConverter::GetInstance().convert_base_overlay(*overlay);
    ret = mapController_->mal_map_update_overlay(map_id, &base_overlay);
    if (ret == MAP_API_SUCCESS) {
        overlay->geometry_type = MapConverter::convert_map_overlay_type(base_overlay.geometry_type);
        overlay->group_id = base_overlay.group_id;
        overlay->map_id = base_overlay.map_id;
        overlay->priority = base_overlay.priority;
        overlay->visible = base_overlay.visible;
        overlay->is_focus = base_overlay.is_focus;
        overlay->min_level = base_overlay.min_level;
        overlay->max_level = base_overlay.max_level;
        overlay->guid = base_overlay.guid;
    }

    return ret;
}

int32_t MapClient::MapUpdatePolylineOverlay(uint32_t map_id, map_polyline_overlay *polyline_overlay)
{
    int32_t ret = -1;
    if (mapController_ == nullptr || polyline_overlay == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapUpdatePolylineOverlay invalid parameters");
        return ret;
    }

    mal_map_polyline_overlay_t *overlay = (mal_map_polyline_overlay_t *)ace_malloc(sizeof(mal_map_polyline_overlay_t));
    if (overlay == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapUpdatePolylineOverlay malloc failed");
        return ret;
    }
    errno_t ret_er = memset_s(overlay, sizeof(mal_map_polyline_overlay_t), 0, sizeof(mal_map_polyline_overlay_t));
    if (ret_er != EOK) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapUpdatePolylineOverlay memset_s failed, ret: %d", ret_er);
        ace_free(overlay);
        return ret;
    }

    overlay->base_overlay = MapConverter::GetInstance().convert_base_overlay(polyline_overlay->base_overlay);

    mal_map_coord2d_t *mal_points = nullptr;
    if (polyline_overlay->points && polyline_overlay->point_size > 0) {
        mal_points = MapConverter::GetInstance().ConvertPointsArray(polyline_overlay->points, polyline_overlay->point_size);
        overlay->points = mal_points;
        overlay->point_size = polyline_overlay->point_size;
    }

    mal_point_64_t *mal_map_points = nullptr;
    if (polyline_overlay->map_points && polyline_overlay->point_size > 0) {
        mal_map_points = (mal_point_64_t *)ace_malloc(polyline_overlay->point_size * sizeof(mal_point_64_t));
        if (mal_map_points) {
            for (size_t i = 0; i < polyline_overlay->point_size; i++) {
                mal_map_points[i] = MapConverter::GetInstance().convert_point_64(polyline_overlay->map_points[i]);
            }
        }
        overlay->map_points = mal_map_points;
    }

    overlay->point_flags = polyline_overlay->point_flags;
    overlay->point_flags_size = polyline_overlay->point_flags_size;
    overlay->tolerance = polyline_overlay->tolerance;
    overlay->max_keep_point_size = polyline_overlay->max_keep_point_size;
    overlay->normal_marker = MapConverter::GetInstance().convert_line_overlay(polyline_overlay->normal_marker);
    overlay->focus_marker = MapConverter::GetInstance().convert_line_overlay(polyline_overlay->focus_marker);
    overlay->arrow_marker = MapConverter::GetInstance().convert_point_overlay_marker(polyline_overlay->arrow_marker);
    overlay->show_arrow = polyline_overlay->show_arrow;
    overlay->pre_index = polyline_overlay->pre_index;
    overlay->location = MapConverter::GetInstance().convert_coord2d(polyline_overlay->location);
    ret = mapController_->mal_map_update_overlay(map_id, &overlay->base_overlay);
    if (ret == MAP_API_SUCCESS) {
        polyline_overlay->base_overlay.geometry_type =
            MapConverter::convert_map_overlay_type(overlay->base_overlay.geometry_type);
        polyline_overlay->base_overlay.group_id = overlay->base_overlay.group_id;
        polyline_overlay->base_overlay.map_id = overlay->base_overlay.map_id;
        polyline_overlay->base_overlay.priority = overlay->base_overlay.priority;
        polyline_overlay->base_overlay.visible = overlay->base_overlay.visible;
        polyline_overlay->base_overlay.is_focus = overlay->base_overlay.is_focus;
        polyline_overlay->base_overlay.min_level = overlay->base_overlay.min_level;
        polyline_overlay->base_overlay.max_level = overlay->base_overlay.max_level;
        polyline_overlay->base_overlay.guid = overlay->base_overlay.guid;
    }

    if (overlay->points) {
        ace_free(overlay->points);
    }
    if (overlay->map_points) {
        ace_free(overlay->map_points);
    }
    ace_free(overlay);

    return ret;
}

int32_t MapClient::MapUpdatePointOverlay(uint32_t map_id, map_point_overlay *point_overlay)
{
    int32_t ret = -1;
    if (mapController_ == nullptr || point_overlay == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapUpdatePointOverlay invalid parameters");
        return ret;
    }

    mal_map_point_overlay_t mal_overlay;
    errno_t ret_er = memset_s(&mal_overlay, sizeof(mal_map_point_overlay_t), 0, sizeof(mal_map_point_overlay_t));
    if (ret_er != EOK) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapUpdatePointOverlay memset_s failed, ret: %d", ret_er);
        return ret;
    }

    mal_overlay.base_overlay = MapConverter::GetInstance().convert_base_overlay(point_overlay->base_overlay);
    mal_overlay.position = MapConverter::GetInstance().convert_coord2d(point_overlay->position);
    mal_overlay.angle = point_overlay->angle;
    mal_overlay.normal_marker = MapConverter::GetInstance().convert_point_overlay_marker(point_overlay->normal_marker);
    mal_overlay.focus_marker = MapConverter::GetInstance().convert_point_overlay_marker(point_overlay->focus_marker);
    mal_overlay.icon_visible = point_overlay->icon_visible;
    mal_overlay.name_visible = point_overlay->name_visible;
    mal_overlay.bubble_visible = point_overlay->bubble_visible;
    ret = mapController_->mal_map_update_overlay(map_id, &mal_overlay.base_overlay);
    if (ret == MAP_API_SUCCESS) {
        point_overlay->base_overlay.geometry_type =
            MapConverter::convert_map_overlay_type(mal_overlay.base_overlay.geometry_type);
        point_overlay->base_overlay.group_id = mal_overlay.base_overlay.group_id;
        point_overlay->base_overlay.map_id = mal_overlay.base_overlay.map_id;
        point_overlay->base_overlay.priority = mal_overlay.base_overlay.priority;
        point_overlay->base_overlay.visible = mal_overlay.base_overlay.visible;
        point_overlay->base_overlay.is_focus = mal_overlay.base_overlay.is_focus;
        point_overlay->base_overlay.min_level = mal_overlay.base_overlay.min_level;
        point_overlay->base_overlay.max_level = mal_overlay.base_overlay.max_level;
        point_overlay->base_overlay.guid = mal_overlay.base_overlay.guid;
    }

    return ret;
}

int32_t MapClient::MapUpdatePolygonOverlay(uint32_t map_id, map_polygon_overlay *polygon_overlay)
{
    int32_t ret = -1;
    if (mapController_ == nullptr || polygon_overlay == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapUpdatePolygonOverlay invalid parameters");
        return ret;
    }

    mal_map_polygon_overlay_t overlay;
    errno_t ret_er = memset_s(&overlay, sizeof(mal_map_polygon_overlay_t), 0, sizeof(mal_map_polygon_overlay_t));
    if (ret_er != EOK) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapUpdatePolygonOverlay memset_s failed, ret: %d", ret_er);
        return ret;
    }

    overlay.base_overlay = MapConverter::GetInstance().convert_base_overlay(polygon_overlay->base_overlay);

    mal_map_coord2d_t *mal_points = nullptr;
    if (polygon_overlay->points && polygon_overlay->point_size > 0) {
        mal_points = MapConverter::GetInstance().ConvertPointsArray(polygon_overlay->points, polygon_overlay->point_size);
        overlay.points = mal_points;
        overlay.point_size = polygon_overlay->point_size;
    }
    overlay.color = polygon_overlay->color;
    ret = mapController_->mal_map_update_overlay(map_id, &overlay.base_overlay);
    if (ret == MAP_API_SUCCESS) {
        polygon_overlay->base_overlay.geometry_type =
            MapConverter::convert_map_overlay_type(overlay.base_overlay.geometry_type);
        polygon_overlay->base_overlay.group_id = overlay.base_overlay.group_id;
        polygon_overlay->base_overlay.map_id = overlay.base_overlay.map_id;
        polygon_overlay->base_overlay.priority = overlay.base_overlay.priority;
        polygon_overlay->base_overlay.visible = overlay.base_overlay.visible;
        polygon_overlay->base_overlay.is_focus = overlay.base_overlay.is_focus;
        polygon_overlay->base_overlay.min_level = overlay.base_overlay.min_level;
        polygon_overlay->base_overlay.max_level = overlay.base_overlay.max_level;
        polygon_overlay->base_overlay.guid = overlay.base_overlay.guid;
    }

    if (mal_points) {
        ace_free(mal_points);
    }

    return ret;
}

int32_t MapClient::MapRemoveOverlay(uint32_t map_id, uint32_t overlay_id)
{
    int32_t ret = -1;
    if (mapController_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapRemoveOverlay mapController is nullptr");
        return ret;
    }
    ret = mapController_->mal_map_remove_overlay(map_id, overlay_id);
    return ret;
}

const map_base_overlay *MapClient::MapFindOverlay(uint32_t map_id, uint32_t overlay_id)
{
    if (mapController_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapFindOverlay mapController is nullptr");
        return nullptr;
    }

    const mal_map_base_overlay_t *mal_overlay = mapController_->mal_map_find_overlay(map_id, overlay_id);
    if (mal_overlay == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapFindOverlay overlay not found");
        return nullptr;
    }

    map_base_overlay *overlay = (map_base_overlay *)ace_malloc(sizeof(map_base_overlay));
    if (overlay == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapFindOverlay ace_malloc failed");
        return nullptr;
    }

    overlay->geometry_type = MapConverter::convert_map_overlay_type(mal_overlay->geometry_type);
    overlay->group_id = mal_overlay->group_id;
    overlay->guid = mal_overlay->guid;
    overlay->is_focus = mal_overlay->is_focus;
    overlay->map_id = mal_overlay->map_id;
    overlay->max_level = mal_overlay->max_level;
    overlay->min_level = mal_overlay->min_level;
    overlay->priority = mal_overlay->priority;
    overlay->visible = mal_overlay->visible;

    return overlay;
}

uint32_t MapClient::MapGetOverlayCount(uint32_t map_id)
{
    uint32_t ret = 0;
    if (mapController_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapGetOverlayCount mapController is nullptr");
        return ret;
    }
    ret = mapController_->mal_map_get_overlay_count(map_id);
    return ret;
}

const map_base_overlay *MapClient::MapGetOverlay(uint32_t map_id, uint32_t index)
{
    if (mapController_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapGetOverlay mapController is nullptr");
        return nullptr;
    }

    const mal_map_base_overlay_t *mal_overlay = mapController_->mal_map_get_overlay(map_id, index);
    if (mal_overlay == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapGetOverlay overlay not found");
        return nullptr;
    }

    map_base_overlay *overlay = (map_base_overlay *)ace_malloc(sizeof(map_base_overlay));
    if (overlay == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapGetOverlay ace_malloc failed");
        return nullptr;
    }

    overlay->geometry_type = MapConverter::convert_map_overlay_type(mal_overlay->geometry_type);
    overlay->group_id = mal_overlay->group_id;
    overlay->guid = mal_overlay->guid;
    overlay->is_focus = mal_overlay->is_focus;
    overlay->map_id = mal_overlay->map_id;
    overlay->max_level = mal_overlay->max_level;
    overlay->min_level = mal_overlay->min_level;
    overlay->priority = mal_overlay->priority;
    overlay->visible = mal_overlay->visible;

    return overlay;
}

int32_t MapClient::MapGetMapBounds(
    uint32_t map_id, map_base_overlay *overlay, map_coord2d *north_east, map_coord2d *south_west, float *level)
{
    int32_t ret = -1;
    if (mapController_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapGetMapBounds mapController is nullptr");
        return ret;
    }
    mal_map_base_overlay_t base_overlay = MapConverter::GetInstance().convert_base_overlay(*overlay);
    mal_map_coord2d_t north_coord2d = MapConverter::GetInstance().convert_coord2d(*north_east);
    mal_map_coord2d_t south_coord2d = MapConverter::GetInstance().convert_coord2d(*south_west);
    ret = mapController_->mal_map_get_map_bounds(map_id, &base_overlay, &north_coord2d, &south_coord2d, level);
    if (ret != -1) {
        north_east->lon = north_coord2d.lon;
        north_east->lat = north_coord2d.lat;
        south_west->lon = south_coord2d.lon;
        south_west->lat = south_coord2d.lat;
    }
    return ret;
}

int32_t MapClient::MapReleaseOverlay(map_base_overlay *overlay)
{
    int32_t ret = -1;
    if (mapController_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapReleaseOverlay mapController is nullptr");
        return ret;
    }
    mal_map_base_overlay_t base_overlay = MapConverter::GetInstance().convert_base_overlay(*overlay);
    ret = mapController_->mal_map_release_overlay(&base_overlay);
    return ret;
}

int32_t MapClient::MapSetViewPort(uint32_t map_id, map_view_port view_port)
{
    int32_t ret = -1;
    if (mapController_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapSetViewPort mapController is nullptr");
        return ret;
    }
    mal_map_view_port_t mal_view_port;
    mal_view_port.width = view_port.width;
    mal_view_port.height = view_port.height;
    ret = mapController_->mal_map_set_view_port(map_id, mal_view_port);
    return ret;
}

int32_t MapClient::MapSetMapViewRect(uint32_t map_id, map_rect_area map_view_rect)
{
    int32_t ret = -1;
    if (mapController_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapSetMapViewRect mapController is nullptr");
        return ret;
    }
    mal_rect_area_t mal_rect;
    mal_rect.height = map_view_rect.height;
    mal_rect.width = map_view_rect.width;
    mal_rect.x = map_view_rect.x;
    mal_rect.y = map_view_rect.y;
    ret = mapController_->mal_map_set_map_view_rect(map_id, mal_rect);
    return ret;
}

int32_t MapClient::MapSetCenter(uint32_t map_id, map_coord2d coord2d)
{
    int32_t ret = -1;
    if (mapController_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapSetCenter mapController is nullptr");
        return ret;
    }
    mal_map_coord2d_t mal_coord2d = MapConverter::GetInstance().convert_coord2d(coord2d);
    ret = mapController_->mal_map_set_center(map_id, mal_coord2d);
    return ret;
}

int32_t MapClient::MapSetLevel(uint32_t map_id, float level)
{
    int32_t ret = -1;
    if (mapController_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapSetLevel mapController is nullptr");
        return ret;
    }
    ret = mapController_->mal_map_set_level(map_id, level);
    return ret;
}

float MapClient::MapGetLevel(uint32_t map_id)
{
    float level = 0;
    if (mapController_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapGetLevel mapController is nullptr");
        return level;
    }
    level = mapController_->mal_map_get_level(map_id);
    return level;
}

int32_t MapClient::MapSetMapViewAnchor(uint32_t map_id, map_view_anchor anchor)
{
    int32_t ret = -1;
    if (mapController_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapSetMapViewAnchor mapController is nullptr");
        return ret;
    }
    mal_map_view_anchor_t mal_view_anchor;
    mal_view_anchor.anchor_x = anchor.anchor_x;
    mal_view_anchor.anchor_y = anchor.anchor_y;
    ret = mapController_->mal_map_set_map_view_anchor(map_id, mal_view_anchor);
    return ret;
}

int32_t MapClient::MapSetRollAngle(uint32_t map_id, float roll_angle)
{
    int32_t ret = -1;
    if (mapController_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapSetRollAngle mapController is nullptr");
        return ret;
    }
    ret = mapController_->mal_map_set_roll_angle(map_id, roll_angle);
    return ret;
}

int32_t MapClient::MapGetPosture(uint32_t map_id, map_posture *posture)
{
    int32_t ret = -1;

    if (mapController_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapGetPosture mapController is nullptr");
        return ret;
    }
    mal_map_posture_t mal_posture;
    mal_posture.map_center = MapConverter::GetInstance().convert_coord2d(posture->map_center);
    mal_posture.view_port = MapConverter::GetInstance().convert_view_port(posture->view_port);
    mal_posture.drawing_port = MapConverter::GetInstance().convert_view_port(posture->drawing_port);
    mal_posture.level = posture->level;
    mal_posture.scale = posture->scale;
    mal_posture.roll_angle = posture->roll_angle;
    ret = mapController_->mal_map_get_posture(map_id, &mal_posture);
    if (ret != -1) {
        posture->map_center.lon = mal_posture.map_center.lon;
        posture->map_center.lat = mal_posture.map_center.lat;
        posture->view_port.width = mal_posture.view_port.width;
        posture->view_port.height = mal_posture.view_port.height;
        posture->drawing_port.width = mal_posture.drawing_port.width;
        posture->drawing_port.height = mal_posture.drawing_port.height;
        posture->level = mal_posture.level;
        posture->scale = mal_posture.scale;
        posture->roll_angle = mal_posture.roll_angle;
    }
    return ret;
}

int32_t MapClient::MapSetVisibleBounds(uint32_t map_id, map_navi_point_bounds bounds, map_edge_insets insets)
{
    int32_t ret = -1;
    if (mapController_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapSetVisibleBounds mapController is nullptr");
        return ret;
    }
    mal_navi_point_bounds_t mal_bounds;
    mal_edge_insets_t mal_insets;
    mal_bounds.north_east = MapConverter::GetInstance().convert_coord2d(bounds.north_east);
    mal_bounds.south_west = MapConverter::GetInstance().convert_coord2d(bounds.south_west);
    mal_insets.bottom = insets.bottom;
    mal_insets.top = insets.top;
    mal_insets.left = insets.left;
    mal_insets.right = insets.right;
    ret = mapController_->mal_map_set_visible_bounds(map_id, mal_bounds, mal_insets);
    return ret;
}

int32_t MapClient::MapLonlatToXY(uint32_t map_id, map_coord2d lonlat, int32_t *x, int32_t *y)
{
    int32_t ret = -1;
    if (mapController_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapLonlatToXY mapController is nullptr");
        return ret;
    }
    mal_map_coord2d_t mal_coord2d = MapConverter::GetInstance().convert_coord2d(lonlat);
    ret = mapController_->mal_map_lonlat_to_xy(map_id, mal_coord2d, x, y);
    return ret;
}

int32_t MapClient::MapXYToLonlat(uint32_t map_id, int32_t x, int32_t y, map_coord2d *lonlat)
{
    int32_t ret = -1;
    if (mapController_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapXYToLonlat mapController is nullptr");
        return ret;
    }
    mal_map_coord2d_t mal_coord2d = MapConverter::GetInstance().convert_coord2d(*lonlat);
    ret = mapController_->mal_map_xy_to_lonlat(map_id, x, y, &mal_coord2d);
    if (ret != -1) {
        lonlat->lon = mal_coord2d.lon;
        lonlat->lat = mal_coord2d.lat;
    }
    return ret;
}

int32_t MapClient::MapTouchBegin(uint32_t map_id, int32_t x, int32_t y)
{
    int32_t ret = -1;
    if (mapController_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapTouchBegin mapController is nullptr");
        return ret;
    }
    ret = mapController_->mal_map_touch_begin(map_id, x, y);
    return ret;
}

int32_t MapClient::MapTouchUpdate(uint32_t map_id, int32_t x, int32_t y)
{
    int32_t ret = -1;
    if (mapController_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapTouchUpdate mapController is nullptr");
        return ret;
    }
    ret = mapController_->mal_map_touch_update(map_id, x, y);
    return ret;
}

int32_t MapClient::MapTouchEnd(uint32_t map_id, int32_t x, int32_t y)
{
    int32_t ret = -1;
    if (mapController_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapTouchEnd mapController is nullptr");
        return ret;
    }
    ret = mapController_->mal_map_touch_end(map_id, x, y);
    return ret;
}

map_point_overlay **MapClient::MapClickPoints(uint32_t map_id, int32_t x, int32_t y, int32_t *overlay_count)
{
    if (mapController_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapClickPoints mapController is nullptr");
        if (overlay_count) {
            *overlay_count = 0;
        }
        return nullptr;
    }

    mal_map_point_overlay_t **mal_overlays = mapController_->mal_map_click_points(map_id, x, y, overlay_count);
    if (mal_overlays == nullptr || overlay_count == nullptr || *overlay_count <= 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapClickPoints no overlay found");
        if (overlay_count) {
            *overlay_count = 0;
        }
        return nullptr;
    }
    int32_t count = *overlay_count;
    map_point_overlay **results = (map_point_overlay **)ace_malloc(count * sizeof(map_point_overlay *));
    if (results == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapClickPoints ace_malloc failed");
        *overlay_count = 0;
        return nullptr;
    }
    errno_t ret_er = memset_s(results, count * sizeof(map_point_overlay *), 0, count * sizeof(map_point_overlay *));
    if (ret_er != EOK) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapClickPoints memset_s failed, ret: %d", ret_er);
        return nullptr;
    }
    for (int32_t i = 0; i < count; i++) {
        mal_map_point_overlay_t *mal = mal_overlays[i];
        if (!mal) {
            continue;
        }
        results[i] = (map_point_overlay *)ace_malloc(sizeof(map_point_overlay));
        if (!results[i]) {
            for (int32_t j = 0; j < i; j++) {
                if (results[j]) {
                    ace_free(results[j]);
                }
            }
            ace_free(results);
            *overlay_count = 0;
            return nullptr;
        }
        errno_t ret_er = memset_s(results[i], sizeof(map_point_overlay), 0, sizeof(map_point_overlay));
        if (ret_er != EOK) {
            HILOG_ERROR(HILOG_MODULE_ACE, "MapClickPoints  memset_s failed, ret: %d", ret_er);
            return nullptr;
        }
        results[i]->base_overlay.geometry_type =
            MapConverter::convert_map_overlay_type(mal->base_overlay.geometry_type);
        results[i]->base_overlay.group_id = mal->base_overlay.group_id;
        results[i]->base_overlay.guid = mal->base_overlay.guid;
        results[i]->base_overlay.is_focus = mal->base_overlay.is_focus;
        results[i]->base_overlay.map_id = mal->base_overlay.map_id;
        results[i]->base_overlay.max_level = mal->base_overlay.max_level;
        results[i]->base_overlay.min_level = mal->base_overlay.min_level;
        results[i]->base_overlay.priority = mal->base_overlay.priority;
        results[i]->base_overlay.visible = mal->base_overlay.visible;

        results[i]->position.lon = mal->position.lon;
        results[i]->position.lat = mal->position.lat;

        results[i]->angle = mal->angle;
        results[i]->icon_visible = mal->icon_visible;
        results[i]->name_visible = mal->name_visible;
        results[i]->bubble_visible = mal->bubble_visible;

        results[i]->normal_marker = MapConverter::GetInstance().convert_map_point_overlay_marker(mal->normal_marker);
        results[i]->focus_marker = MapConverter::GetInstance().convert_map_point_overlay_marker(mal->focus_marker);
    }
    return results;
}

int32_t MapClient::MapReleaseOverlays(map_base_overlay **overlays, int32_t overlay_count)
{
    int32_t ret = -1;
    if (mapController_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapReleaseOverlays mapController is nullptr");
        return ret;
    }

    if (overlays == nullptr || *overlays == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapReleaseOverlays overlays error");
        return ret;
    }

    mal_map_base_overlay_t **mal_overlays =
        (mal_map_base_overlay_t **)ace_malloc(overlay_count * sizeof(mal_map_base_overlay_t *));
    if (mal_overlays == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapReleaseOverlays ace_malloc failed");
        return ret;
    }

    for (int32_t i = 0; i < overlay_count; i++) {
        if (!overlays[i]) {
            mal_overlays[i] = nullptr;
            continue;
        }
        mal_map_base_overlay_t *p_mal = (mal_map_base_overlay_t *)ace_malloc(sizeof(mal_map_base_overlay_t));
        if (p_mal == nullptr) {
            for (int32_t j = 0; j < i; j++) {
                if (mal_overlays[j]) {
                    ace_free(mal_overlays[j]);
                }
            }
            ace_free(mal_overlays);
            return ret;
        }

        *p_mal = MapConverter::GetInstance().convert_base_overlay(*overlays[i]);
        mal_overlays[i] = p_mal;
    }
    ret = mapController_->mal_map_release_overlays(mal_overlays, overlay_count);
    for (int32_t i = 0; i < overlay_count; i++) {
        if (mal_overlays[i]) {
            ace_free(mal_overlays[i]);
        }
    }
    ace_free(mal_overlays);
    return ret;
}

map_track_info *MapClient::MapTrackNaviParseGpx(const char *gpx_data, size_t length)
{
    if (mapController_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapTrackNaviParseGpx mapController is nullptr");
        return nullptr;
    }

    if (gpx_data == nullptr || length == 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapTrackNaviParseGpx invalid input");
        return nullptr;
    }

    mal_map_track_info_t *info = mapController_->mal_map_track_navi_parse_gpx(gpx_data, length);
    if (info == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapTrackNaviParseGpx parse failed");
        return nullptr;
    }

    map_track_info *map_info = (map_track_info *)ace_malloc(sizeof(map_track_info));
    if (map_info == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapTrackNaviParseGpx ace_malloc failed");
        return nullptr;
    }
    errno_t ret_er = memset_s(map_info, sizeof(map_track_info), 0, sizeof(map_track_info));
    if (ret_er != EOK) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapTrackNaviParseGpx  memset_s failed, ret: %d", ret_er);
        return nullptr;
    }
    map_info->name = info->name;
    map_info->desc = info->desc;
    map_info->point_size = info->point_size;
    map_info->via_point_index_size = info->via_point_index_size;
    map_info->turn_point_size = info->turn_point_size;

    if (info->points && info->point_size > 0) {
        map_info->points = (map_coord2d *)ace_malloc(info->point_size * sizeof(map_coord2d));
        if (map_info->points) {
            for (size_t i = 0; i < info->point_size; i++) {
                map_info->points[i].lon = info->points[i].lon;
                map_info->points[i].lat = info->points[i].lat;
            }
        }
    }

    if (info->via_point_indexes && info->via_point_index_size > 0) {
        size_t size = info->via_point_index_size * sizeof(int32_t);
        map_info->via_point_indexes = (int32_t *)ace_malloc(size);
        if (map_info->via_point_indexes) {
            errno_t ret_er = memcpy_s(map_info->via_point_indexes, size, info->via_point_indexes, size);
            if (ret_er != EOK) {
                HILOG_ERROR(HILOG_MODULE_ACE, "MapTrackNaviParseGpx  memset_s failed, ret: %d", ret_er);
                return nullptr;
            }
        }
    }

    if (info->turn_points && info->turn_point_size > 0) {
        map_info->turn_points =
            (map_track_turn_point_info *)ace_malloc(info->turn_point_size * sizeof(map_track_turn_point_info));

        if (map_info->turn_points) {
            for (size_t i = 0; i < info->turn_point_size; i++) {
                mal_map_track_turn_point_info_t *src = &info->turn_points[i];
                map_track_turn_point_info *dst = &map_info->turn_points[i];

                dst->index = src->index;
                dst->bear = src->bear;

                dst->turn_message = src->turn_message;
                dst->turn_location_message = src->turn_location_message;
            }
        }
    }
    return map_info;
}

map_track_info_count *MapClient::MapTrackParserJson(const char *json_data, int mode)
{
    if (mapController_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapTrackParserJson mapController is nullptr");
        return nullptr;
    }

    if (json_data == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapTrackParserJson json_data is nullptr");
        return nullptr;
    }

    mal_map_track_info_count_t *track = mapController_->mal_map_track_parser_json(json_data, mode);
    if (track == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapTrackParserJson track is nullptr");
        return nullptr;
    }

    if (track->count <= 0 || !track->tracks) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapTrackParserJson no tracks");
        return nullptr;
    }

    map_track_info_count *map_track = (map_track_info_count *)ace_malloc(sizeof(map_track_info_count));
    if (map_track == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapTrackParserJson ace_malloc failed");
        return nullptr;
    }

    errno_t ret_er = memset_s(map_track, sizeof(map_track_info_count), 0, sizeof(map_track_info_count));
    if (ret_er != EOK) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapTrackParserJson map_track memset_s failed");
        return nullptr;
    }
    map_track->count = track->count;

    map_track->tracks = (map_track_info **)ace_malloc(track->count * sizeof(map_track_info *));
    if (map_track->tracks == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapTrackParserJson ace_malloc tracks failed");
        ace_free(map_track);
        return nullptr;
    }
    ret_er = memset_s(map_track->tracks, track->count * sizeof(map_track_info *), 0,
                      track->count * sizeof(map_track_info *));
    if (ret_er != EOK) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapTrackParserJson memset_s failed");
        ace_free(map_track->tracks);
        ace_free(map_track);
        return nullptr;
    }

    for (int32_t i = 0; i < track->count; ++i) {
        mal_map_track_info_t *mal_info = track->tracks[i];
        if (!mal_info) {
            continue;
        }

        map_track_info *info = (map_track_info *)ace_malloc(sizeof(map_track_info));
        if (!info) {
            FreeTrackInfoCount(map_track);
            return nullptr;
        }
        ret_er = memset_s(info, sizeof(map_track_info), 0, sizeof(map_track_info));
        if (ret_er != EOK) {
            ace_free(info);
            FreeTrackInfoCount(map_track);
            return nullptr;
        }

        if (mal_info->name) {
            size_t name_len = strlen(mal_info->name) + 1;
            info->name = (char *)ace_malloc(name_len);
            if (info->name) {
                ret_er = strcpy_s(info->name, name_len, mal_info->name);
                if (ret_er != 0) {
                    HILOG_ERROR(HILOG_MODULE_ACE, "MapTrackParserJson strcpy_s name failed, ret=%d", ret_er);
                    ace_free(info->name);
                    ace_free(info);
                    FreeTrackInfoCount(map_track);
                    return nullptr;
                }
            } else {
                HILOG_ERROR(HILOG_MODULE_ACE, "MapTrackParserJson malloc name failed");
                ace_free(info);
                FreeTrackInfoCount(map_track);
                return nullptr;
            }
        }

        if (mal_info->desc) {
            size_t desc_len = strlen(mal_info->desc) + 1;
            info->desc = (char *)ace_malloc(desc_len);
            if (info->desc) {
                ret_er = strcpy_s(info->desc, desc_len, mal_info->desc);
                if (ret_er != 0) {
                    HILOG_ERROR(HILOG_MODULE_ACE, "MapTrackParserJson strcpy_s desc failed, ret=%d", ret_er);
                    if (info->name) ace_free(info->name);
                    ace_free(info->desc);
                    ace_free(info);
                    FreeTrackInfoCount(map_track);
                    return nullptr;
                }
            } else {
                HILOG_ERROR(HILOG_MODULE_ACE, "MapTrackParserJson malloc desc failed");
                if (info->name) ace_free(info->name);
                ace_free(info);
                FreeTrackInfoCount(map_track);
                return nullptr;
            }
        }

        info->point_size = mal_info->point_size;
        info->via_point_index_size = mal_info->via_point_index_size;
        info->turn_point_size = mal_info->turn_point_size;

        if (mal_info->points && mal_info->point_size > 0) {
            info->points = (map_coord2d *)ace_malloc(mal_info->point_size * sizeof(map_coord2d));
            if (info->points) {
                for (size_t j = 0; j < mal_info->point_size; ++j) {
                    info->points[j].lon = mal_info->points[j].lon;
                    info->points[j].lat = mal_info->points[j].lat;
                }
            } else {
                HILOG_ERROR(HILOG_MODULE_ACE, "MapTrackParserJson malloc points failed");
                if (info->name) ace_free(info->name);
                if (info->desc) ace_free(info->desc);
                ace_free(info);
                FreeTrackInfoCount(map_track);
                return nullptr;
            }
        }

        if (mal_info->via_point_indexes && mal_info->via_point_index_size > 0) {
            size_t size = mal_info->via_point_index_size * sizeof(int32_t);
            info->via_point_indexes = (int32_t *)ace_malloc(size);
            if (info->via_point_indexes) {
                ret_er = memcpy_s(info->via_point_indexes, size, mal_info->via_point_indexes, size);
                if (ret_er != 0) {
                    HILOG_ERROR(HILOG_MODULE_ACE, "MapTrackParserJson memcpy_s via_point_indexes failed, ret=%d", ret_er);
                    if (info->points) ace_free(info->points);
                    if (info->name) ace_free(info->name);
                    if (info->desc) ace_free(info->desc);
                    ace_free(info);
                    FreeTrackInfoCount(map_track);
                    return nullptr;
                }
            } else {
                HILOG_ERROR(HILOG_MODULE_ACE, "MapTrackParserJson malloc via_point_indexes failed");
                if (info->points) ace_free(info->points);
                if (info->name) ace_free(info->name);
                if (info->desc) ace_free(info->desc);
                ace_free(info);
                FreeTrackInfoCount(map_track);
                return nullptr;
            }
        }

        if (mal_info->turn_points && mal_info->turn_point_size > 0) {
            info->turn_points = (map_track_turn_point_info *)ace_malloc(
                mal_info->turn_point_size * sizeof(map_track_turn_point_info));
            if (info->turn_points) {
                bool alloc_failed = false;
                for (size_t j = 0; j < mal_info->turn_point_size; ++j) {
                    mal_map_track_turn_point_info_t *src = &mal_info->turn_points[j];
                    map_track_turn_point_info *dst = &info->turn_points[j];
                    dst->index = src->index;
                    dst->bear = src->bear;

                    if (src->turn_message) {
                        size_t msg_len = strlen(src->turn_message) + 1;
                        dst->turn_message = (char *)ace_malloc(msg_len);
                        if (dst->turn_message) {
                            ret_er = strcpy_s(dst->turn_message, msg_len, src->turn_message);
                            if (ret_er != 0) {
                                HILOG_ERROR(HILOG_MODULE_ACE, "MapTrackParserJson strcpy_s turn_message failed, ret=%d", ret_er);
                                ace_free(dst->turn_message);
                                alloc_failed = true;
                                break;
                            }
                        } else {
                            HILOG_ERROR(HILOG_MODULE_ACE, "MapTrackParserJson malloc turn_message failed");
                            alloc_failed = true;
                            break;
                        }
                    } else {
                        dst->turn_message = nullptr;
                    }

                    if (src->turn_location_message) {
                        size_t loc_len = strlen(src->turn_location_message) + 1;
                        dst->turn_location_message = (char *)ace_malloc(loc_len);
                        if (dst->turn_location_message) {
                            ret_er = strcpy_s(dst->turn_location_message, loc_len, src->turn_location_message);
                            if (ret_er != 0) {
                                HILOG_ERROR(HILOG_MODULE_ACE, "MapTrackParserJson strcpy_s turn_location_message failed, ret=%d", ret_er);
                                if (dst->turn_message) ace_free(dst->turn_message);
                                ace_free(dst->turn_location_message);
                                alloc_failed = true;
                                break;
                            }
                        } else {
                            HILOG_ERROR(HILOG_MODULE_ACE, "MapTrackParserJson malloc turn_location_message failed");
                            if (dst->turn_message) ace_free(dst->turn_message);
                            alloc_failed = true;
                            break;
                        }
                    } else {
                        dst->turn_location_message = nullptr;
                    }
                }

                if (alloc_failed) {
                    for (size_t k = 0; k < mal_info->turn_point_size; ++k) {
                        if (info->turn_points[k].turn_message)
                            ace_free(info->turn_points[k].turn_message);
                        if (info->turn_points[k].turn_location_message)
                            ace_free(info->turn_points[k].turn_location_message);
                    }
                    ace_free(info->turn_points);
                    if (info->points) ace_free(info->points);
                    if (info->via_point_indexes) ace_free(info->via_point_indexes);
                    if (info->name) ace_free(info->name);
                    if (info->desc) ace_free(info->desc);
                    ace_free(info);
                    FreeTrackInfoCount(map_track);
                    return nullptr;
                }
            } else {
                HILOG_ERROR(HILOG_MODULE_ACE, "MapTrackParserJson malloc turn_points failed");
                if (info->points) ace_free(info->points);
                if (info->via_point_indexes) ace_free(info->via_point_indexes);
                if (info->name) ace_free(info->name);
                if (info->desc) ace_free(info->desc);
                ace_free(info);
                FreeTrackInfoCount(map_track);
                return nullptr;
            }
        }

        map_track->tracks[i] = info;
    }

    return map_track;
}

char *MapClient::PoiAosJson(const char *search_name)
{
    char *info = nullptr;
    if (mapController_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "PoiAosJson mapController is nullptr");
        return info;
    }
    info = mapController_->mal_poi_aos_json(search_name);
    return info;
}

char *MapClient::RouteAosJson(char *origin_coord, char *dest_coord, int mode)
{
    char *info = nullptr;
    if (mapController_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "RouteAosJson mapController is nullptr");
        return info;
    }
    info = mapController_->mal_route_aos_json(origin_coord, dest_coord, mode);
    return info;
}

map_poi *MapClient::PoiSearchJson(const char *json_str)
{
    if (mapController_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "PoiSearchJson mapController is nullptr");
        return nullptr;
    }

    if (json_str == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "PoiSearchJson json_str is nullptr");
        return nullptr;
    }

    mal_map_poi_t *poi = mapController_->mal_poi_search_json(json_str);
    if (poi == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "PoiSearchJson parse failed");
        return nullptr;
    }

    map_poi *poi_result = (map_poi *)ace_malloc(sizeof(map_poi));
    if (poi_result == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "PoiSearchJson ace_malloc failed");
        return nullptr;
    }

    errno_t ret_er = memset_s(poi_result, sizeof(map_poi), 0, sizeof(map_poi));
    if (ret_er != EOK) {
        HILOG_ERROR(HILOG_MODULE_ACE, "PoiSearchJson memset_s failed");
        return nullptr;
    }
    poi_result->count = poi->count;
    poi_result->infocode = poi->infocode;
    if (poi->poi_info && poi->count > 0) {
        poi_result->poi_info = (map_poi_info *)ace_malloc(poi->count * sizeof(map_poi_info));
        if (poi_result->poi_info) {
            for (int i = 0; i < poi->count; i++) {
                mal_map_poi_info_t *src = &poi->poi_info[i];
                map_poi_info *dst = &poi_result->poi_info[i];
                dst->name = src->name;
                dst->id = src->id;
                dst->type = src->type;
                dst->pname = src->pname;
                dst->cityname = src->cityname;
                dst->adname = src->adname;
                dst->address = src->address;
                dst->point.lon = src->point.lon;
                dst->point.lat = src->point.lat;
                dst->navi.entr_location.lon = src->navi.entr_location.lon;
                dst->navi.entr_location.lat = src->navi.entr_location.lat;
                dst->business.keytag = src->business.keytag;
                dst->business.tel = src->business.tel;
                dst->business.opentime_week = src->business.opentime_week;
                dst->business.business_area = src->business.business_area;
            }
        } else {
            ace_free(poi_result);
            return nullptr;
        }
    }
    return poi_result;
}

void MapClient::MapPoiMemFree(map_poi *map_poi_info_ptr)
{
    if (mapController_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapPoiMemFree mapController is nullptr");
        return;
    }

    if (map_poi_info_ptr == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapPoiMemFree map_poi_info is nullptr");
        return;
    }

    mal_map_poi_t *mal_poi = (mal_map_poi_t *)ace_malloc(sizeof(mal_map_poi_t));
    if (mal_poi == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapPoiMemFree ace_malloc mal_poi failed");
        return;
    }

    errno_t ret_er = memset_s(mal_poi, sizeof(mal_map_poi_t), 0, sizeof(mal_map_poi_t));
    if (ret_er != EOK) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapPoiMemFree mal_poi memset_s failed");
        return;
    }
    mal_poi->count = map_poi_info_ptr->count;
    mal_poi->infocode = map_poi_info_ptr->infocode;

    if (map_poi_info_ptr->poi_info && map_poi_info_ptr->count > 0) {
        mal_poi->poi_info = (mal_map_poi_info_t *)ace_malloc(map_poi_info_ptr->count * sizeof(mal_map_poi_info_t));
        if (mal_poi->poi_info) {
            for (int i = 0; i < map_poi_info_ptr->count; i++) {
                map_poi_info *src = &map_poi_info_ptr->poi_info[i];
                mal_map_poi_info_t *dst = &mal_poi->poi_info[i];
                dst->name = src->name;
                dst->id = src->id;
                dst->type = src->type;
                dst->pname = src->pname;
                dst->cityname = src->cityname;
                dst->adname = src->adname;
                dst->address = src->address;
                dst->point.lon = src->point.lon;
                dst->point.lat = src->point.lat;
                dst->navi.entr_location.lon = src->navi.entr_location.lon;
                dst->navi.entr_location.lat = src->navi.entr_location.lat;
                dst->business.keytag = src->business.keytag;
                dst->business.tel = src->business.tel;
                dst->business.opentime_week = src->business.opentime_week;
                dst->business.business_area = src->business.business_area;
            }
        }
    }

    mapController_->mal_map_poi_mem_free(mal_poi);
    if (map_poi_info_ptr->poi_info) {
        ace_free(map_poi_info_ptr->poi_info);
    }
    ace_free(map_poi_info_ptr);
}

void MapClient::MapTrackNaviParseTrackInfoCountFree(map_track_info_count *info)
{
    if (mapController_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapTrackNaviParseTrackInfoCountFree mapController is nullptr");
        return;
    }

    if (info == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapTrackNaviParseTrackInfoCountFree info is nullptr");
        return;
    }

    mal_map_track_info_count_t mal_info;
    errno_t ret_er = memset_s(&mal_info, sizeof(mal_map_track_info_count_t), 0, sizeof(mal_map_track_info_count_t));
    if (ret_er != EOK) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapTrackNaviParseTrackInfoCountFree memset_s fail");
        return;
    }
    mal_info.count = info->count;
    if (info->tracks && info->count > 0) {
        mal_info.tracks = (mal_map_track_info_t **)ace_malloc(info->count * sizeof(mal_map_track_info_t *));
        if (mal_info.tracks) {
            ret_er = memset_s(mal_info.tracks,
                info->count * sizeof(mal_map_track_info_t *),
                0,
                info->count * sizeof(mal_map_track_info_t *));
            if (ret_er != EOK) {
                HILOG_ERROR(HILOG_MODULE_ACE, "MapTrackNaviParseTrackInfoCountFree tracks memset_s fail");
                return;
            }
            for (int32_t i = 0; i < info->count; i++) {
                if (!info->tracks[i])
                    continue;
                mal_info.tracks[i] = (mal_map_track_info_t *)ace_malloc(sizeof(mal_map_track_info_t));
                if (!mal_info.tracks[i])
                    continue;

                ret_er = memset_s(mal_info.tracks[i], sizeof(mal_map_track_info_t), 0, sizeof(mal_map_track_info_t));
                if (ret_er != EOK) {
                    HILOG_ERROR(HILOG_MODULE_ACE, "MapTrackNaviParseTrackInfoCountFree tracks[i] memset_s fail");
                    return;
                }
                mal_info.tracks[i]->name = info->tracks[i]->name;
                mal_info.tracks[i]->desc = info->tracks[i]->desc;
                mal_info.tracks[i]->point_size = info->tracks[i]->point_size;
                mal_info.tracks[i]->via_point_index_size = info->tracks[i]->via_point_index_size;
                mal_info.tracks[i]->turn_point_size = info->tracks[i]->turn_point_size;
                if (info->tracks[i]->points && info->tracks[i]->point_size > 0) {
                    mal_info.tracks[i]->points =
                        (mal_map_coord2d_t *)ace_malloc(info->tracks[i]->point_size * sizeof(mal_map_coord2d_t));
                    if (mal_info.tracks[i]->points) {
                        for (size_t j = 0; j < info->tracks[i]->point_size; j++) {
                            mal_info.tracks[i]->points[j].lon = info->tracks[i]->points[j].lon;
                            mal_info.tracks[i]->points[j].lat = info->tracks[i]->points[j].lat;
                        }
                    }
                }
                if (info->tracks[i]->via_point_indexes && info->tracks[i]->via_point_index_size > 0) {
                    size_t size = info->tracks[i]->via_point_index_size * sizeof(int32_t);
                    mal_info.tracks[i]->via_point_indexes = (int32_t *)ace_malloc(size);
                    if (mal_info.tracks[i]->via_point_indexes) {
                        ret_er = memcpy_s(
                            mal_info.tracks[i]->via_point_indexes, size, info->tracks[i]->via_point_indexes, size);
                        if (ret_er != EOK) {
                            HILOG_ERROR(HILOG_MODULE_ACE,
                                "MapTrackNaviParseTrackInfoCountFree via_point_indexes memset_s fail");
                            return;
                        }
                    }
                }
                if (info->tracks[i]->turn_points && info->tracks[i]->turn_point_size > 0) {
                    mal_info.tracks[i]->turn_points = (mal_map_track_turn_point_info_t *)ace_malloc(
                        info->tracks[i]->turn_point_size * sizeof(mal_map_track_turn_point_info_t));
                    if (mal_info.tracks[i]->turn_points) {
                        for (size_t j = 0; j < info->tracks[i]->turn_point_size; j++) {
                            map_track_turn_point_info *src = &info->tracks[i]->turn_points[j];
                            mal_map_track_turn_point_info_t *dst = &mal_info.tracks[i]->turn_points[j];
                            dst->index = src->index;
                            dst->bear = src->bear;
                            dst->turn_message = src->turn_message;
                            dst->turn_location_message = src->turn_location_message;
                        }
                    }
                }
            }
        }
    }
    mapController_->mal_map_track_navi_parse_track_info_count_free(&mal_info);
    if (mal_info.tracks) {
        ace_free(mal_info.tracks);
    }

    if (info->tracks) {
        for (int32_t i = 0; i < info->count; i++) {
            if (info->tracks[i]) {
                if (info->tracks[i]->points)
                    ace_free(info->tracks[i]->points);
                if (info->tracks[i]->via_point_indexes)
                    ace_free(info->tracks[i]->via_point_indexes);
                if (info->tracks[i]->turn_points)
                    ace_free(info->tracks[i]->turn_points);
                ace_free(info->tracks[i]);
            }
        }
        ace_free(info->tracks);
    }
    ace_free(info);
}

void MapClient::MapTrackNaviParseTrackInfoFree(map_track_info *track_info)
{
    if (mapController_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapTrackNaviParseTrackInfoFree mapController is nullptr");
        return;
    }

    if (track_info == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapTrackNaviParseTrackInfoFree track_info is nullptr");
        return;
    }

    mal_map_track_info_t mal_info;
    errno_t ret_er = memset_s(&mal_info, sizeof(mal_map_track_info_t), 0, sizeof(mal_map_track_info_t));
    if (ret_er != EOK) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapTrackNaviParseTrackInfoFree memset_s fail");
        return;
    }
    mal_info.name = track_info->name;
    mal_info.desc = track_info->desc;
    mal_info.point_size = track_info->point_size;
    mal_info.via_point_index_size = track_info->via_point_index_size;
    mal_info.turn_point_size = track_info->turn_point_size;

    if (track_info->points && track_info->point_size > 0) {
        mal_info.points = (mal_map_coord2d_t *)ace_malloc(track_info->point_size * sizeof(mal_map_coord2d_t));
        if (mal_info.points) {
            for (size_t i = 0; i < track_info->point_size; i++) {
                mal_info.points[i].lon = track_info->points[i].lon;
                mal_info.points[i].lat = track_info->points[i].lat;
            }
        }
    }

    if (track_info->via_point_indexes && track_info->via_point_index_size > 0) {
        size_t size = track_info->via_point_index_size * sizeof(int32_t);
        mal_info.via_point_indexes = (int32_t *)ace_malloc(size);
        if (mal_info.via_point_indexes) {
            ret_er = memcpy_s(mal_info.via_point_indexes, size, track_info->via_point_indexes, size);
            if (ret_er != EOK) {
                HILOG_ERROR(HILOG_MODULE_ACE, "MapTrackNaviParseTrackInfoFree memcpy_s fail");
                return;
            }
        }
    }

    if (track_info->turn_points && track_info->turn_point_size > 0) {
        mal_info.turn_points = (mal_map_track_turn_point_info_t *)ace_malloc(
            track_info->turn_point_size * sizeof(mal_map_track_turn_point_info_t));

        if (mal_info.turn_points) {
            for (size_t i = 0; i < track_info->turn_point_size; i++) {
                map_track_turn_point_info *src = &track_info->turn_points[i];
                mal_map_track_turn_point_info_t *dst = &mal_info.turn_points[i];
                dst->index = src->index;
                dst->bear = src->bear;

                dst->turn_message = src->turn_message;
                dst->turn_location_message = src->turn_location_message;
            }
        }
    }

    mapController_->mal_map_track_navi_parse_track_info_free(&mal_info);

    if (mal_info.points) {
        ace_free(mal_info.points);
    }

    if (mal_info.via_point_indexes) {
        ace_free(mal_info.via_point_indexes);
    }

    if (mal_info.turn_points) {
        ace_free(mal_info.turn_points);
    }

    if (track_info->points) {
        ace_free(track_info->points);
    }

    if (track_info->via_point_indexes) {
        ace_free(track_info->via_point_indexes);
    }

    if (track_info->turn_points) {
        ace_free(track_info->turn_points);
    }

    ace_free(track_info);
}

int32_t MapClient::MapInitTrackNaviOverlay(map_track_navi_overlay *overlay)
{
    int32_t ret = -1;
    if (mapController_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapInitTrackNaviOverlay mapController is nullptr");
        return ret;
    }

    if (overlay == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapInitTrackNaviOverlay overlay is nullptr");
        return ret;
    }

    mal_map_track_navi_overlay_t *mal_overlay = GetTrackNaviOverlay(overlay);

    if (mal_overlay == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapInitTrackNaviOverlay mal_overlay is nullptr");
        return ret;
    }

    MapConverter::GetInstance().convert_track_listener(&overlay->listener);
    mal_overlay->listener.on_track_pass_point = MapConverter::GetInstance().track_on_track_pass_point;
    mal_overlay->listener.on_trajector_yaw = MapConverter::GetInstance().track_on_trajector_yaw;
    mal_overlay->listener.on_trajector_yaw_end = MapConverter::GetInstance().track_on_trajector_yaw_end;
    mal_overlay->listener.on_navi_event = MapConverter::GetInstance().track_on_navi_event;
    ret = mapController_->mal_map_init_track_navi_overlay(mal_overlay);
    if (ret == MAP_API_SUCCESS) {
        overlay->base_overlay.geometry_type =
            MapConverter::convert_map_overlay_type(mal_overlay->base_overlay.geometry_type);
        overlay->base_overlay.group_id = mal_overlay->base_overlay.group_id;
        overlay->base_overlay.map_id = mal_overlay->base_overlay.map_id;
        overlay->base_overlay.priority = mal_overlay->base_overlay.priority;
        overlay->base_overlay.visible = mal_overlay->base_overlay.visible;
        overlay->base_overlay.is_focus = mal_overlay->base_overlay.is_focus;
        overlay->base_overlay.min_level = mal_overlay->base_overlay.min_level;
        overlay->base_overlay.max_level = mal_overlay->base_overlay.max_level;
        overlay->base_overlay.guid = mal_overlay->base_overlay.guid;
    }

    return ret;
}

int32_t MapClient::MapTrackNaviAddOverlay(uint32_t map_id, map_track_navi_overlay *overlay)
{
    int32_t ret = -1;
    if (mapController_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapTrackNaviAddOverlay mapController is nullptr");
        return ret;
    }

    if (overlay == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapTrackNaviAddOverlay overlay is nullptr");
        return ret;
    }

    mal_map_track_navi_overlay_t *mal_overlay = GetTrackNaviOverlay(overlay);

    if (mal_overlay == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapTrackNaviAddOverlay mal_overlay is nullptr");
        return -1;
    }

    MapConverter::GetInstance().convert_track_listener(&overlay->listener);
    mal_overlay->listener.on_track_pass_point = MapConverter::GetInstance().track_on_track_pass_point;
    mal_overlay->listener.on_trajector_yaw = MapConverter::GetInstance().track_on_trajector_yaw;
    mal_overlay->listener.on_trajector_yaw_end = MapConverter::GetInstance().track_on_trajector_yaw_end;
    mal_overlay->listener.on_navi_event = MapConverter::GetInstance().track_on_navi_event;

    ret = mapController_->mal_map_track_navi_add_overlay(map_id, mal_overlay);
    if (ret == MAP_API_SUCCESS) {
        overlay->base_overlay.geometry_type =
            MapConverter::convert_map_overlay_type(mal_overlay->base_overlay.geometry_type);
        overlay->base_overlay.group_id = mal_overlay->base_overlay.group_id;
        overlay->base_overlay.map_id = mal_overlay->base_overlay.map_id;
        overlay->base_overlay.priority = mal_overlay->base_overlay.priority;
        overlay->base_overlay.visible = mal_overlay->base_overlay.visible;
        overlay->base_overlay.is_focus = mal_overlay->base_overlay.is_focus;
        overlay->base_overlay.min_level = mal_overlay->base_overlay.min_level;
        overlay->base_overlay.max_level = mal_overlay->base_overlay.max_level;
        overlay->base_overlay.guid = mal_overlay->base_overlay.guid;
    }

    return ret;
}

int32_t MapClient::MapTrackNaviRemoveOverlay(uint32_t map_id, map_track_navi_overlay *overlay)
{
    int32_t ret = -1;
    if (mapController_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapTrackNaviRemoveOverlay mapController is nullptr");
        return ret;
    }
    int32_t overlay_id = overlay->base_overlay.guid;
    if (overlay_id <= 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapTrackNaviRemoveOverlay invalid overlay_id");
        return ret;
    }

    ret = mapController_->mal_map_track_navi_remove_overlay(map_id, overlay_id);

    auto it = trackNaviOverlayCache_.find(overlay);
    if (it != trackNaviOverlayCache_.end()) {
        mal_map_track_navi_overlay_t *mal = it->second;
        if (mal) {
            MapConverter::GetInstance().FreeMalTrackNaviOverlay(mal);
            ace_free(mal);
        }
        trackNaviOverlayCache_.erase(it);
    }
    return ret;
}

void MapClient::MapTrackNaviUpdateRealPoint(
    uint32_t map_id, map_coord2d point, float bear, map_track_navi_overlay *overlay)
{
    if (mapController_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapTrackNaviUpdateRealPoint mapController is nullptr");
        return;
    }
    if (overlay == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapTrackNaviUpdateRealPoint invalid parameters");
        return;
    }

    mal_map_coord2d_t mal_point = MapConverter::GetInstance().convert_coord2d(point);

    mal_map_track_navi_overlay_t *mal_overlay = GetTrackNaviOverlay(overlay);

    if (mal_overlay == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Failed to allocate mal_overlay");
        return;
    }

    mal_overlay->listener.on_track_pass_point = MapConverter::GetInstance().track_on_track_pass_point;
    mal_overlay->listener.on_trajector_yaw = MapConverter::GetInstance().track_on_trajector_yaw;
    mal_overlay->listener.on_trajector_yaw_end = MapConverter::GetInstance().track_on_trajector_yaw_end;
    mal_overlay->listener.on_navi_event = MapConverter::GetInstance().track_on_navi_event;

    mapController_->mal_map_track_navi_update_real_point(map_id, mal_point, bear, mal_overlay);

    overlay->base_overlay.geometry_type =
        MapConverter::convert_map_overlay_type(mal_overlay->base_overlay.geometry_type);

    overlay->base_overlay.group_id = mal_overlay->base_overlay.group_id;
    overlay->base_overlay.map_id = mal_overlay->base_overlay.map_id;
    overlay->base_overlay.priority = mal_overlay->base_overlay.priority;
    overlay->base_overlay.visible = mal_overlay->base_overlay.visible;
    overlay->base_overlay.is_focus = mal_overlay->base_overlay.is_focus;
    overlay->base_overlay.min_level = mal_overlay->base_overlay.min_level;
    overlay->base_overlay.max_level = mal_overlay->base_overlay.max_level;
    overlay->base_overlay.guid = mal_overlay->base_overlay.guid;
}

double MapClient::MapTrackNaviGetDistance(
    uint32_t map_id, map_coord2d point, map_track_point_type point_type, map_track_navi_overlay *overlay)
{
    double distance = -1.0;

    if (mapController_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapTrackNaviGetDistance mapController is nullptr");
        return distance;
    }

    if (overlay == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapTrackNaviGetDistance invalid parameters");
        return distance;
    }

    mal_map_coord2d_t mal_point = MapConverter::GetInstance().convert_coord2d(point);

    mal_map_track_point_type mal_point_type = (mal_map_track_point_type)point_type;

    mal_map_track_navi_overlay_t mal_overlay;
    errno_t ret_er =
        memset_s(&mal_overlay, sizeof(mal_map_track_navi_overlay_t), 0, sizeof(mal_map_track_navi_overlay_t));
    if (ret_er != EOK) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapTrackNaviGetDistance memset_s faild");
        return distance;
    }

    mal_overlay.base_overlay.guid = overlay->base_overlay.guid;
    mal_overlay.base_overlay.map_id = overlay->base_overlay.map_id;

    mal_overlay.track_info = (mal_map_track_info_t *)ace_malloc(sizeof(mal_map_track_info_t));
    if (mal_overlay.track_info) {
        MapConverter::GetInstance().ConvertTrackInfo(overlay->track_info, mal_overlay.track_info, true);
    }

    mal_overlay.navi_options.point_correcttion_threshold = overlay->navi_options.point_correcttion_threshold;
    mal_overlay.navi_options.navi_event_distance_threshold = overlay->navi_options.navi_event_distance_threshold;

    distance = mapController_->mal_map_track_navi_get_distance(map_id, mal_point, mal_point_type, &mal_overlay);

    if (mal_overlay.track_info) {
        MapConverter::GetInstance().FreeMalTrackInfo(mal_overlay.track_info);
        ace_free(mal_overlay.track_info);
    }

    return distance;
}

int32_t MapClient::MapRequestTiles(map_rect_area req_rect, map_coord2d map_center, float map_level, int32_t priority)
{
    int32_t ret = -1;
    if (mapController_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapRequestTiles mapController is nullptr");
        return ret;
    }
    mal_rect_area_t mal_area = MapConverter::GetInstance().convert_rect_area(&req_rect);
    mal_map_coord2d_t mal_coord2d = MapConverter::GetInstance().convert_coord2d(map_center);
    ret = mapController_->mal_map_request_tiles(mal_area, mal_coord2d, map_level, priority);
    return ret;
}

int32_t MapClient::MapCancelRequestTiles(int32_t req_id)
{
    int32_t ret = -1;
    if (mapController_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapCancelRequestTiles mapController is nullptr");
        return ret;
    }
    ret = mapController_->mal_map_cancel_request_tiles(req_id);
    return ret;
}

double MapClient::MapCalcPointsDistance(double lon1, double lat1, double lon2, double lat2)
{
    double distance = 0;
    if (mapController_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapCalcPointsDistance mapController is nullptr");
        return distance;
    }
    distance = mapController_->mal_map_calc_points_distance(lon1, lat1, lon2, lat2);
    return distance;
}

void MapClient::MapWgs84ToMgs(double lng_wgs, double lat_wgs, double *lng_mgs, double *lat_mgs)
{
    if (mapController_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapWgs84ToMgs mapController is nullptr");
        return;
    }
    mapController_->mal_map_wgs84_to_mgs(lng_wgs, lat_wgs, lng_mgs, lat_mgs);
}

int32_t MapClient::BindMapToView(int32_t mapId, IMapRender *mapView)
{
    int32_t ret = -1;
    if (mapManager_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "BindMapToView mapController is nullptr");
        return ret;
    }
    ret = mapManager_->BindMapToView(mapId, mapView);
    return ret;
}

int32_t MapClient::UnBindMapToView(int32_t mapId)
{
    int32_t ret = -1;
    if (mapManager_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "UnBindMapToView mapController is nullptr");
        return ret;
    }
    ret = mapManager_->UnBindMapToView(mapId);
    return ret;
}

void MapClient::FreeTrackInfo(map_track_info *info)
{
    if (info == nullptr) {
        return;
    }

    if (info->name) {
        ace_free(info->name);
        info->name = nullptr;
    }
    if (info->desc) {
        ace_free(info->desc);
        info->desc = nullptr;
    }
    if (info->points) {
        ace_free(info->points);
        info->points = nullptr;
    }
    if (info->via_point_indexes) {
        ace_free(info->via_point_indexes);
        info->via_point_indexes = nullptr;
    }
    if (info->turn_points) {
        for (size_t i = 0; i < info->turn_point_size; ++i) {
            if (info->turn_points[i].turn_message) {
                ace_free(info->turn_points[i].turn_message);
            }
            if (info->turn_points[i].turn_location_message) {
                ace_free(info->turn_points[i].turn_location_message);
            }
        }
        ace_free(info->turn_points);
        info->turn_points = nullptr;
    }
    ace_free(info);
}

void MapClient::FreeTrackInfoCount(map_track_info_count *infoCount)
{
    if (infoCount == nullptr) {
        return;
    }
    if (infoCount->tracks) {
        for (int32_t i = 0; i < infoCount->count; ++i) {
            FreeTrackInfo(infoCount->tracks[i]);
        }
        ace_free(infoCount->tracks);
        infoCount->tracks = nullptr;
    }
    ace_free(infoCount);
}

bool MapClient::CheckDirectoryPath(const char *originalPath, char *&outResolvedPath)
{
    outResolvedPath = nullptr;

    if (originalPath == nullptr || originalPath[0] == '\0') {
        return false;
    }

    std::string pathStr(originalPath);
    size_t lastSlash = pathStr.find_last_of('/');
    std::string dirPart, fileName;

    if (lastSlash != std::string::npos) {
        dirPart = pathStr.substr(0, lastSlash);
        fileName = pathStr.substr(lastSlash + 1);
    } else {
        dirPart = ".";
        fileName = pathStr;
    }

    char *realDir = nullptr;
    if (!dirPart.empty()) {
        realDir = realpath(dirPart.c_str(), nullptr);
    }

    if (realDir == nullptr) {
        HILOG_ERROR(
            HILOG_MODULE_ACE, "ResolveDirectoryPath: realpath failed for dir %s, errno=%d", dirPart.c_str(), errno);
        return false;
    }

    std::string fullPath = std::string(realDir) + "/" + fileName;
    size_t len = fullPath.size() + 1;
    outResolvedPath = static_cast<char *>(ace_malloc(len));
    if (outResolvedPath == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "ResolveDirectoryPath: ace_malloc failed");
        free(realDir);
        return false;
    }

    if (memcpy_s(outResolvedPath, len, fullPath.c_str(), len) != EOK) {
        ace_free(outResolvedPath);
        outResolvedPath = nullptr;
        free(realDir);
        return false;
    }

    free(realDir);
    return true;
}

mal_map_track_navi_overlay_t *MapClient::GetTrackNaviOverlay(map_track_navi_overlay *overlay)
{
    if (overlay == nullptr) {
        return nullptr;
    }

    auto it = trackNaviOverlayCache_.find(overlay);
    mal_map_track_navi_overlay_t *mal = nullptr;
    if (it == trackNaviOverlayCache_.end()) {
        mal = (mal_map_track_navi_overlay_t *)ace_malloc(sizeof(mal_map_track_navi_overlay_t));
        if (mal == nullptr) {
            HILOG_ERROR(HILOG_MODULE_ACE, "GetTrackNaviOverlay malloc failed");
            return nullptr;
        }
        errno_t ret_er = memset_s(mal, sizeof(mal_map_track_navi_overlay_t), 0, sizeof(mal_map_track_navi_overlay_t));
        if (ret_er != EOK) {
            ace_free(mal);
            return nullptr;
        }
        trackNaviOverlayCache_[overlay] = mal;
    } else {
        mal = it->second;
    }

    MapConverter::GetInstance().FreeMalTrackNaviOverlay(mal);
    if (!MapConverter::GetInstance().ConvertTrackNaviOverlay(overlay, mal)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "GetOrUpdateTrackNaviOverlay ConvertTrackNaviOverlay failed");
        return nullptr;
    }
    return mal;
}

}  // namespace ACELite
}  // namespace OHOS
