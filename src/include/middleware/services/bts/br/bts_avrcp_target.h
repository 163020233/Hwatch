/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022. All rights reserved.
 * Description: BT AVRCP target module.
 */

/**
 * @defgroup bluetooth_bts_avrcp_target AVRCP TG API
 * @ingroup  bluetooth
 * @{
 */

#ifndef BTS_AVRCP_TARGET_H
#define BTS_AVRCP_TARGET_H

#include "errcode.h"
#include "bts_def.h"
#include "bts_br_gap.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @if Eng
 * @brief Enum of passthrough button state.
 * @else
 * @brief 透传命令按钮状态类型。
 * @endif
 */
typedef enum {
    AVRCP_BUTTON_STATE_PUSHED = 0x0,    /*!< @if Eng Pushed state.
                                            @else   按下状态。 @endif */
    AVRCP_BUTTON_STATE_RELEASED = 0x1    /*!< @if Eng Released state.
                                            @else   释放状态。 @endif */
} avrcp_passthrough_button_state_t;

/**
 * @if Eng
 * @brief The struct of passthrough command.
 * @else
 * @brief 透传命令消息格式。
 * @endif
 */
typedef struct {
    uint8_t     state_flag;     /*!< @if Eng button state, @See avrcp_passthrough_button_state_t.
                                    @else   按钮状态,@参考 avrcp_passthrough_button_state_t @endif */
    uint8_t     op_id;          /*!< @if Eng key operation id, @See avrcp_key_operation_t.
                                    @else   按键操作ID，@参考 avrcp_key_operation_t @endif */
} avrcp_passthrough_cmd_t;

/**
 * @if Eng
 * @brief The struct of register notify request.
 * @else
 * @brief 注册通知请求消息格式。
 * @endif
 */
typedef struct {
    uint8_t event_id;           /*!< @if Eng button state, @See avrcp_notify_event_t.
                                    @else 按钮状态，@参考 avrcp_notify_event_t. @endif */
    uint32_t playback_interval; /*!< @if Eng playback interval,only for AVRCP_NOTIFY_EVENT_PLAYBACK_POS_CHANGED.
                                    @else 播放时隙(秒)，仅限AVRCP_NOTIFY_EVENT_PLAYBACK_POS_CHANGED. @endif */
} avrcp_register_notif_req_t;

/**
 * @if Eng
 * @brief  Callback invoked in response to avrcp tg connection state changed.
 * @par Description:
 *           When registered,the bts uses this callback to deliver avrcp tg connect stat info updated
 *           after connection state updated.
 * @attention  1. This function is called in bts context,should not be blocked or do long time waiting.
 * @attention  2. The memories of pointer are requested and freed by the bts automatically.
 * @param  [in]  bdAddr Type  BdAddr *, a poniter of the peer bluetooth device addresss.
 * @param  [in]  state  Type  BtProfileConnectState avtcp tg connection state.
 * @retval void  no return value.
 * @par Dependency:
 * @li bts_avrcp_target.h
 * @see  avrcp_tg_callbacks_t
 * @else
 * @brief  avrcp tg连接状态改变回调函数。
 * @par 说明:注册该回调函数之后，BTS每次完成avrcp tg状态更新后调用该接口反馈结果给上层。
 * @attention  1. 该回调函数运行于bts线程，不能阻塞或长时间等待。
 * @attention  2. 指针由bts申请内存，也由bts释放，回调中不应释放。
 * @param  [in]  bdAddr Type  BdAddr *, 对端蓝牙设备的地址信息。
 * @param  [in]  state  Type  BtProfileConnectState ,avrcp tg连接状态，参考 {@link profile_connect_state_t}。
 * @retval void  无返回值。
 * @par 依赖:
 * @li bts_avrcp_target.h
 * @see  avrcp_tg_callbacks_t
 * @endif
 */
typedef void (*avrcp_tg_conn_state_changed_callback)(const bd_addr_t *bd_addr, profile_connect_state_t state);

/**
 * @if Eng
 * @brief  Callback invoked when avrcp tg receive passthrough command.
 * @par Description:
 *           When registered,the bts uses this callback to deliver avrcp tg receive passthrough command.
 * @attention  1. This function is called in bts context,should not be blocked or do long time waiting.
 * @attention  2. The memories of pointer are requested and freed by the bts automatically.
 * @param  [in]  bd_addr a poniter of the peer bluetooth device addresss, @See bd_addr_t.
 * @param  [in]  cmd  a poniter of passthrough command info,@See avrcp_passthrough_cmd_t.
 * @retval void  no return value.
 * @par Dependency:
 * @li bts_avrcp_target.h
 * @see  avrcp_tg_callbacks_t
 * @else
 * @brief  avrcp tg透传命令请求回调函数。
 * @par 说明:注册该回调函数之后，BTS每次收到透传命令请求时，上报给上层。
 * @attention  1. 该回调函数运行于bts线程，不能阻塞或长时间等待。
 * @attention  2. 指针由bts申请内存，也由bts释放，回调中不应释放。
 * @param  [in]  bd_addr 对端蓝牙设备的地址信息，参考 {@link bd_addr_t}。
 * @param  [in]  cmd  透传命令请求信息，参考 {@link avrcp_passthrough_cmd_t}。
 * @retval void  无返回值。
 * @par 依赖:
 * @li bts_avrcp_target.h
 * @see  avrcp_tg_callbacks_t
 * @endif
 */
