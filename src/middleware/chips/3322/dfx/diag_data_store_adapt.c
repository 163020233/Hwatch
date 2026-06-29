/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: diag data store adapt
 * This file should be changed only infrequently and with great care.
 */

#include "dfx_adapt_layer.h"
#include "los_queue.h"
#include "dfx_cloud_report.h"
#include "diag_data_store_adapt.h"

#define NON_CRITICAL_QUEUE_NUM 2
#define DIAG_NON_CRITICAL_QUEUE_MAX_ITEMS 512 // Max number of items in the non-critical queue (for log).
#define DIAG_NON_CRITICAL_QUEUE_ITEMS 128 // Number of items in the non-critical queue (for other metrics).
#define DIAG_CRITICAL_QUEUE_MAX_ITEMS 32 // Max number of items in the critical queue.
#define CRITICAL_QUEUE_WAIT_TIMEOUT 5000

static unsigned long g_diag_non_critical_queue_id[NON_CRITICAL_QUEUE_NUM] = { 0xFFFFFFFF, 0xFFFFFFFF };
static unsigned long g_diag_critical_queue_id = 0xFFFFFFFF;

/*
 * Backend implementation for 'deinit'
 */
static void queue_store_non_critical_deinit(void)
{
    diag_data_item_t *item_to_free;
    uint32_t size = sizeof(void *);
    for (int i = 0; i < NON_CRITICAL_QUEUE_NUM; i++) {
        while (osal_msg_queue_read_copy(g_diag_non_critical_queue_id[i], &item_to_free, &size, 0) == ERRCODE_SUCC) {
            dfx_free(0, item_to_free);
        }
        if (g_diag_non_critical_queue_id[i] != 0xFFFFFFFF) {
            (void)osal_msg_queue_delete(g_diag_non_critical_queue_id[i]);
            g_diag_non_critical_queue_id[i] = 0xFFFFFFFF;
        }
    }
}

/*
 * Backend implementation for 'init'
 */
static errcode_t queue_store_non_critical_init(void)
{
    int ret;
    // The size of one element in the queue is the size of our internal wrapper struct.
    uint16_t item_size = sizeof(void *);
    char name[16];
    uint32_t queue_len = 0;

    // Create multiple non-critical queues. The first one is for logs and should be larger.
    for (int i = 0; i < NON_CRITICAL_QUEUE_NUM; i++) {
        ret = snprintf_s(name, sizeof(name), sizeof(name) - 1, "diag_ncq_%d", i);
        if (ret < 0) {
            return ERRCODE_FAIL;
        }
        queue_len = (i == 0) ? DIAG_NON_CRITICAL_QUEUE_MAX_ITEMS : DIAG_NON_CRITICAL_QUEUE_ITEMS;
        ret = osal_msg_queue_create(name, queue_len, &g_diag_non_critical_queue_id[i], 0, item_size);
        if (ret != 0) {
            dfx_log_err("[DIAG_REPORT] diag_ncq create fail\n");
            queue_store_non_critical_deinit();
            return ERRCODE_FAIL;
        }
    }
    return ERRCODE_SUCC;
}

/*
 * Backend implementation for 'non_critical_write'
 */
static errcode_t queue_store_non_critical_write(uint16_t cmd_id, const void *data,
                                                uint32_t len, const diag_data_store_ctx_t *ctx)
{
    if (len > MAX_DIAG_ITEM_PAYLOAD_SIZE || data == NULL || !ctx) {
        // Data is too large to fit in our queue item payload or ctx not specified
        return ERRCODE_FAIL;
    }

    // Select the queue to write to based on the queue index in the context.
    uint8_t qidx = ctx->idx % NON_CRITICAL_QUEUE_NUM;
    uint32_t qid = g_diag_non_critical_queue_id[qidx];
    uint32_t qsize = (qidx == 0) ? DIAG_NON_CRITICAL_QUEUE_MAX_ITEMS : DIAG_NON_CRITICAL_QUEUE_ITEMS;
    bool enable_timestamp = ctx->enable_timestamp;
    if (qid == 0xFFFFFFFF) {
        return ERRCODE_DIAG_NOT_INIT;
    }

    uint32_t total_size = sizeof(diag_data_item_t) + len;
    diag_data_item_t *item = (diag_data_item_t *)dfx_malloc(0, total_size);
    if (item == NULL) {
        return ERRCODE_MALLOC;
    }

    item->cmd_id = cmd_id;
    item->enable_ts = enable_timestamp;
    if (enable_timestamp) {
        item->ts = dfx_get_cur_second();
    }
    item->len = (uint16_t)len;
    if ((errcode_t)memcpy_s(item->payload, len, data, len) != ERRCODE_SUCC) {
        dfx_free(0, item);
        return ERRCODE_MEMCPY;
    }

    uint32_t msg_num = osal_msg_queue_get_msg_num(qid);
    // 当队列满时，丢弃最早的消息，保证队列不被填满
    if (msg_num == qsize) {
        diag_data_item_t *dummy = NULL;
        uint32_t dummy_len = sizeof(void *);
        (void)osal_msg_queue_read_copy(qid, &dummy, &dummy_len, 0);
        if (dummy) {
            dfx_free(0, dummy);
        }
    }

    errcode_t ret = (errcode_t)osal_msg_queue_write_copy(qid, &item, sizeof(void *), 0);
    if (ret != ERRCODE_SUCC) {
        dfx_free(0, item);
    } else {
        diag_write_evt_t evt = {
            .item_num      = msg_num + 1,
            .byte_num      = 0,
            .trigger_flags = DIAG_TRIGGER_NONE,
            .now_ms        = dfx_get_cur_second(),
        };
        // 触发上报事件
        diag_report_write_event(&evt);
    }
    return ret;
}

