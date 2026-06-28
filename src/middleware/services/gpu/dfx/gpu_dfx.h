/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: gpu dfx define header file
 */

#ifndef GPU_DFX_H
#define GPU_DFX_H

#include <stdio.h>
#include <securec.h>
#include <math.h>
#include "td_base.h"
#include "soc_osal.h"
#include "common_def.h"

#ifdef NO_UART_DEBUG
#include "diag_log.h"
#endif

#ifdef GPU_SUPPORT_CHR
#include "massdata_buffer.h"
#include "chr/gpu_chr.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#if defined(CONFIG_GPU_DFX_ERROR)

#ifdef NO_UART_DEBUG
#define gpu_print(fmt, args...) uapi_diag_error_log(0, fmt, ##args)
#define gpu_err(fmt, args...)   uapi_diag_error_log(0, fmt, ##args)
#define gpu_dbg(fmt, args...)   uapi_diag_debug_log(0, fmt, ##args)
#else
#define gpu_print               osal_printk
#define gpu_err(fmt, args...)   osal_printk("[GPU_ERROR][%s %d]"fmt"\n", __func__, __LINE__, ##args)
#define gpu_dbg(fmt, args...)
#endif

#elif defined(CONFIG_GPU_DFX_DEBUG)

#ifdef NO_UART_DEBUG
#define gpu_print(fmt, args...) uapi_diag_error_log(0, fmt, ##args)
#define gpu_err(fmt, args...)   uapi_diag_error_log(0, fmt, ##args)
#define gpu_dbg(fmt, args...)   uapi_diag_debug_log(0, fmt, ##args)
#else
#define gpu_print               osal_printk
#define gpu_err(fmt, args...)   osal_printk("[GPU_ERROR][%s %d]"fmt"\n", __func__, __LINE__, ##args)
#define gpu_dbg(fmt, args...)   osal_printk("[GPU_DEBUG][%s %d]"fmt"\n", __func__, __LINE__, ##args)
#endif

#else

#define gpu_print
#define gpu_err(fmt, args...)
#define gpu_dbg(fmt, args...)

#endif

#ifdef GPU_SUPPORT_CHR
#define gpu_chr_log(event_id, info1, info2, info3) \
    uapi_massdata_record_system_event((uint8_t)(event_id), (uint8_t)(info1), \
                                      (uint8_t)(info2), (uint32_t)(info3))
#define gpu_chr_log_long(event_id, info1, info2, info3, info3_len) \
    uapi_massdata_record_system_long_event((uint8_t)(event_id), (uint8_t)(info1), (uint8_t)(info2), \
                                           (uint8_t *)(info3), (uint8_t)(info3_len))
#else
#define gpu_chr_log(event_id, info1, info2, info3)
#define gpu_chr_log_long(event_id, info1, info2, info3, info3_len)
#endif

static inline td_void gpu_dfx_get_time_us(td_u32 *time_us)
{
    osal_timeval cur_time = { 0 };
    osal_gettimeofday(&cur_time);
    *time_us = (td_u32)(cur_time.tv_sec * 1000000 + cur_time.tv_usec); /* 1000000: index */
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
