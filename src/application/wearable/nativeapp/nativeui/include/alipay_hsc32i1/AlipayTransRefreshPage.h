/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: AlipayTransRefreshPage
 * Create: 2025-06-06
 */

#ifndef ALIPAY_TRANS_REFRESH_PAGE_H
#define ALIPAY_TRANS_REFRESH_PAGE_H

#include "components/ui_label.h"
#include "components/ui_image_animator.h"
#include "components/ui_view_group.h"
#include "SlicePage.h"
#include "AlipayHsc32I1Presenter.h"

#define ALIPAY_TRANS_SEARCH_IMAGE_NUM 8

namespace OHOS {
class AlipayTransRefreshPage : public SlicePage<AlipayHsc32I1Presenter>,
    public UIView::OnClickListener,
    public UIView::OnDragListener {
public:
    AlipayTransRefreshPage() = default;
    AlipayTransRefreshPage(const AlipayTransRefreshPage&) = delete;
    AlipayTransRefreshPage& operator=(const AlipayTransRefreshPage&) = delete;
    ~AlipayTransRefreshPage() override;
    void OnStart(void* data) override;
    bool OnDrag(UIView& view, const DragEvent& event) override;
    bool OnClick(UIView& view, const ClickEvent& event) override;
private:
    void CreateUILabel(void);
    void CreateUIAnimator(void);
    UIViewGroup *group_{nullptr};
    UILabel *textLabel_{nullptr};
    UIImageAnimatorView *imageAnimatorView_{nullptr};
    ImageAnimatorInfo searchImageInfo_[ALIPAY_TRANS_SEARCH_IMAGE_NUM];
};
}  // namespace OHOS
#endif  // ALIPAY_TRANS_REFRESH_PAGE_H