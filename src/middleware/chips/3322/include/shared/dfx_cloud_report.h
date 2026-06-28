/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: dfx cloud report
 * This file should be changed only infrequently and with great care.
 */

#ifndef DFX_CLOUD_REPORT_H
#define DFX_CLOUD_REPORT_H
#include "errcode.h"

#define DFX_CLOUD_REPORT_MSG 1

typedef enum {
    DIAG_TRIGGER_NONE        = 0,
    DIAG_TRIGGER_IMMEDIATE   = 1u << 0,  // critical / 强制立即
    DIAG_TRIGGER_THRESHOLD   = 1u << 1,  // 达到条数/字节阈值
    DIAG_TRIGGER_INTERVAL    = 1u << 2,  // 限频兜底
    DIAG_TRIGGER_FLUSH       = 1u << 3,  // 手动/策略flush
} diag_trigger_flag_t;


/**
 * @brief dfx_report configuration
 */
typedef struct {
    /** Log levels to enable, bitwise OR the values from \ref DIAG_LOG_LEVEL */
    uint8_t log_level;
    /** Node id for dfx_report. If NULL then dfx_report agent uses MAC address as node id */
    const char *node_id;
    /** Authentication key, valid only for https transport */
    const char *auth_key;
} dfx_report_config_t;

/**
 * @brief dfx_report event.
 */
typedef struct {
    /** current item numbers in queue */
    uint32_t     item_num;
    /** optional:current data bytes in queue */
    uint32_t     byte_num;
    /** event reasons */
    uint32_t     trigger_flags;
    /** event time */
    uint32_t     now_ms;
} diag_write_evt_t;

/**
 * @brief write dfx_report event.
 *
 * @param evt diag report event
 */
void diag_report_write_event(const diag_write_evt_t *evt);

/**
 * @brief disable dfx_report.
 *
 * @return ERRCODE_SUCC on success, appropriate error code otherwise
 */
errcode_t dfx_report_disable(void);

/**
 * @brief Enable dfx_report except transport.
 *
 * This API is used in conjunction with dfx_report_transport_register()
 * to start dfx_report with custom transport.
 *
 * @param[in] config Configuration for dfx_report.
 *
 * @return ERRCODE_SUCC on success, appropriate error code otherwise
 */
errcode_t dfx_report_enable(dfx_report_config_t *config);

/**
 * @brief diag_report_consumer_task.
 *
 * @param[in] arg
 */
void diag_report_consumer_task(void *arg);

/**
 * @brief dfx_report_deinit.
 *
 */
void dfx_report_system_deinit(void);

/**
 * @brief dfx_report_init.
 *
 * @return ERRCODE_SUCC on success, appropriate error code otherwise
 */
errcode_t dfx_report_system_init(void);

#endif