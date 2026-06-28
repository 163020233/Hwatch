/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: AlipayTransListPage
 * Create: 2025-06-06
 */

#ifndef ALIPAY_TRANS_LIST_PAGE_H
#define ALIPAY_TRANS_LIST_PAGE_H

#include "components/ui_simple_list.h"
#include "components/ui_label_button.h"
#include "components/ui_view_group.h"
#include "SlicePage.h"
#include "AlipayHsc32I1Presenter.h"
#include "AlipayTransCardItemView.h"

namespace OHOS {
constexpr uint8_t ALIPAY_TRANS_CARD_LIST_ITEM_MAX = 20;

class AlipayTransListPage : public SlicePage<AlipayHsc32I1Presenter>,
    public UIView::OnClickListener,
    public UIView::OnDragListener {
public:
    AlipayTransListPage();
    AlipayTransListPage(const AlipayTransListPage&) = delete;
    AlipayTransListPage& operator=(const AlipayTransListPage&) = delete;
    ~AlipayTransListPage() override;
    void OnStart(void* data) override;
    bool OnDrag(UIView& view, const DragEvent& event) override;
    bool OnClick(UIView& view, const ClickEvent& event) override;
private:
    void CreateUIList(void);
    void CreateUIButton(void);
    UIViewGroup *group_{nullptr};
    UISimpleList *alipayTransList_{nullptr};
    UILabelButton *refreshButton_ { nullptr };
    AlipayTransCardItem alipayTransCards_[ALIPAY_TRANS_CARD_LIST_ITEM_MAX];
};
}  // namespace OHOS
#endif  // ALIPAY_TRANS_LIST_PAGE_H