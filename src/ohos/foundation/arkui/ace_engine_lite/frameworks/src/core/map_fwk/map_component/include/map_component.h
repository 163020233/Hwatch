/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026. All rights reserved.
 * Description: map component.
 * Author: CompanyName
 * Create: 2026-03-09
 */

#ifndef OHOS_ACELITE_MAP_COMPONENT_H
#define OHOS_ACELITE_MAP_COMPONENT_H

#include <string>
#include <vector>
#include <stdint.h>
#include <cstdlib>
#include "jsi.h"
#include "component.h"
#include "ui_amap_view.h"
#include "map_client.h"
#include "jerryscript-core.h"
#include "graphic_types.h"
#include "los_tick.h"
#include "map_defines.h"
#include "cJSON.h"
#include "acelite_config.h"

namespace OHOS {
namespace ACELite {

struct AceMapRoutePointRemain {
    int32_t remainingDistance;  // meters
    int32_t remainingDuration;  // seconds
};

// 路线步骤信息
struct AceMapRouteStep {
    int32_t distance;                // 步骤距离（米）
    int32_t duration;                // 步骤时间（秒）
    int32_t remainingDistance;       // 从该步骤起点到终点的剩余距离
    int32_t remainingDuration;       // 从该步骤起点到终点的剩余时间
    std::string instruction;         // 导航指示 "沿XX路向北骑行100米"
    std::string road;                // 道路名称
    std::string orientation;         // 方向 "北"、"东南" 等
    int32_t polylineStartIndex = 0;  // 该步骤在整体 polylinePoints 中的起始索引
};

// 单条路线信息
struct AceMapRouteInfo {
    int32_t id;
    int32_t distance;                             // 总距离（米）
    int32_t duration;                             // 总时间（秒）
    std::string description;                      // 描述 "路线1: 3.9km, 约22分钟"
    std::vector<AceMapRouteStep> steps;           // 导航步骤列表
    std::vector<map_coord2d> polylinePoints;      // 完整路线坐标点序列（GCJ02）
    std::vector<AceMapRoutePointRemain> remains;  // 与 polylinePoints 一一对应
};

// 线段样式
struct AceMapPolylineStyle {
    int32_t lineWidth = 6;
    int32_t lineColor = 0xFF4A90D9;
    int32_t borderWidth = 2;
    int32_t borderColor = 0xFF2A5FA0;
    bool useDash = false;
    int32_t dashPainted = 5;
    int32_t dashUnpainted = 8;
    int32_t dashOffset = 0;
};

class MapComponent final : public Component {
public:
    ACE_DISALLOW_COPY_AND_MOVE(MapComponent);
    MapComponent() = delete;
    MapComponent(jerry_value_t options, jerry_value_t children, AppStyleManager *styleManager);
    ~MapComponent() override;

    // 地图放大
    static JSValue ZoomIn(const jerry_value_t func, const jerry_value_t context, const jerry_value_t args[],
        const jerry_length_t argsSize);

    // 地图缩小
    static JSValue ZoomOut(const jerry_value_t func, const jerry_value_t context, const jerry_value_t args[],
        const jerry_length_t argsSize);

    // 拖拽地图
    static JSValue TouchEnable(const jerry_value_t func, const jerry_value_t context, const jerry_value_t args[],
        const jerry_length_t argsSize);

    // 预览3路线，js展示，选择开始导航
    static JSValue PreviewRoute(const jerry_value_t func, const jerry_value_t context, const jerry_value_t args[],
        const jerry_length_t argsSize);

    // 选中预览路线
    static JSValue SelectPreviewRoute(const jerry_value_t func, const jerry_value_t context, const jerry_value_t args[],
        const jerry_length_t argsSize);

    // 开始导航
    static JSValue StartNavi(const jerry_value_t func, const jerry_value_t context, const jerry_value_t args[],
        const jerry_length_t argsSize);

    // 更新导航
    static JSValue UpdateNavi(const jerry_value_t func, const jerry_value_t context, const jerry_value_t args[],
        const jerry_length_t argsSize);

    static MapComponent *GetMapComponent(jerry_value_t context);

protected:
    UIView *GetComponentRootView() const override;

private:
    class MapDragListener : public UIView::OnDragListener {
    public:
        explicit MapDragListener(MapComponent *mapComp) : mapComponent_(mapComp)
        {}
        bool OnDragStart(UIView &view, const DragEvent &event) override;
        bool OnDrag(UIView &view, const DragEvent &event) override;
        bool OnDragEnd(UIView &view, const DragEvent &event) override;

    private:
        MapComponent *mapComponent_;
    };

    void CreateViews();

