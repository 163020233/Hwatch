/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026. All rights reserved.
 * Description: map component implement.
 * Author: CompanyName
 * Create: 2026-03-09
 */

#include <cstdlib>
#include <cctype>
#include "acelite_config.h"
#include "map_component.h"
#include "ace_log.h"
#include "key_parser.h"
#include "keys.h"
#include "js_fwk_common.h"
#include "jsi.h"
#include "handler.h"

namespace OHOS {
namespace ACELite {

const char *const MapComponent::FUNC_ZOOM_IN = "zoomIn";
const char *const MapComponent::FUNC_ZOOM_OUT = "zoomOut";
const char *const MapComponent::TOUCH_ENABLE = "touchEnable";
const char *const MapComponent::PREVIEW_ROUTE = "previewRoute";
const char *const MapComponent::SELECT_ROUTE = "selectRoute";
const char *const MapComponent::ROUTEID = "routeId";
const char *const MapComponent::STARTNAVI = "startNavi";
const char *const MapComponent::UPDATENAVI = "updateNavi";
const char *const MapComponent::ENABLE = "enable";
// 开发临时调用KEY
const char *const MapComponent::API_KEY = "cebd61d505fb29c68e8c7cfd8b255e46";

MapComponent::MapComponent(jerry_value_t options, jerry_value_t children, AppStyleManager *styleManager)
    : Component(options, children, styleManager)
{
    SetComponentName(K_MAP);
    jerry_set_object_native_pointer(options, this, nullptr);
    RegisterNamedFunction(FUNC_ZOOM_IN, ZoomIn);
    RegisterNamedFunction(FUNC_ZOOM_OUT, ZoomOut);
    RegisterNamedFunction(TOUCH_ENABLE, TouchEnable);
    RegisterNamedFunction(PREVIEW_ROUTE, PreviewRoute);
    RegisterNamedFunction(SELECT_ROUTE, SelectPreviewRoute);
    RegisterNamedFunction(STARTNAVI, StartNavi);
    RegisterNamedFunction(UPDATENAVI, UpdateNavi);

    if (mapClient_ == nullptr) {
        mapClient_ = new MapClient();
    }

    if (mapView_ == nullptr) {
        mapView_ = new UIAMapView();
    }
}

MapComponent::~MapComponent()
{
    if (routeOverlayId_ > 0) {
        mapClient_->MapRemoveOverlay(map_id_, routeOverlayId_);
    }

    if (naviOverlayId_ > 0 && trackNaviOverlay_) {
        mapClient_->MapTrackNaviRemoveOverlay(map_id_, trackNaviOverlay_);
    }

    if (turnPoints_ != nullptr) {
        for (size_t i = 0; i < turnPointSize_; i++) {
            if (turnPoints_[i].turn_message != nullptr) {
                ace_free(turnPoints_[i].turn_message);
            }
            if (turnPoints_[i].turn_location_message != nullptr) {
                ace_free(turnPoints_[i].turn_location_message);
            }
        }
        ace_free(turnPoints_);
        turnPoints_ = nullptr;
    }

    ClearTrackNaviOverlay(trackNaviOverlay_);
    trackNaviOverlay_ = nullptr;

    mapClient_->UnBindMapToView(map_id_);
    mapClient_->MapDestroyView(map_id_);

    if (mapView_ != nullptr && dragListener_ != nullptr) {
        mapView_->SetOnDragListener(nullptr);
    }

    if (dragListener_ != nullptr) {
        delete dragListener_;
        dragListener_ = nullptr;
    }

    if (mapClient_ != nullptr) {
        delete mapClient_;
        mapClient_ = nullptr;
    }

    if (mapView_ != nullptr) {
        delete mapView_;
        mapView_ = nullptr;
    }

    if (!jerry_value_is_undefined(previewRouteCallback_)) {
        jerry_release_value(previewRouteCallback_);
        previewRouteCallback_ = jerry_create_undefined();
    }

    if (!jerry_value_is_undefined(naviRouteCallback_)) {
        jerry_release_value(naviRouteCallback_);
        naviRouteCallback_ = jerry_create_undefined();
    }

    routeResults_.clear();
}

void MapComponent::CreateViews()
{
    int32_t ret = -1;
    if (mapClient_ == nullptr || mapView_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "CreateViews mapClient_ or mapView_ is nullptr");
        return;
    }

    ret = mapClient_->MapActivateDevice();
    if (ret != MAP_API_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapActivateDevice fail");
        return;
    }

    map_view_create_param params{};
    params.view_param.port.width = GetWidth();
    params.view_param.port.height = GetHeight();
    params.view_param.map_view_rect.x = 0;
    params.view_param.map_view_rect.y = 0;
    params.view_param.map_view_rect.width = GetWidth();
    params.view_param.map_view_rect.height = GetHeight();
    params.map_center.lon = mapCenterLon;
    params.map_center.lat = mapCenterLat;
    params.zoom = 15.0f;
    mapZoom_ = params.zoom;
    map_id_ = mapClient_->MapCreateView(params);
    if (map_id_ <= 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "GetComponentRootView MapCreateView FAIL");
        return;
    }

