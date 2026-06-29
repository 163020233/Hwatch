/*
 * Copyright (c) @HiSilicon (Shanghai) Technologies Co., Ltd. 2023. All rights reserved.
 */

#include "common/low_power_manager.h"
#include "graphic_service_wrapper.h"
#include "animator/animator_manager.h"
#include "common/input_device_manager.h"
#include "common/task_manager.h"
#include "engines/gfx/lite_m_gfx_engine.h"
#include "components/root_view.h"
#include "hal_tick.h"
#include "gfx_utils/list.h"
#include "pm_veto.h"
#include "graphic_event_handler.h"
#include "graphic_service.h"

namespace OHOS {
static const uint32_t TRY_ENTER_LOW_POWER_INTERVAL = 100; // to avoid switch refresh rate frequently during roate
static const uint16_t ENTER_LOG_POWER_FREQ = 10;

void LowPowerManager::EnterLowPower()
{
    uapi_pm_remove_sleep_veto(PM_ID_UIKIT);
}

void LowPowerManager::TryToEnterLowPower()
{
    // this condition can be removed later, switching refresh rate frequently doesn't seen to have any effect
    if (HALTick::GetInstance().GetElapseTime(lastExitTime_) < TRY_ENTER_LOW_POWER_INTERVAL) {
        return;
    }

    if (AnimatorManager::GetInstance()->IsActive() || InputDeviceManager::GetInstance()->IsEventExist()) {
        return;
    }

    if (LiteMGfxEngine::GetInstance()->HasPendingFrame()) {
        return;
    }

    if (IsLiteSurfaceInRootView()) {
        return;
    }

    uint32_t period = TaskManager::GetInstance()->GetMinimumCustomerTaskPeriod();
    int tmp = (period == 0) ? 1 : 1000 / static_cast<int>(period); // 1000: ms
    int freq = (tmp > static_cast<int>(maxFrequence_)) ? static_cast<int>(maxFrequence_) : tmp;
    if (freq <= ENTER_LOG_POWER_FREQ) {
        uapi_pm_remove_sleep_veto(PM_ID_UIKIT);
    }
    SetFrequenceInner(freq);
}

void LowPowerManager::ExitLowPower()
{
    GraphicEventHandler::GetInstance()->PostGraphicEvent([this]() {
        this->ExitLowPowerInner();
    });
}
 
void LowPowerManager::ExitLowPowerInner()
{
    lastExitTime_ = HALTick::GetInstance().GetTime();
    // avoid exit low power mode when screen is off
    if (GraphicService::GetInstance()->IsScreenOn()) {
        uapi_pm_add_sleep_veto(PM_ID_UIKIT);
    }
    SetFrequenceInner(maxFrequence_);
}

bool LowPowerManager::IsLiteSurfaceInRootView()
{
    LiteMGfxEngine* gfxView = LiteMGfxEngine::GetInstance();
    if (gfxView->GetHardwareLayer() == nullptr) {
        return false;
    }

    const UIView* view = dynamic_cast<const UIView *>(gfxView->GetHardwareLayer());
    while (view->GetParent() != nullptr) {
        view = view->GetParent();
    }

    return (view == RootView::GetInstance());
}

void LowPowerManager::SetMaxFrequence(uint32_t freq)
{
    maxFrequence_ = freq;
    if (GetFrequence() > static_cast<int>(maxFrequence_)) {
        SetFrequenceInner(maxFrequence_);
    }
}
}  // namespace OHOS
