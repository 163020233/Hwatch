/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: Player view
 * Author:
 * Create: 2021-07-26
 */

#include <cstdint>
#include "security_unified/trng.h"
#include "player/PlayersModel.h"
#include "thread_adapter.h"
#include "UiConfig.h"
#include "bts_avrcp_controller.h"
#include "bts_avrcp_target.h"
#include "bts_a2dp_source.h"
#include "ux_api_bt_audio.h"
#include "settings/model/SettingBtHeadset.h"
#include "settings/model/SettingBluetoothModel.h"
#include "common_def.h"

namespace OHOS {

static constexpr int VOLUME_MAXIMUM = 100;
static constexpr int VOLUME_MINIMUM = 0;
static constexpr int VOLUME_PER_CHANGE = 10;

#define BUFFER_SIZE 15
static MutexId g_mutex;
PlayersModel::PlayersModel()
{
    g_mutex = MUTEX_InitValue();
}

PlayersModel::~PlayersModel()
{
    listPlayers.Clear();
}

bool PlayersModel::FetchedPlayersCallBack(PlayersMsg *playersMsg)
{
    if (listPlayers.IsEmpty()) {
        return false;
    } else {
        MUTEX_Lock(g_mutex);
        *playersMsg = listPlayers.Front();
        listPlayers.PopFront();
        MUTEX_Unlock(g_mutex);
    }
    return true;
}

void PlayersModel::PushPlayersData(PlayersMsg playersMsg)
{
    MUTEX_Lock(g_mutex);
    listPlayers.PushBack(playersMsg);
    MUTEX_Unlock(g_mutex);
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
    int32_t ret;

    // 设置回调函数
    playerCtr->SetPlayerCallback(playerCallback);
    // 申请资源
    source = std::make_shared<Source>(uri, header);
    if (source.get() == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "source new fail");
        return OHOS_FAILURE;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "Player uri %s.", uri.c_str());
    playerCtr->SetSource(*source);
    playerCtr->Prepare();
    ret = playerCtr->GetDuration(duration_);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[%s,%d] GetDuration:%lld, ret:%d", __func__, __LINE__, duration_, ret);
    if (!isActivateAudioInterrupt) {
        // 申请焦点
        AudioManager& amIntance = AudioManager::GetInstance();
        sessionId = amIntance.MakeSessionId();
        if (sessionId == AUDIO_SESSION_ID_NONE) {
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "session invalid");
            return OHOS_FAILURE;
        }
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "MakeSessionId  %d", sessionId);
        playerInterruptListener = std::make_shared<PlayerInterruptListener>();
        if (playerInterruptListener.get() == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "playerInterruptListener new fail");
            return OHOS_FAILURE;
        }
        playerInterruptListener->SetOwner(playerCtr);
        interrupt = {AUDIO_STREAM_MUSIC, sessionId, playerInterruptListener};
            // 激活音频中断 5s超时
        if (amIntance.ActivateAudioInterrupt(interrupt) == INTERRUPT_FAILED) {
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "ActivateAudioInterrupt faild");
            return OHOS_FAILURE;
        }
        isActivateAudioInterrupt = true;
    }
    playerCtr->SetAudioSessionId(sessionId);
    // 播放
    ret = playerCtr->Play();
    if (ret != 0) {
        playerCtr->Stop(); // 暂停
        playerCtr->Reset(); // 初始化

        PlayersView *playersView = PlayersView::GetInstance();
        if (playersView != nullptr) {
            playersView->SetPlayerButtonState(UICheckBox::UICheckBoxState::UNSELECTED);
            SetCircleProgress(100); // 100:进度100%
            playersView->SetPlaybackProgress(circleProgress);
        } else {
            SetPlayButtonState(false);
            SetCircleProgress(100); // 100:进度100%
        }
        if (!playerInterruptListener->IsInterruptedPaused()) {
            WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP,
                "Other abnormal conditions need to clear interrupts.");
            AudioManager::GetInstance().DeactivateAudioInterrupt(interrupt);
            isActivateAudioInterrupt = false;
        } else {
            WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP,
                "Because an interruption occurs, the interrupt is not cleared.");
        }
    }
    return OHOS_SUCCESS;
}

