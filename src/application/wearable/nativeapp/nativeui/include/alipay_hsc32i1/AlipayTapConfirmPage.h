/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: AlipayTapConfirmPage
 * Create: 2025-06-06
 */

#ifndef ALIPAY_TAP_CONFIRM_PAGE_H
#define ALIPAY_TAP_CONFIRM_PAGE_H

#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_view_group.h"
#include "SlicePage.h"
#include "AlipayHsc32I1Presenter.h"

namespace OHOS {
class AlipayTapConfirmPage : public SlicePage<AlipayHsc32I1Presenter>,
    public UIView::OnClickListener,
    public UIView::OnDragListener {
public:
    AlipayTapConfirmPage() = default;
    AlipayTapConfirmPage(const AlipayTapConfirmPage&) = delete;
    AlipayTapConfirmPage& operator=(const AlipayTapConfirmPage&) = delete;
    ~AlipayTapConfirmPage() override;
    void OnStart(void* data) override;
    bool OnDrag(UIView& view, const DragEvent& event) override;
    bool OnClick(UIView& view, const ClickEvent& event) override;
private:
    void CreateUILabel(void);
    void CreateUIButton(void);
    UIViewGroup *group_{nullptr};
    UILabel *textLabel_{nullptr};
    UILabel *amountLabel_{nullptr};
    UILabelButton *payButton_{nullptr};
    UILabelButton *reinputButton_{nullptr};
    UILabelButton *cancelButton_{nullptr};
};
}  // namespace OHOS
#endif  // ALIPAY_TAP_CONFIRM_PAGE_H