/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: AlipayStartPage
 * Create: 2025-06-06
 */

#ifndef ALIPAY_START_PAGE_H
#define ALIPAY_START_PAGE_H

#include <sys/time.h>
#include "components/ui_image_view.h"
#include "components/ui_view_group.h"
#include "animator/animator.h"
#include "SlicePage.h"
#include "AlipayHsc32I1Presenter.h"

namespace OHOS {
class AlipayStartPage : public SlicePage<AlipayHsc32I1Presenter>,
    public UIView::OnClickListener,
    public UIView::OnDragListener {
public:
    AlipayStartPage() = default;
    AlipayStartPage(const AlipayStartPage&) = delete;
    AlipayStartPage& operator=(const AlipayStartPage&) = delete;
    ~AlipayStartPage() override;
    void OnStart(void* data) override;
    void OnPause() override;
    void OnResume() override;
    bool OnDrag(UIView& view, const DragEvent& event) override;
    bool OnClick(UIView& view, const ClickEvent& event) override;
private:
    void CreateUIImage(void);
    void CreateAnimator(void);
    class AlipayMainAnimatorCallback : public AnimatorCallback {
    public:
        AlipayMainAnimatorCallback(AlipayStartPage* page):page_(page) {}
        virtual ~AlipayMainAnimatorCallback() override {}
        void Callback(UIView* view) override;
    private:
        AlipayStartPage* page_{nullptr};
    };
    AlipayMainAnimatorCallback* callback_{nullptr};
    Animator* animator_{nullptr};
    UIViewGroup *group_{nullptr};
    UIImageView *imageView_{nullptr};
    time_t startTime_{0};
};
}  // namespace OHOS
#endif  // ALIPAY_START_PAGE_H