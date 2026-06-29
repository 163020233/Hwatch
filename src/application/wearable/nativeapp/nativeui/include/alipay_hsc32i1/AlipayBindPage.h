/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: AlipayBindPage
 * Create: 2025-06-06
 */

#ifndef ALIPAY_BIND_PAGE_H
#define ALIPAY_BIND_PAGE_H

#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_qrcode.h"
#include "components/ui_view_group.h"
#include "SlicePage.h"
#include "AlipayHsc32I1Presenter.h"

namespace OHOS {
class AlipayBindPage : public SlicePage<AlipayHsc32I1Presenter>,
    public UIView::OnClickListener,
    public UIView::OnDragListener {
public:
    AlipayBindPage() = default;
    AlipayBindPage(const AlipayBindPage&) = delete;
    AlipayBindPage& operator=(const AlipayBindPage&) = delete;
    ~AlipayBindPage() override;
    void OnStart(void* data) override;
    void OnPause() override;
    void OnResume() override;
    bool OnDragStart(UIView& view, const DragEvent& event) override;
    bool OnDrag(UIView& view, const DragEvent& event) override;
    bool OnDragEnd(UIView& view, const DragEvent& event) override;
    bool OnClick(UIView& view, const ClickEvent& event) override;
private:
    void CreateUILabel(void);
    void CreateUIButton(void);
    void CreateUIQrcode(const char*);
    void CreateAnimator();
    class AlipayBindAnimatorCallback : public AnimatorCallback {
    public:
        AlipayBindAnimatorCallback(AlipayBindPage* page):page_(page) {}
        virtual ~AlipayBindAnimatorCallback() override {}
        void Callback(UIView* view) override;
    private:
        AlipayBindPage* page_{nullptr};
    };
    AlipayBindAnimatorCallback* callback_{nullptr};
    Animator* animator_{nullptr};
    time_t startTime_{0};

#ifndef ALIPAY_NET
    void DrawHelpTipsView(void);
    UIImageView *helpBgImg_{nullptr};
    UILabelButton *tipsButton_{nullptr};
    UIImageView *helpButton_{nullptr};
#endif
    UIViewGroup *group_{nullptr};
    UILabel *titleLabel_{nullptr};
    UILabelButton *retryButton_{nullptr};
    UIQrcode *bindQrcode_{nullptr};
};
}  // namespace OHOS
#endif  // ALIPAY_BIND_PAGE_H