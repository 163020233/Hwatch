/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: read stream data
 * Author: Media Software Group
 * Create: 2025-06-16
 */

#ifndef STREAM_H
#define STREAM_H
#include <stdio.h>
#include <stdint.h>
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */
#define MAX_FILE_NAME_LEN 256
#define STREAM_SOURCE_FORMAT_MAX_LEN 10

typedef struct {
    char fileName[MAX_FILE_NAME_LEN];
    char formatType[STREAM_SOURCE_FORMAT_MAX_LEN];
    FILE *file;
    uint32_t fileLen;
    uint32_t fileReadOffset;
    uint8_t *bufHeader;
    uint32_t bufLen;
    uint32_t readOffset;
    uint32_t availReadLen;
} StreamSourceHandle;

int32_t StreamOpen(StreamSourceHandle *ssHandle, const char *filePath, uint32_t buffLen);

int32_t StreamClose(StreamSourceHandle *ssHandle);

int32_t StreamReset(StreamSourceHandle *ssHandle);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
#endif
