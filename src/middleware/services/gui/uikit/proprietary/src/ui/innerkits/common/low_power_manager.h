/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#ifndef GRAPHIC_LOW_POWER_MANAGER
#define GRAPHIC_LOW_POWER_MANAGER
#include "graphic_config.h"
#include "components/ui_label.h"

namespace OHOS {

class LowPowerManager : public HeapBase {
public:
    /**
     * @brief A constructor used to create a <b>LowPowerManager</b> instance.
     *
     * @since 1.0
     * @version 1.0
     */
    LowPowerManager() {}
    /**
     * @brief A destructor used to delete the <b>LowPowerManager</b> instance.
     *
     * @since 1.0
     * @version 1.0
     */
    ~LowPowerManager() {}

    static LowPowerManager* GetInstance()
    {
        static LowPowerManager lowPowerManager;
        return &lowPowerManager;
    }

    /**
     * @brief Try to enter low power mode
     *
     */
    void TryToEnterLowPower();

    /**
     * @brief Enter low power mode
     *
     */
    void EnterLowPower();

    /**
     * @brief Exit low power mode, must be called on GUI thread.
     *
     */
    void ExitLowPowerInner();

    /**
     * @brief Exit low power mode
     *
     */
    void ExitLowPower();
    /**
     * @brief Check if LiteSurface is in rootView.
     *
     */
    bool IsLiteSurfaceInRootView();

    void SetMaxFrequence(uint32_t freq);

private:
    uint32_t lastExitTime_ = 0;
    uint32_t maxFrequence_ = 60;
};
} // OHOS
#endif // GRAPHIC_LOW_POWER_MANAGER