typedef void (*avrcp_tg_passthrough_ind_callback)(const bd_addr_t *bd_addr, avrcp_passthrough_cmd_t *cmd);

/**
 * @if Eng
 * @brief  Callback invoked when avrcp tg receive set volume command.
 * @par Description:
 *           When registered,the bts uses this callback to deliver avrcp tg receive set volume command.
 * @attention  1. This function is called in bts context,should not be blocked or do long time waiting.
 * @attention  2. The memories of pointer are requested and freed by the bts automatically.
 * @param  [in]  bd_addr a poniter of the peer bluetooth device addresss, @See bd_addr_t.
 * @param  [in]  volume  the volume value(region is 0x0-0x7F).
 * @retval void  no return value.
 * @par Dependency:
 * @li bts_avrcp_target.h
 * @see  avrcp_tg_callbacks_t
 * @else
 * @brief  avrcp tg配置绝对音量命令回调函数。
 * @par 说明:注册该回调函数之后，BTS每次收到配置绝对音量请求时，上报给上层。
 * @attention  1. 该回调函数运行于bts线程，不能阻塞或长时间等待。
 * @attention  2. 指针由bts申请内存，也由bts释放，回调中不应释放。
 * @param  [in]  bd_addr 对端蓝牙设备的地址信息，参考 {@link bd_addr_t}。
 * @param  [in]  volume  音量值（0x0-0x7F）.
 * @retval void  无返回值。
 * @par 依赖:
 * @li bts_avrcp_target.h
 * @see  avrcp_tg_callbacks_t
 * @endif
 */
typedef void (*avrcp_tg_set_volume_ind_callback)(const bd_addr_t *bd_addr, uint8_t volume);

/**
 * @if Eng
 * @brief  Callback invoked when avrcp tg receive register nofitiy command.
 * @par Description:
 *           When registered,the bts uses this callback to deliver avrcp tg receive register nofitiy command.
 * @attention  1. This function is called in bts context,should not be blocked or do long time waiting.
 * @attention  2. The memories of pointer are requested and freed by the bts automatically.
 * @param  [in]  bd_addr a poniter of the peer bluetooth device addresss, @See bd_addr_t.
 * @param  [in]  req  a poniter of register nofitiy command info,@See avrcp_register_notif_req_t.
 * @retval void  no return value.
 * @par Dependency:
 * @li bts_avrcp_target.h
 * @see  avrcp_tg_callbacks_t
 * @else
 * @brief  avrcp tg注册通知请求回调函数。
 * @par 说明:注册该回调函数之后，BTS每次收到注册通知请求时，上报给上层。
 * @attention  1. 该回调函数运行于bts线程，不能阻塞或长时间等待。
 * @attention  2. 指针由bts申请内存，也由bts释放，回调中不应释放。
 * @param  [in]  bd_addr 对端蓝牙设备的地址信息，参考 {@link bd_addr_t}。
 * @param  [in]  volume  注册通知请求信息，参考 {@link avrcp_register_notif_req_t}。
 * @retval void  无返回值。
 * @par 依赖:
 * @li bts_avrcp_target.h
 * @see  avrcp_tg_callbacks_t
 * @endif
 */
typedef void (*avrcp_tg_reg_notify_ind_callback)(const bd_addr_t *bd_addr, avrcp_register_notif_req_t *req);

/**
 * @if Eng
 * @brief Struct of avrcp tg callback function.
 * @else
 * @brief avrcp tg回调接口定义。
 * @endif
 */
typedef struct {
    avrcp_tg_conn_state_changed_callback conn_state_changed_cb;
    avrcp_tg_passthrough_ind_callback passthrough_cmd_cb;
    avrcp_tg_set_volume_ind_callback set_absolute_volume_cmd_cb;
    avrcp_tg_reg_notify_ind_callback register_notify_cb;
} avrcp_tg_callbacks_t;

/**
 * @if Eng
 * @brief  Use this funtion to connect to the peer bluetooth device of avrcp connection by address.
 * @par Description:
 *           Use this funtion to connect to the peer bluetooth device of avrcp connection by address.
 * @attention  NULL
 * @param  [in]  bdAddr Type  BdAddr *, a poniter of the peer bluetooth device addresss.
 * @retval ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval Other                    Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @li bts_avrcp_target.h
 * @else
 * @brief  与对端设备建立avrcp连接。
 * @par 说明:与对端设备建立avrcp连接。
 * @attention  NULL
 * @param  [in]  bdAddr Type  BdAddr *, 对端蓝牙设备的地址信息。
 * @retval ERRCODE_BT_SUCCESS   执行成功。
 * @retval Other               执行失败。参考 @ref errcode_bt_t
 * @par 依赖:
 * @li bts_avrcp_target.h
 * @endif
 */
