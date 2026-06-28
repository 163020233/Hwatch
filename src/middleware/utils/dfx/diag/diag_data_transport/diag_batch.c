/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: diag data transport batch
 * This file should be changed only infrequently and with great care.
 */
 
#include "dfx_adapt_layer.h"
#include "soc_diag_cmd_id.h"
#include "dfx_channel.h"
#include "diag_batch.h"

static inline uint32_t diag_time_diff_ms(uint32_t now, uint32_t old)
{
    return (uint32_t)(now - old);
}

/* 计算单个item编码后占用的总长度 */
static uint16_t diag_batch_calc_encoded_size(const diag_data_item_t *item)
{
    uint16_t payload_len;
    uint16_t total_payload_len;

    if (item == NULL) {
        return 0U;
    }

    payload_len = item->len;

    if (item->enable_ts) {
        total_payload_len = (uint16_t)align_up4(sizeof(item->ts) + item->len);
    } else {
        total_payload_len = payload_len;
    }

    return (uint16_t)(DIAG_ROUTER_HEADER_LEN + DIAG_IND_HEADER_SIZE + total_payload_len);
}

static diag_batch_append_ret_t copy_item_payload(uint8_t *payload_start, uint16_t payload_size,
                                                 const diag_data_item_t *item)
{
    int ret;
    
    if (item->enable_ts) {
        // 拷贝时间戳
        if (payload_size < sizeof(item->ts)) {
            return DIAG_BATCH_APPEND_ERR_PARAM;
        }
        ret = memcpy_s(payload_start, payload_size, &item->ts, sizeof(item->ts));
        if (ret != EOK) {
            return DIAG_BATCH_APPEND_ERR_PARAM;
        }
        
        // 拷贝有效载荷
        uint16_t remaining = payload_size - sizeof(item->ts);
        if (item->len > 0U && remaining >= item->len) {
            ret = memcpy_s(payload_start + sizeof(item->ts), remaining, item->payload, item->len);
            if (ret != EOK) {
                return DIAG_BATCH_APPEND_ERR_PARAM;
            }
        } else if (item->len > 0U) {
            return DIAG_BATCH_APPEND_ERR_PARAM; // 空间不足
        }
    } else {
        // 无时间戳的直接拷贝
        if (item->len > 0U && payload_size >= item->len) {
            ret = memcpy_s(payload_start, payload_size, item->payload, item->len);
            if (ret != EOK) {
                return DIAG_BATCH_APPEND_ERR_PARAM;
            }
        } else if (item->len > 0U) {
            return DIAG_BATCH_APPEND_ERR_PARAM; // 空间不足
        }
    }
    
    return DIAG_BATCH_APPEND_OK;
}

// 更新batch状态
static void update_batch_state(diag_batch_t *batch, uint16_t pkt_size, uint32_t now_ms)
{
    batch->used = (uint16_t)(batch->used + pkt_size);
    batch->item_cnt++;

    if (!batch->started) {
        batch->started = true;
        batch->first_ms = now_ms;
    }
    batch->last_ms = now_ms;
}

errcode_t diag_batch_init(diag_batch_t *batch, uint8_t *buf, uint16_t buf_size, const diag_batch_cfg_t *cfg)
{
    int ret;

    if ((batch == NULL) || (buf == NULL) || (cfg == NULL)) {
        return ERRCODE_INVALID_PARAM;
    }

    if ((cfg->max_buf_size == 0U) ||
        (cfg->bytes_threshold == 0U) ||
        (cfg->timeout_ms == 0U) ||
        (cfg->max_buf_size > buf_size) ||
        (cfg->bytes_threshold > cfg->max_buf_size)) {
        return ERRCODE_INVALID_PARAM;
    }

    ret = memset_s(batch, sizeof(*batch), 0, sizeof(*batch));
    if (ret != EOK) {
        return ERRCODE_MEMSET;
    }

    batch->buf = buf;
    batch->cfg = *cfg;

    ret = memset_s(batch->buf, buf_size, 0, buf_size);
    if (ret != EOK) {
        return ERRCODE_MEMSET;
    }

    return ERRCODE_SUCC;
}

