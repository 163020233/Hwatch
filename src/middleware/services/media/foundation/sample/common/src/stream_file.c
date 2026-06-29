/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: read stram data
 * Author: Media Software Group
 * Create: 2025-06-16
 */

#include "stream_file.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "securec.h"
#include "aac.h"
#include "avc.h"
#include "jpg.h"
#include "media_log.h"
#include "media_mem.h"

#define CHECK_NULL_RETURN(ptr, ret, outStr) \
do { \
    if ((ptr) == NULL) { \
        MEDIA_ERR_LOG("%s ", ((outStr) != NULL) ? (outStr) : " "); \
        return (ret); \
    } \
} while (0)

#define MAX_STREAM_NUM 2

typedef int32_t (*InvokeOpenFunc)(StreamSourceHandle *ssHandle, const char *filePath);
typedef int32_t (*InvokeCloseFunc)(StreamSourceHandle *ssHandle);
typedef int32_t (*InvokeReadFunc)(StreamSourceHandle *ssHandle, uint8_t **frame, size_t frameSize,
    uint32_t *frameLen, uint8_t *keyFrame);

typedef struct {
    const char *cmdName;
    InvokeOpenFunc openInvoke;
} StreamOpenFileInvoke;

typedef struct {
    const char *cmdName;
    InvokeCloseFunc closeInvoke;
} StreamCloseFileInvoke;

typedef struct {
    const char *cmdName;
    InvokeReadFunc readInvoke;
} StreamReadDataInvoke;

static StreamOpenFileInvoke g_openInvokeFuncList[] = {
    {"aac", AacOpen},
    {"h264", AvcOpen},
    {"jpeg", JpgOpen},
};

static StreamCloseFileInvoke g_closeInvokeFuncList[] = {
    {"aac", AacClose},
    {"h264", AvcClose},
    {"jpeg", JpgClose},
};

static StreamReadDataInvoke g_readInvokeFuncList[] = {
    {"aac", AacReadFrame},
    {"h264", AvcReadFrame},
    {"jpeg", JpgReadFrame},
};

int32_t OpenStreamFile(StreamFileHandle *sfHandle, const char *formatType, const char *filePath)
{
    CHECK_NULL_RETURN(sfHandle, MEDIA_INVALID_PARAM, "sfHandle is NULL");
    size_t size = sizeof(g_openInvokeFuncList) / sizeof(StreamOpenFileInvoke);
    InvokeOpenFunc invokeFunc = NULL;
    for (size_t i = 0; i < size; i++) {
        if (strcmp(formatType, g_openInvokeFuncList[i].cmdName) == 0) {
            invokeFunc = g_openInvokeFuncList[i].openInvoke;
            break;
        }
    }
    if (invokeFunc == NULL) {
        MEDIA_ERR_LOG("not found invoke function!, invokeId = %s", formatType);
        return MEDIA_INVALID_PARAM;
    }
    StreamSourceHandle *ssHandle = (StreamSourceHandle *)MediaMemMalloc(sizeof(StreamSourceHandle));
    CHECK_NULL_RETURN(ssHandle, MEDIA_INVALID_PARAM, "ssHandle malloc failed!");
    int32_t ret = invokeFunc(ssHandle, filePath);
    if (ret != 0) {
        MEDIA_ERR_LOG("OpenStreamFile failed!, ret = %d", ret);
        MediaMemFree(ssHandle);
        return ret;
    }
    if (memcpy_s(ssHandle->formatType, STREAM_SOURCE_FORMAT_MAX_LEN,
        formatType, strlen(formatType) + 1) != EOK) {
        MEDIA_ERR_LOG("memcpy_s failed");
        MediaMemFree(ssHandle);
        return MEDIA_ERR;
    }
    *sfHandle = (StreamFileHandle *)ssHandle;
    return MEDIA_OK;
}

int32_t ReadStreamData(StreamFileHandle sfHandle, uint8_t **frame, size_t frameSize,
    uint32_t *frameLen, uint8_t *keyFrame)
{
    StreamSourceHandle *ssHandle = (StreamSourceHandle *)sfHandle;
    CHECK_NULL_RETURN(ssHandle, MEDIA_INVALID_PARAM, "*ssHandle is NULL");
    size_t size = sizeof(g_readInvokeFuncList) / sizeof(StreamReadDataInvoke);
    InvokeReadFunc invokeFunc = NULL;
    for (size_t i = 0; i < size; i++) {
        if (strcmp(ssHandle->formatType, g_readInvokeFuncList[i].cmdName) == 0) {
            invokeFunc = g_readInvokeFuncList[i].readInvoke;
            break;
        }
    }
    if (invokeFunc == NULL) {
        MEDIA_ERR_LOG("not found invoke function!, invokeId = %s", ssHandle->formatType);
        return MEDIA_INVALID_PARAM;
    }
    return invokeFunc(ssHandle, frame, frameSize, frameLen, keyFrame);
}

int32_t CloseStreamFile(StreamFileHandle *sfHandle)
{
    CHECK_NULL_RETURN(sfHandle, MEDIA_INVALID_PARAM, "sfHandle is NULL");
    StreamSourceHandle *ssHandle = (StreamSourceHandle *)*sfHandle;
    CHECK_NULL_RETURN(ssHandle, MEDIA_INVALID_PARAM, "*ssHandle is NULL");
    size_t size = sizeof(g_closeInvokeFuncList) / sizeof(StreamCloseFileInvoke);
    InvokeCloseFunc invokeFunc = NULL;
    for (size_t i = 0; i < size; i++) {
        if (strcmp(ssHandle->formatType, g_closeInvokeFuncList[i].cmdName) == 0) {
            invokeFunc = g_closeInvokeFuncList[i].closeInvoke;
            break;
        }
    }
    if (invokeFunc == NULL) {
        MEDIA_ERR_LOG("not found invoke function!, invokeId = %s", ssHandle->formatType);
        return MEDIA_INVALID_PARAM;
    }
    (void)invokeFunc(ssHandle);
    if (ssHandle != NULL) {
        MediaMemFree(ssHandle);
        ssHandle = NULL;
    }
    return MEDIA_OK;
}

