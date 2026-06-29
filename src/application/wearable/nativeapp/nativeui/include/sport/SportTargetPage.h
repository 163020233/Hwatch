/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SportTargetPage
 * Create: 2025-06-06
 */

#ifndef SPORT_TARGET_PAGE_H
#define SPORT_TARGET_PAGE_H

#include "components/ui_label.h"
#include "components/ui_simple_list.h"
#include "components/ui_image_view.h"
#include "components/ui_scroll_view.h"
#include "SlicePage.h"
#include "ui_resource_sport.h"
#include "ui_view_ids_sport.h"
#include "SportPresenter.h"
#include "SportModel.h"
#include "SportTargetItemView.h"

namespace OHOS {
class SportTargetPage : public SlicePage<SportPresenter>,
    public UIView::OnClickListener,
    public UIView::OnDragListener {
public:
    SportTargetPage();
    SportTargetPage(const SportTargetPage&) = delete;
    SportTargetPage& operator=(const SportTargetPage&) = delete;
    ~SportTargetPage();
    void OnStart(void* data) override;
    bool OnDrag(UIView& view, const DragEvent& event) override;
    bool OnDragStart(UIView &view, const DragEvent &event) override;
    bool OnDragEnd(UIView& view, const DragEvent& event) override;
    bool OnClick(UIView& view, const ClickEvent& event) override;
private:
    void InitBackgroundView();
    void CreateUILabel(void);
    void CreateUIList(void);
    UIScrollView *group_{nullptr};
    UIImageView *background_{nullptr};
    UILabel *titleLabel_{nullptr};
    UISimpleList *targetList_{nullptr};
    SportTargetItem targets_[SPORT_TARGET_MAX] = {
        {SPORT_TARGET_DISTANCE, IMAGE_SPORT_DISTANCE, "距离", UIVIEW_SPORT_UISIMPLELIST_TARGET_DISTANCE},
        {SPORT_TARGET_TIME, IMAGE_SPORT_TIME, "时间", UIVIEW_SPORT_UISIMPLELIST_TARGET_TIME},
        {SPORT_TARGET_CALORIE, IMAGE_SPORT_CALORIE, "热量", UIVIEW_SPORT_UISIMPLELIST_TARGET_CALORIE}};
};
}  // namespace OHOS
#endif  // SPORT_TARGET_PAGE_H