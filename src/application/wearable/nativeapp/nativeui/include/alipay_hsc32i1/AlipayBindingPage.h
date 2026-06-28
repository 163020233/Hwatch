/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: AlipayBindingPage
 * Create: 2025-06-06
 */

#ifndef ALIPAY_BINDING_PAGE_H
#define ALIPAY_BINDING_PAGE_H

#include "components/ui_label.h"
#include "components/ui_image_animator.h"
#include "components/ui_view_group.h"
#include "SlicePage.h"
#include "AlipayHsc32I1Presenter.h"

#define ALIPAY_BIND_SEARCH_IMAGE_NUM 8

namespace OHOS {
class AlipayBindingPage : public SlicePage<AlipayHsc32I1Presenter>,
    public UIView::OnClickListener,
    public UIView::OnDragListener {
public:
    AlipayBindingPage() = default;
    AlipayBindingPage(const AlipayBindingPage&) = delete;
    AlipayBindingPage& operator=(const AlipayBindingPage&) = delete;
    ~AlipayBindingPage() override;
    void OnPause() override;
    void OnResume() override;
    void OnStart(void* data) override;
    bool OnDragStart(UIView& view, const DragEvent& event) override;
    bool OnDrag(UIView& view, const DragEvent& event) override;
    bool OnDragEnd(UIView& view, const DragEvent& event) override;
    bool OnClick(UIView& view, const ClickEvent& event) override;
    void UpdateImageRotate();
private:
    void CreateUILabel(void);
    void CreateUIAnimator(void);
    class AlipayBindingPageCallback : public AnimatorCallback {
    public:
        AlipayBindingPageCallback(AlipayBindingPage* page) : page_(page) {}
        virtual ~AlipayBindingPageCallback() override {}
        void Callback(UIView* view) override;
    private:
        AlipayBindingPage* page_{nullptr};
    };
    AlipayBindingPageCallback* animatorCallBack_{nullptr};
    Animator *animator_ = nullptr;
    time_t startTime_{0};

    UIViewGroup *group_{nullptr};
    UILabel *textLabel_{nullptr};
    UIImageView *bindingImg_{nullptr};
};
}  // namespace OHOS
#endif  // ALIPAY_BINDING_PAGE_H