/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: diag log hook
 * This file should be changed only infrequently and with great care.
 */

#include "diag_log_hook.h"
#include "soc_diag_cmd_id.h"
#include "dfx_adapt_layer.h"
#include "diag_filter.h"
#include "diag_metrics_common.h"
#include "diag_filter.h"
#include "diag_report_control.h"
#ifdef CONFIG_NV_FEATURE_SUPPORT
#include "nv.h"
#endif

// 定义日志过滤参数，
#define ARG_FILTER_LOG1 0xee980d85
#define ARG_FILTER_LOG2 0xe88c0e65
#define ARG_FILTER_LOG3 0xe88c0a5d
#define ARG_FILTER_LOG4 0xee98082a
#define ARG_FILTER_LOG5 0xe88c0a2d

// 定义日志钩子私有数据结构
typedef struct {
    uint8_t enabled_log_level; // 启用的日志级别
    diag_log_config_t config; // 日志配置
    bool init; // 初始化状态
} log_hook_priv_data_t;

// 定义全局私有数据
static log_hook_priv_data_t g_priv_data;

/**
 * 向存储写入日志数据
 *
 * @param cmd_id 命令ID
 * @param data 要写入的数据
 * @param len 数据长度
 * @return 成功返回0，失败返回-1
 */
static errcode_t write_log(uint16_t cmd_id, void *data, uint32_t len)
{
    // 检查写回调函数是否存在
    if (g_priv_data.config.log_write_cb) {
        // 调用写回调函数，将数据写入设备
        return g_priv_data.config.log_write_cb(cmd_id, data, len, g_priv_data.config.cb_arg);
    }
    // 如果没有写回调函数，返回失败
    return ERRCODE_FAIL;
}

/**
 * 向存储写入事件数据
 *
 * @param cmd_id 命令ID
 * @param data 要写入的数据
 * @param len 数据长度
 * @return 成功返回0，失败返回-1
 */
static errcode_t write_event(uint16_t cmd_id, void *data, uint32_t len)
{
    // 检查写回调函数是否存在
    if (g_priv_data.config.event_write_cb) {
        // 调用写回调函数，将数据写入设备
        return g_priv_data.config.event_write_cb(cmd_id, data, len, g_priv_data.config.cb_arg);
    }
    // 如果没有写回调函数，返回失败
    return ERRCODE_FAIL;
}

// 日志过滤检查函数
static errcode_t check_log_filter(uint32_t module_id, uint32_t msg_id, uint8_t level)
{
#if (defined CONFIG_DFX_DATA_TRANSPORT_BT && CONFIG_DFX_DATA_TRANSPORT_BT == DFX_YES) || \
    (defined CONFIG_DFX_DATA_TRANSPORT_BLE && CONFIG_DFX_DATA_TRANSPORT_BLE == DFX_YES)
    if (module_id == LOG_BGHMODULE && (level == DIAG_LOG_LEVEL_INFO || level == DIAG_LOG_LEVEL_DBG)) {
        return ERRCODE_NOT_SUPPORT;
    }
#endif
    if (msg_id == ARG_FILTER_LOG1 || msg_id == ARG_FILTER_LOG2 ||
        msg_id == ARG_FILTER_LOG3 || msg_id == ARG_FILTER_LOG4 || msg_id == ARG_FILTER_LOG5) {
        return ERRCODE_NOT_SUPPORT;
    }
    if (level > 32) { // 防止32bit移位越界
        return ERRCODE_NOT_SUPPORT;
    }
    // 检查日志级别是否启用
    if ((!((1U << level) & g_priv_data.enabled_log_level))) {
        return ERRCODE_INVALID_PARAM;
    }
    if (level != DIAG_LOG_LEVEL_EVENT && !zdiag_log_enable(level, module_id)) {
        return ERRCODE_INVALID_PARAM;
    }
    return ERRCODE_SUCC;
}

/**
 * @brief 添加诊断日志
 * @param module_id 模块ID
 * @param msg_id 消息ID
 * @param buf 日志数据缓冲区
 * @param size 日志数据大小
 * @param level 日志级别
 * @return 返回错误码，ERRCODE_SUCC表示成功
 */
errcode_t diag_log_add(uint32_t module_id, uint32_t msg_id, const uint8_t *buf, uint32_t size, uint8_t level)
{
    // 检查初始化状态
    if (!g_priv_data.init) {
        return ERRCODE_DIAG_NOT_INIT;
    }

    // 统一过滤检查
    errcode_t filter_ret = check_log_filter(module_id, msg_id, level);
    if (filter_ret != ERRCODE_SUCC) {
        return filter_ret;
    }

    static uint32_t msg_sn = 0;
    errcode_t ret = ERRCODE_SUCC;
    diag_cmd_log_layer_t *log = (diag_cmd_log_layer_t *)dfx_malloc(0, sizeof(diag_cmd_log_layer_t) + size);
    if (log == NULL) {
        return ERRCODE_MALLOC;
    }
    diag_mk_log_pkt(log, module_id, msg_id, msg_sn++, true);
    uint32_t log_size = DIAG_LOG_HEADER_SIZE + size;

    // 复制日志数据
    if (size != 0) {
        if (buf == NULL || memcpy_s(log->data, size, buf, size) != EOK) {
            dfx_free(0, log);
            return (buf == NULL) ? ERRCODE_INVALID_PARAM : ERRCODE_MEMCPY;
        }
    }

    // 写入日志数据
#if (CONFIG_DFX_SUPPORT_EVENT_REPORT == DFX_YES)
    if (level == DIAG_LOG_LEVEL_EVENT) {
        ret = write_event(DIAG_CMD_MSG_RPT_SYS, log, log_size);
        if (ret != ERRCODE_SUCC) dfx_log_debug("write_event failed, ret=%d", ret);
    }
#endif
    if (level != DIAG_LOG_LEVEL_EVENT) {
        ret = write_log(DIAG_CMD_MSG_RPT_SYS, log, log_size);
    }
    dfx_free(0, log);
    return ret;
}