errcode_t avrcp_tg_connect(const bd_addr_t *bd_addr);

/**
 * @if Eng
 * @brief  Use this funtion to disconnect  to the peer bluetooth device of avrcp connection by address.
 * @par Description:
 *           Use this funtion to connect to the peer bluetooth device of avrcp connection by address.
 * @attention  NULL
 * @param  [in]  bdAddr Type  BdAddr *, a poniter of the peer bluetooth device addresss.
 * @retval ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @li bts_avrcp_target.h
 * @else
 * @brief  断开与CT的连接。
 * @par 说明:与对端设备建立a2dp连接。
 * @attention  NULL
 * @param  [in]  bdAddr Type  BdAddr *, 对端蓝牙设备的地址信息。
 * @retval ERRCODE_BT_SUCCESS   执行成功。
 * @retval Other               执行失败。参考 @ref errcode_bt_t
 * @par 依赖:
 * @li bts_avrcp_target.h
 * @endif
 */
errcode_t avrcp_tg_disconnect(const bd_addr_t *bd_addr);

/**
 * @if Eng
 * @brief Use this funtion to enable the TG  to notify the CT if the playback status has been changed
 *        on the TG.
 * @par Description:
 *          Use this funtion to enable the TG  to notify the CT if the playback status has been changed
 *          on the TG.
 * @attention  1. This function is called in bts context when the event of AVRCP_NOTIFY_EVENT_PLAYBACK_STATUS_CHANGED
 * @attention     should be registered on the ct.
 * @param  [in]  playStatus playback status {@link AvrcpCtPlayStatus}
 * @retval void    no return value.
 * @par Dependency:
 * @li bts_avrcp_target.h
 * @else
 * @brief 通知CT端播放状态发生改变
 * @par 说明:通知CT端播放状态发生改变
 * @attention  1. 对应CT端注册的 {@link AVRCP_NOTIFY_EVENT_PLAYBACK_STATUS_CHANGED} 事件。
 * @param   [in]  playStatus 播放状态，取值参考 {@link AvrcpCtPlayStatus}
 * @retval void   无返回值。
 * @par 依赖:
 * @li bts_avrcp_target.h
 * @endif
 */
errcode_t avrcp_tg_notify_playback_status_changed(uint8_t play_status);

/**
 * @if Eng
 * @brief  Use this funtion to enable the TG  to notify the CT if the track has been changed
 *         on the TG.
 * @par Description:
 *          Use this funtion to enable the TG  to notify the CT if the track has been changed
 *          on the TG.
 * @attention  1. This function is called in bts context when the event of AVRCP_NOTIFY_EVENT_TRACK_CHANGED
 * @attention     should be registered on the ct.
 * @param  [in]  identifier Type  unsigned long long, identifier Unique Identifier to identify an element
 * @retval void    no return value.
 * @par Dependency:
 * @li bts_avrcp_target.h
 * @else
 * @brief  通知CT端track发生改变
 * @par 说明:通知CT端track发生改变。
 * @attention  1. 对应CT端注册的 {@link AVRCP_NOTIFY_EVENT_TRACK_CHANGED} 事件。
 * @param  [in]  identifier Type  unsigned long long,在TG端，当前element的唯一标识符.
 * @retval void   无返回值。
 * @par 依赖:
 * @li bts_avrcp_target.h
 * @endif
 */
errcode_t avrcp_tg_notify_track_changed(uint64_t identifier);

/**
 * @if Eng
 * @brief  Use this funtion to enable the TG  to notify the CT if the track has been end
 *         on the TG.
 * @par Description:
 *          Use this funtion to enable the TG  to notify the CT if the track has been end
 *          on the TG.
 * @attention  1. This function is called in bts context when the event of AVRCP_NOTIFY_EVENT_TRACK_REACHED_END
 * @attention      should be registered on the ct.
 * @param  NULL
 * @retval void    no return value.
 * @par Dependency:
 * @li bts_avrcp_target.h
 * @else
 * @brief  通知CT端track已播放结束。CNend
 * @par 说明:通知CT端track已播放开始。
 * @attention  1. 对应CT端注册的 {@link AVRCP_NOTIFY_EVENT_TRACK_REACHED_END} 事件。
 * @param  NULL
 * @retval void    no return value.
 * @par 依赖:
 * @li bts_avrcp_target.h
 * @endif
 */
errcode_t avrcp_tg_notify_track_reached_end(void);

