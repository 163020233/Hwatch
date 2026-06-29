/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 *
 * Description: Provide ux event define \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-10-13， Create file. \n
 */
#ifndef UX_EVENT_DEF_H
#define UX_EVENT_DEF_H

#if defined(__cplusplus) && __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum {
    UX_BG_START_PAIR = 0,
    UX_BG_DISCONNECTED,
    UX_BG_START_DISCOVERY,
    UX_BG_STOP_DISCOVERY,
    UX_BG_ENABLE,
    UX_BG_REMOVE_DEVICE,
    UX_BG_DISABLE,
    UX_BG_BT_DISABLE_CB,
    /* 3322新增事件 */
    UX_BG_SCO_STATE_CHANGE,
    UX_BG_SCO_CODEC_CHANGE,
    UX_BG_CALL_CAT1_CHANGE,
    UX_BG_CALL_VOLUME_CHANGE,
    UX_BG_HF_ANSWER,
    UX_BG_HF_HANGUP,
    UX_BG_HF_REJECT,
    UX_BG_HF_DIALING,
    UX_BG_HF_VOLUME_UP,
    UX_BG_HF_VOLUME_DOWN,
    UX_BG_HF_VOLUME_MUTE,

    UX_BG_A2DP_SINK_PROFILE_STATE_CHANGE,   // 蓝牙媒体流传输链路的状态变化事件
    UX_BG_A2DP_SINK_STREAM_STATE_CHANGE,    // 蓝牙媒体流工作模式的状态变化事件
    UX_BG_A2DP_SINK_CODEC_CHANGE,           // 蓝牙媒体流编码格式的状态变化事件

    UX_BG_A2DP_SRC_PROFILE_STATE_CHANGE,   // 蓝牙媒体流传输链路的状态变化事件
    UX_BG_A2DP_SRC_STREAM_STATE_CHANGE,    // 蓝牙媒体流工作模式的状态变化事件
    UX_BG_A2DP_SRC_CODEC_CHANGE,           // 蓝牙媒体流编码格式的状态变化事件
    UX_BG_A2DP_SRC_START_STREAM,           // SRC端建立a2dp音频流
    UX_BG_A2DP_SRC_STOP_STREAM,            // SRC端停止a2dp音频流

    UX_BG_AVRCP_PROFILE_STATE_CHANGE,    // 蓝牙媒体控制链路的状态变化事件
    UX_BG_MEDIA_CONTROL_REQUEST,         // 作为tg端，收到对端的媒体控制请求
    UX_BG_MEDIA_PLAY_STATE_CHANGE,       // 作为ct端，收到对端的媒体播放状态变化事件
    UX_BG_MEDIA_ATTRIBUTE_REPORT,        // 作为ct端，收到对端的媒体信息上报
    UX_BG_MEDIA_VOLUME_SYNC_REPORT,      // 作为ct端，向对端请求同步音量，对端回复
    UX_BG_MEDIA_VOLUME_CONTROL_REQUEST,  // 作为tg端，收到对端的音量调节请求

    UX_BG_MEDIA_PLAY,                // 命令tg端开启蓝牙媒体
    UX_BG_MEDIA_PAUSE,               // 命令tg端暂停蓝牙媒体，进度条不变
    UX_BG_MEDIA_PLAY_NEXT,           // 命令tg端播放下一首
    UX_BG_MEDIA_PLAY_PRE,            // 命令tg端播放上一首
    UX_BG_MEDIA_FAST_FORWARD,        // 命令tg端快进
    UX_BG_MEDIA_VOLUMEUP,            // 命令tg端增加音量
    UX_BG_MEDIA_VOLUMEDOWN,          // 命令tg端减少音量
    UX_BG_MEDIA_MUTE,                // 命令tg端静音

    UX_BG_PBAP_PROFILE_STATE_CHANGE, // 电话簿服务连接回调
    UX_BG_PBAP_RECEVICE_FINISH,      // 电话簿文件传输完成
    UX_BG_BT_CORE_CRASH,             // b核挂死
    /* 3322新增事件 */
} ux_event_bg_bt_t;

#if defined(__cplusplus) && __cplusplus
}
#endif /* __cplusplus */
#endif