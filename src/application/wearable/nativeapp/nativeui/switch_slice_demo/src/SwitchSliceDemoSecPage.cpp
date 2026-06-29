/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: SwitchSliceDemoSecPage
 * Create: 2026-02-12
 */

#include "switch_slice_demo/SwitchSliceDemoSecPage.h"
#include "switch_slice_demo/SwitchSliceDemoPresenter.h"
#include "UiConfig.h"
#include "main/LoadImg.h"
#include "ui_resource_empty.h"
#include "SlicePageFactory.h"
#include "wearable_log.h"
#include "AppViewIDs.h"
#include "NativeAbility.h"

namespace OHOS {

REGIST_SLICE_PAGE(VIEW_SWITCH_SLICE_DEMOA, SwitchSliceDemoPresenter::SWITCH_SLICE_DEMO_PAGE_SEC, SwitchSliceDemoSecPage, false);
REGIST_SLICE_PAGE(VIEW_SWITCH_SLICE_DEMOB, SwitchSliceDemoPresenter::SWITCH_SLICE_DEMO_PAGE_SEC, SwitchSliceDemoSecPage, false);
REGIST_SLICE_PAGE(VIEW_SWITCH_SLICE_DEMOC, SwitchSliceDemoPresenter::SWITCH_SLICE_DEMO_PAGE_SEC, SwitchSliceDemoSecPage, false);
REGIST_SLICE_PAGE(VIEW_SWITCH_SLICE_DEMOD, SwitchSliceDemoPresenter::SWITCH_SLICE_DEMO_PAGE_SEC, SwitchSliceDemoSecPage, false);

SwitchSliceDemoSecPage::SwitchSliceDemoSecPage()
    : fragment_(Color::Silver(), Color::Red(), SwitchSliceDemoPresenter::SWITCH_SLICE_DEMO_PAGE_SEC,
          SwitchSliceDemoPresenter::SWITCH_SLICE_DEMO_PAGE_MAIN, SwitchSliceDemoPresenter::SWITCH_SLICE_DEMO_PAGE_FIR)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SwitchSliceDemoSecPage::SwitchSliceDemoSecPage");
}

SwitchSliceDemoSecPage::~SwitchSliceDemoSecPage()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SwitchSliceDemoSecPage::~SwitchSliceDemoSecPage");
}

void SwitchSliceDemoSecPage::OnStart(void *data)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SwitchSliceDemoSecPage OnStart");
    AddViewToPageContainer(fragment_.CreateView(data));
}
}  // namespace OHOS
