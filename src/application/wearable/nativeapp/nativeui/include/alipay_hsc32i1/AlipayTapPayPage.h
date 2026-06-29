/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: AlipayTapPayPage
 * Create: 2025-06-06
 */

#ifndef ALIPAY_TAP_PAY_PAGE_H
#define ALIPAY_TAP_PAY_PAGE_H

#include "components/ui_label.h"
#include "components/ui_image_view.h"
#include "components/ui_view_group.h"
#include "SlicePage.h"
#include "AlipayHsc32I1Presenter.h"

namespace OHOS {
class AlipayTapPayPage : public SlicePage<AlipayHsc32I1Presenter>,
    public UIView::OnClickListener,
    public UIView::OnDragListener {
public:
    AlipayTapPayPage() = default;
    AlipayTapPayPage(const AlipayTapPayPage&) = delete;
    AlipayTapPayPage& operator=(const AlipayTapPayPage&) = delete;
    ~AlipayTapPayPage() override;
    void OnStart(void* data) override;
    void OnPause() override;
    void OnResume() override;
    bool OnDragStart(UIView& view, const DragEvent& event) override;
    bool OnDrag(UIView& view, const DragEvent& event) override;
    bool OnDragEnd(UIView& view, const DragEvent& event) override;
    bool OnClick(UIView& view, const ClickEvent& event) override;
private:
    void CreateUILabel();
    void CreateUIImage();
    void CreateUIAnimator();
    UIViewGroup *group_{nullptr};
    UILabel *textLabel_{nullptr};
    UILabel *tipsLabel_{nullptr};
    UIImageView *imageView_{nullptr};
    class AlipayTapPayPageCallback : public AnimatorCallback {
    public:
        AlipayTapPayPageCallback(AlipayTapPayPage* page) : page_(page) {}
        virtual ~AlipayTapPayPageCallback() override {}
        void Callback(UIView* view) override;
    private:
        AlipayTapPayPage* page_{nullptr};
    };
    AlipayTapPayPageCallback* animatorCallBack_{nullptr};
    Animator *animator_ = nullptr;
    time_t startTime_{0};
};
}  // namespace OHOS
#endif  // ALIPAY_TAP_PAY_PAGE_H