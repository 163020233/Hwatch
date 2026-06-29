/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description:  diag report control
 * This file should be changed only infrequently and with great care.
 */

#ifndef __DIAG_REPORT_CONTROL_H__
#define __DIAG_REPORT_CONTROL_H__

#include "errcode.h"
#include "diag.h"
#include "dfx_feature_config.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @if Eng
 * @brief  Metrics interval structure for command setup and query.
 * @else
 * @brief  用于命令设置和查询的metrics周期结构体。
 * @endif
 */
typedef struct {
    uint32_t cmd_id;
    uint32_t interval;
} diag_metrics_interval_t;

/**
 * @brief Diagnostics log control operations
*/
typedef enum {
    LOGCTRL_OP_SET_ABS = 0,
    LOGCTRL_OP_ENABLE,
    LOGCTRL_OP_DISABLE,
    LOGCTRL_OP_QUERY,
} diag_logctrl_op_t;

/**
 * @brief Diagnostics metrics control operations
*/
typedef enum {
    METRICS_CTRL_OP_QUERY = 0,
    METRICS_CTRL_OP_INIT,
    METRICS_CTRL_OP_DEINIT,
} diag_metrics_ctrl_op_t;

/**
 * @brief Diagnostics log control command structure
*/
typedef struct {
    uint32_t op_id;
    uint8_t level;
} diag_logctrl_cmd_t;

/**
 * @brief Get current report control.
 *
 * @return True if diag report is enabled, false otherwise.
 */
bool diag_get_report_ctrl(void);

/**
 * @brief Set report control.
 *
 * @param ctrl True to enable diag report, false to disable.
 */
void diag_set_report_ctrl(bool ctrl);

#if defined(CONFIG_DFX_SUPPORT_METRICS_REPORT) && (CONFIG_DFX_SUPPORT_METRICS_REPORT == DFX_YES)
/**
 * @brief Command function for controlling the metrics reporting behavior.
 *
 * @param[in] cmd_id Command ID.
 * @param[in] cmd_param Command parameter.
 * @param[in] cmd_param_size Command parameter size.
 * @param[in] option Reserved.
 * @return ERRCODE_SUCC on success, or an error code on failure.
 */
errcode_t diag_cmd_set_metrics(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option);
#endif

#if defined(CONFIG_DFX_SUPPORT_DIAG_LOG_HOOK)  && (CONFIG_DFX_SUPPORT_DIAG_LOG_HOOK == DFX_YES)
/**
 * @brief Command function for setting the report log hook level.
 *
 * @param cmd_id Command ID.
 * @param cmd_param Command parameter.
 * @param cmd_param_size Command parameter size.
 * @param option Reserved.
 * @return ERRCODE_SUCC on success, or an error code on failure.
 */
errcode_t diag_cmd_set_report_log_level(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size,
                                        diag_option_t *option);
#endif

/**
 * @brief Command function for reporting system boot info.
 *
 * @param cmd_id Command ID.
 * @param cmd_param Command parameter.
 * @param cmd_param_size Command parameter size.
 * @param option Reserved.
 * @return ERRCODE_SUCC on success, or an error code on failure.
 */
errcode_t diag_cmd_report_boot_info(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size,
                                    diag_option_t *option);

#ifdef __cplusplus
}
#endif

#endif /* __DIAG_REPORT_CONTROL_H__ */