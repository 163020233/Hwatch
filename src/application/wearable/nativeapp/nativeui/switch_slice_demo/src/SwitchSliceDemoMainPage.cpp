/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: SwitchSliceDemoMainPage
 * Create: 2026-02-12
 */

#include "switch_slice_demo/SwitchSliceDemoMainPage.h"
#include "switch_slice_demo/SwitchSliceDemoPresenter.h"
#include "UiConfig.h"
#include "main/LoadImg.h"
#include "ui_resource_empty.h"
#include "SlicePageFactory.h"
#include "wearable_log.h"
#include "AppViewIDs.h"
#include "NativeAbility.h"

namespace OHOS {

REGIST_SLICE_PAGE(VIEW_SWITCH_SLICE_DEMOA, SwitchSliceDemoPresenter::SWITCH_SLICE_DEMO_PAGE_MAIN, SwitchSliceDemoMainPage, true);
REGIST_SLICE_PAGE(VIEW_SWITCH_SLICE_DEMOB, SwitchSliceDemoPresenter::SWITCH_SLICE_DEMO_PAGE_MAIN, SwitchSliceDemoMainPage, true);
REGIST_SLICE_PAGE(VIEW_SWITCH_SLICE_DEMOC, SwitchSliceDemoPresenter::SWITCH_SLICE_DEMO_PAGE_MAIN, SwitchSliceDemoMainPage, true);
REGIST_SLICE_PAGE(VIEW_SWITCH_SLICE_DEMOD, SwitchSliceDemoPresenter::SWITCH_SLICE_DEMO_PAGE_MAIN, SwitchSliceDemoMainPage, true);

SwitchSliceDemoMainPage::SwitchSliceDemoMainPage()
    : fragment_(Color::White(), Color::Blue(), SwitchSliceDemoPresenter::SWITCH_SLICE_DEMO_PAGE_MAIN,
          SwitchSliceDemoPresenter::SWITCH_SLICE_DEMO_PAGE_FIR, SwitchSliceDemoPresenter::SWITCH_SLICE_DEMO_PAGE_SEC)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SwitchSliceDemoMainPage::SwitchSliceDemoMainPage");
}

SwitchSliceDemoMainPage::~SwitchSliceDemoMainPage()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SwitchSliceDemoMainPage::~SwitchSliceDemoMainPage");
}

void SwitchSliceDemoMainPage::OnStart(void *data)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SwitchSliceDemoMainPage OnStart");
    AddViewToPageContainer(fragment_.CreateView(data));
}
}  // namespace OHOS