/**
 * @if Eng
 * @brief  Use this funtion to enable the TG  to notify the CT if the track has been started
 *         on the TG.
 * @par Description:
 *          Use this funtion to enable the TG  to notify the CT if the track has been started
 *          on the TG.
 * @attention  1. This function is called in bts context when the event of AVRCP_NOTIFY_EVENT_TRACK_REACHED_START
 * @attention     should be registered on the ct.
 * @param  NULL
 * @retval void    no return value.
 * @par Dependency:
 * @li bts_avrcp_target.h
 * @else
 * @brief  通知CT端track已播放开始。
 * @par 说明:通知CT端track已播放开始。
 * @attention  1. 对应CT端注册的 {@link AVRCP_NOTIFY_EVENT_TRACK_REACHED_START} 事件。
 * @param   NULL
 * @retval void    无返回值。
 * @par 依赖:
 * @li bts_avrcp_target.h
 * @endif
 */
errcode_t avrcp_tg_notify_track_reached_start(void);

/**
 * @if Eng
 * @brief  Use this funtion to enable the TG to notify the CT if the current playback position has changed
 *          on the TG.
 * @par Description:
 *          Use this funtion to enable the TG  to notify the CT if the current playback position has changed
 *          on the TG.
 * @attention  1. This function is called in bts context when the event of AVRCP_NOTIFY_EVENT_PLAYBACK_POS_CHANGED
 * @attention     should be registered on the ct.
 * @attention  2. If no track currently selected, then return 0xFFFFFFFF in the INTERIM response.
 * @param  [in]  playbackPos Type  unsigned int, playbackPos Current playback position in millisecond.
 * @retval void    no return value.
 * @par Dependency:
 * @li bts_avrcp_target.h
 * @else
 * @brief  通知CT端播放位置发生改变。
 * @par 说明:通知CT端播放位置发生改变。
 * @attention  1. 对应CT端注册的 {@link AVRCP_NOTIFY_EVENT_PLAYBACK_POS_CHANGED} 事件。
 * @attention  2. 对应未CT端注册 {@link AVRCP_NOTIFY_EVENT_PLAYBACK_POS_CHANGED} 事件,默认返回0xFFFFFFFF.
 * @param  [in]  playbackPos Type  unsigned int, 当前播放位置(毫秒)。
 * @retval void    无返回值。
 * @par 依赖:
 * @li bts_avrcp_target.h
 * @endif
 */
errcode_t avrcp_tg_notify_playback_pos_changed(uint32_t playback_pos);

/**
 * @if Eng
 * @brief  Use this funtion to enable the TG  to notify the CT if the Now Playing Content has changed
 *         on the TG.
 * @par Description:
 *          Use this funtion to enable the TG  to notify the CT if the Now Playing Content has changed
 *          on the TG.
 * @attention  1. This function is called in bts context when the event of
 * @attention     AVRCP_NOTIFY_EVENT_NOW_PLAYING_CONTENT_CHANGED should be registered on the ct.
 * @param  NULL
 * @retval void    no return value.
 * @par Dependency:
 * @li bts_avrcp_target.h
 * @else
 * @brief  通知CT端NowPlaying folder发生改变。
 * @par 说明:通知CT端NowPlaying folder发生改变。
 * @attention  1. 对应CT端注册的 {@link AVRCP_NOTIFY_EVENT_NOW_PLAYING_CONTENT_CHANGED} 事件。
 * @param   NULL
 * @retval void   无返回值。
 * @par 依赖:
 * @li bts_avrcp_target.h
 * @endif
 */
errcode_t avrcp_tg_notify_now_playing_content_changed(void);

/**
 * @if Eng
 * @brief  Use this funtion to enable the TG  to notify the CT if a new player becomes available to
 *         be addressed on the TG.
 * @par Description:
 *          Use this funtion to enable the TG  to notify the CT if a new player becomes available to
 *          be addressed on the TG.
 * @attention   NULL AVRCP_NOTIFY_EVENT_AVAILABLE_PLAYERS_CHANGED
 * @param  NULL
 * @retval void    no return value.
 * @par Dependency:
 * @li bts_avrcp_target.h
 * @else
 * @brief  通知CT端有新的播放器可用。
 * @par Description:通知CT端有新的播放器可用。
 * @attention   NULL AVRCP_NOTIFY_EVENT_AVAILABLE_PLAYERS_CHANGED
 * @param  NULL
 * @retval void    无返回值。
 * @par Dependency:
 * @li bts_avrcp_target.h
 * @endif
 */
errcode_t avrcp_tg_notify_available_players_changed(void);

