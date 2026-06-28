/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved. \n
 * Description: add media player function \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2026-06-24， Create file. \n
 */

#ifndef UX_API_PLAYER_H
#define UX_API_PLAYER_H

#include "stdint.h"
#include "errcode.h"
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define UX_API_PLAYER_MIX_VOLUME_MAX 100                                  /**< 音乐或通话的混音音量占比100 */
#define UX_API_PLAYER_MIX_VOLUME_DEFAULT 50                               /**< 音乐或通话的混音音量占50 */

/**
 * @brief 媒体播放状态枚举
 */
typedef enum {
    UX_API_PLAYER_STATE_PAUSE,      /**< 暂停状态 */
    UX_API_PLAYER_STATE_PLAYING,    /**< 播放中状态 */
    UX_API_PLAYER_STATE_FINISHED,        /**< 播放结束状态 */
    UX_API_PLAYER_STATE_MAX         /**< 状态最大值 */
} ux_api_player_state_t ;

/**
 * @defgroup 支持产品  [soundbox]
 * @brief 获取当前播放状态
 * @return ux_api_player_state_t    枚举值：初始化，播放中，暂停，播放结束
 */
ux_api_player_state_t ux_api_player_get_state(void);

/**
 * @defgroup 支持产品  [soundbox]
 * @brief 设置音频的存放路径
 * @param[in] file_name 音频文件的路径
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_player_set_source(char* file_name);

/**
 * @defgroup 支持产品  [soundbox]
 * @brief 播放设置好的音频
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_player_start(void);

/**
 * @defgroup 支持产品  [soundbox]
 * @brief 暂停当前正在播放的音频（不销毁player）
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_player_pause(void);

/**
 * @defgroup 支持产品  [soundbox]
 * @brief  恢复被暂停的音频播放
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_player_resume(void);

/**
 * @defgroup 支持产品  [soundbox]
 * @brief 销毁当前音频播放器
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_player_stop(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
