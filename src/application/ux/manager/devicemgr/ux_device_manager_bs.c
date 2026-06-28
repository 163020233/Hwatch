/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 *
 * Description: Provide device manager bs service \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-07-23， Create file. \n
 */
#include <securec.h>
#include <stdio.h>
#include <string.h>
#include "common_def.h"
#include "key_id.h"
#include "property.h"
#include "osal_mutex.h"
#include "ux_common_log.h"
#include "adapter_bt_br.h"
#include "ux_errcode.h"
#include "ux_hal_memory.h"
#include "ux_device_manager.h"

static ux_bg_device_list_t g_ux_bg_paired_device_list = {0};
static ux_bg_device_list_t g_ux_bg_scan_device_list = {0};
static uint16_t g_scan_device_count = 0;

void ux_bg_set_scan_device_count(uint16_t count)
{
    g_scan_device_count = count;
}

uint16_t ux_bg_get_scan_device_count(void)
{
    return g_scan_device_count;
}

ux_bg_device_list_t* ux_bg_get_scan_device_list(void)
{
    return &g_ux_bg_scan_device_list;
}

ux_bg_device_list_t* ux_bg_get_paired_device_list(void)
{
    return &g_ux_bg_paired_device_list;
}

ux_bg_device_info_t *ux_bg_append_blank_device_info(ux_bg_device_list_t *device_list)
{
    /* 使用此函数先在链表插入空结点, 避免直接在栈上构造结构体, 导致爆栈 */
    if (device_list == NULL) {
        ux_log_err("[ux_bg_append_blank_device_info] nullptr.\n");
        return NULL;
    }
    ux_bg_device_info_t *new_node = ux_hal_memory_acquire(UX_MEM_BG, sizeof(ux_bg_device_info_t));
    if (new_node == NULL) {
        return NULL;
    }
    new_node->next = NULL;
    if (device_list->head == NULL) {
        device_list->head = new_node;
    } else {
        ux_bg_device_info_t *cur_node = device_list->head;
        while (cur_node->next != NULL) {
            cur_node = (ux_bg_device_info_t*)cur_node->next;
        }
        cur_node->next = (ux_bg_device_info_t*)new_node;
    }
    device_list->device_num++;
    return new_node;
}

/* 查找设备信息 */
ux_bg_device_info_t *ux_bg_find_device_info_by_addr(ux_bg_device_list_t *device_list,
                                                    const ux_api_bt_addr_t *addr, ux_api_bt_protocol_t protocol)
{
    if (device_list == NULL || addr == NULL) {
        ux_log_err("[ux_bg_find_device_info_by_addr] nullptr.\n");
        return NULL;
    }
    ux_bg_device_info_t *cur_device_info = device_list->head;
    while (cur_device_info != NULL) {
        if (memcmp(addr, &cur_device_info->addr, sizeof(ux_api_bt_addr_t)) == 0 &&
            protocol == cur_device_info->protocol) {
            break;
        }
        cur_device_info = (ux_bg_device_info_t *)cur_device_info->next;
    }
    if (cur_device_info == NULL) {
        ux_log_err("[ux_bg_find_device_info_by_addr] cannot find ****%02x%02x%02x%02x, type = %d, protocol = %d.\n",
            addr->addr[BG_ADDR_INDEX_3], addr->addr[BG_ADDR_INDEX_2],
            addr->addr[BG_ADDR_INDEX_1], addr->addr[BG_ADDR_INDEX_0], addr->type, protocol);
    }
    return cur_device_info;
}

ux_bg_device_info_t *ux_bg_find_device_info_by_id(ux_bg_device_list_t *device_list, uint16_t id)
{
    if (device_list == NULL) {
        ux_log_err("[ux_bg_find_device_info_by_id] nullptr.\n");
        return NULL;
    }
    ux_bg_device_info_t *cur_device_info = device_list->head;
    while (cur_device_info != NULL) {
        if (cur_device_info->id == id) {
            break;
        }
        cur_device_info = (ux_bg_device_info_t *)cur_device_info->next;
    }
    if (cur_device_info == NULL) {
        ux_log_err("[ux_bg_find_device_info_by_id] cannot find id=%d.\n", id);
    }
    return cur_device_info;
}

