/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved. \n
 *
 * Description: Provide ux broadcast customer \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2026-03-05， Create file. \n
 */

#include <stdlib.h>
#include <string.h>
#include "stdio.h"
#include "securec.h"
#ifndef UT_TEST
#include "los_sys.h"
#endif
#include "osal_msgqueue.h"
#include "osal_adapt.h"
#include "ux_errcode.h"
#include "osal_wait.h"
#include "ux_hal_memory.h"
#include "los_typedef.h"
#include "ux_common_log.h"
#include "ux_broadcast_list.h"
#include "ux_api_broadcast.h"
#include "ux_broadcast.h"

// 全局消费者链表头指针
static ux_broadcast_list_t g_consumer_list;
static uint32_t g_next_handle = 1;
static bool g_is_initialized = false;

// 检查消费者是否订阅了指定事件
static bool is_consumer_subscribed(ux_broadcast_consumer_t *consumer, ux_api_broadcast_event_type_t event)
{
    // 如果没有订阅任何事件，表示订阅所有事件
    if (consumer->subscribe_list.count == 0) {
        return true;
    }

    // 检查是否订阅了指定事件
    for (uint32_t i = 0; i < consumer->subscribe_list.count; i++) {
        if (consumer->subscribe_list.subscribe[i].type == event) {
            return true;
        }
    }
    
    return false;
}

// 获取事件处理策略（同步/异步）
static bool get_event_sync_strategy(ux_broadcast_consumer_t *consumer, ux_api_broadcast_msg_t* msg)
{
    // 如果没有订阅任何事件，返回默认异步处理
    if (consumer->subscribe_list.count == 0) {
        return false; // 默认异步
    }

    // 查找该事件的同步/异步设置
    for (uint32_t i = 0; i < consumer->subscribe_list.count; i++) {
        if (consumer->subscribe_list.subscribe[i].type == msg->type) {
            return consumer->subscribe_list.subscribe[i].is_sync;
        }
    }
    
    return false; // 默认异步
}

static errcode_t memcpy_broadcast_msg(ux_api_broadcast_msg_t* dst_msg, ux_api_broadcast_msg_t* src_msg)
{
    dst_msg->need_free_data = src_msg->need_free_data;
    dst_msg->type = src_msg->type;
    dst_msg->data_len = src_msg->data_len;
    dst_msg->data = NULL;
    // 异步场景下，data不为空就需要申请内存，避免使用栈内存，导致使用释放后内存
    if (src_msg->data != NULL) {
        dst_msg->data = ux_hal_memory_acquire(UX_MEM_BROADCAST, src_msg->data_len);
        if (dst_msg->data == NULL) {
            ux_log_err("[ux][broadcast]Failed to allocate memory for data");
            return UX_ERR_NULL_PTR;
        }

        errno_t err = memcpy_s(dst_msg->data, dst_msg->data_len, src_msg->data, src_msg->data_len);
        if (err != EOK) {
            ux_hal_memory_release(UX_MEM_BROADCAST, dst_msg->data);
            ux_log_err("[ux][broadcast]Failed to memcpy_s memory for data");
            return UX_ERR_MEMCPY_FAILED;
        }
        dst_msg->need_free_data = true;
    }

    return UX_ERR_SUCCESS;
}

// 同步处理事件
static errcode_t sync_handle_event(ux_broadcast_consumer_t *consumer, ux_api_broadcast_msg_t* msg)
{
    ux_api_broadcast_msg_t temp_msg;
    errcode_t ret;
    // 同步接口不需要重复申请内存，统一使用原始内存，Broadcast之后统一释放；减少内存占用，提高效率
    temp_msg.need_free_data = false;
    temp_msg.type = msg->type;
    temp_msg.data_len = msg->data_len;
    temp_msg.data = msg->data;
    ret = consumer->sync_event_handler(&temp_msg);
    if (ret != UX_ERR_SUCCESS) {
        ux_log_warn("[ux][broadcast]Sync handler for handle %d returned error %d", consumer->handle, ret);
    }

    return ret;
}

