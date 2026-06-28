/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: diag log hook
 * This file should be changed only infrequently and with great care.
 */

#ifndef __DIAG_LOG_HOOK_H__
#define __DIAG_LOG_HOOK_H__

#include "errcode.h"
#include "massdata_buffer.h"
#include "diag.h"
#include "hcd.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Callback to write log to diagnostics storage
 */
typedef errcode_t (*diag_write_cb_t)(uint16_t cmd_id, void *data, uint32_t len, void *priv_data);

/**
 * @brief Diagnostics log configurations
 */
typedef struct {
    diag_write_cb_t log_write_cb;       /*!< Callback function to write diagnostics log data */
    diag_write_cb_t event_write_cb;     /*!< Callback function to write diagnostics event data */
    void *cb_arg;                       /*!< User data to pass in callback function */
} diag_log_config_t;

/**
 * @brief Log hook for writing logs to diagnostics storage
 *
 * @param[in] module_id Module ID
 * @param[in] msg_id Message ID
 * @param[in] buf Pointer to log data buffer
 * @param[in] size Size of log data
 * @param[in] level Log level
 *
 * @return ERRCODE_SUCC if successful, appropriate error code otherwise.
 */
errcode_t diag_log_add(uint32_t module_id, uint32_t msg_id, const uint8_t *buf, uint32_t size, uint8_t level);

/**
 * @brief Hook for writing system CHR events to diagnostics storage
 *
 * @param[in] event Pointer to CHR system event
 *
 * @return ERRCODE_SUCC if successful, appropriate error code otherwise.
 */
errcode_t diag_massdata_add(system_event_s_t *event);

/**
 * @brief Deinitialize diagnostics log hook
 *
 * @return ERRCODE_SUCC if successful, appropriate error code otherwise.
 */
errcode_t diag_log_hook_deinit(void);

/**
 * @brief Initialize diagnostics log hook
 *
 * @param[in] config Pointer to a config structure of type \ref diag_log_config_t
 *
 * @return ERRCODE_SUCC if successful, appropriate error code otherwise.
 */
errcode_t diag_log_hook_init(diag_log_config_t *config);

/**
 * @brief Enable the diagnostics log hook for provided log level
 *
 * @param[in] level Log level to enable, can be the bitwise OR of log level
 */
void diag_log_hook_enable(uint8_t level);

/**
 * @brief Disable the diagnostics log hook for provided log type
 *
 * @param[in] level Log type to disable, can be the bitwise OR of log level
 *
 */
void diag_log_hook_disable(uint8_t level);

#ifdef __cplusplus
}
#endif

#endif // __DIAG_LOG_HOOK_H__