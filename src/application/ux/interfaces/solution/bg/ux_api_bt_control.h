/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: Provide ux bt media stream api \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-10-10， Create file. \n
 */

#ifndef UX_API_BT_CONTROL_H
#define UX_API_BT_CONTROL_H
#include "errcode.h"
#include "bs_br_service_api.h"
#include "ux_api_bt.h"

#if defined(__cplusplus) && __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief 音量控制类型枚举
 */
typedef enum {
    UX_API_BT_CONTROL_VOLUME_UP = 0,       /**< 音量增加一档 */
    UX_API_BT_CONTROL_VOLUME_DOWN,         /**< 音量降低一档 */
    UX_API_BT_CONTROL_VOLUME_MUTE,         /**< 静音 */
    UX_API_BT_CONTROL_VOLUME_SET_ABSOLUTE, /**< 设置绝对音量 */
    UX_API_BT_CONTROL_VOLUME_MAX,          /**< 无效枚举 */
} ux_api_bt_control_volume_t;

/**
 * @brief 播放控制类型枚举
 */
typedef enum {
    UX_API_BT_CONTROL_PLAY_START = 0,    /**< 继续播放 */
    UX_API_BT_CONTROL_PLAY_STOP,         /**< 停止播放 */
    UX_API_BT_CONTROL_PLAY_PAUSE,        /**< 暂停播放 */
    UX_API_BT_CONTROL_PLAY_NEXT,         /**< 播放下一首 */
    UX_API_BT_CONTROL_PLAY_PRE,          /**< 播放上一首 */
    UX_API_BT_CONTROL_PLAY_REWIND,       /**< 从头开始播放 */
    UX_API_BT_CONTROL_PLAY_FAST_FORWARD, /**< 快进 */
    UX_API_BT_CONTROL_PLAY_MAX,          /**< 无效枚举 */
} ux_api_bt_control_play_t;

/**
 * @brief 蓝牙音量类型枚举
 */
typedef enum {
    UX_API_BT_VOLUME_TYPE_MIC = 0,     /**< 调整麦克风增益 */
    UX_API_BT_VOLUME_TYPE_SPEAKER,     /**< 调整扬声器增益 */
    UX_API_BT_VOLUME_TYPE_MAX,         /**< 最大值 */
} ux_api_bt_volume_type_t;

/**
 * @brief 音量控制信息结构体
 */
typedef struct {
    ux_api_bt_volume_type_t volume_type; /**< 音频设备类型 */
    ux_api_bt_control_volume_t control;  /**< 音量控制请求 */
    uint8_t volume;                      /**< 音量，当control为SET_ABSOLUTE时生效，avrcp范围[0,0x7F]，hfp范围[0,15] */
} ux_api_bt_control_volume_info_t;

/**
 * @brief 通话控制类型枚举
 */
typedef enum {
    UX_API_BT_CONTROL_CALL_DAILING = 0, /*!< @if Eng call out request, param is phone number,
                                       param_len is the length of phone number
                                         @else 呼出请求，入参param为手机号,param_len为手机号长度 @endif */
    UX_API_BT_CONTROL_CALL_ANSWER,  /*!< @if Eng call accept request, param is NULL, param_len is 0
                                         @else 接听请求，入参param为空，param_len为0 @endif */
    UX_API_BT_CONTROL_CALL_HANGUP,  /*!< @if Eng call finish request,param is NULL, param_len is 0.
                                         @else 挂断请求，入参param为空，param_len为0 @endif */
    UX_API_BT_CONTROL_CALL_REJECT,  /*!< @if Eng call reject request, param is NULL, param_len is 0
                                         @else 拒接请求，入参param为空,param_len为0 @endif */
    UX_API_BT_CONTROL_CALL_HOLD,    /*!< @if Eng call hold active call request, param is NULL, param_len is 0
                                         @else 保持当前通话请求，入参param为空，param_len为0 @endif */
    UX_API_BT_CONTROL_CALL_RECORD,  /*!< @if Eng call info request, param is NULL, param_len is 0
                                         @else 通话信息请求，入参param为空,param_len为0 @endif */
    UX_API_BT_CONTROL_CALL_MAX,      /**< 最大值 */
} ux_api_bt_control_call_t;

/**
 * @brief 通话控制信息结构体
 */
typedef struct {
    uint8_t call_id;                    /**< 通话id，为三方通话预留，默认填0 */
    ux_api_bt_control_call_t request;   /**< 通话控制请求 */
    uint8_t *param;                     /**< 通话控制参数，参考 @ref ux_api_bt_control_call_t */
    uint16_t param_len;                 /**< 通话控制参数长度 */
} ux_api_bt_control_call_info_t;

/**
 * @brief 控制链路信息结构体
 */
typedef struct {
    ux_api_bt_profile_state_t profile_state; /**< 链路状态 */
    uint8_t mic_volume;                      /**< 麦克风音量 */
    uint8_t speaker_volume;                  /**< 扬声器音量 */
    bool is_mic_mute;                        /**< 麦克风是否静音 */
    bool is_speaker_mute;                    /**< 扬声器是否静音 */
    uint32_t res[8];                          /**< 预留字段 */
} ux_api_bt_control_info_t;

/**
 * @brief  通过蓝牙控制服务链路控制对端媒体音量
 * @param [in] profile 蓝牙控制服务类型, 参考 @ref ux_api_bt_profile_t
 * @param [in] addr 设备地址，小端序
 * @param [in] volume_control_info 音量控制信息，参考 @ref ux_api_bt_control_volume_info_t
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_bt_control_volume(ux_api_bt_profile_t profile,  const ux_api_bt_addr_t *addr,
    const ux_api_bt_control_volume_info_t *volume_control_info);

/**
 * @brief  通过蓝牙控制服务链路控制对端媒体播放
 * @param [in] profile 蓝牙控制服务类型, 参考 @ref ux_api_bt_profile_t
 * @param [in] addr 设备地址，小端序
 * @param [in] play_control 媒体播放控制类型, 参考 @ref ux_api_bt_control_play_t
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_bt_control_play(ux_api_bt_profile_t profile,  const ux_api_bt_addr_t *addr,
    ux_api_bt_control_play_t play_control);

/**
 * @brief  通过蓝牙控制服务链路控制对端(带cat1卡)通话
 * @param [in] profile 蓝牙控制服务类型, 参考 @ref ux_api_bt_profile_t
 * @param [in] addr 设备地址，小端序
 * @param [in] call_control 通话控制信息, 参考 @ref ux_api_bt_control_call_info_t
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_bt_control_call(ux_api_bt_profile_t profile, const ux_api_bt_addr_t *addr,
    const ux_api_bt_control_call_info_t *call_control);

/**
 * @brief 按地址获取控制链路信息
 * @param [in] profile 服务类型
 * @param [in] addr 设备地址
 * @return ux_api_bt_control_info_t* 控制链路信息结构体指针
 */
const ux_api_bt_control_info_t* ux_api_bt_control_get_info(ux_api_bt_profile_t profile, const ux_api_bt_addr_t *addr);
#if defined(__cplusplus) && __cplusplus
}
#endif /* __cplusplus */

#endif