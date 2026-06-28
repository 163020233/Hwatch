/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: zdiag ind producer
 * This file should be changed only infrequently and with great care.
 */
#include "diag_rom_api.h"

#include "errcode.h"

#include "dfx_feature_config.h"
#if defined (CONFIG_DFX_CLOUD_REPORT) \
    && defined(CONFIG_DFX_SUPPORT_DIAG_LOG_HOOK) && (CONFIG_DFX_SUPPORT_DIAG_LOG_HOOK == DFX_YES)
#include "diag_log_hook.h"
#endif

diag_rom_api_t g_diag_rom_api;

errcode_t uapi_diag_report_sys_msg(uint32_t module_id, uint32_t msg_id, const uint8_t *buf, uint16_t buf_size,
                                   uint8_t level)
{
#if defined (CONFIG_DFX_CLOUD_REPORT) \
    && defined(CONFIG_DFX_SUPPORT_DIAG_LOG_HOOK) && (CONFIG_DFX_SUPPORT_DIAG_LOG_HOOK == DFX_YES)
    // 钩子函数，用于上报日志到dfx cloud存储
    diag_log_add(module_id, msg_id, buf, buf_size, level);
#endif
    if (g_diag_rom_api.report_sys_msg != NULL) {
        return g_diag_rom_api.report_sys_msg(module_id, msg_id, buf, buf_size, level);
    }
    return ERRCODE_FAIL;
}

void diag_rom_api_register(const diag_rom_api_t *api)
{
    g_diag_rom_api = *api;
}