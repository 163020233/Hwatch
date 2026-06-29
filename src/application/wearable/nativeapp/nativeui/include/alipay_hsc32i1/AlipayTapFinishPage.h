/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: AlipayTapFinishPage
 * Create: 2025-06-06
 */

#ifndef ALIPAY_TAP_FINISH_PAGE_H
#define ALIPAY_TAP_FINISH_PAGE_H

#include "components/ui_label.h"
#include "components/ui_image_view.h"
#include "components/ui_label_button.h"
#include "components/ui_view_group.h"
#include "SlicePage.h"
#include "AlipayHsc32I1Presenter.h"

namespace OHOS {
class AlipayTapFinishPage : public SlicePage<AlipayHsc32I1Presenter>,
    public UIView::OnClickListener,
    public UIView::OnDragListener {
public:
    AlipayTapFinishPage() = default;
    AlipayTapFinishPage(const AlipayTapFinishPage&) = delete;
    AlipayTapFinishPage& operator=(const AlipayTapFinishPage&) = delete;
    ~AlipayTapFinishPage() override;
    void OnStart(void* data) override;
    bool OnDrag(UIView& view, const DragEvent& event) override;
    bool OnClick(UIView& view, const ClickEvent& event) override;
private:
    void CreateUILabel(void);
    void CreateUIImage(void);
    void CreateUIButton(void);
    UIViewGroup *group_{nullptr};
    UILabel *textLabel_{nullptr};
    UILabel *symbolLabel_{nullptr};
    UILabel *amountLabel_{nullptr};
    UIImageView *logoImage_{nullptr};
    UILabelButton *labelButton_{nullptr};
};
}  // namespace OHOS
#endif  // ALIPAY_TAP_FINISH_PAGE_H