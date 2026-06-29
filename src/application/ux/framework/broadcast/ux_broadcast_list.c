/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved. \n
 *
 * Description: Provide ux broadcast list \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2026-03-05， Create file. \n
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "osal_msgqueue.h"
#include "securec.h"
#include "ux_errcode.h"
#include "ux_hal_memory.h"
#include "ux_common_log.h"
#include "ux_broadcast_list.h"

#define MSG_QUEUE_SIZE_MAX 10
#define MSG_QUEUE_NAME_SIZE_MAX 20

static errcode_t broadcast_queue_init(unsigned long* queue_id)
{
    char queue_name[MSG_QUEUE_NAME_SIZE_MAX];
    static uint16_t queue_number = 0;

    int32_t ret = snprintf_s(queue_name, sizeof(queue_name), sizeof(queue_name), "queue_number_%u", queue_number++);
    if (ret < 0) {
        ux_log_err("[ux][broadcast]snprintf_s fail\n");
        return UX_ERR_CREATE_MSG_QUEUE_FAILED;
    }
    ret = osal_msg_queue_create(queue_name, MSG_QUEUE_SIZE_MAX, queue_id, 0, sizeof(ux_api_broadcast_msg_t));
    if (ret != 0) {
        ux_log_err("[ux][broadcast]msg queue create fail\n");
        return UX_ERR_CREATE_MSG_QUEUE_FAILED;
    }

    return UX_ERR_SUCCESS;
}

static void broadcast_queue_deinit(unsigned long* queue_id)
{
    osal_msg_queue_delete(*queue_id);
}

// 初始化链表
errcode_t ux_broadcast_list_init(ux_broadcast_list_t *list)
{
    if (list == NULL) {
        ux_log_err("[ux][broadcast]Invalid list parameter");
        return ERRCODE_FAIL;
    }

    list->head = NULL;
    list->count = 0;
    
    ux_log_info("[ux][broadcast]List initialized");
    return UX_ERR_SUCCESS;
}

// 反初始化链表
errcode_t ux_broadcast_list_deinit(ux_broadcast_list_t *list)
{
    if (!list) {
        ux_log_err("[ux][broadcast]Invalid list parameter");
        return ERRCODE_FAIL;
    }

    // 清理所有节点
    ux_broadcast_consumer_t *current = list->head;
    while (current) {
        ux_broadcast_consumer_t *next = current->next;
        broadcast_queue_deinit(&current->queue_id);
        ux_hal_memory_release(UX_MEM_BROADCAST, current);
        current = next;
    }
    
    list->head = NULL;
    list->count = 0;
    
    ux_log_info("[ux][broadcast]List deinitialized");
    return UX_ERR_SUCCESS;
}

// 添加消费者到链表
errcode_t ux_broadcast_list_add_consumer(ux_broadcast_list_t *list, ux_broadcast_consumer_t **consumer)
{
    if (list == NULL) {
        ux_log_err("[ux][broadcast]Invalid parameters for add consumer");
        return ERRCODE_FAIL;
    }

    // 创建消费者节点
    ux_broadcast_consumer_t *new_consumer = (ux_broadcast_consumer_t*)ux_hal_memory_acquire(UX_MEM_BROADCAST,
        sizeof(ux_broadcast_consumer_t));
    if (new_consumer == NULL) {
        ux_log_err("[ux][broadcast]Failed to allocate memory for consumer");
        return UX_ERR_NULL_PTR;
    }
    errno_t err = memset_s(new_consumer, sizeof(ux_broadcast_consumer_t), 0, sizeof(ux_broadcast_consumer_t));
    if (err != EOK) {
        ux_hal_memory_release(UX_MEM_BROADCAST, new_consumer);
        ux_log_err("[ux][broadcast]Failed to memset_s memory for consumer");
        return UX_ERR_MEMSET_FAILED;
    }

    errcode_t ret = broadcast_queue_init(&new_consumer->queue_id);
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[ux][broadcast]Failed to initialize consumer queue ret:0x%x", ret);
        ux_hal_memory_release(UX_MEM_BROADCAST, new_consumer);
        return ret;
    }
    // 添加到链表头部
    new_consumer->next = list->head;
    list->head = new_consumer;
    list->count++;
    *consumer = new_consumer;
    
    ux_log_info("[ux][broadcast]Consumer added to list, total count: %u", list->count);
    return UX_ERR_SUCCESS;
}

// 从链表中移除消费者
errcode_t ux_broadcast_list_remove_consumer(ux_broadcast_list_t *list, uint32_t handle)
{
    if (list == NULL) {
        ux_log_err("[ux][broadcast]Invalid list parameter");
        return UX_ERR_NULL_PTR;
    }

    // 查找要删除的节点
    ux_broadcast_consumer_t **current = &list->head;
    while (*current) {
        if ((*current)->handle == handle) {
            ux_broadcast_consumer_t *node_to_remove = *current;
            *current = (*current)->next;
            // 清理队列
            broadcast_queue_deinit(&node_to_remove->queue_id);
            ux_hal_memory_release(UX_MEM_BROADCAST, node_to_remove);
            list->count--;
            ux_log_info("[ux][broadcast]Consumer removed from list, total count: %u", list->count);
            return UX_ERR_SUCCESS;
        }
        current = &((*current)->next);
    }

    ux_log_warn("[ux][broadcast]Consumer with handle %u not found in list", handle);
    return ERRCODE_FAIL;
}

// 查找消费者
ux_broadcast_consumer_t* ux_broadcast_list_find_consumer(ux_broadcast_list_t *list, uint32_t handle)
{
    if (list == NULL) {
        ux_log_err("[ux][broadcast]Invalid list parameter");
        return NULL;
    }

    ux_broadcast_consumer_t *current = list->head;
    while (current) {
        if (current->handle == handle) {
            return current;
        }
        current = current->next;
    }

    return NULL;
}