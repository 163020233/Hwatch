/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: PullUpDownControls
 * Created: 2025-11-05
 */
#ifndef PULL_UP_DOWN_CONTROLS
#define PULL_UP_DOWN_CONTROLS

#include "gfx_utils/graphic_types.h"
#include "main/LoadImg.h"
#include "wearable_log.h"
#include "components/ui_scroll_view.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_button.h"
#include "components/ui_fragment.h"
#include "components/ui_slider.h"
#include "audio_manager.h"
#include "main/VolumeSettingPresenter.h"
#include "settings/model/SettingDesktopModel.h"
#include "settings/model/SettingDisplayModel.h"
#include "bts_avrcp_controller.h"
#include "screensetting/ScreenModels.h"
#include "bts_avrcp_target.h"
#include "player/PlayersView.h"
#include "settings/model/SettingBtHeadset.h"

enum class SliderType {
    LIGHT = 0,
    VOLUME = 1,
};

enum class ButtonType {
    APPLIST_STYLE = 0,
    HEXAGONS_STYLE = 1,
    FOOTBALL_STYLE = 2,
    WATERFALL_STYLE = 3,
    PLANET_STYLE = 4,
    SPHERICITY = 5,
    MUSIC = 6
};

namespace OHOS {

class PullupDownScrollView : public UIScrollView {
public:
    bool OnDragStartEvent(const DragEvent& event) override;
    bool OnDragEvent(const DragEvent& event) override;
    bool OnDragEndEvent(const DragEvent& event) override;
};

class PullUpDownSlider : public UIViewGroup,
                         public UIView::OnClickListener,
                         public UIView::OnDragListener,
                         public UISlider::UISliderEventListener {
public:
    PullUpDownSlider(SliderType type);
    PullUpDownSlider(const PullUpDownSlider&) = delete;
    PullUpDownSlider& operator=(const PullUpDownSlider&) = delete;
    ~PullUpDownSlider() override;
    void RefreshFragment();
    void OnChange(int32_t progress) override;
    void OnRelease(int32_t progress) override;
    void VolumeSet(int16_t sounding);

private:
    UIImageView* img_{nullptr};
    UISlider *slider_{nullptr};

    SliderType type_ = SliderType::VOLUME;
};

class PullupDownButton : public UIViewGroup,
                         public UIView::OnClickListener {
public:
    PullupDownButton(int imgSource, const char* text, bool highLight);
    PullupDownButton(const PullupDownButton&) = delete;
    PullupDownButton& operator=(const PullupDownButton&) = delete;
    ~PullupDownButton() override;
    void ChangeState(bool flag);
private:
    UIImageView *img_{nullptr};
    UILabel *textLabel_{nullptr};
};
} // OHOS
#endif