// 异步处理事件（入队）
static errcode_t async_handle_event(ux_broadcast_consumer_t *consumer, ux_api_broadcast_msg_t* msg)
{
    ux_api_broadcast_msg_t temp_msg;
    errcode_t ret;

    ret = memcpy_broadcast_msg(&temp_msg, msg);
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[ux][broadcast]memcpy_broadcast_msg error 0x%x", ret);
        return ret;
    }
    int32_t err = osal_msg_queue_write_copy(consumer->queue_id, (void *)&temp_msg, sizeof(ux_api_broadcast_msg_t), 0);
    if (err != 0) {
        if (temp_msg.need_free_data) {
            ux_hal_memory_release(UX_MEM_BROADCAST, temp_msg.data);
        }
        ux_log_warn("[ux][broadcast]Failed to enqueue event:%d for handle:%d", msg->type, consumer->handle);
        return ERRCODE_FAIL;
    }
    return UX_ERR_SUCCESS;
}

errcode_t ux_api_broadcast_init(void)
{
    if (g_is_initialized) {
        ux_log_warn("[ux][broadcast]Broadcast system already initialized");
        return UX_ERR_SUCCESS;
    }

    errcode_t ret = ux_broadcast_list_init(&g_consumer_list);
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[ux][broadcast]Failed to initialize consumer list ret:0x%x", ret);
        return ret;
    }

    g_is_initialized = true;
    ux_log_info("[ux][broadcast]Broadcast system initialized successfully");
    return UX_ERR_SUCCESS;
}

errcode_t ux_api_broadcast_deinit(void)
{
    if (!g_is_initialized) {
        ux_log_warn("[ux][broadcast]Broadcast system not initialized");
        return UX_ERR_SUCCESS;
    }

    errcode_t ret = ux_broadcast_list_deinit(&g_consumer_list);
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[ux][broadcast]Failed to deinitialize consumer list ret:0x%x", ret);
        return ret;
    }

    g_is_initialized = false;
    ux_log_info("[ux][broadcast]Broadcast system deinitialized successfully");
    return UX_ERR_SUCCESS;
}

errcode_t ux_api_broadcast_register_consumer(ux_api_broadcast_event_handler_t sync_event_handler,
    ux_api_broadcast_subscribe_list_t* subscribe_list, uint32_t *handle)
{
    errcode_t ret;

    if (!g_is_initialized) {
        ux_log_err("[ux][broadcast]Broadcast system not initialized");
        return UX_ERR_NOT_INIT;
    }
    if (sync_event_handler == NULL || handle == NULL ||
        subscribe_list == NULL || subscribe_list->count > UX_API_BROADCAST_EVENT_TYPE_MAX) {
        ux_log_err("[ux][broadcast]Invalid parameters for consumer registration");
        return UX_ERR_NULL_PTR;
    }
    // 创建消费者节点
    ux_broadcast_consumer_t *consumer = NULL;
    ret = ux_broadcast_list_add_consumer(&g_consumer_list, &consumer);
    if (ret != UX_ERR_SUCCESS || consumer == NULL) {
        ux_log_err("[ux][broadcast]Failed to add consumer to list ret:0x%x", ret);
        return ret;
    }

    errno_t err = memcpy_s(&consumer->subscribe_list, sizeof(consumer->subscribe_list), subscribe_list,
        sizeof(ux_api_broadcast_subscribe_list_t));
    if (err != EOK) {
        ux_log_err("[ux][broadcast]Failed to memcpy_s memory for consumer");
        return UX_ERR_MEMCPY_FAILED;
    }
    
    consumer->handle = g_next_handle++;
    consumer->sync_event_handler = sync_event_handler;
    *handle = consumer->handle;

    ux_log_info("[ux][broadcast]Registered with handle %u", consumer->handle);
    return UX_ERR_SUCCESS;
}

errcode_t ux_api_broadcast_unregister_consumer(uint32_t handle)
{
    errcode_t ret;

    if (!g_is_initialized) {
        ux_log_err("[ux][broadcast]Broadcast system not initialized");
        return UX_ERR_NOT_INIT;
    }

    ux_broadcast_consumer_t *consumer = ux_broadcast_list_find_consumer(&g_consumer_list, handle);
    if (consumer == NULL) {
        ux_log_warn("[ux][broadcast]Consumer with handle %u not found", handle);
        return UX_ERR_NULL_PTR;
    }

    // 从链表中移除
    ret = ux_broadcast_list_remove_consumer(&g_consumer_list, handle);
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[ux][broadcast]Failed to remove consumer from list ret:0x%x", ret);
        return ret;
    }
    
    ux_log_info("[ux][broadcast]Consumer with handle %u unregistered", handle);
    return UX_ERR_SUCCESS;
}

