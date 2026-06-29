/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: AlipayQrcodePage
 * Create: 2025-06-06
 */

#ifndef ALIPAY_QRCODE_PAGE_H
#define ALIPAY_QRCODE_PAGE_H

#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_image_view.h"
#include "components/ui_qrcode.h"
#include "components/ui_view_group.h"
#include "animator/animator.h"
#include "SlicePage.h"
#include "AlipayHsc32I1Presenter.h"

namespace OHOS {
class AlipayQrcodePage : public SlicePage<AlipayHsc32I1Presenter>,
    public UIView::OnClickListener,
    public UIView::OnDragListener {
public:
    AlipayQrcodePage() = default;
    AlipayQrcodePage(const AlipayQrcodePage&) = delete;
    AlipayQrcodePage& operator=(const AlipayQrcodePage&) = delete;
    ~AlipayQrcodePage() override;
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
    void CreateUIImage(void);
    void CreateUIQrcode(void);
    void CreateAnimator(void);
    void RefreshQrcode();
    class AlipayQrcodeAnimatorCallback : public AnimatorCallback {
    public:
        AlipayQrcodeAnimatorCallback(AlipayQrcodePage* page):page_(page) {}
        virtual ~AlipayQrcodeAnimatorCallback() override {}
        void Callback(UIView* view) override;
    private:
        AlipayQrcodePage* page_{nullptr};
    };
    AlipayQrcodeAnimatorCallback* callback_{nullptr};
    Animator* animator_{nullptr};
    UIViewGroup *group_{nullptr};
    UIImageView *indicatorWhiteImage_{ nullptr };
    UIImageView *indicatorGreyImage_{ nullptr };
    UILabel *titleLabel_{nullptr};
    UIQrcode *payQrcode_{nullptr};
    UILabelButton *changeButton_{nullptr};
    time_t startTime_{0};
#ifdef SUPPORT_ALIPAY_TAP
    void CreateGuideView(void);
    UIImageView *closeImage_{ nullptr };
    UIImageView *lineImage_{ nullptr };
    UIImageView *bgImage_{ nullptr };
    UILabelButton *tapButton_{nullptr};
#endif
};
}  // namespace OHOS
#endif  // ALIPAY_QRCODE_PAGE_H