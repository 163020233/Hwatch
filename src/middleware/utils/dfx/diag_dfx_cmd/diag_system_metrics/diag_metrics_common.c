/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: diag metrics common
 * This file should be changed only infrequently and with great care.
 */

#include <string.h>
#include "los_tick.h"
#include "dfx_adapt_layer.h"
#include "diag_report_control.h"
#include "diag_data_store.h"
#include "diag_metrics_common.h"

#ifdef CONFIG_NV_FEATURE_SUPPORT
#include "nv.h"
#endif

// --- 配置 ---
#define DIAG_METRICS_TASK_STACK_SIZE    0x1000  // 定义任务堆栈大小为0x1000
#define DIAG_METRICS_TASK_PRIORITY      20      // 定义任务优先级为20
#define DIAG_METRICS_MAX_COUNT          32      // 定义最大注册指标数为32
#define DIAG_METRICS_DEINIT_DELAY       100     // 定义去初始化延时100ms
#define DIAG_METRICS_TASK_DELAY_MIN     100     // 定义任务默认延时100ms
#define DIAG_METRICS_TASK_LOOP_INTERVAL 1000    // 定义任务循环间隔1000ms

// 注册指标的私有结构体
typedef struct diag_metrics_node {
    uint32_t data_len;                      // 最大数据长度
    uint32_t interval;                      // 收集间隔，单位为秒
    uint16_t cmd_id;                        // 命令ID
    struct diag_metrics_node *next;         // 链表中的下一个节点
    void *ctx;                              // 上下文指针，用于存储额外的数据
    diag_metrics_getter_t getter;           // 获取函数指针，用于获取诊断数据
} diag_metrics_node_t;

// --- 全局状态 ---
static diag_metrics_config_t g_config;                                  // 全局配置
static diag_metrics_node_t *g_metrics_list_head = NULL;                 // 指标链表头
static metrics_run_state_t g_metrics_state = METRICS_STATE_NOT_INIT;    // 运行状态
static bool g_task_finish = false;                                      // 任务完成状态
static osal_mutex g_metrics_mutex;                                      // 用于保护指标链表的互斥锁
static uint16_t g_metrics_cnt = 0;                                      // 注册metrics数量
osal_task *g_collector_task = NULL;                                     // Metrics收集任务

static void metrics_collector_task(uint32_t arg); // 声明指标收集器任务函数

// 注册诊断指标用于周期性上报
errcode_t uapi_diag_metrics_period_report(uint16_t cmd_id, diag_metrics_getter_t getter, uint32_t interval,
                                          uint32_t data_len, void *ctx)
{
#if defined(CONFIG_DFX_SUPPORT_METRICS_REPORT) && (CONFIG_DFX_SUPPORT_METRICS_REPORT == DFX_YES)
    if (g_metrics_state == METRICS_STATE_NOT_INIT) {
        return ERRCODE_FAIL;
    }
    // 检查参数有效性
    if (getter == NULL || data_len == 0) {
        return ERRCODE_INVALID_PARAM;
    }

    // 创建新的节点
    diag_metrics_node_t *new_node = (diag_metrics_node_t *)dfx_malloc(0, sizeof(diag_metrics_node_t));
    if (new_node == NULL) {
        return ERRCODE_MALLOC;
    }

    // 初始化新节点
    new_node->cmd_id = cmd_id;
    new_node->getter = getter;
    new_node->ctx = ctx;
    new_node->data_len = data_len;
    new_node->interval = interval;

    // 加锁，若失败则释放节点并返回错误
    if (osal_mutex_lock(&g_metrics_mutex) != OSAL_SUCCESS) {
        dfx_free(0, new_node);
        return ERRCODE_FAIL;
    }
    new_node->next = g_metrics_list_head;
    g_metrics_list_head = new_node;
    (void)osal_mutex_unlock(&g_metrics_mutex);
#endif
    return ERRCODE_SUCC;
}

// 查找诊断指标节点（内部函数）
static diag_metrics_node_t* find_metrics_node(uint16_t cmd_id, diag_metrics_node_t** prev_out)
{
    diag_metrics_node_t *prev = NULL;
    diag_metrics_node_t *current = g_metrics_list_head;
    
    while (current != NULL) {
        if (current->cmd_id == cmd_id) {
            if (prev_out != NULL) {
                *prev_out = prev;
            }
            return current;
        }
        prev = current;
        current = current->next;
    }
    
    return NULL;
}

