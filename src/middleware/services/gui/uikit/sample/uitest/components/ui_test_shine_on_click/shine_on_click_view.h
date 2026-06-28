/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: ShineOnClickView
 * Create: 2025-11-18
 */

#ifndef SHINE_ON_CLICK_VIEW
#define SHINE_ON_CLICK_VIEW

#include "animator/animator_manager.h"
#include "components/ui_image_view.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_view_group.h"
#include "hal_tick.h"

namespace OHOS {
class ShineAnimatorCallback : public AnimatorCallback {
public:
    explicit ShineAnimatorCallback() {}
    ~ShineAnimatorCallback() override {}
    void Callback(UIView *view) override;

    void OnStop(UIView& view) override;

private:
    uint32_t startTime_ = 0;
};

class ShineOnClickView : public UIViewGroup, public UIView::OnClickListener {
public:
    ShineOnClickView()
    {
        (void)memset_s(shineMaskSrc_, sizeof(shineMaskSrc_), 0, sizeof(shineMaskSrc_));
    }
    ~ShineOnClickView() override;
    ShineOnClickView(const ShineOnClickView&) = delete;
    ShineOnClickView& operator=(const ShineOnClickView&) = delete;
    static ShineOnClickView *GetInstance(void);
    bool InitView();
    bool OnClick(UIView &view, const ClickEvent &event) override;
    void ShineOnClick(uint32_t elapseTime);
    void ShineEnd(void);
private:
    static constexpr int16_t SHINE_ANIMATE_TIME = 300; // 300: animate time
    static constexpr int16_t BUTTON_COUNT = 5;
    UIImageView *shineMask_{nullptr};
    ImageInfo* shineMaskSrc_[BUTTON_COUNT];
    ShineAnimatorCallback *shineCallBack_{nullptr};
    Animator *shineAnimator_{nullptr};
    UIView *clickView_{nullptr};
    UIViewGroup *container_{nullptr};

    void InitSubViewImg(void);
    void InitViewImg(int16_t x, int16_t y, uint32_t resId, const char* viewId);
    void InitViewImg(void);
    void InitViewLabel(const Rect& rect, const char* text);
    void InitViewLabel(void);
    void InitShineMask(void);
    void DeinitShineMask(void);
    void InitShineAnimator(void);
    void DeinitShineAnimator(void);
};
}
#endif
