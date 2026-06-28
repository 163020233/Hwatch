/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SportMistouchPage
 * Create: 2025-06-06
 */

#ifndef SPORT_MISTOUCH_PAGE_H
#define SPORT_MISTOUCH_PAGE_H

#include "components/ui_label.h"
#include "components/ui_toggle_button.h"
#include "components/ui_image_view.h"
#include "components/ui_scroll_view.h"
#include "SlicePage.h"
#include "ui_view_ids_sport.h"
#include "SportPresenter.h"
#include "SportModel.h"

namespace OHOS {
class SportMistouchPage : public SlicePage<SportPresenter>,
    public UIView::OnClickListener,
    public UIView::OnDragListener {
public:
    SportMistouchPage() {}
    SportMistouchPage(const SportMistouchPage&) = delete;
    SportMistouchPage& operator=(const SportMistouchPage&) = delete;
    ~SportMistouchPage();
    void OnStart(void* data) override;
    bool OnDrag(UIView& view, const DragEvent& event) override;
    bool OnDragStart(UIView &view, const DragEvent &event) override;
    bool OnDragEnd(UIView& view, const DragEvent& event) override;
    bool OnClick(UIView& view, const ClickEvent& event) override;
private:
    UIScrollView* group_ = nullptr;
    UILabel* title_ = nullptr;
    UIImageView* bgScreen_ = nullptr;
    UIToggleButton* buttonScreen_ = nullptr;
    UILabel* titleScreen_ = nullptr;
    UIImageView* imageScreen_ = nullptr;
    UIImageView* bgCrown_ = nullptr;
    UIToggleButton* buttonCrown_ = nullptr;
    UILabel* titleCrown_ = nullptr;
    UIImageView* imageCrown_ = nullptr;
    UILabel* textScreen_ = nullptr;
    UILabel* textCrown_ = nullptr;
};
}  // namespace OHOS
#endif  // SPORT_MISTOUCH_PAGE_H