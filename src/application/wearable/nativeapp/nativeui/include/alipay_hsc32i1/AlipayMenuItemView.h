/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: AlipayMenuItemView
 * Create: 2025-06-06
 */

#ifndef ALIPAY_MENU_ITEM_VIEW_H
#define ALIPAY_MENU_ITEM_VIEW_H

#include "components/ui_view_group.h"
#include "components/ui_image_view.h"
#include "components/ui_label.h"

namespace OHOS {
typedef struct {
    int iconId;
    const char *label;
    const char *viewId;
} AlipayMenuItem;

class AlipayMenuItemView : public UIViewGroup {
public:
    AlipayMenuItemView();
    AlipayMenuItemView(const AlipayMenuItemView &) = delete;
    AlipayMenuItemView &operator=(const AlipayMenuItemView &) = delete;
    ~AlipayMenuItemView() override;

    virtual void SetItemInfo(const AlipayMenuItem &itemInfo);

private:
    UIViewGroup bg_;
    UIImageView icon_;
    UILabel label_;
#ifdef SUPPORT_ALIPAY_TAP
    UIImageView light_;
#endif
};
}
#endif