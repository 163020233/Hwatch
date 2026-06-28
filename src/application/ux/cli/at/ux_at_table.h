/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 *
 * Description: Register at commands \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-04-21， Create file. \n
 */
#ifndef UX_AT_TABLE_H
#define UX_AT_TABLE_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

enum {
    UX_ATCMD_NUM_POWER,
    UX_ATCMD_NUM_BUTTON,
    UX_ATCMD_NUM_BOX,
    UX_ATCMD_NUM_PAIR,
    UX_ATCMD_NUM_UNPAIR,
    UX_ATCMD_NUM_BTCFG,
    UX_ATCMD_NUM_SLECFG,
    UX_ATCMD_NUM_BSCFG,
    UX_ATCMD_NUM_AUDIOCFG,
    UX_ATCMD_NUM_RECOVERY,
    UX_ATCMD_NUM_UPDATE,
    UX_ATCMD_NUM_EARPHONE,
    UX_ATCMD_NUM_CALL,
    UX_ATCMD_NUM_MUSIC,
    UX_ATCMD_NUM_TONE,
    UX_ATCMD_NUM_PLAYTONE,
    UX_ATCMD_NUM_PLAYBACK,
    UX_ATCMD_NUM_SOUND,
    UX_ATCMD_NUM_PROFILE,
    UX_ATCMD_NUM_MUTE,
    UX_ATCMD_NUM_LED,
    UX_ATCMD_NUM_SYS,
    UX_ATCMD_NUM_FTM,
    UX_ATCMD_NUM_FTM_BT_ENABLE,
    UX_ATCMD_NUM_HAID,
    UX_ATCMD_NUM_UXTEST,
    UX_ATCMD_NUM_CUST,
    UX_ATCMD_NUM_PBAP,
#ifdef MELODY_DEMO_EARPHONE
    UX_ATCMD_NUM_BT_SIGNAL_ENABLE,
#endif
#ifdef DEMO_WATCH
    UX_ATCMD_NUM_VERSION,
#endif
#ifdef FT_SUPPORT_SN
    UX_ATCMD_NUM_SN,
#endif
    UX_ATCMD_NUM_MAX = 0xFFFF,
};

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif