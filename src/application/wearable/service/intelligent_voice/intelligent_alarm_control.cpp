/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: intelligent_alarm_control.cpp
 * Create: 2025-11-26
 */

#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include <time.h>
#include <ctime>
#include <sys/time.h>
#include "wearable_log.h"
#include "NativeAbility.h"
#include "alarm/AlarmClockModel.h"
#include "intelligent_transform.h"
#include "intelligent_voice_service.h"
#include "intelligent_alarm_control.h"

#define MAX_HOUR_ONE_DAY 24
#define MAX_HOUR_HALF_DAY 12
#define MAX_ALARM_NUM 5
#define DAY_ONE_WEEK 7

namespace OHOS {
#ifdef __cplusplus
extern "C" {
#endif

void IntelligentAlarmEventProcess(const AsrEngineCmdInfo *info)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MAIN_SERVICE,
        "IntelligentAlarmEventProcess, type = %d, day = %d, timePeriod = %d, "
        "hour = %d, minute = %d",
        info->alarmControlInfo.alarmControlType,
        info->alarmControlInfo.alarmDateTime.day,
        info->alarmControlInfo.alarmDateTime.timePeriod,
        info->alarmControlInfo.alarmDateTime.hour,
        info->alarmControlInfo.alarmDateTime.minute);
    std::string openWords;
    uint32_t hour = info->alarmControlInfo.alarmDateTime.hour;
    uint32_t minute = info->alarmControlInfo.alarmDateTime.minute;
    if (info->alarmControlInfo.alarmDateTime.timePeriod == ASR_ENGINE_TIME_PERIOD_PM) {
        hour += MAX_HOUR_HALF_DAY;
        if (hour > MAX_HOUR_ONE_DAY) {
            openWords = "闹钟设置失败，时间格式错误";
            IntelligentVoiceTTSSpeak(openWords.c_str());
            return;
        }
    }
    int32_t currentAlarmNum = AlarmClockModel::GetInstance()->GetAlarmNum();
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MAIN_SERVICE, "currentAlarmNum=%d\r\n", currentAlarmNum);
    if (currentAlarmNum >= MAX_ALARM_NUM) {
        openWords = "设置闹钟失败闹钟个数超过最大个数";
        IntelligentVoiceTTSSpeak(openWords.c_str());
        return;
    }
    AlarmClockModel::GetInstance()->SetAlarmTime(hour, minute, 0);
    std::time_t now = std::time(nullptr);
    std::tm *localTime = std::localtime(&now);
    if (!localTime) {
        return;
    }
    int weekday = localTime->tm_wday;
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MAIN_SERVICE, "weekday=%d\r\n", weekday);
    if (info->alarmControlInfo.alarmDateTime.day == ASR_ENGINE_TIME_TOMORROW) {
        weekday = weekday + 1;
    } else if (info->alarmControlInfo.alarmDateTime.day == ASR_ENGINE_TIME_THE_DAY_AFTER_TOMORROW) {
        weekday = weekday + 2;
    } else if (info->alarmControlInfo.alarmDateTime.day == ASR_ENGINE_TIME_THE_DAY_AFTER_THE_DAY_AFTER_TOMORROW) {
        weekday = weekday + 3;
    }
    weekday = weekday % DAY_ONE_WEEK;
    uint8_t realWeek = 1 << weekday;
    AlarmClockModel::GetInstance()->SetAlarmWeek(realWeek, 0);
    if (weekday == 0) {
        weekday = 7;
    }
    NativeAbility::GetInstance().ChangeSlice(VIEW_MAIN_ALARM);
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MAIN_SERVICE, "weekday2=%d\r\n", weekday);
    openWords = "已为您设置";
    openWords += "星期" + numberToChinese(weekday);
    openWords += numberToChinese(hour);
    openWords += "小时";
    openWords += numberToChinese(minute);
    openWords += "分钟的闹钟";
    IntelligentVoiceTTSSpeak(openWords.c_str());
}

#ifdef __cplusplus
}
#endif
}  // namespace OHOS