/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: NaviRouteRenderer - Navigation route overlay management
 * Created: 2026-01-14
 */
#ifndef NAVI_ROUTE_RENDERER_H
#define NAVI_ROUTE_RENDERER_H

#include <vector>
#include "MapPageContext.h"
#include "map_client.h"

namespace OHOS {

class NaviRouteRenderer {
public:
    explicit NaviRouteRenderer(MapPageContext &ctx);
    ~NaviRouteRenderer();

    // 禁止拷贝
    NaviRouteRenderer(const NaviRouteRenderer &) = delete;
    NaviRouteRenderer &operator=(const NaviRouteRenderer &) = delete;
    NaviRouteRenderer(NaviRouteRenderer &&) = delete;
    NaviRouteRenderer &operator=(NaviRouteRenderer &&) = delete;

    void SetListener(INaviStateListener *listener)
    {
        listener_ = listener;
    }

    // 绘制 / 清理
    void DrawNaviRoute();
    void ClearNaviRoute();

    // 更新实走点（真实 GPS 或模拟）
    void UpdatePosition(ACELite::map_coord2d coord, float bearing);

private:
    // overlay 构建
    bool AllocAndInitArrowOverlay();
    bool EnsureArrowTexture();
    void ConfigureArrowOverlay(const ACELite::map_coord2d &startPos);
    bool AddArrowOverlayToMap();
    bool CreateNaviArrowOverlay(const ACELite::map_coord2d &startPos);
    bool CreateTrackNaviOverlay();
    bool BuildTrackInfo(const RouteInfo *route);
    bool BuildTurnPoints(const std::vector<RouteStep> &steps);

    // overlay 配置
    void SetupNaviOptions(ACELite::map_track_navi_overlay *overlay);
    void SetupPolylineStyles(ACELite::map_track_navi_overlay *overlay);
    void SetupNaviCallbacks(ACELite::map_track_navi_overlay *overlay);

    // C 回调（通过 s_instance_ 转发给 listener_）
    static void OnNaviEventCallback(ACELite::map_track_navi_event_info *event);
    static void OnTrackPassPointCallback(int32_t index, ACELite::map_coord2d point);
    static void OnTrajectoryYawCallback(double distance);
    static void OnTrajectoryYawEndCallback();

    static NaviRouteRenderer *s_instance_;

    // 纠偏与事件参数
    static constexpr float CORRECTION_THRESHOLD_METERS = 30.0f;
    static constexpr float EVENT_DISTANCE_THRESHOLD_METERS = 50.0f;
    static constexpr int CALC_SAMPLING_WINDOW = 5;
    static constexpr size_t TURN_THRESHOLD_COUNT = 3;

    // 规划线样式
    static constexpr int32_t PLAN_LINE_WIDTH = 8;
    static constexpr int32_t PLAN_LINE_COLOR = 0xFF00C853;
    static constexpr int32_t PLAN_BORDER_WIDTH = 2;
    static constexpr int32_t PLAN_BORDER_COLOR = 0xFF009624;

    // 实走线样式
    static constexpr int32_t REAL_LINE_WIDTH = 8;
    static constexpr int32_t REAL_LINE_COLOR = 0xFF999999;
    static constexpr int32_t REAL_BORDER_WIDTH = 2;
    static constexpr int32_t REAL_BORDER_COLOR = 0xFF009624;

    static constexpr int32_t NAVI_ARROW_OVERLAY_PRIORITY = 100;

    MapPageContext &ctx_;
    INaviStateListener *listener_ = nullptr;

    // overlay 资源
    ACELite::map_track_info trackInfo_{};
    int32_t naviOverlayId_ = -1;
    int32_t naviArrowTextureId_ = -1;
    uint8_t *texDataBuffer_ = nullptr;
    ACELite::map_track_navi_overlay *trackNaviOverlay_ = nullptr;
    ACELite::map_point_overlay *naviArrowOverlay_ = nullptr;
    ACELite::map_track_turn_point_info *turnPoints_ = nullptr;
    size_t turnPointSize_ = 0;
};

}  // namespace OHOS
#endif  // NAVI_ROUTE_RENDERER_H
