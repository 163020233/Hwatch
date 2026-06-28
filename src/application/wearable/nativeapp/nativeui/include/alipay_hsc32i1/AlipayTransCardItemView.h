/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: AlipayTransCardItemView
 * Create: 2025-06-06
 */

#ifndef ALIPAY_TRANS_CARD_ITEM_VIEW_H
#define ALIPAY_TRANS_CARD_ITEM_VIEW_H

#include "components/ui_view_group.h"
#include "components/ui_label.h"

namespace OHOS {
typedef struct {
    uint8_t index;
    const char *label;
    const char *viewId;
} AlipayTransCardItem;

class AlipayTransCardItemView : public UIViewGroup {
public:
    AlipayTransCardItemView();
    ~AlipayTransCardItemView() override;

    uint8_t GetAlipayTransListIndex();
    virtual void SetItemInfo(const AlipayTransCardItem &itemInfo);

private:
    UIViewGroup bg_;
    UILabel label_;
    uint8_t index_;
};
}
#endif