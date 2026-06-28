/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: log flow control
 */

#include "log_limit.h"
#include "osal_timer.h"
#include "common_def.h"
#if defined(SUPPORT_IPC) || defined(IPC_NEW)
#include "ipc.h"
#endif
#include "platform_types.h"
#include "diag_log.h"
#include "diag_msg.h"
#include "panic.h"
#include "chip_core_definition.h"
#ifdef IPC_NEW
#include "ipc_actions.h"
#include "ipc_new_shared.h"
#endif
#include "dfx_adapt_layer.h"
#include "dfx_feature_config.h"

#if (defined(CONFIG_DFX_SUPPORT_LOG_LIMIT) && (CONFIG_DFX_SUPPORT_LOG_LIMIT == DFX_YES))
STATIC log_limit_t *g_log_limits = NULL;
STATIC uint32_t g_log_limit_num = 0;
STATIC bool g_log_limit_enable = false;
STATIC osal_timer g_log_limit_timer = {0};
#if (CORE == APPS)
STATIC log_limit_info_cb_t g_log_limit_info_cb = NULL;
#endif

STATIC log_limit_t *get_group_limit_info(uint8_t group)
{
    for (uint32_t i = 0; i < g_log_limit_num; i++) {
        if (g_log_limits[i].group == group) {
            return &g_log_limits[i];
        }
    }
    return NULL;
}

STATIC bool get_module_limit_info(uint16_t module_id, log_limit_t **llt1, log_limit_t **llt2)
{
    if (g_log_limits == NULL || g_log_limit_num == 0) {
        return false;
    }
    for (uint32_t i = 0; i < g_log_limit_num; i++) {
        if (g_log_limits[i].module_id == module_id) {
            *llt1 = &g_log_limits[i];
            *llt2 = get_group_limit_info((*llt1)->group);
            break;
        }
    }
    if (*llt1 == NULL || *llt2 == NULL) {
        return false;
    }
    return true;
}

bool log_limit_hook(uint16_t module_id, uint8_t level)
{
    if (g_log_limit_enable == false) {
        return true;
    }

    log_limit_t *log_limit = NULL, *log_limit_group = NULL;
    bool ret = get_module_limit_info(module_id, &log_limit, &log_limit_group);
    if (!ret) {
        return true;
    }

    if (!log_limit->enable) {
        log_limit->printed_count++;
        return true;
    }

    if (log_limit_group->available_count > 0) {
        log_limit_group->available_count--;
        log_limit->printed_count++;
        return true;
    } else if (level == DIAG_LOG_LEVEL_ERROR) {
        log_limit->printed_count++;
        return true;
    } else {
        log_limit->lost_count++;
        if (log_limit->lost_count == 1) {
            uapi_diag_error_log(0, "[log_limit]module:0x%x start lost", log_limit->module_id);
        }
    }

    return false;
}

STATIC void log_limit_timer_handler(unsigned long data)
{
    unused(data);
    uapi_log_limit_reset();
    osal_timer_start(&g_log_limit_timer);
}

#ifdef SUPPORT_IPC
#if (CORE != APPS)
STATIC void ipc_send_log_limit_info(cores_t dst)
{
    size_t size = sizeof(ipc_payload_get_log_limit_t) + sizeof(ipc_payload_log_limit_info_t) * g_log_limit_num;
    ipc_payload_get_log_limit_t *ipc_log_limit = (ipc_payload_get_log_limit_t *)dfx_malloc(0, size);
    if (ipc_log_limit == NULL) {
        panic(PANIC_LOG, __LINE__);
        return;
    }
    ipc_log_limit->module_num = g_log_limit_num;
    if (g_log_limit_num > 0 &&
        memcpy_s(ipc_log_limit->info, sizeof(ipc_payload_log_limit_info_t) * g_log_limit_num,
        g_log_limits, sizeof(log_limit_t) * g_log_limit_num) != EOK) {
        dfx_free(0, ipc_log_limit);
        return;
    }
    ipc_spin_send_message_timeout(dst,
                                  IPC_ACTION_GET_LOG_LIMIT,
                                  (ipc_payload *)(uintptr_t)ipc_log_limit,
                                  (uint16_t)size,
                                  IPC_PRIORITY_LOWEST, false, IPC_SPIN_SEND_DEFAULT_TIMEOUT);
    dfx_free(0, ipc_log_limit);
}

