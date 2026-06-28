/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: read acc data
 * Author: Media Software Group
 * Create: 2025-06-16
 */

#include "aac.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "securec.h"
#include "media_log.h"

#define DEFAULT_READ_DATA_LEN  (2 * 1024)

static uint8_t *GetNextADTS(uint8_t *bufPtr, uint32_t u32Len)
{
    uint8_t *endPtr = bufPtr + u32Len;
    while (bufPtr != endPtr) {
        if (bufPtr[0] == 0xFF && bufPtr[1] == 0xF1) {
            return bufPtr;
        } else {
            bufPtr++;
        }
    }
    return NULL;
}

static uint32_t GetAACFrameLen(const uint8_t *adtsHeader)
{
    uint32_t frameLen = 0;
    if (adtsHeader == NULL) {
        return MEDIA_OK;
    }
    uint32_t fBit = adtsHeader[0x3];
    uint32_t mBit = adtsHeader[0x4];
    uint32_t bBit = adtsHeader[0x5];
    frameLen += (bBit >> 0x5);
    frameLen += (mBit << 0x3);
    frameLen += ((fBit & 0x3) << 0xB);
    return frameLen;
}

int32_t AacOpen(StreamSourceHandle *ssHandle, const char *filePath)
{
    if (ssHandle == NULL) {
        MEDIA_ERR_LOG("ssHandle is null");
        return MEDIA_ERR;
    }
    if (filePath == NULL) {
        MEDIA_ERR_LOG("filePath is null");
        return MEDIA_ERR;
    }
    return StreamOpen(ssHandle, filePath, DEFAULT_READ_DATA_LEN);
}

int32_t AacReadFrame(StreamSourceHandle *ssHandle, uint8_t **frame, size_t frameSize,
    uint32_t *frameLen, uint8_t *keyFrame)
{
    MEDIA_UNUSED(keyFrame);
    if (ssHandle == NULL || ssHandle->file == NULL) {
        MEDIA_ERR_LOG("ssHandle or ssHandle->file is null");
        return MEDIA_ERR;
    }
    if (frame == NULL || frameLen == NULL) {
        MEDIA_ERR_LOG("frame or frameLen is null");
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
        MEDIA_ERR_LOG("AAC_ReadFrame read file end");
        ssHandle->fileReadOffset = 0;
        return MEDIA_READ_EOS;
    }
    ssHandle->availReadLen = readLen;
    uint8_t *bufPtr = ssHandle->bufHeader;
    uint8_t *adtsHeader = GetNextADTS(bufPtr, ssHandle->availReadLen);
    if (adtsHeader != NULL) {
        *frameLen = GetAACFrameLen(adtsHeader);
        if (memcpy_s(*frame, frameSize, adtsHeader, *frameLen) != EOK) {
            MEDIA_ERR_LOG("memcpy_s failed");
            return MEDIA_ERR;
        }
        ssHandle->fileReadOffset += *frameLen;
        return MEDIA_OK;
    } else {
        MEDIA_ERR_LOG("aac read EOF");
        ssHandle->fileReadOffset = 0;
        return MEDIA_ERR;
    }
    return MEDIA_OK;
}

int32_t AacClose(StreamSourceHandle *ssHandle)
{
    if (ssHandle == NULL) {
        MEDIA_ERR_LOG("ssHandle is null");
        return MEDIA_ERR;
    }
    return StreamClose(ssHandle);
}

int32_t AacReset(StreamSourceHandle *ssHandle)
{
    if (ssHandle == NULL) {
        MEDIA_ERR_LOG("ssHandle is null");
        return MEDIA_ERR;
    }
    return StreamReset(ssHandle);
}