    ret = mapClient_->BindMapToView(map_id_, mapView_);
    if (ret != MAP_API_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_ACE, "BindMapToView fail");
        return;
    }
    mapView_->SetPosition(0, 0, GetWidth(), GetHeight());

    if (dragListener_ == nullptr) {
        dragListener_ = new MapDragListener(this);
    }

    mapView_->SetOnDragListener(dragListener_);
    mapView_->SetTouchable(true);
    mapView_->SetDraggable(true);
    mapView_->SetVisible(true);

    ret = mapClient_->MapDoRender();
    if (ret != MAP_API_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapDoRender fail");
        return;
    }
}

UIView *MapComponent::GetComponentRootView() const
{
    if (GetWidth() > 0 && GetHeight() > 0 && !isViewInit) {
        const_cast<MapComponent *>(this)->CreateViews();
        isViewInit = true;
    }
    return const_cast<UIAMapView *>(mapView_);
}

void MapComponent::ClearTrackNaviOverlay(map_track_navi_overlay *overlay)
{
    if (overlay != nullptr) {
        if (overlay->track_info && mapClient_ != nullptr) {
            mapClient_->FreeTrackInfo(overlay->track_info);
        } else {
            HILOG_ERROR(HILOG_MODULE_ACE, "ClearTrackNaviOverlay FreeTrackInfo fail");
        }

        if (overlay->plan_polyline_options.point_flags) {
            ace_free(overlay->plan_polyline_options.point_flags);
        }

        if (overlay->navi_options.pass_point_icon) {
            ace_free(overlay->navi_options.pass_point_icon);
        }
        if (overlay->navi_options.turn_point_icon) {
            ace_free(overlay->navi_options.turn_point_icon);
        }
        if (overlay->navi_options.pass_point_around_threshold) {
            ace_free(overlay->navi_options.pass_point_around_threshold);
        }
        if (overlay->navi_options.turn_point_around_threshold) {
            ace_free(overlay->navi_options.turn_point_around_threshold);
        }

        ace_free(overlay);
        overlay = nullptr;
    }
}

JSValue MapComponent::HandleZoomIn()
{
    float newZoom = mapZoom_ + STEP_MAP_ZOOM;
    if (newZoom > MAX_MAP_ZOOM) {
        newZoom = MAX_MAP_ZOOM;
    }

    if (newZoom != mapZoom_ && mapClient_ != nullptr) {
        int32_t ret = mapClient_->MapSetLevel(map_id_, newZoom);
        if (ret == MAP_API_SUCCESS) {
            mapZoom_ = newZoom;
            mapClient_->MapDoRender();
        } else {
            HILOG_ERROR(HILOG_MODULE_ACE, "ZoomIn failed, ret: %d", ret);
        }
    }
    return UNDEFINED;
}

JSValue MapComponent::HandleZoomOut()
{
    float newZoom = mapZoom_ - STEP_MAP_ZOOM;
    if (newZoom < MIN_MAP_ZOOM) {
        newZoom = MIN_MAP_ZOOM;
    }

    if (newZoom != mapZoom_ && mapClient_ != nullptr) {
        int32_t ret = mapClient_->MapSetLevel(map_id_, newZoom);
        if (ret == MAP_API_SUCCESS) {
            mapZoom_ = newZoom;
            mapClient_->MapDoRender();
        } else {
            HILOG_ERROR(HILOG_MODULE_ACE, "ZoomOut failed, ret: %d", ret);
        }
    }
    return UNDEFINED;
}

JSValue MapComponent::HandleTouchEnable(bool touch)
{
    touchEnable_ = touch;
    return UNDEFINED;
}

MapComponent *MapComponent::GetMapComponent(jerry_value_t context)
{
    MapComponent *component = nullptr;
    void *nativePointer = nullptr;
    if (jerry_value_is_object(context)) {
        jerry_get_object_native_pointer(context, &nativePointer, nullptr);
        component = reinterpret_cast<MapComponent *>(nativePointer);
    }

    if (component == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Failed to get MapComponent from context");
    }

    return component;
}

JSValue MapComponent::ZoomIn(
    const jerry_value_t func, const jerry_value_t context, const jerry_value_t args[], const jerry_length_t argsSize)
{
    MapComponent *component = GetMapComponent(context);
    if (component == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapComponent ZoomIn component is nullptr");
        return UNDEFINED;
    }

    return component->HandleZoomIn();
}

JSValue MapComponent::ZoomOut(
    const jerry_value_t func, const jerry_value_t context, const jerry_value_t args[], const jerry_length_t argsSize)
{
    MapComponent *component = GetMapComponent(context);
    if (component == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapComponent ZoomOut component is nullptr");
        return UNDEFINED;
    }

    return component->HandleZoomOut();
}

