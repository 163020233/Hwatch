/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description : The vUart-RPQ (virtual uart rpqueue) Simple Driver Implementation
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

#include "soc/uart.h"
#include "los_printf.h"
#include "los_rpqueue.h"
#include "los_spinlock.h"
#include "vuart_rpq.h"


VOID UART_CORE_FUNC(vuart_rpq, UartEarlyInit)(VOID)
{
    (VOID)LOS_RpqueueInit((VOID *)VUART_RPQ_TX_BUFF_ADDR, VUART_RPQ_TX_BUFF_SIZE, 0);
}

STATIC VOID UartPutStr(UINTPTR base, const CHAR *s, UINT32 len)
{
    LosRpqueueSendCfg queCfg = {0};
    (VOID)LOS_RpqueueSend((VOID *)base, (UINT8 *)s, (UINT16)len, &queCfg);
}

UINT32 UART_CORE_FUNC(vuart_rpq, UartPutsReg)(UINTPTR base, const CHAR *s, UINT32 len, BOOL isLock)
{
    (VOID)isLock;  /* always locked */
    UartPutStr(base, s, len);
    return len;
}

VOID UART_CORE_FUNC(vuart_rpq, UartPuts)(const CHAR *s, UINT32 len, BOOL isLock)
{
    (VOID)UART_CORE_FUNC_CALL(vuart_rpq, UartPutsReg, VUART_RPQ_TX_BUFF_ADDR, s, len, isLock);
}