/**
 * @if Eng
 * @brief  Use this funtion to enable the TG  to notify the CT when the Addressed Players have changed on the TG
 *         register a notification.
 * @par Description:
 *          Use this funtion to enable the TG  to notify the CT when the addressed Players have changed on the TG
 *          register a notification.
 * @attention   1. This function is called in bts context when the event of
 * @attention      AVRCP_NOTIFY_EVENT_ADDRESSED_PLAYER_CHANGED
 * @attention      should be registered on the ct.
 * @param  [in]  uidCounter Type  unsigned short, the UID Counter of the currently browsed player.
 * @param  [in]  playerId   Type  unsigned short, a unique identifier for this media player.
 * @retval void    no return value.
 * @par Dependency:
 * @li bts_avrcp_target.h
 * @else
 * @brief  通知CT端Addressed player发生改变。
 * @par 说明:通知CT端Addressed player发生改变。
 * @attention  1. 对应CT端注册的 {@link AVRCP_NOTIFY_EVENT_ADDRESSED_PLAYER_CHANGED} 事件。
 * @param  [in]  uidCounter Type  unsigned short, 当前浏览的播放器的UID计数器 。
 * @param  [in]  playerId   Type  unsigned short, 当前播放器的id。
 * @retval void    无返回值。
 * @par 依赖:
 * @li bts_avrcp_target.h
 * @endif
 */
errcode_t avrcp_tg_notify_addressed_players_changed(uint16_t player_id, uint16_t uid_counter);

/**
 * @if Eng
 * @brief  Use this funtion to enable the CT to detect when the UIDs have changed on the TG it may
 *         register a notification.
 * @par Description:
 *          Use this funtion to enable the CT to detect when the UIDs have changed on the TG it may
 *          register a notification.
 * @attention   1. This function is called in bts context when the event of
 * @attention      AVRCP_NOTIFY_EVENT_ADDRESSED_PLAYER_CHANGED
 * @attention      should be registered on the ct.
 * @param  [in]  uidCounter Type  unsigned short, the UID Counter of the currently browsed player.
 * @retval void    no return value.
 * @par Dependency:
 * @li bts_avrcp_target.h
 * @else
 * @brief  通知CT端UID发生改变。
 * @par 说明:通知CT端UID发生改变。
 * @attention  1. 对应CT端注册的 {@link AVRCP_NOTIFY_EVENT_ADDRESSED_PLAYER_CHANGED} 事件。
 * @param  [in]  uidCounter Type  unsigned short, 当前浏览的播放器的UID计数器 。
 * @retval void    无返回值。
 * @par 依赖:
 * @li bts_avrcp_target.h
 * @endif
 */
errcode_t avrcp_tg_notify_uid_changed(uint16_t uid_counter);

/**
 * @if Eng
 * @brief  Use this funtion to notify the volume to the CT when the volume has been changed locally on the TG.
 * @par Description:
 *            This notification is used by the CT to detect when the volume has been changed locally on the TG,
 *            or what the actual volume level is following use of relative volume commands.
 * @attention  1. This function is called in bts context when the event of AVRCP_NOTIFY_EVENT_UIDS_CHANGED
 * @attention     should be registered on the ct.
 * @param  [in]  volume Type  unsigned char,this is the current Absolute Volume on the TG, range 0-0x7f.
 * @retval void    no return value.
 * @par Dependency:
 * @li bts_avrcp_target.h
 * @else
 * @brief  TG端通知CT端音量发生改变。
 * @par 说明:TG端通知CT端音量发生改变。
 * @attention  1. 对应CT端注册的 {@link AVRCP_NOTIFY_EVENT_UIDS_CHANGED} 事件。
 * @param  [in]  volume Type  unsigned char, TG端的当前绝对音量，范围0-0x7f。
 * @retval void    无返回值。
 * @par 依赖:
 * @li bts_avrcp_target.h
 * @endif
 */
errcode_t avrcp_tg_notify_volume_changed(uint8_t volume);

/**
 * @if Eng
 * @brief  Use this funtion to enable the TG  to notify the CT if the track has been end
 *         on the TG.
 * @par Description:
 *          Use this funtion to enable the TG  to notify the CT if the track has been end
 *          on the TG.
 * @attention  1. This function is called in bts context when the event of AVRCP_NOTIFY_EVENT_TRACK_REACHED_END
 * @attention      should be registered on the ct.
 * @param  [in]  bd_addr a poniter of the peer bluetooth device addresss, @See bd_addr_t.
 * @retval void    no return value.
 * @par Dependency:
 * @li bts_avrcp_target.h
 * @else
 * @brief  通知CT端track已播放结束。CNend
 * @par 说明:通知CT端track已播放开始。
 * @attention  1. 对应CT端注册的 {@link AVRCP_NOTIFY_EVENT_TRACK_REACHED_END} 事件。
 * @param  [in]  bd_addr 对端蓝牙设备的地址信息，参考 {@link bd_addr_t}。
 * @retval void    no return value.
 * @par 依赖:
 * @li bts_avrcp_target.h
 * @endif
 */
errcode_t avrcp_tg_notify_track_reached_end_multi(const bd_addr_t *bd_addr);