void ux_bg_delete_device_info_by_id(ux_bg_device_list_t *device_list, uint16_t device_id)
{
    if (device_list == NULL) {
        ux_log_err("[ux_bg_delete_device_info_by_id] nullptr.\n");
        return;
    }
    ux_bg_device_info_t dummy = { 0 };
    dummy.next = (ux_bg_device_info_t*)device_list->head;
    ux_bg_device_info_t *last_device_info = &dummy;
    ux_bg_device_info_t *cur_device_info = (ux_bg_device_info_t *)dummy.next;

    while (cur_device_info != NULL) {
        if (cur_device_info->id == device_id) {
            last_device_info->next = cur_device_info->next;
            ux_hal_memory_release(UX_MEM_BG, cur_device_info);
            cur_device_info = (ux_bg_device_info_t *)last_device_info->next;
            device_list->device_num--;
            continue;
        }
        last_device_info = cur_device_info;
        cur_device_info = (ux_bg_device_info_t *)cur_device_info->next;
    }
    device_list->head = (ux_bg_device_info_t*)dummy.next;
    return;
}

void ux_bg_delete_device_info_by_addr(ux_bg_device_list_t *device_list, const ux_api_bt_addr_t *addr,
    ux_api_bt_protocol_t protocol)
{
    if (device_list == NULL || addr == NULL) {
        ux_log_err("[ux_bg_delete_device_info_by_addr] nullptr.\n");
        return;
    }
    ux_bg_device_info_t dummy = { 0 };
    dummy.next = (ux_bg_device_info_t*)device_list->head;
    ux_bg_device_info_t *last_device_info = &dummy;
    ux_bg_device_info_t *cur_device_info = (ux_bg_device_info_t *)dummy.next;

    while (cur_device_info != NULL) {
        if (memcmp(addr, &cur_device_info->addr, sizeof(ux_api_bt_addr_t)) == 0 &&
            cur_device_info->protocol == protocol) {
            last_device_info->next = cur_device_info->next;
            ux_hal_memory_release(UX_MEM_BG, cur_device_info);
            cur_device_info = (ux_bg_device_info_t *)last_device_info->next;
            device_list->device_num--;
            continue;
        }
        last_device_info = cur_device_info;
        cur_device_info = (ux_bg_device_info_t *)cur_device_info->next;
    }
    device_list->head = (ux_bg_device_info_t *)dummy.next;
    return;
}
ux_bg_device_info_t *ux_bg_get_device_info(ux_bg_device_list_t *device_list,
    const ux_bg_device_operate_rule is_target_device, const uint8_t *rule_param, uint16_t len)
{
    if (device_list == NULL || is_target_device == NULL || (len != 0 && rule_param == NULL)) {
        ux_log_err("[ux_bg_get_device_info] nullptr.\n");
        return NULL;
    }
    ux_bg_device_info_t *cur_device_info = device_list->head;
    while (cur_device_info != NULL) {
        if (is_target_device(cur_device_info, rule_param, len)) {
            return  cur_device_info;
        }
        cur_device_info = (ux_bg_device_info_t *)cur_device_info->next;
    }
    return NULL;
}

void ux_bg_clear_device_list(ux_bg_device_list_t *device_list)
{
    if (device_list == NULL) {
        ux_log_err("[ux_bg_clear_device_list] nullptr.\n");
        return;
    }
    ux_bg_device_info_t *cur_device = device_list->head;
    ux_bg_device_info_t *next_device = NULL;
    while (cur_device != NULL) {
        next_device = (ux_bg_device_info_t *)cur_device->next;
        ux_hal_memory_release(UX_MEM_BG, cur_device);
        cur_device = next_device;
        device_list->device_num--;
    }

    // 清空链表后，将链表头设为 NULL
    device_list->head = NULL;
}