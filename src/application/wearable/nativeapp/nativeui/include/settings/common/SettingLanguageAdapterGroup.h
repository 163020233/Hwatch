/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingLanguageAdapterGroup
 * Created: 2025-12-15
 */

#ifndef SET_LANGUAGE_ADAPTER_H
#define SET_LANGUAGE_ADAPTER_H

#include <list>
#include <string>
#include "components/ui_list.h"
#include "components/ui_label.h"
#include "components/ui_view_group.h"
#include "components/ui_image_view.h"
#include "components/abstract_adapter.h"
#include "wearable_log.h"
#include "UiConfig.h"
#include "settings/model/SettingLanguageModel.h"
#include "settings/common/SettingLanguageItem.h"
#include "settings/common/SettingCommon.h"

namespace OHOS {
class SettingLanguageAdapterGroup : public AbstractAdapter {
public:
    explicit SettingLanguageAdapterGroup();
    ~SettingLanguageAdapterGroup() override;

    /* *
    * @brief Sets the UIView adapter data, which is a SettingItem linked list.
    *
    * @param data Indicates the SettingItem linked list data.
    * @since 1.0
    * @version 1.0
    */
    void SetData(std::list<SetLanguages> &data);

    /* *
    * @brief Obtains the <b>data</b> size of the UILabel adapter.
    *
    * @return Returns the <b>data</b> size of the UILabel adapter.
    * @since 1.0
    * @version 1.0
    */
    uint16_t GetCount(void) override;

    UIView *GetView(UIView *inView, int16_t index) override;

    void AddListItem(const SetLanguages &item);

    void SetItemClickListener(UIView::OnClickListener *itemclicklistener)
    {
        itemClickListener = itemclicklistener;
    }
private:
    std::list<SetLanguages> listData;
    UIView::OnClickListener *itemClickListener{nullptr};
};
}

#endif