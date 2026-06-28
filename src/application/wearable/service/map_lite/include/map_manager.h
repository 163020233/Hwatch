
/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Create Date : 2025-11
 */

// 实现地图视图的管理,渲染控制
#ifndef MAP_MANAGER_H
#define MAP_MANAGER_H

#include <map>
#include <cstdint>
#include "map_controller_interface.h"
#include "map/map_render_interface.h"

namespace OHOS {

class MapManager {
public:
    /**
     * 获取单例
     */
    static MapManager *GetInstance();

    int32_t InitMapContext();

    int32_t DeInitMapContext();

    /**
     * @brief 绑定地图视图与UIKit View
     *
     * @param mapId 地图视图ID
     * @param mapView 渲染组件实例，要求由提供方控制生命周期，且生命周期需要比绑定周期长
     * @return int32_t 绑定成功或失败
     */
    int32_t BindMapToView(int32_t mapId, IMapRender *mapView);

    int32_t UnBindMapToView(int32_t mapId);

    /**
     * @brief 获取地图控制器对象，调用前请先初始化环境
     *
     * @return IMapController* 地图控制器指针
     */
    IMapController *GetMapController();

private:
    MapManager();

private:
    /**
     * 是否完成初始化
     */
    volatile bool isInitialized_;

    // 代理类
    IMapController *mapControllerProxy_;
    IMapController *realController_;
};
}  // namespace OHOS
#endif