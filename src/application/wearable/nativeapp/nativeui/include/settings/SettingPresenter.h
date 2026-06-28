/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingPresenter
 * Create: 2025-06-01
 */
#ifndef SETTING_PRESENTER_H
#define SETTING_PRESENTER_H

#include "Presenter.h"
#include "NativeRegisterManager.h"
#include "settings/common/SettingCommon.h"
#include "settings/SettingView.h"
#include "settings/SettingPresenter.h"

namespace OHOS {
class SettingView;
class SettingPresenter : public Presenter<SettingView> {
public:
    ~SettingPresenter() override
    {
        ImageCacheManager::GetInstance().UnloadAllInMultiRes(SETTING_IMAGE);
    }
};
}
#endif // SETTING_PRESENTER_H