/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description : Uart-V500 Simple Driver Implementation
 * Author : Huawei LiteOS Team
 * Create : 2022-11-8
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
#include "los_spinlock.h"
#include "hq_uart.h"

STATIC VOID SetBaudRate(UINT32 baudrate)
{
    UINT32 divider, value, remainder, fraction;
    if (baudrate == 0) {
        return;
    }
    value = 16 * baudrate;                             /* 16: division factors for integer baud rate */
    divider = CONFIG_UART_CLK_INPUT / value;
    remainder = CONFIG_UART_CLK_INPUT % value;
    value = (8 * remainder) / baudrate;                /* 8: division factors for fraction baud rate */
    fraction = (value >> 1) + (value & 1);

    WriteBaudReg(UART_REG_BASE, fraction, divider);
}

VOID UART_CORE_FUNC(hquart, UartEarlyInit)(VOID)
{
    UINT32 regVal;

    /* 1.disable uart */
    regVal = GET_UINT32(UART_REG_BASE + UART_CONFIG);
    regVal &= ~UART_EN;
    regVal |= UART_EN_MSK;
    WRITE_UINT32(regVal, UART_REG_BASE + UART_CONFIG);

    /* 2.clear interrupt/interrupt mask */
    WRITE_UINT32(UART_CLEAR_ALL_INTER, UART_REG_BASE + UART_INT_CLR);
    WRITE_UINT32(UART_UNMASK_RX_INTER, UART_REG_BASE + UART_INT_MSK);

    /* UART_FIFO_CFG line RTS >=full-2, RX: >= 4/8, TX <= 1/8 */
    WRITE_UINT32(UART_FIFO_CFG_RTS_FULL | UART_FIFO_CFG_RX4_8 | UART_FIFO_CFG_TX4_8,
        UART_REG_BASE + UART_FIFO_CFG);

    /* 3.set baud rate */
    SetBaudRate(CONSOLE_UART_BAUDRATE);

    /* 4.config uart frame cfg */
    WRITE_UINT32(UART_CONFIG_FRAME_8_1_NO_PAR, UART_REG_BASE + UART_FRAME_CFG);

    /* 5.enable uart */
    regVal = GET_UINT32(UART_REG_BASE + UART_CONFIG);
    regVal |= UART_CONFIG_ENABLE;
    WRITE_UINT32(regVal, UART_REG_BASE + UART_CONFIG);
}

#ifdef LOSCFG_SERIAL_OUTPUT_ENABLE
STATIC VOID UartPutcReg(UINTPTR base, CHAR c)
{
    UINT32 cycle = 10000;           /* 10000: wait cpu cycles */

    /* Spin while fifo is full */
    while ((READ32(base + UART_STAT) & UART_STAT_TX_FIFO_FULL) && (cycle != 0)) {
        cycle--;
    }
    WRITE32(base + UART_TX_FIFO, c);
}

LITE_OS_SEC_BSS STATIC SPIN_LOCK_INIT(g_uartOutputSpin);

STATIC VOID UartPutStr(UINTPTR base, const CHAR *s, UINT32 len)
{
    UINT32 i;

    for (i = 0; i < len; i++) {
        /*
         * Only system uart output needs to add extra '\r' to improve
         * the compatibility on win.
         */
        if ((*(s + i) == '\n') && (base == UART_REG_BASE))  {
            UartPutcReg(base, '\r');
        }
        UartPutcReg(base, *(s + i));
    }
}
#endif

UINT32 UART_CORE_FUNC(hquart, UartPutsReg)(UINTPTR base, const CHAR *s, UINT32 len, BOOL isLock)
{
#ifdef LOSCFG_SERIAL_OUTPUT_ENABLE
    UINT32 intSave;
    if (isLock) {
        LOS_SpinLockSave(&g_uartOutputSpin, &intSave);
        UartPutStr(base, s, len);
        LOS_SpinUnlockRestore(&g_uartOutputSpin, intSave);
    } else {
        UartPutStr(base, s, len);
    }

    return len;
#else
    (VOID)base;
    (VOID)s;
    (VOID)len;
    (VOID)isLock;
    return 0;
#endif
}

VOID UART_CORE_FUNC(hquart, UartPuts)(const CHAR *s, UINT32 len, BOOL isLock)
{
    UART_CORE_FUNC_CALL(hquart, UartPutsReg, UART_REG_BASE, s, len, isLock);
}