/**
 * @if Eng
 * @brief  Use this funtion to enable the TG  to notify the CT if the track has been started
 *         on the TG.
 * @par Description:
 *          Use this funtion to enable the TG  to notify the CT if the track has been started
 *          on the TG.
 * @attention  1. This function is called in bts context when the event of AVRCP_NOTIFY_EVENT_TRACK_REACHED_START
 * @attention     should be registered on the ct.
 * @param  [in]  bd_addr a poniter of the peer bluetooth device addresss, @See bd_addr_t.
 * @retval void    no return value.
 * @par Dependency:
 * @li bts_avrcp_target.h
 * @else
 * @brief  通知CT端track已播放开始。
 * @par 说明:通知CT端track已播放开始。
 * @attention  1. 对应CT端注册的 {@link AVRCP_NOTIFY_EVENT_TRACK_REACHED_START} 事件。
 * @param  [in]  bd_addr 对端蓝牙设备的地址信息，参考 {@link bd_addr_t}。
 * @retval void    无返回值。
 * @par 依赖:
 * @li bts_avrcp_target.h
 * @endif
 */
errcode_t avrcp_tg_notify_track_reached_start_multi(const bd_addr_t *bd_addr);

/**
 * @if Eng
 * @brief  Use this funtion to enable the TG to notify the CT if the current playback position has changed
 *          on the TG.
 * @par Description:
 *          Use this funtion to enable the TG  to notify the CT if the current playback position has changed
 *          on the TG.
 * @attention  1. This function is called in bts context when the event of AVRCP_NOTIFY_EVENT_PLAYBACK_POS_CHANGED
 * @attention     should be registered on the ct.
 * @attention  2. If no track currently selected, then return 0xFFFFFFFF in the INTERIM response.
 * @param  [in]  bd_addr a poniter of the peer bluetooth device addresss, @See bd_addr_t.
 * @param  [in]  playbackPos Type  unsigned int, playbackPos Current playback position in millisecond.
 * @retval void    no return value.
 * @par Dependency:
 * @li bts_avrcp_target.h
 * @else
 * @brief  通知CT端播放位置发生改变。
 * @par 说明:通知CT端播放位置发生改变。
 * @attention  1. 对应CT端注册的 {@link AVRCP_NOTIFY_EVENT_PLAYBACK_POS_CHANGED} 事件。
 * @attention  2. 对应未CT端注册 {@link AVRCP_NOTIFY_EVENT_PLAYBACK_POS_CHANGED} 事件,默认返回0xFFFFFFFF.
 * @param  [in]  bd_addr 对端蓝牙设备的地址信息，参考 {@link bd_addr_t}。
 * @param  [in]  playbackPos Type  unsigned int, 当前播放位置(毫秒)。
 * @retval void    无返回值。
 * @par 依赖:
 * @li bts_avrcp_target.h
 * @endif
 */
errcode_t avrcp_tg_notify_playback_pos_changed_multi(const bd_addr_t *bd_addr, uint32_t playback_pos);

/**
 * @if Eng
 * @brief  Use this funtion to enable the TG  to notify the CT if the Now Playing Content has changed
 *         on the TG.
 * @par Description:
 *          Use this funtion to enable the TG  to notify the CT if the Now Playing Content has changed
 *          on the TG.
 * @attention  1. This function is called in bts context when the event of
 * @attention     AVRCP_NOTIFY_EVENT_NOW_PLAYING_CONTENT_CHANGED should be registered on the ct.
 * @param  [in]  bd_addr a poniter of the peer bluetooth device addresss, @See bd_addr_t.
 * @retval void    no return value.
 * @par Dependency:
 * @li bts_avrcp_target.h
 * @else
 * @brief  通知CT端NowPlaying folder发生改变。
 * @par 说明:通知CT端NowPlaying folder发生改变。
 * @attention  1. 对应CT端注册的 {@link AVRCP_NOTIFY_EVENT_NOW_PLAYING_CONTENT_CHANGED} 事件。
 * @param  [in]  bd_addr 对端蓝牙设备的地址信息，参考 {@link bd_addr_t}。
 * @retval void   无返回值。
 * @par 依赖:
 * @li bts_avrcp_target.h
 * @endif
 */
errcode_t avrcp_tg_notify_now_playing_content_changed_multi(const bd_addr_t *bd_addr);

/**
 * @if Eng
 * @brief  Use this funtion to enable the TG  to notify the CT if a new player becomes available to
 *         be addressed on the TG.
 * @par Description:
 *          Use this funtion to enable the TG  to notify the CT if a new player becomes available to
 *          be addressed on the TG.
 * @attention   NULL AVRCP_NOTIFY_EVENT_AVAILABLE_PLAYERS_CHANGED
 * @param  [in]  bd_addr a poniter of the peer bluetooth device addresss, @See bd_addr_t.
 * @retval void    no return value.
 * @par Dependency:
 * @li bts_avrcp_target.h
 * @else
 * @brief  通知CT端有新的播放器可用。
 * @par Description:通知CT端有新的播放器可用。
 * @attention   NULL AVRCP_NOTIFY_EVENT_AVAILABLE_PLAYERS_CHANGED
 * @param  [in]  bd_addr 对端蓝牙设备的地址信息，参考 {@link bd_addr_t}。
 * @retval void    无返回值。
 * @par Dependency:
 * @li bts_avrcp_target.h
 * @endif
 */
