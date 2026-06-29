/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: intelligent voice service tts engine.
 */
#ifndef CIRCLE_BUFFER_H
#define CIRCLE_BUFFER_H

#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    volatile uint32_t *write; /* circle buffer write pointer */
    volatile uint32_t *read;  /* circle buffer read pointer */
    uint8_t *data;            /* circle buffer data virtual address */
    uint32_t size;            /* circle buffer size */
    void *lock;
} CircleBuffer;

typedef struct {
    uint8_t *buf;
    uint32_t size;
    uint32_t read;
    uint32_t write;
    CircleBuffer circleBuffer;
} CacheMemoryInfo;

void CircleBufferinit(CircleBuffer *cb, uint32_t *write, uint32_t *read, uint8_t *data, uint32_t len);

void CircleBufferflush(CircleBuffer *cb);

uint32_t CircleBufferQueryBusy(const CircleBuffer *cb);

uint32_t CircleBufferQueryFree(const CircleBuffer *cb);

uint32_t CircleBufferRead(CircleBuffer *cb, uint8_t *to, uint32_t len);

uint32_t CircleBufferWrite(CircleBuffer *cb, const uint8_t *from, uint32_t len);

#ifdef __cplusplus
}
#endif
#endif