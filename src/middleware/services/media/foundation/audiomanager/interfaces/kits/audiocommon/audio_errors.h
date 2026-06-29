/*
* Copyright (c) @HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
* Description: audio error define
* Author: Media Software Group
* Create: 2021-04-21
*/

/**
 * @file audio_errors.h
 *
 * @brief Defines the native audio error codes.
 *
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef AUDIO_ERRORS_H
#define AUDIO_ERRORS_H
#include "errcode.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
enum {
    AUDIO_SUCCESS            = ERRCODE_SUCC,
    AUDIO_ERROR              = ERRCODE_FAIL,
    AUDIO_INVALID_PARAMS     = ERRCODE_INVALID_PARAM,
    AUDIO_INVALID_OPERATION  = ERRCODE_MEDIA_AUDIO_INVALID_OPERATION,
    AUDIO_PERMISSION_DENIED  = ERRCODE_MEDIA_PERMISSION_DENIED,
    AUDIO_NO_INIT            = ERRCODE_MEDIA_AUDIO_NO_INIT,
    AUDIO_NO_MEM             = ERRCODE_MEDIA_AUDIO_NO_MEM,
    AUDIO_RETRY_READ         = ERRCODE_MEDIA_AUDIO_RETRY_READ,
    AUDIO_DEAD_OBJECT        = ERRCODE_MEDIA_AUDIO_DEAD_OBJECT,
    AUDIO_BAD_STATE          = ERRCODE_MEDIA_AUDIO_BAD_STATE,
    AUDIO_NOT_FOUND_ACTIVE_STREAM          = ERRCODE_MEDIA_AUDIO_NOT_FOUND_ACTIVE_STREAM,
    AUDIO_NOT_SUPPORT = ERRCODE_NOT_SUPPORT,
};

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif