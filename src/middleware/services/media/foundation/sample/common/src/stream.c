/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: read stram data
 * Author: Media Software Group
 * Create: 2025-06-16
 */

#include "stream.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "securec.h"
#include "media_log.h"
#include "media_mem.h"

static int32_t CheckParams(StreamSourceHandle *ssHandle, const char *filePath)
{
    if (ssHandle == NULL || filePath == NULL) {
        MEDIA_ERR_LOG("ssHandle or pFilePath is null\n");
        return MEDIA_ERR;
    }
    if (memset_s(ssHandle, sizeof(StreamSourceHandle), 0x00, sizeof(StreamSourceHandle)) != EOK) {
        MEDIA_ERR_LOG("memset_s error!");
        return MEDIA_ERR;
    }
    if (strncpy_s(ssHandle->fileName, MAX_FILE_NAME_LEN, filePath, MAX_FILE_NAME_LEN) != EOK) {
        MEDIA_ERR_LOG("strncpy_s error!");
        return MEDIA_ERR;
    }
    char resolvedpath[PATH_MAX];
    if (realpath(filePath, resolvedpath) == NULL) {
        MEDIA_ERR_LOG(" real path fail");
        return MEDIA_ERR;
    }
    ssHandle->file = fopen(resolvedpath, "rb");
    if (ssHandle->file == NULL) {
        MEDIA_ERR_LOG("fopen stream fail");
        return MEDIA_ERR;
    }
    return MEDIA_OK;
}

int32_t StreamOpen(StreamSourceHandle *ssHandle, const char *filePath, uint32_t buffLen)
{
    if (CheckParams(ssHandle, filePath) != 0) {
        return MEDIA_ERR;
    }
    if (-1 == fseek(ssHandle->file, 0, SEEK_END)) {
        MEDIA_ERR_LOG("fseek stream fail");
        goto EXIT;
    }
    int64_t ret = ftell(ssHandle->file);
    if (ret <= 0 || ret > UINT_MAX) { /* By default, the file length is less than UINT_MAX */
        MEDIA_ERR_LOG("ftell stream fail");
        goto EXIT;
    }
    ssHandle->fileLen = (uint32_t)ret;
    if (-1 == fseek(ssHandle->file, 0, SEEK_SET)) {
        MEDIA_ERR_LOG("fseek stream fail");
        goto EXIT;
    }
    if (buffLen <= 0) {
        MEDIA_ERR_LOG("buffLen <= 0!");
        goto EXIT;
    }
    ssHandle->bufHeader = (uint8_t *)MediaMemMalloc(buffLen);
    if (ssHandle->bufHeader == NULL) {
        MEDIA_ERR_LOG("malloc failed!");
        goto EXIT;
    }
    ssHandle->bufLen = buffLen;
    if (memset_s(ssHandle->bufHeader, buffLen, 0x00, buffLen) != EOK) {
        MEDIA_ERR_LOG("memset_s error!");
        MediaMemFree(ssHandle->bufHeader);
        ssHandle->bufHeader = NULL;
        goto EXIT;
    }
    ssHandle->readOffset = 0;
    ssHandle->fileReadOffset = 0;
    return MEDIA_OK;
EXIT:
    ret = fclose(ssHandle->file);
    if (ret != 0) {
        MEDIA_ERR_LOG("fclose failed!");
    }
    ssHandle->file = NULL;
    return MEDIA_ERR;
}

int32_t StreamClose(StreamSourceHandle *ssHandle)
{
    if (ssHandle == NULL) {
        MEDIA_ERR_LOG("ssHandle is null");
        return MEDIA_ERR;
    }
    if (ssHandle->bufHeader != NULL) {
        MediaMemFree(ssHandle->bufHeader);
        ssHandle->bufHeader = NULL;
    }
    if (ssHandle->file != NULL) {
        fclose(ssHandle->file);
        ssHandle->file = NULL;
    }
    ssHandle->bufLen = 0;
    ssHandle->readOffset = 0;
    ssHandle->availReadLen = 0;
    ssHandle->fileReadOffset = 0;
    return 0;
}

int32_t StreamReset(StreamSourceHandle *ssHandle)
{
    if (ssHandle == NULL) {
        MEDIA_ERR_LOG("ssHandle is null");
        return MEDIA_ERR;
    }
    ssHandle->fileReadOffset = 0;
    return 0;
}