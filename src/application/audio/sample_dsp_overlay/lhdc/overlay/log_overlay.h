/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: head of overlay log
 * Author: audio
 */

#ifndef __LOG_OVERLAY_H__
#define __LOG_OVERLAY_H__

#include "audio_type.h"

td_void log_event_print0(td_u32 log_header, td_u32 press_para);
td_void log_event_print1(td_u32 log_header, td_u32 press_para, td_u32 para1);
td_void log_event_print2(td_u32 log_header, td_u32 press_para, td_u32 para1, td_u32 para2);
td_void log_event_print3(td_u32 log_header, td_u32 press_para, td_u32 para1, td_u32 para2, td_u32 para3);
td_void log_event_print4(td_u32 log_header, td_u32 press_para, td_u32 para1, td_u32 para2, td_u32 para3, td_u32 para4);

#ifdef SAP_IPC_LOG_SUPPORT
#include "log_overlay_ipc.h"

#else
#include "log_overlay_uart.h"
#endif /* SAP_IPC_LOG_SUPPORT */

#endif /* __LOG_OVERLAY_H__ */