// 注销已注册的诊断指标
errcode_t uapi_diag_metrics_stop_report(uint16_t cmd_id)
{
#if defined(CONFIG_DFX_SUPPORT_METRICS_REPORT) && (CONFIG_DFX_SUPPORT_METRICS_REPORT == DFX_YES)
    if (g_metrics_state == METRICS_STATE_NOT_INIT) {
        return ERRCODE_FAIL;
    }
    (void)osal_mutex_lock(&g_metrics_mutex);
    // 获取诊断指标节点及其前驱节点
    diag_metrics_node_t *prev = NULL;
    diag_metrics_node_t *node = find_metrics_node(cmd_id, &prev);
    
    // 如果找到节点，则将其从链表中删除
    if (node != NULL) {
        if (prev == NULL) {
            // 删除的是头节点
            g_metrics_list_head = node->next;
        } else {
            prev->next = node->next;
        }
        dfx_free(0, node);
        g_metrics_cnt -= 1;
    }
    
    (void)osal_mutex_unlock(&g_metrics_mutex);
    
    return node != NULL ? ERRCODE_SUCC : ERRCODE_FAIL;
#else
    return ERRCODE_SUCC;
#endif
}

// 立即报告诊断指标
errcode_t uapi_diag_metrics_report_once(uint16_t cmd_id, const void *data, uint32_t len)
{
#if defined(CONFIG_DFX_SUPPORT_METRICS_REPORT) && (CONFIG_DFX_SUPPORT_METRICS_REPORT == DFX_YES)
    if (g_metrics_state == METRICS_STATE_NOT_INIT) {
        return ERRCODE_FAIL;
    }
    // 检查参数有效性
    if (cmd_id == 0 || data == NULL) {
        return ERRCODE_INVALID_PARAM;
    }
    // 直接调用写回调函数，无延迟
    return g_config.write_cb(cmd_id, data, len, g_config.cb_arg);
#else
    return ERRCODE_SUCC;
#endif
}

// 更新诊断指标报告间隔
errcode_t uapi_diag_metrics_update_interval(uint16_t cmd_id, uint32_t new_interval)
{
#if defined(CONFIG_DFX_SUPPORT_METRICS_REPORT) && (CONFIG_DFX_SUPPORT_METRICS_REPORT == DFX_YES)
    if (g_metrics_state == METRICS_STATE_NOT_INIT) {
        return ERRCODE_FAIL;
    }
    (void)osal_mutex_lock(&g_metrics_mutex);
    // 查找诊断指标节点
    diag_metrics_node_t *node = find_metrics_node(cmd_id, NULL);
    
    if (node != NULL) {
        // 找到节点，更新间隔时间
        node->interval = new_interval;
        (void)osal_mutex_unlock(&g_metrics_mutex);
        return ERRCODE_SUCC;
    } else {
        // 未找到对应的诊断指标
        (void)osal_mutex_unlock(&g_metrics_mutex);
        return ERRCODE_FAIL;
    }
#else
    return ERRCODE_SUCC;
#endif
}

// 调用metrics回调链表节点getter，采集metrics数据
static void process_metrics_node(diag_metrics_node_t *node, uint32_t cnt, void **data_buffer, uint32_t *max_buffer_size)
{
    // 如果节点的间隔时间大于0且计数器cnt是间隔时间的整数倍
    if (node->interval > 0 && cnt % (node->interval) == 0) {
        if (node->data_len > *max_buffer_size) {
            // 先尝试分配新缓冲区，成功后再释放旧缓冲区
            void *new_buffer = dfx_malloc(0, node->data_len);
            if (new_buffer == NULL) {
                // 分配失败，保留原有缓冲区，直接返回
                return;
            }
            // 释放旧缓冲区
            if (*data_buffer) {
                dfx_free(0, *data_buffer);
            }
            *data_buffer = new_buffer;
            *max_buffer_size = node->data_len;
        }
        
        uint32_t data_len = node->data_len;
        // 调用getter来收集数据
        errcode_t getter_ret = node->getter(*data_buffer, &data_len, node->ctx);
        if (getter_ret == ERRCODE_SUCC && data_len > 0 && data_len <= node->data_len) {
            // 使用回调报告数据
            g_config.write_cb(node->cmd_id, *data_buffer, data_len, g_config.cb_arg);
        }
    }
}

