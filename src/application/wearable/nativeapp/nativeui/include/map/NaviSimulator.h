/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: NaviSimulator - Navigation simulation controller
 * Created: 2026-01-14
 */

#ifndef NAVI_SIMULATOR_H
#define NAVI_SIMULATOR_H

#include <cstdint>
#include "MapPageContext.h"
#include "NaviRouteRenderer.h"
#include "MapSimulateTask.h"
#include "map_client.h"

namespace OHOS {

static constexpr float DEFAULT_SIMULATED_HEADING = 0.0f;  // 模拟导航默认朝向角度

class NaviSimulator {
public:
    NaviSimulator(MapPageContext &ctx, NaviRouteRenderer &renderer);
    ~NaviSimulator();

    NaviSimulator(const NaviSimulator &) = delete;
    NaviSimulator &operator=(const NaviSimulator &) = delete;

    // 初始化模拟起点
    void Reset(ACELite::map_coord2d startCoord);

    // 开始 / 停止
    void Start();
    void Stop();

    bool IsRunning() const { return simulating_; }
    bool IsFinished() const { return finished_; }

private:
    void SimulateNextStep();
    static void OnSimulateCallback(void *context);
    static float CalcHeading(const ACELite::map_coord2d &from, const ACELite::map_coord2d &to);
    MapPageContext &ctx_;
    NaviRouteRenderer &renderer_;

    MapSimulateTask *task_ = nullptr;
    ACELite::map_coord2d simulatedCoord_{};
    int32_t simulatedIndex_ = 0;
    bool simulating_ = false;
    bool finished_ = false;
    float lastHeading_ = DEFAULT_SIMULATED_HEADING;  // 缓存上一次航向，最后一个点复用
};

}  // namespace OHOS
#endif  // NAVI_SIMULATOR_H
