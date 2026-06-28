/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: PauseReasonPresenter
 * Create: 2026-03-18
 */

#ifndef PAUSE_REASON_PRESENTER_H
#define PAUSE_REASON_PRESENTER_H
#include "Presenter.h"

namespace OHOS {
class PauseReasonView;
class PauseReasonPresenter : public Presenter<PauseReasonPresenter> {
public:
    static constexpr uint16_t PAGE_ONE = 1;
    static constexpr uint16_t PAGE_TWO = 2;
};
} // namespace OHOS
#endif // PAUSE_REASON_PRESENTER_H
