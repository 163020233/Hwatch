/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: read avc data
 * Author: Media Software Group
 * Create: 2025-06-16
 */

#ifndef AVC_H
#define AVC_H

#include <stdint.h>
#include "stream.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

int32_t AvcOpen(StreamSourceHandle *ssHandle, const char *filePath);
int32_t AvcReadFrame(StreamSourceHandle *ssHandle, uint8_t **frame, size_t frameSize,
    uint32_t *frameLen, uint8_t *keyFrame);
int32_t AvcClose(StreamSourceHandle *ssHandle);
int32_t AvcReset(StreamSourceHandle *ssHandle);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
#endif
