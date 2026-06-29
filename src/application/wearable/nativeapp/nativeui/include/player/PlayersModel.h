/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: Player view
 * Author:
 * Create: 2021-07-26
 */

#ifndef PLAYERS_MODEL_H
#define PLAYERS_MODEL_H

#include <cstdio>
#include "gfx_utils/list.h"
#include "main/UiServersMsg.h"
#include "player.h"
#include "source.h"
#include "audio_manager.h"
#include "audio_base_type.h"
#include "player/PlayersPresenter.h"
#include "bts_avrcp_controller.h"
#include "ux_api_bt_control.h"

namespace OHOS {
using OHOS::Media::Player;
using OHOS::Media::Source;
using namespace OHOS::Media;
using namespace::Audio;
using Audio::AudioManager;

constexpr int ERR_RET = -1;

enum PlayerModelState : uint8_t {
    LOOP,
    SEQUENCE,
    REPEATS,
    RANDOM,
};

enum {
    NORMAL,
    NONE_SONG_PLAY,
    SONG_ALREADY_PLAY,
    SONG_ALREADY_PAUSE,
    SONG_ALREADY_STOP,
    EXECUTE_FUNC_FAILED,
    NO_SUPPORT_SCENE,
    PLAYER_RESPONESE_MAX
};

class PlayerInterruptListener;
typedef struct {
    uint16 playerModel;
    uint32 value;
} PlayersMsg;

class PlayersModel {
public:
    PlayersModel();
    virtual ~PlayersModel();
    static PlayersModel *GetInstance();
    void PlayersPopFront(void);
    bool FetchedPlayersCallBack(PlayersMsg *playersMsg);
    void PushPlayersData(PlayersMsg playersMsg);
    int PlayerInit(void);
    int PlayerStat(void);
    std::string GetPlayerUri(void);
    void AudioInit(void);
    void SetPlayerUri(std::string src);
    void SetCircleProgress(int64_t value);
    void SetPlayButtonState(bool value);
    int16_t GetPlayerModel(void);
    void SetPlayerModel(int16_t value);
    void SetPlayerModelAddOne(void);
    int16_t GetCurrentIndex(void);
    void SetCurrentIndex(int16_t value);
    std::shared_ptr<Player> GetPlayerCtr();
    bool GetAudioInitStatus(void);
    void LoopingPlayout(void);
    void PreviousPlayout(void);
    void PlaySwitching(void);
    std::shared_ptr<PlayerInterruptListener> GetPlayerIntrptListener(void);
    int GetPlayerState(void);
    bool HasPlayerPlayed(void);
    int PlayerPlay(void);
    int PlayerPause(void);
    int PlayerStop(void);
    int PreSongOperate(void);
    int NextSongOperate(void);
    int IncreaseVolume(void);
    int DecreaseVolume(void);
    int32_t GetPlayerstatus();
    void SetBTMusicPlay(ux_api_bt_control_play_t playCtrl);
    int32_t ConnectBtAndLocalPlay(int32_t connCtState, int32_t connTgState, int32_t playerState); // 蓝牙连接和本地播放
    int32_t ConnectBtAndLocalPause(int32_t connCtState, int32_t connTgState);
    void ConncetBtResetButton();
    void ClearActivateAudioInterrupt(void);
    void GetDuration(int64_t &duration);

    void SetViewStatus(bool viewStatus) { viewInitStatus_ = viewStatus; };
    bool GetViewStatus(void) { return viewInitStatus_;};
private:
    int64_t duration_{0};
    bool isInterruptedPause_{false};
    bool audioInitStatus{false};
    std::string sliceId;
    List<PlayersMsg> listPlayers;
    std::string uri;
    std::shared_ptr<Player> playerCtr{nullptr};
    std::shared_ptr<Source> source{nullptr};
    std::map<std::string, std::string> header;
    std::shared_ptr<PlayerCallback> playerCallback{nullptr};
    AudioSession sessionId = 0;
    AudioInterrupt interrupt = {TYPE_DEFAULT, 0, nullptr};
    std::shared_ptr<PlayerInterruptListener> playerInterruptListener{nullptr};
    int64_t circleProgress{0};
    bool playButtonState{false};
    void SequencePlayout(void);
    void RandomPlayout(void);
    void NextSong(int16_t index);
    int16_t currentIndex{0};
    int16_t playerModel{0};
    char *currentSongName{nullptr};
    bool isStopAndReset{false};
    bool isActivateAudioInterrupt{false};
    bool viewInitStatus_ = false;
};
}

#endif // PLAYERS_MODEL_H