JSValue MapComponent::StartNavi(
    const jerry_value_t func, const jerry_value_t context, const jerry_value_t args[], const jerry_length_t argsSize)
{
    MapComponent *component = GetMapComponent(context);
    if (component == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapComponent StartNavi component is nullptr");
        return UNDEFINED;
    }

    if (component->routeOverlayId_ > 0) {
        component->mapClient_->MapRemoveOverlay(component->map_id_, component->routeOverlayId_);
        component->routeOverlayId_ = -1;
    }

    if (component->naviOverlayId_ > 0 && component->trackNaviOverlay_) {
        component->mapClient_->MapTrackNaviRemoveOverlay(component->map_id_, component->trackNaviOverlay_);
        component->naviOverlayId_ = -1;
        if (component->trackNaviOverlay_ != nullptr) {
            component->ClearTrackNaviOverlay(component->trackNaviOverlay_);
            component->trackNaviOverlay_ = nullptr;
        }
    }

    int16_t id = -1;
    id = JerryGetIntegerProperty(args[0], ROUTEID);
    if (id < 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapComponent StartNavi JerryGetIntegerProperty failed");
        return UNDEFINED;
    }

    jerry_value_t obj = args[0];

    if (!jerry_value_is_object(obj)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "StartNavi: parameter is not an object");
        return UNDEFINED;
    }

    jerry_value_t callbackParam = jerryx_get_property_str(obj, "callback");
    if (!jerry_value_is_function(callbackParam)) {
        jerry_release_value(callbackParam);
        HILOG_ERROR(HILOG_MODULE_ACE, "StartNavi: parameter get failed");
        return UNDEFINED;
    }

    if (!jerry_value_is_undefined(component->naviRouteCallback_)) {
        jerry_release_value(component->naviRouteCallback_);
    }

    component->naviRouteCallback_ = jerry_acquire_value(callbackParam);

    jerry_release_value(callbackParam);

    return component->HandleStartNavi(id);
}

JSValue MapComponent::HandleStartNavi(int16_t id)
{
    size_t count = routeResults_.size();
    if (count == 0 || count < id) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapComponent HandleStartNavi id error");
        return UNDEFINED;
    }

    AceMapRouteInfo route = routeResults_[id];

    BuildTrackInfo(&route);

    trackNaviOverlay_ = (map_track_navi_overlay *)ace_calloc(1, sizeof(map_track_navi_overlay));

    if (mapClient_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapComponent HandleStartNavi mapClient_ nullptr");
        ace_free(trackNaviOverlay_);
        trackNaviOverlay_ = nullptr;
        return UNDEFINED;
    }
    int32_t ret = mapClient_->MapInitTrackNaviOverlay(trackNaviOverlay_);
    if (ret != MAP_API_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapComponent HandleStartNavi MapInitTrackNaviOverlay failed");
        ace_free(trackNaviOverlay_);
        trackNaviOverlay_ = nullptr;
        return UNDEFINED;
    }

    trackNaviOverlay_->base_overlay.map_id = map_id_;
    trackNaviOverlay_->base_overlay.geometry_type = MAP_OVERLAY_TYPE_TRACK_NAVI;
    trackNaviOverlay_->base_overlay.visible = true;
    trackNaviOverlay_->track_info = &trackInfo_;

    trackNaviOverlay_->navi_options.point_correcttion_threshold = NAVI_POINT_CORRECTTION_THRESHOLD;
    trackNaviOverlay_->navi_options.navi_event_distance_threshold = NAVI_EVENT_DISTANCE_THRESHOLD;
    trackNaviOverlay_->navi_options.navi_calc_sampling = NAVI_CALC_SAMPLING;
    trackNaviOverlay_->navi_options.turn_point_around_threshold = const_cast<double *>(ACEMAP_TURN_THRESHOLDS);
    trackNaviOverlay_->navi_options.turn_point_around_threshold_size = NAVI_TURN_POINT_AROUND_THRESHOLD_SIZE;

    trackNaviOverlay_->plan_polyline_options.plan_polyline_marker.line_width = PLAN_LINE_WIDTH;
    trackNaviOverlay_->plan_polyline_options.plan_polyline_marker.line_color = PLAN_LINE_COLOR;
    trackNaviOverlay_->plan_polyline_options.plan_polyline_marker.border_width = PLAN_BORDER_WIDTH;
    trackNaviOverlay_->plan_polyline_options.plan_polyline_marker.border_color = PLAN_BORDER_COLOR;

    trackNaviOverlay_->real_polyline_options.real_polyline_marker.line_width = REAL_LINE_WIDTH;
    trackNaviOverlay_->real_polyline_options.real_polyline_marker.line_color = REAL_LINE_COLOR;
    trackNaviOverlay_->real_polyline_options.real_polyline_marker.border_width = REAL_BORDER_WIDTH;
    trackNaviOverlay_->real_polyline_options.real_polyline_marker.border_color = REAL_BORDER_COLOR;

    trackNaviOverlay_->listener.on_navi_event = OnNaviEventCallback;
    trackNaviOverlay_->listener.on_track_pass_point = OnTrackPassPointCallback;
    trackNaviOverlay_->listener.on_trajector_yaw = OnTrajectoryYawCallback;
    trackNaviOverlay_->listener.on_trajector_yaw_end = OnTrajectoryYawEndCallback;

    ret = mapClient_->MapTrackNaviAddOverlay(map_id_, trackNaviOverlay_);
    if (ret != MAP_API_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapComponent HandleStartNavi MapTrackNaviAddOverlay failed");
        ace_free(trackNaviOverlay_);
        trackNaviOverlay_ = nullptr;
        return UNDEFINED;
    }

    naviOverlayId_ = trackNaviOverlay_->base_overlay.guid;

    ret = mapClient_->MapSetCenter(map_id_, route.polylinePoints[0]);
    if (ret != MAP_API_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapComponent HandleStartNavi MapSetCenter failed");
        ace_free(trackNaviOverlay_);
        trackNaviOverlay_ = nullptr;
        return UNDEFINED;
    }

    ret = mapClient_->MapDoRender();
    if (ret != MAP_API_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapComponent HandleStartNavi MapDoRender failed");
        ace_free(trackNaviOverlay_);
        trackNaviOverlay_ = nullptr;
        return UNDEFINED;
    }

    jerry_value_t routesArray = jerry_create_array(route.polylinePoints.size());
    for (size_t i = 0; i < route.polylinePoints.size(); i++) {
        jerry_value_t routeObj = jerry_create_object();
        JerrySetStringProperty(routeObj, "lon", std::to_string(route.polylinePoints[i].lon).c_str());
        JerrySetStringProperty(routeObj, "lat", std::to_string(route.polylinePoints[i].lat).c_str());
        jerry_set_property_by_index(routesArray, i, routeObj);
        jerry_release_value(routeObj);
    }
    jerry_value_t thisVal = jerry_create_undefined();
    jerry_value_t retValue = jerry_call_function(naviRouteCallback_, thisVal, &routesArray, 1);
    if (jerry_value_is_error(retValue)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapComponent Callback fail");
    }

    jerry_release_value(thisVal);
    jerry_release_value(routesArray);
    jerry_release_value(retValue);

    return UNDEFINED;
}

