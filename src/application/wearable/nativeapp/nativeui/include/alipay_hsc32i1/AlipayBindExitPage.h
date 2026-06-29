/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: AlipayBindExitPage
 * Create: 2025-06-06
 */

#ifndef ALIPAY_BIND_EXIT_PAGE_H
#define ALIPAY_BIND_EXIT_PAGE_H

#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_view_group.h"
#include "SlicePage.h"
#include "AlipayHsc32I1Presenter.h"

namespace OHOS {
class AlipayBindExitPage : public SlicePage<AlipayHsc32I1Presenter>,
    public UIView::OnClickListener,
    public UIView::OnDragListener {
public:
    AlipayBindExitPage() = default;
    AlipayBindExitPage(const AlipayBindExitPage&) = delete;
    AlipayBindExitPage& operator=(const AlipayBindExitPage&) = delete;
    ~AlipayBindExitPage() override;
    void OnStart(void* data) override;
    bool OnDrag(UIView& view, const DragEvent& event) override;
    bool OnClick(UIView& view, const ClickEvent& event) override;
private:
    void CreateUILabel(void);
    void CreateUIButton(void);
    UIViewGroup *group_{nullptr};
    UILabel *titleLabel_{nullptr};
    UILabel *textLabel_{nullptr};
    UILabelButton *exitButton_{nullptr};
    UILabelButton *bindButton_{nullptr};
};
}  // namespace OHOS
#endif  // ALIPAY_BIND_EXIT_PAGE_H