/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: diag metrics
 * This file should be changed only infrequently and with great care.
 */

#ifndef __DIAG_METRICS_COMMON_H__
#define __DIAG_METRICS_COMMON_H__

#include "errcode.h"
#include "hcd.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define METRICS_POLLING_INTERVAL_S 10       // 10 seconds
#define VARIABLES_POLLING_INTERVAL_S 60     // 60 seconds

/**
 * @if Eng
 * @brief  Callback function pointer type to write metric data into diag_report storage.
 * This function will be called by the metrics module to output data.
 * @else
 * @brief  Callback函数指针类型，用于将metric数据写入diag_report存储。
 * 此函数由metrics模块调用，用于输出数据。
 * @endif
 */
typedef uint32_t (*diag_metrics_write_cb_t)(uint16_t cmd_id, const void *data, uint32_t len, void *cb_arg);

/**
 * @if Eng
 * @brief  State of the metrics module.
 * @else
 * @brief  metrics模块的状态。
 * @endif
 */
typedef enum {
    METRICS_STATE_NOT_INIT = 0,
    METRICS_STATE_READY_DISABLED, // 资源已初始化，但未开启采集
    METRICS_STATE_RUNNING,        // 采集任务已启动
} metrics_run_state_t;

/**
 * @if Eng
 * @brief  Configuration structure for the metrics module.
 * @else
 * @brief  metrics模块的配置结构体。
 * @endif
 */
typedef struct {
    diag_metrics_write_cb_t write_cb; /*!< Callback function to write diagnostics data */
    void *cb_arg;                     /*!< User data to pass in callback function */
} diag_metrics_config_t;

/**
 * @brief Initialize the metrics module ressources.
 *
 * @return ERRCODE_SUCC on success, or an error code on failure.
 */
errcode_t diag_metrics_init(void);

/**
 * @brief Deinitialize the metrics module ressources.
 *
 * @return ERRCODE_SUCC on success, or an error code on failure.
 */
errcode_t diag_metrics_deinit(void);

/**
 * @brief Enable metrics collector task.
 *
 * @return ERRCODE_SUCC on success, or an error code on failure.
 */
errcode_t diag_metrics_enable(void);

/**
 * @brief Disable metrics collector task.
 *
 * @return ERRCODE_SUCC on success, or an error code on failure.
 */
errcode_t diag_metrics_disable(void);

#ifdef __cplusplus
}
#endif
#endif