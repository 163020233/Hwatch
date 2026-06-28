/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: read jpg data
 * Author: Media Software Group
 * Create: 2025-06-16
 */

#ifndef JPG_H
#define JPG_H
#include "stream.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

int32_t JpgOpen(StreamSourceHandle *ssHandle, const char *filePath);

int32_t JpgReadFrame(StreamSourceHandle *ssHandle, uint8_t **frame, size_t frameSize,
    uint32_t *frameLen, uint8_t *keyFrame);

int32_t JpgClose(StreamSourceHandle *ssHandle);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
#endif
