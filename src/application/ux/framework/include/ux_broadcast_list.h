/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved. \n
 *
 * Description: Provide ux broadcast list \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2026-03-05， Create file. \n
 */

#ifndef UX_BROADCAST_LIST_H
#define UX_BROADCAST_LIST_H

#include "errcode.h"
#include "ux_api_broadcast.h"

#ifdef __cplusplus
extern "C" {
#endif

// 消费者节点结构
typedef struct ux_broadcast_consumer_t {
    uint32_t handle;  // 消费者句柄
    ux_api_broadcast_event_handler_t sync_event_handler;  // 同步事件处理函数
    ux_api_broadcast_subscribe_list_t subscribe_list;  // 订阅列表
    unsigned long queue_id;  // 队列ID
    struct ux_broadcast_consumer_t *next;  // 指向下一个消费者的指针
} ux_broadcast_consumer_t;

// 链表结构
typedef struct {
    ux_broadcast_consumer_t *head;  // 链表头节点
    uint32_t count;  // 链表中消费者的数量
} ux_broadcast_list_t;

// 初始化广播列表
errcode_t ux_broadcast_list_init(ux_broadcast_list_t *list);

// 反初始化广播列表，释放资源
errcode_t ux_broadcast_list_deinit(ux_broadcast_list_t *list);

// 向广播列表中添加消费者
errcode_t ux_broadcast_list_add_consumer(ux_broadcast_list_t *list, ux_broadcast_consumer_t **consumer);

// 根据句柄从广播列表中移除消费者
errcode_t ux_broadcast_list_remove_consumer(ux_broadcast_list_t *list, uint32_t handle);

// 根据句柄查找消费者，返回消费者节点
ux_broadcast_consumer_t* ux_broadcast_list_find_consumer(ux_broadcast_list_t *list, uint32_t handle);

#ifdef __cplusplus
}
#endif

#endif // UX_BROADCAST_LIST_H
