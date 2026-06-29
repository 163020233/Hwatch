/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 *
 * Description: voice_cloud_LLM sample private error codes. \n
 *
 * History: \n
 * 2026-06-27, Create file. \n
 */
#ifndef VOICE_CLOUD_LLM_ERRCODE_H
#define VOICE_CLOUD_LLM_ERRCODE_H

#include "errcode.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Sample-private errcodes layered on top of the global errcode_t space. */
/* Picked above the typical ERRCODE_FAIL range so they are easy to spot in logs. */
#define ERRCODE_VOICE_CLOUD_LLM_CONFIG_INVALID    ((errcode_t)0x40010001)
#define ERRCODE_VOICE_CLOUD_LLM_NETWORK_NOT_READY ((errcode_t)0x40010002)
#define ERRCODE_VOICE_CLOUD_LLM_ALREADY_STARTED   ((errcode_t)0x40010003)

#ifdef __cplusplus
}
#endif

#endif