errcode_t avrcp_tg_notify_available_players_changed_multi(const bd_addr_t *bd_addr);

/**
 * @if Eng
 * @brief  Use this funtion to enable the TG  to notify the CT when the Addressed Players have changed on the TG
 *         register a notification.
 * @par Description:
 *          Use this funtion to enable the TG  to notify the CT when the addressed Players have changed on the TG
 *          register a notification.
 * @attention   1. This function is called in bts context when the event of
 * @attention      AVRCP_NOTIFY_EVENT_ADDRESSED_PLAYER_CHANGED
 * @attention      should be registered on the ct.
 * @param  [in]  bd_addr a poniter of the peer bluetooth device addresss, @See bd_addr_t.
 * @param  [in]  uidCounter Type  unsigned short, the UID Counter of the currently browsed player.
 * @param  [in]  playerId   Type  unsigned short, a unique identifier for this media player.
 * @retval void    no return value.
 * @par Dependency:
 * @li bts_avrcp_target.h
 * @else
 * @brief  通知CT端Addressed player发生改变。
 * @par 说明:通知CT端Addressed player发生改变。
 * @attention  1. 对应CT端注册的 {@link AVRCP_NOTIFY_EVENT_ADDRESSED_PLAYER_CHANGED} 事件。
 * @param  [in]  bd_addr 对端蓝牙设备的地址信息，参考 {@link bd_addr_t}。
 * @param  [in]  uidCounter Type  unsigned short, 当前浏览的播放器的UID计数器 。
 * @param  [in]  playerId   Type  unsigned short, 当前播放器的id。
 * @retval void    无返回值。
 * @par 依赖:
 * @li bts_avrcp_target.h
 * @endif
 */
errcode_t avrcp_tg_notify_addressed_players_changed_multi(const bd_addr_t *bd_addr,
    uint16_t player_id, uint16_t uid_counter);

/**
 * @if Eng
 * @brief  Use this funtion to enable the CT to detect when the UIDs have changed on the TG it may
 *         register a notification.
 * @par Description:
 *          Use this funtion to enable the CT to detect when the UIDs have changed on the TG it may
 *          register a notification.
 * @attention   1. This function is called in bts context when the event of
 * @attention      AVRCP_NOTIFY_EVENT_ADDRESSED_PLAYER_CHANGED
 * @attention      should be registered on the ct.
 * @param  [in]  bd_addr a poniter of the peer bluetooth device addresss, @See bd_addr_t.
 * @param  [in]  uidCounter Type  unsigned short, the UID Counter of the currently browsed player.
 * @retval void    no return value.
 * @par Dependency:
 * @li bts_avrcp_target.h
 * @else
 * @brief  通知CT端UID发生改变。
 * @par 说明:通知CT端UID发生改变。
 * @attention  1. 对应CT端注册的 {@link AVRCP_NOTIFY_EVENT_ADDRESSED_PLAYER_CHANGED} 事件。
 * @param  [in]  bd_addr 对端蓝牙设备的地址信息，参考 {@link bd_addr_t}。
 * @param  [in]  uidCounter Type  unsigned short, 当前浏览的播放器的UID计数器 。
 * @retval void    无返回值。
 * @par 依赖:
 * @li bts_avrcp_target.h
 * @endif
 */
errcode_t avrcp_tg_notify_uid_changed_multi(const bd_addr_t *bd_addr, uint16_t uid_counter);

/**
 * @if Eng
 * @brief  Use this funtion to notify the volume to the CT when the volume has been changed locally on the TG.
 * @par Description:
 *            This notification is used by the CT to detect when the volume has been changed locally on the TG,
 *            or what the actual volume level is following use of relative volume commands.
 * @attention  1. This function is called in bts context when the event of AVRCP_NOTIFY_EVENT_UIDS_CHANGED
 * @attention     should be registered on the ct.
 * @param  [in]  bd_addr a poniter of the peer bluetooth device addresss, @See bd_addr_t.
 * @param  [in]  volume Type  unsigned char,this is the current Absolute Volume on the TG, range 0-0x7f.
 * @retval void    no return value.
 * @par Dependency:
 * @li bts_avrcp_target.h
 * @else
 * @brief  TG端通知CT端音量发生改变。
 * @par 说明:TG端通知CT端音量发生改变。
 * @attention  1. 对应CT端注册的 {@link AVRCP_NOTIFY_EVENT_UIDS_CHANGED} 事件。
 * @param  [in]  bd_addr 对端蓝牙设备的地址信息，参考 {@link bd_addr_t}。
 * @param  [in]  volume Type  unsigned char, TG端的当前绝对音量，范围0-0x7f。
 * @retval void    无返回值。
 * @par 依赖:
 * @li bts_avrcp_target.h
 * @endif
 */
