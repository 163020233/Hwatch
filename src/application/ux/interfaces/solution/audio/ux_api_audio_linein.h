/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 * Description: Provide ux audio priority manager uapi \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-09-04， Create file. \n
 */

#ifndef UX_API_AUDIO_LINEIN_H
#define UX_API_AUDIO_LINEIN_H

#include "stdint.h"
#include "errcode.h"
#include "ux_api_audio_common.h"


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief 开始linein类型音频流
 */
errcode_t ux_api_audio_linein_stream_start(void);

/**
 * @brief 停止linein类型音频流
 */
errcode_t ux_api_audio_linein_stream_stop(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