/*
 * Backend implementation for 'non_critical_read'
 */
static errcode_t queue_store_non_critical_read(diag_data_item_t **item, uint32_t timeout_ms)
{
    if (item == NULL) {
        return ERRCODE_INVALID_PARAM;
    }
    static uint16_t cursor = 0; // Round-robin read index from the non-critical queues.
    uint32_t remaining_ms = timeout_ms;
    const uint32_t slice_ms_base =
        (timeout_ms == (uint32_t)OSAL_MSGQ_WAIT_FOREVER) ? 50 : 5; // 每次尝试片段（5~50ms）
    const uint32_t slice_ms_min  = (timeout_ms == 0) ? 0 : 1;

    for (;;) {
        uint16_t start = cursor;
        for (uint8_t i = 0; i < NON_CRITICAL_QUEUE_NUM; ++i) {
            uint16_t idx = (start + i) % NON_CRITICAL_QUEUE_NUM;
            uint32_t qid = g_diag_non_critical_queue_id[idx];
            if (qid == 0xFFFFFFFF) {
                continue;
            }

            uint32_t slice_ms;
            uint32_t len = sizeof(void *);
            uint32_t ret;
            if (timeout_ms == (uint32_t)OSAL_MSGQ_WAIT_FOREVER) {
                slice_ms = slice_ms_base;
            } else if (remaining_ms == 0) {
                slice_ms = 0;
            } else {
                // 平均分配剩余时间，避免单队列独占
                uint32_t avg = remaining_ms / (uint32_t)(NON_CRITICAL_QUEUE_NUM - i);
                avg = (avg < slice_ms_min) ? slice_ms_min : avg;
                avg = (avg > slice_ms_base) ? slice_ms_base : avg;
                slice_ms = avg;
            }

            ret = (errcode_t)osal_msg_queue_read_copy(qid, item, &len, slice_ms);
            if (ret == ERRCODE_SUCC) {
                // 下一次从当前队列的下一个开始
                cursor = (uint16_t)((idx + 1) % NON_CRITICAL_QUEUE_NUM);
                return ERRCODE_SUCC;
            }

            if (timeout_ms != (uint32_t)OSAL_MSGQ_WAIT_FOREVER && remaining_ms >= slice_ms) {
                remaining_ms -= slice_ms;
            }
        }

        if (timeout_ms == 0) {
            return ERRCODE_FAIL; // 非阻塞模式，一圈没读到直接返回
        }
        if (timeout_ms != (uint32_t)OSAL_MSGQ_WAIT_FOREVER && remaining_ms == 0) {
            return ERRCODE_FAIL; // 超时
        }
        // FOREVER 模式：继续下一轮
    }
}

/*
 * Backend implementation for 'non_critical_release'
 * For a standard queue where read is destructive, this is a no-op.
 * It's here for API compatibility with storage like ring buffers.
 */
static errcode_t queue_store_non_critical_release(diag_data_item_t *item)
{
    if (item == NULL) {
        return ERRCODE_INVALID_PARAM;
    }
    dfx_free(0, item);
    return ERRCODE_SUCC;
}

/*
 * Backend implementation for 'init'
 */
static errcode_t queue_store_critical_init(void)
{
    int ret;
    // The size of one element in the queue is the size of our internal wrapper struct.
    uint16_t item_size = sizeof(void *);
    ret = osal_msg_queue_create("diag_cq", DIAG_CRITICAL_QUEUE_MAX_ITEMS, &g_diag_critical_queue_id, 0, item_size);
    if (ret != 0) {
        dfx_log_err("[DIAG_REPORT] diag critical Q create fail\n");
        return ERRCODE_FAIL;
    }

    return ERRCODE_SUCC;
}