bool PlayersModel::GetAudioInitStatus(void)
{
    return audioInitStatus;
}

void PlayersModel::AudioInit(void)
{
    if (audioInitStatus) {
        return;
    }
    AudioManager& amIntance = AudioManager::GetInstance();
    audioInitStatus = amIntance.Initialize();
    if (!audioInitStatus) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AudioManager initialization failed");
        return;
    }
    playerCtr = std::make_shared<Player>();
    if (playerCtr.get() == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "playerCtr new fail");
        return;
    }
    playerCallback = std::make_shared<PlayerCallbackImpl>();
    if (playerCallback.get() == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "playerCallback new fail");
        return;
    }
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
    PlayersMsg playersMsg1;
    playersMsg1.playerModel = GetPlayerModel();
    PushPlayersData(playersMsg1);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "PlaySwitching playerModel1 %d!!!", GetPlayerModel());
    int32_t ret;
    unused(ret);
    PlayersMsg playersMsg;
    if (FetchedPlayersCallBack(&playersMsg)) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "FetchedPlayersCallBack true");
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "Stop to play");
        switch (playersMsg.playerModel) {
            case PlayerModelState::LOOP:
                // 循环
                WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "LoopingPlayout!!!");
                LoopingPlayout();
                break;
            case PlayerModelState::SEQUENCE:
                // 顺序
                WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SequencePlayout!!!");
                SequencePlayout();
                break;
            case PlayerModelState::REPEATS:
                // 重复
                WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "RepeatsPlayout!!!");
                ret = PlayersModel::GetInstance()->playerCtr->Stop(); // 停止
                WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "Stop, ret:%d", ret);
                ret = PlayersModel::GetInstance()->playerCtr->Reset();
                WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "Reset, ret:%d", ret);
                PlayersModel::GetInstance()->PlayerStat(); // 播放
                break;
            case PlayerModelState::RANDOM:
                // 随机
                WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "RandomPlayout!!!");
                RandomPlayout();
                break;
            default:
                break;
        }
    }
}

int PlayersModel::PlayerInit(void)
{
    sliceId = PlayersListGroup::GetInstance()->GetCase(currentIndex);
    if (sliceId.empty()) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "GetCase %d sliceId err !!!", currentIndex);
        char strFile[BUFFER_SIZE] = "No music files";
        sliceId = strFile;
    }
    std::string strSongName = sliceId;
    std::string pureName = strSongName.substr(0, strSongName.rfind("."));
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "PlayerInit, url: %s ", sliceId.c_str());
    std::string strSrcMusic(APP_MUSIC_PATH);
    std::string tempStr("/");
    std::string strName = sliceId;
    uri = strSrcMusic + tempStr + strName;
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "PlayerInit end!");
    PlayersView *playersView = PlayersView::GetInstance();
    if (playersView == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PlayerInit playersView GetInstance fail");
        return OHOS_FAILURE;
    }
    playersView->TitleSetText(pureName.c_str());
    playersView->SetPlaybackProgress(circleProgress);
    playersView->SetPlayerState(playButtonState);
    return 0;
}

