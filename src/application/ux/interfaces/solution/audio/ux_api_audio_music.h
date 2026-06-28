/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved. \n
 * Description: Provide ux audio music uapi \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2026-01-30， Create file. \n
 */

#ifndef UX_API_AUDIO_MUSIC_H
#define UX_API_AUDIO_MUSIC_H

#include "stdint.h"
#include "errcode.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define UX_API_AUDIO_ASSISTANT_LEN 8               /**< 音频助手参数长度 */

/**
 * @brief 音频助手参数结构体
 */
typedef struct {
    uint8_t reserved[UX_API_AUDIO_ASSISTANT_LEN]; /**< 保留字段 */
} ux_api_audio_assistant_param_t;

/**
 * @defgroup 支持产品 [earphone]
 * @brief 切换当前音乐播放状态: 播放中 -> 暂停; 暂停中 -> 播放
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_audio_toggle_music_play(void);

/**
 * @defgroup 支持产品 [earphone]
 * @brief 设置音乐静音状态
 * @param [in] mute 音乐静音状态，true表示静音，false表示取消静音
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_audio_music_set_mute(bool mute);

/**
 * @defgroup 支持产品 [earphone]
 * @brief 获取当前音乐静音状态
 * @return bool 静音状态，true表示静音，false表示未静音
 */
bool ux_api_audio_music_get_mute(void);

/**
 * @defgroup 支持产品 [earphone]
 * @brief 暂停音频音乐播放
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_audio_music_pause(void);

/**
 * @defgroup 支持产品 [earphone]
 * @brief 恢复音频音乐播放
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_audio_music_resume(void);

/**
 * @defgroup 支持产品 [earphone]
 * @brief 启动音乐播放
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_audio_music_play(void);

/**
 * @defgroup 支持产品 [earphone]
 * @brief 上一首
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_audio_music_previous(void);

/**
 * @defgroup 支持产品 [earphone]
 * @brief 下一首
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_audio_music_next(void);

/**
 * @defgroup 支持产品 [earphone]
 * @brief 音量加
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_audio_music_volume_up(void);

/**
 * @defgroup 支持产品 [earphone]
 * @brief 音量减
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_audio_music_volume_down(void);

/**
 * @defgroup 支持产品 [earphone]
 * @brief 获取音乐音量
 * @return uint16_t 音乐音量值
 */
uint16_t ux_api_audio_music_get_volume(void);

/**
 * @defgroup 支持产品 [earphone]
 * @brief 唤醒语言助手
 * @param [in] param 辅助功能参数结构体，包含唤醒助手所需的各种配置信息
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_audio_music_wake_up_assistant(ux_api_audio_assistant_param_t *param);

/**
 * @defgroup 支持产品 [earphone]
 * @brief 使能是否开启蓝牙音乐歌词信息上报
 * @param [in] enable 是否使能
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_audio_music_set_attribute_enable(bool enable);

/**
 * @defgroup 支持产品 [earphone]
 * @brief 获取蓝牙音乐歌词信息上报使能状态
 * @return bool 是否使能，true表示使能，false表示未使能
 */
bool ux_api_audio_music_get_attribute_enable(void);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
