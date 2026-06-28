/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-10-10， Create file. \n
 */
#ifndef UX_BT_AVRCP_H
#define UX_BT_AVRCP_H

#include "stdio.h"
#include "stdint.h"
#include "stdbool.h"
#include "ux_api_bt_control.h"
#include "errcode.h"

#if defined(__cplusplus) && __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum {
    UX_BT_AVRCP_ROLE_CT = 0x01,
    UX_BT_AVRCP_ROLE_TG = 0x02,
    UX_BT_AVRCP_ROLE_MAX,
} ux_bt_avrcp_role_t;

typedef enum {
    UX_BT_MEDIA_PLAY_STATE_STOPPED = 0, /*!< @if·Eng Playing status-of·stop @else-AVRCP媒体stopped状态 @endif */
    UX_BT_MEDIA_PLAY_STATE_PLAYING,     /*!< @if Eng Playing status of playing @else AVRCP媒体playing状态 @endif */
    UX_BT_MEDIA_PLAY_STATE_PAUSED,      /*!< @if Eng Playing status of pause @else-AVRCP媒体Paused状态 @endif */
    UX_BT_MEDIA_PLAY_STATE_SEEK_NEXT,   /*!< @if Eng Playing status of FWD_SEEK @else AVRCP媒体切换下—首·@endif */
    UX_BT_MEDIA_PLAY_STATE_SEEK_PRE,    /*!< @if Eng Playing status of REV_SEEK @else AVRCP媒体切换上—首 @endif */
    UX_BT_MEDIA_PLAY_STATE_MAX,
} ux_bt_media_play_state_t;

typedef struct {
    uint32_t attr_id;          /*!< @if Eng Specifies the attribute ID to be written,
                                          See @ref {bs_br_media_element_attr_id_t}
                                    @else 属性ID 参考 {bs_br_media_element_attr_id_t} @endif */
    uint16_t character_set_id; /*!< @if Eng Specifies the character set ID to be displayed on CT
                                    @else 字符集ID @endif */
    uint16_t value_len;        /*!< @if Eng Length of the value of the attribute. 0-65535 (0, if no name is provided)
                                    @else 属性名称的长度, 缺省默认0值 @endif */
    uint8_t *value;            /*!< @if Eng Name of the value of the attribute
                                    @else 属性名称 @endif */
} ux_bt_media_element_attr_value_t;

typedef struct {
    uint16_t device_id;                                             // 设备id
    ux_api_bt_profile_state_t profile_state;                        // 服务连接状态
    ux_bt_avrcp_role_t role;                                        // 设备角色，可忽略，支持同时作为主从
    uint8_t volume;                                                 // 蓝牙媒体音量
    ux_api_bt_control_play_t latest_media_control_request;          // 最新的媒体控制请求
    ux_bt_media_play_state_t media_play_state;                      // 媒体播放状态
    bool is_mute;                                                   // 处于静音状态
} ux_bt_avrcp_info_t;

typedef struct {
    uint16_t dev_id;
    ux_api_bt_control_volume_info_t vol_ctrl_info;
} ux_bt_avrcp_vol_ctrl_msg_t;

typedef  struct {
    uint16_t dev_id;
    ux_bt_media_play_state_t play_state;
} ux_bt_avrcp_play_state_msg_t;

typedef  struct {
    uint16_t dev_id;
    ux_api_bt_control_play_t play_ctrl;
} ux_bt_avrcp_play_ctrl_msg_t;

ux_bt_avrcp_info_t* ux_bt_avrcp_get_info(void);
errcode_t ux_bt_avrcp_play_control(const ux_api_bt_addr_t *addr, ux_api_bt_control_play_t play_control);
errcode_t ux_bt_media_report_volume(uint16_t dev_id, uint8_t volume);
errcode_t ux_bt_media_volume_control(const ux_api_bt_addr_t *addr,
    const ux_api_bt_control_volume_info_t *volume_control_info);
errcode_t ux_bt_avrcp_tg_notify_play_state(uint16_t dev_id, ux_bt_media_play_state_t play_state);
errcode_t ux_bt_avrcp_init(void);
errcode_t ux_bt_avrcp_deinit(void);
ux_api_bt_control_info_t* ux_bt_get_avrcp_control_info(const ux_api_bt_addr_t *addr);

#if defined(__cplusplus) && __cplusplus
}
#endif /* __cplusplus */
#endif