void PlayersModel::NextSong(int16_t index)
{
    SetCurrentIndex(index);
    // 播放
    sliceId = PlayersListGroup::GetInstance()->GetCase(index);
    if (sliceId.empty()) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "GetCase %d sliceId err!!!", index);
        return;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "GetCase sliceId %s.", sliceId.c_str());
    std::string strSrcMusic(APP_MUSIC_PATH);
    std::string tempStr("/");
    std::string strName = sliceId;
    SetPlayerUri(strSrcMusic + tempStr + strName);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "uri: %s.", GetPlayerUri().c_str());
    std::string strSongName = sliceId;
    std::string pureName = strSongName.substr(0, strSongName.rfind("."));
    PlayersView *playersView = PlayersView::GetInstance();
    if (playersView != nullptr) {
        playersView->TitleSetText(pureName.c_str());
        playersView->SetPlaybackProgress(0); // 0:进度0%
    }
    int32_t ret;
    int32_t playerState;
    playerCtr->GetPlayerState(playerState);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "playerState Value %d.", playerState);
    if ((playerState == PlayerStates::PLAYER_STARTED) || (playerState == PlayerStates::PLAYER_PLAYBACK_COMPLETE)) {
        /* start play */
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "start play, 160.");
        ret = playerCtr->Stop(); // 停止
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "Stop, ret:%d", ret);
        ret = playerCtr->Reset();
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "Reset, ret:%d", ret);
        ret = PlayerStat(); // 播放
        if (ret != 0) {
            /* stop and exit */
            ret = playerCtr->Stop(); // 停止
            ret = playerCtr->Reset();
        }
    } else if (playerState == PlayerStates::PLAYER_PAUSED || (playerState == PlayerStates::PLAYER_IDLE)) {
        ret = playerCtr->Reset();
        AudioManager::GetInstance().DeactivateAudioInterrupt(interrupt);
        ClearActivateAudioInterrupt();
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "Reset, ret:%d", ret);
    }
}

void PlayersModel::LoopingPlayout()
{
    int16_t index;
    List<PlayerCaseInfo> &playersCaseList = PlayersListGroup::GetInstance()->GetCase();
    index = GetCurrentIndex();
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "LoopingPlayout, index %d.", index);
    if ((index > playersCaseList.Size() - 1) || (index < 0)) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "currentIndex err");
        SetCurrentIndex(0);
        return;
    }
    if (GetPlayerModel() == PlayerModelState::RANDOM) {
        RandomPlayout();
        return;
    } else {
        index++; // 下一曲
    }
    if (index > playersCaseList.Size() - 1) {
        index = 0;
    }
    NextSong(index);
    SetCircleProgress(0); // 0:进度0%
}

void PlayersModel::SequencePlayout()
{
    int16_t index;
    List<PlayerCaseInfo> &playersCaseList = PlayersListGroup::GetInstance()->GetCase();
    index = GetCurrentIndex();
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SequencePlayout, index %d.", index);
    if ((index > playersCaseList.Size() - 1) || (index < 0)) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "currentIndex err");
        SetCurrentIndex(0);
        return;
    }
    index++; // 下一曲
    if (index > playersCaseList.Size() - 1) {
        playerCtr->Stop(); // 暂停
        playerCtr->Reset(); // 初始化
        PlayersView *playersView = PlayersView::GetInstance();
        if (playersView != nullptr) {
            playersView->SetPlayerButtonState(UICheckBox::UICheckBoxState::UNSELECTED);
        } else {
            SetPlayButtonState(false);
        }
        SetCircleProgress(100); // 100:进度100%
        index = playersCaseList.Size() - 1;
    }
    NextSong(index);
}

void PlayersModel::RandomPlayout()
{
    int16_t random;
    int16_t indexMax;
    srand((unsigned)time(nullptr));
    List<PlayerCaseInfo> &playersCaseList = PlayersListGroup::GetInstance()->GetCase();
    if (playersCaseList.Size() == 0) {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "playersCaseList is empty");
        return;
    }
    indexMax = playersCaseList.Size() - 1;
    uint32_t randnum;
    uapi_drv_cipher_trng_get_random(&randnum);
    random = (static_cast<int16_t>(randnum) % (indexMax + 1));
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "RANDOM %d.", random);
    if ((random > playersCaseList.Size() - 1) || (random < 0)) {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "random err!set playIndex is currentIndex: %d", currentIndex);
        SetCurrentIndex(0);
        NextSong(currentIndex);
        SetCircleProgress(0); // 0:进度0%
        return;
    }
    if (GetCurrentIndex() == random) {
        random++;
        if (random > playersCaseList.Size() - 1) {
            random = 0;
        }
    }
    NextSong(random);
    SetCircleProgress(0); // 0:进度0%
}

