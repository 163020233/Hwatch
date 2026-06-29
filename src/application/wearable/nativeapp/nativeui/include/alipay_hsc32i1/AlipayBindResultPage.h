/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: AlipayBindResultPage
 * Create: 2025-06-06
 */

#ifndef ALIPAY_BIND_RESULT_PAGE_H
#define ALIPAY_BIND_RESULT_PAGE_H

#include "components/ui_label.h"
#include "components/ui_image_view.h"
#include "components/ui_view_group.h"
#include "animator/animator.h"
#include "SlicePage.h"
#include "AlipayHsc32I1Presenter.h"

namespace OHOS {
class AlipayBindResultPage : public SlicePage<AlipayHsc32I1Presenter>,
    public UIView::OnClickListener,
    public UIView::OnDragListener {
public:
    AlipayBindResultPage() = default;
    AlipayBindResultPage(const AlipayBindResultPage&) = delete;
    AlipayBindResultPage& operator=(const AlipayBindResultPage&) = delete;
    ~AlipayBindResultPage() override;
    void OnStart(void* data) override;
    void OnPause() override;
    void OnResume() override;
    bool OnDrag(UIView& view, const DragEvent& event) override;
    bool OnClick(UIView& view, const ClickEvent& event) override;
private:
    void CreateUILabel(void);
    void CreateUIImage(void);
    void CreateAnimator(void);
    class AlipayBindResultAnimatorCallback : public AnimatorCallback {
    public:
        AlipayBindResultAnimatorCallback(AlipayBindResultPage* page):page_(page) {}
        virtual ~AlipayBindResultAnimatorCallback() override {}
        void Callback(UIView* view) override;
    private:
        AlipayBindResultPage* page_{nullptr};
    };
    AlipayBindResultAnimatorCallback* callback_{nullptr};
    Animator* animator_{nullptr};
    UIViewGroup *group_{nullptr};
    UILabel *textLabel_{nullptr};
    UIImageView *imageView_{nullptr};
    time_t startTime_{0};
};
}  // namespace OHOS
#endif  // ALIPAY_BIND_RESULT_PAGE_H