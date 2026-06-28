/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: uart header for SELiteOS.
 */

#ifndef DRV_UART_H
#define DRV_UART_H

#include "los_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define UART_WITHOUT_LOCK 0
#define UART_WITH_LOCK    1

extern VOID UartPuts(const CHAR *s, UINT32 len, BOOL isLock);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* DRV_UART_H */