void PlayersModel::PreviousPlayout()
{
    int16_t index;
    List<PlayerCaseInfo> &playersCaseList = PlayersListGroup::GetInstance()->GetCase();
    index = GetCurrentIndex();
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "PreviousPlayout, index %d.", index);
    if ((index > playersCaseList.Size() - 1) || (index < 0)) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "currentIndex err");
        SetCurrentIndex(0);
        return;
    }
    index--; // 上一曲
    if (index < 0) {
        index = playersCaseList.Size() - 1;
    }
    NextSong(index);
    SetCircleProgress(0); // 0:进度0%
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
    int state;
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[PlayersModel] GetPlayerState");

    if (playerCtr.get() == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "playerCtr.get() is nullptr!!");
        return ERR_RET;
    }
    playerCtr->GetPlayerState(state);
    return state;
}

bool PlayersModel::HasPlayerPlayed(void)
{
    int state = GetPlayerState();
    if (state == ERR_RET) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "Get Player State failed!!");
        return false;
    }
    if (state != PlayerStates::PLAYER_STARTED && state != PlayerStates::PLAYER_PAUSED &&
        state != PlayerStates::PLAYER_STOPPED && state != PlayerStates::PLAYER_PLAYBACK_COMPLETE) {
        if (!isStopAndReset) {
            return false;
        }
    }
    return true;
}

/* 蓝牙连接的音频控制，对按钮属性进行设置 */
void PlayersModel::ConncetBtResetButton()
{
    ux_api_bt_profile_state_t a2dp_state = UX_API_BT_PROFILE_STATE_MAX;
    BtAvrcpTgInf *bt_avrcp_srv_inf = BtAvrcpSrvGetInfo();
    bd_addr_t *bt_avrcp_addr = &bt_avrcp_srv_inf->bd_addr;
    if (reinterpret_cast<ux_api_bt_addr_t *>(bt_avrcp_addr) == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "bt_avrcp_addr_sec_trans failed");
        return;
    }
    ux_api_bt_get_profile_state(UX_API_BT_PROFILE_A2DP,
        reinterpret_cast<ux_api_bt_addr_t *>(bt_avrcp_addr), &a2dp_state);
    if (static_cast<int>(a2dp_state) == static_cast<int>(PROFILE_STATE_CONNECTED)) {
        if (PlayersView::GetInstance() == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PlayersView::GetInstance() == nullptr");
            return;
        }
        PlayersView::GetInstance()->PlayerHeadsetReset();
    }
}

int PlayersModel::PlayerPlay(void)
{
    int state;
    int ret;
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[PlayersModel] PlayerPlay");
    state = GetPlayerState();
    if (state == ERR_RET) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "Get state of player failed!!");
        return EXECUTE_FUNC_FAILED;
    }

    if (playerCtr.get() == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "playerCtr.get() is nullptr!!");
        return EXECUTE_FUNC_FAILED;
    }
    if (state == PlayerStates::PLAYER_PAUSED || state == PlayerStates::PLAYER_STOPPED) {           // 暂停状态则直接开始
        ret = playerCtr->Play();
        if (ret != 0) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "Play err, ret:%d", ret);
            return EXECUTE_FUNC_FAILED;
        }
        PlayersModel::GetInstance()->SetPlayButtonState(true);      // 播放控件图形同步
        ConncetBtResetButton(); // 蓝牙连接的音频控制，对按钮属性进行设置
        playerInterruptListener->SetUserPaused(false);
        return NORMAL;
    } else if (state == PlayerStates::PLAYER_STARTED || state == PlayerStates::PLAYER_PLAYBACK_COMPLETE) {   // 已经在播放状态则不做操作
        return SONG_ALREADY_PLAY;
    } else {                  // 其它情况的处理
        ret = PlayersModel::GetInstance()->PlayerStat();
        if (ret != 0) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PlayerStat err, ret:%d", ret);
            return EXECUTE_FUNC_FAILED;
        }
        PlayersModel::GetInstance()->SetPlayButtonState(true);      // 播放控件图形同步
        ConncetBtResetButton();
        return NORMAL;
    }
}

