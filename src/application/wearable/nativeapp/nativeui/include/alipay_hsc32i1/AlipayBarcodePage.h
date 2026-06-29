/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: AlipayBarcodePage
 * Create: 2025-06-06
 */

#ifndef ALIPAY_BARCODE_PAGE_H
#define ALIPAY_BARCODE_PAGE_H

#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_image_view.h"
#include "components/ui_barcode.h"
#include "components/ui_view_group.h"
#include "animator/animator.h"
#include "SlicePage.h"
#include "AlipayHsc32I1Presenter.h"

namespace OHOS {
class AlipayBarcodePage : public SlicePage<AlipayHsc32I1Presenter>,
    public UIView::OnClickListener,
    public UIView::OnDragListener {
public:
    AlipayBarcodePage() = default;
    AlipayBarcodePage(const AlipayBarcodePage&) = delete;
    AlipayBarcodePage& operator=(const AlipayBarcodePage&) = delete;
    ~AlipayBarcodePage() override;
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
    void CreateUIBarcode(void);
    void CreateAnimator(void);
    void RefreshBarcode();
    void ReformatBarcode(uint8_t *str, size_t strLen,
                         uint8_t *strBarcode, size_t barcodeLen);
    class AlipayBarcodeAnimatorCallback : public AnimatorCallback {
    public:
        AlipayBarcodeAnimatorCallback(AlipayBarcodePage* page):page_(page) {}
        virtual ~AlipayBarcodeAnimatorCallback() override {}
        void Callback(UIView* view) override;
    private:
        AlipayBarcodePage* page_{nullptr};
    };
    AlipayBarcodeAnimatorCallback* callback_{nullptr};
    Animator* animator_{nullptr};
    UIViewGroup *group_{nullptr};
    UIImageView *indicatorWhiteImage_{ nullptr };
    UIImageView *indicatorGreyImage_{ nullptr };
    UILabel *titleLabel_{nullptr};
    UILabel *payCodeLabel_{nullptr};
    UIBarcode *payBarcode_{nullptr};
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
#endif  // ALIPAY_BARCODE_PAGE_H