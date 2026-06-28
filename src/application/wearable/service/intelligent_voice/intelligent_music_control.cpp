/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: intelligent_music_control.cpp
 * Create: 2025-11-26
 */

#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include <string.h>
#include "AppViewIDs.h"
#include "main/MainViewSample.h"
#include "wearable_log.h"
#include "NativeAbility.h"
#include "utf8_to_gbk.h"
#include "player/PlayersModel.h"
#include "player/PlayersPresenter.h"
#include "intelligent_app_control.h"
#include "intelligent_voice_service.h"

namespace OHOS {
#ifdef __cplusplus
extern "C" {
#endif

int ChangeViewToPlayer(void)  // 播放音乐
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_MAIN_SERVICE, "Enter ChangeViewToPlayer");
    if (!PlayersModel::GetInstance()->HasPlayerPlayed()) {  // 播放过了就不用再初始化
        PlayersListGroup::GetInstance()->SetUpListCase();
        PlayersModel::GetInstance()->PlayerInit();
    }
    int ret = PlayersModel::GetInstance()->PlayerPlay();
    return ret;
}

int ChangePreSong(void)  // 上一首
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_MAIN_SERVICE, "Enter ChangePreSong!!");
    int ret = PlayersModel::GetInstance()->PreSongOperate();
    if (ret == NORMAL) {
        ret = PlayersModel::GetInstance()->PlayerPlay();
        if (ret != EXECUTE_FUNC_FAILED) {
            return NORMAL;
        }
    }
    return ret;
}

int ChangeNextSong(void)  // 下一首
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_MAIN_SERVICE, "Enter ChangeNextSong!!");
    int ret = PlayersModel::GetInstance()->NextSongOperate();
    if (ret == NORMAL) {
        ret = PlayersModel::GetInstance()->PlayerPlay();
        if (ret != EXECUTE_FUNC_FAILED) {
            return NORMAL;
        }
    }
    return ret;
}

int ChangeStopPlayback(void)  // 停止播放
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_MAIN_SERVICE, "Enter ChangeStopPlaybcak!!");
    int ret = PlayersModel::GetInstance()->PlayerStop();
    return ret;
}

int ChangePausePlayback(void)  // 暂停
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_MAIN_SERVICE, "Enter ChangePausePlayback!!");
    int ret = PlayersModel::GetInstance()->PlayerPause();
    return ret;
}

void IntelligentMusicEventProcess(const AsrEngineCmdInfo *info)
{
    WEARABLE_LOGE(WEARABLE_LOG_MODULE_MAIN_SERVICE,
        "IntelligentMusicEventProcess, type = %d",
        info->musicControlInfo.musicControlType);
    AsrEngineMusicControlInfo musicInfo = info->musicControlInfo;
    // 所有关闭都返回主表盘
    std::string openWords;
    switch (musicInfo.musicControlType) {
        case ASR_ENGINE_MUSIC_CTRL_PLAY:
            openWords = "已启动播放";
            ChangeViewToPlayer();
            break;
        case ASR_ENGINE_MUSIC_CTRL_PAUSE:
            openWords = "已暂停";
            ChangePausePlayback();
            break;
        case ASR_ENGINE_MUSIC_CTRL_PLAY_NEXT:
            openWords = "已切换下一首";
            ChangeNextSong();
            break;
        case ASR_ENGINE_MUSIC_CTRL_PLAY_PRE:
            openWords = "已切换上一首";
            ChangePreSong();
            break;
        default:
            openWords = "当前不支持这个操作";
            break;
    }
    IntelligentVoiceTTSSpeak(openWords.c_str());
}
#ifdef __cplusplus
}
#endif
}  // namespace OHOS