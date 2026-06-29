/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: DevRotateDemoPresenter
 * Create: 2026-02-12
 */

#ifndef DEV_ROTATE_DEMO_PRESENTER_H
#define DEV_ROTATE_DEMO_PRESENTER_H

#include "Presenter.h"

namespace OHOS {
class DevRotateDemoView;
class DevRotateDemoPresenter : public Presenter<DevRotateDemoView> {
public:
    static constexpr uint16_t DEV_ROTATE_DEMO_PAGE_MAIN = 1;
    static constexpr uint16_t DEV_ROTATE_DEMO_PAGE_FIR = 2;
    static constexpr uint16_t DEV_ROTATE_DEMO_PAGE_SEC = 3;
};
}

#endif  // DEV_ROTATE_DEMO_PRESENTER_H