/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingBtHeadset.
* Created: 2025-09-30
 */
#ifndef SETTING_BT_HEADSET_H
#define SETTING_BT_HEADSET_H

#include "bts_def.h"
#include "bts_br_gap.h"
#include "td_type.h"
#include "ux_api_bt.h"
#include "ux_api_bt_control.h"
#include "bt_audio.h"
#ifdef __cplusplus
extern "C" {
#endif

enum {
    AVRCP_SRV_PRESS_VALUE,
    AVRCP_SRV_RELEASED_VALUE,
    AVRCP_SRV_INIT_VALUE
};

typedef struct {
    td_pvoid stream_hdl;
    uint8_t loc_volume;
    uint8_t play_stat;
    uint8_t dir_forward;
    uint8_t avrcp_button_flag;
    bd_addr_t bd_addr;
    uint8_t reg_cap_auto;
    uint8_t resrv;
    profile_connect_state_t avrcp_srv_conn_stat;
} BtAvrcpTgInf;

void UsingPressButtonStatusCallback(td_u32 keyOperation, td_u32 keyValue);
void SetAvrcpSrcInfAddr(const bd_addr_t &bdAddr);
void AvrcpTgPlayControlProcess(ux_api_bt_control_play_t PlayControl);
void AvrcpTgVolumeControlProcess(uint8_t AvrcpVolume);
ux_api_bt_profile_state_t GetA2dpProfileState(void);
BtAvrcpTgInf *BtAvrcpSrvGetInfo(void);
#ifdef __cplusplus
}
#endif
#endif // SETTING_BT_HEADSET_H