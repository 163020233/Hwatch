/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SportTargetSettingPage
 * Create: 2025-06-06
 */

#ifndef SPORT_TARGET_SETTING_PAGE_H
#define SPORT_TARGET_SETTING_PAGE_H

#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_view_group.h"
#include "components/ui_image_view.h"
#include "components/ui_picker.h"
#include "SlicePage.h"
#include "ui_view_ids_sport.h"
#include "SportPresenter.h"
#include "SportModel.h"

namespace OHOS {
class SportTargetSettingPage : public SlicePage<SportPresenter>,
    public UIView::OnClickListener,
    public UIView::OnDragListener,
    public UIPicker::SelectedListener {
public:
    SportTargetSettingPage() {}
    SportTargetSettingPage(const SportTargetSettingPage&) = delete;
    SportTargetSettingPage& operator=(const SportTargetSettingPage&) = delete;
    ~SportTargetSettingPage();
    void OnStart(void* data) override;
    bool OnDrag(UIView& view, const DragEvent& event) override;
    bool OnDragStart(UIView &view, const DragEvent &event) override;
    bool OnDragEnd(UIView& view, const DragEvent& event) override;
    bool OnClick(UIView& view, const ClickEvent& event) override;
private:
    void UpdateViewByTargetType();
    void UpdateViewByRemindType();
    void CreateUILabel(void);
    void CreateUIPicker(void);
    void CreateUIButton(void);
    UIViewGroup *group_{nullptr};
    UILabel *titleLabel_{nullptr};
    UIPicker *picker_{nullptr};
    UIImageView *confirmButton_{nullptr};
};
}  // namespace OHOS
#endif  // SPORT_TARGET_SETTING_PAGE_H