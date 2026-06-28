/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: API of audio debug
 * Author: audio
 */

#ifndef __AUDIO_DEBUG_HIFI_H__
#define __AUDIO_DEBUG_HIFI_H__

#include "log_dsp.h"

#ifdef SAP_IPC_LOG_SUPPORT
#include "audio_debug_ipc.h"
#else
#include "audio_debug_uart.h"
#endif /* SAP_IPC_LOG_SUPPORT */

#endif /* __AUDIO_DEBUG_HIFI_H__ */
