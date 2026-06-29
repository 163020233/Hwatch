/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: AlipayView
 * Create: 2025-06-06
 */

#ifndef ALIPAY_VIEW_H
#define ALIPAY_VIEW_H

#include "View.h"
#include "AlipayHsc32I1Presenter.h"

namespace OHOS {
constexpr uint8_t DIV_HALF = 2;
enum AlipayPages : uint8_t {
    ALIPAY_PAGE_START = 1,
    ALIPAY_PAGE_MENU,
    ALIPAY_PAGE_BIND,
    ALIPAY_PAGE_BINDING,
    ALIPAY_PAGE_BIND_EXIT,
    ALIPAY_PAGE_BIND_RESULT,
    ALIPAY_PAGE_PAY_QRCODE,
    ALIPAY_PAGE_PAY_BARCODE,
    ALIPAY_PAGE_PAY_SUCCESS,
    ALIPAY_PAGE_PAY_FAILED,
    ALIPAY_PAGE_SETTING,
    ALIPAY_PAGE_UNBIND,
    ALIPAY_PAGE_UNBIND_FINISH,
    ALIPAY_PAGE_HELP,
    ALIPAY_PAGE_TRANS_QRCODE,
    ALIPAY_PAGE_TRANS_LIST,
    ALIPAY_PAGE_TRANS_ENABLE,
    ALIPAY_PAGE_TRANS_GUIDE,
    ALIPAY_PAGE_TRANS_REFRESH,
    ALIPAY_PAGE_TRANS_TIPS,
    ALIPAY_PAGE_TRANS_RESULT,
    ALIPAY_PAGE_TAP_PAY,
    ALIPAY_PAGE_TAP_PAYING,
    ALIPAY_PAGE_TAP_FINISH,
    ALIPAY_PAGE_TAP_INPUT,
    ALIPAY_PAGE_TAP_AMOUNT,
    ALIPAY_PAGE_TAP_CONFIRM,
    ALIPAY_PAGE_TAP_RESULT,
    ALIPAY_PAGE_MAX,
};

class AlipayView : public View<AlipayHsc32I1Presenter> {
public:
    AlipayView() = default;
    AlipayView(const AlipayView &) = delete;
    AlipayView &operator=(const AlipayView &) = delete;
    ~AlipayView() override = default;
};
}
#endif  // ALIPAY_VIEW_H