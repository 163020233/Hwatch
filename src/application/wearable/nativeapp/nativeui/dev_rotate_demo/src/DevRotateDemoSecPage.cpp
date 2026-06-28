/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: DevRotateDemoSecPage
 * Create: 2026-02-12
 */

#include "dev_rotate_demo/DevRotateDemoSecPage.h"
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
static constexpr uint8_t ROT_CAP =
    static_cast<uint8_t>(AppRotationType::ROTATION_0) | static_cast<uint8_t>(AppRotationType::ROTATION_90);

REGIST_SLICE_PAGE(VIEW_DEV_ROTATE_DEMO, DevRotateDemoPresenter::DEV_ROTATE_DEMO_PAGE_SEC, DevRotateDemoSecPage, false);
REG_SLICE_PAGE_ROT_CAP(VIEW_DEV_ROTATE_DEMO, DevRotateDemoPresenter::DEV_ROTATE_DEMO_PAGE_SEC, ROT_CAP);

DevRotateDemoSecPage::DevRotateDemoSecPage()
    : fragment_(Color::Silver(), Color::Red(), DevRotateDemoPresenter::DEV_ROTATE_DEMO_PAGE_SEC,
          DevRotateDemoPresenter::DEV_ROTATE_DEMO_PAGE_MAIN)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "DevRotateDemoSecPage::DevRotateDemoSecPage");
}

DevRotateDemoSecPage::~DevRotateDemoSecPage()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "DevRotateDemoSecPage::~DevRotateDemoSecPage");
}

void DevRotateDemoSecPage::OnStart(void *data)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "DevRotateDemoSecPage OnStart");
    AddViewToPageContainer(fragment_.CreateView());
}

void DevRotateDemoSecPage::OnOrientationChanged(AppRotationType rot)
{
    fragment_.OnRotationChanged(rot);
}
}  // namespace OHOS
