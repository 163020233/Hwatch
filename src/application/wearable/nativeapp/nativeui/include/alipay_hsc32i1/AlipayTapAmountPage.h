/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: AlipayTapAmountPage
 * Create: 2025-06-06
 */

#ifndef ALIPAY_TAP_AMOUNT_PAGE_H
#define ALIPAY_TAP_AMOUNT_PAGE_H

#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_view_group.h"
#include "SlicePage.h"
#include "AlipayHsc32I1Presenter.h"

constexpr uint16_t BTN_NUM = 13;
namespace OHOS {
class AlipayTapAmountPage : public SlicePage<AlipayHsc32I1Presenter>,
    public UIView::OnClickListener,
    public UIView::OnDragListener {
public:
    AlipayTapAmountPage() = default;
    AlipayTapAmountPage(const AlipayTapAmountPage&) = delete;
    AlipayTapAmountPage& operator=(const AlipayTapAmountPage&) = delete;
    ~AlipayTapAmountPage() override;
    void OnStart(void* data) override;
    bool OnDragStart(UIView& view, const DragEvent& event) override;
    bool OnDrag(UIView& view, const DragEvent& event) override;
    bool OnDragEnd(UIView& view, const DragEvent& event) override;
    bool OnClick(UIView& view, const ClickEvent& event) override;
    void ClickOK();
    bool ClickZero();
    bool ClickNonZero(UIView &view);
    void ClickDot();
    void ClickDel();
private:
    void CreateUILabel(void);
    void CreateUIButton(void);
    UIViewGroup *group_{nullptr};
    UILabel *textLabel_{nullptr};
    UILabelButton *labelButton_[BTN_NUM]{nullptr};
    std::string inputAmount_;
};
}  // namespace OHOS
#endif  // ALIPAY_TAP_AMOUNT_PAGE_H