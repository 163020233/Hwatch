/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: intelligent_timer_control.cpp
 * Create: 2025-11-26
 */

#include <stdio.h>
#include <cstdlib>
#include <map>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include "AppViewIDs.h"
#include "wearable_log.h"
#include "NativeAbility.h"
#include "utf8_to_gbk.h"
#include "intelligent_app_control.h"
#include "intelligent_voice_service.h"
#include "main/MainViewSample.h"
#include "timers/TimerView.h"
#include "timers/TimerSettingPage.h"
#include "timers/TimerCountDown.h"
#include "timers/TimerPresenter.h"
#include "timers/TimerMainPage.h"
#include "intelligent_transform.h"
#include "intelligent_timer_control.h"

#define TIME_STEP 60
#define SEC_PER_HOUR 3600
#define MAX_TIME 100
#define MS_PER_SEC 1000

namespace OHOS {
#ifdef __cplusplus
extern "C" {
#endif

void IntelligentTimerEventProcess(const AsrEngineCmdInfo *info)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MAIN_SERVICE,
        "IntelligentTimerEventProcess type = %d, hour = %d, minute = %d, second = %d",
        info->timerControlInfo.timerControlType,
        info->timerControlInfo.hour,
        info->timerControlInfo.minute,
        info->timerControlInfo.second);
    uint32_t hour = info->timerControlInfo.hour;
    uint32_t minute = info->timerControlInfo.minute;
    uint32_t second = info->timerControlInfo.second;
    AsrEngineTimerControlInfo timerInfo = info->timerControlInfo;
    if (second >= TIME_STEP) {
        minute += second / TIME_STEP;
        second = second % TIME_STEP;
    }
    if (minute >= TIME_STEP) {
        hour = hour + minute / TIME_STEP;
        minute = minute % TIME_STEP;
    }

    std::string openWords;
    if (hour >= MAX_TIME) {
        openWords = "不支持设置超过一百个小时的定时器";
        IntelligentVoiceTTSSpeak(openWords.c_str());
        return;
    }
    switch (timerInfo.timerControlType) {
        case ASR_ENGINE_TIMER_CTRL_SET: {
            openWords = "已设置";
            if (hour != 0) {
                openWords += numberToChinese(hour);
                openWords += "小时";
            }
            if (minute != 0) {
                openWords += numberToChinese(minute);
                openWords += "分钟";
            }
            if (second != 0) {
                openWords += numberToChinese(second);
                openWords += "秒钟";
            }
            openWords += "的计时器";
            uint64 time = hour * SEC_PER_HOUR + minute * TIME_STEP + second;
            TimerModel::GetInstance().SetTotalTime(time);
            TimerModel::GetInstance().SetEclipseTime(true, 0);
            TimerModel::GetInstance().SetExistTimer(true);
            struct timeval current_time;
            gettimeofday(&current_time, nullptr);
            uint64_t s_time = (uint64_t)current_time.tv_sec * MS_PER_SEC + current_time.tv_usec / MS_PER_SEC;
            TimerModel::GetInstance().SetStartTime(s_time);
            NativeAbility::GetInstance().ChangeSlice(VIEW_TIMER);
            break;
        }
        case ASR_ENGINE_TIMER_CTRL_START: {
            openWords = "已启动定时器";
            if (TimerModel::GetInstance().GetIsCountDown()) {
                openWords = "定时器已启动，不能重复启动定时器";
            } else {
                Point lp = {0, 0};
                UIView countView = {};
                ClickEvent countEvent = {lp};
                countView.SetViewId("startOrPauseButton");
                TimerMainPage::GetInstance()->GetTimerView()->OnClick(countView, countEvent);
            }
            break;
        }
        case ASR_ENGINE_TIMER_CTRL_PAUSE: {
            if (!TimerModel::GetInstance().GetIsCountDown()) {
                openWords = "定时器已暂停，不能重复暂停定时器";
            } else {
                Point lp = {0, 0};
                UIView countView = {};
                ClickEvent countEvent = {lp};
                countView.SetViewId("startOrPauseButton");
                TimerMainPage::GetInstance()->GetTimerView()->OnClick(countView, countEvent);
            }
            break;
        }
        default:
            // 播放TTS当前不支持打开这个应用
            openWords = "定时器不支持这个操作";
            WEARABLE_LOGI(WEARABLE_LOG_MODULE_MAIN_SERVICE, "Unknown identification failed!");
            break;
    }
    IntelligentVoiceTTSSpeak(openWords.c_str());
}
#ifdef __cplusplus
}
#endif
}  // namespace OHOS