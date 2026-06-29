/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: uart source for SELiteOS.
 */

#include "drv_uart.h"
#include "uart_porting.h"
#include "uart.h"

VOID UartPuts(const CHAR *s, UINT32 len, BOOL isLock)
{
    (void)isLock;
    uapi_uart_write(SW_DEBUG_UART_BUS, (const void *)s, len, 0);
    return;
}
