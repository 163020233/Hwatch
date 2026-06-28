/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: read jpg data
 * Author: Media Software Group
 * Create: 2025-06-16
 */

#include "jpg.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "securec.h"
#include "media_log.h"

#define DEFAULT_READ_DATA_LEN  (25 * 1024)

int32_t JpgOpen(StreamSourceHandle *ssHandle, const char *filePath)
{
    if (ssHandle == NULL) {
        MEDIA_ERR_LOG("ssHandle is null");
        return MEDIA_ERR;
    }
    if (filePath == NULL) {
        MEDIA_ERR_LOG("pstFilePath is null");
        return MEDIA_ERR;
    }
    return StreamOpen(ssHandle, filePath, DEFAULT_READ_DATA_LEN);
}

static uint8_t *GetNextMjpeg(uint8_t *bufPtr, uint32_t u32Len)
{
    uint8_t *endPtr = bufPtr + u32Len;
    while (bufPtr != endPtr) {
        if (bufPtr[0] == 0xFF && bufPtr[1] == 0xD8) {
            return bufPtr;
        } else {
            bufPtr++;
        }
    }
    return NULL;
}

static uint32_t GetMjpegFrameLen(const uint8_t *mjpgHeader, uint32_t u32Len)
{
    uint32_t frameLen = 0;
    if (mjpgHeader == NULL) {
        return frameLen;
    }
    uint8_t *endPtr = (uint8_t *)(mjpgHeader + u32Len);
    while (mjpgHeader != endPtr) {
        if (mjpgHeader[0] == 0xFF && mjpgHeader[1] == 0xD9) {
            frameLen += 0x2;
            break;
        } else {
            frameLen++;
            mjpgHeader++;
        }
    }
    return frameLen;
}

int32_t JpgReadFrame(StreamSourceHandle *ssHandle, uint8_t **frame, size_t frameSize,
    uint32_t *frameLen, uint8_t *keyFrame)
{
    MEDIA_UNUSED(keyFrame);
    if (ssHandle == NULL) {
        MEDIA_ERR_LOG("ssHandle is null");
        return MEDIA_ERR;
    }
    if (ssHandle->file == NULL) {
        MEDIA_ERR_LOG("ssHandle->file is null");
        return MEDIA_ERR;
    }
    if (ssHandle->bufHeader == NULL) {
        MEDIA_ERR_LOG("ssHandle->bufHeader is null");
        return MEDIA_ERR;
    }
    if (fseek(ssHandle->file, ssHandle->fileReadOffset, SEEK_SET) != EOK) {
        MEDIA_ERR_LOG("fseek error!");
        return MEDIA_ERR;
    }
    if (memset_s(ssHandle->bufHeader, ssHandle->bufLen, 0x00,
        ssHandle->bufLen) != EOK) {
        MEDIA_ERR_LOG("memset_s error!");
        return MEDIA_ERR;
    }
    uint32_t readLen = fread(ssHandle->bufHeader, 1, ssHandle->bufLen,
                             ssHandle->file);
    if (readLen == 0) {
        MEDIA_ERR_LOG("read file end");
        return MEDIA_READ_EOS;
    }
    ssHandle->availReadLen = readLen;
    uint8_t *mjpegSHeader = GetNextMjpeg(ssHandle->bufHeader, ssHandle->availReadLen);
    if (mjpegSHeader != NULL) {
        *frameLen = GetMjpegFrameLen(mjpegSHeader, ssHandle->availReadLen);
        if (memcpy_s(*frame, frameSize, mjpegSHeader, *frameLen) != EOK) {
            MEDIA_ERR_LOG("memcpy_s failed");
            return MEDIA_ERR;
        }
        ssHandle->fileReadOffset += *frameLen;
        return MEDIA_OK;
    } else {
        MEDIA_ERR_LOG("jpg read EOF");
        ssHandle->fileReadOffset = 0;
        return MEDIA_ERR;
    }
    return MEDIA_OK;
}

int32_t JpgClose(StreamSourceHandle *ssHandle)
{
    if (ssHandle == NULL) {
        MEDIA_ERR_LOG("ssHandle is null");
        return MEDIA_ERR;
    }
    return StreamClose(ssHandle);
}