int PlayersModel::PlayerPause(void)
{
    int state;
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[PlayersModel] PlayerPause");

    state = GetPlayerState();
    if (state == ERR_RET) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "Get state of player failed!!");
        return EXECUTE_FUNC_FAILED;
    }

    if (playerCtr.get() == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "playerCtr.get() is nullptr!!");
        return EXECUTE_FUNC_FAILED;
    }
    if (state == PlayerStates::PLAYER_PAUSED) {           // 暂停状态则不操作
        return SONG_ALREADY_PAUSE;
    } else if (state == PlayerStates::PLAYER_STARTED || state == PlayerStates::PLAYER_PLAYBACK_COMPLETE) {
        int ret = playerCtr->Pause();
        if (ret != 0) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "Pause err, ret:%d!!", ret);
            return EXECUTE_FUNC_FAILED;
        } else {
            PlayersModel::GetInstance()->SetPlayButtonState(false);
            ConncetBtResetButton();
            playerInterruptListener->SetUserPaused(true);
            return NORMAL;
        }
    } else if (state == PlayerStates::PLAYER_STOPPED) {
        return SONG_ALREADY_STOP;
    }
    return NORMAL;
}

int PlayersModel::PlayerStop(void)
{
    int state;
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[PlayersModel] PlayerStop");
    state = GetPlayerState();
    if (state == ERR_RET) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "Get state of player failed!!");
        return EXECUTE_FUNC_FAILED;
    }
    if (playerCtr.get() == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "playerCtr.get() is nullptr!!");
        return EXECUTE_FUNC_FAILED;
    }
    if (state == PlayerStates::PLAYER_STARTED || state == PlayerStates::PLAYER_PLAYBACK_COMPLETE) {
        int ret = playerCtr->Stop();
        if (ret != 0) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "Stop err, ret:%d", ret);
            return EXECUTE_FUNC_FAILED;
        }
        ret = playerCtr->Reset();
        if (ret != 0) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "Reset err, ret:%d", ret);
            return EXECUTE_FUNC_FAILED;
        } else {
            isStopAndReset = true;
            PlayersModel::GetInstance()->SetPlayButtonState(false);
            PlayersModel::GetInstance()->SetCircleProgress(0);
            ConncetBtResetButton();
        }
        AudioManager::GetInstance().DeactivateAudioInterrupt(interrupt);
        return NORMAL;
    } else if (state == PlayerStates::PLAYER_PAUSED) {
        int ret = playerCtr->Reset();
        if (ret != 0) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "Reset err, ret:%d", ret);
            return EXECUTE_FUNC_FAILED;
        } else {
            PlayersModel::GetInstance()->SetPlayButtonState(false);
            PlayersModel::GetInstance()->SetCircleProgress(0);
            ConncetBtResetButton();
        }
        AudioManager::GetInstance().DeactivateAudioInterrupt(interrupt);
        return NORMAL;
    } else if (state == PlayerStates::PLAYER_STOPPED) {
        return SONG_ALREADY_STOP;
    }
    return NORMAL;
}

int PlayersModel::PreSongOperate(void)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[PlayersModel] PreSongOperate");

    PlayersModel::GetInstance()->PreviousPlayout();
    PlayersModel::GetInstance()->SetPlayButtonState(true);
    return NORMAL;
}

int PlayersModel::NextSongOperate(void)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[PlayersModel] NextSongOperate");

    PlayersModel::GetInstance()->LoopingPlayout();
    PlayersModel::GetInstance()->SetPlayButtonState(true);
    return NORMAL;
}