void diag_batch_reset(diag_batch_t *batch)
{
    if ((batch == NULL) || (batch->buf == NULL)) {
        return;
    }

    batch->item_cnt = 0U;
    batch->used = 0U;
    batch->started = false;
    batch->first_ms = 0U;
    batch->last_ms = 0U;
}

// 每个item在append时，直接编码成完整单包
diag_batch_append_ret_t diag_batch_append(diag_batch_t *batch, const diag_data_item_t *item, uint32_t now_ms)
{
    // 参数校验
    if ((batch == NULL) || (item == NULL) || (batch->buf == NULL)) {
        return DIAG_BATCH_APPEND_ERR_PARAM;
    }

    // 计算编码大小
    uint16_t pkt_size = diag_batch_calc_encoded_size(item);
    if (pkt_size > batch->cfg.max_buf_size) {
        return DIAG_BATCH_APPEND_ERR_ITEM_TOO_LARGE; /* 单条完整编码后都放不下，建议单发 */
    }

    // 空间检查
    if ((uint16_t)(batch->used + pkt_size) > batch->cfg.max_buf_size) {
        return DIAG_BATCH_APPEND_ERR_NO_SPACE;
    }

    uint8_t *pkt_start = &batch->buf[batch->used];
    
    // 清零目标区域
    int ret = memset_s(pkt_start, (size_t)(batch->cfg.max_buf_size - batch->used), 0, pkt_size);
    if (ret != EOK) {
        return DIAG_BATCH_APPEND_ERR_PARAM;
    }

    // 设置帧头
    diag_router_frame_t *frame = (diag_router_frame_t *)pkt_start;
    diag_mk_frame_header_1(frame, pkt_size - DIAG_ROUTER_HEADER_LEN);

    // 设置ind头
    diag_head_ind_stru_t *head = (diag_head_ind_stru_t *)(pkt_start + DIAG_ROUTER_HEADER_LEN);
    diag_mk_ind_header(head, item->cmd_id);

    // 拷贝有效载荷
    uint8_t *payload_start = pkt_start + DIAG_ROUTER_HEADER_LEN + DIAG_IND_HEADER_SIZE;
    uint16_t payload_size = pkt_size - (DIAG_ROUTER_HEADER_LEN + DIAG_IND_HEADER_SIZE);
    diag_batch_append_ret_t copy_ret = copy_item_payload(payload_start, payload_size, item);
    if (copy_ret != DIAG_BATCH_APPEND_OK) {
        return copy_ret;
    }

    // 更新批处理状态
    update_batch_state(batch, pkt_size, now_ms);

    // 检查是否需要刷新
    return (batch->used >= batch->cfg.bytes_threshold) ?
            DIAG_BATCH_APPEND_NEED_FLUSH :
            DIAG_BATCH_APPEND_OK;
}

bool diag_batch_should_flush(const diag_batch_t *batch, uint32_t now_ms)
{
    if ((batch == NULL) || (batch->buf == NULL) || (!batch->started) || (batch->used == 0U)) {
        return false;
    }

    if (batch->used >= batch->cfg.bytes_threshold) {
        return true;
    }

    if (diag_time_diff_ms(now_ms, batch->last_ms) >= batch->cfg.timeout_ms) {
        return true;
    }

    return false;
}

uint32_t diag_batch_next_wait_ms(const diag_batch_t *batch, uint32_t now_ms)
{
    uint32_t elapsed;

    if ((batch == NULL) || (batch->buf == NULL) || (!batch->started) || (batch->used == 0U)) {
        return 0xFFFFFFFFU;
    }

    elapsed = diag_time_diff_ms(now_ms, batch->last_ms);
    if (elapsed >= batch->cfg.timeout_ms) {
        return 0U;
    }

    return (batch->cfg.timeout_ms - elapsed);
}

const uint8_t *diag_batch_data(const diag_batch_t *batch)
{
    if ((batch == NULL) || (batch->buf == NULL)) {
        return NULL;
    }
    return batch->buf;
}

uint16_t diag_batch_size(const diag_batch_t *batch)
{
    if (batch == NULL) {
        return 0U;
    }
    return batch->used;
}

uint16_t diag_batch_count(const diag_batch_t *batch)
{
    if (batch == NULL) {
        return 0U;
    }
    return batch->item_cnt;
}