STATIC bool set_log_limit_action_handler(ipc_action_t message,
                                         const volatile ipc_payload *payload_p, cores_t src, uint32_t id)
{
    unused(message);
    unused(id);

    uint16_t module_id = payload_p->set_log_limit.module_id;
    bool enable = payload_p->set_log_limit.enable;
    if (module_id == LOG_LIMIT_SET_ALL) {
        uapi_log_limit_set_enable(CORE, enable);
    } else {
        uapi_log_limit_set_mod_enable(CORE, module_id, enable);
    }
    ipc_send_log_limit_info(src);

    return true;
}
#endif

STATIC bool get_log_limit_action_handler(ipc_action_t message,
                                         const volatile ipc_payload *payload_p, cores_t src, uint32_t id)
{
    unused(message);
    unused(payload_p);
    unused(id);

#if (CORE == APPS)
    if (g_log_limit_info_cb != NULL) {
        g_log_limit_info_cb(src, (const log_limit_t *)payload_p->get_log_limit.info,
                            payload_p->get_log_limit.module_num);
    }
#else
    ipc_send_log_limit_info(src);
#endif

    return true;
}
#elif defined(IPC_NEW)
#if (CORE != APPS)
STATIC void ipc_send_log_limit_info(cores_t dst)
{
    size_t size = sizeof(ipc_payload_get_log_limit_t) + sizeof(ipc_payload_log_limit_info_t) * g_log_limit_num;
    ipc_payload_get_log_limit_t *ipc_log_limit = (ipc_payload_get_log_limit_t *)dfx_malloc(0, size);
    if (ipc_log_limit == NULL) {
        panic(PANIC_LOG, __LINE__);
        return;
    }
    ipc_log_limit->module_num = g_log_limit_num;
    ipc_log_limit->core = CORE;

    if (g_log_limit_num > 0 &&
        memcpy_s(ipc_log_limit->info, sizeof(ipc_payload_log_limit_info_t) * g_log_limit_num,
        g_log_limits, sizeof(log_limit_t) * g_log_limit_num) != EOK) {
        uapi_diag_error_log(0, "[log_limit][set_enable] memcpy_s fail!");
        dfx_free(0, ipc_log_limit);
        return;
    }

    ipc_msg_info_t msg_info;
    msg_info.dst_core = dst;
    msg_info.priority = IPC_PRIORITY_LOWEST;
    msg_info.channel = 0;
    msg_info.msg_id = IPC_MSG_GET_LOG_LIMIT;
    msg_info.buf_addr = (uint8_t *)ipc_log_limit;
    msg_info.buf_len = (uint32_t)size;

    errcode_t ret = uapi_ipc_send_msg_sync(&msg_info);
    if (ret != ERRCODE_SUCC) {
        uapi_diag_error_log(0, "[log_limit][send] send ipc fail[0x%x]!", ret);
    }

    dfx_free(0, ipc_log_limit);
}

STATIC void set_log_limit_action_handler(uint8_t *payload_addr, uint32_t payload_len)
{
    unused(payload_len);
    ipc_payload_set_log_limit_t *ipc_set_log_limit = (ipc_payload_set_log_limit_t *)payload_addr;
    uint16_t module_id = ipc_set_log_limit->module_id;
    bool enable = ipc_set_log_limit->enable;
    if (module_id == LOG_LIMIT_SET_ALL) {
        uapi_log_limit_set_enable(CORE, enable);
    } else {
        uapi_log_limit_set_mod_enable(CORE, module_id, enable);
    }
    ipc_send_log_limit_info(CORES_APPS_CORE);
}
#endif

STATIC void get_log_limit_action_handler(uint8_t *payload_addr, uint32_t payload_len)
{
    unused(payload_len);
    ipc_payload_get_log_limit_t *ipc_log_limit = (ipc_payload_get_log_limit_t *)payload_addr;

#if (CORE == APPS)
    if (g_log_limit_info_cb != NULL) {
        g_log_limit_info_cb((uint8_t)ipc_log_limit->core, (const log_limit_t *)ipc_log_limit->info,
                            ipc_log_limit->module_num);
    }
#else
    ipc_send_log_limit_info(CORES_APPS_CORE);
#endif
}
#endif

