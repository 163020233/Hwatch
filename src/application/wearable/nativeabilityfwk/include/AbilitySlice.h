/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: AbilitySlice
 * Create: 2025-11
 */

#ifndef NATIVEABILITYFWK_ABILITY_SLICE_H
#define NATIVEABILITYFWK_ABILITY_SLICE_H
#include <typeinfo>
#include "Slice.h"
#include "Presenter.h"
#include "View.h"
#include "SlicePageManager.h"

namespace OHOS {
enum class SliceState : uint8_t {
    UNINITIALIZED,
    START,
    RESUME,
    PAUSE,
    STOP
};
class AbilitySlice : public Slice {
public:
    AbilitySlice(ViewBase* viewBase, PresenterBase* presenterBase, uint32_t targetId)
        : sliceId_(targetId & SLICE_MASK), view_(viewBase), presenter_(presenterBase),
          slicePageMgr_(targetId), state_(SliceState::UNINITIALIZED) {}

    void OnStart(void* data) override;

    void OnResume() override;

    void OnPause(PauseReason reason) override;

    void OnStop() override;

    UIViewGroup* GetSliceContainer() override;

    bool ChangeSlicePage(uint16_t pageId, void* data, TransitionType type, bool canBack);

    bool BackToPrevSlicePage(TransitionType type = TransitionType::TRANSITION_INVALID);

    bool BackToCachedSlicePage(uint16_t pageId, TransitionType type = TransitionType::TRANSITION_INVALID);

    SlicePageManager* GetSlicePageManger()
    {
        return &slicePageMgr_;
    }

    bool IsCachedSlicePage(uint16_t pageId);

    void OnOrientationChanged(AppRotationType rot);

    bool IsBackDragging() const;

    void EnablePendingRotation();

    AppRotationType GetCurRotation() const;

    void StopSlicePageSwitchAnimator();

    bool IsPageBeingSwitched()
    {
        return slicePageMgr_.IsPageBeingSwitched();
    }

private:
    uint16_t sliceId_;
    ViewBase* view_;
    PresenterBase* presenter_;
    SlicePageManager slicePageMgr_;
    SliceState state_;
    bool hasPendingRotateOperation_ = false;
};
} // namespace OHOS

#endif // NATIVEABILITY_ABILITY_SLICE_H
