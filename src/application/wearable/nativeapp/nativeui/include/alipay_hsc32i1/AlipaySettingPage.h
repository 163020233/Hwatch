/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: AlipaySettingPage
 * Create: 2025-06-06
 */

#ifndef ALIPAY_SETTING_PAGE_H
#define ALIPAY_SETTING_PAGE_H

#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_view_group.h"
#include "SlicePage.h"
#include "AlipayHsc32I1Presenter.h"

namespace OHOS {
class AlipaySettingPage : public SlicePage<AlipayHsc32I1Presenter>,
    public UIView::OnClickListener,
    public UIView::OnDragListener {
public:
    AlipaySettingPage() = default;
    AlipaySettingPage(const AlipaySettingPage&) = delete;
    AlipaySettingPage& operator=(const AlipaySettingPage&) = delete;
    ~AlipaySettingPage() override;
    void OnStart(void* data) override;
    bool OnDragStart(UIView& view, const DragEvent& event) override;
    bool OnDrag(UIView& view, const DragEvent& event) override;
    bool OnDragEnd(UIView& view, const DragEvent& event) override;
    bool OnClick(UIView& view, const ClickEvent& event) override;
private:
    void CreateUILabel(void);
    void CreateUIButton(void);
    UIViewGroup *group_{nullptr};
    UILabel *titleLabel_{nullptr};
    UILabel *nameLabel_{nullptr};
    UILabel *idLabel_{nullptr};
    UILabelButton *unbindButton_{nullptr};
};
}  // namespace OHOS
#endif  // ALIPAY_SETTING_PAGE_H