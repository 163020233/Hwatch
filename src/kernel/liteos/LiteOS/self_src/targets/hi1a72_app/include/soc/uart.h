/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: os uart header.
 * Author: Huawei LiteOS Team
 * Create: 2025-01-24
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --------------------------------------------------------------------------- */
#ifndef HISOC_UART_H
#define HISOC_UART_H

#include "asm/platform.h"
#include "los_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define UART_WITHOUT_LOCK 0
#define UART_WITH_LOCK    1
#define UART_REG_BASE           DBG_UART_REG_BASE

#define TTY_DEVICE              "/dev/uartdev-0"

#define CONFIG_UART_CLK_INPUT   24000000
#define CONSOLE_UART_BAUDRATE   115200

typedef enum UartBus {
    UART_BUS_0,             /*!< UART Bus 0 */
    UART_BUS_1,             /*!< UART Bus 1 */
    UART_BUS_2,             /*!< UART Bus 2 */
    UART_BUS_3,             /*!< UART Bus 3 */
    UART_BUS_MAX_NUM
} uart_bus_t;

VOID UartPuts(const CHAR *s, UINT32 len, BOOL isLock);

#ifdef __cplusplus
}
#endif

#endif
