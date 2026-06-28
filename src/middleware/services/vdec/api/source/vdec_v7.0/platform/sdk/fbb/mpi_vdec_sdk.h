/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2019-04-22
 */

#ifndef __MPI_VDEC_SDK_H__
#define __MPI_VDEC_SDK_H__

#include "stdio.h"
#include "drv_video_ext.h"
#include "td_base.h"
#include "drv_ioctl_vdec.h"
#include "soc_osal.h"

#ifdef NO_UART_DEBUG
#include "diag_log.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#ifndef STATIC
#define STATIC static
#endif

#define VDEC_API_PRINT printf
#define CONFIG_API_VDEC_DFX_ERROR

#if defined(CONFIG_API_VDEC_DFX_DEBUG)

#ifdef NO_UART_DEBUG
#define vdec_print_alert(fmt, args...)    uapi_diag_error_log(0, fmt, ##args)
#define vdec_print_fatal(fmt, args...)    uapi_diag_error_log(0, fmt, ##args)
#define vdec_print_err(fmt, args...)      uapi_diag_error_log(0, fmt, ##args)
#define vdec_print_warn(fmt, args...)     uapi_diag_error_log(0, fmt, ##args)
#define vdec_print_info(fmt, args...)     uapi_diag_error_log(0, fmt, ##args)
#define vdec_print_dbg(fmt, args...)      uapi_diag_error_log(0, fmt, ##args)
#define vdec_print_trace(fmt, args...)    uapi_diag_error_log(0, fmt, ##args)
#elif defined VDEC_NO_DEBUG_LOG_SUPPORT
#define vdec_print_alert(fmt, args...)
#define vdec_print_fatal(fmt, args...)
#define vdec_print_err(fmt, args...)
#define vdec_print_warn(fmt, args...)
#define vdec_print_info(fmt, args...)
#define vdec_print_dbg(fmt, args...)
#define vdec_print_trace(fmt, args...)
#else
#define vdec_print_alert(fmt, args...)    VDEC_API_PRINT("[vdec][%s %d]: " fmt, __func__, __LINE__, ##args)
#define vdec_print_fatal(fmt, args...)    VDEC_API_PRINT("[vdec][%s %d]: " fmt, __func__, __LINE__, ##args)
#define vdec_print_err(fmt, args...)      VDEC_API_PRINT("[vdec][%s %d]: " fmt, __func__, __LINE__, ##args)
#define vdec_print_warn(fmt, args...)     VDEC_API_PRINT("[vdec][%s %d]: " fmt, __func__, __LINE__, ##args)
#define vdec_print_info(fmt, args...)     VDEC_API_PRINT("[vdec][%s %d]: " fmt, __func__, __LINE__, ##args)
#define vdec_print_dbg(fmt, args...)      VDEC_API_PRINT("[vdec][%s %d]: " fmt, __func__, __LINE__, ##args)
#define vdec_print_trace(fmt, args...)    VDEC_API_PRINT("[vdec][%s %d]: " fmt, __func__, __LINE__, ##args)
#endif

#elif defined(CONFIG_API_VDEC_DFX_ERROR)

#ifdef NO_UART_DEBUG
#define vdec_print_alert(fmt, args...)    uapi_diag_error_log(0, fmt, ##args)
#define vdec_print_fatal(fmt, args...)    uapi_diag_error_log(0, fmt, ##args)
#define vdec_print_err(fmt, args...)      uapi_diag_error_log(0, fmt, ##args)
#elif defined VDEC_NO_DEBUG_LOG_SUPPORT
#define vdec_print_alert(fmt, args...)
#define vdec_print_fatal(fmt, args...)
#define vdec_print_err(fmt, args...)
#else
#define vdec_print_alert(fmt, args...)    VDEC_API_PRINT("[vdec][%s %d]: " fmt, __func__, __LINE__, ##args)
#define vdec_print_fatal(fmt, args...)    VDEC_API_PRINT("[vdec][%s %d]: " fmt, __func__, __LINE__, ##args)
#define vdec_print_err(fmt, args...)      VDEC_API_PRINT("[vdec][%s %d]: " fmt, __func__, __LINE__, ##args)
#endif

#define vdec_print_warn(fmt, args...)
#define vdec_print_info(fmt, args...)
#define vdec_print_dbg(fmt, args...)
#define vdec_print_trace(fmt, args...)
#else
#define vdec_print_alert(fmt, args...)
#define vdec_print_fatal(fmt, args...)
#define vdec_print_err(fmt, args...)
#define vdec_print_warn(fmt, args...)
#define vdec_print_info(fmt, args...)
#define vdec_print_dbg(fmt, args...)
#define vdec_print_trace(fmt, args...)
#endif

#define vdec_malloc(size)         malloc(size)
#define vdec_free(addr)           free(addr)
#define vdec_mpi_unused(x)        ((x) = (x))

td_void vdec_get_time(td_u32 *time);
td_char* vdec_show_std(ext_vdec_standard std);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
