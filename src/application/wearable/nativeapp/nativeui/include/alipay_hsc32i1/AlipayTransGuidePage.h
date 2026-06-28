/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: AlipayTransGuidePage
 * Create: 2025-06-06
 */

#ifndef ALIPAY_TRANS_GUIDE_PAGE_H
#define ALIPAY_TRANS_GUIDE_PAGE_H

#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_view_group.h"
#include "SlicePage.h"
#include "AlipayHsc32I1Presenter.h"

namespace OHOS {
class AlipayTransGuidePage : public SlicePage<AlipayHsc32I1Presenter>,
    public UIView::OnClickListener,
    public UIView::OnDragListener {
public:
    AlipayTransGuidePage() = default;
    AlipayTransGuidePage(const AlipayTransGuidePage&) = delete;
    AlipayTransGuidePage& operator=(const AlipayTransGuidePage&) = delete;
    ~AlipayTransGuidePage() override;
    void OnStart(void* data) override;
    bool OnDrag(UIView& view, const DragEvent& event) override;
    bool OnClick(UIView& view, const ClickEvent& event) override;
private:
    void CreateUILabel(void);
    void CreateUIButton(void);
    UIViewGroup *group_{nullptr};
    UILabel *guideLabel_{nullptr};
    UILabelButton *exitButton_{nullptr};
    UILabelButton *finishButton_{nullptr};
};
}  // namespace OHOS
#endif  // ALIPAY_TRANS_GUIDE_PAGE_H