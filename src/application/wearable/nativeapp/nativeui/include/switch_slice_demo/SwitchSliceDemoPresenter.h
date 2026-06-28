/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: SwitchSliceDemoPresenter
 * Create: 2026-02-12
 */

#ifndef SWITCH_SLICE_DEMO_PRESENTER_H
#define SWITCH_SLICE_DEMO_PRESENTER_H

#include "Presenter.h"

namespace OHOS {
class SwitchSliceDemoView;
class SwitchSliceDemoPresenter : public Presenter<SwitchSliceDemoView> {
public:
    static constexpr uint16_t SWITCH_SLICE_DEMO_PAGE_MAIN = 1;
    static constexpr uint16_t SWITCH_SLICE_DEMO_PAGE_FIR = 2;
    static constexpr uint16_t SWITCH_SLICE_DEMO_PAGE_SEC = 3;
};
}

#endif  // SWITCH_SLICE_DEMO_PRESENTER_H