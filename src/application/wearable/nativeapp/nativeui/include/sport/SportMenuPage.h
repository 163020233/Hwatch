/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SportMenuPage
 * Create: 2025-06-06
 */

#ifndef SPORT_MENU_PAGE_H
#define SPORT_MENU_PAGE_H

#include "components/ui_label.h"
#include "components/ui_simple_list.h"
#include "components/ui_image_view.h"
#include "components/ui_scroll_view.h"
#include "ui_resource_sport.h"
#include "ui_view_ids_sport.h"
#include "SlicePage.h"
#include "SportPresenter.h"
#include "SportModel.h"
#include "SportMenuItemView.h"

namespace OHOS {
class SportMenuPage : public SlicePage<SportPresenter>,
    public UIView::OnClickListener,
    public UIView::OnDragListener {
public:
    SportMenuPage();
    SportMenuPage(const SportMenuPage&) = delete;
    SportMenuPage& operator=(const SportMenuPage&) = delete;
    ~SportMenuPage();
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
    UISimpleList *sportList_{nullptr};
    SportMenuItem sportMenu_[SPORT_TYPE_MAX] = {
        {IMAGE_SPORT_ITEMBG1, IMAGE_SPORT_OUTDOOR_RUNNING1, "户外跑步", UIVIEW_SPORT_UISIMPLELIST_OUTDOOR_RUNNING},
        {IMAGE_SPORT_ITEMBG2, IMAGE_SPORT_INDOOR_RUNNING1, "室内跑步", UIVIEW_SPORT_UISIMPLELIST_INDOOR_RUNNING},
        {IMAGE_SPORT_ITEMBG3, IMAGE_SPORT_CYCLING1, "骑行", UIVIEW_SPORT_UISIMPLELIST_CYCLING},
        {IMAGE_SPORT_ITEMBG4, IMAGE_SPORT_CLIMBING1, "登山", UIVIEW_SPORT_UISIMPLELIST_CLIMBING},
        {IMAGE_SPORT_ITEMBG6, IMAGE_SPORT_INDOOR_SWIMMING1, "室内游泳", UIVIEW_SPORT_UISIMPLELIST_INDOOR_SWIMING},
        {IMAGE_SPORT_ITEMBG5, IMAGE_SPORT_OUTDOOR_SWIMMING1, "户外游泳", UIVIEW_SPORT_UISIMPLELIST_OUTDOOR_SWIMING}};
};
}  // namespace OHOS
#endif  // SPORT_MENU_PAGE_H