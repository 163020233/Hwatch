/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: MapResultContentItemView
 * Created: 2026-01-14
 */

#ifndef MAP_RESULT_CONTENT_ITEM_VIEW_H
#define MAP_RESULT_CONTENT_ITEM_VIEW_H

#include "components/ui_label_button.h"
#include "components/ui_view_group.h"
#include "MapModel.h"

namespace OHOS {

static constexpr const char *RESULT_LISTITEM_ID = "resultlistitem";

class MapResultContentItemView : public UIViewGroup {
public:
    MapResultContentItemView();
    ~MapResultContentItemView() override;

    void SetItemInfo(const MapResultItem &itemInfo);

private:
    UILabelButton labelButton_;
};

}  // namespace OHOS

#endif  // MAP_RESULT_CONTENT_ITEM_VIEW_H