void MapComponent::BuildTrackInfo(const AceMapRouteInfo *route)
{
    trackInfo_.points = const_cast<map_coord2d *>(route->polylinePoints.data());
    trackInfo_.point_size = route->polylinePoints.size();
    BuildTurnPoints(route->steps);
    trackInfo_.turn_points = turnPoints_;
    trackInfo_.turn_point_size = turnPointSize_;
    trackInfo_.via_point_indexes = nullptr;
    trackInfo_.via_point_index_size = 0;
}

void MapComponent::BuildTurnPoints(const std::vector<AceMapRouteStep> &steps)
{
    if (turnPoints_) {
        for (size_t i = 0; i < turnPointSize_; i++) {
            if (turnPoints_[i].turn_message) {
                ace_free(turnPoints_[i].turn_message);
                turnPoints_[i].turn_message = nullptr;
            }
            if (turnPoints_[i].turn_location_message) {
                ace_free(turnPoints_[i].turn_location_message);
                turnPoints_[i].turn_location_message = nullptr;
            }
        }
        ace_free(turnPoints_);
        turnPoints_ = nullptr;
    }
    turnPointSize_ = steps.size();
    turnPoints_ = (map_track_turn_point_info *)ace_calloc(turnPointSize_, sizeof(map_track_turn_point_info));
    if (!turnPoints_) {
        HILOG_ERROR(HILOG_MODULE_ACE, "BuildTurnPoints turnPoints_ calloc fail");
        turnPointSize_ = 0;
        return;
    }
    for (size_t i = 0; i < steps.size(); i++) {
        turnPoints_[i].index = steps[i].polylineStartIndex;
        turnPoints_[i].bear = 0.0f;
        turnPoints_[i].turn_message = const_cast<char *>(steps[i].instruction.c_str());
        turnPoints_[i].turn_location_message = const_cast<char *>(steps[i].road.c_str());
    }
}

bool MapComponent::IsValidNumber(const char *number)
{
    if (!number || *number == '\0') {
        return false;
    }
    char *endptr = nullptr;
    strtod(number, &endptr);

    while (endptr && isspace((unsigned char)*endptr)) {
        ++endptr;
    }
    return (endptr && *endptr == '\0');
}

bool MapComponent::IsValidLonLat(double lon, double lat)
{
    return (lon >= MIN_LON && lon <= MAX_LON) && (lat >= MIN_LAT && lat <= MAX_LAT);
}

