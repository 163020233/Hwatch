/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: AlipayTapPayingPage
 * Create: 2025-06-06
 */

#ifndef ALIPAY_TAP_PAYING_PAGE_H
#define ALIPAY_TAP_PAYING_PAGE_H

#include "components/ui_image_animator.h"
#include "components/ui_view_group.h"
#include "SlicePage.h"
#include "AlipayHsc32I1Presenter.h"

#define ALIPAY_TAP_PAYING_IMAGE_NUM 12

namespace OHOS {
class AlipayTapPayingPage : public SlicePage<AlipayHsc32I1Presenter>,
    public UIView::OnClickListener,
    public UIView::OnDragListener,
    public UIImageAnimatorView::AnimatorStopListener {
public:
    AlipayTapPayingPage() = default;
    AlipayTapPayingPage(const AlipayTapPayingPage&) = delete;
    AlipayTapPayingPage& operator=(const AlipayTapPayingPage&) = delete;
    ~AlipayTapPayingPage() override;
    void OnStart(void* data) override;
    void OnPause() override;
    void OnResume() override;
    bool OnDragStart(UIView& view, const DragEvent& event) override;
    bool OnDrag(UIView& view, const DragEvent& event) override;
    bool OnDragEnd(UIView& view, const DragEvent& event) override;
    bool OnClick(UIView& view, const ClickEvent& event) override;
private:
    void CreateUIAnimator();
    void CreateImageAnimator();
    UIViewGroup *group_{nullptr};
    UIImageAnimatorView *imageAnimatorView_{nullptr};
    ImageAnimatorInfo tapPayingImageInfo_[ALIPAY_TAP_PAYING_IMAGE_NUM];
    class AlipayTapPayingPageCallback : public AnimatorCallback {
    public:
        AlipayTapPayingPageCallback(AlipayTapPayingPage* page) : page_(page) {}
        virtual ~AlipayTapPayingPageCallback() override {}
        void Callback(UIView* view) override;
    private:
        AlipayTapPayingPage* page_{nullptr};
    };
    AlipayTapPayingPageCallback* animatorCallBack_{nullptr};
    Animator *animator_ = nullptr;
    time_t startTime_{0};
};
}  // namespace OHOS
#endif  // ALIPAY_TAP_PAYING_PAGE_H