int PlayersModel::IncreaseVolume(void)    // 不带界面增加音量
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[PlayersModel] IncreaseVolume");

    int curSound = AudioManager::GetInstance().GetVolume(AUDIO_STREAM_MUSIC);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "Presound is %d", curSound);
    if (curSound > VOLUME_MAXIMUM - VOLUME_PER_CHANGE) {
            curSound = VOLUME_MAXIMUM;
    } else {
        curSound += VOLUME_PER_CHANGE;
    }
    AudioManager::GetInstance().SetVolume(AUDIO_STREAM_MUSIC, curSound);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "Now music sound is %d", curSound);
    return NORMAL;
}

int PlayersModel::DecreaseVolume(void)    // 不带界面减小音量
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[PlayersModel] DecreaseVolume");

    int curSound = AudioManager::GetInstance().GetVolume(AUDIO_STREAM_MUSIC);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "Presound is %d", curSound);
    if (curSound < VOLUME_MINIMUM + VOLUME_PER_CHANGE) {
            curSound = VOLUME_MINIMUM;
    } else {
        curSound -= VOLUME_PER_CHANGE;
    }
    AudioManager::GetInstance().SetVolume(AUDIO_STREAM_MUSIC, curSound);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "Now music sound is %d", curSound);
    return NORMAL;
}

int32_t PlayersModel::GetPlayerstatus()
{
    int32_t playerState;
    if (playerCtr.get() == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PlayersModel::GetPlayerstatus playerCtr is nullptr");
        return EXECUTE_FUNC_FAILED;
    }
    playerCtr->GetPlayerState(playerState);

    switch (playerState) {
        case PLAYER_STARTED:
            playerState = AVRCP_PLAY_STATUS_PLAYING;
            break;
        case PLAYER_PAUSED:
            playerState = AVRCP_PLAY_STATUS_PAUSED;
            break;
        case PLAYER_STOPPED:
            playerState = AVRCP_PLAY_STATUS_STOPPED;
            break;
        default:
            playerState = AVRCP_PLAY_STATUS_STOPPED;
            break;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[PlayerModel::GetPlayerstatus]status = [%d]", playerState);
    return playerState;
}

void PlayersModel::SetBTMusicPlay(ux_api_bt_control_play_t playCtrl)
{
    int ret;
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[PlayerModel] SetBTMusicPlay");
    if (SettingBluetoothModel::GetInstance()->GetLastConnedAddr() == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[PlayerModel::SetBTMusicPlay] GetLastConnedAddr is nullptr");
        return;
    }
    BtAvrcpTgInf *bt_avrcp_srv_inf = BtAvrcpSrvGetInfo();
    bd_addr_t *bt_avrcp_addr = &bt_avrcp_srv_inf->bd_addr;

    if (reinterpret_cast<ux_api_bt_addr_t *>(bt_avrcp_addr) == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "bt_avrcp_addr failed");
        return;
    }
    if (playCtrl == UX_API_BT_CONTROL_PLAY_START) {
        // 设置耳机的状态为播放
        ret = static_cast<int>(ux_api_bt_audio_start(UX_API_BT_PROFILE_A2DP,
            reinterpret_cast<ux_api_bt_addr_t *>(bt_avrcp_addr)));
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "[PlayerModel::SetBTMusicPlay] A2dpSrcStartPlaying ret:%x", ret);
    } else if (playCtrl == UX_API_BT_CONTROL_PLAY_STOP || playCtrl == UX_API_BT_CONTROL_PLAY_PAUSE) {
        // 设置耳机的状态暂停
        ret = static_cast<int>(ux_api_bt_audio_stop(UX_API_BT_PROFILE_A2DP,
            reinterpret_cast<ux_api_bt_addr_t *>(bt_avrcp_addr)));
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "[PlayerModel::SetBTMusicPlay] A2dpSrcSuspendPlaying ret:%x", ret);
    }
}
// 蓝牙连接和本地的音乐播放
int32_t PlayersModel::ConnectBtAndLocalPlay(int32_t connCtState, int32_t connTgState, int32_t playerState)
{
    int ret;
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[PlayerModel] ConnectBtAndLocalPlay connCtState = %d connTgState = %d",
        connCtState, connTgState);
    ux_api_bt_control_play_t playCtrl = UX_API_BT_CONTROL_PLAY_START;
    if (PlayersModel::GetInstance()->GetPlayerCtr() == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[PlayersModel::ConnectBtAndLocalPlay] BT GetPlayerCtr is nullptr ");
        return -1;
    }
    if (connCtState == UX_API_BT_PROFILE_STATE_CONNECTED || connTgState == UX_API_BT_PROFILE_STATE_CONNECTED) {
        if (playerState == PlayerStates::PLAYER_PAUSED) {
            if (playerInterruptListener == nullptr) {
                WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[PlayersModel::ConnectBtAndLocalPlay] playerInterruptListener is nullptr");
                return -1;
            }
            if (playerInterruptListener->IsInterruptedPaused()) {
                WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "Paused by interrupt, dont resume");
                return -1;
            }
            ret = PlayersModel::GetInstance()->GetPlayerCtr()->Play(); // 暂停后的播放
            if (ret == 0) { // 仅在播放成功后去开启toggle
                PlayersModel::GetInstance()->SetBTMusicPlay(playCtrl);
                playerInterruptListener->SetUserPaused(false);
            }
            WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "[PlayersModel::ConnBtAndLocalPlay] Paused Play ret:%d", ret);
        } else {
            ret = PlayersModel::GetInstance()->PlayerStat(); // 蓝牙连接播放
            if (ret == 0) { // 仅在播放成功后去开启toggle
                PlayersModel::GetInstance()->SetBTMusicPlay(playCtrl);
            }
            WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "[PlayersModel::ConnBtAndLocalPlay] PlayerStat ret:%d", ret);
        }
    } else {
        if (playerState == PlayerStates::PLAYER_PAUSED) {
            if (playerInterruptListener == nullptr) {
                WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[PlayersModel::ConnectBtAndLocalPlay] playerInterruptListener is nullptr");
                return -1;
            }
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "PLAYER_PAUSED play, 122.");
            if (playerInterruptListener->IsInterruptedPaused()) {
                WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "Paused by interrupt, dont resume");
                return -1;
            }
            ret = PlayersModel::GetInstance()->GetPlayerCtr()->Play();
            playerInterruptListener->SetUserPaused(false);
        } else {
            ret = PlayersModel::GetInstance()->PlayerStat(); // 本地播放
            if (ret != 0) {
                WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PlayerStat err, ret:%d", ret);
            }
        }
    }
    return ret;
}

