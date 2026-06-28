/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: MapPageContext - Shared map resources for state handlers
 * Created: 2026-01-14
 */
#ifndef MAP_PAGE_CONTEXT_H
#define MAP_PAGE_CONTEXT_H

#include <memory>
#include <functional>
#include "components/ui_label_button.h"
#include "components/ui_view_group.h"
#include "components/ui_amap_view.h"
#include "mal_map_defines.h"
#include "map_manager.h"
#include "map_controller_interface.h"
#include "MapPresenter.h"
#include "MapDragZoomHandler.h"
#include "MapUtils.h"

namespace OHOS {

enum class MapState {
    STATE_MAIN,
    STATE_POI,
    STATE_ROUTE,
    STATE_NAVIGATION,
};

/*
 * MapPageContext: 所有 StateHandler 共享的资源
 * MapPage 拥有此对象，StateHandler 持有引用
 */
class MapPageContext {
public:
    // SDK 支持 zoom 范围 3~19，当前离线瓦片资源仅覆盖 6~16
    static constexpr uint16_t MAP_W = 466;
    static constexpr uint16_t MAP_H = 466;

    static constexpr double MAP_CENTER_LAT = 31.1248;
    static constexpr double MAP_CENTER_LON = 121.3646;
    // static constexpr double MAP_CENTER_LAT = 39.919082448993144;
    // static constexpr double MAP_CENTER_LON = 116.4587049610359;
    static constexpr float ZOOM_STEP = 1.0f;
    static constexpr float ZOOM_MIN = 6.0f;
    static constexpr float ZOOM_MAX = 16.0f;

    MapPageContext() = default;
    ~MapPageContext() = default;

    // === 初始化 / 销毁 ===
    bool InitSDK();
    bool CreateMapView(uint16_t w, uint16_t h);
    void DestroyMapView();

    // === 地图操作 ===
    void InitDragZoomHandler();
    void RestoreMapState();
    void ZoomIn();
    void ZoomOut();
    bool HandleDragStart(const DragEvent &event);
    bool HandleDrag(const DragEvent &event);
    bool HandleDragEnd(const DragEvent &event);

    // === 公共点击处理（缩放按钮，各 handler 共用） ===
    bool HandleCommonClick(const char *viewId);

    // === 状态切换回调（由 MapPage 设置，handler 调用） ===
    std::function<void(MapState)> onStateSwitchRequested;
    void RequestStateSwitch(MapState newState);

    // === 共享资源（handler 直接访问） ===
    UIViewGroup *container_ = nullptr;
    UIAMapView *uiMapView_ = nullptr;
    MapManager *mapManager_ = nullptr;
    IMapController *mapController_ = nullptr;
    MapPresenter *presenter_ = nullptr;  // InitSDK() 中设置
    int32_t map_id_ = -1;
    std::unique_ptr<MapDragZoomHandler> dragZoomHandler_;
};

}  // namespace OHOS
#endif  // MAP_PAGE_CONTEXT_H
