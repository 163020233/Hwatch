/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingBtHeadset
 * Create: 2025-9-30
 */

#include "settings/model/SettingBtHeadset.h"
#include "bts_avrcp_target.h"
#include "bts_avrcp_controller.h"
#include "ux_api_bt_control.h"
#include "player/PlayersModel.h"
#include "graphic_service.h"

using namespace OHOS;

#ifdef __cplusplus
extern "C" {
#endif

#define AVRCP_CT_AUTO_REG_TYPE    1
uint8_t g_avrcp_tg_local_media_vol = AVRCP_ABSOLUTE_VOLUME_INVALID;
#define BT_A2DP_MEDIA_VOL_LEVEL 100
#define BT_AVRCP_STANDARD_VOL_LEVEL 127
BtAvrcpTgInf g_avrcp_srv_inf;

void SetAvrcpSrcInfAddr(const bd_addr_t &bdAddr)
{
    (void)memcpy_s(g_avrcp_srv_inf.bd_addr.addr, UX_API_BT_ADDR_LEN, bdAddr.addr, UX_API_BT_ADDR_LEN);
}

void AvrcpTgPlayControlProcess(ux_api_bt_control_play_t PlayControl)
{
    switch (PlayControl) {
        case UX_API_BT_CONTROL_PLAY_START: // 播放
            WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "A2dp src play start.");
            (void)PlayersModel::GetInstance()->PlayerPlay();
            PlayersModel::GetInstance()->SetBTMusicPlay(UX_API_BT_CONTROL_PLAY_START);
            break;
        case UX_API_BT_CONTROL_PLAY_PAUSE: // 暂停
            (void)PlayersModel::GetInstance()->PlayerPause();
            PlayersModel::GetInstance()->SetBTMusicPlay(UX_API_BT_CONTROL_PLAY_PAUSE);
            break;
        case UX_API_BT_CONTROL_PLAY_STOP: // 停止
            WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "A2dp src play stop.");
            (void)PlayersModel::GetInstance()->PlayerStop();
            PlayersModel::GetInstance()->SetBTMusicPlay(UX_API_BT_CONTROL_PLAY_STOP);
            break;
        case UX_API_BT_CONTROL_PLAY_NEXT: // 下一曲
            WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "A2dp src play next one.");
            PlayersModel::GetInstance()->LoopingPlayout();
            (void)PlayersModel::GetInstance()->PlayerPlay();
            PlayersModel::GetInstance()->SetBTMusicPlay(UX_API_BT_CONTROL_PLAY_START);
            break;
        case UX_API_BT_CONTROL_PLAY_PRE: // 上一曲
            WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "A2dp src play pre one.");
            PlayersModel::GetInstance()->PreviousPlayout();
            (void)PlayersModel::GetInstance()->PlayerPlay();
            PlayersModel::GetInstance()->SetBTMusicPlay(UX_API_BT_CONTROL_PLAY_START);
            break;
        default:
            break;
    }
}

void AvrcpTgVolumeControlProcess(uint8_t AvrcpVolume)
{
    uint8_t media_volume =  ((AvrcpVolume * BT_A2DP_MEDIA_VOL_LEVEL) / BT_AVRCP_STANDARD_VOL_LEVEL);
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "NotifyVolume = %d", media_volume);
    AudioManager::GetInstance().SetVolume(AUDIO_STREAM_MUSIC, media_volume); // 耳机连接手表场景调节手表音量
}

BtAvrcpTgInf *BtAvrcpSrvGetInfo(void)
{
    return &g_avrcp_srv_inf;
}

#ifdef __cplusplus
}
#endif