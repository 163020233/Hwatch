/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: PhoneMenuCallingVoice
 * Created: 2025-06-05
 */
#ifndef PHONE_MENU_CALLING_VOICE
#define PHONE_MENU_CALLING_VOICE

#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_fragment.h"
#include "components/ui_slider.h"
#include "components/ui_image_view.h"
#include "components/ui_circle_progress.h"
#include "audio_manager.h"

namespace OHOS {
class PhoneMenuCallingVoice : public UIFragment,
                              public UIView::OnClickListener,
                              public UIView::OnDragListener,
                              public UISlider::UISliderEventListener {
public:
    void RefreshFragment();
    bool OnClick(UIView &view, const ClickEvent &event) override;
    void OnChange(int32_t progress) override;
    void OnRelease(int32_t progress) override;
    bool OnDragStart(UIView& view, const DragEvent& event) override;
    bool OnDrag(UIView& view, const DragEvent& event) override;
    bool OnDragEnd(UIView& view, const DragEvent& event) override;
    void VolumeSet(int16_t volume);

protected:
    void OnCreateView(void* data) override;
    void OnDestroyView() override;

private:
    void InitVoiceFragment();
    UISlider *slider_{nullptr};
    UILabel* titleLabel_ = nullptr;
    UILabel* voiceLabel_ = nullptr;
    UIImageView* confirmButton_ = nullptr;
    UIImageView* additionImg_ = nullptr;
    UIImageView* subtractionImg_ = nullptr;
};
} // OHOS
#endif