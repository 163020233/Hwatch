/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: PauseReasonPageOne
 * Create: 2026-03-18
 */

#include "pause_reason_test/PauseReasonPageOne.h"
#include "AppViewIDs.h"
#include "SlicePageFactory.h"

namespace OHOS {
namespace {
const uint16_t CUR_PAGE = PauseReasonPresenter::PAGE_ONE;
}
REGIST_SLICE_PAGE(VIEW_PAUSE_TEST, CUR_PAGE, PauseReasonPageOne, true);

PauseReasonPageOne::PauseReasonPageOne() : fragment_(Color::Blue(), PauseReasonPresenter::PAGE_TWO)
{
}

PauseReasonPageOne::~PauseReasonPageOne()
{
}

void PauseReasonPageOne::OnStart(void* data)
{
    AddViewToPageContainer(fragment_.CreateView());
}

void PauseReasonPageOne::OnPause(PauseReason reason)
{
    printf("PauseReasonPageOne::OnPause reason %d\n", static_cast<int16_t>(reason));
}
} // namespace OHOS
