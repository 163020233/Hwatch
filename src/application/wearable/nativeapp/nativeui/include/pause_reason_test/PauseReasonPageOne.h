/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: PauseReasonPageOne
 * Create: 2026-03-18
 */

#ifndef PAUSE_REASON_PAGE_ONE_H
#define PAUSE_REASON_PAGE_ONE_H
#include "SlicePage.h"
#include "PauseReasonPresenter.h"
#include "PauseReasonFragment.h"

namespace OHOS {
class PauseReasonPageOne : public SlicePage<PauseReasonPageOne> {
public:
    PauseReasonPageOne();
    ~PauseReasonPageOne() override;
    void OnStart(void* data) override;
    void OnPause(PauseReason reason) override;

private:
    PauseReasonFragment fragment_;
};
} // namespace OHOS
#endif // PAUSE_REASON_PAGE_ONE_H
