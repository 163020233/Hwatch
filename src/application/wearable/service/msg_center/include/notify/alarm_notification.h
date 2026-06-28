/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: alarm notification api
 * Author: CompanyName
 * Create: 2023-08-11
 */

#ifndef ALARM_NOTIFICATION_H
#define ALARM_NOTIFICATION_H

enum class AlarmTimerType {
    NOTIFY_ALARM_CLOSE = 0,
    NOTIFY_ALARM_DELAY = 1,
    NOTIFY_ALARM_RING = 2,
    NOTIFY_ALARM_UNKNOWN = 10,
};

namespace OHOS {
UIViewGroup* CreateAlarmView(void);
UIViewGroup *CreateAlarmCloseView(void);
UIViewGroup *CreateAlarmRingView(void);
void FreeAlarmCloseView(void);
void FreeAlarmRingView(void);
void FreeAlarmView(void);
void StartAlarmTimer(AlarmTimerType type, int32_t interval);
void DisableAlarmTimer(AlarmTimerType type);

class AlarmViewEventListener : public OHOS::UIView::OnClickListener,
                               public OHOS::UIView::OnLongPressListener,
                               public OHOS::UIView::OnTouchListener,
                               public OHOS::UIView::OnDragListener {
public:
    AlarmViewEventListener();
    virtual ~AlarmViewEventListener();
    static AlarmViewEventListener *GetInstance(void);
    bool OnDrag(UIView& view, const DragEvent& event) override;
    bool OnDragEnd(OHOS::UIView& view, const OHOS::DragEvent& event) override;
    bool OnClick(OHOS::UIView& view, const OHOS::ClickEvent& event) override;
    bool OnPress(OHOS::UIView& view, const OHOS::PressEvent& event) override
    {
        return true;
    }
    bool OnLongPress(OHOS::UIView& view, const OHOS::LongPressEvent& event) override
    {
        return true;
    }
    bool OnRelease(OHOS::UIView& view, const OHOS::ReleaseEvent& event) override
    {
        return true;
    }

    bool OnDragStart(OHOS::UIView& view, const OHOS::DragEvent& event) override
    {
        return true;
    }
};
}; // namespace OHOS

#endif /* ALARM_NOTIFICATION_H */
