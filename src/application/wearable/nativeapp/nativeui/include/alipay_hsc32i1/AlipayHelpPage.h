/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: AlipayHelpPage
 * Create: 2025-06-06
 */

#ifndef ALIPAY_PROMPT_PAGE_H
#define ALIPAY_PROMPT_PAGE_H

#include "components/ui_label.h"
#include "components/ui_qrcode.h"
#include "components/ui_view_group.h"
#include "SlicePage.h"
#include "AlipayHsc32I1Presenter.h"

namespace OHOS {
class AlipayHelpPage : public SlicePage<AlipayHsc32I1Presenter>,
    public UIView::OnClickListener,
    public UIView::OnDragListener {
public:
    AlipayHelpPage() = default;
    AlipayHelpPage(const AlipayHelpPage&) = delete;
    AlipayHelpPage& operator=(const AlipayHelpPage&) = delete;
    ~AlipayHelpPage() override;
    void OnStart(void* data) override;
    bool OnDragStart(UIView& view, const DragEvent& event) override;
    bool OnDrag(UIView& view, const DragEvent& event) override;
    bool OnDragEnd(UIView& view, const DragEvent& event) override;
    bool OnClick(UIView& view, const ClickEvent& event) override;
private:
    void CreateUILabel(void);
    void CreateUIQrcode(void);
    UIViewGroup *group_{nullptr};
    UILabel *titleLabel_{nullptr};
    UIQrcode *qrcode_{nullptr};
};
}  // namespace OHOS
#endif  // ALIPAY_PROMPT_PAGE_H