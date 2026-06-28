/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved. \n
 * Description: Provide ux audio call uapi \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2026-01-30， Create file. \n
 */

#ifndef UX_API_AUDIO_CALL_H
#define UX_API_AUDIO_CALL_H

#include "stdint.h"
#include "errcode.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief 设置通话静音状态
 * @param [in] mute 通话静音状态，true表示静音，false表示取消静音
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_audio_call_set_mute(bool mute);

/**
 * @brief 获取当前通话静音状态
 * @return bool 返回当前通话静音状态，true表示静音，false表示未静音
 */
bool ux_api_audio_call_get_mute(void);

/**
 * @brief 挂断音频通话
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_audio_call_hangup(void);

/**
 * @brief 接受通话请求
 * @param [in] is_preemption 是否启用抢占模式
 *        - true: 启用抢占模式，允许新通话抢占当前通话
 *        - false: 禁用抢占模式，按顺序处理通话
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_audio_call_accept(bool is_preemption);

/**
 * @brief 拒绝音频通话
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_audio_call_reject(void);

/**
 * @brief 通话音量增加
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_audio_call_volume_up(void);

/**
 * @brief 通话音量减少
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_audio_call_volume_down(void);

/**
 * @brief 获取通话音量
 * @return uint16_t 通话音量值
 */
uint16_t ux_api_audio_call_get_volume(void);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