errcode_t uapi_log_limit_init(log_limit_t *log_limits, uint32_t log_limit_num)
{
    if (log_limits == NULL) {
        return ERRCODE_FAIL;
    }
    for (uint8_t i = 0; i < log_limit_num; i++) {
        if (log_limits[i].max_count == 0 && log_limits[i].available_count == 0) {
            continue;
        }
        for (uint8_t j = i + 1; j < log_limit_num; j++) {
            if (log_limits[i].group == log_limits[j].group) {
                log_limits[i].max_count += log_limits[j].max_count;
                log_limits[i].available_count += log_limits[j].available_count;
                log_limits[j].max_count = 0;
                log_limits[j].available_count = 0;
            }
        }
    }
    g_log_limits = log_limits;
    g_log_limit_num = log_limit_num;

    uapi_diag_register_msg_flow_control_hook(log_limit_hook);

    g_log_limit_timer.handler = log_limit_timer_handler;
    g_log_limit_timer.data = 0;
    g_log_limit_timer.interval = LOG_LIMIT_RESET_TIMEOUT;
    if (osal_timer_init(&g_log_limit_timer) < 0) {
        uapi_diag_error_log(0, "[log_limit]Create reset timer failed!");
        return ERRCODE_FAIL;
    }

#ifdef SUPPORT_IPC
#if (CORE != APPS)
    ipc_register_handler(IPC_ACTION_SET_LOG_LIMIT, set_log_limit_action_handler);
#endif
    ipc_register_handler(IPC_ACTION_GET_LOG_LIMIT, get_log_limit_action_handler);
#elif defined (IPC_NEW)
    ipc_rx_handler_info_t info;
#if (CORE != APPS)
    info.msg_id = IPC_MSG_SET_LOG_LIMIT;
    info.cb = set_log_limit_action_handler;
    uapi_ipc_register_rx_handler(&info);
#endif
    info.msg_id = IPC_MSG_GET_LOG_LIMIT;
    info.cb = get_log_limit_action_handler;
    uapi_ipc_register_rx_handler(&info);
#endif

    return ERRCODE_SUCC;
}

void uapi_log_limit_reset(void)
{
    for (uint32_t i = 0; i < g_log_limit_num; i++) {
        log_limit_t *log_limit = &g_log_limits[i];
        uapi_diag_error_log(0, "[log_limit]module:0x%x,available_count:%d,lost_count:%d,printed_count:%d,enable:%u\n",
                            log_limit->module_id, log_limit->available_count, log_limit->lost_count,
                            log_limit->printed_count, log_limit->enable);
        uint32_t irq = osal_irq_lock();
        log_limit->all_printed_count += log_limit->printed_count;
        log_limit->all_lost_count += log_limit->lost_count;
        log_limit->available_count = log_limit->max_count;
        log_limit->printed_count = 0;
        log_limit->lost_count = 0;
        osal_irq_restore(irq);
    }
}

void uapi_log_limit_set_enable(uint8_t core, bool enable)
{
    if (core == CORE) {
        if (g_log_limit_enable == enable) {
            return;
        }
        g_log_limit_enable = enable;
        if (enable == true) {
            osal_timer_start(&g_log_limit_timer);
            uapi_diag_info_log(0, "[log_limit]log limit is enabled");
        } else {
            osal_timer_stop(&g_log_limit_timer);
            uapi_diag_info_log(0, "[log_limit]log limit is disabled");
        }
    } else {
#if (CORE == APPS)
#ifdef SUPPORT_IPC
    ipc_payload_set_log_limit_t ipc_log_limit;
    ipc_log_limit.module_id = LOG_LIMIT_SET_ALL;
    ipc_log_limit.enable = enable;
    ipc_spin_send_message_timeout((cores_t)core,
                                  IPC_ACTION_SET_LOG_LIMIT,
                                  (ipc_payload *)&ipc_log_limit,
                                  sizeof(ipc_payload_set_log_limit_t),
                                  IPC_PRIORITY_LOWEST, false, IPC_SPIN_SEND_DEFAULT_TIMEOUT);
#elif defined(IPC_NEW)
    ipc_payload_set_log_limit_t ipc_log_limit;
    ipc_log_limit.module_id = LOG_LIMIT_SET_ALL;
    ipc_log_limit.enable = enable;

    ipc_msg_info_t msg_info;
    msg_info.dst_core = core;
    msg_info.priority = IPC_PRIORITY_LOWEST;
    msg_info.channel = 0;
    msg_info.msg_id = IPC_MSG_SET_LOG_LIMIT;
    msg_info.buf_addr = (uint8_t *)&ipc_log_limit;
    msg_info.buf_len = sizeof(ipc_payload_set_log_limit_t);

    errcode_t ret = uapi_ipc_send_msg_sync(&msg_info);
    if (ret != ERRCODE_SUCC) {
        uapi_diag_error_log(0, "[log_limit][set_enable] send ipc fail[0x%x]!", ret);
    }
#endif
#endif
    }
}

