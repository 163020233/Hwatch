/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: AlipayHsc32I1Presenter
 * Create: 2025-06-06
 */

#ifndef ALIPAY_HSC32I1_PRESENTER_H
#define ALIPAY_HSC32I1_PRESENTER_H

#include "Presenter.h"

namespace OHOS {
class AlipayView;
class AlipayHsc32I1Presenter : public Presenter<AlipayView> {
public:
    AlipayHsc32I1Presenter() = default;
    AlipayHsc32I1Presenter(const AlipayHsc32I1Presenter &) = delete;
    AlipayHsc32I1Presenter &operator=(const AlipayHsc32I1Presenter &) = delete;
    ~AlipayHsc32I1Presenter() override = default;
    void OnResume() override;
    void OnPause() override;
};
}

#endif  // ALIPAY_HSC32I1_PRESENTER_H