/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: AlipayMenuPage
 * Create: 2025-06-06
 */

#ifndef ALIPAY_MENU_PAGE_H
#define ALIPAY_MENU_PAGE_H

#include "components/ui_simple_list.h"
#include "components/ui_view_group.h"
#include "ui_resource_alipay.h"
#include "SlicePage.h"
#include "AlipayHsc32I1Presenter.h"
#include "AlipayMenuItemView.h"

namespace OHOS {
constexpr uint8_t ALIPAY_LIST_ITEM_DEFAULT = 3;
#ifdef SUPPORT_ALIPAY_TAP
constexpr uint8_t ALIPAY_LIST_ITEM_TAP = 1;
#else
constexpr uint8_t ALIPAY_LIST_ITEM_TAP = 0;
#endif
#ifdef SUPPORT_ALIPAY_TRANS
constexpr uint8_t ALIPAY_LIST_ITEM_TRANS = 1;
#else
constexpr uint8_t ALIPAY_LIST_ITEM_TRANS = 0;
#endif
constexpr uint8_t ALIPAY_LIST_ITEM_MAX = ALIPAY_LIST_ITEM_TAP + ALIPAY_LIST_ITEM_TRANS + ALIPAY_LIST_ITEM_DEFAULT;

constexpr const char* ALIPAY_ITEM_PAY = "AlipayItemPay";
constexpr const char* ALIPAY_ITEM_TAP = "AlipayItemTap";
constexpr const char* ALIPAY_ITEM_TRANS = "AlipayItemTrans";
constexpr const char* ALIPAY_ITEM_SETTING = "AlipayItemSetting";
constexpr const char* ALIPAY_ITEM_HELP = "AlipayItemHelp";

class AlipayMenuPage : public SlicePage<AlipayHsc32I1Presenter>,
    public UIView::OnClickListener,
    public UIView::OnDragListener {
public:
    AlipayMenuPage();
    AlipayMenuPage(const AlipayMenuPage&) = delete;
    AlipayMenuPage& operator=(const AlipayMenuPage&) = delete;
    ~AlipayMenuPage() override;
    void OnStart(void* data) override;
    bool OnDragStart(UIView& view, const DragEvent& event) override;
    bool OnDrag(UIView& view, const DragEvent& event) override;
    bool OnDragEnd(UIView& view, const DragEvent& event) override;
    bool OnClick(UIView& view, const ClickEvent& event) override;
private:
    void CreateUIList(void);
    UIViewGroup *group_{nullptr};
    UISimpleList *alipayList_{nullptr};

    AlipayMenuItem alipayMenu_[ALIPAY_LIST_ITEM_MAX] = {
        {IMAGE_ALIPAY_PAY, "付款码", ALIPAY_ITEM_PAY},
#ifdef SUPPORT_ALIPAY_TAP
        {IMAGE_ALIPAY_TAP, "碰一下", ALIPAY_ITEM_TAP},
#endif
#ifdef SUPPORT_ALIPAY_TRANS
        {IMAGE_ALIPAY_TRANS, "乘车码", ALIPAY_ITEM_TRANS},
#endif
        {IMAGE_ALIPAY_SETTING, "设置", ALIPAY_ITEM_SETTING},
        {IMAGE_ALIPAY_HELP, "帮助", ALIPAY_ITEM_HELP}};
};
}  // namespace OHOS
#endif  // ALIPAY_MENU_PAGE_H