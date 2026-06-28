/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: AlipayUnbindFinishPage
 * Create: 2025-06-06
 */

#ifndef ALIPAY_UNBIND_FINISH_PAGE_H
#define ALIPAY_UNBIND_FINISH_PAGE_H

#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_view_group.h"
#include "SlicePage.h"
#include "AlipayHsc32I1Presenter.h"

namespace OHOS {
class AlipayUnbindFinishPage : public SlicePage<AlipayHsc32I1Presenter>,
    public UIView::OnClickListener,
    public UIView::OnDragListener {
public:
    AlipayUnbindFinishPage() = default;
    AlipayUnbindFinishPage(const AlipayUnbindFinishPage&) = delete;
    AlipayUnbindFinishPage& operator=(const AlipayUnbindFinishPage&) = delete;
    ~AlipayUnbindFinishPage() override;
    void OnStart(void* data) override;
    bool OnDrag(UIView& view, const DragEvent& event) override;
    bool OnClick(UIView& view, const ClickEvent& event) override;
private:
    void CreateUILabel(void);
    void CreateUIButton(void);
    UIViewGroup *group_{nullptr};
    UILabel *titleLabel_{nullptr};
    UILabel *textLabel_{nullptr};
    UILabelButton *confirmButton_{nullptr};
};
}  // namespace OHOS
#endif  // ALIPAY_UNBIND_FINISH_PAGE_H