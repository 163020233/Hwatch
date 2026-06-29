/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: AlipayTransTipsPage
 * Create: 2025-06-06
 */

#ifndef ALIPAY_TRANS_TIPS_PAGE_H
#define ALIPAY_TRANS_TIPS_PAGE_H

#include "components/ui_label.h"
#include "components/ui_image_view.h"
#include "components/ui_label_button.h"
#include "components/ui_view_group.h"
#include "SlicePage.h"
#include "AlipayHsc32I1Presenter.h"

namespace OHOS {
class AlipayTransTipsPage : public SlicePage<AlipayHsc32I1Presenter>,
    public UIView::OnClickListener,
    public UIView::OnDragListener {
public:
    AlipayTransTipsPage() = default;
    AlipayTransTipsPage(const AlipayTransTipsPage&) = delete;
    AlipayTransTipsPage& operator=(const AlipayTransTipsPage&) = delete;
    ~AlipayTransTipsPage() override;
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
    UILabelButton *okButton_{nullptr};
};
}  // namespace OHOS
#endif  // ALIPAY_TRANS_TIPS_PAGE_H