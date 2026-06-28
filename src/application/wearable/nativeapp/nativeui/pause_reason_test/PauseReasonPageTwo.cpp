/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: PauseReasonPageTwo
 * Create: 2026-03-18
 */

#include "pause_reason_test/PauseReasonPageTwo.h"
#include "AppViewIDs.h"
#include "SlicePageFactory.h"

namespace OHOS {
namespace {
const uint16_t CUR_PAGE = PauseReasonPresenter::PAGE_TWO;
}
REGIST_SLICE_PAGE(VIEW_PAUSE_TEST, CUR_PAGE, PauseReasonPageTwo, false);

PauseReasonPageTwo::PauseReasonPageTwo() : fragment_(Color::Green(), PauseReasonPresenter::PAGE_ONE)
{
}

PauseReasonPageTwo::~PauseReasonPageTwo()
{
}

void PauseReasonPageTwo::OnStart(void* data)
{
    AddViewToPageContainer(fragment_.CreateView());
}

void PauseReasonPageTwo::OnPause(PauseReason reason)
{
    printf("PauseReasonPageTwo::OnPause reason %d\n", static_cast<int16_t>(reason));
}
} // namespace OHOS