// metrics收集器任务函数，遍历metrics回调链表上报
static void metrics_collector_task(uint32_t arg)
{
    (void)arg;
    void *data_buffer = NULL;
    uint32_t max_buffer_size = 0;
    uint32_t cnt = 0;
    uint64_t begin = 0;
    uint64_t next_wakeup = 0;
    uint32_t sleep = 0;
    g_task_finish = false;

    while (g_metrics_state == METRICS_STATE_RUNNING) {
        begin = (cnt == 0) ? osal_get_jiffies() : next_wakeup;
        (void)osal_mutex_lock(&g_metrics_mutex);

        // 遍历所有已注册的度量指标
        for (diag_metrics_node_t *node = g_metrics_list_head; node != NULL; node = node->next) {
            process_metrics_node(node, cnt, &data_buffer, &max_buffer_size);
        }

        (void)osal_mutex_unlock(&g_metrics_mutex);
        
        // 计算休眠时间
        next_wakeup = begin + DIAG_METRICS_TASK_LOOP_INTERVAL;
        sleep = (uint32_t)(next_wakeup - osal_get_jiffies());
        if (sleep < DIAG_METRICS_TASK_DELAY_MIN || sleep > DIAG_METRICS_TASK_LOOP_INTERVAL) {
            sleep = DIAG_METRICS_TASK_DELAY_MIN;
        }
        dfx_sleep(sleep);
        cnt++;
    }

    g_task_finish = true;

    if (data_buffer) {
        dfx_free(0, data_buffer);
    }
}

// 回调：metrics上报，写入diag存储
static errcode_t metrics_write_cb(uint16_t cmd_id, void *data, uint32_t len, void *cb_arg)
{
    unused(cb_arg);
    diag_data_store_ctx_t store_ctx = {
        .idx = 1,
        .enable_timestamp = true,
    };
#if defined(CONFIG_DFX_SUPPORT_METRICS_REPORT) && (CONFIG_DFX_SUPPORT_METRICS_REPORT == DFX_YES)
    // metrics写入非关键数据存储
    return diag_data_store_non_critical_write(cmd_id, data, len, &store_ctx);
#endif
    return ERRCODE_SUCC;
}

// 初始化metrics资源
errcode_t diag_metrics_init(void)
{
    if (g_metrics_state != METRICS_STATE_NOT_INIT) {
        return ERRCODE_FAIL;
    }
    diag_metrics_config_t metrics_config = {
        .write_cb = (diag_metrics_write_cb_t)metrics_write_cb,
        .cb_arg = NULL,
    };

    // 1. 保存配置
    g_config = metrics_config;

    // 2. 创建互斥锁
    errcode_t ret = (errcode_t)osal_mutex_init(&g_metrics_mutex);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    g_metrics_state = METRICS_STATE_READY_DISABLED;
    return ERRCODE_SUCC;
}

// 启动metrics任务
errcode_t diag_metrics_enable(void)
{
    errcode_t ret;

    (void)osal_mutex_lock(&g_metrics_mutex);

    if (g_metrics_state == METRICS_STATE_NOT_INIT) {
        ret = diag_metrics_init();
        if (ret != ERRCODE_SUCC) {
            (void)osal_mutex_unlock(&g_metrics_mutex);
            return ret;
        }
    }

    if (g_metrics_state == METRICS_STATE_RUNNING) {
        (void)osal_mutex_unlock(&g_metrics_mutex);
        return ERRCODE_FAIL;
    }

    // 创建并启动收集器任务
    g_metrics_state = METRICS_STATE_RUNNING;
    (void)osal_mutex_unlock(&g_metrics_mutex);  // 创建任务前解锁，避免长时间持锁

    g_collector_task = osal_kthread_create((osal_kthread_handler)metrics_collector_task, NULL,
                                           "DiagMetrics", DIAG_METRICS_TASK_STACK_SIZE);
    if (g_collector_task == NULL) {
        (void)osal_mutex_lock(&g_metrics_mutex);
        g_metrics_state = METRICS_STATE_READY_DISABLED;
        (void)osal_mutex_unlock(&g_metrics_mutex);
        return ERRCODE_FAIL;
    }

    // 设置任务优先级
    osal_kthread_set_priority(g_collector_task, DIAG_METRICS_TASK_PRIORITY);
    return ERRCODE_SUCC;
}

