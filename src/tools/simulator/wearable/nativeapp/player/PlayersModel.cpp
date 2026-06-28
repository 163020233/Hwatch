/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: Player view
 * Author:
 * Create: 2021-07-26
 */

#include "player/PlayersModel.h"
#include "thread_adapter.h"
#include "UiConfig.h"
#include "bts_avrcp_controller.h"
#include "bts_avrcp_target.h"
#include "bts_a2dp_source.h"

namespace OHOS {

static constexpr int VOLUME_MAXIMUM = 100;
static constexpr int VOLUME_MINIMUM = 0;
static constexpr int VOLUME_PER_CHANGE = 10;

#define BUFFER_SIZE 15
static MutexId g_mutex;
PlayersModel::PlayersModel()
{
}

PlayersModel::~PlayersModel()
{
}

bool PlayersModel::FetchedPlayersCallBack(PlayersMsg *playersMsg)
{
    return false;
}

void PlayersModel::PushPlayersData(PlayersMsg playersMsg)
{
}

PlayersModel *PlayersModel::GetInstance()
{
    static PlayersModel instance;
    return &instance;
}

std::string PlayersModel::GetPlayerUri(void)
{
    return uri;
}

void PlayersModel::SetPlayerUri(std::string src)
{
    uri = src;
}

void PlayersModel::SetPlayButtonState(bool value)
{
    playButtonState = value;
}

int PlayersModel::PlayerStat()
{
    return OHOS_SUCCESS;
}

bool PlayersModel::GetAudioInitStatus(void)
{
    return audioInitStatus;
}

void PlayersModel::AudioInit(void)
{
}

void PlayersModel::SetCircleProgress(int64_t value)
{
    circleProgress = value;
}

int16_t PlayersModel::GetCurrentIndex(void)
{
    return currentIndex;
}

void PlayersModel::SetCurrentIndex(int16_t value)
{
    currentIndex = value;
}

int16_t PlayersModel::GetPlayerModel(void)
{
    return playerModel;
}

void PlayersModel::SetPlayerModel(int16_t value)
{
    playerModel = value;
}

void PlayersModel::SetPlayerModelAddOne(void)
{
    playerModel++;
}

void PlayersModel::PlaySwitching(void)
{
}

int PlayersModel::PlayerInit(void)
{
    return 0;
}

void PlayersModel::NextSong(int16_t index)
{
}

void PlayersModel::LoopingPlayout()
{
}

void PlayersModel::SequencePlayout()
{
}

void PlayersModel::RandomPlayout()
{
}

void PlayersModel::PreviousPlayout()
{
}

std::shared_ptr<Player> PlayersModel::GetPlayerCtr()
{
    return playerCtr;
}

std::shared_ptr<PlayerInterruptListener> PlayersModel::GetPlayerIntrptListener(void)
{
    return playerInterruptListener;
}

int PlayersModel::GetPlayerState(void)
{
    return OHOS_SUCCESS;
}

bool PlayersModel::HasPlayerPlayed(void)
{
    return true;
}

/* 蓝牙连接的音频控制，对按钮属性进行设置 */
void PlayersModel::ConncetBtResetButton()
{
}

int PlayersModel::PlayerPlay(void)
{
    return NORMAL;
}

int PlayersModel::PlayerPause(void)
{
    return NORMAL;
}

int PlayersModel::PlayerStop(void)
{
    return NORMAL;
}

int PlayersModel::PreSongOperate(void)
{
    return NORMAL;
}

int PlayersModel::NextSongOperate(void)
{
    return NORMAL;
}

int PlayersModel::IncreaseVolume(void)    // 不带界面增加音量
{
    return NORMAL;
}

int PlayersModel::DecreaseVolume(void)    // 不带界面减小音量
{
    return NORMAL;
}

int32_t PlayersModel::GetPlayerstatus()
{
    return NORMAL;
}

void PlayersModel::SetBTMusicPlay(ux_api_bt_control_play_t notification)
{
}
// 蓝牙连接和本地的音乐播放
int32_t PlayersModel::ConnectBtAndLocalPlay(int32_t connCtState, int32_t connTgState, int32_t playerState)
{
    return 0;
}

// 蓝牙连接和本地音乐的暂停
int32_t PlayersModel::ConnectBtAndLocalPause(int32_t connCtState, int32_t connTgState)
{
    return 0;
}

void PlayersModel::ClearActivateAudioInterrupt()
{
    isActivateAudioInterrupt = false;
}

void  PlayersModel::GetDuration(int64_t &duration)
{
    duration = duration_;
}

namespace Media {

int32_t Player::GetPlayerState(int32_t &state) const
{
    return 0;
}

int32_t Player::GetCurrentTime(int64_t &time) const
{
    return 0;
}
int32_t Player::Stop()
{
    return 0;
}
int32_t Player::Reset()
{
    return 0;
}
int32_t Player::Play()
{
    return 0;
}
int32_t Player::Pause()
{
    return 0;
}
int32_t Player::Release()
{
    return 0;
}
}


}