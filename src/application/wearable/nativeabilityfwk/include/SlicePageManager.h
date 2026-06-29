/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SlicePageManager
 * Create: 2025-11
 */

#ifndef SLICE_PAGE_MANAGER_H
#define SLICE_PAGE_MANAGER_H
#include "animator/animator.h"
#include "Presenter.h"
#include "Slice.h"
#include "SlicePage.h"
#include "gfx_utils/vector.h"
#include "TransitionType.h"
#include "TransitionCallback.h"
#include "RotationManager.h"

namespace OHOS {
class SlicePageManager;
class SlicePageSwitchCallback : public AnimatorCallback {
public:
    SlicePageSwitchCallback(SlicePageManager* transMgr)
        : isStarted_(false), startTime_(0), pageTransMgr_(transMgr) {}
    void Callback(UIView* view) override;
    void OnStop(UIView& view) override;

private:
    bool isStarted_;
    uint32_t startTime_;
    SlicePageManager* pageTransMgr_;
};

class SlicePageManager : public HeapBase {
public:
    SlicePageManager(uint32_t targetId)
        : isPageBeingSwitched_(false), sliceId_(static_cast<uint16_t>(targetId & SLICE_MASK)),
          targetPageId_(static_cast<uint16_t>(targetId >> PAGE_OFFSET)),
          curPage_(nullptr), nextPage_(nullptr), pagesInBackQueue_(4), // 4, default queue size
          presenter_(nullptr), view_{nullptr}, transitionCallback_(nullptr), canBack_(false),
          isBackOperationRunning_(false) {}

    ~SlicePageManager();

    // Get current page which is display on the screen
    SlicePageBase* GetCurSlicePage()
    {
        return curPage_;
    }

    // Get specified page in the back queue
    SlicePageBase* GetSlicePageInBackQueue(uint16_t pageId);

    // Get previous page before current page in the back queue
    SlicePageBase* GetLastPageInBackQueue()
    {
        return pagesInBackQueue_.Back();
    }

    // Get size of pages in the back queue
    uint16_t GetPageSizeInBackQueue()
    {
        return pagesInBackQueue_.Size();
    }

    void ClearBackQueue();

    bool ChangeSlicePage(uint16_t pageId, void* data, TransitionType type, bool canBack);

    bool IsAnimatorRunning()
    {
        return ((animator_ != nullptr) && (animator_->GetState() != Animator::STOP));
    }

    bool BackToPrevSlicePage(TransitionType type = TransitionType::TRANSITION_INVALID);

    bool BackToCachedSlicePage(uint16_t pageId, TransitionType type = TransitionType::TRANSITION_INVALID);

    bool IsCachedSlicePage(uint16_t pageId);

    void OnOrientationChanged(AppRotationType rot);

    bool IsPageBeingSwitched() const;

    bool IsBackDragging() const;

    void EnablePendingRotation();

    AppRotationType GetCurRotation() const;

    void SetCurRotation(AppRotationType rot);

    bool IsPageBeingSwitched()
    {
        return isPageBeingSwitched_;
    }

    SlicePageManager(const SlicePageManager&) = delete;
    SlicePageManager& operator=(const SlicePageManager&) = delete;
    SlicePageManager(SlicePageManager&&) = delete;
    SlicePageManager& operator=(SlicePageManager&&) = delete;

private:
    friend class AbilitySlice;
    friend class SlicePageSwitchCallback;
    void AttachVP(PresenterBase* p, ViewBase* v)
    {
        presenter_ = p;
        view_ = v;
    }
    void StartSlicePage(void* data = nullptr);
    void ResumeSlicePage();
    void PauseSlicePage(PauseReason reason);
    void StopSlicePage(bool clearBackQueue = true);
    bool StartSwitchAnimator();
    void StopSwitchAnimator();
    void PopPagesInBackQueue(uint8_t popCnt);

    Animator* animator_ = nullptr;
    SlicePageSwitchCallback* switchCallback_ = nullptr;
    bool isPageBeingSwitched_;
    uint16_t sliceId_;
    uint16_t targetPageId_;
    SlicePageBase* curPage_;
    SlicePageBase* nextPage_;
    Graphic::Vector<SlicePageBase*> pagesInBackQueue_;
    PresenterBase* presenter_;
    ViewBase* view_;
    TransitionCallback* transitionCallback_;
    bool canBack_;
    bool isBackOperationRunning_;
    bool hasPendingRotateOperation_ = false;
    AppRotationType curRot_ = AppRotationType::ROTATION_0;
};
}
#endif