/**
 * 启用日志钩子
 * @param level 要启用的日志级别
 *
 * 此函数用于启用指定的日志级别。
 */
void diag_log_hook_enable(uint8_t level)
{
    // 将指定的日志级别添加到已启用的日志级别中
    g_priv_data.enabled_log_level |= level;
}

/**
 * 禁用日志钩子
 * @param level 要禁用的日志级别
 *
 * 此函数用于禁用指定的日志级别。
 */
void diag_log_hook_disable(uint8_t level)
{
    // 从已启用的日志级别中移除指定的日志级别
    g_priv_data.enabled_log_level &= (~level);
}

errcode_t diag_cmd_set_report_log_level(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size,
                                        diag_option_t *option)
{
    // 检查cmd_param是否为正确的类型
    if (cmd_param_size != sizeof(diag_logctrl_cmd_t)) {
        return ERRCODE_INVALID_PARAM;
    }

    const diag_logctrl_cmd_t *logctrl_cmd = (const diag_logctrl_cmd_t *)cmd_param;
    switch (logctrl_cmd->op_id) {
        case LOGCTRL_OP_SET_ABS:
            g_priv_data.enabled_log_level = logctrl_cmd->level;
            break;
        case LOGCTRL_OP_ENABLE:
            diag_log_hook_enable(logctrl_cmd->level);
            break;
        case LOGCTRL_OP_DISABLE:
            diag_log_hook_disable(logctrl_cmd->level);
            break;
        case LOGCTRL_OP_QUERY:
        default:
            break;
    }
    diag_logctrl_cmd_t response_logctrl_cmd = *logctrl_cmd;
    response_logctrl_cmd.level = g_priv_data.enabled_log_level;
#ifdef CONFIG_NV_FEATURE_SUPPORT
    // 保存日志级别至NV存储
    uapi_nv_write(NV_ID_HCD_LOG_ENABLE_FLAG, &response_logctrl_cmd.level, sizeof(uint8_t));
#endif
    // 上报日志级别
    uapi_diag_metrics_report_once(DIAG_CMD_REPORT_LOG_LEVEL, &response_logctrl_cmd.level, sizeof(uint8_t));
    return uapi_diag_report_packet(cmd_id, option, (uint8_t *)&response_logctrl_cmd,
                                   sizeof(diag_logctrl_cmd_t), true);
}

// 回调：获取上报日志过滤器级别
static errcode_t diag_get_log_level(void *data_ptr, size_t *len_ptr, void *ctx)
{
    unused(ctx);
    if (!data_ptr || !len_ptr || *len_ptr < sizeof(uint8_t)) {
        return ERRCODE_FAIL;
    }

    uint8_t *level_ptr = (uint8_t *)data_ptr;
    *level_ptr = (uint8_t)g_priv_data.enabled_log_level;
    *len_ptr = sizeof(uint8_t);

    return ERRCODE_SUCC;
}

errcode_t diag_massdata_add(system_event_s_t *event)
{
    // 检查参数有效性
    if (event == NULL) {
        return ERRCODE_DIAG_INVALID_PARAMETER;
    }
    // 检查初始化状态
    if (!g_priv_data.init) {
        return ERRCODE_DIAG_NOT_INIT;
    }
    return write_event(DIAG_CMD_REPORT_MASSDATA, event, sizeof(system_event_s_t) + event->data_len);
}

errcode_t diag_log_hook_deinit(void)
{
    // 检查是否已初始化
    if (!g_priv_data.init) {
        return ERRCODE_SUCC;
        // 清空私有数据
    }
    g_priv_data.enabled_log_level = 0;
    g_priv_data.init = false;
    return ERRCODE_SUCC;
}

/**
 * 初始化诊断日志钩子
 *
 * @param config 诊断日志配置
 * @return 错误码，ERRCODE_SUCC表示成功，其他值表示失败
 */
errcode_t diag_log_hook_init(diag_log_config_t *config)
{
    errcode_t ret;
    // 检查参数有效性
    if (!config || !config->log_write_cb || !config->event_write_cb) {
        return ERRCODE_INVALID_PARAM;
    }
    // 检查是否已初始化
    if (g_priv_data.init) {
        return ERRCODE_FAIL;
    }
    // 复制配置到全局变量
    ret = (errcode_t)memcpy_s(&g_priv_data.config, sizeof(diag_log_config_t), config, sizeof(diag_log_config_t));
    if (ret != ERRCODE_SUCC) {
        dfx_log_err("memcpy failed : 0x%x", ret);
        return ERRCODE_MEMCPY;
    }
    g_priv_data.enabled_log_level = 0;
    // Register log level filter metric
    ret = uapi_diag_metrics_period_report(DIAG_CMD_REPORT_LOG_LEVEL, diag_get_log_level, VARIABLES_POLLING_INTERVAL_S,
                                          sizeof(uint8_t), NULL);
    if (ret != ERRCODE_SUCC) {
        dfx_log_err("register log level metric failed: 0x%x", ret);
        return ret;
    }
    // 设置初始化标志
    g_priv_data.init = true;
    return ERRCODE_SUCC;
}