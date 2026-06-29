/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: read stram data
 * Author: Media Software Group
 * Create: 2025-06-16
 */

#ifndef STREAM_FILE_H
#define STREAM_FILE_H
#include <stdio.h>
#include <stdint.h>
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

typedef void *StreamFileHandle;

int32_t OpenStreamFile(StreamFileHandle *sfHandle, const char *formatType, const char *filePath);

int32_t ReadStreamData(StreamFileHandle sfHandle, uint8_t **frame, size_t frameSize,
    uint32_t *frameLen, uint8_t *keyFrame);

int32_t CloseStreamFile(StreamFileHandle *sfHandle);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
#endif