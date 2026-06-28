

/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: MapSimulateTask - Periodic timer for navigation simulation
 * Created: 2026-01-14
 */

#ifndef MAP_SIMULATE_TASK_H
#define MAP_SIMULATE_TASK_H

#include "task.h"

namespace OHOS {

using SimulateCallback = void (*)(void *context);

class MapSimulateTask : public Task {
public:
    static constexpr uint32_t SIMULATE_INTERVAL_MS = 100;

    MapSimulateTask(SimulateCallback cb, void *context) : Task(SIMULATE_INTERVAL_MS), callback_(cb), context_(context)
    {}
    ~MapSimulateTask() override = default;

    void Callback() override
    {
        if (callback_) {
            callback_(context_);
        }
    }

private:
    SimulateCallback callback_;
    void *context_;
};

}  // namespace OHOS
#endif