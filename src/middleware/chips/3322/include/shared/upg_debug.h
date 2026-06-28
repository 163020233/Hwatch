/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: UPG debug functions header file
 */
#ifndef UPG_DEBUG_H
#define UPG_DEBUG_H

#include "debug_print.h"
#include "upg_config.h"
#include "upg_definitions.h"
#ifndef BUILD_APPLICATION_SSB
#include "diag_log.h"
#endif
#include "stdio.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#if (UPG_CFG_DEBUG_PRINT_ENABLED == YES)

#define upg_log_debug(fmt, args...)   PRINT(fmt, ##args)
#define upg_log_info(fmt, args...)    PRINT(fmt, ##args)
#define upg_log_err(fmt, args...)     PRINT(fmt, ##args)

#else

#define upg_log_debug(...)
#define upg_log_info(...)
#define upg_log_err(...)

#endif /* UPG_CFG_DEBUG_PRINT_ENABLED */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* UPG_DEBUG_H */
