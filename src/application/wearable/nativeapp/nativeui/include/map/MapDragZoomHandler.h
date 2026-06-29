/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: MapDragZoomHandler
 * Created: 2026-01-14
 */

#ifndef MAP_DRAG_HANDLER_H
#define MAP_DRAG_HANDLER_H

#include "events/drag_event.h"
#include "map_controller_interface.h"
#include "MapUtils.h"

namespace OHOS {

class MapDragZoomHandler {
public:
    MapDragZoomHandler(int32_t mapId);
    ~MapDragZoomHandler() = default;
    // 拖拽处理
    bool HandleDragStart(const DragEvent &event);
    bool HandleDrag(const DragEvent &event);
    bool HandleDragEnd(const DragEvent &event);

    // 缩放控制
    void SetMapZoom(float zoomLevel);

    float GetCurrentZoom() const { return currentZoom_; }

private:
    static constexpr uint32_t RENDER_INTERVAL_MS = 30;
    int32_t mapId_;
    volatile uint32_t lastRenderTick_ = 0;
    float currentZoom_ = MapUtils::DEFAULT_MAP_ZOOM;
};

}  // namespace OHOS

#endif  // MAP_DRAG_HANDLER_H
