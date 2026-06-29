/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: MapPage - Coordinator with shared UIAMapView and state handlers
 * Created: 2026-01-14
 */
#ifndef MAP_PAGE_H
#define MAP_PAGE_H

#include <memory>
#include "components/ui_view_group.h"
#include "components/ui_amap_view.h"
#include "UiConfig.h"
#include "SlicePageFactory.h"
#include "MapPageContext.h"
#include "IMapStateHandler.h"
#include "MainStateHandler.h"
#include "PoiStateHandler.h"
#include "RouteStateHandler.h"
#include "NaviStateHandler.h"

namespace OHOS {

class MapPage : public SlicePage<MapPresenter>, public UIView::OnDragListener {
public:
    MapPage();
    MapPage(const MapPage &) = delete;
    MapPage &operator=(const MapPage &) = delete;
    ~MapPage() override;

    static MapPage *GetInstance();

    // SlicePage 生命周期
    void OnStart(void *data) override;

    // 拖拽统一转发
    bool OnDragStart(UIView &view, const DragEvent &event) override;
    bool OnDrag(UIView &view, const DragEvent &event) override;
    bool OnDragEnd(UIView &view, const DragEvent &event) override;

    // 状态切换（Context 和 Handler 调用）
    void SwitchState(MapState newState);

private:
    IMapStateHandler *GetHandler(MapState state);

    // 共享资源
    MapPageContext ctx_;
    std::unique_ptr<UIViewGroup> container_;
    std::unique_ptr<UIAMapView> uiMapView_;

    // 状态处理器
    std::unique_ptr<MainStateHandler> mainHandler_;
    std::unique_ptr<PoiStateHandler> poiHandler_;
    std::unique_ptr<RouteStateHandler> routeHandler_;
    std::unique_ptr<NaviStateHandler> naviHandler_;
    IMapStateHandler *activeHandler_ = nullptr;

    MapState currentState_ = MapState::STATE_MAIN;
};

}  // namespace OHOS
#endif  // MAP_PAGE_H