void uapi_log_limit_set_mod_enable(uint8_t core, uint16_t module_id, bool enable)
{
    if (core == CORE) {
        log_limit_t *log_limit = NULL, *log_limit_group = NULL;
        bool ret = get_module_limit_info(module_id, &log_limit, &log_limit_group);
        if (!ret) {
            return;
        }
        log_limit->enable = enable;
        if (enable == true) {
            uapi_diag_info_log(0, "[log_limit]log limit module 0x%x is enabled", module_id);
        } else {
            uapi_diag_info_log(0, "[log_limit]log limit module 0x%x is disabled", module_id);
        }
    } else {
#if (CORE == APPS)
#ifdef SUPPORT_IPC
    ipc_payload_set_log_limit_t ipc_log_limit;
    ipc_log_limit.module_id = module_id;
    ipc_log_limit.enable = enable;
    ipc_spin_send_message_timeout((cores_t)core,
                                  IPC_ACTION_SET_LOG_LIMIT,
                                  (ipc_payload *)&ipc_log_limit,
                                  sizeof(ipc_payload_set_log_limit_t),
                                  IPC_PRIORITY_LOWEST, false, IPC_SPIN_SEND_DEFAULT_TIMEOUT);
#elif defined(IPC_NEW)
    ipc_payload_set_log_limit_t ipc_log_limit;
    ipc_log_limit.module_id = module_id;
    ipc_log_limit.enable = enable;
    ipc_msg_info_t msg_info;
    msg_info.dst_core = core;
    msg_info.priority = IPC_PRIORITY_LOWEST;
    msg_info.channel = 0;
    msg_info.msg_id = IPC_MSG_SET_LOG_LIMIT;
    msg_info.buf_addr = (uint8_t *)&ipc_log_limit;
    msg_info.buf_len = sizeof(ipc_payload_set_log_limit_t);

    errcode_t ret = uapi_ipc_send_msg_sync(&msg_info);
    if (ret != ERRCODE_SUCC) {
        uapi_diag_error_log(0, "[log_limit][set_mod_enable] send ipc fail[0x%x]!", ret);
    }
#endif
#endif
    }
}

#if (CORE == APPS)
void uapi_log_limit_get_info(uint8_t core, log_limit_info_cb_t callback)
{
    if (core == CORE) {
        if (callback != NULL) {
            callback(core, g_log_limits, g_log_limit_num);
        }
    } else {
        g_log_limit_info_cb = callback;
#ifdef SUPPORT_IPC
        ipc_spin_send_message_timeout((cores_t)core,
                                      IPC_ACTION_GET_LOG_LIMIT,
                                      NULL,
                                      0,
                                      IPC_PRIORITY_LOWEST, false, IPC_SPIN_SEND_DEFAULT_TIMEOUT);
#elif defined(IPC_NEW)
        ipc_payload_get_log_limit_t ipc_log_limit;
        ipc_log_limit.core = CORE;

        ipc_msg_info_t msg_info = {0};
        msg_info.dst_core = core;
        msg_info.priority = IPC_PRIORITY_LOWEST;
        msg_info.channel = 0;
        msg_info.msg_id = IPC_MSG_GET_LOG_LIMIT;
        msg_info.buf_addr = (uint8_t *)&ipc_log_limit;
        msg_info.buf_len = sizeof(ipc_log_limit);

        errcode_t ret = uapi_ipc_send_msg_sync(&msg_info);
        if (ret != ERRCODE_SUCC) {
            uapi_diag_error_log(0, "[log_limit][get_info] send ipc fail[0x%x]!", ret);
        }
#endif
    }
}
#endif
#endif /* CONFIG_DFX_SUPPORT_LOG_LIMIT */