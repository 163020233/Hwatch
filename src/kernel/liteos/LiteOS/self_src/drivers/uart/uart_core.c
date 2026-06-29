/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description : The Uart Core Implementation
 * Author : Huawei LiteOS Team
 * Create : 2023-08-12
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

#include "uart_pri.h"
#include "hq_uart_v5xx/hq_uart_core.h"
#include "arm_pl011/arm_pl011_core.h"
#include "vuart_rpq/vuart_rpq_core.h"

VOID UartEarlyInit(VOID)
{
    UART_CORE_FUNC_CALL(UART_CORE_MAIN_NAME, UartEarlyInit);
}

VOID UartPuts(const CHAR *s, UINT32 len, BOOL isLock)
{
    UART_CORE_FUNC_CALL(UART_CORE_MAIN_NAME, UartPuts, s, len, isLock);
}

VOID UartPutsReg(UINTPTR regBase, const CHAR *s, UINT32 len, BOOL isLock)
{
    UART_CORE_FUNC_CALL(UART_CORE_MAIN_NAME, UartPutsReg, regBase, s, len, isLock);
}
