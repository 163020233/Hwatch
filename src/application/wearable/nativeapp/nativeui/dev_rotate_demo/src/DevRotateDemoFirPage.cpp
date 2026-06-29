/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: DevRotateDemoFirPage
 * Create: 2026-02-12
 */

#include "dev_rotate_demo/DevRotateDemoFirPage.h"
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

REGIST_SLICE_PAGE(VIEW_DEV_ROTATE_DEMO, DevRotateDemoPresenter::DEV_ROTATE_DEMO_PAGE_FIR, DevRotateDemoFirPage, false);
REG_SLICE_PAGE_ROT_CAP(VIEW_DEV_ROTATE_DEMO, DevRotateDemoPresenter::DEV_ROTATE_DEMO_PAGE_FIR, ROT_CAP);

DevRotateDemoFirPage::DevRotateDemoFirPage()
    : fragment_(Color::Orange(), Color::Gray(), DevRotateDemoPresenter::DEV_ROTATE_DEMO_PAGE_FIR,
          DevRotateDemoPresenter::DEV_ROTATE_DEMO_PAGE_SEC)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "DevRotateDemoFirPage::DevRotateDemoFirPage");
}

DevRotateDemoFirPage::~DevRotateDemoFirPage()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "DevRotateDemoFirPage::~DevRotateDemoFirPage");
}

void DevRotateDemoFirPage::OnStart(void *data)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "DevRotateDemoFirPage OnStart");
    AddViewToPageContainer(fragment_.CreateView());
}

void DevRotateDemoFirPage::OnOrientationChanged(AppRotationType rot)
{
    fragment_.OnRotationChanged(rot);
}
}  // namespace OHOS
