/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "buffer_source.h"
#include "securec.h"
#include "media_log.h"
#include "player_check.h"
#include "media_mem.h"
#include "format_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

const int32_t EOS_LEN = 4; /* set eos len 4, ensure the eos flag can be read */
const uint32_t BUFFER_SOURCE_FLAG_EOS = 4; /* STREAM_FLAG_EOS */
const uint32_t AAC_BUFER_SIZE = (2 * 1024);
const uint32_t H264_BUFER_SIZE = (38 * 1024);
const uint32_t MJPEG_BUFER_SIZE = (25 * 1024);
const uint32_t VIDEO_BUFER_SIZE = 4;
const uint32_t AUDIO_BUFER_SIZE = 3;

void BufferSource::InitCodecbufferSize(void)
{
    codecBufferSize_.emplace(CODEC_AAC, AAC_BUFER_SIZE);
    codecBufferSize_.emplace(CODEC_H264, H264_BUFER_SIZE);
    codecBufferSize_.emplace(CODEC_MJPEG, MJPEG_BUFER_SIZE);
}

uint32_t BufferSource::GetCodecbufferSizeByFormatType(uint32_t formatType)
{
    auto iter = codecBufferSize_.find(formatType);
    if (iter == codecBufferSize_.end()) {
        MEDIA_INFO_LOG("not support formatType: %u", formatType);
        return 0;
    }
    return iter->second;
}

BufferSource::BufferSource(void)
    : inited_(false),
      idleQueMutex_(MediaMutexCreate(nullptr)),
      filledQueMutex_(MediaMutexCreate(nullptr))
{
    idleBuffer_.clear();
    filledBuffer_.clear();
    if (memset_s(buffer_, sizeof(BufferInfo) * QUEUE_SIZE, 0, sizeof(BufferInfo) * QUEUE_SIZE) != EOK)  {
        MEDIA_ERR_LOG("BufferSource memset_s failed");
    }
    InitCodecbufferSize();
}

BufferSource::~BufferSource(void)
{
    int i;
    MediaMutexDestroy(&idleQueMutex_);
    MediaMutexDestroy(&filledQueMutex_);
    if (!inited_) {
        return;
    }
    idleBuffer_.clear();
    filledBuffer_.clear();
    for (i = 0; i < QUEUE_SIZE; i++) {
        if (buffer_[i].virAddr != nullptr) {
            MediaMemFree(buffer_[i].virAddr);
            buffer_[i].virAddr = nullptr;
        }
    }
    inited_ = false;
}

int32_t BufferSource::InitBuffer(uint32_t index, int32_t flag, uint32_t bufferSize)
{
    buffer_[index].virAddr = MediaMemMalloc(bufferSize);
    if (buffer_[index].virAddr == nullptr) {
        MEDIA_ERR_LOG("BufferSource::Init, malloc failed\n");
        return -1;
    }
    buffer_[index].phyAddr = 0;
    buffer_[index].fd = -1;
    buffer_[index].bufLen = bufferSize;
    buffer_[index].offset = 0;
    buffer_[index].size = bufferSize;
    buffer_[index].idx = index;

    QueBuffer buffer;
    buffer.idx = index;
    buffer.flag = flag;
    buffer.offset = 0;
    buffer.size = 0;
    buffer.timestamp = 0;
    QueIdleBuffer(&buffer);
    return 0;
}

int32_t BufferSource::InitAudio(uint32_t formatType)
{
    uint32_t bufferSize = GetCodecbufferSizeByFormatType(formatType);
    if (bufferSize == 0) {
        MEDIA_ERR_LOG("GetCodecbufferSizeByFormatType failed!Not support formatType[%u]", formatType);
        return -1;
    }
    for (uint32_t i = 0; i < AUDIO_BUFER_SIZE; i++) {
        if (InitBuffer(i, BUFFER_FLAG_AUDIO_FRAME, bufferSize) < 0) {
            MEDIA_ERR_LOG("InitBuffer failed!");
            goto failed;
        }
    }
    MEDIA_INFO_LOG("idleQue size:%d", idleBuffer_.size());
    return 0;
failed:
    inited_ = false;
    for (uint32_t i = 0; i < AUDIO_BUFER_SIZE; i++) {
        if (buffer_[i].virAddr != nullptr) {
            MediaMemFree(buffer_[i].virAddr);
            buffer_[i].virAddr = nullptr;
        }
    }
    return -1;
}

int32_t BufferSource::InitVideo(uint32_t formatType)
{
    uint32_t bufferSize = GetCodecbufferSizeByFormatType(formatType);
    if (bufferSize == 0) {
        MEDIA_ERR_LOG("GetCodecbufferSizeByFormatType failed!Not support formatType[%u]", formatType);
        return -1;
    }
    int32_t idleBufSize = GetIdleQueSize();
    if (idleBufSize < 0) {
        MEDIA_ERR_LOG("GetCodecbufferSizeByFormatType failed! idleBufSize[%d]", idleBufSize);
        return -1;
    }
    for (uint32_t i = idleBufSize; i < static_cast<uint32_t>(idleBufSize) + VIDEO_BUFER_SIZE; i++) {
        if (InitBuffer(i, BUFFER_FLAG_VIDEO_FRAME, bufferSize) < 0) {
            MEDIA_ERR_LOG("InitBuffer failed!");
            goto failed;
        }
    }
    MEDIA_INFO_LOG("idleQue size:%d", idleBuffer_.size());
    return 0;
failed:
    inited_ = false;
    for (uint32_t i = AUDIO_BUFER_SIZE; i < AUDIO_BUFER_SIZE + VIDEO_BUFER_SIZE; i++) {
        if (buffer_[i].virAddr != nullptr) {
            MediaMemFree(buffer_[i].virAddr);
            buffer_[i].virAddr = nullptr;
        }
    }
    return -1;
}

