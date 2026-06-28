/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 * Description:Audio Manager Interface. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-03-08， Create file. \n
 */
#ifndef UX_AUDIO_MANAGER_H
#define UX_AUDIO_MANAGER_H

#include <stdbool.h>
#include <stddef.h>
#include "audio.h"
#include "audio_types.h"
#include "audio_base_type.h"
#include "errcode.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief 初始化音频管理器。
 * @retval ERRCODE_SUCC 成功
 * @retval Other 失败，参考 @ref errcode_t
 */
errcode_t ux_audio_manager_init(void);

/**
 * @brief 去初始化音频管理器，释放资源。
 * @retval ERRCODE_SUCC 成功
 * @retval Other 失败，参考 @ref errcode_t
 */
errcode_t ux_audio_manager_deinit(void);

/**
 * @brief 检查音频管理器是否已初始化。
 * @retval true 已初始化
 * @retval false 未初始化
 */
bool uapi_audio_manager_is_init(void);

/**
 * Set audio mic role.
 * @param[in]   mic_role - audio mic role
 * @return      Audio error status code (0 - Success)
 */
int32_t ux_audio_manager_set_mono_mode(bool mono_mode);

/**
 * Get mono mode
 * @return      Audio mono mode
 */
bool ux_audio_manager_get_mono_mode(void);

/**
 * Set tws mode. Set tws mode to non-tws/forward.
 * @param[in]   tws_mode - tws mode
 * @return      Audio error status code (0 - Success)
 */
int32_t ux_audio_manager_set_tws_mode(audio_tws_mode tws_mode);

/**
 * @brief 读取下行音量档位数量和音量值
 * @param [in, out] volume_levels 音量档位信息
 * @param [out] volume_count 档位数量, 取值范围[8, 31]
 * @retval ERRCODE_SUCC 成功
 * @retval Other 失败, 参考 @ref errcode_t
 */
errcode_t ux_audio_manager_get_volume_levels(int32_t *volume_levels, uint16_t *volume_count);

/**
 * @brief 读取最大音量档位
 * @retval uint8_t 最大音量档位
 */
uint8_t ux_audio_manager_get_max_volume_level(void);

/**
 * @brief 设置音乐播放通道的静音状态。
 * @param[in] is_mute 是否开启静音：
 *                        - `true`：静音音乐音频流；
 *                        - `false`：取消静音，恢复音量。
 * @retval ERRCODE_SUCC 成功
 * @retval Other 失败, 参考 @ref errcode_t
 */
errcode_t ux_audio_set_music_mute_state(bool is_mute);


/**
 * @brief 设置通话通道的静音状态。
 * @param[in] is_mute 是否开启静音：
 *                        - `true`：静音通话音频流（上下行均静音）；
 *                        - `false`：取消静音，恢复通话音量。
 * @retval ERRCODE_SUCC 成功
 * @retval Other 失败, 参考 @ref errcode_t
 */
errcode_t ux_audio_set_call_mute_state(bool is_mute);

errcode_t ux_audio_set_stream_db_vol(AudioStreamType stream_type, int32_t db_vol, int32_t flag);
errcode_t ux_audio_set_downlink_stream_toggle_cfg(AudioStreamType stream_type, uint32_t toggle_cnt, bool valid_flag);
#ifdef __cplusplus
}
#endif

#endif