JSValue MapComponent::UpdateNavi(
    const jerry_value_t func, const jerry_value_t context, const jerry_value_t args[], const jerry_length_t argsSize)
{
    MapComponent *component = GetMapComponent(context);
    if (component == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapComponent UpdateNavi component is nullptr");
        return UNDEFINED;
    }

    jerry_value_t obj = args[0];

    if (!jerry_value_is_object(obj)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "UpdateNavi: parameter is not an object");
        return UNDEFINED;
    }

    jerry_value_t lonParam = jerryx_get_property_str(obj, "lon");
    jerry_value_t latParam = jerryx_get_property_str(obj, "lat");
    if (!jerry_value_is_string(lonParam) || !jerry_value_is_string(latParam)) {
        jerry_release_value(lonParam);
        jerry_release_value(latParam);
        HILOG_ERROR(HILOG_MODULE_ACE, "UpdateNavi: parameter get failed");
        return UNDEFINED;
    }

    char *lon = MallocStringOf(lonParam);
    char *lat = MallocStringOf(latParam);
    jerry_release_value(lonParam);
    jerry_release_value(latParam);

    if (!IsValidNumber(lon) || !IsValidNumber(lat)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "UpdateNavi: invalid numeric string");
        ace_free(lon);
        ace_free(lat);
        return UNDEFINED;
    }

    map_coord2d coord = {};
    if (!component->ParseCoord(lon, &coord.lon)) {
        coord.lon = 0.0;
    }
    if (!component->ParseCoord(lat, &coord.lat)) {
        coord.lat = 0.0;
    }

    if (!IsValidLonLat(coord.lon, coord.lat)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "UpdateNavi: lon/lat out of range: %f, %f", coord.lon, coord.lat);
        ace_free(lon);
        ace_free(lat);
        return UNDEFINED;
    }

    if (component->mapClient_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "UpdateNavi: mapClient_ is nullptr");
        ace_free(lon);
        ace_free(lat);
        return UNDEFINED;
    }

    component->mapClient_->MapTrackNaviUpdateRealPoint(component->map_id_, coord, 0.0f, component->trackNaviOverlay_);

    int32_t ret = component->mapClient_->MapSetCenter(component->map_id_, coord);
    if (ret != MAP_API_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_ACE, "UpdateNavi  MapSetCenter failed");
        ace_free(lon);
        ace_free(lat);
        return UNDEFINED;
    }

    ret = component->mapClient_->MapDoRender();
    if (ret != MAP_API_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_ACE, "UpdateNavi  MapDoRender failed");
        ace_free(lon);
        ace_free(lat);
        return UNDEFINED;
    }

    ace_free(lon);
    ace_free(lat);
    return UNDEFINED;
}

JSValue MapComponent::TouchEnable(
    const jerry_value_t func, const jerry_value_t context, const jerry_value_t args[], const jerry_length_t argsSize)
{
    if (argsSize != 1) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapComponent TouchEnable parameter error");
        return UNDEFINED;
    }

    MapComponent *component = GetMapComponent(context);
    if (component == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapComponent TouchEnable component is nullptr");
        return UNDEFINED;
    }

    bool touch = false;
    if (!JerryGetBoolProperty(args[0], ENABLE, touch)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapComponent TouchEnable JerryGetBoolProperty failed");
        return UNDEFINED;
    }
    return component->HandleTouchEnable(touch);
}

JSValue MapComponent::SelectPreviewRoute(
    const jerry_value_t func, const jerry_value_t context, const jerry_value_t args[], const jerry_length_t argsSize)
{
    if (argsSize != 1) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapComponent SelectPreviewRoute parameter error");
        return UNDEFINED;
    }

    MapComponent *component = GetMapComponent(context);
    if (component == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapComponent SelectPreviewRoute component is nullptr");
        return UNDEFINED;
    }

    int16_t id = -1;
    id = JerryGetIntegerProperty(args[0], ROUTEID);
    if (id < 0 || id >= component->routeResults_.size()) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapComponent SelectPreviewRoute JerryGetBoolProperty failed");
        return UNDEFINED;
    }

    return component->HandleSelectPreviewRoute(id);
}

JSValue MapComponent::HandleSelectPreviewRoute(int16_t id)
{
    if (routeOverlayId_ >= 0 && map_id_ >= 0 && mapClient_ != nullptr) {
        int32_t ret = mapClient_->MapRemoveOverlay(map_id_, routeOverlayId_);
        if (ret != MAP_API_SUCCESS) {
            HILOG_ERROR(HILOG_MODULE_ACE, "MapComponent HandleSelectPreviewRoute failed");
        }
        routeOverlayId_ = -1;
    }

    AceMapPolylineStyle normalStyle;

    UpdateRouteView(this->routeResults_[id].polylinePoints, normalStyle);

    return UNDEFINED;
}

JSValue MapComponent::HandlePreviewRoute(std::map<std::string, std::string> &args)
{
    mapClient_->GetBikePath(args, [this](const char *json) {
        if (json == nullptr) {
            HILOG_ERROR(HILOG_MODULE_ACE, "MapComponent HandlePreviewRoute json nullptr");
            return;
        }

        this->routeResults_ = this->ParseRouteJson(json);

        AceMapPolylineStyle normalStyle;

        if (this->routeResults_.size() <= 0) {
            HILOG_ERROR(HILOG_MODULE_ACE, "MapComponent HandlePreviewRoute Invalid route");
            return;
        }

        this->UpdateRouteView(this->routeResults_[0].polylinePoints, normalStyle);

        jerry_value_t routesArray = jerry_create_array(this->routeResults_.size());
        for (size_t i = 0; i < this->routeResults_.size(); i++) {
            jerry_value_t routeObj = jerry_create_object();
            JerrySetStringProperty(routeObj, "des", this->routeResults_[i].description.c_str());
            JerrySetNumberProperty(routeObj, "id", static_cast<double>(this->routeResults_[i].id));
            jerry_set_property_by_index(routesArray, i, routeObj);
            jerry_release_value(routeObj);
        }
        jerry_value_t thisVal = jerry_create_undefined();
        jerry_value_t ret = jerry_call_function(this->previewRouteCallback_, thisVal, &routesArray, 1);
        if (jerry_value_is_error(ret)) {
            HILOG_ERROR(HILOG_MODULE_ACE, "MapComponent Callback fail");
        }

        jerry_release_value(thisVal);
        jerry_release_value(routesArray);
        jerry_release_value(ret);
    });
    return UNDEFINED;
}

