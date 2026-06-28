/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: read avc data
 * Author: Media Software Group
 * Create: 2025-06-16
 */

#include "avc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "securec.h"
#include "media_log.h"

#define DEFAULT_READ_DATA_LEN  (40 * 1024)

static uint8_t *GetNextNal(uint8_t *bufPtr, uint32_t u32Len, uint32_t *startCodeLen)
{
    if (bufPtr == NULL) {
        MEDIA_ERR_LOG("ssHandle is null\n");
        return NULL;
    }
    if (startCodeLen == NULL) {
        MEDIA_ERR_LOG("startCodeLen is null\n");
        return NULL;
    }
    uint8_t *endPtr = bufPtr + u32Len;
    while (bufPtr != endPtr) {
        if (bufPtr[0] == 0x00 && bufPtr[1] == 0x00 &&
            bufPtr[2] == 0x00 && bufPtr[3] == 0x01) { /* 2 3: start code position */
            *startCodeLen = 4; /* 4: start code len */
            return bufPtr;
        } else if (bufPtr[0] == 0x00 && bufPtr[1] == 0x00 && bufPtr[2] == 0x01) { /* 2: start code position */
            *startCodeLen = 3; /* 3: start code len */
            return bufPtr;
        } else {
            bufPtr++;
        }
    }
    return NULL;
}

int32_t AvcOpen(StreamSourceHandle *ssHandle, const char *filePath)
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

static int32_t ReadFrame(StreamSourceHandle *ssHandle, uint8_t **frame, size_t frameSize,
    uint32_t *frameLen, uint8_t *keyFrame)
{
    uint32_t firstNaltartCodeLen = 0;
    uint32_t secondNalstartCodeLen = 0;
    uint8_t *endPtr = ssHandle->bufHeader + ssHandle->availReadLen;
    uint8_t *firstNalPtr = GetNextNal(ssHandle->bufHeader,
        ssHandle->availReadLen, &firstNaltartCodeLen);
    if (firstNalPtr == NULL) {
        MEDIA_ERR_LOG("avc read error\n");
        ssHandle->fileReadOffset = 0;
        return MEDIA_ERR;
    }

    uint8_t *secondNalPtr = GetNextNal(firstNalPtr + firstNaltartCodeLen,
        endPtr - (firstNalPtr + firstNaltartCodeLen), &secondNalstartCodeLen);
    int32_t nalType = (firstNalPtr + firstNaltartCodeLen)[0] & 0x1F;
    if (nalType == 7 && secondNalPtr != NULL) {  /* 7: type SPS */
        int32_t frameType = (secondNalPtr + secondNalstartCodeLen)[0] & 0x1F;
        while (frameType != 5 && (secondNalPtr != NULL) && secondNalPtr < endPtr) {  /* 5: IDR */
            secondNalPtr = GetNextNal(secondNalPtr + secondNalstartCodeLen,
                endPtr - (secondNalPtr + secondNalstartCodeLen), &secondNalstartCodeLen);
            if (secondNalPtr != NULL) {
                frameType = (secondNalPtr + secondNalstartCodeLen)[0] & 0x1F;
            }
        }
        if (frameType == 5) {  /* 5: IDR */
            *keyFrame = 1;
            if (secondNalPtr != NULL) {
                secondNalPtr = GetNextNal(secondNalPtr + secondNalstartCodeLen,
                    endPtr - (secondNalPtr + secondNalstartCodeLen), &secondNalstartCodeLen);
            }
        }
    } else if (nalType == 5) {  /* 5: type IDR I frame */
        *keyFrame = 1;
    }
    if (secondNalPtr == NULL) {
        MEDIA_ERR_LOG("read second nal error");
        *frameLen = ssHandle->availReadLen;
    } else {
        *frameLen = (secondNalPtr - firstNalPtr);
    }
    if (memcpy_s(*frame, frameSize, firstNalPtr, *frameLen) != EOK) {
        MEDIA_ERR_LOG("memcpy_s failed");
        return MEDIA_ERR;
    }
    ssHandle->fileReadOffset += *frameLen;
    return MEDIA_OK;
}

int32_t AvcReadFrame(StreamSourceHandle *ssHandle, uint8_t **frame, size_t frameSize,
    uint32_t *frameLen, uint8_t *keyFrame)
{
    if (ssHandle == NULL || frame == NULL) {
        MEDIA_ERR_LOG("ssHandle or frame is null\n");
        return MEDIA_ERR;
    }
    if (ssHandle->file == NULL) {
        MEDIA_ERR_LOG("ssHandle->file is null");
        return MEDIA_ERR;
    }
    if (ssHandle->bufHeader == NULL) {
        MEDIA_ERR_LOG("ssHandle->bufHeader is null\n");
        return MEDIA_ERR;
    }
    if (fseek(ssHandle->file, ssHandle->fileReadOffset, SEEK_SET) != EOK) {
        MEDIA_ERR_LOG("fseek error ! %s  %d\n", __FUNCTION__, __LINE__);
        return MEDIA_ERR;
    }
    if (memset_s(ssHandle->bufHeader, ssHandle->bufLen, 0x00,
        ssHandle->bufLen) != EOK) {
        MEDIA_ERR_LOG("memset_s error ! %s  %d\n", __FUNCTION__, __LINE__);
        return MEDIA_ERR;
    }
    uint32_t readLen = fread(ssHandle->bufHeader, 1, ssHandle->bufLen,
        ssHandle->file);
    if (readLen == 0) {
        MEDIA_ERR_LOG("AVC_ReadFrame read file end\n");
        return MEDIA_READ_EOS;
    }
    if (frameLen == NULL || keyFrame == NULL) {
        MEDIA_ERR_LOG("frameLen or keyFrame is null\n");
        return MEDIA_ERR;
    }
    ssHandle->availReadLen = readLen;
    return ReadFrame(ssHandle, frame, frameSize, frameLen, keyFrame);
}

int32_t AvcReset(StreamSourceHandle *ssHandle)
{
    if (ssHandle == NULL) {
        MEDIA_ERR_LOG("ssHandle is null\n");
        return MEDIA_ERR;
    }
    return StreamReset(ssHandle);
}

int32_t AvcClose(StreamSourceHandle *ssHandle)
{
    if (ssHandle == NULL) {
        MEDIA_ERR_LOG("ssHandle is null\n");
        return MEDIA_ERR;
    }
    return StreamClose(ssHandle);
}
