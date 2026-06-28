/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingLanguageAdapterGroup.cpp
 * Create: 2025-09-20
 */

#include "settings/common/SettingLanguageAdapterGroup.h"

namespace OHOS {
SettingLanguageAdapterGroup::SettingLanguageAdapterGroup()
{}

SettingLanguageAdapterGroup::~SettingLanguageAdapterGroup()
{
    listData.clear();
}

uint16_t SettingLanguageAdapterGroup::GetCount(void)
{
    return listData.size();
}

void SettingLanguageAdapterGroup::SetData(std::list<SetLanguages> &data)
{
    if (!listData.empty()) {
        listData.clear();
    }
    std::list<SetLanguages>::iterator it;
    for (it = data.begin(); it != data.end(); it++) {
        listData.push_back(*it);
    }
}

UIView *SettingLanguageAdapterGroup::GetView(UIView *inView, int16_t index)
{
    if (listData.empty()) {
        return nullptr;
    }
    if (index < 0) {
        return nullptr;
    }
    size_t uindex = index;
    if (uindex >= listData.size()) {
        return nullptr;
    }

    std::list<SetLanguages>::iterator it = listData.begin();
    for (int i = 0; i < index; i++) {
        it++;
    }
    SetLanguages itemData = *it;
    SettingLanguageItem *item = nullptr;
    if (inView == nullptr) {
        item = new SettingLanguageItem();
        if (item == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "malloc fail, %s, %d\r\n", __func__, __LINE__);
            return nullptr;
        }
    } else {
        item = static_cast<SettingLanguageItem *>(inView);
    }

    item->SetLanguagesInfo(itemData);
    item->SetTouchable(true);

    item->SetOnClickListener(itemClickListener);
    return item;
}

void SettingLanguageAdapterGroup::AddListItem(const SetLanguages &item)
{
    listData.push_back(item);
}
}  // namespace OHOS