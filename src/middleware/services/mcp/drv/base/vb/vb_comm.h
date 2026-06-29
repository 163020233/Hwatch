/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: vb common header file
 * Author: AuthorNameMagicTag
 * Create: 2021-08-10
 */

#ifndef __VB_COMM_H__
#define __VB_COMM_H__

#include "drv_mcp_dependency.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#undef  LOG_MODULE_ID
#define LOG_MODULE_ID     MCP_MOD_ID_VB

#define vb_print_alert(fmt...)    mcp_log_alert(fmt)
#define vb_print_fatal(fmt...)    mcp_log_fatal(fmt)
#define vb_print_err(fmt...)      mcp_log_err(fmt)
#define vb_print_warn(fmt...)     mcp_log_warn(fmt)
#define vb_print_notice(fmt...)   mcp_log_notice(fmt)
#define vb_print_info(fmt...)     mcp_log_info(fmt)
#define vb_print_dbg(fmt...)      mcp_log_dbg(fmt)
#define vb_print_trace(fmt...)    mcp_log_trace(fmt)

/* function trace log, print the called function name when function is error */
#define vb_print_func_err(func, err_code) vb_print_err("%s failed 0x%08X\n", #func, err_code)

/* Function trace log, print the pointer name when pointer is null */
#define vb_print_null_pointer(val) vb_print_err("%s=%px, null pointer!\n", #val, val)

#define vb_assert(cond) mcp_assert(cond)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __VB_COMM_H_ */

