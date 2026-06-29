/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * File
 * Description:
 */
#ifndef UX_BT_HAL_HFP_H
#define UX_BT_HAL_HFP_H

#include "stdio.h"
#include "stdint.h"
#include "stdbool.h"
#include "ux_errcode.h"
#include "ux_bg_config.h"
#include "ux_api_bt_control.h"
#include "errcode.h"

#if defined(__cplusplus) && __cplusplus
extern "C" {
#endif /* __cplusplus */

#define UX_BT_HAL_CALL_PHONENUM_MAX_DIGITS 32

typedef enum {
    UX_BT_HAL_HFP_ROLE_HF,
    UX_BT_HAL_HFP_ROLE_AG,
    UX_BT_HAL_HFP_ROLE_MAX,
} ux_bt_hal_hfp_role_t;


typedef enum {
    UX_BT_HAL_CALL_STATE_ACTIVE = 0, /*!< @if Eng HFP phone calling state of active @else HFP HF正在通话 @endif */
    UX_BT_HAL_CALL_STATE_HELD,       /*!< @if Eng HFP phone calling state of hold @else HFP HF呼叫保持通话 @endif */
    UX_BT_HAL_CALL_STATE_DIALING,    /*!< @if Eng HFP phone calling state of dialing @else HFP HF呼叫正在拨号 @endif */
    UX_BT_HAL_CALL_STATE_ALERTING,   /*!< @if Eng HFP phone calling state of alerting @else HFP HF呼叫正在响铃 @endif */
    UX_BT_HAL_CALL_STATE_INCOMING,   /*!< @if Eng HFP phone calling state of incoming @else HFP HF呼叫接入 @endif */
    UX_BT_HAL_CALL_STATE_WAITING,    /*!< @if Eng HFP phone calling state of waiting @else HFP HF呼叫等待 @endif */
    UX_BT_HAL_CALL_STATE_RESPONSE_HELD, /*!< @if Eng HFP phone calling state of respond hold
                                          @else HFP HF呼叫响应保持 @endif */
    UX_BT_HAL_CALL_STATE_FINISHED,       /*!< @if Eng HFP phone calling state of finshed @else HFP HF呼叫结束 @endif */
    UX_BT_HAL_AUDIO_STATE_ACTIVE,
    UX_BT_HAL_AUDIO_STATE_NOT_READY,
    UX_BT_HAL_AUDIO_STATE_IDLE,
    UX_BT_HAL_CALL_STATE_MAX
} ux_bt_hal_call_state_t;

typedef struct {
    uint8_t id;                     /*!< @if Eng Call index @else 编号 @endif */
    ux_bt_hal_call_state_t state;   /*!< @if Eng Call state see ux_bt_hal_call_state_t
                                    @else  电话呼叫状态 参考 @ref ux_bt_hal_call_state_t @endif */
    bool multi_party;         /*!< @if Eng Is multiparty flag @else 是否多方电话 @endif */
    bool outgoing;            /*!< @if Eng Incoming/outing flag @else 呼入/呼出 @endif */
    uint8_t number_len;       /*!< @if Eng Call number length @else  电话号码位数 @endif */
    uint8_t number[UX_BT_HAL_CALL_PHONENUM_MAX_DIGITS];             /*!< @if Eng Call number @else 电话号码 @endif */
} ux_bt_hal_call_cat1_info_t;

typedef enum {
    UX_BT_HAL_HFP_STATE_CONNECTING = 0x1,    /*!< @if Eng status of connecting. @else 正在连接。 @endif */
    UX_BT_HAL_HFP_STATE_CONNECTED,     /*!< @if Eng status of connected. @else 已连接。 @endif */
    UX_BT_HAL_HFP_STATE_DISCONNECTING, /*!< @if Eng status of disconnecting. @else 正在断连。 @endif */
    UX_BT_HAL_HFP_STATE_DISCONNECTED,  /*!< @if Eng status of disconnected. @else 已断连。 @endif */
    UX_BT_HAL_HFP_STATE_MAX,
} ux_bt_hal_hfp_connect_state_t;

typedef struct {
    ux_api_bt_volume_type_t volume_type;
    uint8_t volume;
    uint32_t toggle_cnt;    /* toggle 值 */
    bool toggle_valid_flag;    /* 1: toggle_cnt 有效，需要跟对耳同步音量； 0: toggle_cnt 无效，不需要跟对耳同步音量 */
} ux_bt_hal_volume_info_t;

/* 连接状态回调 */
typedef void (*ux_bt_hal_hfp_conn_state_cbk)(const ux_api_bt_addr_t *addr,
    ux_bt_hal_hfp_role_t hfp_role, ux_bt_hal_hfp_connect_state_t conn_state);
/* 音量设置回调 */
typedef void (*ux_bt_hal_call_set_volume_request_cbk)(const ux_api_bt_addr_t *addr,
    const ux_bt_hal_volume_info_t *volume_info);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif