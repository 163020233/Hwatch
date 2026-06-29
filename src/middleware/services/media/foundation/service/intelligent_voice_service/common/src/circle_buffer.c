
/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: intelligent voice service tts engine.
 */

#include "circle_buffer.h"
#include <string.h>
#include <stdlib.h>
#include "securec.h"
#include "media_thread_adapt.h"

static inline uint32_t CircleBufferMin(uint32_t x, uint32_t y)
{
    return (x <= y) ? x : y;
}

static inline uint32_t SaturateAdd(uint32_t x, uint32_t y, uint32_t saturation)
{
    uint32_t temp = x + y;
    return (temp >= saturation) ? (temp - saturation) : temp;
}

void CircleBufferinit(CircleBuffer *cb, uint32_t *write, uint32_t *read, uint8_t *data, uint32_t len)
{
    cb->write = write;
    cb->read = read;
    cb->data = data;
    cb->size = len;
    *cb->write = 0;
    *cb->read = 0;
}

void CircleBufferflush(CircleBuffer *cb)
{
    *cb->read = 0;
    *cb->write = 0;
}

static inline uint32_t CircleBufferDataSize(uint32_t write, uint32_t read, uint32_t size)
{
    return (write >= read) ? (write - read) : (size - read + write);
}

static inline uint32_t CircleBufferFreeSize(uint32_t write, uint32_t read, uint32_t size)
{
    return size - CircleBufferDataSize(write, read, size);
}

static inline uint32_t CbReadData(const CircleBuffer *cb, uint8_t *to, uint32_t len, uint32_t off)
{
    if ((len == 0) || (memcpy_s(to, len * sizeof(uint8_t), cb->data + off, len) != EOK)) {
        return 0;
    }
    return len;
}

static inline uint32_t CircleBufferReadData(const CircleBuffer *cb, uint8_t *to, uint32_t len, uint32_t off)
{
    uint32_t minLen = CircleBufferMin(len, cb->size - off);
    if (to == NULL) {
        return 0;
    }
    return CbReadData(cb, to, minLen, off) + CbReadData(cb, to + minLen, len - minLen, 0);
}

static inline uint32_t CbWriteData(CircleBuffer *cb, const uint8_t *from, uint32_t len, uint32_t off)
{
    if ((len == 0) || (memcpy_s(cb->data + off, cb->size - off, from, len) != EOK)) {
        return 0;
    }
    return len;
}

static inline uint32_t CbWriteZero(CircleBuffer *cb, const uint8_t *from, uint32_t len, uint32_t off)
{
    if ((len == 0) || (memset_s(cb->data + off, len, 0, len) != EOK)) {
        return 0;
    }
    (void)from;
    return len;
}

static inline uint32_t CircleBufferWriteData(CircleBuffer *cb, const uint8_t *from, uint32_t len, uint32_t off)
{
    uint32_t l = CircleBufferMin(len, cb->size - off);
    uint32_t (*action)(CircleBuffer *, const uint8_t *, uint32_t, uint32_t) =
        (from == NULL) ? CbWriteZero : CbWriteData;
    return action(cb, from, l, off) + action(cb, from + l, len - l, 0);
}

uint32_t CircleBufferQueryBusy(const CircleBuffer *cb)
{
    MediaMutexLock(cb->lock);
    uint32_t ret = CircleBufferDataSize(*cb->write, *cb->read, cb->size);
    MediaMutexUnLock(cb->lock);
    return ret;
}

uint32_t CircleBufferQueryFree(const CircleBuffer *cb)
{
    MediaMutexLock(cb->lock);
    uint32_t ret = CircleBufferFreeSize(*cb->write, *cb->read, cb->size);
    MediaMutexUnLock(cb->lock);
    return ret;
}

uint32_t CircleBufferRead(CircleBuffer *cb, uint8_t *to, uint32_t len)
{
    MediaMutexLock(cb->lock);
    uint32_t read = *cb->read;
    if ((len == 0) || (CircleBufferDataSize(*cb->write, read, cb->size) < len) ||
        (CircleBufferReadData(cb, to, len, read) != len)) {
        MediaMutexUnLock(cb->lock);
        return 0;
    }
    *cb->read = SaturateAdd(read, len, cb->size);
    MediaMutexUnLock(cb->lock);
    return len;
}

uint32_t CircleBufferWrite(CircleBuffer *cb, const uint8_t *from, uint32_t len)
{
    MediaMutexLock(cb->lock);
    uint32_t write = *cb->write;
    if ((len == 0) || (CircleBufferWriteData(cb, from, len, write) != len)) {
        MediaMutexUnLock(cb->lock);
        return 0;
    }
    *cb->write = SaturateAdd(write, len, cb->size);
    MediaMutexUnLock(cb->lock);
    return len;
}