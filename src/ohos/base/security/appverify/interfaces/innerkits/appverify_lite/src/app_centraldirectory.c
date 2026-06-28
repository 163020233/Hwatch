/*
 * Copyright (c) 2020-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "app_centraldirectory.h"
#include <fcntl.h>
#include <stdbool.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "app_common.h"
#include "mbedtls/base64.h"
#include "mbedtls/md.h"
#include "mbedtls/pk.h"
#include "mbedtls/x509_crt.h"
#include "mbedtls_pkcs7.h"
#include "securec.h"

void HapPutByte(const HapBuf *hapBuffer, int32_t offset, char value)
{
    if (hapBuffer == NULL || hapBuffer->buffer == NULL) {
        return;
    }
    if ((offset >= 0) && (hapBuffer->len - offset >= (int)(sizeof(value)))) {
        *(char *)((char *)hapBuffer->buffer + offset) = value;
    }
    return;
}

void HapPutData(const HapBuf *hapBuffer, int32_t offset, const unsigned char *data, int32_t len)
{
    if (hapBuffer == NULL || hapBuffer->buffer == NULL) {
        return;
    }
    if ((data != NULL) && (offset >= 0) && len > 0 && ((hapBuffer->len - offset) >= len)) {
        if (memcpy_s(((unsigned char *)hapBuffer->buffer + offset), (hapBuffer->len - offset), data, len) != EOK) {
            LOG_ERROR("memcpy_s fail");
        }
    }
}

void HapSetInt32(const HapBuf *buffer, int32_t offset, int32_t value)
{
    if (value < 0) {
        LOG_ERROR("int32 value of out range: %d", value);
        return;
    }
    if (buffer == NULL || buffer->buffer == NULL) {
        return;
    }
    if ((offset >= 0) && ((buffer->len - offset) >= (int)(sizeof(value)))) {
        HapPutInt32((unsigned char *)buffer->buffer + offset, buffer->len - offset, (int)(value));
    }
    return;
}

bool CreateHapBuffer(HapBuf *hapBuffer, int32_t len)
{
    if (hapBuffer == NULL || len <= 0) {
        LOG_ERROR("create buf fail, buf is null");
        return false;
    }
    hapBuffer->buffer = APPV_MALLOC(len);
    if (hapBuffer->buffer == NULL) {
        LOG_ERROR("create buf fail");
        return false;
    }
    hapBuffer->len = len;
    return true;
}

void ClearHapBuffer(HapBuf *hapBuffer)
{
    if (hapBuffer == NULL || hapBuffer->buffer == NULL) {
        return;
    }
    (void)memset_s(hapBuffer->buffer, hapBuffer->len, 0, hapBuffer->len);
    APPV_FREE(hapBuffer->buffer);
    hapBuffer->buffer = NULL;
    hapBuffer->len = 0;
}

// ohos begin
// support hap verify
static bool GetEocd(const FileRead *hapFile, HapEocd *hapEocd, int32_t *eocdOffset)
{
    if (hapFile == NULL || hapEocd == NULL || eocdOffset == NULL) {
        return false;
    }

    int fd = hapFile->fp;
    int32_t fileSize = hapFile->len;

    // Search for the signature of MinEocd from the end of the file.
    const int32_t minEocdSize = sizeof(MinEocd);
    const int32_t signature = 0x06054b50;

    // Search for signatures from the end of the file.
    int32_t searchOffset = fileSize - minEocdSize;
    while (searchOffset >= 0) {
        lseek(fd, searchOffset, SEEK_SET);
        // Reading the magic field
        int32_t magic;
        if (read(fd, &magic, sizeof(magic)) != sizeof(magic)) {
            return false;
        }

        if (magic == signature) {
            // Find the EOCD signature and set the EOCD offset.
            *eocdOffset = searchOffset;

            // Reads the entire MinEocd structure.
            lseek(fd, searchOffset, SEEK_SET);
            if (read(fd, &hapEocd->eocdHead, minEocdSize) != minEocdSize) {
                return false;
            }

            // Reading Comments
            if (hapEocd->eocdHead.commentLen > 0) {
                hapEocd->comment = (char *)APPV_MALLOC(hapEocd->eocdHead.commentLen + 1);
                if (hapEocd->comment == NULL) {
                    return false;
                }
                if (read(fd, hapEocd->comment, hapEocd->eocdHead.commentLen) != hapEocd->eocdHead.commentLen) {
                    APPV_FREE(hapEocd->comment);
                    return false;
                }
                hapEocd->comment[hapEocd->eocdHead.commentLen] = '\0';
            } else {
                hapEocd->comment = NULL; // No comment
            }
            return true;
        }
        searchOffset--; // Keep searching forward
    }
    return false; // EOCD signature not found
}
// ohos end

bool FindSignature(const FileRead *hapFile, SignatureInfo *signInfo)
{
    if (hapFile == NULL || signInfo == NULL) {
        return false;
    }
    
    int32_t eocdOffset = 0;
    HapEocd hapEocd = {0};
    bool success = false;
    
    if (!GetEocd(hapFile, &hapEocd, &eocdOffset)) {
        LOG_ERROR("find Eocd fail");
        goto CLEANUP;
    }
    
    signInfo->hapEocdOffset = eocdOffset;
    signInfo->hapEocdSize = hapFile->len - eocdOffset;
    signInfo->hapCoreDirOffset = HapGetInt((unsigned char*)(&(hapEocd.eocdHead.coreDirOffset)), sizeof(int));
    
    if (signInfo->hapCoreDirOffset <= 0 || 
        signInfo->hapCoreDirOffset >= eocdOffset ||
        signInfo->hapEocdSize <= 0 || 
        signInfo->hapEocdOffset <= 0) {
        LOG_ERROR("core dir error");
        goto CLEANUP;
    }
    
    success = true;
    
CLEANUP:
    if (hapEocd.comment != NULL) {
        APPV_FREE(hapEocd.comment);
        hapEocd.comment = NULL;
    }
    
    return success;
}

int32_t ReadFileFullyFromOffset(const HapBuf *buffer, int32_t offset, const FileRead *file)
{
    if (buffer == NULL || buffer->buffer == NULL || file == NULL) {
        return DEST_BUFFER_IS_NULL;
    }
    if (offset < 0 || offset > file->len) {
        return READ_OFFSET_OUT_OF_RANGE;
    }
    lseek(file->fp, offset, SEEK_SET);
    int32_t readLen = read(file->fp, buffer->buffer, buffer->len);
    if (readLen != buffer->len) {
        LOG_ERROR("file read error %d --- %d", readLen, buffer->len);
        return READ_OFFSET_OUT_OF_RANGE;
    }
    return buffer->len;
}
