/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: PauseReasonPageTwo
 * Create: 2026-03-18
 */

#ifndef PAUSE_REASON_PAGE_TWO_H
#define PAUSE_REASON_PAGE_TWO_H
#include "SlicePage.h"
#include "PauseReasonPresenter.h"
#include "PauseReasonFragment.h"

namespace OHOS {
class PauseReasonPageTwo : public SlicePage<PauseReasonPageTwo> {
public:
    PauseReasonPageTwo();
    ~PauseReasonPageTwo() override;
    void OnStart(void* data) override;
    void OnPause(PauseReason reason) override;

private:
    PauseReasonFragment fragment_;
};
} // namespace OHOS
#endif // PAUSE_REASON_PAGE_TWO_H
