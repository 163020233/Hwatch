/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: API of audio debug
 * Author: audio
 */

#ifndef __AUDIO_DEBUG_H__
#define __AUDIO_DEBUG_H__

#include "chip_type.h"

#if (SAP_OS_TYPE == os_type_xtensa)
#include "audio_debug_hifi.h"
#else
#include "audio_debug_acore.h"
#endif /* SAP_OS_TYPE */

#ifdef SAP_DEBUG_MODULE_HANDLE
#define sap_debug_module_handle(fmt, handle) audio_log1(AUDIO_LOG_LEVEL_ALERT, fmt, handle)
#else
#define sap_debug_module_handle(fmt, handle)
#endif

#endif /* __AUDIO_DEBUG_H__ */
