/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: WeekStep
 * Author:
 * Create: 2021-09-11
 */

#ifndef PULL_UP_DOWN_VIEW_H
#define PULL_UP_DOWN_VIEW_H

#include "gfx_utils/graphic_types.h"
#include "wearable_log.h"
#include "components/ui_card_page.h"
#include "components/ui_view_group.h"
#include "components/ui_image_view.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_scroll_view.h"
#include "main/PullUpDownControls.h"

namespace OHOS {
#define VERSION_STRING "1.00.23.11"
enum class ConnectState {
    ONLY_PHONE = 1,
    ONLY_BLUETOOTH = 2,
    PHONE_BLUETOOTH = 3
};

class PullUpDownView : public UICardPage,
                       public UIView::OnClickListener,
                       public UIView::OnDragListener {
public:
    PullUpDownView();
    PullUpDownView(const PullUpDownView&) = delete;
    PullUpDownView& operator=(const PullUpDownView&) = delete;
    ~PullUpDownView() override;
    static PullUpDownView *GetInstance(void);
    bool InitView();
    void PreLoad(void) override;
    void UpdatePhoneAndHeadPhoneConnectStatus();
    void Refresh();
private:
    PullupDownScrollView *scroll_{nullptr};
    UIImageView *headPhone_{nullptr};
    UIImageView *phone_{nullptr};
    UIImageView *battery_{nullptr};
    UILabel *batteryLabel_{nullptr};
    UILabel *dateLabel_{nullptr};
    PullUpDownSlider *lightSlider_{nullptr};
    PullUpDownSlider *volumeSlider_{nullptr};
    UILabel *lightLabel_{nullptr};
    UILabel *volumeLabel_{nullptr};
    PullupDownButton *flashLightButton_{nullptr};
    PullupDownButton *deskStyleButton_{nullptr};
    PullupDownButton *dnotDisturButton_{nullptr};
    PullupDownButton *alarmButton_{nullptr};
    PullupDownButton *screenButton_{nullptr};
    PullupDownButton *findPhoneButton_{nullptr};
    PullupDownButton *setButton_{nullptr};
    PullupDownButton *muteButton_{nullptr};
    UILabel *versionLabel_{nullptr};
    bool viewInitStatus_{false};
    void InitViewSlider(void);
    void InitButton(void);
    void InitViewInfo(void);
};
}
#endif