    JSValue HandleZoomIn();
    JSValue HandleZoomOut();
    JSValue HandlePreviewRoute(std::map<std::string, std::string> &args);
    JSValue HandleTouchEnable(bool touch);
    JSValue HandleDestroy();
    JSValue HandleSelectPreviewRoute(int16_t id);
    JSValue HandleStartNavi(int16_t id);
    JSValue HandlePauseNavi(int16_t id);
    JSValue HandleResumeNavi(int16_t id);

    void ParsePolylineString(const char *str, std::vector<map_coord2d> &outPoints);
    AceMapRouteStep ParseSingleStep(cJSON *item, std::vector<map_coord2d> &polylinePoints, int stepIndex);
    void ParseSteps(cJSON *path, AceMapRouteInfo &info);
    AceMapRouteInfo ParseSinglePath(cJSON *path, int index);
    std::vector<AceMapRouteInfo> ParseRouteJson(const char *json);

    void UpdateRouteView(const std::vector<map_coord2d> &routePoints, const AceMapPolylineStyle &style);
    void PolylineApplyStyle(map_line_overlay_marker &marker, const AceMapPolylineStyle &style);

    void BuildTrackInfo(const AceMapRouteInfo *route);
    void BuildTurnPoints(const std::vector<AceMapRouteStep> &steps);
    bool ParseCoord(const char *str, double *out);
    int SafeAtoi(const char *str, int default_value);
    void ClearTrackNaviOverlay(map_track_navi_overlay *overlay);

    static void OnNaviEventCallback(map_track_navi_event_info *event);
    static void OnTrackPassPointCallback(int32_t index, map_coord2d point);
    static void OnTrajectoryYawCallback(double distance);
    static void OnTrajectoryYawEndCallback();

    static bool IsValidNumber(const char *number);
    static bool IsValidLonLat(double lon, double lat);

    static const char *const FUNC_ZOOM_IN;
    static const char *const FUNC_ZOOM_OUT;
    static const char *const TOUCH_ENABLE;
    static const char *const PREVIEW_ROUTE;
    static const char *const SELECT_ROUTE;
    static const char *const ROUTEID;
    static const char *const STARTNAVI;
    static const char *const UPDATENAVI;
    static const char *const ENABLE;
    static const char *const API_KEY;

    UIAMapView *mapView_ = nullptr;
    MapClient *mapClient_ = nullptr;
    MapDragListener *dragListener_ = nullptr;

    jerry_value_t previewRouteCallback_ = jerry_create_undefined();
    jerry_value_t naviRouteCallback_ = jerry_create_undefined();

    mutable bool isViewInit = false;
    int32_t map_id_ = -1;
    int32_t routeOverlayId_ = -1;
    double mapCenterLon = 116.4587049610359;
    double mapCenterLat = 39.919082448993144;
    float mapZoom_ = 0.0f;
    float STEP_MAP_ZOOM = 1.0f;
    float MAX_MAP_ZOOM = 15.0f;
    float MIN_MAP_ZOOM = 6.0f;
    uint32_t RENDER_INTERVAL_MS = 30;
    volatile uint32_t lastRenderTick_ = 0;
    bool touchEnable_ = true;
    float NAVI_POINT_CORRECTTION_THRESHOLD = 30.0f;
    int NAVI_CALC_SAMPLING = 5;
    float NAVI_EVENT_DISTANCE_THRESHOLD = 50.0f;
    size_t NAVI_TURN_POINT_AROUND_THRESHOLD_SIZE = 3;
    double ACEMAP_TURN_THRESHOLDS[3] = {100.0, 50.0, 20.0};
    int32_t NORMAL_LINE_WIDTH = 6;
    int32_t LON_LAT_SIZE = 2;

    std::vector<AceMapRouteInfo> routeResults_;

    map_track_info trackInfo_{};
    int32_t naviOverlayId_ = -1;
    map_track_navi_overlay *trackNaviOverlay_ = nullptr;
    map_track_turn_point_info *turnPoints_ = nullptr;
    size_t turnPointSize_ = 0;

    static constexpr int32_t PLAN_LINE_WIDTH = 8;
    static constexpr int32_t PLAN_LINE_COLOR = 0xFF00C853;
    static constexpr int32_t PLAN_BORDER_WIDTH = 2;
    static constexpr int32_t PLAN_BORDER_COLOR = 0xFF009624;

    static constexpr int32_t REAL_LINE_WIDTH = 8;
    static constexpr int32_t REAL_LINE_COLOR = 0xFF999999;
    static constexpr int32_t REAL_BORDER_WIDTH = 2;
    static constexpr int32_t REAL_BORDER_COLOR = 0xFF009624;

    static constexpr double MAX_LON = 180.0;
    static constexpr double MIN_LON = -180.0;
    static constexpr double MAX_LAT = 90;
    static constexpr double MIN_LAT = -90;
};
}  // namespace ACELite
}  // namespace OHOS
#endif  // OHOS_ACELITE_MAP_COMPONENT_H
