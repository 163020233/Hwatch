/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: intelligent_app_control.cpp
 * Create: 2025-11-26
 */

#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include <string.h>
#include "AppViewIDs.h"
#include "main/MainPresenterSample.h"
#include "wearable_log.h"
#include "NativeAbility.h"
#include "main/MainViewSample.h"
#include "intelligent_voice_service.h"
#include "intelligent_app_control.h"

namespace OHOS {
#ifdef __cplusplus
extern "C" {
#endif

void IntelligentAppEventProcess(const AsrEngineCmdInfo *info)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MAIN_SERVICE,
        "IntelligentAppEventProcess, result appId = %d, appType = %d",
        info->appControlInfo.appId,
        info->appControlInfo.appControlType);
    AsrEngineAppControlInfo appInfo = info->appControlInfo;
    // 所有关闭都返回主表盘
    std::string openWords;
    if (appInfo.appControlType == ASR_ENGINE_APP_CTRL_CLOSE) {
        openWords = "已关闭应用";
        NativeAbility::GetInstance().ChangeSlice(VIEW_MAIN_SAMPLE);
        IntelligentVoiceTTSSpeak(openWords.c_str());
        return;
    }
    switch (appInfo.appId) {
        case ASR_ENGINE_APP_SETTING:
            openWords = "已打开设置";
            NativeAbility::GetInstance().ChangeSlice(VIEW_SETTING_FIRST);
            break;
        case ASR_ENGINE_APP_HEART_RATE:
            openWords = "已打开心率";
            NativeAbility::GetInstance().ChangeSlice(VIEW_HEARTRATE);
            break;
        case ASR_ENGINE_APP_SPORT:
            openWords = "已打开运动";
            NativeAbility::GetInstance().ChangeSlice(VIEW_SPORT);
            break;
        case ASR_ENGINE_APP_SPORT_RECORD:
            openWords = "已打开运动记录";
            MainPresenterSample::GetInstance()->SetFromSetCardView(true);
            MainPresenterSample::GetInstance()->SetFromCardId(ACTIVITY);
            NativeAbility::GetInstance().ChangeSlice(VIEW_MAIN_SAMPLE);
            break;
        case ASR_ENGINE_APP_TRANING_STATUS:
            openWords = "已打开训练状态";
            NativeAbility::GetInstance().ChangeSlice(VIEW_SPORT);
            break;
        case ASR_ENGINE_APP_SPO2:
            openWords = "已打开血氧";
            NativeAbility::GetInstance().ChangeSlice(VIEW_BLOOD);
            break;
        case ASR_ENGINE_APP_SLEEP:
            openWords = "已打开睡眠";
            NativeAbility::GetInstance().ChangeSlice(VIEW_SLEEPING);
            break;
        case ASR_ENGINE_APP_PRESSURE:
            openWords = "压力检测应用未安装，请尝试其他应用，已帮您返回主表盘";
            NativeAbility::GetInstance().ChangeSlice(VIEW_MAIN_SAMPLE);
            break;
        case ASR_ENGINE_APP_BREATH_TRAIN:
            openWords = "已打开呼吸训练";
            NativeAbility::GetInstance().ChangeSlice(VIEW_BREATH);
            break;
        case ASR_ENGINE_APP_ELECTROCARDIO:
            openWords = "心电检测应用未安装，请尝试其他应用，已帮您返回主表盘";
            NativeAbility::GetInstance().ChangeSlice(VIEW_MAIN_SAMPLE);
            break;
        case ASR_ENGINE_APP_CALL_RECORDS:
            openWords = "已打开通话记录";
            NativeAbility::GetInstance().ChangeSlice(VIEW_CALLER_SAMPLE);
            NativeAbility::GetInstance().SwitchPageInSlice(4, TransitionType::TRANSITION_INVALID, false);
            break;
        case ASR_ENGINE_APP_FAVOURITES:
            openWords = "已打开常用联系人";
            NativeAbility::GetInstance().ChangeSlice(VIEW_CALLER_SAMPLE);
            NativeAbility::GetInstance().SwitchPageInSlice(2, TransitionType::TRANSITION_INVALID, false);
            break;
        case ASR_ENGINE_APP_WEATHER:  // 天气
            openWords = "已打开天气，今日天气阴，空气质量良，气温十度到二十五度，气温较冷注意保暖";
            NativeAbility::GetInstance().ChangeSlice(VIEW_WEATHER);
            break;
        case ASR_ENGINE_APP_MUSIC_APP:
            openWords = "已打开音岳";  // 打开音乐
            MainPresenterSample::GetInstance()->SetFromSetCardView(true);
            MainPresenterSample::GetInstance()->SetFromCardId(MUSIC_PLAYER);
            NativeAbility::GetInstance().ChangeSlice(VIEW_MAIN_SAMPLE);
            break;
        case ASR_ENGINE_APP_REMOTE_PHOTO:
            openWords = "已打开遥控拍照";
            NativeAbility::GetInstance().ChangeSlice(VIEW_CAMERA);
            break;
        case ASR_ENGINE_APP_NOTIFICATION:
            openWords = "没有通知这个应用，帮您返回主表盘";
            NativeAbility::GetInstance().ChangeSlice(VIEW_MAIN_SAMPLE);
            break;
        case ASR_ENGINE_APP_ALARM_APP:
            openWords = "已打开闹钟";
            NativeAbility::GetInstance().ChangeSlice(VIEW_MAIN_ALARM);
            break;
        case ASR_ENGINE_APP_CALENDAR:
            openWords = "已打开日历";
            NativeAbility::GetInstance().ChangeSlice(VIEW_DATE);
            break;
        case ASR_ENGINE_APP_COMPASS:
            openWords = "已打开指南针";
            NativeAbility::GetInstance().ChangeSlice(VIEW_COMPASS);
            break;
        case ASR_ENGINE_APP_STOPWATCH:
            openWords = "已打开秒表";
            NativeAbility::GetInstance().ChangeSlice(VIEW_STOPWATCH);
            break;
        case ASR_ENGINE_APP_TIMER:
            openWords = "已打开计时器";
            NativeAbility::GetInstance().ChangeSlice(VIEW_TIMER);
            break;
        case ASR_ENGINE_APP_WECHAT_PAY:
            openWords = "没有微信支付这个应用，帮您返回表盘";
            NativeAbility::GetInstance().ChangeSlice(VIEW_MAIN_SAMPLE);
            break;
        case ASR_ENGINE_APP_ALI_PAY:
            openWords = "没有支付宝这个应用，帮您返回表盘";
            NativeAbility::GetInstance().ChangeSlice(VIEW_MAIN_SAMPLE);
            break;
        case ASR_ENGINE_APP_BAROMETER:
            openWords = "没有海拔气压计这个应用，帮您返回表盘";
            NativeAbility::GetInstance().ChangeSlice(VIEW_MAIN_SAMPLE);
            break;
        case ASR_ENGINE_APP_ALTIMETER:
            openWords = "没有气压高度计这个应用，帮您返回表盘";
            NativeAbility::GetInstance().ChangeSlice(VIEW_MAIN_SAMPLE);
            break;
        case ASR_ENGINE_APP_NETEASY_MUSIC:
            openWords = "没有网易音乐这个应用，帮您返回表盘";
            NativeAbility::GetInstance().ChangeSlice(VIEW_MAIN_SAMPLE);
            break;
        case ASR_ENGINE_APP_TELEPHONE:
            openWords = "已打开电话";
            NativeAbility::GetInstance().ChangeSlice(VIEW_PHONE_MENU);
            break;
        case ASR_ENGINE_APP_MESSAGE:
            openWords = "已打开短信";
            NativeAbility::GetInstance().ChangeSlice(VIEW_PHONE_MENU);
            break;
        case ASR_ENGINE_APP_WATCH_FACE:
            openWords = "已打开表盘";
            NativeAbility::GetInstance().ChangeSlice(VIEW_MAIN_SAMPLE);
            break;
        case ASR_ENGINE_APP_BUS_CARD:
            openWords = "没有公交卡这个应用，帮您返回表盘";
            NativeAbility::GetInstance().ChangeSlice(VIEW_MAIN_SAMPLE);
            break;
        case ASR_ENGINE_APP_ACCESS_CARD:
            openWords = "没有门禁卡这个应用，帮您返回表盘";
            NativeAbility::GetInstance().ChangeSlice(VIEW_SPORT);
            break;
        default:
            // 播放TTS当前不支持打开这个应用
            openWords = "当前算法未适配此应用，可以尝试打开，运动记录，血氧，睡眠，压力，呼吸，训练，天气，日历，指南针"
                        "秒表，计时器，电话";
            NativeAbility::GetInstance().ChangeSlice(VIEW_MAIN_SAMPLE);
            break;
    }
    IntelligentVoiceTTSSpeak(openWords.c_str());
}
#ifdef __cplusplus
}
#endif
}  // namespace OHOS