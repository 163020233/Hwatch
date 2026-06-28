/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2019-04-22
 */

#ifndef __DRV_VDEC_SDK_H__
#define __DRV_VDEC_SDK_H__

#include "td_type.h"
#include "stdio.h"
#include "drv_vdec_osal.h"
#include "drv_video_ext.h"
#include "vfmw_intf.h"

#ifdef NO_UART_DEBUG
#include "diag_log.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifndef STATIC
#define STATIC static
#endif

#define VDEC_DRV_PRINT printf
#define CONFIG_DRV_VDEC_DFX_ERROR

#if defined(CONFIG_DRV_VDEC_DFX_DEBUG)

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
#define vdec_print_alert(fmt, args...)    VDEC_DRV_PRINT("[vdec][%s %d]: " fmt, __func__, __LINE__, ##args)
#define vdec_print_fatal(fmt, args...)    VDEC_DRV_PRINT("[vdec][%s %d]: " fmt, __func__, __LINE__, ##args)
#define vdec_print_err(fmt, args...)      VDEC_DRV_PRINT("[vdec][%s %d]: " fmt, __func__, __LINE__, ##args)
#define vdec_print_warn(fmt, args...)     VDEC_DRV_PRINT("[vdec][%s %d]: " fmt, __func__, __LINE__, ##args)
#define vdec_print_notice(fmt, args...)   VDEC_DRV_PRINT("[vdec][%s %d]: " fmt, __func__, __LINE__, ##args)
#define vdec_print_info(fmt, args...)     VDEC_DRV_PRINT("[vdec][%s %d]: " fmt, __func__, __LINE__, ##args)
#define vdec_print_dbg(fmt, args...)      VDEC_DRV_PRINT("[vdec][%s %d]: " fmt, __func__, __LINE__, ##args)
#define vdec_print_trace(fmt, args...)    VDEC_DRV_PRINT("[vdec][%s %d]: " fmt, __func__, __LINE__, ##args)
#endif

#elif defined(CONFIG_DRV_VDEC_DFX_ERROR)

#ifdef NO_UART_DEBUG
#define vdec_print_alert(fmt, args...)    uapi_diag_error_log(0, fmt, ##args)
#define vdec_print_fatal(fmt, args...)    uapi_diag_error_log(0, fmt, ##args)
#define vdec_print_err(fmt, args...)      uapi_diag_error_log(0, fmt, ##args)
#elif defined VDEC_NO_DEBUG_LOG_SUPPORT
#define vdec_print_alert(fmt, args...)
#define vdec_print_fatal(fmt, args...)
#define vdec_print_err(fmt, args...)
#else
#define vdec_print_alert(fmt, args...)    VDEC_DRV_PRINT("[vdec][%s %d]: " fmt, __func__, __LINE__, ##args)
#define vdec_print_fatal(fmt, args...)    VDEC_DRV_PRINT("[vdec][%s %d]: " fmt, __func__, __LINE__, ##args)
#define vdec_print_err(fmt, args...)      VDEC_DRV_PRINT("[vdec][%s %d]: " fmt, __func__, __LINE__, ##args)
#endif

#define vdec_print_warn(fmt, args...)
#define vdec_print_notice(fmt, args...)
#define vdec_print_info(fmt, args...)
#define vdec_print_dbg(fmt, args...)
#define vdec_print_trace(fmt, args...)
#else
#define vdec_print_alert(fmt, args...)
#define vdec_print_fatal(fmt, args...)
#define vdec_print_err(fmt, args...)
#define vdec_print_warn(fmt, args...)
#define vdec_print_notice(fmt, args...)
#define vdec_print_info(fmt, args...)
#define vdec_print_dbg(fmt, args...)
#define vdec_print_trace(fmt, args...)
#endif

td_s32  vdec_drv_get_vfmw_func(vfmw_export_fun *func);
td_s32  vdec_drv_register(td_void *func);
td_void vdec_drv_unregister(td_void);
td_void vdec_drv_stat_event(td_u32 event_id, td_u32 value);
td_u8 *vdec_drv_map(td_void *dma_buf);
td_void vdec_drv_unmap(td_void *dma_buf, td_u8 *vir_addr);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
