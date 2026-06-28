/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: diag data transport
 * This file should be changed only infrequently and with great care.
 */

#include "dfx_adapt_layer.h"
#include "diag_data_transport.h"

typedef struct {
    bool init;  // 初始化标志
    diag_data_transport_config_t config; // diag数据传输通道回调配置
} priv_data_t;

static priv_data_t g_priv_data;

// 注册diag数据传输通道回调
errcode_t diag_data_transport_register(diag_data_transport_config_t *config)
{
    if (g_priv_data.init) {
        dfx_log_debug("[DIAG_REPORT]Already initialized, reinitializing..");
        return ERRCODE_DIAG_NOT_INIT;
    }
    /* We at least need data_send callback */
    if (!(config && config->callbacks.data_send)) {
        dfx_log_err("[DIAG_REPORT]Failed to init transport, Please set at least data_send callback");
        return ERRCODE_INVALID_PARAM;
    }
    
    errno_t ret = memcpy_s(&g_priv_data.config, sizeof(g_priv_data.config), config, sizeof(g_priv_data.config));
    if (ret != EOK) {
        dfx_log_err("[DIAG_REPORT]diag_data_transport_register memcpy fail : %d", ret);
        return ERRCODE_MEMCPY;
    }
    g_priv_data.init = true;

    /* Call transport init */
    if (g_priv_data.config.callbacks.init) {
        g_priv_data.config.callbacks.init(config->userdata);
    }
    return ERRCODE_SUCC;
}

// 注销diag数据传输通道回调
void diag_data_transport_unregister(void)
{
    errno_t ret;
    if (!g_priv_data.init) {
        dfx_log_debug("[DIAG_REPORT]Diag_data_transport not initialized");
        return;
    }
    /* Call transport deinit */
    if (g_priv_data.config.callbacks.deinit) {
        g_priv_data.config.callbacks.deinit();
    }

    ret = memset_s(&g_priv_data, sizeof(g_priv_data), 0, sizeof(g_priv_data));
    if (ret != EOK) {
        dfx_log_err("[DIAG_REPORT]diag_data_transport_unregister memset fail : %d", ret);
    }
}

// 连接diag数据传输通道
errcode_t diag_data_transport_connect(void)
{
    if (!g_priv_data.init) {
        dfx_log_debug("[DIAG_REPORT]Transport callbacks not registered");
        return ERRCODE_DIAG_NOT_INIT;
    }
    if (g_priv_data.config.callbacks.connect) {
        return g_priv_data.config.callbacks.connect();
    }
    dfx_log_debug("[DIAG_REPORT]connect callback not set");
    return ERRCODE_SUCC;
}

// 断开diag数据传输通道
void diag_data_transport_disconnect(void)
{
    if (!g_priv_data.init) {
        dfx_log_debug("[DIAG_REPORT]Transport callbacks not registered");
        return;
    }
    if (g_priv_data.config.callbacks.disconnect) {
        return g_priv_data.config.callbacks.disconnect();
    }
    dfx_log_debug("[DIAG_REPORT]disconnect callback not set");
}

// 获取diag数据传输通道连接状态
bool diag_data_transport_is_connected(void)
{
    if (!g_priv_data.init) {
        dfx_log_debug("[DIAG_REPORT]Transport callbacks not registered");
        return false;
    }
    if (g_priv_data.config.callbacks.is_connected) {
        return g_priv_data.config.callbacks.is_connected();
    }
    dfx_log_debug("[DIAG_REPORT]is_connected callback not set");
    return false;
}

// 发送diag数据至云端
errcode_t diag_data_transport_data_send(diag_data_item_t *item)
{
    if (!g_priv_data.init) {
        dfx_log_debug("[DIAG_REPORT]Transport callbacks not registered");
        return ERRCODE_DIAG_NOT_INIT;
    }
    if (g_priv_data.config.callbacks.data_send) {
        return g_priv_data.config.callbacks.data_send(item);
    }
    dfx_log_debug("[DIAG_REPORT]data send callback not set");
    return ERRCODE_SUCC;
}

// 发送diag数据至云端
errcode_t diag_data_transport_data_send_buf(const uint8_t *buf, uint16_t len)
{
    if (!g_priv_data.init) {
        dfx_log_debug("[DIAG_REPORT]Transport callbacks not registered");
        return ERRCODE_DIAG_NOT_INIT;
    }
    if (g_priv_data.config.callbacks.data_send_buf) {
        return g_priv_data.config.callbacks.data_send_buf(buf, len);
    }
    dfx_log_debug("[DIAG_REPORT]data send buf callback not set");
    return ERRCODE_SUCC;
}
