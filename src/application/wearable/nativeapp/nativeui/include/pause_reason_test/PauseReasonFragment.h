/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: PauseReasonFragment
 * Create: 2026-03-18
 */

#ifndef PAUSE_REASON_FRAGMENT_H
#define PAUSE_REASON_FRAGMENT_H
#include "components/ui_fragment.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"

namespace OHOS {
class PauseReasonFragment : public UIFragment, public UIView::OnClickListener {
public:
    PauseReasonFragment(ColorType color, uint16_t dstPage);
    ~PauseReasonFragment() override;
    PauseReasonFragment(const PauseReasonFragment&) = delete;
    PauseReasonFragment &operator=(const PauseReasonFragment&) = delete;
    bool OnClick(UIView& view, const ClickEvent& event) override;

private:
    void OnCreateView(void* data) override;
    void OnDestroyView() override;
    bool InitViews();
    void DeinitViews();
    void Layout();
    void InitText(UILabelButton &btn, const char *txt);

    UILabelButton *btnExit_ = nullptr;
    UILabelButton *btnPageSwitch_ = nullptr;
    UILabelButton *btnPageSwitchWithAni_ = nullptr;
    UILabelButton *btnNotify_ = nullptr;
    UILabelButton *btnBack_ = nullptr;
    ColorType color_;
    uint16_t dstPage_ = 0;
};
} // namespace OHOS
#endif // PAUSE_REASON_FRAGMENT_H