/*
 * Backend implementation for 'deinit'
 */
static void queue_store_critical_deinit(void)
{
    diag_data_item_t *item_to_free;
    uint32_t item_size = sizeof(void *);
    while (osal_msg_queue_read_copy(g_diag_critical_queue_id, &item_to_free, &item_size, 0) == ERRCODE_SUCC) {
        dfx_free(0, item_to_free);
    }
    if (g_diag_critical_queue_id != 0xFFFFFFFF) {
        (void)osal_msg_queue_delete(g_diag_critical_queue_id);
        g_diag_critical_queue_id = 0xFFFFFFFF;
    }
}

/*
 * Backend implementation for 'critical_write'
 */
static errcode_t queue_store_critical_write(uint16_t cmd_id, const void *data, uint32_t len, bool enable_timestamp)
{
    if (g_diag_critical_queue_id == 0xFFFFFFFF) {
        return ERRCODE_DIAG_NOT_INIT;
    }
    if (len > MAX_DIAG_ITEM_PAYLOAD_SIZE || data == NULL) {
        // Data is too large to fit in our queue item payload
        return ERRCODE_FAIL;
    }
    dfx_log_debug("[DIAG] critical_write cmd_id=0x%04x, len=%u\n", cmd_id, len);
    uint32_t total_size = sizeof(diag_data_item_t) + len;
    diag_data_item_t *item = (diag_data_item_t *)dfx_malloc(0, total_size);

    if (item == NULL) {
        return ERRCODE_MALLOC;
    }

    item->cmd_id = cmd_id;
    item->enable_ts = enable_timestamp;
    if (enable_timestamp) {
        item->ts = dfx_get_cur_second();
    }
    item->len = (uint16_t)len;
    int ret = (int)memcpy_s(item->payload, len, data, len);
    if (ret != EOK) {
        dfx_free(0, item);
        return ERRCODE_MEMCPY;
    }

    ret = osal_msg_queue_write_copy(g_diag_critical_queue_id, &item, sizeof(void *), 0);
    if (ret != 0) {
        dfx_free(0, item);
        return ERRCODE_FAIL;
    } else {
        diag_write_evt_t evt = {
            .item_num      = 0,
            .byte_num      = 0,
            .trigger_flags = DIAG_TRIGGER_IMMEDIATE,
            .now_ms        = dfx_get_cur_second(),
        };
        // 触发上报事件
        diag_report_write_event(&evt);
        return ERRCODE_SUCC;
    }
}

/*
 * Backend implementation for 'critical_read'
 */
static errcode_t queue_store_critical_read(diag_data_item_t **item, uint32_t timeout_ms)
{
    if (g_diag_critical_queue_id == 0xFFFFFFFF || item == NULL) {
        return ERRCODE_DIAG_NOT_INIT;
    }
    uint32_t len = sizeof(void *);
    errcode_t ret = (errcode_t)osal_msg_queue_read_copy(g_diag_critical_queue_id, item, &len, timeout_ms);
    return ret;
}

/*
 * Backend implementation for 'critical_release'
 * For a standard queue where read is destructive, this is a no-op.
 * It's here for API compatibility with storage like ring buffers.
 */
static errcode_t queue_store_critical_release(diag_data_item_t *item)
{
    if (item == NULL) {
        return ERRCODE_INVALID_PARAM;
    }
    dfx_free(0, item);
    return ERRCODE_SUCC;
}

// --- 静态驱动实例 ---
// 注册到核心逻辑层的 驱动对象
static diag_data_store_driver_t g_queue_non_critical_driver = {
    .init = queue_store_non_critical_init,
    .deinit = queue_store_non_critical_deinit,
    .write = queue_store_non_critical_write,
    .read = queue_store_non_critical_read,
    .release = queue_store_non_critical_release,
};

static diag_data_store_driver_t g_queue_critical_driver = {
    .init = queue_store_critical_init,
    .deinit = queue_store_critical_deinit,
    .write = queue_store_critical_write,
    .read = queue_store_critical_read,
    .release = queue_store_critical_release,
};

// 获取非关键队列驱动
diag_data_store_driver_t* diag_data_store_queue_get_non_critical_driver(void)
{
    return &g_queue_non_critical_driver;
}

// 获取关键队列驱动
diag_data_store_driver_t* diag_data_store_queue_get_critical_driver(void)
{
    return &g_queue_critical_driver;
}