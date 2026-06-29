/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2022. All rights reserved. \n
 * Description: common UNF header file for audio \n
 * Author: @CompanyNameTag \n
 */

 /** @defgroup audio sys */

#ifndef __SOC_UAPI_AUDIO_SYS_H__
#define __SOC_UAPI_AUDIO_SYS_H__

#include "soc_uapi_audio.h"
#include "soc_uapi_sound.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/**
 * @defgroup soc_uapi_audio_sys soc_uapi_audio_sys
 * @ingroup  audio
 * @{
 */

/**
 * @if Eng
 * @brief  Init audio system.
 * @retval SUCCESS Success.
 * @retval FAILURE Failure.
 * @else
 * @brief  初始化音频系统
 * @retval SUCCESS 成功
 * @retval FAILURE 失败
 * @endif
 */
int32_t uapi_audio_init(void);

/**
 * @if Eng
 * @brief  Deinit audio system.
 * @retval SUCCESS Success.
 * @retval FAILURE Failure.
 * @else
 * @brief  去初始化音频系统
 * @retval SUCCESS 成功
 * @retval FAILURE 失败
 * @endif
 */
int32_t uapi_audio_deinit(void);

/**
 * @if Eng
 * @brief  Set audio product form.
 * @retval SUCCESS Success.
 * @retval FAILURE Failure.
 * @else
 * @brief  设置音频产品形态
 * @retval SUCCESS 成功
 * @retval FAILURE 失败
 * @endif
 */
int32_t uapi_audio_set_product_form(uapi_audio_product_form product_form);

/**
 * @if Eng
 * @brief  Set configuration related to debugging.
 * @retval SUCCESS Success.
 * @retval FAILURE Failure.
 * @else
 * @brief  设置调试相关的配置
 * @retval SUCCESS 成功
 * @retval FAILURE 失败
 * @endif
 */
int32_t uapi_audio_set_debug_cfg(const uapi_audio_debug_cfg *cfg);

/**
 * @if Eng
 * @brief  Get configuration related to debugging.
 * @retval SUCCESS Success.
 * @retval FAILURE Failure.
 * @else
 * @brief  获取调试相关的配置
 * @retval SUCCESS 成功
 * @retval FAILURE 失败
 * @endif
 */
int32_t uapi_audio_get_debug_cfg(uapi_audio_debug_cfg *cfg);

/**
 * @if Eng
 * @brief  Set audio tws(True Wireless Stereo) mono mode.
 * @retval SUCCESS Success.
 * @retval FAILURE Failure.
 * @else
 * @brief  设置真无线耳机单耳模式
 * @retval SUCCESS 成功
 * @retval FAILURE 失败
 * @endif
 */
int32_t uapi_audio_set_tws_mono_mode(uint8_t mono);

/**
 * @if Eng
 * @brief  Set audio tws(True Wireless Stereo) mode.
 * @retval SUCCESS Success.
 * @retval FAILURE Failure.
 * @else
 * @brief  设置真无线耳机工作模式
 * @retval SUCCESS 成功
 * @retval FAILURE 失败
 * @endif
 */
int32_t uapi_audio_set_tws_mode(uapi_audio_tws_mode tws_mode);

/**
 * @if Eng
 * @brief  Set audio tws(True Wireless Stereo) role.
 * @retval SUCCESS Success.
 * @retval FAILURE Failure.
 * @else
 * @brief  设置真无线耳机角色
 * @retval SUCCESS 成功
 * @retval FAILURE 失败
 * @endif
 */
int32_t uapi_audio_set_tws_role(uapi_audio_tws_role tws_role);

/**
 * @if Eng
 * @brief  Set DSP log level.
 * @retval SUCCESS Success.
 * @retval FAILURE Failure.
 * @else
 * @brief  设置DSP打印等级
 * @retval SUCCESS 成功
 * @retval FAILURE 失败
 * @endif
 */
int32_t uapi_audio_set_dsp_log_level(uint32_t log_level);

/**
 * @if Eng
 * @brief  Get Audio Scene.
 * @retval SUCCESS Success.
 * @retval FAILURE Failure.
 * @else
 * @brief  获取音频场景
 * @retval SUCCESS 成功
 * @retval FAILURE 失败
 * @endif
 */
int32_t uapi_audio_get_audio_scene(uint32_t handle, uapi_audio_scene_config *audio_scene_cfg);

/**
 * @if Eng
 * @brief  Set Audio Scene.
 * @retval SUCCESS Success.
 * @retval FAILURE Failure.
 * @else
 * @brief  设置音频场景
 * @retval SUCCESS 成功
 * @retval FAILURE 失败
 * @endif
 */
int32_t uapi_audio_set_audio_scene(uint32_t handle, const uapi_audio_scene_config *audio_scene_cfg);

/**
 * @if Eng
 * @brief  Get Audio volume.
 * @param  [in]  handle   handle
 * @param  [in]  id       identification
 * @param  [out] vol_cfg  volume configuration
 * @retval SUCCESS Success.
 * @retval FAILURE Failure.
 * @else
 * @brief  获取音频音量
 * @param  [in]  handle   模块句柄
 * @param  [in]  id       模块id
 * @param  [out] vol_cfg  音量配置
 * @retval SUCCESS 成功
 * @retval FAILURE 失败
 * @endif
 */
int32_t uapi_audio_get_volume(uint32_t handle, uapi_audio_volume_cfg *vol_cfg);

/**
 * @if Eng
 * @brief  Set Audio volume.
 * @param  [in]  handle   handle
 * @param  [in]  id       identification
 * @param  [in]  vol_cfg  volume configuration
 * @retval SUCCESS Success.
 * @retval FAILURE Failure.
 * @else
 * @brief  设置音频音量
 * @param  [in]  handle   模块句柄
 * @param  [in]  id       模块id
 * @param  [in]  vol_cfg  音量配置
 * @retval SUCCESS 成功
 * @retval FAILURE 失败
 * @endif
 */
int32_t uapi_audio_set_volume(uint32_t handle, const uapi_audio_volume_cfg *vol_cfg);

/**
 * @if Eng
 * @brief  Set DSP binary file path.
 * @retval SUCCESS Success.
 * @retval FAILURE Failure.
 * @else
 * @brief  设置DSP二进制文件路径
 * @retval SUCCESS 成功
 * @retval FAILURE 失败
 * @endif
 */
td_s32 uapi_audio_set_dsp_file_path(const td_char *file_path, td_u32 len);

/**
 * @if Eng
 * @brief  Set audio wireless microphone work mode.
 * @retval SUCCESS Success.
 * @retval FAILURE Failure.
 * @else
 * @brief  设置无线麦克风工作模式
 * @retval SUCCESS 成功
 * @retval FAILURE 失败
 * @endif
 */
int32_t uapi_audio_set_mic_mode(uapi_audio_mic_mode mic_mode);

/**
 * @if Eng
 * @brief  Set audio wireless microphone mix mode.
 * @retval SUCCESS Success.
 * @retval FAILURE Failure.
 * @else
 * @brief  设置无线麦克风混音模式
 * @retval SUCCESS 成功
 * @retval FAILURE 失败
 * @endif
 */
int32_t uapi_audio_set_mic_mix_mode(uapi_audio_mic_mix_mode mic_mix_mode);

/**
 * @if Eng
 * @brief  Get Audio global mute.
 * @param  [out] mute  mute status 0/1
 * @retval SUCCESS Success.
 * @retval FAILURE Failure.
 * @else
 * @brief  获取音频全局静音标志
 * @param  [out] mute  静音标志，0或者1
 * @retval SUCCESS 成功
 * @retval FAILURE 失败
 * @endif
 */
int32_t uapi_audio_get_global_mute(uint8_t *mute);

/**
 * @if Eng
 * @brief  Set Audio global mute, default 0.
 * @param  [in]  mute  mute status 0/1
 * @retval SUCCESS Success.
 * @retval FAILURE Failure.
 * @else
 * @brief  设置音频全局静音标志，默认不静音
 * @param  [in]  mute  静音标志，0或者1
 * @retval SUCCESS 成功
 * @retval FAILURE 失败
 * @endif
 */
int32_t uapi_audio_set_global_mute(uint8_t mute);

/**
 * @if Eng
 * @brief  Set Audio volume matrix of specific out port.
 * @param  [in]  port out port
 * @param  [in]  volume_matrix volume matrix
 * @retval SUCCESS Success.
 * @retval FAILURE Failure.
 * @else
 * @brief  设置特定音频输出端口的音量矩阵
 * @param  [in]  port 音频输出端口
 * @param  [in]  volume_matrix 音量矩阵
 * @retval SUCCESS 成功
 * @retval FAILURE 失败
 * @endif
 */
int32_t uapi_audio_set_volume_matrix(uapi_snd_out_port port, const uapi_audio_volume_matrix *volume_matrix);

/**
 * @if Eng
 * @brief  Get Audio volume matrix of specific out port.
 * @param  [in]  port out port
 * @param  [out]  volume_matrix volume matrix
 * @retval SUCCESS Success.
 * @retval FAILURE Failure.
 * @else
 * @brief  获取特定音频输出端口的音量矩阵
 * @param  [in]  port 音频输出端口
 * @param  [out] volume_matrix 音量矩阵
 * @retval SUCCESS 成功
 * @retval FAILURE 失败
 * @endif
 */
int32_t uapi_audio_get_volume_matrix(uapi_snd_out_port port, uapi_audio_volume_matrix *volume_matrix);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __SOC_UAPI_AUDIO_SYS_H__ */
