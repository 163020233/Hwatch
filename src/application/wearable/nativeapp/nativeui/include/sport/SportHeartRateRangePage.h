/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SportHeartRateRangePage
 * Create: 2025-06-06
 */

#ifndef SPORT_HEART_RATE_RANGE_PAGE_H
#define SPORT_HEART_RATE_RANGE_PAGE_H

#include "components/ui_label.h"
#include "components/ui_simple_list.h"
#include "components/ui_scroll_view.h"
#include "SlicePage.h"
#include "UiConfig.h"
#include "ui_view_ids_sport.h"
#include "SportPresenter.h"
#include "SportModel.h"
#include "SportHeartRateRangeItemView.h"

namespace OHOS {
class SportHeartRateRangePage : public SlicePage<SportPresenter>,
    public UIView::OnClickListener,
    public UIView::OnDragListener {
public:
    SportHeartRateRangePage();
    SportHeartRateRangePage(const SportHeartRateRangePage&) = delete;
    SportHeartRateRangePage& operator=(const SportHeartRateRangePage&) = delete;
    ~SportHeartRateRangePage();
    void OnStart(void* data) override;
    bool OnDrag(UIView& view, const DragEvent& event) override;
    bool OnDragStart(UIView &view, const DragEvent &event) override;
    bool OnDragEnd(UIView& view, const DragEvent& event) override;
    bool OnClick(UIView& view, const ClickEvent& event) override;
private:
    void CreateUILabel(void);
    void CreateUIList(void);
    UIScrollView *group_{nullptr};
    UILabel *titleLabel_{nullptr};
    UISimpleList *heartRateRangeList_{nullptr};
    SportHeartRateRangeItem heartRateRanges_[SPORT_HEARTRATE_RANGE_MAX] = {
        {SPORT_HEARTRATE_RANGE_ITEM1, "热身燃脂", UIVIEW_SPORT_UISIMPLELIST_REMIND_HEARTRATE_RANGE_1},
        {SPORT_HEARTRATE_RANGE_ITEM2, "有氧耐力", UIVIEW_SPORT_UISIMPLELIST_REMIND_HEARTRATE_RANGE_2},
        {SPORT_HEARTRATE_RANGE_ITEM3, "无氧耐力", UIVIEW_SPORT_UISIMPLELIST_REMIND_HEARTRATE_RANGE_3},
        {SPORT_HEARTRATE_RANGE_ITEM4, "极限训练", UIVIEW_SPORT_UISIMPLELIST_REMIND_HEARTRATE_RANGE_4}};
};
}  // namespace OHOS
#endif  // SPORT_HEART_RATE_RANGE_PAGE_H