// 蓝牙连接和本地音乐的暂停
int32_t PlayersModel::ConnectBtAndLocalPause(int32_t connCtState, int32_t connTgState)
{
    int32_t ret = 0;
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[PlayerModel] ConnectBtAndLocalPause");
    ux_api_bt_control_play_t playCtrl = UX_API_BT_CONTROL_PLAY_PAUSE;
    if (connCtState == UX_API_BT_PROFILE_STATE_CONNECTED || connTgState == UX_API_BT_PROFILE_STATE_CONNECTED) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "state == BTUNSELECTED, 105.");
        if (PlayersModel::GetInstance()->GetPlayerCtr() == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[PlayersModel::ConnectBtAndLocalPause] BT GetPlayerCtr is nullptr");
            return -1;
        }
        ret = PlayersModel::GetInstance()->GetPlayerCtr()->Pause(); // 蓝牙连接暂停
        if (ret == 0) { // 仅在暂停成功后去设置toggle
            PlayersModel::GetInstance()->SetBTMusicPlay(playCtrl);
            playerInterruptListener->SetUserPaused(true);
        }
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "BT Music Pause, ret:%d", ret);
    } else {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "state == UNSELECTED, 105.");
        ret = PlayersModel::GetInstance()->GetPlayerCtr()->Pause(); // 本地暂停
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "Pause, ret:%d", ret);
        playerInterruptListener->SetUserPaused(true);
    }
    return ret;
}

void PlayersModel::ClearActivateAudioInterrupt()
{
    isActivateAudioInterrupt = false;
}

void  PlayersModel::GetDuration(int64_t &duration)
{
    duration = duration_;
}

}