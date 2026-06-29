/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: LiteOS Printf Implementation
 * Author: Huawei LiteOS Team
 * Create: 2022-06-30
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
#include "los_printf_pri.h"
#include "los_base.h"
#include "drv_uart.h"
#ifdef LOSCFG_BOX
#include "udrv_uart.h"
#endif

#define SIZEBUF  256

typedef VOID (*OutputFunc)(const CHAR *str, UINT32 len, BOOL isLock);

typedef enum {
    NO_OUTPUT = 0,
    UART_OUTPUT = 1,
    CONSOLE_OUTPUT = 2
} OutputType;

#ifdef LOSCFG_BOX
__attribute__((__used__)) STATIC OutputFunc g_output = DBX_UartPuts;
#else
__attribute__((__used__)) STATIC OutputFunc g_output = UartPuts;
#endif

#if (LOSCFG_LOG_ENABLE == 1)
STATIC VOID OutputControl(const CHAR *str, UINT32 len, OutputType type)
{
    switch (type) {
        case CONSOLE_OUTPUT:
            /* fall-through */
        case UART_OUTPUT:
            g_output(str, len, (BOOL)UART_WITH_LOCK);
            break;
        default:
            break;
    }
    return;
}

VOID OsVprintf(const CHAR *fmt, va_list ap, OutputType type)
{
    INT32 len;
    CHAR aBuf[SIZEBUF] = {0};
    CHAR *bBuf = NULL;
    UINT32 bufLen = SIZEBUF;
    const CHAR *errVsnp = "OsVprintf, Output illegal string!\n";

    bBuf = aBuf;
    len = vsnprintf_s(bBuf, bufLen, bufLen - 1U, fmt, ap);
    if ((len == -1) && (*bBuf == '\0')) {
        /* parameter is illegal or some features in fmt dont support */
        OutputControl(errVsnp, (UINT32)strlen(errVsnp), type);
        return;
    }

    const CHAR *errBuf = "OsVprintf, initial buf is not enough!\n";
    if (len == -1) {
        /* vsnprintf_s error, buf is not enough. */
        len = SIZEBUF - 1;
        OutputControl(errBuf, (UINT32)strlen(errBuf), type);
    }

    *(bBuf + len) = '\0';
    OutputControl(bBuf, (UINT32)len, type);
}
#endif

__attribute__ ((noinline)) VOID LOS_Printf(const CHAR *fmt, ...)
{
#if (LOSCFG_LOG_ENABLE == 1)
    va_list ap;
    va_start(ap, fmt);
    OsVprintf(fmt, ap, CONSOLE_OUTPUT);
    va_end(ap);
#else
    (void)fmt;
#endif
}

__attribute__ ((noinline)) VOID LOS_PrintfStr(const CHAR *str)
{
    if (str == NULL || strlen(str) >= SIZEBUF) {
        return;
    }

    g_output(str, strlen(str), (BOOL)UART_WITH_LOCK);
}

