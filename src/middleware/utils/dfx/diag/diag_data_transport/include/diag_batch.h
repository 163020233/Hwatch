/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: diag data transport
 * This file should be changed only infrequently and with great care.
 */

#ifndef DIAG_BATCH_H
#define DIAG_BATCH_H
#include "errcode.h"
#include "diag_data_transport.h"

typedef struct {
    uint16_t max_buf_size;       /* batch buffer总容量 */
    uint16_t bytes_threshold;    /* 到这个字节数立即flush */
    uint32_t timeout_ms;         /* 最后一条加入后，超过这个时间flush */
} diag_batch_cfg_t;

typedef enum {
    DIAG_BATCH_APPEND_OK = 0,
    DIAG_BATCH_APPEND_NEED_FLUSH = 1,
    DIAG_BATCH_APPEND_ERR_PARAM = 2,
    DIAG_BATCH_APPEND_ERR_ITEM_TOO_LARGE = 3,
    DIAG_BATCH_APPEND_ERR_NO_SPACE = 4,
} diag_batch_append_ret_t;

typedef struct {
    uint8_t            *buf;
    uint16_t            used;
    uint16_t            item_cnt;
    bool                started;
    uint32_t            first_ms;
    uint32_t            last_ms;
    diag_batch_cfg_t    cfg;
} diag_batch_t;

/* 初始化，buffer由调用方提供，避免模块内部动态申请 */
errcode_t diag_batch_init(diag_batch_t *batch, uint8_t *buf, uint16_t buf_size, const diag_batch_cfg_t *cfg);

void diag_batch_reset(diag_batch_t *batch);

/*
 * 返回值：diag_batch_append_ret_t
 * DIAG_BATCH_APPEND_OK                     : append成功，暂不需要flush
 * DIAG_BATCH_APPEND_NEED_FLUSH             : append成功，达到阈值，建议立即flush
 * DIAG_BATCH_APPEND_ERR_PARAM              : 参数错误
 * DIAG_BATCH_APPEND_ERR_ITEM_TOO_LARGE     : 单条item太大，整个batch都放不下，建议单发
 * DIAG_BATCH_APPEND_ERR_NO_SPACE           : 当前batch剩余空间不足，建议先flush再重试
 */
diag_batch_append_ret_t diag_batch_append(diag_batch_t *batch, const diag_data_item_t *item, uint32_t now_ms);

bool diag_batch_should_flush(const diag_batch_t *batch, uint32_t now_ms);

/*
 * 返回距离超时flush还要等多久：
 * 0xFFFFFFFFU 表示当前batch为空，可无限等事件
 * 0          表示已经到时，应立即flush
 * 其他值      表示建议等待毫秒数
 */
uint32_t diag_batch_next_wait_ms(const diag_batch_t *batch, uint32_t now_ms);

const uint8_t *diag_batch_data(const diag_batch_t *batch);
uint16_t diag_batch_size(const diag_batch_t *batch);
uint16_t diag_batch_count(const diag_batch_t *batch);

#endif