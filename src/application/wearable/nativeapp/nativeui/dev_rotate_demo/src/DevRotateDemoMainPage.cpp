/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: DevRotateDemoMainPage
 * Create: 2026-02-12
 */

#include "dev_rotate_demo/DevRotateDemoMainPage.h"
#include "dev_rotate_demo/DevRotateDemoPresenter.h"
#include "UiConfig.h"
#include "main/LoadImg.h"
#include "ui_resource_empty.h"
#include "SlicePageFactory.h"
#include "wearable_log.h"
#include "AppViewIDs.h"
#include "NativeAbility.h"
#include "RotationManager.h"

namespace OHOS {
static constexpr uint8_t ROT_CAP = static_cast<uint8_t>(AppRotationType::ROTATION_0);

REGIST_SLICE_PAGE(VIEW_DEV_ROTATE_DEMO, DevRotateDemoPresenter::DEV_ROTATE_DEMO_PAGE_MAIN, DevRotateDemoMainPage, true);
REG_SLICE_PAGE_ROT_CAP(VIEW_DEV_ROTATE_DEMO, DevRotateDemoPresenter::DEV_ROTATE_DEMO_PAGE_MAIN, ROT_CAP);

DevRotateDemoMainPage::DevRotateDemoMainPage()
    : fragment_(Color::White(), Color::Blue(), DevRotateDemoPresenter::DEV_ROTATE_DEMO_PAGE_MAIN,
          DevRotateDemoPresenter::DEV_ROTATE_DEMO_PAGE_FIR)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "DevRotateDemoMainPage::DevRotateDemoMainPage");
}

DevRotateDemoMainPage::~DevRotateDemoMainPage()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "DevRotateDemoMainPage::~DevRotateDemoMainPage");
}

void DevRotateDemoMainPage::OnStart(void *data)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "DevRotateDemoMainPage OnStart");
    AddViewToPageContainer(fragment_.CreateView());
}
}  // namespace OHOS