int BufferSource::Init(uint32_t audioFormatType, uint32_t videoFormatType)
{
    inited_ = true;
    int32_t audioRet = InitAudio(audioFormatType);
    int32_t videoRet = InitVideo(videoFormatType);
    if (audioRet != 0 && videoRet != 0) {
        inited_ = false;
        return -1;
    }
    return 0;
}

int BufferSource::GetQueSize(void)
{
    return QUEUE_SIZE;
}

int BufferSource::GetBufferInfo(int idx, BufferInfo* info)
{
    if (!inited_) {
        return -1;
    }
    CHK_NULL_RETURN(info, "info is nullptr");
    if (idx < 0 || idx >= QUEUE_SIZE) {
        return -1;
    }
    *info = buffer_[idx];
    return 0;
}

int BufferSource::QueIdleBuffer(const QueBuffer *buffer)
{
    if (!inited_) {
        return -1;
    }
    MediaMutexLock(idleQueMutex_);
    CHK_NULL_RETURN_WITH_UNLOCK(buffer, idleQueMutex_, "buffer is nullptr");
    idleBuffer_.push_back(*buffer);
    MediaMutexUnLock(idleQueMutex_);
    return 0;
}

int BufferSource::DequeIdleBuffer(QueBuffer* buffer, int timeOut)
{
    MEDIA_UNUSED(timeOut);
    if (!inited_) {
        return -1;
    }
    MediaMutexLock(idleQueMutex_);
    CHK_NULL_RETURN_WITH_UNLOCK(buffer, idleQueMutex_, "buffer is nullptr");
    if (idleBuffer_.empty() != 0) {
        MediaMutexUnLock(idleQueMutex_);
        return -1;
    }
    *buffer = idleBuffer_[0];
    idleBuffer_.erase(idleBuffer_.begin());
    MediaMutexUnLock(idleQueMutex_);
    return 0;
}

int32_t BufferSource::GetIdleQueSize(void)
{
    size_t size;
    if (!inited_) {
        return -1;
    }
    MediaMutexLock(idleQueMutex_);
    size = idleBuffer_.size();
    MediaMutexUnLock(idleQueMutex_);
    return size;
}

int BufferSource::QueFilledBuffer(const QueBuffer *buffer)
{
    if (!inited_) {
        return -1;
    }
    MediaMutexLock(filledQueMutex_);
    CHK_NULL_RETURN_WITH_UNLOCK(buffer, filledQueMutex_, "buffer is nullptr");
    filledBuffer_.push_back(*buffer);
    MediaMutexUnLock(filledQueMutex_);
    return 0;
}

int BufferSource::DequeFilledBuffer(QueBuffer* buffer, int timeOut)
{
    MEDIA_UNUSED(timeOut);
    if (!inited_) {
        return -1;
    }
    MediaMutexLock(filledQueMutex_);
    CHK_NULL_RETURN_WITH_UNLOCK(buffer, filledQueMutex_, "buffer is nullptr");
    if (filledBuffer_.empty() != 0) {
        MediaMutexUnLock(filledQueMutex_);
        return -1;
    }
    *buffer = filledBuffer_[0];
    filledBuffer_.erase(filledBuffer_.begin());
    MediaMutexUnLock(filledQueMutex_);
    return 0;
}

size_t BufferSource::GetFilledQueSize(void)
{
    size_t size;
    if (!inited_) {
        return -1;
    }
    MediaMutexLock(filledQueMutex_);
    size = filledBuffer_.size();
    MediaMutexUnLock(filledQueMutex_);
    return size;
}

int32_t BufferSource::GetFilledQueDataSize(void)
{
    size_t size;
    int32_t len = 0;
    if (!inited_) {
        return -1;
    }
    MediaMutexLock(filledQueMutex_);
    size = filledBuffer_.size();
    if (size > 0) {
        len = filledBuffer_[0].size;
    }
    /* fix the issue last buffer have not been read, when the buffer only include a eos flag */
    if (size == 1 && len == 0 && filledBuffer_[0].flag == BUFFER_SOURCE_FLAG_EOS) {
        len = EOS_LEN;
    }
    MediaMutexUnLock(filledQueMutex_);
    return len;
}

int BufferSource::GetFilledBuffer(size_t idx, QueBuffer* buffer)
{
    size_t queSize;
    if (!inited_) {
        return -1;
    }
    MediaMutexLock(filledQueMutex_);
    CHK_NULL_RETURN_WITH_UNLOCK(buffer, filledQueMutex_, "buffer is nullptr");
    queSize = filledBuffer_.size();
    if (queSize <= idx) {
        MediaMutexUnLock(filledQueMutex_);
        return -1;
    }
    *buffer = filledBuffer_[idx];
    MediaMutexUnLock(filledQueMutex_);
    return 0;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
