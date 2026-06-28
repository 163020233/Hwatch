/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SlicePage
 * Create: 2025-11
 */

#ifndef SLICE_PAGE_H
#define SLICE_PAGE_H
#include "components/ui_view_group.h"
#include "Presenter.h"
#include "RotationManager.h"
#include "Slice.h"

namespace OHOS {
enum class SlicePageState : uint8_t {
    UNINITIALIZED,
    START,
    RESUME,
    PAUSE,
    STOP
};
class SlicePageBase {
public:
    SlicePageBase() : pageId_(0), pageContainer_(nullptr), backDragListener_(nullptr), state_(SlicePageState::UNINITIALIZED) {}

    virtual ~SlicePageBase() = default;

    virtual void OnStart(void* data) = 0;

    virtual void OnResume() {}

    /**
     * @deprecated 推荐使用方法OnPause(PauseReason reason)
     */
    virtual void OnPause() {}

    virtual void OnPause(PauseReason reason) {}

    virtual void OnStop() {}

    UIViewGroup* GetSlicePageContainer()
    {
        return pageContainer_;
    }

    BackDragListener* GetBackDragListener()
    {
        return backDragListener_;
    }

    void SetPageState(SlicePageState state)
    {
        state_ = state;
    }

    SlicePageState GetPageState()
    {
        return state_;
    }

    void AddViewToPageContainer(UIView* view);

    virtual void AttachPresenter(PresenterBase* p) = 0;

    virtual void OnOrientationChanged(AppRotationType rot) {}

    SlicePageBase(const SlicePageBase&) = delete;
    SlicePageBase& operator=(const SlicePageBase&) = delete;
    SlicePageBase(SlicePageBase&&) = delete;
    SlicePageBase& operator=(SlicePageBase&&) = delete;
    uint16_t pageId_;

private:
    friend class SlicePageManager;
    void SetupView(bool canBack);
    void TearDownView();
    UIViewGroup* pageContainer_;
    BackDragListener* backDragListener_;
    AppRotationType curRot_ = AppRotationType::ROTATION_0;
    SlicePageState state_;
};

template<class P>
class SlicePage : public SlicePageBase {
public:
    void AttachPresenter(PresenterBase* p) override
    {
        presenter_ = dynamic_cast<P*>(p);
    }
protected:
    P* presenter_ = nullptr;
};
}
#endif