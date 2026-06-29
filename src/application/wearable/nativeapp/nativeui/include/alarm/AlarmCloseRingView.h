/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: Alarm
 * Author:
 * Create: 2021-09-11
 */

#ifndef ALARM_CLOSE_RING_VIEW_H
#define ALARM_CLOSE_RING_VIEW_H

#include "components/ui_label.h"
#include "components/ui_view_group.h"
#include "components/ui_image_view.h"
#include "components/ui_button.h"
#include "components/ui_label_button.h"
#include "UiConfig.h"
#include "alarm/AlarmClockModel.h"
#include "ChangeSliceListener.h"

namespace OHOS {
class AlarmCloseRingView {
public:
    AlarmCloseRingView();
    AlarmCloseRingView(const AlarmCloseRingView&) = delete;
    AlarmCloseRingView& operator=(const AlarmCloseRingView&) = delete;
    virtual ~AlarmCloseRingView();
    static AlarmCloseRingView *GetInstance(void);
    UIViewGroup *InitCloseRingView(void);
    void AlarmCloseShowTimer(void);
    void CancelCloseHandle(void);
    UIViewGroup *group{nullptr};
private:
    UIImageView *imageClose{nullptr};
    UILabel *label{nullptr};
};
} // namespace OHOS
#endif