errcode_t avrcp_tg_notify_volume_changed_multi(const bd_addr_t *bd_addr, uint8_t volume);

/**
 * @if Eng
 * @brief  Use this funtion to register callback function of framework.
 * @par Description:Use this funtion to register callback function of framework.
 * @attention  NULL
 * @param  [in]  func Type  avrcp_tg_callbacks_t *, a poniter of the callback funcs.
 * @retval ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @li bts_avrcp_target.h
 * @else
 * @brief  注册上层应用的回调。
 * @par 说明:注册上层应用的回调。
 * @attention  NULL
 * @param  [in] func Type  avrcp_tg_callbacks_t *, 指向回调函数接口定义的指针, 参考 {@link avrcp_tg_callbacks_t}。
 * @retval ERRCODE_BT_SUCCESS   执行成功。
 * @retval Other               执行失败。参考 @ref errcode_bt_t
 * @par 依赖:
 * @li bts_avrcp_target.h
 * @endif
 */
errcode_t avrcp_tg_register_callbacks(avrcp_tg_callbacks_t *func);

/**
 * @if Eng
 * @brief  Use this funtion to deregister callback function of framework.
 * @par Description:
 *           Use this funtion to deregister callback function of framework.
 * @param  [in]  func   A poniter of the callback function. See @ref avrcp_tg_callbacks_t
 * @retval ERRCODE_BT_SUCCESS   Success.
 * @retval Other                Failure. For details, see @ref errcode_bt_t
 * @else
 * @brief  去注册上层应用的回调。
 * @par 说明:
 *           去注册上层应用的回调。
 * @param  [in]  func   回调函数指针。参考 @ref avrcp_tg_callbacks_t
 * @retval ERRCODE_BT_SUCCESS   成功。
 * @retval Other                失败，参考 @ref errcode_bt_t
 * @endif
 * @par Dependency:
 * @li bts_a2dp_source.h
 */
errcode_t avrcp_tg_unregister_callbacks(void);

/**
 * @if Eng
 * @brief  Use this funtion to register tg supported notify event before connect.
 * @par Description:
 *            Use this funtion to register tg supported notify event before connect.
 * @attention  1. This function is called before connect.
 * @param  [in]  event_id,a point of event id list, event id @See avrcp_notify_event_t.
 * @param  [in]  event_num,the number of event id.
 * @retval void    no return value.
 * @par Dependency:
 * @li bts_avrcp_target.h
 * @else
 * @brief  TG端注册支持的通知事件（连接之前调用）。
 * @par 说明:TG端注册支持的通知事件（连接之前调用）。
 * @attention  1. This function is called before connect。
 * @param  [in]  event_id,事件id列表 @See avrcp_notify_event_t.
 * @param  [in]  event_num,事件ID个数.
 * @retval void    无返回值。
 * @par 依赖:
 * @li bts_avrcp_target.h
 * @endif
 */
errcode_t avrcp_tg_supported_event_register(uint8_t *event_id, uint8_t event_num);

/**
 * @if Eng
 * @brief  Use this funtion to register AVRCP TG service.
 * @par Description:
 *           Use this funtion to register AVRCP TG service.
 * @param  NULL
 * @retval ERRCODE_BT_SUCCESS   Success.
 * @retval Other                Failure. For details, see @ref errcode_bt_t
 * @else
 * @brief  注册AVRCP TG服务。
 * @par 说明:
 *           注册AVRCP TG服务。
 * @param   NULL
 * @retval ERRCODE_BT_SUCCESS   成功。
 * @retval Other                失败，参考 @ref errcode_bt_t
 * @endif
 * @par Dependency:
 * @li bts_avrcp_target.h
 */
errcode_t avrcp_tg_service_register(void);

/**
 * @if Eng
 * @brief  Use this funtion to unregister AVRCP TG service.
 * @par Description:
 *           Use this funtion to unregister AVRCP TG service.
 * @param  NULL
 * @retval ERRCODE_BT_SUCCESS   Success.
 * @retval Other                Failure. For details, see @ref errcode_bt_t
 * @else
 * @brief  去注册AVRCP TG服务。
 * @par 说明:
 *           去注册AVRCP TG服务。
 * @param   NULL
 * @retval ERRCODE_BT_SUCCESS   成功。
 * @retval Other                失败，参考 @ref errcode_bt_t
 * @endif
 * @par Dependency:
 * @li bts_avrcp_target.h
 */
errcode_t avrcp_tg_service_unregister(void);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif
#endif
/** @} */

