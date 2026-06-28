/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: Alarm
 * Author:
 * Create: 2021-09-11
 */

#ifndef ALARM_PRESENTER_H
#define ALARM_PRESENTER_H

#include "Presenter.h"
#include "ChangeSliceListener.h"
#include "UiConfig.h"
#include "components/ui_label.h"
#include "common/screen.h"
#include "components/ui_label.h"
#include "components/root_view.h"
#include "components/ui_view_group.h"
#include "components/ui_checkbox.h"
#include "components/ui_image_view.h"
#include "gfx_utils/graphic_types.h"
#include "components/ui_button.h"
#include "components/ui_label_button.h"
#include "components/ui_time_picker.h"
#include "components/ui_digital_clock.h"
#include "gfx_utils/graphic_log.h"
#include "alarm/AlarmClockModel.h"
#include "wearable_log.h"
#include "color.h"
#include "ui_view_ids_alarm.h"

namespace OHOS {
constexpr char* EVERY_DAY = (char *)"每天";
constexpr char* DO_NOT_REPEAT = (char *)"不重复";

constexpr char* MON_DAY = (char *)"一 ";
constexpr char* TUE_DAY = (char *)"二 ";
constexpr char* WED_DAY = (char *)"三 ";
constexpr char* THUR_DAY = (char *)"四 ";
constexpr char* FRI_DAY = (char *)"五 ";
constexpr char* SAT_DAY = (char *)"六 ";
constexpr char* SUN_DAY = (char *)"日";

constexpr int16_t SET_SIX = 6;

constexpr const char* ALARM_STATUS[6] = {
    UIVIEW_ALARM_UITOGGLEBUTTON_CLOCK0_STATUS_BUTTON,
    UIVIEW_ALARM_UITOGGLEBUTTON_CLOCK1_STATUS_BUTTON,
    UIVIEW_ALARM_UITOGGLEBUTTON_CLOCK2_STATUS_BUTTON,
    UIVIEW_ALARM_UITOGGLEBUTTON_CLOCK3_STATUS_BUTTON,
    UIVIEW_ALARM_UITOGGLEBUTTON_CLOCK4_STATUS_BUTTON,
    UIVIEW_ALARM_UITOGGLEBUTTON_CLOCK5_STATUS_BUTTON
    }; // 闹钟按钮最大个数为6个
constexpr const char* ALARM_CLOCK[6] = {
    UIVIEW_ALARM_UIDIGITALCLOCK_CLOCK0_BUTTON,
    UIVIEW_ALARM_UIDIGITALCLOCK_CLOCK1_BUTTON,
    UIVIEW_ALARM_UIDIGITALCLOCK_CLOCK2_BUTTON,
    UIVIEW_ALARM_UIDIGITALCLOCK_CLOCK3_BUTTON,
    UIVIEW_ALARM_UIDIGITALCLOCK_CLOCK4_BUTTON,
    UIVIEW_ALARM_UIDIGITALCLOCK_CLOCK5_BUTTON
    }; // 闹钟个数最大个数为6个
constexpr const char* WEEK_CONTAINER[7] = {
    UIVIEW_ALARM_UILABELBUTTON_SUNDAY_BUTTON,
    UIVIEW_ALARM_UILABELBUTTON_MONDAY_BUTTON,
    UIVIEW_ALARM_UILABELBUTTON_TUESDAY_BUTTON,
    UIVIEW_ALARM_UILABELBUTTON_WEDNESDAY_BUTTON,
    UIVIEW_ALARM_UILABELBUTTON_THURSDAY_BUTTON,
    UIVIEW_ALARM_UILABELBUTTON_FRIDAY_BUTTON,
    UIVIEW_ALARM_UILABELBUTTON_SATURDAY_BUTTON
    }; // 日期按钮最大个数为7个

typedef enum {
    MAIN_INTERFACE = 0,
    CHANGE_INTERFACE_CONFIG,
    CHANGE_INTERFACE_SET_TIME,
    CHANGE_INTERFACE_SET_WEEK,
    CHANGE_INTERFACE_JUDGE,
    CHANGE_INTERFACE_NEW_TIME,
    CHANGE_INTERFACE_MAXCLOCK,
} InterfacesId;

class AlarmStatusBtnChangeListener : public OHOS::UICheckBox::OnChangeListener {
public:
    explicit AlarmStatusBtnChangeListener(UIView *uiView)
    {
        std::string alarmStatusId = uiView->GetViewId();
        std::string alarmCmpId;
        for (int16_t num = 0; num < SET_SIX; num++) { // 闹钟按钮的最大个数为6个
            alarmCmpId = ALARM_STATUS[num];
            if (alarmStatusId == alarmCmpId) {
                alarmStatusNum = num;
            }
        }
    }

    ~AlarmStatusBtnChangeListener() {}
    bool OnChange(UICheckBox::UICheckBoxState state) override;
private:
    int16_t alarmStatusNum;
};

class MainAlarmView;
class AlarmPresenter : public Presenter<MainAlarmView>, public UIView::OnClickListener,
 public UITimePicker::SelectedListener, public UIView::OnDragListener {
public:
    AlarmPresenter();
    ~AlarmPresenter() override;
    static AlarmPresenter *GetInstance();
    static void PlayRing(void *arg);
    int16_t GetInterStatus(void);
    ColorType ShadowBlue(void);
    bool OnClick(UIView &view, const ClickEvent &event) override;
    bool OnDrag(UIView& view, const DragEvent& event) override;
    bool ClickFuncId(const char *buttonName);
    bool ClickFuncView(UIView &view);
    void OnTimePickerStoped(UITimePicker &picker) override;
    void ClickBlue(UIView &view);
    void ClickBlack(UIView &view);
    void GetAlarmNum(uint16_t alarmNum);
    void ClickSetTime(uint16_t alarmNum);
    void ClickSetWeek(uint16_t alarmNum);
    void ClickAlarmClockDelete(uint16_t alarmNum);
    void ClickJudgeDelete(uint16_t alarmNum);
    void ClickJudgeTrue(uint16_t alarmNum);
    void ClickAlarmTimeOk(uint16_t alarmNum);
    void ClickAlarmClock(uint16_t alarmNum);
    void ClickAlarmWeekOk(uint16_t alarmNum);
    void ClickAlarmNewTime(uint16_t alarmNum);
    void ClickAlarmNext(uint16_t alarmNum);
    void OnTimePickerInaction(uint32_t hour, uint32_t min);
    uint8_t alarmWeek[7] = {1, 1, 1, 1, 1, 1, 1}; // 闹钟默认所有的日期都选中，因此初始值为1，7是指一个星期中七天
private:
    uint32_t timeHour{0};
    uint32_t timeMin{0};
    uint16_t setListener = 0;
    uint16_t alarmListener{0};
    uint16_t clockQty{0};
    int16_t interListener = 0;
    bool buttonStratus = true;
};
}
#endif