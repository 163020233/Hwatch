/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: API of audio debug
 * Author: audio
 */

#ifndef __AUDIO_DEBUG_ACORE_H__
#define __AUDIO_DEBUG_ACORE_H__

#ifdef BRANDY_PROT /* bt core id */
#include "audio_debug_uart.h"
#else
#ifdef SAP_DIAG_LOG_SUPPORT
#ifdef SAP_SOCMN2
#include "audio_debug_diag.h"
#endif /* SAP_CHIP_TYPE */
#if (SAP_CHIP_TYPE == brandy)
#ifdef SW_UART_DEBUG /* platform uart support macro */
#include "audio_debug_uart.h"
#else
#include "audio_debug_diag.h"
#endif /* SW_UART_DEBUG */
#endif /* SAP_CHIP_TYPE */
#if (SAP_CHIP_TYPE == sw21)
#include "audio_debug_diag.h"
#endif /* SAP_CHIP_TYPE */
#else
#include "audio_debug_uart.h"
#endif /* SAP_DIAG_LOG_SUPPORT */
#endif /* BRANDY_PROT */

#define oml_dsp_log_print0(level, fmt)                 audio_log0(level, fmt)
#define oml_dsp_log_print1(level, fmt, p1)             audio_log1(level, fmt, p1)
#define oml_dsp_log_print2(level, fmt, p1, p2)         audio_log2(level, fmt, p1, p2)
#define oml_dsp_log_print3(level, fmt, p1, p2, p3)     audio_log3(level, fmt, p1, p2, p3)
#define oml_dsp_log_print4(level, fmt, p1, p2, p3, p4) audio_log4(level, fmt, p1, p2, p3, p4)
#define oml_dsp_log_printx(level, fmt, ...)            audio_logx(level, fmt, ...)

#endif /* __AUDIO_DEBUG_ACORE_H__ */
