/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingLanguageItem
 * Created: 2025-12-15
 */

#ifndef SETTING_LANGUAGES_VIEW_H
#define SETTING_LANGUAGES_VIEW_H

#include "components/ui_label.h"
#include "components/ui_view_group.h"
#include "components/ui_image_view.h"
#include "SettingCommon.h"

#include "UiConfig.h"

namespace OHOS {
class SettingLanguageItem : public UIViewGroup {
public:
    explicit SettingLanguageItem();
    ~SettingLanguageItem() override;
    virtual void SetLanguagesInfo(const SetLanguages itemInfo);
    virtual void SetUpChild();
    LanguageId GetViewId();
private:
    UILabel sLabel;
    SetLanguages sItemInfo = { INVALID_ID, nullptr};
};
}

#endif