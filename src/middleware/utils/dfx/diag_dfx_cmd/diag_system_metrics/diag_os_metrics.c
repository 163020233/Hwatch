/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: diag os metrics
 * This file should be changed only infrequently and with great care.
 */

#include <stddef.h>
#include <sys/statfs.h>
#include "event_code.h"
#include "diag_data_mock.h"
#include "osal_adapt.h"
#include "los_cpup.h"
#include "diag_metrics_common.h"
#include "diag_metrics.h"
#include "dfx_mem.h"
#include "dfx_adapt_layer.h"
#include "soc_diag_cmd_id.h"
#include "hcd.h"

#define CPUP_CRITICAL_TH_PERCENT 80
#define MEM_CRITICAL_TH_PROPORTION 10

typedef struct {
    bool is_running;
    bool init;
} os_diag_priv_data_t;

typedef enum {
    METRICS_STATE_NORMAL = 0,
    METRICS_STATE_ABNORMAL
} metrics_state_t;

static metrics_state_t g_cpup_state = METRICS_STATE_NORMAL;
static metrics_state_t g_memory_state = METRICS_STATE_NORMAL;
static os_diag_priv_data_t g_priv_data;

// 获取内存信息
static errcode_t diag_get_mem_info(void *data_ptr, size_t *len_ptr, void *ctx)
{
    unused(ctx);
    if (!data_ptr || !len_ptr) {
        return ERRCODE_INVALID_PARAM;
    }
    if (*len_ptr < sizeof(mdm_mem_info_t)) {
        return ERRCODE_FAIL;
    }
    errcode_t ret;
    metrics_state_t new_memory_state = METRICS_STATE_NORMAL;
    ret = dfx_mem_get_sys_pool_info((mdm_mem_info_t *)data_ptr);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    uint32_t total_mem = ((mdm_mem_info_t *)data_ptr)->total;
    uint32_t free_mem = ((mdm_mem_info_t *)data_ptr)->free;
    if (free_mem < total_mem / MEM_CRITICAL_TH_PROPORTION) {
        new_memory_state = METRICS_STATE_ABNORMAL;
    } else {
        new_memory_state = METRICS_STATE_NORMAL;
    }
    // 只有状态发生变化时才上报
    if (new_memory_state != g_memory_state) {
        switch (new_memory_state) {
            case METRICS_STATE_ABNORMAL:
                uapi_diag_report_event(0, "EVTCODE_MEM_CRITICAL(0x%x)@Free memory lower than 10%%.",
                                    EVTCODE_MEM_CRITICAL);
                break;
            case METRICS_STATE_NORMAL:
                break;
        }
        g_memory_state = new_memory_state;
    }
    *len_ptr = sizeof(mdm_mem_info_t);
    return ERRCODE_SUCC;
}

// 获取cpup信息
static errcode_t diag_get_cpup_info(void *data_ptr, size_t *len_ptr, void *ctx)
{
    unused(ctx);
    metrics_state_t new_state = METRICS_STATE_NORMAL;
    if (*len_ptr < sizeof(uint8_t)) {
        return ERRCODE_FAIL;
    }
#ifdef LOSCFG_KERNEL_CPUP
    uint32_t permil = (uint32_t)osal_cpup_history_cpu_usage(CPUP_LAST_ONE_SECONDS);
    // mock data for demo
    permil = inject_cpu_anomaly(permil);
    uint8_t percent = (uint8_t)(permil / 10U);
    if (percent >= CPUP_CRITICAL_TH_PERCENT) {
        new_state = METRICS_STATE_ABNORMAL;
    } else {
        new_state = METRICS_STATE_NORMAL;
    }
    // 只有状态发生变化时才上报
    if (new_state != g_cpup_state) {
        switch (new_state) {
            case METRICS_STATE_ABNORMAL:
                uapi_diag_report_event(0, "EVTCODE_CPU_LOAD_CRITICAL(0x%x)@CPU load higher than 80%%.",
                                    EVTCODE_CPU_LOAD_CRITICAL);
                break;
            case METRICS_STATE_NORMAL:
                break;
        }
        g_cpup_state = new_state;
    }

    *(uint8_t *)data_ptr = percent;
    *len_ptr = sizeof(uint8_t);
    return ERRCODE_SUCC;
#endif
    return ERRCODE_FAIL;
}

