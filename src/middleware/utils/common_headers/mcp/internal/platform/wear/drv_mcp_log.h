/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: internal debug define.
 * Author: AuthorNameMagicTag
 * Create: 2025-07-21
 */

#ifndef __DRV_MCP_LOG_H__
#define __DRV_MCP_LOG_H__

#include "td_base.h"
#include "soc_osal.h"

#ifdef MCP_DIAG_LOG_DEBUG
#include "diag_log.h"
#endif

#ifdef __cplusplus
#if __cplusplus
    extern "C" {
#endif
#endif /* __cplusplus */

#if !defined(CONFIG_MCP_UT)
#define CONFIG_MCP_DFX_ERROR
#else

#endif

#if defined(CONFIG_MCP_UT)

#define mcp_alert_fatal(mod, fmt, args...)     printf("[MCP_ALERT][%s %d]"fmt"\n", __func__, __LINE__, ##args)
#define mcp_err_trace(mod, fmt, args...)       printf("[MCP_ERROR][%s %d]"fmt"\n", __func__, __LINE__, ##args)
#define mcp_warn_trace(mod, fmt, args...)
#define mcp_info_trace(mod, fmt, args...)
#define mcp_debug_trace(mod, fmt, args...)
#define mcp_alert_trace(mod, fmt, args...)     printf("[MCP_ALERT][%s %d]"fmt"\n", __func__, __LINE__, ##args)

#elif defined(CONFIG_MCP_DFX_ERROR)

#define mcp_alert_fatal(mod, fmt, args...)     osal_printk("[MCP_ALERT][%s %d]"fmt"\n", __func__, __LINE__, ##args)
#define mcp_err_trace(mod, fmt, args...)       osal_printk("[MCP_ERROR][%s %d]"fmt"\n", __func__, __LINE__, ##args)
#define mcp_warn_trace(mod, fmt, args...)
#define mcp_info_trace(mod, fmt, args...)
#define mcp_debug_trace(mod, fmt, args...)
#define mcp_alert_trace(mod, fmt, args...)     osal_printk("[MCP_ALERT][%s %d]"fmt"\n", __func__, __LINE__, ##args)

#elif defined(CONFIG_MCP_DFX_DEBUG)

#define mcp_alert_fatal(mod, fmt, args...)     osal_printk("[MCP_ERROR][%s %d]"fmt"\n", __func__, __LINE__, ##args)
#define mcp_err_trace(mod, fmt, args...)       osal_printk("[MCP_ERROR][%s %d]"fmt"\n", __func__, __LINE__, ##args)
#define mcp_warn_trace(mod, fmt, args...)      osal_printk("[MCP_ERROR][%s %d]"fmt"\n", __func__, __LINE__, ##args)
#define mcp_info_trace(mod, fmt, args...)      osal_printk("[MCP_ERROR][%s %d]"fmt"\n", __func__, __LINE__, ##args)
#define mcp_debug_trace(mod, fmt, args...)     osal_printk("[MCP_ERROR][%s %d]"fmt"\n", __func__, __LINE__, ##args)
#define mcp_alert_trace(mod, fmt, args...)     osal_printk("[MCP_ERROR][%s %d]"fmt"\n", __func__, __LINE__, ##args)

#else

#define mcp_alert_fatal(mod, fmt, args...)
#define mcp_err_trace(mod, fmt, args...)
#define mcp_warn_trace(mod, fmt, args...)
#define mcp_info_trace(mod, fmt, args...)
#define mcp_debug_trace(mod, fmt, args...)
#define mcp_alert_trace(mod, fmt, args...)

#endif

#define soc_log_alert                          mcp_alert_fatal
#define soc_log_err                            mcp_err_trace
#define soc_log_warn                           mcp_warn_trace
#define soc_log_notice                         mcp_info_trace
#define soc_log_info                           mcp_debug_trace
#define soc_log_dbg                            mcp_alert_trace

#define mcp_log_err(fmt, args...)              mcp_err_trace(SOC_ID_ISP, fmt, ##args)
#define mcp_log_fatal(fmt, args...)            mcp_alert_fatal(SOC_ID_ISP, fmt, ##args)
#define mcp_log_warn(fmt, args...)             mcp_warn_trace(SOC_ID_ISP, fmt, ##args)
#define mcp_log_dbg(fmt, args...)              mcp_debug_trace(SOC_ID_ISP, fmt, ##args)
#define mcp_log_alert(fmt, args...)            mcp_alert_trace(SOC_ID_ISP, fmt, ##args)
#define mcp_log_info(fmt, args...)             mcp_info_trace(SOC_ID_ISP, fmt, ##args)
#define mcp_log_trace(fmt, args...)            mcp_info_trace(SOC_ID_ISP, fmt, ##args)
#define mcp_log_notice(fmt, args...)           mcp_info_trace(SOC_ID_ISP, fmt, ##args)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __DRV_MCP_LOG_H__ */
