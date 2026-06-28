/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: AlipayTransEnablePage
 * Create: 2025-06-06
 */

#ifndef ALIPAY_TRANS_ENABLE_PAGE_H
#define ALIPAY_TRANS_ENABLE_PAGE_H

#include "components/ui_label.h"
#include "components/ui_qrcode.h"
#include "components/ui_view_group.h"
#include "SlicePage.h"
#include "AlipayHsc32I1Presenter.h"

namespace OHOS {
class AlipayTransEnablePage : public SlicePage<AlipayHsc32I1Presenter>,
    public UIView::OnClickListener,
    public UIView::OnDragListener {
public:
    AlipayTransEnablePage() = default;
    AlipayTransEnablePage(const AlipayTransEnablePage&) = delete;
    AlipayTransEnablePage& operator=(const AlipayTransEnablePage&) = delete;
    ~AlipayTransEnablePage() override;
    void OnStart(void* data) override;
    void OnPause() override;
    void OnResume() override;
    bool OnDrag(UIView& view, const DragEvent& event) override;
    bool OnClick(UIView& view, const ClickEvent& event) override;
private:
    void CreateUILabel(void);
    void CreateUIQrcode(void);
    UIViewGroup *group_{nullptr};
    UILabel *titleLabel_{nullptr};
    UILabel *tipsLabel_{nullptr};
    UIQrcode *guideQrcode_{nullptr};
};
}  // namespace OHOS
#endif  // ALIPAY_TRANS_ENABLE_PAGE_H