JSValue MapComponent::PreviewRoute(
    const jerry_value_t func, const jerry_value_t context, const jerry_value_t args[], const jerry_length_t argsSize)
{
    if (argsSize != 1) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapComponent PreviewRoute parameter error");
        return UNDEFINED;
    }

    MapComponent *component = GetMapComponent(context);
    if (component == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapComponent PreviewRoute component is nullptr");
        return UNDEFINED;
    }

    jerry_value_t obj = args[0];

    if (!jerry_value_is_object(obj)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "PreviewRoute: parameter is not an object");
        return UNDEFINED;
    }

    jerry_value_t startParam = jerryx_get_property_str(obj, "start");
    jerry_value_t endParam = jerryx_get_property_str(obj, "end");
    jerry_value_t callbackParam = jerryx_get_property_str(obj, "callback");
    if (!jerry_value_is_string(startParam) || !jerry_value_is_string(endParam) ||
        !jerry_value_is_function(callbackParam)) {
        jerry_release_value(startParam);
        jerry_release_value(endParam);
        jerry_release_value(callbackParam);
        HILOG_ERROR(HILOG_MODULE_ACE, "PreviewRoute: parameter get failed");
        return UNDEFINED;
    }

    if (!jerry_value_is_undefined(component->previewRouteCallback_)) {
        jerry_release_value(component->previewRouteCallback_);
    }
    component->previewRouteCallback_ = jerry_acquire_value(callbackParam);

    char *start = MallocStringOf(startParam);
    char *end = MallocStringOf(endParam);

    if (start == nullptr || end == nullptr) {
        jerry_release_value(startParam);
        jerry_release_value(endParam);
        jerry_release_value(callbackParam);
        return UNDEFINED;
    }

    std::map<std::string, std::string> params;
    params["key"] = API_KEY;
    params["origin"] = std::string(start);
    params["destination"] = std::string(end);
    params["show_fields"] = "polyline,cost";
    params["alternative_route"] = "3";

    jerry_release_value(startParam);
    jerry_release_value(endParam);
    jerry_release_value(callbackParam);
    ace_free(start);
    ace_free(end);

    return component->HandlePreviewRoute(params);
}

void MapComponent::UpdateRouteView(const std::vector<map_coord2d> &routePoints, const AceMapPolylineStyle &style)
{
    if (routePoints.empty()) {
        HILOG_ERROR(HILOG_MODULE_ACE, "UpdateRouteView: routePoints is empty");
        return;
    }
    map_polyline_overlay overlay{};
    int32_t ret = mapClient_->MapInitLineOverlay(&overlay);
    if (ret != MAP_API_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapInitLineOverlay failed: %d", ret);
        return;
    }

    size_t count = routePoints.size();
    map_coord2d *points = (map_coord2d *)ace_malloc(sizeof(map_coord2d) * count);
    if (points == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "UpdateRouteView: malloc failed");
        ace_free(points);
        return;
    }

    for (size_t i = 0; i < count; i++) {
        points[i].lat = routePoints[i].lat;
        points[i].lon = routePoints[i].lon;
    }

    overlay.points = points;
    overlay.point_size = count;
    overlay.pre_index = 0;
    overlay.show_arrow = true;

    PolylineApplyStyle(overlay.normal_marker, style);
    PolylineApplyStyle(overlay.focus_marker, style);
    ret = mapClient_->MapAddPolylineOverlay(map_id_, &overlay);
    if (ret != MAP_API_SUCCESS) {
        ace_free(points);
        HILOG_ERROR(HILOG_MODULE_ACE, "MapAddPolylineOverlay failed: %d", ret);
        return;
    }

    ret = mapClient_->MapSetCenter(map_id_, routePoints[0]);
    if (ret != MAP_API_SUCCESS) {
        ace_free(points);
        HILOG_ERROR(HILOG_MODULE_ACE, "MapSetCenter failed: %d", ret);
        return;
    }

    ret = mapClient_->MapDoRender();
    if (ret != MAP_API_SUCCESS) {
        ace_free(points);
        HILOG_ERROR(HILOG_MODULE_ACE, "MapDoRender failed: %d", ret);
        return;
    }

    routeOverlayId_ = overlay.base_overlay.guid;

    ace_free(points);
}