// 获取存储信息
static errcode_t diag_get_storage_info(void *data_ptr, size_t *len_ptr, void *ctx)
{
    unused(ctx);
    if (*len_ptr < sizeof(diag_storage_info_t)) {
        return ERRCODE_FAIL;
    }

    int ret;
    static const char *mount_point[] = {"/system", "/user", "/dev"};

    memset_s(data_ptr, sizeof(diag_storage_info_t), 0, sizeof(diag_storage_info_t));
    for (size_t i = 0; i < sizeof(mount_point) / sizeof(mount_point[0]); i++) {
        struct statfs tmpStat = {0};
        ret = statfs(mount_point[i], &tmpStat);
        if (ret != 0) {
            dfx_log_err("[DIAG_REPORT]fail to get %s storage info, %s", mount_point[i], strerror(errno));
            continue;
        }

        uint32_t total_size = (uint32_t)(tmpStat.f_blocks * tmpStat.f_bsize);
        uint32_t used_size = (uint32_t)((tmpStat.f_blocks - tmpStat.f_bfree) * tmpStat.f_bsize);
        uint32_t free_size = (uint32_t)(tmpStat.f_bfree * tmpStat.f_bsize);

        ((diag_storage_info_t *)data_ptr)->total_size += total_size;
        ((diag_storage_info_t *)data_ptr)->used_size += used_size;
        ((diag_storage_info_t *)data_ptr)->free_size += free_size;
    }

    *len_ptr = sizeof(diag_storage_info_t);
    return ERRCODE_SUCC;
}

// 初始化，注册系统metrics
errcode_t diag_os_metrics_init(void)
{
    errcode_t ret;

    if (g_priv_data.init) {
        return ERRCODE_SUCC; // Or an error indicating it's already running
    }

    // Create periodic task
    int err = memset_s(&g_priv_data, sizeof(g_priv_data), 0, sizeof(g_priv_data));
    if (err != EOK) {
        return ERRCODE_MEMSET;
    }
    g_priv_data.is_running = true;

    // Register memory metric
    ret = uapi_diag_metrics_period_report(DIAG_CMD_DIAG_REPORT_MEM_INFO, diag_get_mem_info, METRICS_POLLING_INTERVAL_S,
                                          sizeof(mdm_mem_info_t), NULL);
    if (ret != ERRCODE_SUCC) {
        dfx_log_debug("register mem info failed: 0x%x", ret);
        return ret;
    }

#ifdef LOSCFG_KERNEL_CPUP
    // Register cpup metric
    ret = uapi_diag_metrics_period_report(DIAG_CMD_DIAG_REPORT_CPUP_INFO, diag_get_cpup_info, METRICS_POLLING_INTERVAL_S,
                                          sizeof(dfx_cpup_item_usage_info_t), NULL);
    if (ret != ERRCODE_SUCC) {
        dfx_log_debug("register cpup info failed: 0x%x", ret);
        // 注意：已注册的内存指标无法自动回滚，但未设置 init，上层调用会感知到失败
        return ret;
    }
#endif

    // Register storage metric
    ret = uapi_diag_metrics_period_report(DIAG_CMD_DIAG_REPORT_STORAGE_INFO, diag_get_storage_info,
                                          METRICS_POLLING_INTERVAL_S, sizeof(diag_storage_info_t), NULL);
    if (ret != ERRCODE_SUCC) {
        dfx_log_debug("register storage info failed: 0x%x", ret);
        return ret;
    }

    g_priv_data.init = true;
    return ERRCODE_SUCC;
}

// 去初始化内存metrics
errcode_t diag_os_metrics_deinit(void)
{
    int ret;
    if (!g_priv_data.init) {
        return ERRCODE_SUCC; // Or an error indicating it's not running
    }
    g_priv_data.is_running = false;
    uapi_diag_metrics_stop_report(DIAG_CMD_DIAG_REPORT_MEM_INFO);
    uapi_diag_metrics_stop_report(DIAG_CMD_DIAG_REPORT_CPUP_INFO);
    uapi_diag_metrics_stop_report(DIAG_CMD_DIAG_REPORT_STORAGE_INFO);
    g_priv_data.init = false;
    ret = memset_s(&g_priv_data, sizeof(g_priv_data), 0, sizeof(g_priv_data));
    if (ret != ERRCODE_SUCC) {
        return ERRCODE_MEMSET;
    }
    return ERRCODE_SUCC;
}