/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: os platform config files
 */

#ifndef _SOC_UART_H
#define _SOC_UART_H

#include "los_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define UART_WITH_LOCK    1
#define UART_WITHOUT_LOCK 0

extern VOID UartPuts(const CHAR *s, UINT32 len, BOOL isLock);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
