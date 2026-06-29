/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: AlipayPayFailedPage
 * Create: 2025-06-06
 */

#ifndef ALIPAY_PAY_FAILED_PAGE_H
#define ALIPAY_PAY_FAILED_PAGE_H

#include "components/ui_label.h"
#include "components/ui_image_view.h"
#include "components/ui_label_button.h"
#include "components/ui_view_group.h"
#include "SlicePage.h"
#include "AlipayHsc32I1Presenter.h"

namespace OHOS {
class AlipayPayFailedPage : public SlicePage<AlipayHsc32I1Presenter>,
    public UIView::OnClickListener,
    public UIView::OnDragListener {
public:
    AlipayPayFailedPage() = default;
    AlipayPayFailedPage(const AlipayPayFailedPage&) = delete;
    AlipayPayFailedPage& operator=(const AlipayPayFailedPage&) = delete;
    ~AlipayPayFailedPage() override;
    void OnStart(void* data) override;
    bool OnDrag(UIView& view, const DragEvent& event) override;
    bool OnClick(UIView& view, const ClickEvent& event) override;
private:
    void CreateUILabel(void);
    void CreateUIImage(void);
    void CreateUIButton(void);
    UIViewGroup *group_{nullptr};
    UILabel *textLabel_{nullptr};
    UIImageView *imageView_{nullptr};
    UILabelButton *labelButton_{nullptr};
};
}  // namespace OHOS
#endif  // ALIPAY_PAY_FAILED_PAGE_H