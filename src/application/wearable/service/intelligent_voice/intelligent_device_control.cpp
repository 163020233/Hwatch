/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: intelligent_device_control.cpp
 * Create: 2025-11-26
 */

#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include "wearable_log.h"
#include "NativeAbility.h"
#include "player/PlayersModel.h"
#include "intelligent_voice_asr_engine.h"
#include "intelligent_voice_service.h"
#include "intelligent_device_control.h"

namespace OHOS {
#ifdef __cplusplus
extern "C" {
#endif

int ChangeIncreaseVolume(void)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "Enter ChangeResumePlayback!!");

    int ret = PlayersModel::GetInstance()->IncreaseVolume();
    return ret;
}

int ChangeDecreaseVolume(void)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "Enter ChangeResumePlayback!!");

    int ret = PlayersModel::GetInstance()->DecreaseVolume();
    return ret;
}

void IntelligentDeviceEventProcess(const AsrEngineCmdInfo *info)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MAIN_SERVICE,
        "IntelligentDeviceEventProcess,type = %d",
        info->deviceControlInfo.deviceControlType);
    std::string openWords;
    switch (info->deviceControlInfo.deviceControlType) {
        case ASR_ENGINE_DEVICE_CTRL_INCREASE_VOLUME:
            openWords = "音量已增加";
            ChangeIncreaseVolume();
            break;
        case ASR_ENGINE_DEVICE_CTRL_DECREASE_VOLUME:
            openWords = "音量已减小";
            ChangeDecreaseVolume();
            break;
        default:
            openWords = "不支持这个操作";
    }
    IntelligentVoiceTTSSpeak(openWords.c_str());
}

#ifdef __cplusplus
}
#endif
}  // namespace OHOS