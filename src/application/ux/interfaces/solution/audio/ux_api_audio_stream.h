/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: Provide ux audio path uapi \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-09-01， Create file. \n
 */

#ifndef UX_API_AUDIO_STREAM_H
#define UX_API_AUDIO_STREAM_H

#include "stdint.h"
#include "errcode.h"
#include "ux_api_audio_common.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum {
    UX_API_AUDIO_STREAM_STATE_IDLE,         /* 空闲 */
    UX_API_AUDIO_STREAM_STATE_PREPARING,    /* 准备中 */
    UX_API_AUDIO_STREAM_STATE_PREPARED,     /* 已准备 */
    UX_API_AUDIO_STREAM_STATE_PLAYING,      /* 播放中 */
    UX_API_AUDIO_STREAM_STATE_STOPPED,      /* 停止 */
    UX_API_AUDIO_STREAM_STATE_ERROR,        /* 错误 */
    UX_API_AUDIO_STREAM_STATE_MAX
}ux_api_audio_stream_state_t;

/**
 * @brief 获取音频流当前播放状态
 * @param [in] stream_type 音频流类型 @ref ux_api_audio_stream_type_t
 * @return 对应音频流类型的播放状态 @ref ux_api_audio_stream_state_t
 */
ux_api_audio_stream_state_t ux_api_audio_stream_get_state(ux_api_audio_stream_type_t stream_type);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