// 删除metrics任务
errcode_t diag_metrics_disable(void)
{
    // 检查参数是否有效
    if (g_metrics_state != METRICS_STATE_RUNNING) {
        return ERRCODE_FAIL;
    }

    // 退出任务
    g_metrics_state = METRICS_STATE_READY_DISABLED;
    while (g_task_finish == false) {
        dfx_sleep(DIAG_METRICS_DEINIT_DELAY);
    }
    osal_kthread_destroy(g_collector_task, 1);
    g_collector_task = NULL;
    return ERRCODE_SUCC;
}

// 诊断指标去初始化函数
// 此函数用于去初始化诊断指标，释放所有已注册的指标和锁资源
errcode_t diag_metrics_deinit(void)
{
    // 如果未运行，则直接返回
    if (g_metrics_state == METRICS_STATE_NOT_INIT) {
        return ERRCODE_FAIL;
    }

    if (g_metrics_state == METRICS_STATE_RUNNING) {
        (void)diag_metrics_disable();
    }
    // 释放所有已注册的指标
    (void)osal_mutex_lock(&g_metrics_mutex);
    diag_metrics_node_t *current = g_metrics_list_head;
    while (current != NULL) {
        diag_metrics_node_t *to_delete = current;
        current = current->next;
        dfx_free(0, to_delete);
    }
    g_metrics_list_head = NULL;
    (void)osal_mutex_unlock(&g_metrics_mutex);
    osal_mutex_destroy(&g_metrics_mutex);
    g_metrics_state = METRICS_STATE_NOT_INIT;
    return ERRCODE_SUCC;
}

#if defined(CONFIG_DFX_SUPPORT_METRICS_REPORT) && (CONFIG_DFX_SUPPORT_METRICS_REPORT == DFX_YES)
errcode_t diag_cmd_set_metrics(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option)
{
    // 检查cmd_param是否为正确的类型
    if (cmd_param == NULL || cmd_param_size != sizeof(diag_metrics_interval_t)) {
        return ERRCODE_INVALID_PARAM;
    }

    diag_metrics_interval_t interval_t = *(diag_metrics_interval_t *)cmd_param;

    switch (interval_t.cmd_id) {
        case METRICS_CTRL_OP_QUERY: {
            diag_metrics_interval_t temp_interval;
            interval_t.interval = g_metrics_state;
            uapi_diag_report_packet(cmd_id, option, (uint8_t *)&interval_t, sizeof(diag_metrics_interval_t), true);
            if (g_metrics_state != METRICS_STATE_RUNNING) {
                return ERRCODE_SUCC;
            }
            (void)osal_mutex_lock(&g_metrics_mutex);
            for (diag_metrics_node_t *node = g_metrics_list_head; node != NULL; node = node->next) {
                temp_interval.cmd_id = node->cmd_id;
                temp_interval.interval = node->interval;
                uapi_diag_report_packet(cmd_id, option, (uint8_t *)&temp_interval, sizeof(diag_metrics_interval_t), true);
            }
            (void)osal_mutex_unlock(&g_metrics_mutex);
            return ERRCODE_SUCC;
        }
        case METRICS_CTRL_OP_INIT: {
            diag_metrics_enable();
#ifdef CONFIG_NV_FEATURE_SUPPORT
            uint8_t flag = 1;
            uapi_nv_write(NV_ID_HCD_METRICS_ENABLE_FLAG, &flag, sizeof(uint8_t));
#endif
            return uapi_diag_report_packet(cmd_id, option, (uint8_t *)&interval_t, sizeof(diag_metrics_interval_t), true);
        }
        case METRICS_CTRL_OP_DEINIT: {
            diag_metrics_disable();
#ifdef CONFIG_NV_FEATURE_SUPPORT
            uint8_t flag = 0;
            uapi_nv_write(NV_ID_HCD_METRICS_ENABLE_FLAG, &flag, sizeof(uint8_t));
#endif
            return uapi_diag_report_packet(cmd_id, option, (uint8_t *)&interval_t, sizeof(diag_metrics_interval_t), true);
        }
        default:
            // 其余cmd_id表示metrics标示，更新指定cmd_id的上报interval
            if (uapi_diag_metrics_update_interval(interval_t.cmd_id, interval_t.interval) != ERRCODE_SUCC) {
                interval_t.cmd_id = 0;
                interval_t.interval = 0;
            }
            return uapi_diag_report_packet(cmd_id, option, (uint8_t *)&interval_t, sizeof(diag_metrics_interval_t), true);
    }
}
#endif