bool MapComponent::MapDragListener::OnDragStart(UIView &view, const DragEvent &event)
{
    UNUSED(view);
    if (!mapComponent_->touchEnable_) {
        return true;
    }
    Point startPoint = event.GetStartPoint();
    mapComponent_->lastRenderTick_ = 0;
    int32_t ret = mapComponent_->mapClient_->MapTouchBegin(mapComponent_->map_id_, startPoint.x, startPoint.y);
    if (ret != MAP_API_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapComponent OnDragStart Failed");
        return false;
    }
    return true;
}

bool MapComponent::MapDragListener::OnDrag(UIView &view, const DragEvent &event)
{
    UNUSED(view);
    if (!mapComponent_->touchEnable_) {
        return true;
    }
    UINT64 currentTick = LOS_TickCountGet();
    UINT32 interval = LOS_MS2Tick(mapComponent_->RENDER_INTERVAL_MS);
    if ((currentTick - mapComponent_->lastRenderTick_) < interval) {
        return true;  // 限制帧率
    }
    Point curPoint = event.GetCurrentPos();
    int32_t ret = mapComponent_->mapClient_->MapTouchUpdate(mapComponent_->map_id_, curPoint.x, curPoint.y);
    if (ret != MAP_API_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapComponent OnDrag Failed");
        return false;
    }
    mapComponent_->mapClient_->MapDoRender();
    mapComponent_->lastRenderTick_ = currentTick;
    return true;
}

bool MapComponent::MapDragListener::OnDragEnd(UIView &view, const DragEvent &event)
{
    UNUSED(view);
    if (!mapComponent_->touchEnable_) {
        return true;
    }
    Point endPoint = event.GetCurrentPos();
    int32_t ret = mapComponent_->mapClient_->MapTouchEnd(mapComponent_->map_id_, endPoint.x, endPoint.y);
    mapComponent_->lastRenderTick_ = 0;

    if (ret != MAP_API_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapComponent OnDragEnd Failed");
        return false;
    }
    return true;
}

std::vector<AceMapRouteInfo> MapComponent::ParseRouteJson(const char *json)
{
    std::vector<AceMapRouteInfo> routes;
    if (json == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "ParseRouteJson: json is null");
        return routes;
    }

    cJSON *root = cJSON_Parse(json);
    if (root == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "ParseRouteJson: cJSON_Parse failed");
        return routes;
    }

    // 检查 status
    const char *status = cJSON_GetStringValue(cJSON_GetObjectItem(root, "status"));
    if (status == nullptr || strcmp(status, "1") != 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "ParseRouteJson: status=%s", status ? status : "null");
        cJSON_Delete(root);
        return routes;
    }

    // route.paths
    cJSON *route = cJSON_GetObjectItem(root, "route");
    cJSON *paths = route ? cJSON_GetObjectItem(route, "paths") : nullptr;
    if (paths == nullptr || !cJSON_IsArray(paths)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "ParseRouteJson: paths not found");
        cJSON_Delete(root);
        return routes;
    }

    int pathCount = cJSON_GetArraySize(paths);
    for (int p = 0; p < pathCount; p++) {
        cJSON *path = cJSON_GetArrayItem(paths, p);
        if (path != nullptr) {
            routes.push_back(ParseSinglePath(path, p));
        } else {
            HILOG_ERROR(HILOG_MODULE_ACE, "ParseRouteJson:path is nullptr");
        }
    }

    cJSON_Delete(root);
    return routes;
}

AceMapRouteInfo MapComponent::ParseSinglePath(cJSON *path, int index)
{
    AceMapRouteInfo info{};
    const char *distStr = cJSON_GetStringValue(cJSON_GetObjectItem(path, "distance"));
    info.distance = SafeAtoi(distStr, 0);

    const char *durStr = cJSON_GetStringValue(cJSON_GetObjectItem(path, "duration"));
    info.duration = SafeAtoi(durStr, 0);

    ParseSteps(path, info);

    float km = info.distance / 1000.0f;
    int min = info.duration / 60;
    char desc[128];

    int ret = snprintf_s(desc, sizeof(desc), sizeof(desc) - 1, "路线%d: %.1fkm, 约%d分钟", index + 1, km, min);
    if (ret < 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "ParseRouteJson:ParseSinglePath snprintf_s error");
        desc[0] = '\0';
    }
    info.description = desc;
    info.id = index;

    return info;
}

void MapComponent::ParseSteps(cJSON *path, AceMapRouteInfo &info)
{
    cJSON *stepsArr = cJSON_GetObjectItem(path, "steps");
    if (stepsArr == nullptr || !cJSON_IsArray(stepsArr)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "ParseSteps: error ");
        return;
    }
    int stepCount = cJSON_GetArraySize(stepsArr);
    for (int s = 0; s < stepCount; s++) {
        cJSON *item = cJSON_GetArrayItem(stepsArr, s);
        if (item == nullptr) {
            continue;
        }
        info.steps.push_back(ParseSingleStep(item, info.polylinePoints, s));
    }
}