errcode_t ux_api_broadcast_push_customer_msg(uint32_t handle, ux_api_broadcast_msg_t* msg)
{
    if (!g_is_initialized) {
        ux_log_err("[ux][broadcast]Broadcast system not initialized");
        return UX_ERR_NOT_INIT;
    }

    if (msg == NULL) {
        ux_log_err("[ux][broadcast]Invalid mag info");
        return UX_ERR_NULL_PTR;
    }

    errcode_t ret;
    // 遍历所有消费者
    ux_broadcast_consumer_t *customer = ux_broadcast_list_find_consumer(&g_consumer_list, handle);
    if (customer == NULL) {
        ux_log_err("[ux][broadcast]Can't find customer");
        return UX_ERR_NULL_PTR;
    }
    // 获取事件处理策略
    bool is_sync = get_event_sync_strategy(customer, msg);
    if (is_sync) {
        // 同步处理
        ret = sync_handle_event(customer, msg);
    } else {
        // 异步处理 - 入队
        ret = async_handle_event(customer, msg);
    }
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[ux][broadcast]Handle event:%d failed:0x%x", msg->type, ret);
    }
    if (msg->need_free_data) {
        ux_hal_memory_release(UX_MEM_BROADCAST, msg->data);
    }
    ux_log_info("[ux][broadcast]Event type %d broadcasted", msg->type);
    return UX_ERR_SUCCESS;
}

errcode_t ux_broadcast_broadcast_msg(ux_api_broadcast_msg_t* msg)
{
    if (!g_is_initialized) {
        ux_log_err("[ux][broadcast]Broadcast system not initialized");
        return UX_ERR_NOT_INIT;
    }

    if (msg == NULL) {
        ux_log_err("[ux][broadcast]Invalid mag info");
        return UX_ERR_NULL_PTR;
    }

    errcode_t ret;
    // 遍历所有消费者
    ux_broadcast_consumer_t *current = g_consumer_list.head;
    while (current) {
        // 检查消费者是否订阅了该事件
        if (is_consumer_subscribed(current, msg->type)) {
            // 获取事件处理策略
            bool is_sync = get_event_sync_strategy(current, msg);
            if (is_sync) {
                // 同步处理
                ret = sync_handle_event(current, msg);
            } else {
                // 异步处理 - 入队
                ret = async_handle_event(current, msg);
            }
            if (ret != UX_ERR_SUCCESS) {
                ux_log_err("[ux][broadcast]Handle event:%d failed:0x%x", msg->type, ret);
            }
        }
        
        current = current->next;
    }
    if (msg->need_free_data) {
        ux_hal_memory_release(UX_MEM_BROADCAST, msg->data);
    }
    ux_log_info("[ux][broadcast]Event type %d broadcasted", msg->type);
    return UX_ERR_SUCCESS;
}

errcode_t ux_broadcast_broadcast_event(ux_api_broadcast_event_type_t event)
{
    ux_api_broadcast_msg_t msg = {
        .type = event,
        .data = NULL,
        .data_len = 0,
        .need_free_data = false
    };
    
    return ux_broadcast_broadcast_msg(&msg);
}

errcode_t ux_api_broadcast_query_msg(uint32_t handle, ux_api_broadcast_msg_t* msg, uint32_t timeout_ms)
{
    int32_t ret;
    uint32_t msg_size = sizeof(ux_api_broadcast_msg_t);
    uint32_t tick;

    ux_broadcast_consumer_t* consumer = ux_broadcast_list_find_consumer(&g_consumer_list, handle);
    if (consumer == NULL) {
        ux_log_warn("[ux][broadcast]Consumer with handle %u not found", handle);
        return UX_ERR_NULL_PTR;
    }
    if (msg == NULL) {
        ux_log_warn("[ux][broadcast]param msg is null");
        return UX_ERR_NULL_PTR;
    }

    if (timeout_ms == UX_API_BROADCAST_WAIT_FOREVER) {
        tick = OSAL_WAIT_FOREVER;
    } else {
        tick = osal_ms2tick(timeout_ms);
    }
    ret = osal_msg_queue_read_copy(consumer->queue_id, msg, &msg_size, tick);
    if (ret != 0 || msg_size != sizeof(ux_api_broadcast_msg_t)) {
        ux_log_err("[ux][broadcast]get msg fail, ret:0x%x\n", ret);
        return UX_ERR_OSAL_READ_MSG_FAILED;
    }

    return UX_ERR_SUCCESS;
}

void ux_api_broadcast_memory_release(void *memory)
{
    ux_hal_memory_release(UX_MEM_BROADCAST, memory);
}