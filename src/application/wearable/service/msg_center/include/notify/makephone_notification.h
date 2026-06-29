/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2025. All rights reserved.
 * Description: makephone_notification
 * Created: 2023-8-11
 */

#ifndef MAKEPHONE_NOTIFICATION_H
#define MAKEPHONE_NOTIFICATION_H

enum class CallTimerType {
    NOTIFY_PHONE_CALLING_DISPLAY = 0,
    NOTIFY_PHONE_CURTIME_DISPLAY = 1,
    NOTIFY_PHONE_UNKNOWN = 10,
};

namespace OHOS {
UIViewGroup* CreatePhoneCallView(void);
UIViewGroup* CreatePhoneCallViewOutGoing(void);
UIViewGroup* CreateCallingView(void);
UIViewGroup* CreateMultiplePhoneView(void);
void FreeCallingView(void);
void FreePhoneCallView(void);
void FreeMultiplePhoneCallView(void);
void FreeWaitingHintsView(void);
void CallTimerCallBackProc(int data);
void EnableWaitingView(void);
void DisableWatingView(void);
void PhoneNotifyLoadVoiceImg(void);
void ResetNoOperationCount(void);

class CallViewEventListener : public OHOS::UIView::OnClickListener,
                              public OHOS::UIView::OnLongPressListener,
                              public OHOS::UIView::OnTouchListener,
                              public OHOS::UIView::OnDragListener {
public:
    CallViewEventListener();
    ~CallViewEventListener() override;
    static CallViewEventListener *GetInstance(void);
    bool OnClick(OHOS::UIView& view, const OHOS::ClickEvent& event) override;
    bool OnDrag(UIView& view, const DragEvent& event) override
    {
        return true;
    }
    bool OnDragEnd(OHOS::UIView& view, const OHOS::DragEvent& event) override
    {
        return true;
    }
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

#endif /* MAKEPHONE_NOTIFICATION_H */
