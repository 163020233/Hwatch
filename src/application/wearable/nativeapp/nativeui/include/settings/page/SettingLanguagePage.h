/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingLanguagePage
 * Created: 2025-12-15
 */

#ifndef SETTING_LANGUAGE_PAGE_H
#define SETTING_LANGUAGE_PAGE_H

#include "View.h"
#include "components/ui_list.h"
#include "components/ui_image_view.h"
#include "components/ui_toggle_button.h"
#include "ChangeSliceListener.h"
#include "UiConfig.h"
#include "components/ui_view_group.h"
#include "components/ui_circle_progress.h"
#include "components/ui_scroll_view_nested.h"
#include "components/ui_label.h"
#include "components/ui_button.h"
#include "animator/animator.h"
#include "SlicePage.h"
#include "settings/SettingPresenter.h"
#include "settings/common/SettingCommon.h"
#include "ui_list_nested.h"
#include "settings/common/SettingLanguageAdapterGroup.h"

namespace OHOS {

class SettingLanguagePage : public SlicePage<SettingPresenter>,
                                     public UIView::OnClickListener,
                                     public UIView::OnLongPressListener,
                                     public UIView::OnTouchListener,
                                     public UIView::OnDragListener {
public:
    SettingLanguagePage();
    ~SettingLanguagePage() override;
    void OnStart(void* data) override;
    bool OnDrag(UIView& view, const DragEvent& event) override;
    bool OnClick(UIView& view, const ClickEvent& event) override;
    void RefreshSetLanguageList(const SetLanguages *item, uint8_t count);

private:
    UIScrollView *scrollView_{nullptr};
    UIList *contentList_{nullptr};
    UILabel *title_{nullptr};
    SettingLanguageAdapterGroup *listAdapter_{nullptr};
};
}  // namespace OHOS
#endif  // SET_Language_VIEW_H