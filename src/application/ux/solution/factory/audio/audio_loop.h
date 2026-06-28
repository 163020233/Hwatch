/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: audio_loop.c header file
 * Author : AuthorNameMagicTag
 * Create : 2023-07-11
 */

#ifndef AUDIO_LOOP_H
#define AUDIO_LOOP_H

#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

#define EXT_SUCCESS 0
#define EXT_FAILURE (-1)

int32_t ux_audio_loop_start(int32_t volume, uint32_t mic);
void ux_audio_loop_stop(void);

#ifdef __cplusplus
}
#endif

#endif