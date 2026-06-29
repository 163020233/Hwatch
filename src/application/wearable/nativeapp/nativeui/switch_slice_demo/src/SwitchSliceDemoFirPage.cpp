/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: SwitchSliceDemoFirPage
 * Create: 2026-03-12
 */

#include "switch_slice_demo/SwitchSliceDemoFirPage.h"
#include "switch_slice_demo/SwitchSliceDemoPresenter.h"
#include "UiConfig.h"
#include "main/LoadImg.h"
#include "ui_resource_empty.h"
#include "SlicePageFactory.h"
#include "wearable_log.h"
#include "AppViewIDs.h"
#include "NativeAbility.h"

namespace OHOS {

REGIST_SLICE_PAGE(VIEW_SWITCH_SLICE_DEMOA, SwitchSliceDemoPresenter::SWITCH_SLICE_DEMO_PAGE_FIR, SwitchSliceDemoFirPage, false);
REGIST_SLICE_PAGE(VIEW_SWITCH_SLICE_DEMOB, SwitchSliceDemoPresenter::SWITCH_SLICE_DEMO_PAGE_FIR, SwitchSliceDemoFirPage, false);
REGIST_SLICE_PAGE(VIEW_SWITCH_SLICE_DEMOC, SwitchSliceDemoPresenter::SWITCH_SLICE_DEMO_PAGE_FIR, SwitchSliceDemoFirPage, false);
REGIST_SLICE_PAGE(VIEW_SWITCH_SLICE_DEMOD, SwitchSliceDemoPresenter::SWITCH_SLICE_DEMO_PAGE_FIR, SwitchSliceDemoFirPage, false);

SwitchSliceDemoFirPage::SwitchSliceDemoFirPage()
    : fragment_(Color::Orange(), Color::Gray(), SwitchSliceDemoPresenter::SWITCH_SLICE_DEMO_PAGE_FIR,
          SwitchSliceDemoPresenter::SWITCH_SLICE_DEMO_PAGE_SEC, SwitchSliceDemoPresenter::SWITCH_SLICE_DEMO_PAGE_MAIN)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SwitchSliceDemoFirPage::SwitchSliceDemoFirPage");
}

SwitchSliceDemoFirPage::~SwitchSliceDemoFirPage()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SwitchSliceDemoFirPage::~SwitchSliceDemoFirPage");
}

void SwitchSliceDemoFirPage::OnStart(void *data)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SwitchSliceDemoFirPage OnStart");
    AddViewToPageContainer(fragment_.CreateView(data));
}
}  // namespace OHOS