AceMapRouteStep MapComponent::ParseSingleStep(cJSON *item, std::vector<map_coord2d> &polylinePoints, int stepIndex)
{
    AceMapRouteStep step{};
    const char *inst = cJSON_GetStringValue(cJSON_GetObjectItem(item, "instruction"));
    const char *ori = cJSON_GetStringValue(cJSON_GetObjectItem(item, "orientation"));
    const char *road = cJSON_GetStringValue(cJSON_GetObjectItem(item, "road_name"));
    cJSON *cost = cJSON_GetObjectItem(item, "cost");
    const char *duration = cJSON_GetStringValue(cJSON_GetObjectItem(cost, "duration"));
    cJSON *distItem = cJSON_GetObjectItem(item, "step_distance");

    step.instruction = inst ? inst : "";
    step.orientation = ori ? ori : "";
    step.road = road ? road : "";
    step.distance = (distItem && cJSON_IsNumber(distItem)) ? distItem->valueint : 0;
    step.duration = SafeAtoi(duration, 0);

    const char *polyline = cJSON_GetStringValue(cJSON_GetObjectItem(item, "polyline"));
    if (polyline != nullptr) {
        step.polylineStartIndex = static_cast<int32_t>(polylinePoints.size());
        ParsePolylineString(polyline, polylinePoints);
    } else {
        HILOG_ERROR(HILOG_MODULE_ACE, "ParseSteps: polyline is nullptr ");
    }
    return step;
}

void MapComponent::ParsePolylineString(const char *str, std::vector<map_coord2d> &outPoints)
{
    if (str == nullptr) {
        return;
    }

    size_t len = strlen(str);
    char *buf = (char *)ace_malloc(len + 1);
    if (buf == nullptr) {
        return;
    }
    errno_t ret_er = memcpy_s(buf, len + 1, str, len + 1);
    if (ret_er != EOK) {
        return;
    }

    char *savePtr = nullptr;
    char *token = strtok_r(buf, ";", &savePtr);
    while (token != nullptr) {
        double lon = 0.0;
        double lat = 0.0;
        if (sscanf_s(token, "%lf,%lf", &lon, &lat) == LON_LAT_SIZE) {
            map_coord2d coord;
            coord.lon = lon;
            coord.lat = lat;
            bool dup = false;
            if (!outPoints.empty()) {
                const auto &last = outPoints.back();
                if (last.lon == coord.lon && last.lat == coord.lat) {
                    dup = true;
                }
            }
            if (!dup) {
                outPoints.push_back(coord);
            }
        }
        token = strtok_r(nullptr, ";", &savePtr);
    }

    ace_free(buf);
}

void MapComponent::PolylineApplyStyle(map_line_overlay_marker &marker, const AceMapPolylineStyle &style)
{
    marker.line_width = style.lineWidth;
    marker.line_color = style.lineColor;
    marker.border_width = style.borderWidth;
    marker.border_color = style.borderColor;
    marker.use_dash = style.useDash;
    marker.dash_painted_length = style.dashPainted;
    marker.dash_unpainted_length = style.dashUnpainted;
    marker.dash_offset = style.dashOffset;
}

bool MapComponent::ParseCoord(const char *str, double *out)
{
    if (!str || !out) {
        HILOG_ERROR(HILOG_MODULE_ACE, "ParseCoord: args is nullptr ");
        return false;
    }
    char *endptr;
    double val = strtod(str, &endptr);
    if (endptr == str || *endptr != '\0') {
        HILOG_ERROR(HILOG_MODULE_ACE, "ParseCoord: parse fail ");
        return false;
    }
    *out = val;
    return true;
}

int MapComponent::SafeAtoi(const char *str, int default_value)
{
    if (!str || !*str) {
        HILOG_ERROR(HILOG_MODULE_ACE, "SafeAtoi: parse fail ");
        return default_value;
    }
    char *endptr;
    long val = strtol(str, &endptr, 10);
    if (endptr == str || *endptr != '\0') {
        HILOG_ERROR(HILOG_MODULE_ACE, "SafeAtoi: val unconverted or partially converted ");
        return default_value;
    }

    if (val < INT_MIN || val > INT_MAX) {
        HILOG_ERROR(HILOG_MODULE_ACE, "SafeAtoi: val overflow ");
        return default_value;
    }
    return static_cast<int>(val);
}

void MapComponent::OnNaviEventCallback(map_track_navi_event_info *event)
{
    HILOG_INFO(HILOG_MODULE_ACE, "OnNaviEventCallback: info ");
}

void MapComponent::OnTrackPassPointCallback(int32_t index, map_coord2d point)
{
    HILOG_INFO(HILOG_MODULE_ACE, "OnTrackPassPointCallback: info ");
}

void MapComponent::OnTrajectoryYawCallback(double distance)
{
    HILOG_INFO(HILOG_MODULE_ACE, "OnTrajectoryYawCallback: info ");
}

void MapComponent::OnTrajectoryYawEndCallback()
{
    HILOG_INFO(HILOG_MODULE_ACE, "OnTrajectoryYawEndCallback: info ");
}

}  // namespace ACELite
}  // namespace OHOS
