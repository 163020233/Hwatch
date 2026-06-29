/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SportRemindHeartRatePage
 * Create: 2025-06-06
 */

#ifndef SPORT_REMIND_HEART_RATE_PAGE_H
#define SPORT_REMIND_HEART_RATE_PAGE_H

#include "components/ui_label.h"
#include "components/ui_simple_list.h"
#include "components/ui_view_group.h"
#include "UiConfig.h"
#include "ui_resource_sport.h"
#include "ui_view_ids_sport.h"
#include "SlicePage.h"
#include "SportPresenter.h"
#include "SportModel.h"
#include "SportRemindHeartRateItemView.h"

namespace OHOS {
class SportRemindHeartRatePage : public SlicePage<SportPresenter>,
    public UIView::OnClickListener,
    public UIView::OnDragListener {
public:
    SportRemindHeartRatePage();
    SportRemindHeartRatePage(const SportRemindHeartRatePage&) = delete;
    SportRemindHeartRatePage& operator=(const SportRemindHeartRatePage&) = delete;
    ~SportRemindHeartRatePage();
    void OnStart(void* data) override;
    bool OnDrag(UIView& view, const DragEvent& event) override;
    bool OnDragStart(UIView &view, const DragEvent &event) override;
    bool OnDragEnd(UIView& view, const DragEvent& event) override;
    bool OnClick(UIView& view, const ClickEvent& event) override;
private:
    void CreateUILabel(void);
    void CreateUIList(void);
    UIViewGroup *group_{nullptr};
    UILabel *titleLabel_{nullptr};
    UISimpleList *remindHeartRateList_{nullptr};
    SportRemindHeartRateItem remindHeartRates_[SPORT_REMIND_HEARTRATE_MAX] = {
        {SPORT_REMIND_HEARTRATE_ENABLE, IMAGE_SPORT_HEARTRATE, "心率提醒", UIVIEW_SPORT_UISIMPLELIST_REMIND_HEARTRATE_SWITCH},
        {SPORT_REMIND_HEARTRATE_RANGE, IMAGE_SPORT_HEARTRATE, "心率范围", UIVIEW_SPORT_UISIMPLELIST_REMIND_HEARTRATE_RANGE}};
};
}  // namespace OHOS
#endif  // SPORT_REMIND_HEART_RATE_PAGE_H